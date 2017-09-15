/////////////////////////////////////////////////////
// VideoReader.cpp

#include "stdafx.h"
#include "VideoReader.h"
#include <time.h>

uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;

VideoInfo::~VideoInfo()
{
    if (audio_pkt != NULL)
    {
        av_free_packet(audio_pkt);
        av_packet_free(&audio_pkt);
    }
}

void VideoInfo::ClearVideos()
{
    suic::Mutex::Locker guard(vedioQueue.GetLock());
    AVPacket* packet = NULL;

    while (vedioQueue.PopItem(packet))
    {
        if (NULL != packet)
        {
            av_free_packet(packet);
            av_packet_free(&packet);
        }
    }
}

void VideoInfo::ClearAudios()
{
    suic::Mutex::Locker guard(audioQueue.GetLock());

    AVPacket* packet = NULL;

    while (audioQueue.PopItem(packet))
    {
        if (NULL != packet)
        {
            av_free_packet(packet);
            av_packet_free(&packet);
        }
    }
}

VideoReaderThr::VideoReaderThr(suic::String filename, suic::InvokeProxy* reflesh)
    : _filename(filename)
    , _reflesh(reflesh)
    , _decodeThr(NULL)
{
    
}

VideoReaderThr::~VideoReaderThr()
{
    if (NULL != _decodeThr)
    {
        _decodeThr->unref();
    }
}

void VideoReaderThr::StopPlay()
{
    SDL_PauseAudio(1);

    if (NULL != _decodeThr)
    {
        _decodeThr->Stop();
    }

    Stop();
}

void VideoReaderThr::PausePlay(bool bPause)
{
    if (IsPlaying())
    {
        _videoInfo.isPause = bPause;
        SDL_PauseAudio(bPause ? 1 : 0);
    }
}

bool VideoReaderThr::IsPlaying() const
{
    return !_videoInfo.quit;
}

bool VideoReaderThr::IsPause() const
{
    return _videoInfo.isPause;
}

void VideoReaderThr::SetPlayVolume(int volume)
{
    _videoInfo.volume = volume;
}

void VideoReaderThr::SetPlayProgress(float v)
{
    // 
    // 只有在播放过程中设置才有效
    // 
    if (IsPlaying())
    {
        _videoInfo.seek_pos = _videoInfo.iDuration * v;
        _videoInfo.seek_req = true;
    }
}

void avcodec_get_frame_defaults(AVFrame *frame)
{
#if LIBAVCODEC_VERSION_MAJOR >= 55
     // extended_data should explicitly be freed when needed, this code is unsafe currently
     // also this is not compatible to the <55 ABI/API
    if (frame->extended_data != frame->data && 0)
        av_freep(&frame->extended_data);
#endif

    memset(frame, 0, sizeof(AVFrame));
    av_frame_unref(frame);
}

