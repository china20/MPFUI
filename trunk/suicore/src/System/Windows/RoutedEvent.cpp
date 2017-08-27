// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// uiroutedevent.cpp


#include <System/Windows/Object.h>
#include <System/Windows/Delegate.h>
#include <System/Windows/DpProperty.h>
#include <System/Windows/EventManager.h>

namespace suic
{

RoutedEvent::RoutedEvent(const String& name, RoutingStrategyType routStrategy, RTTIOfInfo* handlerType, RTTIOfInfo* ownerType)
{
    _name = name;
    _routStrategy = routStrategy; 
    _ownerType = ownerType;
    _handlerType = handlerType;
    _hashCode = suic::DpProperty::CalcHashCode(_name, _ownerType);

    _globalIndex = EventControl::GetNextGlobalIndex(this);
}

RoutingStrategyType RoutedEvent::GetRoutingStrategy() const
{
    return _routStrategy;
}

RTTIOfInfo* RoutedEvent::GetOwnerType() const
{
    return _ownerType;
}

RTTIOfInfo* RoutedEvent::GetHandlerType() const
{
    return _handlerType;
}

String RoutedEvent::GetName() const
{
    return _name;
}

int RoutedEvent::GetIndex() const
{
    return _globalIndex;
}

int RoutedEvent::GetHashCode() const
{
    return _hashCode;
}

}
