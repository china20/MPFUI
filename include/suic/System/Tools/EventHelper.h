// 华勤科技版权所有 2010-2011
// 
// 文件名：uieventhelper.h>
// 功  能：核心框架事件工具类。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UIEVENTHELPER_H_
#define _UIEVENTHELPER_H_

#include <System/Windows/Object.h>
#include <System/Windows/Element.h>

namespace suic
{

class SUICORE_API EventHelper
{
public:

    static RoutedEvent* RegisterRoutedEvent(const String& name, RoutingStrategyType routingStrategy, RTTIOfInfo* handlerType,RTTIOfInfo* ownerType);
    static void RegisterClassHandler(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent, Handler* handler,bool handledToo);
    static void RegisterAttachedHandler(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent, Handler* handler,bool handledToo);

    static RoutedEvent* FindRoutedEvent(const String& name, RTTIOfInfo* ownerType);
    static void RemoveClassHandler(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent, Handler* handler);

    static EventHandlersStore* GetClassEventHandlersStore(RTTIOfInfo* ownerType);
    static RoutedEventHandlerInfoList* GetClassRoutedEventHandlerInfo(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent);

protected:

    EventHelper() {}
};

};

#endif
