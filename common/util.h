#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QVariant>
#include <QSettings>
#include <QEventLoop>
#include <QJsonObject> 

#define DATETIME_FORMAT_DEFAULT         "yyyy-MM-dd hh:mm:ss"


class QLabel;

namespace util {
    /*
    * ʱ�䣬����
    */
    QString currentDateTimeString(const QString &format = DATETIME_FORMAT_DEFAULT);
    QString formatTime(int secs, const QString &format);
    QString timestamp2String(qint64 ms);
    qint64 mSecsSinceEpoch();

    // ��ǰ�߳̿����¼�ѭ���ȴ�msecs����, ����QEventLoopִ�н��
    int esleep(QEventLoop *loop, int msecs);

    /*
    * �ļ���Ŀ¼
    */
    // ��������ļ�·�������غ���������ļ�·�����������·���������򴴽���isCover��ʾ�Ƿ�ɾ������
    QString checkFile(const QString &filepath, bool isCover = false);

    // ѭ��ɾ��Ŀ¼. �޷�ɾ������ʹ�õ��ļ���Ŀ¼
    bool rmDir(const QString &path);

    bool isFileExist(const QString &path);

    QStringList filePathListInDir(const QString &dirPath, QStringList filter = QStringList());

    bool isDriveExist(const QString &drive);

    /*
    * UI
    */
    int screenWidth();
    int screenHeight();

    int newWidth(int width);
    QSize newSize(QSize size);

    void resetMaxWidth(QLabel *label, int maxWidth);

    // QLabel��ʾ�ı����������������ʾʡ�Ժ�
    void setTextWithEllipsis(QLabel *label, const QString &text);

    /*
    * JSON
    */
    QString Variant2String(const QVariant &map);


    // ʹ����Ψһ���ڣ�����Ѵ��ڷ���false
    bool setProgramUnique(const QString &name);

    // ��ȡӲ����Ϣ����������
    QString getWMIC(const QString &cmd);

    // app�汾��
    QString appVersion();

    // �Ƿ���Ӣΰ���Կ�
    bool haveNvidiaGpu();

    QString logDir();
    QString logRtspUrl(const QString &url);

};

#endif // UTIL_H
