#include "PlayerControler.h"
#include "DecodeWorker.h"
#include "RenderWorker.h"

#include <VAUtil.h>
#include <GlobalDefine.h>
#include <GlobalConfig.h>

#include <QDebug>
#include <QThread>
#include <QTime>

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

using namespace std;

PlayerControler::PlayerControler(QWidget *widget, QObject *parent)
    : QObject(parent)
    , m_widget(widget)
{
    qDebug() << "PlayerControler() 1";
    m_thread = new QThread;
    this->moveToThread(m_thread);
    connect(m_thread, &QThread::started, this, &PlayerControler::process);
    //connect(m_thread, &QThread::finished, m_thread, &QThread::deleteLater);
    //connect(this, &PlayerControler::finished, m_thread, &QThread::quit);
    //connect(this, &PlayerControler::finished, this, &PlayerControler::deleteLater);

    m_decodeWorker = new DecodeWorker(this);
    m_renderWorker = new RenderWorker(this);
    connect(m_renderWorker, &RenderWorker::draw, this, &PlayerControler::draw);
    qDebug() << "PlayerControler() 2";
}

PlayerControler::~PlayerControler()
{
    qDebug() << "~PlayerControler() 1";


    SafeRelease(m_decodeWorker);
    SafeRelease(m_renderWorker);

    SafeRelease(m_thread);

    qDebug() << "~PlayerControler() 2";
}

// closeInput+stop耗时52毫秒，如果是56路则接近3秒了
void PlayerControler::release()
{
    qDebug() << "PlayerControler::release 1";
    closeInput();
    stop();
    qDebug() << "PlayerControler::release 2";
    emit releaseFinished();
}

void PlayerControler::start()
{
    if (!m_thread)
    {
        return;
    }
    if (!m_isStopped)
    {
        return;
    }
    m_isStopped = false;

    m_thread->start();
}

void PlayerControler::stop()
{
    if (!m_thread)
    {
        return;
    }
    if (m_isStopped)
    {
        return;
    }
    m_isStopped = true;

    qDebug() << "PlayerControler::stop 1";
    m_thread->quit();
    // 就在本线程调用quit，不需要wait
    //m_thread->wait();
    qDebug() << "PlayerControler::stop 2";
}

void PlayerControler::pause()
{
    m_canPlay = !m_canPlay;
    if (m_canPlay)
    {
        m_justPaused = true;
    }
    //m_decodeWorker->pause();
}

bool PlayerControler::openInput(const QString &path)
{
    if (path.isEmpty())
    {
        emit openInputFailed();
        return true;
    }
    if (m_isOpened)
    {
        emit openInputFailed();
        return false;
    }
    QTime t = QTime::currentTime();
    m_path = path;

    if (!allocFFFmpegResource())
    {
        emit openInputFailed();
        return false;
    }

    m_decodeWorker->start();
    m_renderWorker->start();

    m_isOpened = true;
    qDebug() << QString("Open %1, time: %2ms").arg(m_path).arg(t.elapsed());
    return true;
}

void PlayerControler::closeInput()
{
    if (!m_isOpened)
    {
        return;
    }

    QTime t = QTime::currentTime();

    if (m_decodeWorker)
    {
        m_decodeWorker->stop();
    }
    if (m_renderWorker)
    {
        m_renderWorker->stop();
    }

    //QMutexLocker locker(&m_mutex);

    releaseFFmpegResource();
    init();

    qDebug() << QString("Close %1, time: %2ms").arg(m_path).arg(t.elapsed());
    emit closeInputFinished();
}

//void PlayerControler::destruct()
//{
//    emit finished();
//}

atomic_bool& PlayerControler::canPlay() /*const*/
{
    return m_canPlay;
}

QWidget* PlayerControler::widget()
{
    return m_widget;
}

double PlayerControler::getFps() const
{
    return m_fps ? m_fps : 25;
}

