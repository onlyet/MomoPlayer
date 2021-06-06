#pragma once

#include <QObject>

#include <atomic>
#include <condition_variable>
#include <mutex>

class PlayerControler;
class QThread;

class DecodeWorker : public QObject
{
    Q_OBJECT

public:
    DecodeWorker(PlayerControler *ctrl, QObject *parent = nullptr);
    ~DecodeWorker();

signals:
    //void finished();

public slots:
    void start();
    void stop();
    //void pause();
    void process();

private:
    QThread                     *m_thread = nullptr;
    PlayerControler             *m_ctrl = nullptr;
    std::atomic_bool            m_isStopped{ true };
    //bool                        m_isPaused = false;
    //std::condition_variable     m_notPausedCond;
    //std::mutex                  m_notPausedMtx;
};
