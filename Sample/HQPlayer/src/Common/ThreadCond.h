// 华勤科技版权所有 2010-2021
// 
// 文件名：ThreadCond.h
// 功  能：线程安全队列。
// 
// 作  者：MPF开发组
// 时  间：2017-01-02
// 
// ============================================================================

#ifndef _THREADCONDITION_H
#define _THREADCONDITION_H

namespace core
{

#ifdef __OS_WIN32

// 实现windows下的信号量
class Semaphore
{
public:

    Semaphore(long = 0);
    ~Semaphore();

    void wait() const;
    bool timedWait(suic::Int64) const;
    void post(int = 1) const;

private:

    mutable HANDLE _sem;
};

#endif

class NullCond
{
	NullCond(){};
	~NullCond(){};
	
    bool Wait(int iTimeout)
    {
        return true;
    }

	void Notify() {};
	void NotifyAll() {};
};

class ThreadCond
{
public:

	ThreadCond();
	~ThreadCond();

    void Notify();
    void NotifyAll();

#ifdef __OS_WIN32

    template <typename M> void
    Wait(const M& mutex) const
    {
        preWait();

        typename M::LockState state;

        mutex.unlock(state);

        try
        {
            dowait();
            mutex.lock(state);
        }
        catch(...)
        {
            mutex.lock(state);
            throw;
        }
    }

    template <typename M> bool
    TimedWait(const M& mutex, suic::Int64 timeout) const
    {
        preWait();

        typename M::LockState state;

        mutex.unlock(state);

        try
        {
            bool rc = timedDowait(timeout);
            mutex.lock(state);
            return rc;
        }
        catch(...)
        {
            mutex.lock(state);
            throw;
        }
    }
    
    //////////////////////////////////////////////////////////////////////////

    void wake(bool);
    void preWait() const;
    void postWait(bool) const;
    bool timedDowait(suic::Int64) const;
    void dowait() const;

protected:

    suic::Mutex m_internal;
    
    mutable long m_blocked;
    mutable long m_unblocked;

    core::Semaphore m_gate;
    core::Semaphore m_queue;

    enum State
    {
        StateIdle,
        StateSignal,
        StateBroadcast
    };
    mutable State m_state;
#else
    mutable pthread_cond_t m_cond;
#endif

};

}

#endif
