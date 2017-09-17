// 华勤科技版权所有 2008-2022
// 
// 文件名：eventmanager.h
// 功  能：定义核心库的事件管理。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIEVENTMANAGER_H_
#define _UIEVENTMANAGER_H_

#include <System/Tools/Array.h>
#include <System/Tools/HashMap.h>
#include <System/Types/Integer.h>
#include <System/Windows/RoutetEvent.h>
#include <System/Windows/EventHandlersStore.h>

namespace suic
{

struct EventHandlersInfo
{
    EventHandlersStore* classHandlers;
    EventHandlersStore* attachedHandlers;
    bool hasAttach;

    EventHandlersInfo()
    {
        classHandlers = NULL;
        attachedHandlers = NULL;
        hasAttach = false;
    }
};

class ClassTypeMap
{
private:
    
    ArrayType<EventHandlersInfo*, false> _entries;
    HashMap<IntegerType, EventHandlersInfo*> _overFlow;

public:
    
    ClassTypeMap()
    {
        _entries.Reset(256, true);
    }

    void Clear()
    {
        /*
        if (iter->second.classHandlers)
        {
        iter->second.classHandlers->Clear();
        iter->second.classHandlers->__release();
        }
        if (iter->second.attachedHandlers)
        {
        iter->second.attachedHandlers->Clear();
        iter->second.attachedHandlers->__release();
        }
        */
    }

    EventHandlersInfo* Remove(int index)
    {
        EventHandlersInfo* retInfo = NULL;

        if (index < _entries.Length())
        {
            if (_entries[index] != NULL)
            {
                retInfo = _entries[index];
                _entries[index] = NULL;
            }
        }
        else
        {
            IntegerType itv(index);
            retInfo = GetHandlersInfo(index);
            _overFlow.erase(itv);
        }
        return retInfo;
    }

    void Add(int index, EventHandlersInfo* val)
    {
        if (index < _entries.Length())
        {
            _entries[index] = val;
        }
        else
        {
            IntegerType itv(index);
            _overFlow.insert(itv, val);
        }
    }

    EventHandlersInfo* operator[](int index)
    {
        return GetHandlersInfo(index);
    }

private:

    EventHandlersInfo* GetHandlersInfo(int index)
    {
        if (index < _entries.Length())
        {
            return _entries[index];
        }
        else
        {
            IntegerType itv(index);
            HashMap<IntegerType, EventHandlersInfo*>::iterator iter = _overFlow.find(itv);
            if (iter != _overFlow.end())
            {
                return iter->second;
            }
            else
            {
                return NULL;
            }
        }
    }
};

class EventControl
{
public:

    static RoutedEvent* RegisterRoutedEvent(String name, RoutingStrategyType routingStrategy, 
        RTTIOfInfo* handlerType,RTTIOfInfo* ownerType);
    static void RegisterClassHandler(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent, 
        Handler* handler,bool handledToo);
    static void RegisterAttachedHandler(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent, 
        Handler* handler,bool handledToo);
    static RoutedEvent* FindRoutedEvent(const String& name, RTTIOfInfo* ownerType);

    static void RemoveClassHandler(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent, Object* handler);

    static EventHandlersStore* GetClassEventHandlersStore(RTTIOfInfo* ownerType);
    static EventHandlersStore* GetAttachedEventHandlersStore(RTTIOfInfo* ownerType);

    static RoutedEventHandlerInfoList* GetClassRoutedEventHandlerInfo(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent);
    static RoutedEventHandlerInfoList* GetAttachedRoutedEventHandlerInfo(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent);

    static int GetNextGlobalIndex(RoutedEvent* val);
    static void Clear();

protected:

    static int _countRoutedEvents;
    static ValueDic<int, RoutedEvent*> _routedEvents;
    static ClassTypeMap _handlers;
};

};

#endif
