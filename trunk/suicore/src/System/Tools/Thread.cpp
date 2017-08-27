/////////////////////////////////////////////////////////////////////////////////
// ThreadControl

#include <System/Tools/Thread.h>
#include <time.h>

namespace suic {

#ifdef __OS_WIN32

Thread::Thread() :
    _started(false),
    _running(false),
    _handle(0),
    _id(0)
{
}

Thread::~Thread()
{
}

unsigned int WINAPI Thread::StartHook(void* arg)
{
    Thread* thread = NULL;

    try
    {
        thread = static_cast<Thread*>(arg);
        thread->Run();
    }
    catch (...)
    {
        //terminate();
    }

    thread->_Done();
    thread->_Clear();
    
    thread->unref();

    return 0;
}

#include <process.h>

bool Thread::Start(Int32 stackSize, int iPriority)
{
    ThreadPtr keepMe = this;

    Mutex::Locker sync(_mutex);

    if (!_started)
    {
        ref();
        unsigned int Id = 0;
        _started = true;

        _handle = 
            reinterpret_cast<HANDLE>(
            _beginthreadex(0, 
            static_cast<unsigned int>(stackSize), 
            StartHook, this, 0, &Id));
        _id = Id;

        if (_handle == 0)
        {
            _started = false;
            unref();
        }
        else
        {
            _running = true;
            ::SetThreadPriority(_handle, THREAD_PRIORITY_ABOVE_NORMAL);
        }        
    }

    return _running;
}

bool Thread::operator==(const Thread& rhs) const
{
    return this == &rhs;
}

bool Thread::operator!=(const Thread& rhs) const
{
    return this != &rhs;
}

bool Thread::operator<(const Thread& rhs) const
{
    return this < &rhs;
}

bool Thread::IsAlive() const
{
    Mutex::Locker sync(const_cast<Mutex&>(_mutex));
    return _running;
}

void Thread::_Done()
{
    Mutex::Locker sync(_mutex);
    _running = false;
}

bool Thread::IsStopped() const
{
    return !_started;
}

void Thread::Stop()
{
    _started = false;
}

void Thread::_Clear()
{
    Stop();
    if (_handle != 0)
    {
        CloseHandle(_handle);
    }
    _handle = 0;
}

void Thread::Join()
{
    DWORD retval = WaitForSingleObject(_handle, INFINITE);

    if (retval != WAIT_OBJECT_0)
    {
        //throw ThreadSyscallException(__FILE__, __LINE__, GetLastError());
    }

    _Clear();
}

void Thread::Sleep(const Int32& timeout)
{
    ::Sleep(static_cast<long>(timeout));
}

void Thread::YieldThr()
{
    //
    // A value of zero causes the thread to relinquish the remainder
    // of its time slice to any other thread of equal priority that is
    // ready to run.
    //
    ::Sleep(0);
}

Uint32 Thread::CurrentThreadId()
{
    return (Uint32)GetCurrentThreadId();
}

#else

Thread::Thread() :
    _started(false),
    _running(false)
{
}

Thread::~Thread()
{
}

extern "C" 
{
static void* StartHook(void* arg)
{
    //
    // Ensure that the thread doesn't go away until run() has
    // completed.
    //
    ThreadPtr thread;

    try
    {
        Thread* rawThread = static_cast<Thread*>(arg);

        thread = rawThread;

        //
        // See the comment in Thread::Start() for details.
        //
        rawThread->unref();
        thread->run();
    }
    catch (...)
    {
    }

    thread->_Done();
    
    return 0;
}
}

ThreadControl Thread::Start(size_t stackSize)
{
    //
    // Keep this alive for the duration of Start
    //
    ThreadPtr keepMe = this;
    Mutex::Locker sync(_mutex);

    if (_started)
    {
        //throw ThreadStartedException(__FILE__, __LINE__);
    }
    __incSt();

    if (stackSize > 0)
    {
        pthread_attr_t attr;
        int rc = pthread_attr_init(&attr);

        if (rc != 0)
        {
            unref();
            //throw ThreadSyscallException(__FILE__, __LINE__, rc);
        }
        if (stackSize < PTHREAD_STACK_MIN)
        {
            stackSize = PTHREAD_STACK_MIN;
        }
#ifdef __APPLE__
        if (stackSize % 4096 > 0)
        {
            stackSize = stackSize / 4096 * 4096 + 4096;
        }
#endif
        rc = pthread_attr_setstacksize(&attr, stackSize);
        if (rc != 0)
        {
            unref();
            throw ThreadSyscallException(__FILE__, __LINE__, rc);
        }
        rc = pthread_create(&_thread, &attr, StartHook, this);
        if (rc != 0)
        {
            unref();
            throw ThreadSyscallException(__FILE__, __LINE__, rc);
        }
    }
    else
    {
        int rc = pthread_create(&_thread, 0, StartHook, this);
        if (rc != 0)
        {
            unref();
            throw ThreadSyscallException(__FILE__, __LINE__, rc);
        }
    }

    _started = true;
    _running = true;

    return ThreadControl(_thread);
}

ThreadControl Thread::GetThreadControl() const
{
    Mutex::Locker sync(_mutex);
    if (!_started)
    {
        throw ThreadNotStartedException(__FILE__, __LINE__);
    }
    return ThreadControl(_thread);
}

bool Thread::operator==(const Thread& rhs) const
{
    return this == &rhs;
}

bool Thread::operator!=(const Thread& rhs) const
{
    return this != &rhs;
}

bool Thread::operator<(const Thread& rhs) const
{
    return this < &rhs;
}

bool Thread::IsAlive() const
{
    Mutex::Locker sync(_mutex);
    return _running;
}

void Thread::_Done()
{
    Mutex::Locker sync(_mutex);
    _running = false;
}

void Thread::clear()
{
}

void Thread::join()
{
    if (!_detachable)
    {
        //throw BadThreadControlException(__FILE__, __LINE__);
    }

    void* ignore = 0;
    int rc = pthread_join(_thread, &ignore);
    if (rc != 0)
    {
        //throw ThreadSyscallException(__FILE__, __LINE__, rc);
    }
}

void Thread::sleep(const xp::Time&)
{
    struct timeval tv = timeout;
    struct timespec ts;
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000L;
    nanosleep(&ts, 0);
}

void Thread::yield()
{
    sched_yield();
}

#endif


}
