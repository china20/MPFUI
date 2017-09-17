// 华勤科技版权所有 2008-2022
// 
// 文件名：EventHandlersStore.h
// 功  能：存储事件处理。
// 
// 作  者：MPF开发组
// 时  间：2012-07-02
// 
// ============================================================================

#ifndef _UIEVENTHANDLERSTORE_H_
#define _UIEVENTHANDLERSTORE_H_

#include <System/Windows/Object.h>
#include <System/Windows/RoutetEvent.h>
#include <System/Tools/Collection.h>

namespace suic 
{

class SUICORE_API RoutedEventHandlerInfo : public Object
{
public:

    int handlerType;
    Handler* handler;
    bool invokeHandledEventsToo;

    RoutedEventHandlerInfo();
    ~RoutedEventHandlerInfo();

    void Clear();
};

class SUICORE_API RoutedEventHandlerInfoList
{
public:

    Array<RoutedEventHandlerInfo*, false> RoutedEHIList;

    RoutedEventHandlerInfoList();
    ~RoutedEventHandlerInfoList();

    void Clear();
};

class SUICORE_API EventHandlersStore
{
public:

    EventHandlersStore();
    virtual ~EventHandlersStore();

    RoutedEventHandlerInfoList* GetRoutedEventHandlerInfoList(const RoutedEvent*);

    RoutedEventHandlerInfoList* AddHandler(const RoutedEvent* routedEvent, Handler* handler, bool handledEventsToo);
    void RemoveHandler(const RoutedEvent* routedEvent, Handler* handler);

    void Clear();

protected:

    ValueDic<int, RoutedEventHandlerInfoList*> _entries;
};

}

#endif