static int audio_decode_frame(VideoInfo *is, double *pts_ptr)
{
    int decodedDataSize = 0;
    int convertAudioSize = 0;
    int decoded_data_size = 0;
    
    // 解析音频帧是否正确
    int iGotframe = 0;
    int wanted_nb_samples = 0;
    int resampled_data_size = 0;
    int n = 0;

    int64_t dec_channel_layout = 0;

    AVPacket *pkt = is->audio_pkt;
    
    double pts;

    for (;;) 
    {
        // 
        // audio_pkt_size大于0表示已经读取了一帧音频数据，进行处理
        //
        while (is->audio_pkt_size > 0) 
        {

            // 
            // 已经暂停
            // 
            if (is->isPause)
            {
                SDL_Delay(10);
                continue;
            }

            if (!is->audio_frame) 
            {
                if (!(is->audio_frame = av_frame_alloc())) 
                {
                    return AVERROR(ENOMEM);
                }
            } 
            else
            {
                avcodec_get_frame_defaults(is->audio_frame);
            }

            // 
            // 解码音频数据流，返回值表示已经解码的数据流大小
            // 
            decodedDataSize = avcodec_decode_audio4(is->audioStrm->codec, is->audio_frame, &iGotframe, pkt);

            if (decodedDataSize < 0) 
            {
                // 
                // 跳过错误的帧
                // 
                is->audio_pkt_size = 0;
                break;
            }

            // 
            // 调整未解码的音频数据流
            // 
            is->audio_pkt_data += decodedDataSize;
            is->audio_pkt_size -= decodedDataSize;

            if (iGotframe == 0)
            {
                continue;
            }

            // 
            // 计算解码出来的桢需要的缓冲大小
            // 
            decoded_data_size = av_samples_get_buffer_size(NULL, is->audio_frame->channels, 
                is->audio_frame->nb_samples, (AVSampleFormat)is->audio_frame->format, 1);

            if (is->audio_frame->channel_layout == 0)
            {
                dec_channel_layout = av_get_default_channel_layout(is->audio_frame->channels);
            }
            else
            {
                if (is->audio_frame->channels == av_get_channel_layout_nb_channels(is->audio_frame->channel_layout))
                {
                    dec_channel_layout = is->audio_frame->channel_layout;
                }
                else
                {
                    dec_channel_layout = av_get_default_channel_layout(is->audio_frame->channels);
                }
            }

            wanted_nb_samples = is->audio_frame->nb_samples;

            if (is->audio_frame->format != is->audio_src_fmt
                    || dec_channel_layout != is->audio_src_channel_layout
                    || is->audio_frame->sample_rate != is->audio_src_freq
                    || (wanted_nb_samples != is->audio_frame->nb_samples && !is->audioSwrCtx)) 
            {
                if (NULL != is->audioSwrCtx)
                {
                    swr_free(&is->audioSwrCtx);
                }

                is->audioSwrCtx = swr_alloc_set_opts(NULL,
                        is->audio_tgt_channel_layout, (AVSampleFormat)is->audio_tgt_fmt, 
                        is->audio_tgt_freq, dec_channel_layout,
                        (AVSampleFormat)is->audio_frame->format, is->audio_frame->sample_rate, 0, NULL);

                if (!is->audioSwrCtx || swr_init(is->audioSwrCtx) < 0) 
                {
                    break;
                }

                is->audio_src_channel_layout = dec_channel_layout;
                is->audio_src_channels = is->audioStrm->codec->channels;
                is->audio_src_freq = is->audioStrm->codec->sample_rate;
                is->audio_src_fmt = is->audioStrm->codec->sample_fmt;
            }

            // 
            // 这里我们可以对采样数进行调整，增加或者减少，一般可以用来做声画同步
            // 
            if (NULL != is->audioSwrCtx) 
            {
                const uint8_t **in = (const uint8_t **) is->audio_frame->extended_data;
                uint8_t *out[] = { is->audio_buf_temp };

                if (wanted_nb_samples != is->audio_frame->nb_samples) 
                {
                    if (swr_set_compensation(is->audioSwrCtx,
                            (wanted_nb_samples - is->audio_frame->nb_samples)
                             * is->audio_tgt_freq / is->audio_frame->sample_rate, 
                            wanted_nb_samples * is->audio_tgt_freq / is->audio_frame->sample_rate) < 0) 
                    {
                        break;
                    }
                }

                int out_count = sizeof(is->audio_buf_temp) / is->audio_tgt_channels / av_get_bytes_per_sample(is->audio_tgt_fmt);

                // 
                // 把音频数据解码到缓冲区out
                // 
                convertAudioSize = swr_convert(is->audioSwrCtx, out, out_count, in, is->audio_frame->nb_samples);

                if (convertAudioSize < 0) 
                {
                    break;
                }

                if (convertAudioSize == out_count) 
                {
                    swr_init(is->audioSwrCtx);
                }

                is->audio_buf = is->audio_buf_temp;
                resampled_data_size = convertAudioSize * is->audio_tgt_channels * av_get_bytes_per_sample(is->audio_tgt_fmt);
            }
            else 
            {
                resampled_data_size = decoded_data_size;
                is->audio_buf = is->audio_frame->data[0];
            }

            pts = is->audioClock;
            *pts_ptr = pts;
            n = 2 * is->audioStrm->codec->channels;

            is->audioClock += (double) resampled_data_size / (double) (n * is->audioStrm->codec->sample_rate);

            if (is->seek_flag_audio)
            {
                // 
                // 这里需要把毫秒转换到秒
                // 
                int64_t iAudioTime = is->audioClock * 1000;

                // 
                // 发生了跳转 则跳过关键帧到目的时间的这几帧
                // 
                if (iAudioTime < is->seekTime)
                {
                    break;
                }
                else
                {
                    is->seek_flag_audio = 0;
                }
            }

            // We have data, return it and come back for more later
            return resampled_data_size;
        }

        if (is->audio_pkt != NULL)
        {
            av_free_packet(is->audio_pkt);
            av_packet_free(&is->audio_pkt);
            is->audio_pkt = NULL;
        }
        
        if (is->quit)
        {
            return -1;
        }

        if (is->isPause == true)
        {
            SDL_Delay(10);
            continue;
        }

        if (!is->audioQueue.Pop(is->audio_pkt))
        {
            return -1;
        }

        // 
        // 发生了跳转，刷新缓存
        // 
        if (NULL == is->audio_pkt)
        {
            avcodec_flush_buffers(is->audioStrm->codec);
            continue;
        }

        pkt = is->audio_pkt;

        // 
        // 收到这个数据，说明刚刚执行过跳转，现在需要把解码器的数据，清除一下
        // 
        if (pkt->data == 0)
        {
            avcodec_flush_buffers(is->audioStrm->codec);
            av_free_packet(is->audio_pkt);
            is->audio_pkt = NULL;
        }
        else
        {
            is->audio_pkt_data = pkt->data;
            is->audio_pkt_size = pkt->size;

            // 
            // if update, update the audio clock w/pts
            // 
            if (pkt->pts != AV_NOPTS_VALUE) 
            {
                is->audioClock = av_q2d(is->audioStrm->time_base) * pkt->pts;
            }
        }
    }

    return 0;
}

