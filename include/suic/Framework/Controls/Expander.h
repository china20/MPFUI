// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Expander.h
// ��  �ܣ�����չ�ؼ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UIEXPANDER_H_
#define _UIEXPANDER_H_

#include <Framework/Controls/ToggleButton.h>
#include <Framework/Controls/HeaderedContentControl.h>

namespace suic
{

enum ExpandDirection
{
    edLeft,
    edRight,
    edUp,
    edDown,
};

/// <summary>
/// ��չ��ťʵ��
/// </summary>
class SUICORE_API Expander : public HeaderedContentControl
{
public:

    static RoutedEvent* ExpandedEvent;
    static RoutedEvent* CollapsedEvent;

    static DpProperty* ExpandDirectionProperty;
    // �Ƿ���չ���ԣ�Boolean����
    static DpProperty* IsExpandedProperty;

    static bool StaticInit();
    static void OnIsExpandedChanged(DpObject* d, DpPropChangedEventArg* e);

    Expander();
    virtual ~Expander();

    RTTIOfClass(Expander)

    void AddExpanded(RoutedEventHandler* handler);
    void AddCollapsed(RoutedEventHandler* handler);

    ExpandDirection GetExpandDirection();
    void SetExpandDirection(ExpandDirection val);

    bool IsExpanded();
    void Expand();
    void Collapse();

protected:

    virtual void OnCollapsed();
    virtual void OnExpanded();
};

}

#endif
