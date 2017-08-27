// 华勤科技版权所有 2008-2022
// 
// 文件名：TimerBase.h
// 功  能：定义核心库的定时器对象。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UITIMERBASE_H_
#define _UITIMERBASE_H_

#include <System/Windows/EventHandler.h>
#include <System/Windows/AssignerInvoker.h>

namespace suic
{

class Assigner;
class AssignerTimer;

class SUICORE_API TimerBase : public Object
{
public:

    TimerBase();
    virtual ~TimerBase();

    bool IsEnabled();
    void SetEnabled(bool val);

    Uint32 GetDueTime() const;
    void SetTick(EventHandler cb);
    void SetInterval(Uint32 interval);
    void SetPriority(InvokerPriority priority);

    void ClearTick();

protected:

    virtual void OnStart();
    virtual void OnEnd();
    virtual void OnFinish();

    virtual void OnTimer();

    void Initialize(InvokerPriority priority, Uint32 interval);

protected:

    Uint32 _dueTime;
    Uint32 _interval;
    bool _enabled;

    EventHandler Tick;
    InvokerPriority _priority;
};

inline Uint32 TimerBase::GetDueTime() const
{
    return _dueTime;
}

inline bool TimerBase::IsEnabled()
{
    return _enabled;
}

inline void TimerBase::SetEnabled(bool val)
{
    _enabled = val;
}

}

#endif
