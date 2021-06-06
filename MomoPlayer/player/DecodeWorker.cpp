#include "DecodeWorker.h"
#include "PlayerControler.h"
#include <GlobalDefine.h>

#include <QThread>
#include <QDebug>

extern "C"
{
#include <libavcodec/packet.h>
}

using namespace std;

DecodeWorker::DecodeWorker(PlayerControler *ctrl, QObject *parent)
    : QObject(parent)
    , m_ctrl(ctrl)
{
    m_thread = new QThread;
    this->moveToThread(m_thread);
    connect(m_thread, &QThread::started, this, &DecodeWorker::process);
    //connect(t, &QThread::finished, t, &QThread::deleteLater);
    //connect(this, &DecodeWorker::finished, t, &QThread::quit);
    //connect(this, &DecodeWorker::finished, this, &DecodeWorker::deleteLater);
    qDebug() << "DecodeWorker()";
}

DecodeWorker::~DecodeWorker()
{
    delete m_thread;
    m_thread = nullptr;
    qDebug() << "~DecodeWorker()";
}

void DecodeWorker::start()
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

void DecodeWorker::stop()
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
#if 0
    if (m_isPaused)
    {
        m_isPaused = false;
        m_notPausedCond.notify_one();
    }
#endif

    qDebug() << "DecodeWorker::stop 1";
    m_thread->quit();
    m_thread->wait();
    qDebug() << "DecodeWorker::stop 2";
}

#if 0
void DecodeWorker::pause()
{
    m_isPaused = !m_isPaused;
    if (!m_isPaused)
    {
        m_notPausedCond.notify_one();
    }
}
#endif

void DecodeWorker::process()
{
    qDebug() << qstr("DecodeWorker线程开始");
    if (!m_ctrl)
    {
        return;
    }
    while (!m_isStopped)
    {
#if 0
        // rtsp似乎不能暂停读包，不然av_read_frame一直EOF
        {
            unique_lock<mutex> lock(m_notPausedMtx);
            m_notPausedCond.wait(lock, [this]() { return !m_isPaused; });
        }
#endif
        m_ctrl->readPacketAndDecode();
    }
    qDebug() << qstr("DecodeWorker线程退出");
}


