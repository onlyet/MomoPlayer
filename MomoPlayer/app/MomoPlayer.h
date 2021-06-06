#pragma once

#include <QLabel>
#include <QPixmap>

class PlayerControler;

class MomoPlayer : public QLabel
{
    Q_OBJECT

public:
    MomoPlayer(QWidget *parent = Q_NULLPTR);
    ~MomoPlayer();

    void release();

protected:
    void paintEvent(QPaintEvent *pEvent);


signals:
    void openInput(const QString &path);
    void closeInput();
    void pauseSig();
    //void destruct();
    void releaseSig();
    void releaseFinished();
    void openInputFailed();

public slots:
    void play(const QString &path);
    void stop();
    void pause();

private slots:
    void draw(const QImage &img);
    void closeInputFinished();


private:
    PlayerControler     *m_playerControler = nullptr;
    QString             m_waitPic;
    QPixmap             m_pixmap;
};
