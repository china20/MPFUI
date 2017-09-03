// 华勤科技版权所有 2010-2021
// 
// 文件名：VideoPlay.h
// 功  能：主界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2017-01-02
// 
// ============================================================================

#ifndef _VIDEOPLAY_H_
#define _VIDEOPLAY_H_

class VideoPlay : public suic::Thread
{
public:

    VideoPlay();
    ~VideoPlay();

    void Run();
};

#endif
