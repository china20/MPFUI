/////////////////////////////////////////////////////////////////////////
// uiassignerinvoker.cpp

#include <System/Windows/AssignerInvoker.h>
#include <System/Windows/Assigner.h>

namespace suic
{

AssignerInvoker::AssignerInvoker()
{
    _signal = (Handle)(DWORD_PTR)::CreateEvent(NULL, FALSE, FALSE, NULL);
    _priority = eBackgound;
}

AssignerInvoker::~AssignerInvoker()
{
    CloseHandle((HANDLE)(DWORD_PTR)_signal);
}

Assigner* AssignerInvoker::GetAssigner()
{
    return Assigner::Current();
}

InvokerPriority AssignerInvoker::GetPriority()
{
    return _priority;
}

void AssignerInvoker::SetPriority(InvokerPriority value)
{
    _priority = value;
}

AssignerInvokerStatus AssignerInvoker::GetStatus()
{
    return _status;
}

bool AssignerInvoker::Abort()
{
    return false;
}

AssignerInvokerStatus AssignerInvoker::Wait()
{
    return Wait(-1);
}

AssignerInvokerStatus AssignerInvoker::Wait(Int32 timeout)
{
    if (::WaitForSingleObject((HANDLE)(DWORD_PTR)_signal, timeout) == WAIT_TIMEOUT)
    {
        ;
    }
    return _status;
}

void AssignerInvoker::OnComplete()
{
    ::SetEvent((HANDLE)(DWORD_PTR)_signal);
}

}
