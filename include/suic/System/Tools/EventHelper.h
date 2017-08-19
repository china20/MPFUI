// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uieventhelper.h>
// ��  �ܣ����Ŀ���¼������ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
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
