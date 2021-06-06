#pragma once

#include <QObject>
#include <QImage>

#include <condition_variable>
#include <mutex>

class PlayerControler;
class QWidget;
class QThread;

class RenderWorker : public QObject
{
    Q_OBJECT

public:
    RenderWorker(PlayerControler *ctrl, QObject *parent = nullptr);
    ~RenderWorker();


signals:
    void draw(const QImage &img);
    //void finished();

public slots:
    void start();
    void stop();
    void process();

private:
    QThread                     *m_thread = nullptr;
    PlayerControler             *m_ctrl = nullptr;
    std::atomic_bool            m_isStopped{ true };
    std::mutex                  m_mtxStopped;
    std::condition_variable     m_cvStopped;
};