/*
void audio_track_change(Uint8 *pBuffer, int iSize, int AudioChannel, int AudioBits )
{
  int i=0,w_pos=0,r_pos=0, n=0;

  n = iSize / 4; //循环次数

  switch (AudioBits)
  {
  case 8:
    //进行声道选择
    switch(AudioChannel)
    {
    case 1:r_pos=0;
      w_pos=1;
      break;             //左声道

    case 2:r_pos=1;
      w_pos=0;  
      break;             //右声道

    default:return ;           //不处理（双声道模式）

    }
    //执行声道切换操作  
    for(i=0;i<n;i++){  
      memcpy(pBuffer+w_pos,pBuffer+r_pos,1);  
      w_pos+=2;
      r_pos+=2;
    }
    break;
  case 16:
    //进行声道选择
    switch(AudioChannel)
    {
    case 1:r_pos=0;
      w_pos=2;
      break;             //左声道

    case 2:r_pos=2;
      w_pos=0;  
      break;             //右声道

    default:return ;           //不处理（双声道模式）

    }
    //执行声道切换操作  
    for(i=0;i<n;i++){  
      memcpy(pBuffer+w_pos,pBuffer+r_pos,2);  
      w_pos+=4;
      r_pos+=4;
    }
    break;
  case 24:
    //进行声道选择
    switch(AudioChannel)
    {
    case 1:r_pos=0;
      w_pos=3;
      break;             //左声道

    case 2:r_pos=3;
      w_pos=0;  
      break;             //右声道

    default:return;           //不处理（双声道模式）

    }
    //执行声道切换操作  
    for(i=0;i<n;i++){  
      memcpy(pBuffer+w_pos,pBuffer+r_pos,3);  
      w_pos+=6;
      r_pos+=6;
    }
    break;
  case 32:
    //进行声道选择
    switch(AudioChannel)
    {
    case 1:r_pos=0;
      w_pos=4;
      break;             //左声道

    case 2:r_pos=4;
      w_pos=0;  
      break;             //右声道

    default:return ;           //不处理（双声道模式）

    }
    //执行声道切换操作  
    for(i=0;i<n;i++){  
      memcpy(pBuffer+w_pos,pBuffer+r_pos,4);  
      w_pos+=8;
      r_pos+=8;
    }
    break;
  }
  return ;		
}
*/

