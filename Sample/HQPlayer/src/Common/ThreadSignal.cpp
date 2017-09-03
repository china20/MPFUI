
#include "stdafx.h"
#include "ThreadSignal.h"

namespace core {

Signal::Signal(bool bSignal/*=true*/)
{
    // create a event
    signal_cond = ::CreateEvent(NULL, FALSE, bSignal ? TRUE : FALSE, NULL);
}

Signal::Signal(bool bMenuReset,bool bSignal)
{
    // create a event
    signal_cond = ::CreateEvent(NULL, bMenuReset ? TRUE : FALSE, bSignal ? TRUE : FALSE, NULL);
}

Signal::~Signal()
{
    // close event handle
    ::CloseHandle(signal_cond);
}

Signal::operator HANDLE() const
{
    return signal_cond;
}

bool Signal::Wait(int iTimeout/*=INFINITE*/)
{
    // wait event for finishing, if timeout, return false
    if (::WaitForSingleObject(signal_cond, iTimeout) == WAIT_TIMEOUT)
    {
        return false;
    }

    return true;
}

int Signal::Wait(core::Signal& _Other, int iTimeout/*=INFINITE*/)
{
    HANDLE sigs[2] = {signal_cond, _Other.signal_cond};

    DWORD dwRet = ::WaitForMultipleObjects(2, sigs, FALSE, (DWORD)iTimeout);

    if (dwRet == WAIT_OBJECT_0)
    {
        return 1;
    }
    else if (dwRet == WAIT_OBJECT_0 + 1)
    {
        return 2;
    }
    else if (WAIT_TIMEOUT == dwRet)
    {
        return 3;
    }
    else
    {
        return -1;
    }
}

void Signal::Notify()
{
    // send a signal to notify outer
    ::SetEvent(signal_cond);
}

void Signal::Reset()
{
    // send a signal to notify outer
    ::ResetEvent(signal_cond);
}

}