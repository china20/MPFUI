// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ItemsControl.h
// ��  �ܣ�ʵ��Item�����Ļ���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UIITEMSCONTROL_H_
#define _UIITEMSCONTROL_H_

#include <Framework/Controls/Panel.h>
#include <Framework/Controls/Control.h>
#include <Framework/Controls/ScrollViewer.h>
#include <Framework/Controls/ItemCollection.h>
#include <Framework/Controls/ItemNavigateArg.h>
#include <Framework/Controls/HierarchicalItem.h>
#include <Framework/Controls/DataTemplateSelector.h>
#include <Framework/Controls/ItemsPanelTemplate.h>

#include <Framework/Presenter/ItemContainerGenerator.h>
#include <System/Resources/StyleSelector.h>

namespace suic
{

typedef Array<Size> SizeArray;

/// <summary>
/// ItemsControl,������Ļ��ࡣ
/// </summary>
class SUICORE_API ItemsControl : public Control, public IAddChild
{
public:

    static DpProperty* HasItemsProperty;
    static DpProperty* IsGroupingProperty;
    static DpProperty* GroupStyleSelectorProperty;
    static DpProperty* AlternationIndexProperty;
    static DpProperty* ItemContainerStyleProperty;
    static DpProperty* ItemContainerStyleSelectorProperty;
    static DpProperty* ItemsPanelProperty;
    static DpProperty* ItemsSourceProperty;
    static DpProperty* ItemTemplateProperty;
    static DpProperty* ItemTemplateSelectorProperty;

    static bool StaticInit();
    static ItemsControl* GetItemsOwner(DpObject* element);

    static void OnItemContainerStylePropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnItemsPanelPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnItemsSourcePropChanged(DpObject* d, DpPropChangedEventArg* e);

    static void OnGroupStyleSelectorPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnItemTemplateSelectorPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnItemContainerStyleSelectorPropChanged(DpObject* d, DpPropChangedEventArg* e);

    static void OnHasItemsPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnItemTemplatePropChanged(DpObject* d, DpPropChangedEventArg* e);

    static void OnGotKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e);
    static void OnLostKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e);

    static void OnItemSourceNamePropChanged(DpObject* d, DpPropChangedEventArg* e);

    ItemsControl();
    virtual ~ItemsControl();

    RTTIOfAbstractClass(ItemsControl)

public:

    static HierarchicalItem* HiItemFromEntry(ItemEntry* itemEntry)
    {
        return RTTICast<HierarchicalItem>(itemEntry->GetItem());
    }

    static void ClearBeforeLinkContainer(Element* container);
    static ItemsControl* ItemsControlFromItemContainer(DpObject* container);

    bool HasItems();

    ScrollViewer* GetScrollHost();
    ItemContainerGenerator* GetItemContainerGenerator();

    void Reset();
    Panel* GetItemsHost();

    ItemEntry* GetFocusedItem();
    void SetFocusedItem(ItemEntry* item);

    bool IsGrouping();
    void SetIsGrouping(bool val);

    GroupStyleSelector* GetGroupStyleSelector();
    void SetGroupStyleSelector(GroupStyleSelector* val);

    GroupStyle* GetGroupStyle(ViewGroup* group, int level);

    bool IsInitPending();
    bool IsVirtualizing();

    bool IsLogicalVertical();
    bool IsLogicalHorizontal();

    int GetCount() const;
    Object* GetItem(int index) const;

    ItemEntry* ItemEntryFromItem(Object* item);
    ItemEntry* ItemEntryFromIndex(int index) const;

    int FindFromText(const String& name);
    int FindFromItem(const Object* item);

    ItemsPanelTemplate* GetItemsPanel();
    void SetItemsPanel(ItemsPanelTemplate* val);

    Style* GetItemContainerStyle();
    void SetItemContainerStyle(Style* val);

    ItemCollection* GetItemsSource();
    void SetItemsSource(ItemCollection* val);

    DataTemplate* GetItemTemplate();
    void SetItemTemplate(DataTemplate* val);

    DataTemplateSelector* GetItemTemplateSelector();
    void SetItemTemplateSelector(DataTemplateSelector* val);

    StyleSelector* GetItemContainerStyleSelector();
    void SetItemContainerStyleSelector(StyleSelector* style);

    void ReadItemSize(ItemEntry* item, bool bHori, int index, Size& size);
    void StoreItemSize(ItemEntry* item, int index, Size size);

    bool IsOnCurrentPage(Element* elem, bool fullyVisible);

    /// <summary>
    ///  ��������
    /// </summary>
    /// <param name="value">����</param>
    /// <returns>��</returns>
    void AddChild(Object* value);
    void AddText(String text);
    void RemoveChild(Object* child);

    void ClearItemsControl(ItemEntry* item);
    void DoAutoScroll();
    void DoAutoScroll(Object* startingItem);

    /// <summary>
    ///  ���û������ȡ��Ӧ��ʵ�ʿؼ�����
    ///  ����������ܻ�ȡ��Ϊ��
    /// </summary>
    /// <param name="item">����</param>
    /// <returns>ʵ�ʿؼ�����</returns>
    Element* GetContainerFromItem(Object* item);

    /// <summary>
    ///  ����ָ�����������һ���ǿؼ�����)
    ///  ������������Ҫ��ȡʵ�ʵ�ҵ�����ʹ��GetDataContext��ȡ
    /// </summary>
    /// <param name="point">�����</param>
    /// <returns>����</returns>
    Element* HitTestItem(Point point);

    /// <summary>
    ///  ��ȡ�����Ӧ���������������Ѿ��Է��ص��������������ref
    ///  ���ü�����1���ⲿʹ�ú���Ҫ����unref�ͷ����ü���
    /// <param name="point">�������</param>
    /// <returns>��������</returns>
    Element* GetContainerForItem(Object* item);
    void ClearContainerForItem(DpObject* element, ItemEntry* item);
    
    virtual void OnPrepareContainer();
    virtual bool IsItemItsOwnContainer(Object* item);

