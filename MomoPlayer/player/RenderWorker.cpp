#include "RenderWorker.h"
#include "PlayerControler.h"
#include "GlobalDefine.h"

#include <QTime>
#include <QWidget>
#include <QDebug>
#include <QThread>

using namespace std;

RenderWorker::RenderWorker(PlayerControler *ctrl, QObject *parent)
    : QObject(parent)
    , m_ctrl(ctrl)
{
    m_thread = new QThread;
    this->moveToThread(m_thread);
    connect(m_thread, &QThread::started, this, &RenderWorker::process);
    //connect(m_thread, &QThread::finished, m_thread, &QThread::deleteLater);
    //connect(this, &RenderWorker::finished, m_thread, &QThread::quit);
    //connect(this, &RenderWorker::finished, this, &RenderWorker::deleteLater);
    qDebug() << "RenderWorker()";
}

RenderWorker::~RenderWorker()
{
    delete m_thread;
    m_thread = nullptr;
    qDebug() << "~RenderWorker()";
}

void RenderWorker::start()
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

void RenderWorker::stop()
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
    m_cvStopped.notify_one();

    qDebug() << "RenderWorker::stop 1";
    m_thread->quit();
    m_thread->wait();
    qDebug() << "RenderWorker::stop 2";
}

void RenderWorker::process()
{
    qDebug() << qstr("RenderWorker线程开始");
    if (!m_ctrl || !m_ctrl->widget())
    {
        return;
    }
    int w = 0;
    int h = 0;
    QTime t;
    int duration = 1000 / m_ctrl->getFps();
    QWidget* widget = m_ctrl->widget();

    while (!m_isStopped)
    {
        t.restart();
        if (m_ctrl->canPlay() && widget->isVisible())
        {
            w = widget->width() / 2 * 2;
            h = widget->height() / 2 * 2;
            if (w > 0 && h > 0)
            {
                QImage img(w, h, QImage::Format_ARGB32);
                if (m_ctrl->toRgb(img.bits(), w, h))
                {
                    emit draw(img);
                }
            }
        }

        int sleepTime = duration - t.elapsed();
        if (sleepTime > 0)
        {
            //QThread::msleep(st);
            unique_lock<mutex> lock(m_mtxStopped);
            m_cvStopped.wait_for(lock, chrono::milliseconds(sleepTime), [this]() {
                bool b = m_isStopped;
                return b;
            });
        }
    }
    qDebug() << qstr("RenderWorker线程退出");
}
