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

// SQLITEֻ���ļ��������룬û���û��������˿�
#define DbName ""
#define DbPwd ""

Database::Database(QObject *parent)
    : QObject(parent)
    , m_connectionName(QUuid::createUuid().toString())
{
}

Database::~Database()
{
    // QSqlDatabase::database��������������Ƴ�ʱ��Ҫ���ڴ�����У���ֹÿ����������
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
    // �ж����ݿ������Ƿ�֧��sqlite
    QStringList drivers = QSqlDatabase::drivers();
    if (!drivers.contains("SQLITECIPHER"))
    {
        // ��������Ϊȱ��qsql��̬��
        qDebug() << tr("Failed to initialize database, check if sqlitecipher.dll is missing");
        return false;
    }

#if 0
    // ���ݿ��ļ�������ȶ԰汾����������Ӱ�װĿ¼����һ������
    QString backupDb = GlobalUtil::findDB(BaseUtil::modulePath());
    QString dbPath = GlobalUtil::findDB(BaseUtil::modulePath() + "/data/");
    if ((!dbPath.isEmpty()) && QFile::exists(dbPath))
    {
        if (backupDb.isEmpty())
        {
            qDebug() << QStringLiteral("���ݿⲻ���ڣ�");
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
        CMessageBox::info(qstr("���ݿ��ļ������ڣ�����װ�ͻ���"));
        qDebug() << qstr("�ļ�%1������").arg(dbPath);
        return false;
    }

    // ���ó�ʱʱ�䣬��ֹƵ���ܾ�����
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
