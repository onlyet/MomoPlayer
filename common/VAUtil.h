#pragma once

extern "C"
{
#include <libavutil/rational.h>
}

#include <QString>

namespace VAUtil
{
    // 初始化ffmpeg, 注册所有容器
    void initFFmpeg();

    // 解析ffmpeg错误码, 返回错误描述
    QString parseErr(int errNum);

    // 将ffmpeg的AVRational转化为可用的fps
    double r2d(AVRational r);
};