static void audio_callback(void *userdata, Uint8 *stream, int len) 
{
    VideoInfo *is = (VideoInfo*)userdata;

    // 
    // 当前需要拷贝到SDL缓存的音频数据长度
    // 
    int needCopyDataSize = 0;
    int audio_data_size = 0;

    double pts;

    // 
    // len是由SDL传入的SDL缓冲区的大小，如果这个缓冲未满，我们就一直往里填充数据
    // 
    while (len > 0) 
    {
        // 
        // audio_buf_index指示当前音频缓冲数据的开始位置
        // audio_buf_size指示当前读取的音频缓冲数据的总长度
        // 如果audio_buf_index大于等于audio_buf_size的时候，表示所有的数据都已经拷贝SDL缓冲
        // 需要再次读取音频数据到缓冲
        // 
        if (is->audio_buf_index >= is->audio_buf_size) 
        {

            audio_data_size = audio_decode_frame(is, &pts);

            // 
            // audio_data_size < 0 标示没能解码出数据，我们默认播放静音
            // 
            if (audio_data_size < 0) 
            {                
                // 
                // 静音播放 
                //
                is->audio_buf_size = 1024;

                if (is->audio_buf == NULL) 
                {
                    return;
                }
                
                if (is->audio_buf == &is->audio_buf_temp[0])
                {
                    memset(is->audio_buf, 0, is->audio_buf_size);
                }
            } 
            else 
            {
                is->audio_buf_size = audio_data_size;
            }

            is->audio_buf_index = 0;
        }

        // 
        // 查看stream可用空间，决定一次copy多少数据，剩下的下次继续copy
        // 
        needCopyDataSize = is->audio_buf_size - is->audio_buf_index;

        // 
        // SDL缓冲区长度太小，调整下需要拷贝的数据长度 
        //
        if (needCopyDataSize > len) 
        {
            needCopyDataSize = len;
        }

        if (is->audio_buf == NULL) 
        {
            return;
        }

        SDL_memset(stream, 0, len);

        // 声道控制函数, gTrack 1 - 左声道, 2 - 右声道, 0 - 立体声
        // audio_track_change((uint8_t *)is->audio_buf + is->audio_buf_index, len1, gTrack, 16);    

        // 
        // 把音频数据拷贝到SDL缓冲，准备播放
        // 
        // memcpy(stream, (uint8_t*)is->audio_buf + is->audio_buf_index, needCopyDataSize);
        SDL_MixAudio(stream, (uint8_t*)is->audio_buf + is->audio_buf_index, needCopyDataSize, is->volume); 

        // 
        // 调整下缓冲区位置 
        //
        len -= needCopyDataSize;
        //stream += needCopyDataSize;
        is->audio_buf_index += needCopyDataSize;
    }
}

int VideoReaderThr::InitAudioComponent(VideoInfo *pVI, int streamIndex)
{
    AVCodecContext *codecCtx = pVI->aCodecCtx;
    AVCodec *codec = pVI->aCodec;

    SDL_AudioSpec wanted_spec;
    SDL_AudioSpec spec;
    int64_t wanted_channel_layout = 0;
    int wanted_nb_channels;

    // 
    // SDL支持的声道数为 1, 2, 4, 6
    // 后面我们会使用这个数组来纠正不支持的声道数目
    //
    const int next_nb_channels[] = { 0, 0, 1, 6, 2, 6, 4, 6 };

    if (streamIndex < 0 || streamIndex >= pVI->formatCtx->nb_streams) 
    {
        return -1;
    }

    AVStream *pAudioStrm = pVI->formatCtx->streams[streamIndex];

    wanted_nb_channels = codecCtx->channels;

    if (!wanted_channel_layout 
        || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) 
    {
        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
        wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
    }

    // wanted_spec.channels = codecCtx->channels;
    wanted_spec.channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
    wanted_spec.freq = codecCtx->sample_rate;

    if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) 
    {
        return -1;
    }

    // 具体含义请查看“SDL宏定义”部分
    wanted_spec.format = AUDIO_S16SYS;

    // 
    // 0指示静音
    //
    wanted_spec.silence = 0;            
    wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;  // 自定义SDL缓冲区大小
    wanted_spec.callback = audio_callback;        // 音频解码的关键回调函数
    wanted_spec.userdata = pVI;                    // 传给上面回调函数的外带数据


    while (SDL_OpenAudio(&wanted_spec, &spec) < 0)
    {
        fprintf(stderr, "SDL_OpenAudio(%d channels): %s\n", wanted_spec.channels, SDL_GetError());

        // FFMIN()由ffmpeg定义的宏，返回较小的数
        wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)]; 

        if (!wanted_spec.channels)
        {
            fprintf(stderr, "No more channel to try\n");
            return -1;
        }
        
        wanted_channel_layout = av_get_default_channel_layout(wanted_spec.channels);
    }

    // 
    // 检查实际使用的配置（保存在spec，由SDL_OpenAudio()填充）
    // 
    if (spec.format != AUDIO_S16SYS) 
    {
        fprintf(stderr,"SDL advised audio format %d is not supported!\n",spec.format);
        return -1;
    }

    if (spec.channels != wanted_spec.channels) 
    {
        wanted_channel_layout = av_get_default_channel_layout(spec.channels);
        if (!wanted_channel_layout) 
        {
            fprintf(stderr,"SDL advised channel count %d is not supported!\n",spec.channels);
            return -1;
        }
    }

    pVI->audio_hw_buf_size = spec.size;

    // 
    // 把设置好的参数保存到大结构中
    //
    pVI->audio_src_fmt = pVI->audio_tgt_fmt = AV_SAMPLE_FMT_S16;
    pVI->audio_src_freq = pVI->audio_tgt_freq = spec.freq;
    pVI->audio_src_channel_layout = pVI->audio_tgt_channel_layout = wanted_channel_layout;
    pVI->audio_src_channels = pVI->audio_tgt_channels = spec.channels;
    
    pAudioStrm->discard = AVDISCARD_DEFAULT;
    
    switch (codecCtx->codec_type) 
    {
    case AVMEDIA_TYPE_AUDIO:
        pVI->audioStrm = pVI->formatCtx->streams[streamIndex];
        pVI->audio_buf_size = 0;
        pVI->audio_buf_index = 0;
        SDL_PauseAudio(0);
        // SDL_PauseAudioDevice(pVI->audioID, 0);
        break;

    default:
        break;
    }

    return 0;
}

