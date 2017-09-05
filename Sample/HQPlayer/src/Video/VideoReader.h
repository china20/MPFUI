// 华勤科技版权所有 2010-2021
// 
// 文件名：VideoReader.h
// 功  能：主界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2017-01-02
// 
// ============================================================================

#ifndef _VIDEOREADER_H_
#define _VIDEOREADER_H_

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include <libavutil/time.h>
    #include "libavutil/pixfmt.h"
    #include "libswscale/swscale.h"
    #include "libswresample/swresample.h"
}

#include <src/Video/VideoPlayThread.h>

// 内部常量定义
static const AVRational TIMEBASE_MS = { 1, 1000 };

class VideoDecodeThr : public suic::Thread
{
public:

    VideoDecodeThr(VideoInfo* vInfo, suic::InvokeProxy* reflesh);
    ~VideoDecodeThr();

    void Run();

private:

    double SynchronizeVideo(AVFrame *srcFrame, double pts);

private:

    VideoInfo* _videoInfo;
    suic::InvokeProxy* _reflesh;
    VideoPlayThread* _playThread;
};

class VideoReaderThr : public suic::Thread
{
public:

    VideoReaderThr(suic::String filename, suic::InvokeProxy* reflesh);
    ~VideoReaderThr();

    void Run();

    void StopPlay();
    void PausePlay(bool bPause);
    bool IsPlaying() const;
    bool IsPause() const;

    void SetPlayVolume(int volume);
    void SetPlayProgress(float v);

private:

    void DoSeekFrameReq();
    int InitAudioComponent(VideoInfo *pVI, int streamIndex);

private:

    suic::String _filename;
    VideoInfo _videoInfo;

    suic::InvokeProxy* _reflesh;
    VideoDecodeThr* _decodeThr;
};

#endif
