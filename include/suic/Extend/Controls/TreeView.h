// 华勤科技版权所有 2008-2022
// 
// 文件名：TreeView.h
// 功  能：实现树容器
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UITREEVIEW_H_
#define _UITREEVIEW_H_

#include <Extend/suiwgxconfig.h>
#include <Extend/Controls/TreeViewItem.h>
#include <Framework/Controls/RoutedPropertyChangedEventArg.h>

namespace suic
{

/// <summary>
/// 记录估算树子项大小。
/// </summary>
struct ContainerSize : public Object
{
    int size;
    int containerCount;

    ContainerSize(int val)
    {
        size = val;
        containerCount = 1;
    }

    bool IsCloseTo(ContainerSize cSize)
    {
        return (size == cSize.size);
    }
};

/// <summary>
/// TreeView。
/// </summary>
class SUIWGX_API TreeView : public ItemsControl
{
public:

    static DpProperty* SelectedItemProperty;
    static RoutedEvent* SelectedItemChangedEvent;

    static UncommonField<XArray>* CachedContainerSizeCount;
    static UncommonField<ContainerSize>* CachedEstimateContainerSize;

    static bool StaticInit();

    static void OnSelectedItemPropChanged(DpObject* d, DpPropChangedEventArg* e);

    TreeView();
    virtual ~TreeView();

    RTTIOfClass(TreeView)

    void SetItemIsExpanded(Object* item, bool val);

    void ScrollIntoView(Object* item, bool atTopOfViewport);
    void ScrollToEdge(bool bEnd);

    void AddSelectedItemChanged(RoutedPropChangedEventHandler* handler);
    void RemoveSelectedItemChanged(RoutedPropChangedEventHandler* handler);

    void SetSelectedItem(Object* selItem);
    Object* GetSelectedItem();

    void RegisterContainerSize(int size);
    Size EstimatedContainerSize(bool isHorizontal);

    void DoCollapsedAndSelection(TreeViewItem* pTreeItem);
    void ChangeSelection(Object* data, TreeViewItem *container, bool selected, bool bNeedSet);

    Element* GetHeaderContainerFromItem(Object* item);

public:

    void OnKeyDown(suic::KeyboardEventArg* e);
    void OnGotFocus(RoutedEventArg* e);

    void OnMouseLeftButtonDown(MouseButtonEventArg* e);

    bool IsItemItsOwnContainer(Object* item);
    Element* GetContainerForItemOverride();
    void PrepareContainerForItemOverride(DpObject* element, ItemEntry* item);

protected:

    void ItemSetIsSelected(ItemEntry* item, bool value);
    void HandleMouseButtonDown(TreeViewItem* item);
    void UpdateSelectedValue(Object* selectedItem);
    XArray* GetCachedContainerSizeCount();

    ItemEntry* FindPrevItem(TreeViewItem* pItem);
    ItemEntry* FindPrevLastItem(ItemEntry* pItem);
    ItemEntry* FindNextItem(TreeViewItem* pItem, bool bFirst);

    void ScrollToItem(ItemEntry* pItem, bool bUp);

    bool ComputeItemOffset(ItemCollection* items, bool bHori, Object* item, int& iOffset, int& iItemSize);

    void OnPrepareContainer();
    void OnItemsChanged(NotifyCollChangedEventArg* e);
    void ClearContainerForItemOverride(DpObject* container, ItemEntry* item);
    
    virtual void OnSelectedItemChanged(RoutedPropChangedEventArg* e);

private:

    WeakRef _selCachedItem;
    bool _isSelChangeActive;
    TreeViewItem* _selContainer;
    ContainerSize _containerSize;

    friend class TreeViewItem;
};

}

#endif
