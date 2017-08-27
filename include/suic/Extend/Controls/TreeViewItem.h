// 华勤科技版权所有 2008-2022
// 
// 文件名：HeaderedItemsControl.h
// 功  能：实现树容器项
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UITREEVIEWITEM_H_
#define _UITREEVIEWITEM_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/HeaderedItemsControl.h>
#include <Framework/Controls/VisualizeStackPanel.h>

namespace suic
{

class TreeView;
class IProvideStackingSize;

/// <summary>
/// TreeViewItem。
/// </summary>
class SUIWGX_API TreeViewItem : public HeaderedItemsControl
                               , public IProvideStackingSize
{
public:

    static RoutedEvent* ExpandedEvent;
    static RoutedEvent* CollapsedEvent;

    static DpProperty* IndentProperty;
    static DpProperty* IndentValueProperty;
    static DpProperty* ClickExpandProperty;
    static DpProperty* IsExpandedProperty;
    static DpProperty* IsSelectedProperty;
    static DpProperty* IsSelectionActiveProperty;

    static bool StaticInit();
    static void OnIndentChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIndentValueChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIsExpandedChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIsSelectedChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMouseButtonDown(Element* sender, MouseButtonEventArg* e);

    TreeViewItem();
    virtual ~TreeViewItem();

    RTTIOfClass(TreeViewItem)

    bool IsExpanded();
    void SetIsExpanded(bool val);

    bool IsSelected();
    void SetIsSelected(bool val);

    bool IsSelectionActive();
    void SetIsSelectionActive(bool val);

    int GetIndent() const;
    int GetIndentValue() const;

    TreeView* GetParentTreeView();
    TreeViewItem* GetParentTreeViewItem();
    ItemsControl* GetParentItemsControl();

    int GetHeaderSize(bool isHorizontal);
    Size EstimatedContainerSize(bool isHorizontal);

    void UpdateContainsSelection(bool selected);

public:

    bool IsItemItsOwnContainer(Object* item);
    Element* GetContainerForItemOverride();

    void PrepareContainerForItemOverride(DpObject* elem, ItemEntry* item);
    void ClearContainerForItemOverride(DpObject* container, ItemEntry* item);

    void PrepareHeaderedItemsControl(ItemEntry* item, ItemsControl* parentItemsControl);

protected:

    void Select(bool selected);
    void SetIsExpandedDynamic(bool val);

    Size ComputeRenderSize();

    Size OnMeasure(const Size& constraint);
    void OnItemsChanged(NotifyCollChangedEventArg* e);

    void OnMouseLeftButtonDown(MouseButtonEventArg* e);

    void OnGotFocus(RoutedEventArg* e);

    virtual void OnCollapsed(RoutedEventArg* e);
    virtual void OnExpanded(RoutedEventArg* e);

    friend class TreeView;

private:

    int _indent; 
    int _indentValue;

    Byte _pending;
    WeakRef _parentTreeView;
};

inline int TreeViewItem::GetIndent() const
{
    return _indent;
}

inline int TreeViewItem::GetIndentValue() const
{
    return _indentValue;
}

inline bool TreeViewItem::IsExpanded()
{
    return ReadItemFlags(IsExpandedFlag);
}

inline void TreeViewItem::SetIsExpanded(bool val)
{
    SetValue(IsExpandedProperty, BOOLTOBOOLEAN(val));
}

inline bool TreeViewItem::IsSelected()
{
    return GetValue(IsSelectedProperty)->ToInt() != 0;
}

inline void TreeViewItem::SetIsSelected(bool val)
{
    SetValue(IsSelectedProperty, BOOLTOBOOLEAN(val));
}

inline bool TreeViewItem::IsSelectionActive()
{
    return GetValue(IsSelectionActiveProperty)->ToInt() != 0;
}

inline void TreeViewItem::SetIsSelectionActive(bool val)
{
    SetValue(IsSelectionActiveProperty, BOOLTOBOOLEAN(val));
}

}

#endif
