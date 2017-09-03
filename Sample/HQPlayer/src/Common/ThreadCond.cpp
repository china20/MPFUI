
#include "stdafx.h"
#include "ThreadCond.h"

namespace core {

#ifdef __OS_WIN32

Semaphore::Semaphore(long initial)
{
    _sem = CreateSemaphore(0, initial, 0x7fffffff, 0);

    if (_sem == INVALID_HANDLE_VALUE)
    {
        //throw core::SyscallException(__FILE__, __LINE__, GetLastError());
    }
}

Semaphore::~Semaphore()
{
    CloseHandle(_sem);
}

void Semaphore::wait() const
{
    DWORD rc = WaitForSingleObject(_sem, INFINITE);

    if (rc != WAIT_OBJECT_0)
    {
        //throw core::SyscallException(__FILE__, __LINE__, GetLastError());
    }
}

bool Semaphore::timedWait(suic::Int64 timeout) const
{
    suic::Int64 msTimeout = timeout;

    if (msTimeout < 0 || msTimeout > 0x7FFFFFFF)
    {
        //throw core::InvalidTimeoutException(__FILE__, __LINE__, timeout);
    } 

    DWORD rc = WaitForSingleObject(_sem, static_cast<DWORD>(msTimeout));

    if (rc != WAIT_TIMEOUT && rc != WAIT_OBJECT_0)
    {
        return false;
    }
    return rc != WAIT_TIMEOUT;
}

void Semaphore::post(int count) const
{
    int rc = ReleaseSemaphore(_sem, count, 0);

    if (rc == 0)
    {
        //throw core::SyscallException(__FILE__, __LINE__, GetLastError());
    }
}

#endif

/////////////////////////////////////////////////////////////////////////////
// ThreadCond

ThreadCond::ThreadCond()
    : m_gate(1)
    , m_blocked(0)
    , m_unblocked(0)
    , m_state(core::ThreadCond::StateIdle)
{
}

ThreadCond::~ThreadCond()
{
}

void ThreadCond::Notify()
{
    wake(false);
}

void ThreadCond::NotifyAll()
{
    wake(true);
}

#ifdef __OS_WIN32

void ThreadCond::wake(bool broadcast)
{
    //
    // Lock gate. The gate will be locked if there are threads waiting
    // to drain from postWait.
    //
    m_gate.wait();

    //
    // Lock the internal mutex.
    //
    suic::Mutex::Locker sync(m_internal);

    //
    // Adjust the count of the number of waiting/blocked threads.
    //
    if (m_unblocked != 0)
    {
        m_blocked -= m_unblocked;
        m_unblocked = 0;
    }

    //
    // If there are waiting threads then we enter a signal or
    // broadcast state.
    //
    if (m_blocked > 0)
    {
        //
        // Unblock some number of waiters. We use -1 for the signal
        // case.
        //
        assert(m_state == StateIdle);
        m_state = (broadcast) ? StateBroadcast : StateSignal;
        //
        // Posting the queue wakes a single waiting thread. After this
        // occurs the waiting thread will wake and then either post on
        // the m_queue to wake the next waiting thread, or post on the
        // gate to permit more signaling to proceed.
        //
        // Release before posting to avoid potential immediate
        // context switch due to the mutex being locked.
        //
        sync.Release();
        m_queue.post();
    }
    else
    {
        //
        // Otherwise no blocked waiters, release the gate.
        //
        // Release before posting to avoid potential immediate
        // context switch due to the mutex being locked.
        //
        sync.Release();
        m_gate.post();
    }
}

void ThreadCond::preWait() const
{
    //
    // m_gate is used to protect m_blocked. Furthermore, this prevents
    // further threads from entering the wait state while a
    // signal/broadcast is being processed.
    //
    m_gate.wait();
    m_blocked++;
    m_gate.post();
}

void ThreadCond::postWait(bool timedOutOrFailed) const
{
    suic::Mutex::Locker sync(m_internal);

    //
    // One more thread has unblocked.
    //
    m_unblocked++;

    //
    // If m_state is StateIdle then this must be a timeout, otherwise its a
    // spurious wakeup which is incorrect.
    //
    if (m_state == StateIdle)
    {
        assert(timedOutOrFailed);
        return;
    }

    if (timedOutOrFailed)
    {
        //
        // If the thread was the last blocked thread and there's a
        // pending signal/broadcast, reset the signal/broadcast to
        // prevent spurious wakeup.
        //
        if (m_blocked == m_unblocked)
        {
            m_state = StateIdle;
            //
            // Consume the queue post to prevent spurious wakeup. Note
            // that although the internal mutex could be released
            // prior to this wait() call, doing so gains nothing since
            // this wait() MUST return immediately (if it does not
            // there is a major bug and the entire application will
            // deadlock).
            //
            m_queue.wait();
            //
            // Release before posting to avoid potential immediate
            // context switch due to the mutex being locked.
            //
            sync.Release();
            m_gate.post();
        }
    }
    else
    {
        //
        // At this point, the thread must have been woken up because
        // of a signal/broadcast.
        //
        if (m_state == StateSignal || m_blocked == m_unblocked) // Signal or no more blocked threads
        {
            m_state = StateIdle;
            // Release before posting to avoid potential immediate
            // context switch due to the mutex being locked.
            sync.Release();
            m_gate.post();
        }
        else // Broadcast and more blocked threads to wake up.
        {
            // Release before posting to avoid potential immediate
            // context switch due to the mutex being locked.
            sync.Release();
            m_queue.post();
        }
    }
}

void ThreadCond::dowait() const
{
    try
    {
        m_queue.wait();
        postWait(false);
    }
    catch(...)
    {
        postWait(true);
        throw;
    }
}

bool ThreadCond::timedDowait(suic::Int64 timeout) const
{
    try
    {
        bool rc = m_queue.timedWait(timeout);
        postWait(!rc);
        return rc;
    }
    catch(...)
    {
        postWait(true);
        throw;
    }
}

#endif

}
