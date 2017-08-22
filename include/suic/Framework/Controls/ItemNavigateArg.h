// 华勤科技版权所有 2010-2011
// 
// 文件名：ItemNavigateArg.h
// 功  能：实现Item容器的基类
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIITEMNAVIGATEARG_H_
#define _UIITEMNAVIGATEARG_H_

#include <System/Input/InputDevice.h>

namespace suic
{

class ItemNavigateArg : public EventArg
{
private:
    
    InputDevice* _deviceUsed;
    Uint16 _modifierKey;

    static ItemNavigateArg _empty;

public:
    
    ItemNavigateArg(InputDevice* deviceUsed, Uint16 modifierKey)
    {
        _deviceUsed = deviceUsed;
        _modifierKey = modifierKey;
    }

    InputDevice* GetDeviceUsed() const
    {
        return _deviceUsed;
    }

    Uint16 GetModifierKey() const
    {
        return _modifierKey;
    }

    static ItemNavigateArg GetEmpty()
    {
        return _empty;
    }
};

typedef delegate<void(Object*, ItemsChangedEventArg*)> ItemsChangedEventHandler;

}

#endif
