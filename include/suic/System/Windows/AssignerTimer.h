// 华勤科技版权所有 2010-2011
// 
// 文件名：AssignerTimer.h
// 功  能：定义核心库的定时器对象。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIASSIGNERTIMER_H_
#define _UIASSIGNERTIMER_H_

#include <System/Windows/TimerBase.h>

namespace suic
{

class Assigner;
class AssignerTimer;

class SUICORE_API AssignerTimer : public TimerBase
{
public:

    AssignerTimer();
    AssignerTimer(suic::Assigner* assigner);
    AssignerTimer(Uint32 interval);
    AssignerTimer(Uint32 interval, EventHandler cb);
    AssignerTimer(Uint32 interval, InvokerPriority priority);
    AssignerTimer(Uint32 interval, InvokerPriority priority, EventHandler cb);

    void Start();
    void Stop();

private:

    Object* FireTick(bool unused);
    void Promote();
    void Restart();

    void Initialize(Assigner* assigner, InvokerPriority priority, Uint32 interval);

    suic::Assigner* _assigner;
    friend class Assigner;
};

}

#endif
