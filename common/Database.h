#pragma once
#include <QObject>
#include <QVariant>

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    static Database& instance();

    bool init();

private:
    QString m_connectionName;
};

#define GDB Database::instance()
