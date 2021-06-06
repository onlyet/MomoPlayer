#pragma once

#include <QObject>
#include <QMutex>

#include <atomic>

class DecodeWorker;
class RenderWorker;

class QWidget;
class QThread;

extern "C"
{
    class AVFormatContext;
    class AVCodecContext;
    class AVFrame;
    class AVPacket;
    class SwsContext;

#include <libavutil/pixfmt.h>

}

class PlayerControler : public QObject
{
    Q_OBJECT

public:
    PlayerControler(QWidget *widget, QObject *parent = nullptr);
    ~PlayerControler();

    // 位于PlayerControler线程
    void release();

    void readPacketAndDecode();
    std::atomic_bool& canPlay() /*const*/;

    QWidget* widget();
    double getFps() const;
    bool toRgb(uchar *out, int outWidth, int outHeight);

private:
    void init();
    bool readPacket(AVPacket *packet);
    bool decode(AVPacket *packet);
    bool allocFFFmpegResource();
    void releaseFFmpegResource();

public slots:
    void start();
    void stop();
    void pause();

    bool openInput(const QString &path);
    void closeInput();

    //void destruct();

private slots:
    void process();

signals:
    void finished();
    void draw(const QImage &img);
    void openInputFailed();
    void closeInputFinished();
    void releaseFinished();

private:
    QString             m_path;
    QThread             *m_thread = nullptr;
    DecodeWorker        *m_decodeWorker = nullptr;
    RenderWorker        *m_renderWorker = nullptr;
    QWidget             *m_widget = nullptr;

    AVFormatContext     *m_fmtCtx = nullptr;
    AVCodecContext      *m_vDecodeCtx = nullptr;
    SwsContext          *m_swsCtx = nullptr;
    AVFrame             *m_decodeFrame = nullptr;         // 解码后的视频帧
    QMutex              m_decodeFrameMtx;
    //QMutex              m_mutex;                        // 互斥变量，多线程时避免同时间的读写AVFormatContext和AVCodecContext

    std::atomic_bool    m_isStopped{ true };
    std::atomic_bool    m_canPlay{ true };               // 是否播放
    std::atomic_bool    m_justPaused{ false };

    bool                m_hwDecode = false;
    bool                m_isEof = false;                 // 是否读完;
    double              m_fps = 0;                       // 视频流帧率
    bool                m_isOpened = false;              // 是否已打开媒体
    int                 m_videoStream = -1;              // 视频流index

};