void VideoReaderThr::DoSeekFrameReq()
{
    if (_videoInfo.seek_req)
    {
        int stream_index = -1;
        int64_t seek_target = _videoInfo.seek_pos;

        if (_videoInfo.videoIndex >= 0)
        {
            stream_index = _videoInfo.videoIndex;
        }
        else if (_videoInfo.audioIndex >= 0)
        {
            stream_index = _videoInfo.audioIndex;
        }

        AVRational aVRational = {1, AV_TIME_BASE};
        
        if (stream_index >= 0)
        {
            // 
            // seek_target单位是毫秒，这里需要转换到FFMpeg时间单位
            // 
            seek_target = av_rescale_q(seek_target, aVRational, _videoInfo.GetStream(stream_index)->time_base);
        }

        // 
        // 定位帧到指定时间
        // 
        if (av_seek_frame(_videoInfo.formatCtx, stream_index, seek_target, AVSEEK_FLAG_BACKWARD) < 0) 
        {
            fprintf(stderr, "%s: error while seeking\n", _videoInfo.formatCtx->filename);
        } 
        else 
        {
            if (_videoInfo.audioIndex >= 0) 
            {
                _videoInfo.ClearVideos();
                _videoInfo.vedioQueue.Add(NULL);
            }

            if (_videoInfo.videoIndex >= 0) 
            {
                _videoInfo.ClearAudios();
                _videoInfo.audioQueue.Add(NULL);
            }
        }

        _videoInfo.seek_req = false;

        // 
        // 转换到秒
        // 
        _videoInfo.seekTime = _videoInfo.seek_pos / 1000000.0;
        _videoInfo.seek_flag_audio = true;
        _videoInfo.seek_flag_video = true;
    }
}

