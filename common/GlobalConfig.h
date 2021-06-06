#pragma once

#include <QObject>
#include <QVariant>

/*-----------------------------------------------�����ļ�key-----------------------------------------*/
// ���޸ĵ�
#define RecordPath                      "Volatile/RecordPath"            // ¼�񱣴�·��

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

    // ��Ƶ¼��·��
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

    // �����̿ռ�
    bool isDiskSpaceEnough();

private:
    bool        m_reduceAnalyzeTime;
    bool        m_enableAudioStream;
    QString		m_recordPath;               // ¼����Ƶ����·��
    bool        m_recordEnabled = true;     // �Ƿ�����¼����Ƶ
    bool        m_rtspOverTcp;
    QString     m_rtspUrl;
};

#define CONFIG GlobalConfig::instance()