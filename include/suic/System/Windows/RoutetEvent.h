// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����UIRouteEvent.h
// ��  �ܣ�������Ŀ��·���¼����ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIROUTEEVENT_H_
#define _UIROUTEEVENT_H_

#include <System/Windows/Object.h>
#include <System/Windows/Delegate.h>

namespace suic
{

/// <summary>
/// ����Ԫ�صĴ���ʽ��Tunneling�����ע���˻����¼���ϵͳ�¼�����ģ��
/// ��������¼�����ֱ���¼����������
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