void VideoReaderThr::Run()
{
    suic::Mulstr fname(_filename.c_str());

    AVFormatContext *pFormatCtx = NULL;

    // 
    // 视频相关解码器
    // 
    AVCodecContext *pVCodecCtx = NULL;
    AVCodec *pVCodec = NULL;

    // 
    // 音频相关解码器
    // 
    AVCodecContext *pACodecCtx = NULL;
    AVCodec *pACodec = NULL;

    int audioStream = -1;
    int videoStream = -1;
    int i = 0;

    av_register_all();

    //
    // 分配AVFormatContext.
    // 
    pFormatCtx = avformat_alloc_context();

    // 
    // 打开视频文件
    // 
    if (avformat_open_input(&pFormatCtx, fname.c_str(), NULL, NULL) != 0) {
        printf("can't open the file. \n");
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) 
    {
        printf("Could't find stream infomation.\n");
        return;
    }

    // 获取视频总长度
    if (pFormatCtx->duration != AV_NOPTS_VALUE) 
    {
        _videoInfo.iDuration = pFormatCtx->duration + 5000;
    }

    //
    // 查找音频、视频信息
    // 循环查找视频中包含的流信息，
    // 
    for (i = 0; i < pFormatCtx->nb_streams; i++) 
    {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
        }

        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO  && audioStream < 0)
        {
            audioStream = i;
        }
    }

    _videoInfo.formatCtx = pFormatCtx;

    // 如果videoStream为-1 说明没有找到视频流
    if (videoStream == -1) {
        printf("Didn't find a video stream.\n");
        return;
    }

    // 
    // 没有声音，但是有画面
    // 
    if (audioStream == -1) {
        printf("Didn't find a audio stream.\n");
    }

    _videoInfo.videoIndex = videoStream;
    _videoInfo.audioIndex = audioStream;

    // 
    // 输出视频信息
    // 
    av_dump_format(pFormatCtx, 0, fname.c_str(), 0); 

    // 
    // 查找视频解码器
    // 
    pVCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pVCodec = avcodec_find_decoder(pVCodecCtx->codec_id);

    _videoInfo.vCodecCtx = pVCodecCtx;
    _videoInfo.vCodec = pVCodec;

    if (pVCodec == NULL) 
    {
        printf("PCodec not found.\n");
        return;
    }

    // 
    // 打开视频解码器
    // 
    if (avcodec_open2(pVCodecCtx, pVCodec, NULL) < 0) 
    {
        printf("Could not open video codec.\n");
        return;
    }

    _videoInfo.vrate = _videoInfo.GetVideoStrm()->r_frame_rate;
    if (_videoInfo.vrate.num / _videoInfo.vrate.den > 100) 
    {
        _videoInfo.vrate.num = 21;
        _videoInfo.vrate.den = 1;
    }

    float iRate = _videoInfo.vrate.num / _videoInfo.vrate.den;

    _videoInfo.tickframe = 1000 / iRate;
    _videoInfo.ticksleep = _videoInfo.tickframe;

    if (audioStream != -1)
    {
        //
        // 查找视频解码器
        //
        pACodecCtx = pFormatCtx->streams[audioStream]->codec;
        pACodec = avcodec_find_decoder(pACodecCtx->codec_id);

        _videoInfo.aCodec = pACodec;
        _videoInfo.aCodecCtx = pACodecCtx;

        // 
        // 打开音频解码器
        // 
        if (avcodec_open2(pACodecCtx, pACodec, NULL) < 0) {
            printf("Could not open audio codec.\n");
            return;
        }

        _videoInfo.audioStrm = pFormatCtx->streams[audioStream];

        // 
        // 初始化音频播放环境（SDL）
        // 
        InitAudioComponent(&_videoInfo, audioStream);
    }

    _decodeThr = new VideoDecodeThr(&_videoInfo, _reflesh);
    _decodeThr->ref();
    _decodeThr->Start();

    // 
    // 分配一个packet 用来存放读取的视频
    //
    AVPacket *packet = NULL;
    int ySize = pVCodecCtx->width * pVCodecCtx->height;

    _reflesh->PostInvoker(1, NULL);

    for (;;)
    {
        // 
        // 线程已经退出
        //
        if (IsStopped())
        {
            // 
            // 等待解码线程退出（其使用本线程分配的资源）
            //
            _decodeThr->Stop();
            _decodeThr->Join();
            break;
        }

        DoSeekFrameReq();

        if (_videoInfo.isPause)
        {
            Sleep(100);
            continue;
        }

        if (_videoInfo.vedioQueue.IsFull() ||
            _videoInfo.audioQueue.IsFull())
        {
            printf("过多数据，延时\n");
            SDL_Delay(100);
            continue;
        }

        //
        // 分配一个packet对象，用来存储数据
        //
        packet = av_packet_alloc();

        // 
        // 读取一帧视频流，数据放到packet对象中
        //
        if (av_read_frame(pFormatCtx, packet) < 0)
        {
            // 
            // 释放刚刚分配的内存
            // 
            av_packet_free(&packet);

            if ((pFormatCtx->pb->error) == 0)
            {
                SDL_Delay(100); //不是出错，可能是读完了
                //
                // 读取结束，设置标志
                //
                _videoInfo.readFinished = true;

                // 等解码线程结束
                _decodeThr->Join();
                break;
            }
            else
            {
                // 出错了，继续读
                continue; 		
            }
        }

        // 
        // 这是视频流，放入视频流解码队列，准备显示
        // 
        if (packet->stream_index == videoStream)
        {
            _videoInfo.vedioQueue.Add(packet);
        }
        else if( packet->stream_index == audioStream)
        {
            // 
            // 音频数据包压入队列
            // 
            _videoInfo.audioQueue.Add(packet);
        }
        else
        {
            // 
            // 没有对应的播放类型，释放包
            // 
            av_free_packet(packet);
            av_packet_free(&packet);
        }
    }

    // 
    // 释放资源，准备退出
    // 
    avcodec_close(pACodecCtx);
    avcodec_close(pVCodecCtx);
    avformat_close_input(&pFormatCtx);
    avformat_free_context(pFormatCtx);

    SDL_CloseAudio();

    _reflesh->PostInvoker(0, NULL);
}

