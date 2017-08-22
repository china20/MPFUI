// 华勤科技版权所有 2010-2011
// 
// 文件名：UIEvent.h
// 功  能：定义核心库的事件基类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIEVENT_H_
#define _UIEVENT_H_

#include <System/Windows/Object.h>
#include <System/Windows/Delegate.h>
#include <System/Windows/EventArg.h>

namespace suic
{

class SUICORE_API CancelEventArg  : public EventArg
{
public:

    CancelEventArg(bool cancel);

    bool IsCancel();
    void SetCancel(bool cancel);

protected:

    bool _cancel;
};

};

#endif
