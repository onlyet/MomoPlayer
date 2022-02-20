#pragma once

#include <QObject>
#include <QVariant>

/*-----------------------------------------------配置文件key-----------------------------------------*/
// 常修改的
#define UserRtspUrl                     "User/RtspUrl"
#define RecordPath                      "User/RecordPath"            // 录像保存路径

#define ReduceAnalyzeTime               "App/ReduceAnalyzeTime"
#define UseVectoredExceptionHandler     "App/UseVectoredExceptionHandler"
#define RtspOverTcp                     "App/RtspOverTcp"
#define EnableAudioStream               "App/EnableAudioStream"
#define EnableDXVA                      "App/EnableDXVA"


/*--------------------------------------------------------------------------------------------------*/


class QSettings;

class GlobalConfig : public QObject
{
	Q_OBJECT

public:
	static GlobalConfig& instance();

	bool init();

    QString rtspUrl() const;
    void saveRtspUrl(const QString &url);

    bool reduceAnalyzeTime();
    bool audioStreamEnabled();

    // 视频录制路径
    QString recordPath();
    void setRecordPath(const QString &path);
    void enableRecordVideo(bool enabled);
    bool recordVideoEnabled();
    bool isRtspOverTcp() const;
    bool DXVAEnabled() const;

private:
	GlobalConfig(QObject *parent = Q_NULLPTR);
	~GlobalConfig();

    bool loadUserConfig();
    //bool loadAppConfig();

    bool saveUserConfig();
    //bool saveAppConfig();

    bool isRecordDirExist(QSettings &setting);

    // 检查磁盘空间
    bool isDiskSpaceEnough();

private:
    QString     m_appPath;

    QString     m_rtspUrl;
    QString		m_recordPath;               // 录制视频保存路径
    bool        m_recordEnabled = true;     // 是否允许录制视频

    bool        m_rtspOverTcp;
    bool        m_reduceAnalyzeTime;
    bool        m_enableAudioStream;
    bool        m_enableDXVA;
};

#define CONFIG GlobalConfig::instance()