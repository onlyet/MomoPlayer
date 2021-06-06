#include "GlobalSignalSlot.h"

GlobalSignalSlot::GlobalSignalSlot(QObject *parent)
    : QObject(parent)
{
}

GlobalSignalSlot::~GlobalSignalSlot()
{
}

void GlobalSignalSlot::notifyCrash()
{
    emit crashed();
}

void GlobalSignalSlot::IpcParamsErrorSlot(const QString &msg)
{
    emit IpcParamsErrorSig(msg);
}
