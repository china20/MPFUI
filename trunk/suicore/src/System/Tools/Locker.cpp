
#include <System/Tools/Locker.h>

namespace suic 
{

#ifdef __OS_WIN32

Mutex::Mutex()
{
    ::InitializeCriticalSection(&m_mutex);
}

Mutex::~Mutex()
{
    ::DeleteCriticalSection(&m_mutex);
}

void Mutex::lock() const
{
    ::EnterCriticalSection(&m_mutex);
}

void Mutex::unlock() const
{
    ::LeaveCriticalSection(&m_mutex);
}

void Mutex::lock(LockState&) const
{
    ::EnterCriticalSection(&m_mutex);
}

void Mutex::unlock(LockState&) const
{
    ::LeaveCriticalSection(&m_mutex);
}

#else
Mutex::Mutex()
{
    ::pthread_mutex_init(&m_mutex, NULL);
}

Mutex::~Mutex()
{
    ::pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock() const
{
    ::pthread_mutex_lock(&m_mutex);
}

void Mutex::unlock() const
{
    ::pthread_mutex_unlock(&m_mutex);
}
#endif


}