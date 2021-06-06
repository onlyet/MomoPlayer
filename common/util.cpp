#include "util.h"

#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QLocalSocket>
#include <QLocalServer>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
//#include <QCoreApplication>
#include <QApplication>
#include <QProcess>
#include <QDesktopWidget>
#include <QLabel>

#ifdef WIN32
#include <DXGI.h>
#pragma comment(lib, "DXGI.lib")
#endif


QString util::currentDateTimeString(const QString &format)
{
    return QDateTime::currentDateTime().toString(format);
}

int util::esleep(QEventLoop *loop, int msecs)
{
    QTimer *timer = new QTimer(loop);
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, loop, &QEventLoop::quit);
    timer->start(msecs);
    return loop->exec();
}

bool util::setProgramUnique(const QString &name)
{
    QLocalSocket localSocket;
    localSocket.connectToServer(name, QIODevice::WriteOnly);
    if(localSocket.waitForConnected())
    {
        char kk[1] = {'\0'};
        localSocket.write(kk, 3);
        localSocket.close();

        qWarning() << name << "is Existed.";
        return false;
    }
    localSocket.close();

    QLocalServer::removeServer(name);
    QLocalServer *localServer = new QLocalServer(qApp);
    localServer->listen(name);
    return true;
}

QString util::checkFile(const QString &filepath, bool isCover)
{
    QFileInfo finfo(filepath);
    QString suffix = finfo.completeSuffix();
    QString name = finfo.baseName();
    QString path = finfo.absolutePath();
    QDir dir;
    dir.mkpath(path);

    QString url = filepath;
    QFile file(url);
    if (isCover)
    {
        if (file.exists())
        {
            file.remove();
        }
    }
    else
    {
        int i = 0;
        while(file.exists())
        {
            ++i;
            url = QString("%1/%2_%3.%4").arg(path).arg(name).arg(i).arg(suffix);
            file.setFileName(url);
        }
    }
    return url;
}

bool util::rmDir(const QString &path)
{
    if (path.isEmpty())
        return false;

    QDir dir(path);
    if(!dir.exists())
        return true;

    return dir.removeRecursively();
}

qint64 util::mSecsSinceEpoch()
{
    return QDateTime::currentMSecsSinceEpoch();
}

// 获取cpu名称：       wmic cpu get Name
// 获取cpu核心数：     wmic cpu get NumberOfCores
// 获取cpu线程数：     wmic cpu get NumberOfLogicalProcessors
// 查询cpu序列号：     wmic cpu get processorid
// 查询主板序列号：    wmic baseboard get serialnumber
// 查询BIOS序列号：    wmic bios get serialnumber
// 查看硬盘：          wmic diskdrive get serialnumber
// wmic csproduct get UUID
QString util::getWMIC(const QString &cmd)
{
    QProcess p;
    p.start(cmd);
    bool success = p.waitForFinished(3000);
    if (!success)
    {
        return QString();
    }
    QString result = QString::fromLocal8Bit(p.readAllStandardOutput());
    QStringList list = cmd.split(" ");
    result = result.remove(list.last(), Qt::CaseInsensitive);
    result = result.replace("\r", "");
    result = result.replace("\n", "");
    result = result.simplified();
    return result;
}

int util::screenWidth()
{
    return QApplication::desktop()->geometry().width();
}

int util::screenHeight()
{
    return QApplication::desktop()->geometry().height();
}

int util::newWidth(int width)
{
    int screenWidth = QApplication::desktop()->geometry().width();
    int newWidth = width / (float)1920 * screenWidth;
    return newWidth;
}

QSize util::newSize(QSize size)
{
    int screenWidth = QApplication::desktop()->geometry().width();
    int screenHeight = QApplication::desktop()->geometry().height();

    int newWidth = size.width() / (float)1920 * screenWidth;
    int newHeight = size.height()/ (float)1080 * screenHeight;
    return QSize(newWidth, newHeight);
}

