#include "GlobalConfig.h"
#include <util.h>
#include <CMessageBox.h>
#include <GlobalDefine.h>

#include <QFile>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QStandardpaths>
#include <QDateTime>
#include <QDir>
#include <QStorageInfo>

GlobalConfig::GlobalConfig(QObject *parent)
	: QObject(parent)
{
}

GlobalConfig::~GlobalConfig()
{
    saveUserConfig();
    saveAppConfig();
}

bool GlobalConfig::loadUserConfig()
{
    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString path = QString("%1/%2.ini").arg(dirPath)
        .arg(UserConfigFileName);
    QDir dir;
    if (!dir.mkpath(dirPath))
    {
        QString msg = QStringLiteral("创建目录%1失败").arg(dirPath);
        qInfo() << msg;
        CMessageBox::info(msg);
        return false;
    }
    QSettings setting(path, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    QFile file(path);
    if (!file.exists())
    {
        if (!file.open(QIODevice::ReadWrite))
        {
            qWarning() << QString("Can not open file: %1").arg(path);
            QString msg = QStringLiteral("无法打开用户配置文件，程序退出");
            qWarning() << msg;
            CMessageBox::info(msg);
            return false;
        }
    }

    m_rtspUrl = setting.value(UserRtspUrl).toString();
    if (!isRecordDirExist(setting) || !isDiskSpaceEnough())
    {
        return false;
    }
    return true;
}

bool GlobalConfig::loadAppConfig()
{
    QString path = QString("%1/%2.ini").arg(QCoreApplication::applicationDirPath()).arg(QCoreApplication::applicationName());
    QSettings setting(path, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    QFile file(path);
    if (!file.exists())
    {
        if (!file.open(QIODevice::ReadWrite))
        {
            qWarning() << QString("Can not open file: %1").arg(path);
            QString msg = QStringLiteral("无法打开app配置文件，程序退出");
            qWarning() << msg;
            CMessageBox::info(msg);
            return false;
        }
    }

    m_reduceAnalyzeTime = setting.value(ReduceAnalyzeTime, false).toBool();
    m_enableAudioStream = setting.value(EnableAudioStream, false).toBool();
    m_rtspOverTcp = setting.value(RtspOverTcp, true).toBool();

    return true;
}


bool GlobalConfig::saveUserConfig()
{
    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString path = QString("%1/%2.ini").arg(dirPath).arg(UserConfigFileName);
    QSettings setting(path, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    QFile file(path);
    if (!file.exists())
    {
        if (!file.open(QIODevice::ReadWrite))
        {
            qWarning() << QString("Can not open file: %1").arg(path);
            return false;
        }
    }

    setting.setValue(UserRtspUrl, m_rtspUrl);
    return true;
}

bool GlobalConfig::saveAppConfig()
{
    QString path = QString("%1/%2.ini").arg(QCoreApplication::applicationDirPath()).arg(QCoreApplication::applicationName());
    QSettings setting(path, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    QFile file(path);
    if (!file.exists())
    {
        if (!file.open(QIODevice::ReadWrite))
        {
            qWarning() << QString("Can not open file: %1").arg(path);
            return false;
        }
    }

    return true;
}

bool GlobalConfig::isRecordDirExist(QSettings &setting)
{
    m_recordPath = setting.value(RecordPath).toString();
    QString oldRecordPath = m_recordPath;
    QDir dir(m_recordPath);
    if (m_recordPath.isEmpty())
    {
        m_recordPath = QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation))
            .arg(QCoreApplication::applicationName());
        qInfo() << QStringLiteral("录制目录为空，创建默认目录: %1").arg(m_recordPath);
        if (!dir.mkpath(m_recordPath))
        {
            QString msg = QStringLiteral("创建默认录制目录%1失败").arg(m_recordPath);
            qInfo() << msg;
            CMessageBox::info(msg);
            return false;
        }
    }
    else if (!dir.exists())
    {
        if (m_recordPath.length() >= 3)
        {
            QString drive = m_recordPath.left(3);
            if (!util::isDriveExist(drive))
            {
                drive.replace(drive, "D:/", Qt::CaseInsensitive);
                if (!util::isDriveExist(drive))
                {
                    drive.replace(drive, "C:/", Qt::CaseInsensitive);
                }
                m_recordPath.replace(m_recordPath.left(3), drive, Qt::CaseInsensitive);
            }
        }

        qInfo() << QStringLiteral("创建目录：%1").arg(m_recordPath);
        if (!dir.mkpath(m_recordPath))
        {
            QString msg = QStringLiteral("创建录制目录%1失败").arg(m_recordPath);
            qInfo() << msg;
            CMessageBox::info(msg);
            return false;
        }
    }

    if (oldRecordPath != m_recordPath)
    {
        setting.setValue(RecordPath, m_recordPath);
    }
    return true;
}

GlobalConfig& GlobalConfig::instance()
{
	static GlobalConfig s_instance;
	return s_instance;
}

bool GlobalConfig::init()
{
    if (!loadUserConfig())
    {
        return false;
    }
    return loadAppConfig();
}

QString GlobalConfig::rtspUrl() const
{
    return m_rtspUrl;
}

void GlobalConfig::saveRtspUrl(const QString &url)
{
    m_rtspUrl = url;
}

bool GlobalConfig::reduceAnalyzeTime()
{
    return m_reduceAnalyzeTime;
}

bool GlobalConfig::audioStreamEnabled()
{
    return m_enableAudioStream;
}

QString GlobalConfig::recordPath()
{
    return m_recordPath;
}

void GlobalConfig::setRecordPath(const QString &path)
{
    m_recordPath = path;
}

void GlobalConfig::enableRecordVideo(bool enabled)
{
    m_recordEnabled = enabled;
}

bool GlobalConfig::recordVideoEnabled()
{
    return m_recordEnabled;
}
bool GlobalConfig::isDiskSpaceEnough()
{
    QStorageInfo si(m_recordPath);
    float available = (float)si.bytesAvailable() / 1024 / 1024 / 1024;
    float total = (float)si.bytesTotal() / 1024 / 1024 / 1024;
    float percent = available / total;
    qInfo() << QStringLiteral("录制视频目录%1所在磁盘的可用空间为%2%").arg(m_recordPath).arg((int)(percent * 100));
    if (percent <= 0.2)
    {
        m_recordEnabled = false;
        CMessageBox::info(qstr("%1盘空间不足!\n清理磁盘或修改录制视频保存路径").arg(m_recordPath.left(1)));
        return false;
    }
    return true;
}

bool GlobalConfig::isRtspOverTcp() const
{
    return m_rtspOverTcp;
}
