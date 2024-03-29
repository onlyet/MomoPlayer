#ifndef LOG_H
#define LOG_H

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QMutex>
#include <iostream>
#include <QDateTime>
#include <QThread>
#include <QCoreApplication>
#include <QStandardPaths>
#include <CMessageBox.h>

#include <sstream>

#define LogFileMaxCount 10      // 历史日志文件最大个数
#define LogMaxBytes (10<<20)    // 单个历史日志文件最大字节数
#define LogDirName "log"        // 日志文件存放目录

static QString s_LogName = "log";
static QString s_LogVer = "0.0.0.0";

static QString getLogFileName(const QString &fileName, quint32 idx = 0)
{
#if 0
	QString exeDirPath = QCoreApplication::applicationDirPath();
#endif
    QString exeDirPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    if(idx == 0)
    {
        return QString("%1/%2/%3.log").arg(exeDirPath).arg(LogDirName).arg(fileName);
    }
    return QString("%1/%2/%3_%4.log").arg(exeDirPath).arg(LogDirName).arg(fileName).arg(idx);
}

static void writeLog(const QString &msg, const QString &fileName = s_LogName)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    QFile sLogFile(getLogFileName(fileName));

    // 日志文件上限为10M，最多10个历史日志，1是最新的历史日志，10是最旧的
    static const qint64 maxSize = LogMaxBytes;
    if (sLogFile.size() > maxSize)
    {
        QFile file(getLogFileName(fileName, LogFileMaxCount));
        if(file.exists()) file.remove();
        for(int i = LogFileMaxCount-1; i >= 0; --i)
        {
            QString path1 = getLogFileName(fileName, i);
            QString path2 = getLogFileName(fileName, i+1);
            file.setFileName(path1);
            file.rename(path2);
        }
    }
    sLogFile.setFileName(getLogFileName(fileName));
    if (sLogFile.open(QIODevice::Append))
    {
        sLogFile.write(msg.toLocal8Bit());
        sLogFile.close();
    }
    else
    {
        //std::cout << "[Warn]" << sLogFile.errorString().toLocal8Bit().constData() << std::endl;
    }
}

static void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //static QMutex mutex;
    //QMutexLocker locker(&mutex);

    if(msg.isEmpty())
    {
        writeLog("\r\n");
        return;
    }

    QString text;
    switch(type)
    {
    case QtInfoMsg:
        text = QString("[Info]");
        break;

    case QtDebugMsg:
        text = QString("[Debug]");
        break;

    case QtWarningMsg:
        text = QString("[Warn]");
        break;

    case QtCriticalMsg:
        text = QString("[Crtcl]");
        break;

    // 出现断言时，可以把断点定在这里
    case QtFatalMsg:
        text = QString("[Fatal]");
        break;
    }

    QString context_info = "";
    QString fileText = "";
    QString funcText;
    if (context.line > 0)
    {
        fileText = QString(context.file);
        if (!fileText.isEmpty())
        {
            fileText.replace(QRegExp(".+\\\\"), "");
            QStringList fl = fileText.split("/");
            fileText = fl.last();
        }

        funcText = QString(context.function);
        if (!funcText.isEmpty())
        {
            int idx = funcText.indexOf('(');
            if (idx >= 0)
            {
                funcText.truncate(idx);
                QStringList fl = funcText.split(" ");
                funcText = fl.last();
            }
            QStringList fl = funcText.split("::");
            funcText = fl.last();
        }

        context_info = QString("[%1:%2][%3]").arg(fileText).arg(context.line).arg(funcText);
    }

	std::ostringstream os;
	os << std::this_thread::get_id();
	QString current_thread = QString("[%1]").arg(QString::fromStdString(os.str()));
    QString current_date_time = QString("[%1]").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
    QString message = QString("%1%2%3%4 %5\r\n").arg(current_date_time).arg(text).arg(current_thread).arg(context_info).arg(msg);

    writeLog(message);
}

bool makeLogDir()
{
#if 0
    QDir dir(QCoreApplication::applicationDirPath());
    if (!dir.exists(LogDirName))
    {
        dir.mkdir(LogDirName);
    }
#endif
    QString dirPath = QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
        .arg(LogDirName);
    QDir dir;
    if (!dir.exists(LogDirName))
    {
        if (!dir.mkpath(dirPath))
        {
            QString msg = QStringLiteral("创建目录%1失败").arg(dirPath);
            qInfo() << msg;
            CMessageBox::info(msg);
            return false;
        }
    }
    return true;
}

bool LogInit(const QString &logName = QString(), const QString &logVer = QString())
{
    if(!logName.isEmpty()) s_LogName = logName;
    if(!logVer.isEmpty()) s_LogVer = logVer;

    if (!makeLogDir())
    {
        return false;
    }

    qInstallMessageHandler(outputMessage);

    qInfo();qInfo();qInfo();
    qInfo("\r\n    +---------------------------------+"
          "\r\n    +  %s Start.  V %s"
          "\r\n    +---------------------------------+",
          qPrintable(logName), qPrintable(logVer));

    qDebug() << "Qt version: " << qVersion();
    return true;
}

#endif // LOG_H