bool PlayerControler::toRgb(uchar *out, int outWidth, int outHeight)
{
    if (!out || outWidth <= 0 || outHeight <= 0)
    {
        return false;
    }

    try {
        QMutexLocker locker(&m_decodeFrameMtx);

        if (!m_decodeFrame || 0 == m_decodeFrame->linesize[0] || 
            m_decodeFrame->width <= 0 || m_decodeFrame->height <= 0 ||
            AV_PIX_FMT_NONE == m_decodeFrame->format)
        {
            return false;
        }

        //QTime t = QTime::currentTime();
        m_swsCtx = sws_getCachedContext(m_swsCtx, 
            m_decodeFrame->width, m_decodeFrame->height, (AVPixelFormat)m_decodeFrame->format,
            outWidth, outHeight, AV_PIX_FMT_BGRA,
            SWS_BICUBIC,
            nullptr, nullptr, nullptr);
        if (!m_swsCtx)
        {
            qDebug() << "sws_getCachedContext failed";
            return false;
        }
        //qDebug() << "sws_getCachedContext time:" << t.elapsed();

        uint8_t *data[AV_NUM_DATA_POINTERS] = { nullptr };
        data[0] = out;     // 第一位输出RGB
        int lineSize[AV_NUM_DATA_POINTERS] = { 0 };
        lineSize[0] = outWidth * 4; // 一行的宽度为4个字节（32位）

        int h = sws_scale(m_swsCtx,
            m_decodeFrame->data,             // 当前处理区域的每个通道的数据指针
            m_decodeFrame->linesize,         // 每个通道行字节数
            0,
            m_decodeFrame->height,          // 原视频帧的高度
            data,                           // 输出的每个通道数据指针
            lineSize);                      // 每个通道行字节数

        return h > 0;
    }
    catch (std::exception &e)
    {
        qCritical() << "std::exception: " << e.what();
        return false;
    }
}

void PlayerControler::readPacketAndDecode()
{
    AVPacket pkt{ 0 };
    //memset(&pkt, 0, sizeof(AVPacket));
    //av_init_packet();
    if (!readPacket(&pkt))
    {
        return;
    }

    if (!m_canPlay)
    {
        av_packet_unref(&pkt);
        return;
    }

    if (m_justPaused)
    {
        if (pkt.flags & AV_PKT_FLAG_KEY)
        {
            m_justPaused = false;
        }
        else
        {
            av_packet_unref(&pkt);
            return;
        }
    }

    decode(&pkt);
    av_packet_unref(&pkt);
}


void PlayerControler::process()
{
    qDebug() << qstr("PlayerControler线程开始");
    VAUtil::initFFmpeg();
}

void PlayerControler::init()
{
    m_fps = 0;
    m_isEof = false;
    m_videoStream = -1;
    m_isOpened = false;

    m_canPlay = true;
    m_justPaused = false;
}

bool PlayerControler::readPacket(AVPacket *packet)
{
    if (!m_fmtCtx)
    {
        return false;
    }

    int ret = av_read_frame(m_fmtCtx, packet);
    if (ret != 0)
    {
        if (ret == AVERROR_EOF)
        {
            m_isEof = true;// 文件读取结束
            // 避免一直EOF造成高CPU占用
            QThread::msleep(25);
        }
        qDebug() << QString("av_read_frame failed: %1, %2").arg(VAUtil::parseErr(ret)).arg(m_path);
        return false;
    }

    if (packet->size <= 0)
    {
        av_packet_unref(packet);
        QThread::msleep(5);
        return false;
    }
    if (packet->stream_index != m_videoStream)
    {
        av_packet_unref(packet);
        return false;
    }
    return true;
}

bool PlayerControler::decode(AVPacket *packet)
{
    if (!m_fmtCtx)
    {
        return false;
    }

    // 任务管理器看到这里会增加内存
    int ret = avcodec_send_packet(m_vDecodeCtx, packet);
    if (ret < 0)
    {
        qDebug() << "avcodec_send_packet failed, " << VAUtil::parseErr(ret);
        return false;
    }

    QMutexLocker lock(&m_decodeFrameMtx);
    if (!m_decodeFrame)
    {
        m_decodeFrame = av_frame_alloc();
    }
    if (m_hwDecode)
    {
        //while (ret >= 0)
        //{
        //    ret = avcodec_receive_frame(m_vDecodeCtx, m_pYuv);
        //    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        //    {
        //        //qDebug() << "avcodec_receive_frame failed, " << parseError(ret);
        //        return false;

        //    }
        //    else if (ret < 0)
        //    {
        //        qDebug() << "avcodec_receive_frame failed, " << parseError(ret);
        //        return ret;
        //    }

        //    //获取数据同时渲染
        //    dxva2_retrieve_data_call(m_vDecodeCtx, m_pYuv);
        //}
    }
    else
    {
        ret = avcodec_receive_frame(m_vDecodeCtx, m_decodeFrame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            qDebug() << "avcodec_receive_frame failed, " << VAUtil::parseErr(ret);
            return false;

        }
        else if (ret < 0)
        {
            qDebug() << "avcodec_receive_frame failed, " << VAUtil::parseErr(ret);
            return ret;
        }
    }

    return 0;
}

