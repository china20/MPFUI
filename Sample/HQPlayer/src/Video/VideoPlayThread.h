// 华勤科技版权所有 2010-2021
// 
// 文件名：VideoPlayThread.h
// 功  能：主界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2017-01-02
// 
// ============================================================================

#ifndef _VIDEOPLAYTHREAD_H_
#define _VIDEOPLAYTHREAD_H_

#define SYNC_THRESHOLD 5
#define NOSYNC_THRESHOLD 5

#define SDL_AUDIO_BUFFER_SIZE 1024
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include <libavutil/time.h>
    #include "libavutil/pixfmt.h"
    #include "libswscale/swscale.h"
    #include "libswresample/swresample.h"
}

#include "SDL.h"

#include <src/Common/ThreadQueue.h>

typedef delegate<void(bool, int)> PlayVideoCb;

class BmpInfo : public suic::Object
{
public:

    suic::Bitmap bmp;
    int64_t duration;
    int64_t curDuration;

    BmpInfo()
    {
        duration = 0;
        curDuration = 0;
    }

    ~BmpInfo()
    {
        bmp.Clear();
    }
};

class PlayInfo
{
public:

    int repeat_pict;
    double videoPts;
    BmpInfo* bmpInfo;

    PlayInfo()
    {
        bmpInfo = NULL;
        repeat_pict = 0;
        videoPts = 0;
    }

    ~PlayInfo()
    {
        if (NULL != bmpInfo)
        {
            bmpInfo->unref();
        }
    }
};

class VideoInfo
{
public:

    AVPacket *vPacket;

    AVFormatContext *formatCtx;
    AVFrame *audio_frame;

    int audioIndex;
    int videoIndex;

    AVStream* audioStrm;

    AVCodec* aCodec;
    AVCodec* vCodec;

    AVCodecContext *vCodecCtx;
    AVCodecContext *aCodecCtx;

    SwrContext *audioSwrCtx;

    core::NormalQueue<AVPacket*> vedioQueue;
    core::NormalQueue<AVPacket*> audioQueue;

    AVRational vrate;

    // 视频总长度
    int64_t iDuration;

    int tickavdiff;
    int tickframe;
    int ticksleep;
    int ticklast;

    bool isPause;
    bool quit;
    bool readFinished;
    bool decodeFinished;

    bool seek_req;
    bool seek_flag_audio;
    bool seek_flag_video;

    // 跳转到播放的时间点
    int seekTime;
    int64_t seek_pos;

    int volume;
    suic::Uint32 audioID;
    
    suic::Uint32 audio_hw_buf_size;
    AVSampleFormat audio_src_fmt;
    AVSampleFormat audio_tgt_fmt;
    suic::Int32 audio_src_freq;
    suic::Int32 audio_tgt_freq;
    suic::Int64 audio_src_channel_layout;
    suic::Int64 audio_tgt_channel_layout;

    suic::Uint8 audio_src_channels;
    suic::Uint8 audio_tgt_channels;

    suic::Int32 audio_buf_size;
    suic::Int32 audio_buf_index;

    suic::Int32 audio_pkt_size;

    suic::Uint8 *audio_buf;
    suic::Uint8 *audio_pkt_data;

    DECLARE_ALIGNED(16,uint8_t,audio_buf_temp) [AVCODEC_MAX_AUDIO_FRAME_SIZE * 4];
    //uint8_t            audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];

    AVPacket* audio_pkt;

    double videoClock;
    double audioClock;
    double frame_last_delay;
    double frame_last_pts;

    VideoInfo()
        : readFinished(false)
        , decodeFinished(false)
        , isPause(false)
        , quit(false)
        , seek_req(false)
        , seek_flag_audio(false)
        , seek_flag_video(false)
        , vPacket(NULL)
        , formatCtx(NULL)
        , audioIndex(-1)
        , videoIndex(-1)
        , audioStrm(NULL)
        , audio_frame(NULL)
        , iDuration(0)
        , vCodecCtx(NULL)
        , aCodecCtx(NULL)
        , audioSwrCtx(NULL)
        , seekTime(0)
        , seek_pos(0)
        , videoClock(0)
        , audioClock(0)
        , tickavdiff(0)
        , tickframe(0)
        , ticksleep(0)
        , ticklast(0)
        , audio_buf(NULL)
        , audio_pkt(NULL)
        , audio_pkt_size(0)
        , audio_pkt_data(NULL)
        , audioID(0)
        , audio_hw_buf_size(0)
        , audio_src_fmt(AVSampleFormat::AV_SAMPLE_FMT_S16P)
        , audio_tgt_fmt(AVSampleFormat::AV_SAMPLE_FMT_S16P)
        , audio_src_freq(0)
        , audio_tgt_freq(0)
        , audio_src_channel_layout(0)
        , audio_tgt_channel_layout(0)
        , audio_src_channels(0)
        , audio_tgt_channels(0)
        , audio_buf_size(0)
        , audio_buf_index(0)
        , frame_last_delay(0)
        , frame_last_pts(0)
        , volume(SDL_MIX_MAXVOLUME)
    {

    }

    ~VideoInfo();

    AVStream* GetVideoStrm()
    {
        return GetStream(videoIndex);
    }

    AVStream* GetAudioStrm()
    {
        return GetStream(audioIndex);
    }

    AVStream* GetStream(int index)
    {
        if (NULL != formatCtx && index >= 0)
        {
            return formatCtx->streams[index];
        }
        else
        {
            return NULL;
        }
    }

    void PushVideoPacket(AVPacket* packet)
    {
        vedioQueue.Add(packet);
    }

    void PushAudioPacket(AVPacket* packet)
    {
        audioQueue.Add(packet);
    }

    void ClearVideos();
    void ClearAudios();
};

class VideoPlayThread : public suic::Thread
{
public:

    VideoPlayThread(VideoInfo* vInfo, suic::InvokeProxy* reflesh);
    ~VideoPlayThread();

    void Run();

    void AddPlayInfo(PlayInfo* playInfo);

private:

    double SynchronizeVideo(double pts, int repeat_pict);

private:

    VideoInfo* _videoInfo;
    suic::InvokeProxy* _reflesh;
    core::NormalQueue<PlayInfo*> _playInfo;
};

#endif
