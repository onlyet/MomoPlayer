#pragma once

#include <QWidget>
#include <QPoint>

namespace Ui { class MainWin; };

class QMouseEvent;

class MainWin : public QWidget
{
    Q_OBJECT

private:
    Ui::MainWin     *ui;
    bool            m_isPaused = false;
    QString         m_waitPic;
    bool            m_isDraging = false;
    QPoint          m_dragPos;

public:
    MainWin(QWidget *parent = Q_NULLPTR);
    ~MainWin();

protected:
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void mouseDoubleClickEvent(QMouseEvent *ev) override;

signals:
    void appExitSig();

private slots:
    void play();
    void pause();
    void stop();
    void switchMaxNormal();
    void appExit();
    void saveRtspUrl();
    void onOpenInputFailed();
};