void util::resetMaxWidth(QLabel *label, int maxWidth)
{
    label->setMaximumWidth(util::newWidth(maxWidth));
}

void util::setTextWithEllipsis(QLabel *label, const QString &text)
{
    QFontMetrics fontWidth(label->font());
    QString elideNote = fontWidth.elidedText(text, Qt::ElideRight, label->maximumWidth());
    label->setText(elideNote);//显示省略号的字符串
    label->setToolTip(text);
}

QString util::logDir()
{
    return QString("%1/log").arg(QApplication::applicationDirPath());
}

QString util::appVersion()
{
    QString appVer = QString("%1").arg(QApplication::applicationVersion());
    appVer.chop(2);
    return appVer;
}

bool util::haveNvidiaGpu()
{
    IDXGIFactory * pFactory;
    IDXGIAdapter * pAdapter;
    std::vector <IDXGIAdapter*> vAdapters;            // 显卡           
    int iAdapterNum = 0; // 显卡的数量  

    // 创建一个DXGI工厂  
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));

    if (FAILED(hr))
    {
        qWarning() << "CreateDXGIFactory failed";
        return false;
    }

    // 枚举适配器  
    while (pFactory->EnumAdapters(iAdapterNum, &pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        vAdapters.push_back(pAdapter);
        ++iAdapterNum;
    }

    for (size_t i = 0; i < vAdapters.size(); i++)
    {
        DXGI_ADAPTER_DESC adapterDesc;
        vAdapters[i]->GetDesc(&adapterDesc);
        std::wstring wstrDesc(adapterDesc.Description);
        QString desc = QString::fromStdWString(wstrDesc);
        if (desc.contains("NVIDIA GeForce"))
        {
            return true;
        }
    }
    return false;
}


QString util::formatTime(int secs, const QString &format)
{
    int h = secs / 3600;
    int m = (secs % 3600) / 60;
    int s = (secs % 3600) % 60;

    return format.arg(h).arg(m).arg(s);
}

QString util::timestamp2String(qint64 ms)
{
    return QDateTime::fromMSecsSinceEpoch(ms).toString("yyyy-MM-dd hh:mm:ss.zzz");
}

QString util::Variant2String(const QVariant &var)
{
    return QJsonDocument::fromVariant(var).toJson(QJsonDocument::Compact);
}

QString util::logRtspUrl(const QString &url)
{
    QString ret;
    QStringList sl = url.split('@');
    if (2 != sl.length())
    {
        return ret;
    }
    QString ipPart = sl.last();
    QStringList sl2 = ipPart.split('/');
    if (sl2.empty())
    {
        return ret;
    }
    QString channel = sl2.last();
    QString ipPort = sl2.first();
    QStringList sl3 = ipPort.split(':');
    if (sl3.empty())
    {
        return ret;
    }
    QString ip = sl3.first();
    QStringList sl4 = ip.split('.');
    if (4 != sl4.length())
    {
        return ret;
    }
    return QString("%1-%2-%3").arg(sl4[2]).arg(sl4[3]).arg(channel);
}

bool util::isFileExist(const QString &path)
{
    QFile file(path);
    return file.exists();
}

QStringList util::filePathListInDir(const QString &dirPath, QStringList filter)
{
    QDir dir(dirPath);
    if (!dir.exists())
    {
        return QStringList();
    }
    QStringList pathList;
    QFileInfoList list = dir.entryInfoList(filter, QDir::Files | QDir::Readable, QDir::Name);
    for each (QFileInfo fi in list)
    {
        pathList.append(fi.absoluteFilePath());
    }
    return pathList;
}

bool util::isDriveExist(const QString &drive)
{
    for each (const QFileInfo &fi in QDir::drives())
    {
        qDebug() << fi.filePath();
        if (fi.filePath() == drive)
        {
            return true;
        }
    }
    return false;
}
