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
    * 时间，日期
    */
    QString currentDateTimeString(const QString &format = DATETIME_FORMAT_DEFAULT);
    QString formatTime(int secs, const QString &format);
    QString timestamp2String(qint64 ms);
    qint64 mSecsSinceEpoch();

    // 当前线程开启事件循环等待msecs毫秒, 返回QEventLoop执行结果
    int esleep(QEventLoop *loop, int msecs);

    /*
    * 文件，目录
    */
    // 检查完整文件路径，返回合理的完整文件路径。如果所属路径不存在则创建。isCover表示是否删除已有
    QString checkFile(const QString &filepath, bool isCover = false);

    // 循环删除目录. 无法删除正在使用的文件和目录
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

    // QLabel显示文本，超过最大宽度则显示省略号
    void setTextWithEllipsis(QLabel *label, const QString &text);

    /*
    * JSON
    */
    QString Variant2String(const QVariant &map);


    // 使程序唯一存在，如果已存在返回false
    bool setProgramUnique(const QString &name);

    // 获取硬件信息，阻塞调用
    QString getWMIC(const QString &cmd);

    // app版本号
    QString appVersion();

    // 是否有英伟达显卡
    bool haveNvidiaGpu();

    QString logDir();
    QString logRtspUrl(const QString &url);

};

#endif // UTIL_H
