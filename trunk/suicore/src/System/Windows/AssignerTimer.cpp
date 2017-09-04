/////////////////////////////////////////////////////////////////////////
// assigner.cpp

#include <System/Windows/Assigner.h>
#include <System/Windows/AssignerTimer.h>

namespace suic
{

TimerBase::TimerBase()
{
    Initialize(InvokerPriority::eBackgound, 20);
}

TimerBase::~TimerBase()
{

}

void TimerBase::SetTick(EventHandler cb)
{
    Tick = cb;
}

void TimerBase::ClearTick()
{
    Tick.Clear();
}

void TimerBase::SetInterval(Uint32 interval)
{
    _interval = interval;
}

void TimerBase::SetPriority(InvokerPriority priority)
{
    _priority = priority;
}

void TimerBase::Initialize(InvokerPriority priority, Uint32 interval)
{
    _enabled = false;
    _priority = priority;
    _dueTime = 0;
    _interval = interval;
}

void TimerBase::OnStart()
{

}

void TimerBase::OnEnd()
{

}

void TimerBase::OnFinish()
{

}

void TimerBase::OnTimer()
{
    if (Tick.safe_bool())
    {
        EventArg e;
        Tick(this, &e);
    }
}

// ======================================================
// AssignerTimer

AssignerTimer::AssignerTimer()
{
    _assigner = suic::Assigner::Current();
}

AssignerTimer::AssignerTimer(suic::Assigner* assigner)
{
    Initialize(assigner, InvokerPriority::eBackgound, 10);
}

AssignerTimer::AssignerTimer(Uint32 interval)
{
    Initialize(NULL, InvokerPriority::eBackgound, interval);
}

AssignerTimer::AssignerTimer(Uint32 interval, EventHandler cb)
{
    Initialize(NULL, InvokerPriority::eBackgound, interval);
    SetTick(cb);
}

AssignerTimer::AssignerTimer(Uint32 interval, InvokerPriority priority)
{
    Initialize(NULL, priority, interval);
}

AssignerTimer::AssignerTimer(Uint32 interval, InvokerPriority priority, EventHandler cb)
{
    Initialize(NULL, priority, interval);
    SetTick(cb);
}

void AssignerTimer::Start()
{
    if (!IsEnabled())
    {
        SetEnabled(true);
        Restart();
        OnStart();
    }
}

void AssignerTimer::Stop()
{
    if (IsEnabled())
    {
        SetEnabled(false);
        _assigner->RemoveTimer(this);
    }
}

Object* AssignerTimer::FireTick(bool unused)
{
    if (IsEnabled())
    {
        OnTimer();
        Restart();
    }
    else
    {
        OnFinish();
    }

    unref();

    return NULL;
}

void AssignerTimer::Initialize(Assigner* assigner, InvokerPriority priority, Uint32 interval)
{
    if (assigner != NULL)
    {
        _assigner = assigner;
    }
    else
    {
        _assigner = Assigner::Current();
    }

    TimerBase::Initialize(priority, interval);
}

void AssignerTimer::Promote()
{

}

void AssignerTimer::Restart()
{
    if (NULL != _assigner)
    {
        _dueTime = GetTickCount() + _interval;
        _assigner->AddTimer(this);
    }
}

}
