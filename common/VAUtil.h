#pragma once

extern "C"
{
#include <libavutil/rational.h>
}

#include <QString>

namespace VAUtil
{
    // ��ʼ��ffmpeg, ע����������
    void initFFmpeg();

    // ����ffmpeg������, ���ش�������
    QString parseErr(int errNum);

    // ��ffmpeg��AVRationalת��Ϊ���õ�fps
    double r2d(AVRational r);
};
