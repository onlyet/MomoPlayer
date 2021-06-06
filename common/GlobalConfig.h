#pragma once

#include <QObject>
#include <QVariant>

/*-----------------------------------------------配置文件key-----------------------------------------*/
// 常修改的
#define RecordPath                      "Volatile/RecordPath"            // 录像保存路径

#define ReduceAnalyzeTime               "Common/ReduceAnalyzeTime"
#define UseVectoredExceptionHandler     "Common/UseVectoredExceptionHandler"
#define RtspOverTcp                     "Common/RtspOverTcp"
#define EnableAudioStream               "Common/EnableAudioStream"


#define UserRtspUrl                     "RtspUrl"
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

private:
	GlobalConfig(QObject *parent = Q_NULLPTR);
	~GlobalConfig();

    bool loadUserConfig();
    bool loadAppConfig();

    bool saveUserConfig();
    bool saveAppConfig();

    bool isRecordDirExist(QSettings &setting);

    // 检查磁盘空间
    bool isDiskSpaceEnough();

private:
    bool        m_reduceAnalyzeTime;
    bool        m_enableAudioStream;
    QString		m_recordPath;               // 录制视频保存路径
    bool        m_recordEnabled = true;     // 是否允许录制视频
    bool        m_rtspOverTcp;
    QString     m_rtspUrl;
};

#define CONFIG GlobalConfig::instance()