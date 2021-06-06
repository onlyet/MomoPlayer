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

    // ��ʼ����־
    LogInit(a.applicationName(), a.applicationVersion());

    // ��ʼ������ģ��
    DumpInit();

    //ʹ����Ψһ
    if (!util::setProgramUnique(a.applicationName()))
    {
        qWarning() << qstr("�Ѵ���һ���ͻ���ʵ��");
        return -1;
    }

    // ��ʼ��ȫ������
    if (!CONFIG.init())
    {
        return -1;
    }

#if 0
    // ��ʼ�����ݿ�
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