bool PlayerControler::allocFFFmpegResource()
{
    AVDictionary* options = nullptr;
    if (m_path.startsWith("rtsp"))
    {
        av_dict_set(&options, "stimeout", "10000000", 0);   // 10秒，太小的话avformat_open_input会返回Operation not permitted
        av_dict_set(&options, "max_delay", "100000", 0);    // 100毫秒

        if (CONFIG.isRtspOverTcp())
        {
            av_dict_set(&options, "rtsp_transport", "tcp", 0);
        }
        else
        {
            av_dict_set(&options, "rtsp_transport", "udp", 0);
            // UDP协议需要增大buffer_size，不然播放会出现花屏,10M，TCP应该不需要设置
            av_dict_set(&options, "buffer_size", "10240000", 0);
        }
    }

    int ret = 0;
    QByteArray burl = m_path.toUtf8();
    //QMutexLocker locker(&m_mutex);
    ret = avformat_open_input(&m_fmtCtx, burl.constData(), nullptr, &options); // 打开解封装器
    if (ret != 0)
    {
        qDebug() << "avformat_open_input" << m_path << "failed:" << VAUtil::parseErr(ret);
        closeInput();
        return false;
    }

    if (m_fmtCtx->duration <= 0)    // 如果读取到的媒体信息有误
    {
        // 读取文件信息（有些媒体文件在打开时读不到，需要使用此函数）
        if (CONFIG.reduceAnalyzeTime())
        {
            // 启用这两个选项会获取不到fps
            m_fmtCtx->probesize = 1024;
            m_fmtCtx->max_analyze_duration = 1000;
        }
        ret = avformat_find_stream_info(m_fmtCtx, Q_NULLPTR);
        if (ret < 0)
        {
            qDebug() << "avformat_find_stream_info failed:" << VAUtil::parseErr(ret);
            closeInput();
            return false;
        }
    }

    QString vcodecName;
    for (int i = 0; i < m_fmtCtx->nb_streams; ++i)
    {
        AVStream* stream = m_fmtCtx->streams[i];
        AVCodecParameters* codecpar = stream->codecpar;
        if (codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
        {
            // 暂不支持音频
            continue;
        }

        AVCodecID codecId = codecpar->codec_id;
        AVCodec* decoder = avcodec_find_decoder(codecId);
        if (!decoder)
        {
            qDebug() << "avcodec_find_decoder failed" << codecId;
            closeInput();
            return false;
        }

        //从视频流中拷贝参数到codecCtx
        m_vDecodeCtx = avcodec_alloc_context3(decoder);
        if ((ret = avcodec_parameters_to_context(m_vDecodeCtx, codecpar)) < 0)
        {
            qDebug() << "Video avcodec_parameters_to_context failed:" << VAUtil::parseErr(ret);
            closeInput();
            return false;
        }
        m_vDecodeCtx->flags |= AV_CODEC_FLAG_LOW_DELAY;
        // 尝试显示缩略图
        m_vDecodeCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        if (m_hwDecode)
        {
        }
        else
        {
            m_vDecodeCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
        }

        m_videoStream = i;
        m_fps = VAUtil::r2d(stream->avg_frame_rate);

        ret = avcodec_open2(m_vDecodeCtx, decoder, Q_NULLPTR);
        if (0 != ret)
        {
            //qWarning() << "Open video decoder Error:" << parseError(err);
            closeInput();
            return false;
        }
        vcodecName = decoder->name;
    }

    qDebug().nospace() << QString("Video size: %1x%2, FPS: %3, type: %4, pixFmt: %5")
        .arg(m_vDecodeCtx->width).arg(m_vDecodeCtx->height).arg(m_fps)
        .arg(vcodecName).arg(m_vDecodeCtx->pix_fmt);

    if (0 == m_vDecodeCtx->width || 0 == m_vDecodeCtx->height)
    {
        closeInput();
        return false;
    }
    return true;
}

void PlayerControler::releaseFFmpegResource()
{
    if (m_vDecodeCtx)
    {
        m_vDecodeCtx->opaque = nullptr;
        avcodec_free_context(&m_vDecodeCtx);
        m_vDecodeCtx = nullptr;
    }
    if (m_fmtCtx)
    {
        avformat_close_input(&m_fmtCtx);
        m_fmtCtx = nullptr;
    }
    if (m_swsCtx)
    {
        sws_freeContext(m_swsCtx);
        m_swsCtx = nullptr;
    }
    {
        QMutexLocker lock(&m_decodeFrameMtx);
        if (m_decodeFrame)
        {
            av_frame_free(&m_decodeFrame);
            m_decodeFrame = nullptr;
        }
    }
}
