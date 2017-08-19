// 华勤科技版权所有 2010-2011
// 
// 文件名：Expander.h
// 功  能：可扩展控件
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
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
/// 扩展按钮实现
/// </summary>
class SUICORE_API Expander : public HeaderedContentControl
{
public:

    static RoutedEvent* ExpandedEvent;
    static RoutedEvent* CollapsedEvent;

    static DpProperty* ExpandDirectionProperty;
    // 是否扩展属性，Boolean类型
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
