#include "MomoPlayer.h"
#include <PlayerControler.h>
#include <GlobalDefine.h>

#include <QTimer>
#include <QDebug>
#include <QPainter>

MomoPlayer::MomoPlayer(QWidget *parent)
    : QWidget(parent)
    //, m_pixmap(200, 200)
{
    m_waitPic = ":/logo.png";
    //setPixmap(m_waitPic);
    m_pixmap = m_waitPic;
    //m_pixmap.load(m_waitPic);
    qDebug() << "MomoPlayer()";

    //ui->widget->
}

MomoPlayer::~MomoPlayer()
{
    qDebug() << "~MomoPlayer() 1";

    SafeRelease(m_playerControler);
    //emit destruct();
    qDebug() << "~MomoPlayer() 2";
}

void MomoPlayer::release()
{
    //if (m_playerControler)
    //{
    //    m_playerControler->release();
    //}
    if (m_playerControler)
    {
        emit releaseSig();
    }
    else
    {
        QTimer::singleShot(1, [this] {
            emit releaseFinished();
        });
    }
}

void MomoPlayer::paintEvent(QPaintEvent *pEvent)
{
    QPainter painter(this);
    painter.drawPixmap(/*rect()*/(width() - m_pixmap.width()) / 2, (height() - m_pixmap.height()) / 2, m_pixmap);
}

void MomoPlayer::play(const QString &path)
{
    if (path.isEmpty())
    {
        return;
    }
    if (!m_playerControler)
    {
        m_playerControler = new PlayerControler(this);
        connect(this, &MomoPlayer::openInput, m_playerControler, &PlayerControler::openInput);
        connect(this, &MomoPlayer::closeInput, m_playerControler, &PlayerControler::closeInput);
        connect(this, &MomoPlayer::pauseSig, m_playerControler, &PlayerControler::pause);
        //connect(this, &MomoPlayer::destruct, m_playerControler, &PlayerControler::destruct);
        connect(this, &MomoPlayer::releaseSig, m_playerControler, &PlayerControler::release);
        connect(m_playerControler, &PlayerControler::releaseFinished, this, &MomoPlayer::releaseFinished);

        connect(m_playerControler, &PlayerControler::draw, this, &MomoPlayer::draw);
        connect(m_playerControler, &PlayerControler::openInputFailed, this, &MomoPlayer::openInputFailed);
        connect(m_playerControler, &PlayerControler::closeInputFinished, this, &MomoPlayer::closeInputFinished);
        m_playerControler->start();
    }
    emit openInput(path);
}

void MomoPlayer::stop()
{
    if (!m_playerControler)
    {
        return;
    }

    emit closeInput();
}

void MomoPlayer::pause()
{
    if (!m_playerControler)
    {
        return;
    }
    emit pauseSig();
}

void MomoPlayer::closeInputFinished()
{
    QTimer::singleShot(10, [this]() {
        //setPixmap(m_waitPic);
        m_pixmap = m_waitPic;
        update();
    });
    //m_pixmap.load(m_waitPic);
}

void MomoPlayer::draw(const QImage &img)
{
    // QLabel setPixmap放大后就不能缩小了，故不能用setPixmap播放视频，改用paintEvent中绘制
    //setPixmap(QPixmap::fromImage(img));

    m_pixmap = QPixmap::fromImage(img);
    update();
}
