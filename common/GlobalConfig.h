#pragma once

#include <QObject>
#include <QVariant>

/*-----------------------------------------------�����ļ�key-----------------------------------------*/
// ���޸ĵ�
#define UserRtspUrl                     "User/RtspUrl"
#define RecordPath                      "User/RecordPath"            // ¼�񱣴�·��

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

    // ��Ƶ¼��·��
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

    // �����̿ռ�
    bool isDiskSpaceEnough();

private:
    QString     m_appPath;

    QString     m_rtspUrl;
    QString		m_recordPath;               // ¼����Ƶ����·��
    bool        m_recordEnabled = true;     // �Ƿ�����¼����Ƶ

    bool        m_rtspOverTcp;
    bool        m_reduceAnalyzeTime;
    bool        m_enableAudioStream;
    bool        m_enableDXVA;
};

#define CONFIG GlobalConfig::instance()