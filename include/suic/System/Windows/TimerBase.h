// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����TimerBase.h
// ��  �ܣ�������Ŀ�Ķ�ʱ������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
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
