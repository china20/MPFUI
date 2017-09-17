// 华勤科技版权所有 2008-2022
// 
// 文件名：TimerThread.h
// 功  能：线程定时器
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UITIMERTHREAD_H_
#define _UITIMERTHREAD_H_

#include <System/Tools/Thread.h>
#include <System/Types/Interface.h>
#include <System/Windows/TimerBase.h>
#include <System/Windows/AssignerInvoker.h>

namespace suic
{

class TimerInvoker;

class TimerEntry : public TimerBase
{
public:

    TimerEntry();
    TimerEntry(Uint32 interval);
    TimerEntry(Uint32 interval, EventHandler cb);
    TimerEntry(Uint32 interval, InvokerPriority priority);
    TimerEntry(Uint32 interval, InvokerPriority priority, EventHandler cb);

    ~TimerEntry();

    void Start();
    void Stop();

    void SetAssigner(suic::Assigner* assigner);

private:

    Object* FireTick(bool unused);
    void Promote();
    void Restart();

    void Initialize(Assigner* assigner, InvokerPriority priority, Uint32 interval);

    Assigner* _assigner;
    TimerInvoker* _timerInvoker;

    friend class TimerThread;
    friend class TimerInvoker;
};

class TimerInvoker : public AssignerInvoker
{
public:

    TimerInvoker();
    ~TimerInvoker();

    void Invoke();

private:

    WeakRef _timerEntry;
    friend TimerThread;
};

class TimerThread : public Thread
{
public:

    static TimerThread* Ins();

    void Close();

private:

    void Run();

    void FinishTimerThread();
    void PromoteTimers(Uint32 currTimeInTicks);
    void SetAndCheckInterval(int iTicks);
    void UpdateDispachterTimer(bool unUsed);

    void AddTimer(TimerEntry* timer);
    void RemoveTimer(TimerEntry* timer);

    TimerThread();

    friend class TimerEntry;

private:

    bool _bExit;
    Mutex insLock;
    Uint32 _timersVersion;
    Uint32 _dueTimeInTicks;
    int _interval;
    bool _isTimerSet;
    bool _dueTimeFound;
    Array<TimerEntry*, false> _timers;
};

}

#endif