public:

    virtual void OnItemContainerStyleChanged(Style* oldItemContainerStyle, Style* newItemContainerStyle);
    virtual void OnItemContainerStyleSelectorChanged(StyleSelector* oldItemContainerStyleSelector, StyleSelector* newItemContainerStyleSelector);

    virtual void OnItemsChanged(NotifyCollChangedEventArg* e);
    virtual void OnItemsPanelChanged(ItemsPanelTemplate* oldItemsPanel, ItemsPanelTemplate* newItemsPanel);
    virtual void OnItemsSourceChanged(ItemCollection* oldValue, ItemCollection* newValue);

    virtual void OnItemTemplateChanged(DataTemplate* oldItemTemplate, DataTemplate* newItemTemplate);
    virtual void OnItemTemplateSelectorChanged(DataTemplateSelector* oldItemTemplateSelector, DataTemplateSelector* newItemTemplateSelector);

    Object* OnBringItemIntoView(Object* arg);

    void OnItemCollectionChanged(Object* sender,  NotifyCollChangedEventArg* e);
    void OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate);

public:

    void PrepareItemsControl(ItemEntry* item, ItemsControl* parentItemsControl);
    void RemoveItemContainerStyle(DpObject* container);

    virtual bool ShouldApplyItemContainerStyle(DpObject* container, ItemEntry* item);
    virtual void PrepareContainerForItemOverride(DpObject* element, ItemEntry* item);

protected:

    void ApplyItemContainerStyle(DpObject* container, ItemEntry* item);
    ItemEntry* RecursiveFindItemEntry(ItemCollection* items, Object* item);

    virtual bool IsHostForItemContainer(DpObject* container);
    virtual Size EstimatedContainerSize(bool isHorizontal);
    virtual void PrepareItemContainer(DpObject* container, ItemEntry* item); 

    /// <summary>
    ///  ���������Ӧ��������������ʵ��ֱ��new������Ҫ����ref�������ü���
    ///  ����������Դй©
    /// <returns>��������</returns>
    virtual Element* GetContainerForItemOverride();
    virtual void ClearContainerForItemOverride(DpObject* element, ItemEntry* item);

    friend class ItemContainerGenerator;

protected:

    void BeginInit();
    void EndInit();

    void SetItemsHost(Panel* val);
    void SetItemsSourceInternal(ItemCollection* newColl);

    void SetFocusItem(Object* item);
    Object* FindFocusable(int iStart, int dir, int& outIndex);

    int ComputeOffsetFromItem(Object* item, int& offset, int& itemSize);
    int ComputeOffsetFromIndex(int index, int& offset, int& itemSize);

    bool MakeVisible(int index);
    bool MakeVisible(int index, bool atTopOfViewport);
    bool MakeVisible(Object* item, int offset, int iItemSize, bool atTopOfViewport);

    bool IsOnCurrentPage(Element* elem, AxisDirection axis, bool fullyVisible);
    void SetItemValuesOnContainer(DpObject* container, ItemEntry* item, int* dpIndices);

    void NavigateToEnd(ItemNavigateArg* e);
    void NavigateToItem(Object* item, int offset, int itemLen, ItemNavigateArg* e, bool alwaysAtTopOfViewport);

    friend class Panel;

private:

    void CreateItemCollectionAndGenerator();

private:

    // ��ǰ��ȡ������
    WeakRef _focusedItem;

    ItemCollection* _items;
    Style* _itemContainerStyle;
    DataTemplate* _itemTemplate;
    DataTemplateSelector* _itemTempSelector;
    ItemContainerGenerator* _itemContainerGenerator;

    ObservableCollection _groupStyles;

    // ����������
    Panel* _itemsHost;
    // ���������ͼ
    ScrollViewer* _scrollHost;
    bool _isVirtualizing;
    bool _isCachedScrollHost;
};

typedef suic::shared<ItemsControl> ItemsControlPtr;

inline Style* ItemsControl::GetItemContainerStyle()
{
    return RTTICast<Style>(GetValue(ItemContainerStyleProperty));
}

inline void ItemsControl::SetItemContainerStyle(Style* val)
{
    SetValue(ItemContainerStyleProperty, val);
}

inline ItemsPanelTemplate* ItemsControl::GetItemsPanel()
{
    return RTTICast<ItemsPanelTemplate>(GetValue(ItemsPanelProperty));
}

inline void ItemsControl::SetItemsPanel(ItemsPanelTemplate* val)
{
    SetValue(ItemsPanelProperty, val);
}

inline bool ItemsControl::HasItems()
{
    return ReadItemFlags(HasItemsFlag);
}

inline ItemContainerGenerator* ItemsControl::GetItemContainerGenerator()
{
    return _itemContainerGenerator;
}

inline Panel* ItemsControl::GetItemsHost()
{
    return _itemsHost;
}

inline ItemEntry* ItemsControl::GetFocusedItem()
{
    return _focusedItem.GetElement<ItemEntry>();
}

inline int ItemsControl::GetCount() const
{
    if (NULL != _items)
    {
        return _items->GetCount();
    }
    else
    {
        return 0;
    }
}

inline Object* ItemsControl::GetItem(int index) const
{
    if (NULL != _items)
    {
        return _items->GetItem(index);
    }
    else
    {
        return 0;
    }
}

inline ItemEntry* ItemsControl::ItemEntryFromIndex(int index) const
{
    if (NULL != _items)
    {
        return _items->GetItemEntry(index);
    }
    else
    {
        return 0;
    }
}

}

#endif
