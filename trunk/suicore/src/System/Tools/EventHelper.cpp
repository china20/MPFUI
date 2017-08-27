
#include <System/Tools/EventHelper.h>
#include <System/Windows/EventManager.h>
#include <System/Windows/EventHandler.h>

namespace suic
{

RoutedEvent* EventHelper::
RegisterRoutedEvent(const String& name, RoutingStrategyType routingStrategy, RTTIOfInfo* handlerType, RTTIOfInfo* ownerType)
{
    return EventControl::RegisterRoutedEvent(name, routingStrategy, handlerType, ownerType);
}

void EventHelper::
RegisterClassHandler(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent, Handler* handler,bool handledToo)
{
    EventControl::RegisterClassHandler(ownerType, routedEvent, handler, handledToo);
}

void EventHelper::
RegisterAttachedHandler(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent, Handler* handler,bool handledToo)
{
    EventControl::RegisterAttachedHandler(ownerType, routedEvent, handler, handledToo);
}

EventHandlersStore* EventHelper::GetClassEventHandlersStore(RTTIOfInfo* ownerType)
{
    return EventControl::GetClassEventHandlersStore(ownerType);
}

RoutedEventHandlerInfoList* EventHelper::GetClassRoutedEventHandlerInfo(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent)
{
    return EventControl::GetClassRoutedEventHandlerInfo(ownerType, routedEvent);
}

RoutedEvent* EventHelper::FindRoutedEvent(const String& name, RTTIOfInfo* ownerType)
{
    return EventControl::FindRoutedEvent(name, ownerType);
}

void EventHelper::
RemoveClassHandler(RTTIOfInfo* ownerType, const RoutedEvent* routedEvent, Handler* handler)
{
    EventControl::RemoveClassHandler(ownerType, routedEvent, handler);
}

}
