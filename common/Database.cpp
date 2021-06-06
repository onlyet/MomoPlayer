#include "Database.h"
#include <util.h>
#include <CMessageBox.h>
#include <GlobalDefine.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFileInfo>
#include <QUuid>
#include <QCoreApplication>

// SQLITE只有文件名和密码，没有用户名主机端口
#define DbName ""
#define DbPwd ""

Database::Database(QObject *parent)
    : QObject(parent)
    , m_connectionName(QUuid::createUuid().toString())
{
}

Database::~Database()
{
    // QSqlDatabase::database有特殊的作用域，移除时需要放在代码段中，防止每次新增引用
    bool openned = false;
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        openned = db.isOpen();
        if (openned)
        {
            db.close();
        }
    }

    if (openned)
    {
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

Database & Database::instance()
{
    static Database s_instance;
    return s_instance;
}

bool Database::init()
{
    // 判断数据库类型是否支持sqlite
    QStringList drivers = QSqlDatabase::drivers();
    if (!drivers.contains("SQLITECIPHER"))
    {
        // 往往是因为缺少qsql动态库
        qDebug() << tr("Failed to initialize database, check if sqlitecipher.dll is missing");
        return false;
    }

#if 0
    // 数据库文件存在则比对版本，不存在则从安装目录复制一个过来
    QString backupDb = GlobalUtil::findDB(BaseUtil::modulePath());
    QString dbPath = GlobalUtil::findDB(BaseUtil::modulePath() + "/data/");
    if ((!dbPath.isEmpty()) && QFile::exists(dbPath))
    {
        if (backupDb.isEmpty())
        {
            qDebug() << QStringLiteral("数据库不存在！");
            return false;
        }

        if (QFileInfo(dbPath).baseName() != QFileInfo(backupDb).baseName())
        {
            if (!QFile::remove(dbPath))
            {
                return false;
            }

            dbPath = BaseUtil::modulePath() + "/data/" + QFileInfo(backupDb).baseName() + ".db";
            if (!QFile::copy(backupDb, dbPath))
            {
                return false;
            }
        }
    }
    else
    {
        if (backupDb.isEmpty())
        {
            return false;
        }

        if (!BaseUtil::createDir(BaseUtil::modulePath() + "/data"))
        {
            return false;
        }

        dbPath = BaseUtil::modulePath() + "/data/" + QFileInfo(backupDb).baseName() + ".db";
        if (!QFile::copy(backupDb, dbPath))
        {
            return false;
        }
    }
#endif

    QString dbPath = QString("%1/%2.db").arg(QCoreApplication::applicationDirPath()).arg(DbName);
    if (!util::isFileExist(dbPath))
    {
        CMessageBox::info(qstr("数据库文件不存在，请重装客户端"));
        qDebug() << qstr("文件%1不存在").arg(dbPath);
        return false;
    }

    // 设置超时时间，防止频繁拒绝访问
    QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER", m_connectionName);
    db.setDatabaseName(dbPath);
    db.setPassword(DbPwd);
    db.setConnectOptions("QSQLITE_USE_CIPHER=aes256cbc; QSQLITE_ENABLE_REGEXP");
    //db.setConnectOptions("QSQLITE_USE_CIPHER=aes256cbc; QSQLITE_ENABLE_REGEXP; QSQLITE_BUSY_TIMEOUT=100000");
    if (!db.open())
    {
        db.setConnectOptions();
        qDebug() << tr("Failed to initialize database, unable to open database: ") << db.lastError().text();
        return false;
    }
    return true;
}
