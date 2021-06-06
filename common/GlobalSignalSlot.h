#pragma once

#include <QObject>

class GlobalSignalSlot : public QObject
{
    Q_OBJECT

public:
    GlobalSignalSlot(QObject *parent = nullptr);
    ~GlobalSignalSlot();

    static GlobalSignalSlot& instance() {
        static GlobalSignalSlot s_instance;
        return s_instance;
    }

    void notifyCrash();

public slots:
    void IpcParamsErrorSlot(const QString &msg);

signals:
    void IpcParamsErrorSig(const QString &msg);
    void crashed();
};

#define GSignalSlot GlobalSignalSlot::instance()