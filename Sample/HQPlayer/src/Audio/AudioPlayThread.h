// 华勤科技版权所有 2010-2021
// 
// 文件名：AudioPlayThread.h
// 功  能：主界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2017-01-02
// 
// ============================================================================

#ifndef _AUDIOPLAYTHREAD_H_
#define _AUDIOPLAYTHREAD_H_

class AudioPlayThread : public suic::Thread
{
public:

    AudioPlayThread();
    ~AudioPlayThread();

    void Run();
};

#endif
