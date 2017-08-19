// ���ڿƼ���Ȩ���� 2010-2013
// 
// �ļ�����uithread.h
// ��  �ܣ���װ�̶߳����Զ�������������������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-12-12
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
    // ���һ���߳��Ƿ���������
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