VideoDecodeThr::VideoDecodeThr(VideoInfo* vInfo, suic::InvokeProxy* reflesh)
    : _videoInfo(vInfo)
    , _reflesh(reflesh)
{
}

VideoDecodeThr::~VideoDecodeThr()
{
}

double VideoDecodeThr::SynchronizeVideo(AVFrame *srcFrame, double pts)
{
    double frameDelay;

    if (pts != 0) 
    {
        _videoInfo->videoClock = pts;
    } 
    else 
    {
        pts = _videoInfo->videoClock;
    }

    // 
    // 更新时钟
    // 
    frameDelay = av_q2d(_videoInfo->GetVideoStrm()->codec->time_base);
    frameDelay += srcFrame->repeat_pict * (frameDelay * 0.5);

    _videoInfo->videoClock += frameDelay;

    return pts;
}

unsigned long long GetCurrentTimeMsec()  
{  
#ifdef __OS_WIN32  
    struct timeval tv;  
    time_t clock;  
    struct tm tm;  
    SYSTEMTIME wtm;  

    GetLocalTime(&wtm);  
    tm.tm_year = wtm.wYear - 1900;  
    tm.tm_mon = wtm.wMonth - 1;  
    tm.tm_mday = wtm.wDay;  
    tm.tm_hour = wtm.wHour;  
    tm.tm_min = wtm.wMinute;  
    tm.tm_sec = wtm.wSecond;  
    tm.tm_isdst = -1;  
    clock = mktime(&tm);  
    tv.tv_sec = clock;  
    tv.tv_usec = wtm.wMilliseconds * 1000;  
    return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);  
#else  
    struct timeval tv;  
    gettimeofday(&tv,NULL);  
    return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);  
#endif  
}

