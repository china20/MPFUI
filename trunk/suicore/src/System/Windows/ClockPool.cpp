
#include "clockpool.h"

#include <System/Types/Interface.h>
#include <System/Tools/Vector.h>

#include <System/Tools/SSE.h>
#include <System/Windows/RenderInfo.h>
#include <System/Windows/HwndObject.h>
#include <System/Windows/CoreTool.h>
#include <System/Interop/InternalWindowOper.h>

#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")

namespace suic
{

static UIMutex g_clock_mutex;
static UIMutex g_clock_group_mutex;

TimerGroup* GetTimerGroup(Element* elem, int id, int elapse)
{
    VisualHost* target = suic::GetRenderTarget(elem);
    if (!target)
    {
        return NULL;
    }

    RenderInfoPtr rinfo(target->GetRenderInfo());
    HwndObject* hwndObj(rinfo->GetHwndObject());

    //return hwndObj->GetHightClockGroup();

    /*if (-1 == id)
    {
        return hwndObj->GetHightClockGroup();
    }
    else*/
    {
        return hwndObj->GetClockGroup(id, elapse);
    }
}

void RemoveTimerGroup(TimerGroup* grp)
{
    ClockGroup* cgp = ClockGroupPtr::cast(grp);
    if (cgp)
    {
        cgp->ClearTimers();
    }
}

ClockGroup::ClockGroup(int id, HwndObject* hwndobj)
    : _hwndobj(hwndobj)
    , _elapse(UI_SYSINTERVAL)
    , _timerid(id)
    , _started(false)
    , _stopped(false)
    , _hostHwnd(NULL)
{
}

ClockGroup::ClockGroup()
    : _hwndobj(NULL)
    , _elapse(UI_SYSINTERVAL)
    , _timerid(-1)
    , _started(false)
    , _stopped(false)
    , _hostHwnd(NULL)
{
}

ClockGroup::~ClockGroup()
{
}

bool ClockGroup::Start(HwndObject* hwndobj)
{
    // 根元素不相等
    if (_hwndobj != hwndobj)
    {
        return false;
    }

    if (_started)
    {
        return true;
    }

    Element* root(_hwndobj->GetRootElement());
    if (root)
    {
        HWND hwnd = uiHwndFromElement(root);
        int id = (int)::SetTimer(hwnd, _timerid, _elapse, NULL);

        _hostHwnd = hwnd;

        if (id == _timerid)
        {
            _started = true;
        }
    }

    return true;
}

int ClockGroup::GetElapse()
{
    return _elapse;
}

void ClockGroup::SetElapse(int val)
{
    _elapse = val;
}

bool ClockGroup::IsRunning()
{
    return _started;
}

int ClockGroup::GetCount()
{
    UIMutex::Locker sync(g_clock_group_mutex);
    return _clocks.GetCount();
}

int ClockGroup::AddTimer(int elapse, TimerBase* tb, TimerFunc tf, int priority)
{
    int index = FindTimer(tb);
    if (index != -1 || _stopped)
    {
        return index;
    }

    ClockItem cim;

    cim.elapse = elapse;
    cim.tb = tb;
    cim.tf = tf;
    cim.id = 0; //A身份证 F境外人员身份证

    UIMutex::Locker sync(g_clock_group_mutex);

    ValueDic<int, ClockItem>* clocks = &_clocks;

    if (priority == TimerGroup::TimerHight)
    {
        clocks = &(_hwndobj->GetHightClockGroup()->_clocks);
    }

    for (;;)
    {
        cim.id++;
        if (!clocks->Contains(cim.id))
        {
            break;
        }
    }

    clocks->Add(cim.id, cim);

    return cim.id;
}

int ClockGroup::AddTimer(int elapse, TimerBase* tb, int priority)
{
    return AddTimer(elapse, tb, TimerFunc(), priority);
}

void ClockGroup::StopTimer(TimerBase* tb)
{

}

void ClockGroup::RemoveTimer(TimerBase* tb)
{
    UIMutex::Locker sync(g_clock_group_mutex);

    ValueDic<int, ClockItem>::Enumerator enumer(&_clocks, 0);
    
    while (enumer.HasNext())
    {
        if (enumer.Current()->val.tb.get() == tb)
        {
            enumer.Current()->val.id = 0;
            break;
        }
    }
}

void ClockGroup::RemoveTimerByIndex(int id)
{
    UIMutex::Locker sync(g_clock_group_mutex);
    
    ValueDic<int, ClockItem>::Enumerator enumer(&_clocks, 0);

    while (enumer.HasNext())
    {
        if (enumer.Current()->val.id == id)
        {
            enumer.Current()->val.id = 0;
            break;
        }
    }
}

void ClockGroup::ClearTimers()
{
    UIMutex::Locker sync(g_clock_group_mutex);

    _stopped = true;

    if (!_hwndobj || !_started)
    {
        _clocks.Clear();
        return;
    }

    /*Element* root(_hwndobj->GetRootElement());
    if (root)
    {
        HWND hwnd = __GetHwndFromElement(root);
        ::KillTimer(hwnd, _timerid);
    }*/

    _clocks.Clear();
    _started = false;
}

void ClockGroup::InternalRemoveTimer()
{
    for (;;)
    {
        bool bLoop = false;

        ValueDic<int, ClockItem>::Enumerator enumer(&_clocks, 0);
        while (enumer.HasNext())
        {
            if (enumer.Current()->val.id == 0)
            {
                _clocks.Remove(enumer.Current()->key);
                bLoop = true;
                break;
            }
        }

        if (!bLoop)
        {
            break;
        }
    }

    //Debug::Trace(_T("运行定时器个数：%d\n"), _clocks.size());
}

void ClockGroup::RemoveTimer(TimerFunc tb)
{
    UIMutex::Locker sync(g_clock_group_mutex);

    ValueDic<int, ClockItem>::Enumerator enumer(&_clocks, 0);
    while (enumer.HasNext())
    {
        if (enumer.Current()->val.tf.Equals(&tb))
        {
            _clocks.Remove(enumer.Current()->key);
            break;
        }
    }
}

int ClockGroup::GetTimerid()
{
    return _timerid;
}

bool ClockGroup::Contains(TimerBase* tb)
{
    return (FindTimer(tb) != -1);
}

int ClockGroup::FindTimer(TimerBase* tb)
{
    UIMutex::Locker sync(g_clock_group_mutex);

    ValueDic<int, ClockItem>::Enumerator enumer(&_clocks, 0);
    while (enumer.HasNext())
    {
        if (enumer.Current()->val.tb.get() == tb)
        {
            return enumer.Current()->key;
        }
    }

    return -1;
}

void ClockGroup::OnTimeout(int id, Object*)
{
    if (!_stopped)
    {
        UIMutex::Locker sync(g_clock_group_mutex);

        ValueDic<int, ClockItem>::Enumerator enumer(&_clocks, 0);
        while (enumer.HasNext())
        {
            const ClockItem& cim = enumer.Current()->val;
            if (cim.id > 0)
            {
                if (cim.tf.safe_bool())
                {
                    //cim.tf(id, cim.data);
                }
                else if (cim.tb && cim.tb->CheckInterval(GetElapse()))
                {
                    cim.tb->OnTimeout(id, cim.data.get());
                }
            }
        }

        InternalRemoveTimer();
    }
}

//===========================================================
ImmTimerPool::ImmTimerPool()
    : _timerid(0)
{

}

ClockGroup* ImmTimerPool::GetClockGroup(int timerid)
{
    UIMutex::Locker sync(g_clock_mutex);

    Map<int, ClockGroupPtr>::iterator iter = _clocks.find(timerid);
    if (iter == _clocks.end())
    {
        return NULL;
    }
    else
    {
        return iter->second;
    }
}

int ImmTimerPool::AddClockGroup(HwndObject* hwndobj)
{
    UIMutex::Locker sync(g_clock_mutex);

    int timerid = 0;
    for (;;)
    {
        ++timerid;
        if (_clocks.find(timerid) == _clocks.end())
        {
            break;
        }
    }

    if (_timerid == 0)
    {
        int interval = UI_SYSINTERVAL - 5;
        if (hwndobj->GetRootElement())
        {
            Object* obj(hwndobj->GetRootElement()->GetAttribute(_U("Interval")));
            if (obj)
            {
                int val = obj->ToInt();
                if (val >= 10 && val <= 100)
                {
                    interval = val;
                }
            }
        }
        _timerid = timeSetEvent(interval, 10, &ImmTimerPool::TimerProc, (DWORD_PTR)this
            , TIME_CALLBACK_FUNCTION | TIME_PERIODIC/* | TIME_KILL_SYNCHRONOUS*/);
    }
    
    ClockGroup* clk = new ClockGroup(timerid, hwndobj);
    _clocks.insert(timerid, clk);
    return timerid;
}

void ImmTimerPool::RemoveClockGroup(int timerid)
{
    UIMutex::Locker sync(g_clock_mutex);
    Map<int, ClockGroupPtr>::iterator iter = _clocks.find(timerid);
    if (iter != _clocks.end())
    {
        iter->second->ClearTimers();
        _clocks.erase(iter);
    }
    if (_clocks.size() == 0)
    {
        timeKillEvent(_timerid);
        _timerid = 0;
    }
}

void CALLBACK ImmTimerPool::TimerProc(UINT id, UINT msg, DWORD_PTR user, DWORD_PTR dw1, DWORD_PTR dw2)
{
    ImmTimerPool* itp = (ImmTimerPool*)user;
    Array<ClockGroupPtr> tmps;

    {
        UIMutex::Locker sync(g_clock_mutex);
        Map<int, ClockGroupPtr>::iterator iter = itp->_clocks.begin();
        while (iter != itp->_clocks.end())
        {
            tmps.Add(iter->second);
            ++iter;
        }
    }

    for (int i = 0; i < tmps.Length(); ++i)
    {
        tmps[i]->OnTimeout(id, NULL);
    }
}

}
