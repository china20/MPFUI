// 华勤科技版权所有 2008-2022
// 
// 文件名：UIRouteEvent.h
// 功  能：定义核心库的路由事件基类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIROUTEEVENT_H_
#define _UIROUTEEVENT_H_

#include <System/Windows/Object.h>
#include <System/Windows/Delegate.h>

namespace suic
{

/// <summary>
/// 界面元素的处理方式，Tunneling：如果注册了回溯事件，系统事件处理模块
/// 会调用其事件处理，直到事件被处理掉。
/// </summary>
struct RoutingStrategy
{
    enum
    {
        Direct,
        Bubble,
        Tunnel,
    };
};

typedef Byte RoutingStrategyType;

class RoutedEvent
{
public:

    RoutedEvent(const String& name, RoutingStrategyType routingStrategy, RTTIOfInfo* handlerType, RTTIOfInfo* ownerType);
    RoutingStrategyType GetRoutingStrategy() const;
    RTTIOfInfo* GetOwnerType() const;
    RTTIOfInfo* GetHandlerType() const;
    String GetName() const;
    int GetIndex() const;
    int GetHashCode() const;

protected:

    String _name;
    int _hashCode;
    int _globalIndex;

    RTTIOfInfo* _ownerType;
    RTTIOfInfo* _handlerType;
    
    RoutingStrategyType _routStrategy;    
};

};

#endif
