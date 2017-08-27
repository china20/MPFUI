// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// eventhandlersstore.cpp

#include <System/Windows/EventHandlersStore.h>

namespace suic 
{

RoutedEventHandlerInfo::RoutedEventHandlerInfo()
    : handlerType(0)
    , handler(NULL)
    , Object(false)
{
    
}

RoutedEventHandlerInfo::~RoutedEventHandlerInfo()
{
    if (handler)
    {
        handler->unref();
    }
}

void RoutedEventHandlerInfo::Clear()
{
    if (handler)
    {
        handler->unref();
        handler = NULL;
    }
}

RoutedEventHandlerInfoList::RoutedEventHandlerInfoList()
{

}

RoutedEventHandlerInfoList::~RoutedEventHandlerInfoList()
{
    Clear();
}

void RoutedEventHandlerInfoList::Clear()
{
    for (int i = 0; i < RoutedEHIList.Length(); ++i)
    {
        RoutedEHIList[i]->Clear();
        delete RoutedEHIList[i];
    }
    RoutedEHIList.Clear();
}

EventHandlersStore::EventHandlersStore()
{
}

EventHandlersStore::~EventHandlersStore()
{
    Clear();
}

void EventHandlersStore::Clear()
{
    Collection<RoutedEventHandlerInfoList*> values;
    _entries.GetValues(values);

    for (int i = 0; i < values.GetCount(); ++i)
    {
        values[i]->Clear();
        delete values[i];
    }

    /*ValueDic<int, RoutedEventHandlerInfoList*>::Enumerator enumer(&_entries, 0);
    
    while (enumer.HasNext())
    {
        delete enumer.Current()->val;
    }*/
    _entries.Clear();
}

RoutedEventHandlerInfoList* EventHandlersStore::GetRoutedEventHandlerInfoList(const RoutedEvent* routedEvent)
{
    RoutedEventHandlerInfoList* rehi = NULL;
    _entries.TryGetValue(routedEvent->GetIndex(), rehi);
    return rehi;
}

RoutedEventHandlerInfoList* EventHandlersStore::AddHandler(const RoutedEvent* routedEvent, Handler* handler, bool handledToo)
{
    RoutedEventHandlerInfoList* rehi = GetRoutedEventHandlerInfoList(routedEvent);
    RoutedEventHandlerInfo* reh(new RoutedEventHandlerInfo());

    reh->handler = handler;
    if (NULL != reh->handler)
    {
        reh->handler->ref();
    }

    reh->invokeHandledEventsToo = handledToo;

    if (NULL == rehi)
    {
        rehi = new RoutedEventHandlerInfoList();
        rehi->RoutedEHIList.Add(reh);
        _entries.Add(routedEvent->GetIndex(), rehi);

        return rehi;
    }
    else
    {
        rehi->RoutedEHIList.Add(reh);
        return rehi;
    }
}

void EventHandlersStore::RemoveHandler(const RoutedEvent* routedEvent, Handler* handler)
{
    RoutedEventHandlerInfoList* rehi = GetRoutedEventHandlerInfoList(routedEvent);

    if (NULL != rehi)
    {
        for (int i = 0; i < rehi->RoutedEHIList.Length(); ++i)
        {
            if (!handler)
            {
                delete rehi->RoutedEHIList[i];
            }
            else if (handler->Equals(rehi->RoutedEHIList[i]->handler))
            {
                delete rehi->RoutedEHIList[i];
                rehi->RoutedEHIList.RemoveAt(i);
                break;
            }
        }

        //
        // 如果为空，清除所有注册的事件处理
        //
        if (!handler)
        {
            rehi->RoutedEHIList.Clear();
        }

        if (rehi->RoutedEHIList.Length() == 0)
        {
            delete rehi;
            _entries.Remove(routedEvent->GetIndex());
        }
    }
}

}
