

#include <System/Tools/Signal.h>

namespace suic 
{

#ifndef __OS_UNIX

UISignal::UISignal(bool bSignal/*=true*/)
{
    // create a event
    _signal = (Handle)(DWORD_PTR)::CreateEvent(NULL, FALSE, bSignal ? TRUE : FALSE, NULL);
}

UISignal::UISignal(bool bMenuReset,bool bSignal)
{
    // create a event
    _signal = (Handle)(DWORD_PTR)::CreateEvent(NULL, bMenuReset ? TRUE : FALSE
        , bSignal ? TRUE : FALSE, NULL);
}

UISignal::~UISignal()
{
    // close event handle
    ::CloseHandle((HANDLE)(DWORD_PTR)_signal);
}

UISignal::operator Handle() const
{
    return _signal;
}

bool UISignal::Wait(int iTimeout/*=INFINITE*/)
{
    // wait event for finishing, if timeout, return false
    if (::WaitForSingleObject((HANDLE)(DWORD_PTR)_signal, iTimeout) == WAIT_TIMEOUT)
    {
        return false;
    }

    return true;
}

int UISignal::Wait(UISignal& _Other, int iTimeout/*=INFINITE*/)
{
    HANDLE sigs[2] = {(HANDLE)(DWORD_PTR)_signal, (HANDLE)(DWORD_PTR)_Other._signal};

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

void UISignal::Notify()
{
    // send a signal to notify outer
    ::SetEvent((HANDLE)(DWORD_PTR)_signal);
}

void UISignal::Reset()
{
    // send a signal to notify outer
    ::ResetEvent((HANDLE)(DWORD_PTR)_signal);
}

#endif


}