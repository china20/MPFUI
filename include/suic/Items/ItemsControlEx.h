// 华勤科技版权所有 2010-2011
// 
// 文件名：GroupItem.h
// 功  能：ItemsControl分组对象
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIITEMCONTROLEX_H_
#define _UIITEMCONTROLEX_H_

#include <Framework/Controls/Panel.h>
#include <Framework/Controls/Content.h>
#include <Framework/Controls/ItemSource.h>
#include <Framework/Controls/ScrollViewer.h>

#include <Framework/Controls/ItemsControl.h>

#include <System/Resources/Style.h>
#include <System/Windows/RoutedEventArg.h>

#include <Framework/Items/Enumerable.h>
#include <Framework/Items/ItemCollectionEx.h>
#include <Framework/Items/IGeneratorHost.h>
#include <Framework/Items/CollectionViewGroup.h>
#include <Framework/Items/ItemContainerGeneratorEx.h>

namespace suic
{
    
class ItemsControlEx : public suic::Control 
                     , public IAddChild
                     , public IGeneratorHost
{
private:
    
    WeakRef _focusedItem;
    Array<GroupStyle*> _groupStyle;
    
    Panel* _itemsHost;
    ItemCollectionEx* _items;
    ScrollViewer* _scrollHost;
    ItemContainerGeneratorEx* _itemContainerGenerator;

public:

    static DpProperty* AlternationCountProperty;
    static DpProperty* AlternationIndexProperty;
    static DpProperty* DisplayMemberPathProperty;
    static DpProperty* GroupStyleSelectorProperty;
    static DpProperty* HasItemsProperty;
    static DpProperty* IsGroupingProperty;
    static DpProperty* IsTextSearchCaseSensitiveProperty;
    static DpProperty* IsTextSearchEnabledProperty;
    static DpProperty* ItemBindingGroupProperty;
    static DpProperty* ItemContainerStyleProperty;
    static DpProperty* ItemContainerStyleSelectorProperty;
    static DpProperty* ItemsPanelProperty;
    static DpProperty* ItemsSourceProperty;
    static DpProperty* ItemTemplateProperty;
    static DpProperty* ItemTemplateSelectorProperty;

    static void OnItemContainerStyleChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnItemContainerStyleSelectorChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnDisplayMemberPathChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnGotFocus(Object* sender, KeyboardFocusEventArg* e);
    static void OnGroupStyleSelectorChanged(DpObject* d, DpPropChangedEventArg* e);

    static void OnItemsPanelChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnItemsSourceChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnItemTemplateChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnItemTemplateSelectorChanged(DpObject* d, DpPropChangedEventArg* e);
    static void SetAlternationIndex(DpObject* d, int value);

    static int GetAlternationIndex(DpObject* element);
    static ItemsPanelTemplate* GetDefaultItemsPanelTemplate();
    static ItemsControlEx* GetItemsOwner(DpObject* element);
    static ItemsControlEx* ItemsControlFromItemContainer(DpObject* container);
    static DpObject* ContainerFromElement(ItemsControlEx* itemsControl, DpObject* d);
    static bool IsContainerForItemsControl(DpObject* element, ItemsControlEx* itemsControl);

    static bool StaticInit();

    RTTIOfClass(ItemsControlEx)

    void BeginInit();
    void EndInit();

    void SetIsGrouping(bool isGrouping);
    bool ShouldSerializeGroupStyle();

    int GetAlternationCount();
    void SetAlternationCount(int val);

    String GetDisplayMemberPath();
    void SetDisplayMemberPath(String val);

    bool IsTextSearchCaseSensitive();
    void SetIsTextSearchCaseSensitive(bool val);

    bool IsTextSearchEnabled();
    void SetIsTextSearchEnabled(bool val);

    Object* GetFocusedItem();

    Array<GroupStyle*>& GetGroupStyle();
    GroupStyleSelector* GetGroupStyleSelector();

    void SetGroupStyleSelector(GroupStyleSelector* val);

    bool HasItems();
    bool IsGrouping();
    bool IsInitPending() const;

    ItemContainerGeneratorEx* GetItemContainerGenerator();
    Style* GetItemContainerStyle();
    void SetItemContainerStyle(Style* val);

    StyleSelector* GetItemContainerStyleSelector();
    void SetItemContainerStyleSelector(StyleSelector* val);

    ItemCollectionEx* GetItems();

    Panel* GetItemsHost() const;
    void SetItemsHost(Panel* val);

    ItemsPanelTemplate* GetItemsPanel();
    void SetItemsPanel(ItemsPanelTemplate* val);

    IEnumerable* GetItemsSource();
    void SetItemsSource(IEnumerable* val);

    DataTemplate* GetItemTemplate();
    void SetItemTemplate(DataTemplate* val);

    DataTemplateSelector* GetItemTemplateSelector();
    void SetItemTemplateSelector(DataTemplateSelector* val);

    ItemCollectionEx* GetView();

    ScrollViewer* GetScrollHost();

    void ApplyItemContainerStyle(DpObject* container, Object* item);
    DpObject* ContainerFromElement(DpObject* d);

protected:

    static void ClearAlternationIndex(DpObject* d);

    void AddChild(Object* value);
    void AddText(String text);

    bool IsVirtualizing();

    bool IsLogicalHorizontal();
    bool IsLogicalVertical();

    void ClearItemsControl(Object* item);
    void DoAutoScroll();
    void DoAutoScroll(Object* startingItem);

    Object* GetItemOrContainerFromContainer(DpObject* container);
    bool MakeVisible(int index, bool alwaysAtTopOfViewport, bool alignMinorAxisToo);

    void NavigateByLine(FocusNavDirection direction, ItemNavigateArg& itemNavigateArgs);
    void NavigateByLine(Object* startingItem, FocusNavDirection direction, ItemNavigateArg& e);
    void NavigateByLineInternal(Object* startingItem, FocusNavDirection direction, ItemNavigateArg& e);
    void NavigateByPage(FocusNavDirection direction, ItemNavigateArg& e);
    void NavigateByPage(Object* startingItem, FocusNavDirection direction, ItemNavigateArg& e);

    void NavigateByPageInternal(Object* startingItem, FocusNavDirection direction, ItemNavigateArg& e);
    void NavigateToEnd(ItemNavigateArg& e);
    void NavigateToFirstItemOnCurrentPage(Object* startingItem, FocusNavDirection direction, ItemNavigateArg& e);
    void NavigateToItem(Object* item, ItemNavigateArg& e);
    void NavigateToItem(Object* item, int itemIndex, ItemNavigateArg& e);

    void NavigateToItem(Object* item, ItemNavigateArg& e, bool alwaysAtTopOfViewport);
    void NavigateToItem(Object* item, int elementIndex, ItemNavigateArg& e, bool alwaysAtTopOfViewport);

    void NavigateToStart(ItemNavigateArg& e);

protected:

    virtual void ClearContainerForItemOverride(DpObject* d, Object* item);
    virtual void FocusItem(Object* item, ItemNavigateArg& e);
    virtual DpObject* GetContainerForItemOverride();
    virtual bool IsItemItsOwnContainerOverride(Object* item);

    virtual void OnItemsChanged(NotifyCollChangedEventArg* e);
    virtual void OnItemsPanelChanged(ItemsPanelTemplate* oldItemsPanel, ItemsPanelTemplate* newItemsPanel);
    virtual void OnDisplayMemberPathChanged(String oldDisplayMemberPath, String newDisplayMemberPath);
    virtual void OnGroupStyleSelectorChanged(GroupStyleSelector* oldGroupStyleSelector, GroupStyleSelector* newGroupStyleSelector);
    virtual void OnItemContainerStyleChanged(Style* oldItemContainerStyle, Style* newItemContainerStyle);
    virtual void OnItemContainerStyleSelectorChanged(StyleSelector* oldItemContainerStyleSelector, StyleSelector* newItemContainerStyleSelector);
    virtual void OnItemsSourceChanged(IEnumerable* oldValue, IEnumerable* newValue);
    virtual void OnItemTemplateChanged(DataTemplate* oldItemTemplate, DataTemplate* newItemTemplate);
    virtual void OnItemTemplateSelectorChanged(DataTemplateSelector* oldItemTemplateSelector, DataTemplateSelector* newItemTemplateSelector);
    virtual void PrepareContainerForItemOverride(DpObject* d, Object* item);
    virtual bool ShouldApplyItemContainerStyle(DpObject* container, Object* item);

private:

    void CheckTemplateSource();
    void CreateItemCollectionAndGenerator();

    Object* FindFocusable(int startIndex, int direction, int& foundIndex);
    Object* GetEncapsulatingItem(FrameworkElement* element);

    Object* GetFirstItemOnCurrentPage(Object* startingItem, FocusNavDirection direction, int& foundIndex);

    bool IsOnCurrentPage(Object* item, FocusNavDirection axis);
    bool IsOnCurrentPage(FrameworkElement* element, FocusNavDirection axis);
    bool IsOnCurrentPage(FrameworkElement* element, FocusNavDirection axis, bool fullyVisible);

    bool IsRTL(FrameworkElement* element);
    bool MakeVisible(int index);

    // 实现 IGeneratorHost接口
    void ClearContainerForItem(DpObject* container, Object* item);
    DpObject* GetContainerForItem(Object* item);
    GroupStyle* GetGroupStyle(CollectionViewGroup* group, int level);
    bool IsHostForItemContainer(DpObject* container);
    bool IsItemItsOwnContainer(Object* item);

    void PrepareItemContainer(DpObject* container, Object* item);

    void OnAlternationCountChanged(int oldAlternationCount, int newAlternationCount);
    void OnAlternationCountChanged(DpObject* d, DpPropChangedEventArg* e);

    Object* OnBringItemIntoView(Object* e);
    void OnGroupStyleChanged(Object* sender, NotifyCollChangedEventArg* e);
    void OnItemCollectionChanged(Object* sender, NotifyCollChangedEventArg* e);

    void OnKeyDown(KeyboardEventArg* e);
    void OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate);

    void OnTextInput(KeyboardEventArg* e);

    void PrepareItemsControl(Object* item, ItemsControlEx* parentItemsControl);
    void RemoveItemContainerStyle(DpObject* container);
    void UpdateDisplayMemberTemplateSelector();
};

}

#endif
