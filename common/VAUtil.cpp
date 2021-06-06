#include "VAUtil.h"

#include <QDebug>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
}

// 错误登记设置成AV_LOG_ERROR。AV_LOG_WARNING的话可能会输出大量的deprecated日志（deprecated pixel format used, make sure you did set range correctly）
#define FFmpegLogLevel AV_LOG_ERROR

void FFmpegLogFunc(void *ptr, int level, const char *fmt, va_list vl)
{
    if (level > FFmpegLogLevel)
    {
        return;
    }
    char buf[512] = { 0 };
    vsprintf_s(buf, 512, fmt, vl);
    qDebug() << "err:" << buf;
}

namespace VAUtil
{

    void initFFmpeg()
    {
        static bool s_isFFmpegInited = false;
        if (s_isFFmpegInited)
        {
            return;
        }
        s_isFFmpegInited = true;

        //av_register_all();
        avdevice_register_all();
        //avformat_network_init();
        // 将FFmpeg日志写到文件
        av_log_set_callback(FFmpegLogFunc);
        av_log_set_level(FFmpegLogLevel);
        qInfo() << "FFmpeg" << av_version_info();
    }

    QString parseErr(int errNum)
    {
        char err[1024] = { 0 };
        av_strerror(errNum, err, 1024);
        return err;
    }

    double r2d(AVRational r)
    {
        return r.den == 0 ? 0 : (double)(r.num) / (double)(r.den);
    }

};