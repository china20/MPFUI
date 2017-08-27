
#include <System/Animation/TimerThread.h>

namespace suic
{

TimerInvoker::TimerInvoker()
{
    SetPriority((InvokerPriority)eRender);
}

TimerInvoker::~TimerInvoker()
{
}

void TimerInvoker::Invoke()
{
    if (_timerEntry.IsAlive())
    {
        ((TimerEntry*)_timerEntry.GetTarget())->FireTick(false);
    }
}

TimerEntry::TimerEntry()
{
    _assigner = NULL;
    _timerInvoker = NULL;
}

TimerEntry::TimerEntry(Uint32 interval)
{
    Initialize(NULL, InvokerPriority::eBackgound, interval);
}

TimerEntry::TimerEntry(Uint32 interval, EventHandler cb)
{
    Initialize(NULL, InvokerPriority::eBackgound, interval);
    SetTick(cb);
}

TimerEntry::TimerEntry(Uint32 interval, InvokerPriority priority)
{
    Initialize(NULL, priority, interval);
}

TimerEntry::TimerEntry(Uint32 interval, InvokerPriority priority, EventHandler cb)
{
    Initialize(NULL, priority, interval);
    SetTick(cb);
}

TimerEntry::~TimerEntry()
{
    if (_timerInvoker != NULL)
    {
        _timerInvoker->unref();
    }
}

void TimerEntry::Initialize(Assigner* assigner, InvokerPriority priority, Uint32 interval)
{
    TimerBase::Initialize(priority, interval);
    _assigner = assigner;
    _timerInvoker = NULL;
}

void TimerEntry::Start()
{
    if (!IsEnabled())
    {
        SetEnabled(true);
        Restart();
        OnStart();
    }
}

void TimerEntry::Stop()
{
    if (IsEnabled())
    {
        SetEnabled(false);
        TimerThread::Ins()->RemoveTimer(this);
    }
}

void TimerEntry::SetAssigner(suic::Assigner* assigner)
{
    _assigner = assigner;
}

Object* TimerEntry::FireTick(bool unused)
{
    OnTimer();

    if (IsEnabled())
    {
        Restart();
    }
    else
    {
        OnFinish();
    }

    unref();

    return NULL;
}

void TimerEntry::Promote()
{

}

void TimerEntry::Restart()
{
    _dueTime = Environment::GetSystemTick() + _interval;
    TimerThread::Ins()->AddTimer(this);
}

TimerThread::TimerThread()
{
    _bExit = false;
    _interval = 10;
    _timersVersion = 0;
    _dueTimeInTicks = 0;
    _isTimerSet = false;
    _dueTimeFound = false;

    setAutoDelete(false);
}

TimerThread* TimerThread::Ins()
{
    static TimerThread _ins;
    return &_ins;
}

void TimerThread::Close()
{
    _bExit = true;
    Join();
}

void TimerThread::AddTimer(TimerEntry* timer)
{
    {
        suic::Mutex::Locker sync(insLock);

        if (!_bExit)
        {
            bool bExist = false;

            timer->ref();

            for (int i = 0; i < _timers.Length(); ++i)
            {
                if (_timers[i] == timer)
                {
                    bExist = true;
                    break;
                }
            }

            if (!bExist)
            {
                _timers.Add(timer);
                _timersVersion += 1;

                if (!IsAlive())
                {
                    Start(0);
                }
            }
            else
            {
                timer->unref();
            }
        }
    }

    UpdateDispachterTimer(false);
}

void TimerThread::RemoveTimer(TimerEntry* timer)
{
    {
        suic::Mutex::Locker sync(insLock);

        if (!_bExit)
        {
            for (int i = 0; i < _timers.Length(); ++i)
            {
                if (_timers[i] == timer)
                {
                    timer->unref();
                    _timers.RemoveAt(i);
                    break;
                }
            }
            _timersVersion += 1L;
        }
    }

    UpdateDispachterTimer(false);
}

void TimerThread::Run()
{
    for (;;)
    {
        // Ä¬ÈÏ10ºÁÃëÒ»´Î
        Sleep(_interval);

        if (_bExit)
        {
            break;
        }

        PromoteTimers(Environment::GetSystemTick());
    }

    FinishTimerThread();
}

void TimerThread::PromoteTimers(Uint32 currTimeInTicks)
{
    {
        suic::Mutex::Locker sync(insLock);

        if (_bExit || !_dueTimeFound || _dueTimeInTicks > currTimeInTicks)
        {
            return;
        }
    }

    for (;;)
    {
        TimerEntry* pTimer = NULL;

        {
            suic::Mutex::Locker sync(insLock);

            for (int i = 0; i < _timers.Length(); ++i)
            {
                if (_timers[i]->GetDueTime() <= currTimeInTicks)
                {
                    pTimer = _timers[i];
                    _timers.RemoveAt(i);
                    break;
                }
            }
        }

        if (NULL == pTimer)
        {
            break;
        }

        if (NULL != pTimer->_assigner)
        {
            if (pTimer->_timerInvoker == NULL)
            {
                pTimer->_timerInvoker = new TimerInvoker();
                pTimer->_timerInvoker->ref();
            }

            pTimer->_timerInvoker->_timerEntry = pTimer;
            pTimer->_assigner->BeginInvoke(pTimer->_timerInvoker);
        }
        else
        {
            pTimer->FireTick(false);
        }
    }

    UpdateDispachterTimer(false);
}

void TimerThread::SetAndCheckInterval(int iTicks)
{
    _interval = iTicks - Environment::GetSystemTick();

    if (_interval < 1)
    {
        _interval = 1;
    }

    _isTimerSet = true;
}

void TimerThread::UpdateDispachterTimer(bool unUsed)
{
    if (!_bExit)
    {
        bool flag = _dueTimeFound;
        int num2 = _dueTimeInTicks;
        _dueTimeFound = false;
        _dueTimeInTicks = 0;

        if (_timers.Length() > 0)
        {
            for (int i = 0; i < _timers.Length(); i++)
            {
                TimerEntry* timer = _timers[i];

                if (!_dueTimeFound || (timer->_dueTime < _dueTimeInTicks))
                {
                    _dueTimeFound = true;
                    _dueTimeInTicks = timer->_dueTime;
                }
            }
        }

        if (_dueTimeFound)
        {
            if ((!_isTimerSet || !flag) || (num2 != _dueTimeInTicks))
            {
                SetAndCheckInterval(_dueTimeInTicks);
            }
        }
        else if (flag)
        {
            ;
        }
    }
}

void TimerThread::FinishTimerThread()
{
    for (int i = 0; i < _timers.Length(); ++i)
    {
        if (_timers[i] != NULL)
        {
            _timers[i]->unref();
        }
    }

    _timers.Clear();
}

}