void VideoDecodeThr::Run()
{
    // 解码视频相关
    AVFrame *pFrame = NULL;
    AVFrame *pFrameRGB = NULL;

    // 解码后的rgb数据
    uint8_t *pRgbBuffer;

    AVPacket *pPacket = NULL;

    // 用于解码后的视频格式转换
    struct SwsContext *pImgConvertCtx;

    int iRet = 0;
    int iGotPicture = 0;
    int iNumBytes = 0;

    double videoPts = 0; // 当前视频的pts
    double audioPts = 0; // 音频pts

    // av_register_all();

    // 视频解码器
    AVCodecContext *pCodecCtx = _videoInfo->vCodecCtx;

    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    // 这里改成将解码后的YUV数据转换成RGB32
    pImgConvertCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
            pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
            AV_PIX_FMT_BGRA, SWS_BICUBIC, NULL, NULL, NULL);

    iNumBytes = avpicture_get_size(AV_PIX_FMT_BGRA, pCodecCtx->width, pCodecCtx->height);

    pRgbBuffer = (uint8_t *) av_malloc(iNumBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, pRgbBuffer, AV_PIX_FMT_BGRA,
            pCodecCtx->width, pCodecCtx->height);

    static int index = 0;
    unsigned long long startTime = GetCurrentTimeMsec();

    //_playThread = new VideoPlayThread(_videoInfo, _reflesh);
    //_playThread->Start();

    for (;;)
    {
        // 线程已经退出
        if (IsStopped())
        {
            //_playThread->Stop();
            break;
        }

        if (_videoInfo->isPause)
        {
            Sleep(100);
            continue;
        }

        pPacket = NULL; 

        if (!_videoInfo->vedioQueue.Pop(pPacket))
        {
            if (_videoInfo->readFinished)
            {
                // 
                // 队列里面没有数据且读取完毕了
                //
                _videoInfo->quit = true;
                break;
            }
            else
            {
                // 
                // 队列只是暂时没有数据而已
                //
                Sleep(5);
                continue;
            }
        }

        if (NULL == pPacket)
        {
            _videoInfo->ticklast = 0;
            avcodec_flush_buffers(_videoInfo->GetVideoStrm()->codec);
            continue;
        }

        startTime = GetCurrentTimeMsec();

        //
        // 解码视频流数据
        // 
        iRet = avcodec_decode_video2(pCodecCtx, pFrame, &iGotPicture, pPacket);

        if (iRet < 0) 
        {
            suic::Debug::Trace(suic::String("decode error.\n").c_str());
            av_free_packet(pPacket);
            av_packet_free(&pPacket);
            continue;
        }

        int64_t aPts = _videoInfo->audioClock * 1000;
        int64_t vPts = av_rescale_q(av_frame_get_best_effort_timestamp(pFrame), 
            _videoInfo->GetVideoStrm()->time_base, TIMEBASE_MS);
        DWORD tickcur = GetTickCount();
        int tickdiff = tickcur - _videoInfo->ticklast;
        int64_t avDiff = aPts - vPts - _videoInfo->tickavdiff;

        _videoInfo->ticklast = tickcur;

        if (_videoInfo->seek_flag_video)
        {
            // 
            // 发生了跳转 则跳过关键帧到目的时间的这几帧
            //
            if (vPts < _videoInfo->seekTime)
            {
                av_free_packet(pPacket);
                av_packet_free(&pPacket);
                continue;
            }
            else
            {
                _videoInfo->seek_flag_video = false;
            }
        }

        if (tickdiff - _videoInfo->tickframe >  2) 
        {
            _videoInfo->ticksleep--;
        }

        if (tickdiff - _videoInfo->tickframe < -2) 
        {
            _videoInfo->ticksleep++;
        }

        if (aPts > 0 && vPts > 0) 
        {
            if (avDiff > 5) 
            {
                _videoInfo->ticksleep -= 2;
            }

            if (avDiff < -5) 
            {
                _videoInfo->ticksleep += 2;
            }
        }

        if (_videoInfo->ticksleep < 0) 
        {
            _videoInfo->ticksleep = 0;
        }
        
        if (_videoInfo->ticksleep > 0) 
        {
            Sleep(_videoInfo->ticksleep);
        }

        if (iGotPicture != 0)
        {
            // 
            // 转换图像格式
            //
            sws_scale(pImgConvertCtx, (uint8_t const * const *) pFrame->data, 
                pFrame->linesize, 0, pCodecCtx->height, 
                pFrameRGB->data, pFrameRGB->linesize);

            // 
            // 扔给界面进行渲染
            //
            BmpInfo* bmp = new BmpInfo();
            bmp->ref();
            bmp->bmp.Create(pCodecCtx->width, pCodecCtx->height, pRgbBuffer, 32);
            //bmp->bmp.SetConfig(pCodecCtx->width, pCodecCtx->height, 32);
            //bmp->bmp.SetPixels(pRgbBuffer);
            bmp->curDuration = vPts;

            // 
            // 转换到毫秒
            // 
            bmp->duration = _videoInfo->iDuration / 1000;

            _reflesh->PostInvoker(0, bmp);

            /*PlayInfo* pInfo = new PlayInfo();
            pInfo->bmpInfo = bmp;
            pInfo->videoPts = videoPts;
            pInfo->repeat_pict = pFrame->repeat_pict;

            _playThread->AddPlayInfo(pInfo);*/
        }

        av_free_packet(pPacket);
        av_packet_free(&pPacket);
    }

    //
    // 释放相关资源，结束，退出
    // 
    av_free(pFrame);
    av_free(pFrameRGB);
    av_free(pRgbBuffer);
    sws_freeContext(pImgConvertCtx);

    // 
    // 解码线程退出，设置标志，通知解析线程退出
    // 
    _videoInfo->decodeFinished = true;

    //_playThread->Join();
}
