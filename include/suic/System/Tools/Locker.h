// 华勤科技版权所有 2008-2022
// 
// 文件名：UIuilocker.h>
// 功  能：定义核心库的线程临界对象。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ========================================================

#ifndef _UILOCKER_H_
#define _UILOCKER_H_

#include <System/Windows/Object.h>
#include <System/Tools/Guard.h>

#define __OS_WIN32

namespace suic
{

class SUICORE_API Mutex
{
public:

    typedef UIGuard<Mutex> Locker;

    Mutex();
    virtual ~Mutex();

#ifdef __OS_WIN32
    struct LockState
    {
    };
#else
    struct LockState
    {
        pthread_mutex_t* mutex;
    };
#endif

    void lock() const;
	void unlock() const;

	void lock(LockState&) const;
	void unlock(LockState&) const;

private:

#ifdef __OS_WIN32
	mutable CRITICAL_SECTION m_mutex;
#else
    mutable pthread_mutex_t  m_mutex;
#endif

	Mutex(const Mutex&){};
	void operator=(const Mutex&){};
};

};

#endif
