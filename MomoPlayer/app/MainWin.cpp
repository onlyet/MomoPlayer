#include "MainWin.h"
#include "ui_MainWin.h"
#include "MomoPlayer.h"

#include <GlobalDefine.h>
#include <FramelessHelper.h>
#include <GlobalConfig.h>

#include <QPushButton>
#include <QTimer>
#include <QDebug>
#include <QEventLoop>
#include <QTime>
#include <QMouseEvent>

#define TitlebarHeight 40

MainWin::MainWin(QWidget *parent)
    : QWidget(parent)
{
    qDebug() << "MainWin() 1";

    setWindowFlags(Qt::FramelessWindowHint);

    ui = new Ui::MainWin();
    ui->setupUi(this);

    FramelessHelper *helper = new FramelessHelper(this);
    helper->setDraggableMargins(6, 6, 6, 6);
    helper->setMaximizedMargins(3, 3, 3, 3);
    //helper->setTitleBarHeight(0);

    helper->addExcludeItem(ui->minBtn);
    helper->addExcludeItem(ui->maxBtn);
    helper->addExcludeItem(ui->closeBtn);

    connect(ui->playBtn, &QPushButton::clicked, this, &MainWin::play);
    connect(ui->pauseBtn, &QPushButton::clicked, this, &MainWin::pause);
    connect(ui->stopBtn, &QPushButton::clicked, this, &MainWin::stop);
    connect(ui->minBtn, &QPushButton::clicked, this, &MainWin::showMinimized);
    connect(ui->maxBtn, &QPushButton::clicked, this, &MainWin::switchMaxNormal);
    connect(ui->closeBtn, &QPushButton::clicked, this, &MainWin::appExit);
    connect(ui->rtspEdit, &QLineEdit::editingFinished, this, &MainWin::saveRtspUrl);
    connect(ui->player, &MomoPlayer::openInputFailed, this, &MainWin::onOpenInputFailed);

    ui->pauseBtn->hide();
    ui->rtspEdit->setText(CONFIG.rtspUrl());

    qDebug() << "MainWin() 2";
}

MainWin::~MainWin()
{
    qDebug() << "~MainWin() 1";
    delete ui;
    qDebug() << "~MainWin() 2";
}

void MainWin::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        m_dragPos = ev->globalPos() - frameGeometry().topLeft();
        m_isDraging = true;
    }
    ev->accept();
}

// Note that the returned value is always Qt::NoButton for mouse move events.
void MainWin::mouseMoveEvent(QMouseEvent *ev)
{
    if (isMaximized())
    {
        ev->ignore();
        return;
    }
    if (m_isDraging && ev->buttons() & Qt::LeftButton)
    {
        move(ev->globalPos() - m_dragPos);
        ev->accept();
    }
}

void MainWin::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        m_isDraging = false;
    }
    ev->accept();
}

void MainWin::mouseDoubleClickEvent(QMouseEvent *ev)
{
    qDebug() << ev->y() << ui->titleFrame->height();
    if (ev->button() == Qt::LeftButton && ev->y() < ui->titleFrame->height())
    {
        switchMaxNormal();
    }
    ev->accept();
}

void MainWin::play()
{
    if (m_isPaused)
    {
        ui->player->pause();
        m_isPaused = false;
    }
    else
    {
        QString url = ui->rtspEdit->text();
        if (!url.startsWith("rtsp://"))
        {
            return;
        }
        ui->player->play(ui->rtspEdit->text());
    }
    ui->playBtn->hide();
    ui->pauseBtn->show();
}

void MainWin::pause()
{
    m_isPaused = true;
    ui->player->pause();
    ui->playBtn->show();
    ui->pauseBtn->hide();
}

void MainWin::stop()
{
    ui->player->stop();
    m_isPaused = false;
    ui->playBtn->show();
    ui->pauseBtn->hide();
}

void MainWin::switchMaxNormal()
{
    if (isMaximized())
    {
        showNormal();
        ui->maxBtn->setIcon(QIcon(":/appbar.app.png"));
        ui->maxBtn->setToolTip(qstr("最大化"));
    }
    else
    {
        showMaximized();
        ui->maxBtn->setIcon(QIcon(":/appbar.fullscreen.box.png"));
        ui->maxBtn->setToolTip(qstr("恢复尺寸"));
    }
}

void MainWin::appExit()
{
    qDebug() << qstr("点击关闭按钮");
    // 等待播放器释放资源（FFmpeg资源，等待线程退出）
    QEventLoop loop;
    connect(ui->player, &MomoPlayer::releaseFinished, [&loop]() {
        loop.quit();
    });
    ui->player->release();

    QTime t = QTime::currentTime();
    loop.exec();
    qDebug() << "loop time: " << t.elapsed();
    //emit appExitSig();
    close();
}

void MainWin::saveRtspUrl()
{
    //qDebug() << ui->rtspEdit->text();
    CONFIG.saveRtspUrl(ui->rtspEdit->text());
}

void MainWin::onOpenInputFailed()
{
    m_isPaused = false;
    ui->playBtn->show();
    ui->pauseBtn->hide();
}