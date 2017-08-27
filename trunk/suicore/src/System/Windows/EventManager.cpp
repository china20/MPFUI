// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// eventmanager.cpp


#include <System/Windows/EventManager.h>
#include <System/Windows/Element.h>

namespace suic
{

int EventControl::_countRoutedEvents = 0;
ValueDic<int, RoutedEvent*> EventControl::_routedEvents;
ClassTypeMap EventControl::_handlers;

RoutedEvent* EventControl::
RegisterRoutedEvent(String name, RoutingStrategyType routingStrategy, RTTIOfInfo* handlerType, RTTIOfInfo* ownerType)
{
    RoutedEvent* routedEvent = new RoutedEvent(name,routingStrategy, handlerType, ownerType); 
    _countRoutedEvents++;
    return routedEvent;
}

void EventControl::
RemoveClassHandler(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent, Object* handler)
{
    EventHandlersInfo* pInfo = _handlers.Remove(ownerType->classIndex);
    if (pInfo)
    {
        delete pInfo;
    }
}

void EventControl::RegisterClassHandler(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent, 
                          Handler* handler,bool handledToo)
{
    RoutedEventHandlerInfoList* eSelf = NULL;

    //
    // 根据元素类型找到对应类处理信息
    //
    RoutedEventHandlerInfoList* eOthers = GetClassRoutedEventHandlerInfo(ownerType->baseType(), routedEvent);

    //
    // 查找此元素类有没有被注册过
    //
    EventHandlersInfo* pInfo = _handlers[ownerType->classIndex];

    //
    // 没有被注册过，则在元素类上新建一个EventHandlersInfo
    //
    if (pInfo == NULL)
    {
        EventHandlersInfo* ehi = new EventHandlersInfo();

        ehi->classHandlers = new EventHandlersStore();
        //ehi->classHandlers->ref();

        //
        // 记录此类上的一个事件处理例程
        //
        eSelf = ehi->classHandlers->AddHandler(routedEvent, handler, handledToo);
        _handlers.Add(ownerType->classIndex, ehi);
    }
    else
    {
        if (!pInfo->classHandlers)
        {
            pInfo->classHandlers = new EventHandlersStore();
            //pInfo->classHandlers->ref();
            eSelf = pInfo->classHandlers->AddHandler(routedEvent, handler, handledToo);
        }
        else if (!pInfo->classHandlers->GetRoutedEventHandlerInfoList(routedEvent))
        {
            eSelf = pInfo->classHandlers->AddHandler(routedEvent, handler, handledToo);
        }
    }

    if (eSelf && eOthers && eOthers->RoutedEHIList.Length() > 0)
    {
        eSelf->RoutedEHIList.Add(eOthers->RoutedEHIList[0]);
    }
}

void EventControl::RegisterAttachedHandler(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent, 
                             Handler* handler,bool handledToo)
{
    EventHandlersInfo* pInfo = _handlers[ownerType->classIndex];

    if (pInfo == NULL)
    {
        EventHandlersInfo* ehi = new EventHandlersInfo();

        ehi->attachedHandlers = new EventHandlersStore();
        ehi->attachedHandlers->AddHandler(routedEvent, handler, handledToo);
        _handlers.Add(ownerType->classIndex, ehi);
    }
    else
    {
        if (!pInfo->attachedHandlers)
        {
            pInfo->attachedHandlers = new EventHandlersStore();
            pInfo->attachedHandlers->AddHandler(routedEvent, handler, handledToo);
        }
        else if (!pInfo->attachedHandlers->GetRoutedEventHandlerInfoList(routedEvent))
        {
            pInfo->attachedHandlers->AddHandler(routedEvent, handler, handledToo);
        }
    }
}

RoutedEvent* EventControl::FindRoutedEvent(const String& name, RTTIOfInfo* ownerType)
{
    RoutedEvent* routedEvt = NULL;
    int hashCode = 0;
    RTTIOfInfo* tmpType = ownerType;

    while (tmpType)
    {
        hashCode = suic::DpProperty::CalcHashCode(name, tmpType);
        if (_routedEvents.TryGetValue(hashCode, routedEvt))
        {
            break;
        }
        if (tmpType == DpObject::RTTIType())
        {
            break;
        }
        tmpType = (*tmpType->baseType)();
    }
    return routedEvt;
}

EventHandlersStore* EventControl::GetAttachedEventHandlersStore(RTTIOfInfo* ownerType)
{
    EventHandlersInfo* pInfo = _handlers[ownerType->classIndex];

    if (pInfo != NULL)
    {
        return pInfo->attachedHandlers;
    }
    else
    {
        return NULL;
    }
}

EventHandlersStore* EventControl::GetClassEventHandlersStore(RTTIOfInfo* ownerType)
{
    RTTIOfInfo* tmpType = ownerType;

    while (tmpType != NULL)
    {
        EventHandlersInfo* pInfo = _handlers[tmpType->classIndex];

        if (pInfo != NULL)
        {
            return pInfo->classHandlers;
        }

        tmpType = (*tmpType->baseType)();
    }

    return NULL;
}

RoutedEventHandlerInfoList* EventControl::GetClassRoutedEventHandlerInfo(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent)
{
    RTTIOfInfo* tmpType = ownerType;

    while (tmpType != NULL)
    {
        EventHandlersInfo* pInfo = _handlers[tmpType->classIndex];

        if (pInfo != NULL && pInfo->classHandlers)
        {
            RoutedEventHandlerInfoList* reh(pInfo->classHandlers->GetRoutedEventHandlerInfoList(routedEvent));
            if (reh)
            {
                return reh;
            }
        }

        if (tmpType == Element::RTTIType())
        {
            break;
        }

        tmpType = (*tmpType->baseType)();
    }

    return NULL;
}

RoutedEventHandlerInfoList* EventControl::GetAttachedRoutedEventHandlerInfo(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent)
{
    EventHandlersInfo* pInfo = _handlers[ownerType->classIndex];

    if (pInfo != NULL && pInfo->attachedHandlers)
    {
        RoutedEventHandlerInfoList* reh(pInfo->attachedHandlers->GetRoutedEventHandlerInfoList(routedEvent));
        if (reh)
        {
            return reh;
        }
    }

    return NULL;
}

int EventControl::GetNextGlobalIndex(RoutedEvent* val)
{
    _routedEvents.Add(val->GetHashCode(), val);
    return _routedEvents.GetCount();
}

void EventControl::Clear()
{
    _handlers.Clear();
}

}
