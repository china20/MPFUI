// 华勤科技版权所有 2010-2013
// 
// 文件名：uithread.h
// 功  能：封装线程对象，自动完成启动，挂起操作。
// 
// 作  者：MPF开发组
// 时  间：2010-12-12
// 
// ============================================================

#ifndef _UITHREAD_H_
#define _UITHREAD_H_

#include <System/Windows/DpObject.h>
#include <System/Tools/Locker.h>

namespace suic {

class SUICORE_API Thread : public Object
{
public:

    Thread();
    virtual ~Thread();

    virtual void Run() = 0;

    bool Start(Int32 = 0, int iPriority=0);

    bool operator==(const Thread&) const;
    bool operator!=(const Thread&) const;
    bool operator<(const Thread&) const;

    //
    // 检测一个线程是否在运行中
    //
    bool IsAlive() const;
    bool IsStopped() const;

    void Stop();
    void Join();

    static void Sleep(const Int32&);
    static void YieldThr();
    static Uint32 CurrentThreadId();

protected:

    void _Done();
    void _Clear();

    static unsigned int WINAPI StartHook(void* arg);

protected:

    Mutex _mutex;
    bool _started;
    bool _running;

#ifdef __OS_WIN32
    HANDLE _handle;
    DWORD  _id;
#else
    pthread_t _thread;
#endif

private:

    Thread(const Thread&);
    void operator=(const Thread&); 
};

typedef shared<Thread> ThreadPtr;

}

#endif
