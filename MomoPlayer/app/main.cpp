#include "MainWin.h"
#include <log.h>
#include <Dump.h>
#include <util.h>
#include <GlobalConfig.h>
#include <GlobalDefine.h>

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 初始化日志
    LogInit(a.applicationName(), a.applicationVersion());

    // 初始化崩溃模块
    DumpInit();

    //使程序唯一
    if (!util::setProgramUnique(a.applicationName()))
    {
        qWarning() << qstr("已存在一个客户端实例");
        return -1;
    }

    // 初始化全局配置
    if (!CONFIG.init())
    {
        return -1;
    }

#if 0
    // 初始化数据库
    if (!GDB.init())
    {
        return -1;
    }
#endif

    MainWin w;
    w.show();

    QObject::connect(&a, &QApplication::aboutToQuit, []() {
        qDebug() << "aboutToQuit";
    });
    return a.exec();
    qDebug() << "after exec";
}
