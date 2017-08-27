// 华勤科技版权所有 2008-2022
// 
// 文件名：Selector.h
// 功  能：支持项选择的基类的基类。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
// 
// ============================================================================

#ifndef _UISELECTOR_H_
#define _UISELECTOR_H_

#include <Framework/Controls/ItemsControl.h>
#include <System/Windows/SelectionChangedEventArg.h>

namespace suic
{

class Selector;

enum SelectionMode
{
    eSingle,
    eMultiple,
    eExtended
};

enum SelectPendingType
{
    eSelectUnknown,
    eSelectInvoke,
};

struct SUICORE_API SelectionItems
{
    SelectionItems();
    ~SelectionItems();

    int GetCount() const;
    void Add(ItemEntry* item);
    bool Remove(ItemEntry* item);
    bool RemoveItem(Object* item);
    bool Contains(ItemEntry* item);
    void Clear();

    int IndexOf(Object* item);

    ItemEntry* GetItem(int index) 
    { 
        return items.GetItem(index); 
    }

    ItemEntry* operator[](int index);

private:

    Array<ItemEntry*> items;
    ObjectDic<Object*, int> indexes;
};

struct SUICORE_API SelectionChanger
{
    bool active;
    Selector* owner;
    SelectionItems selItems;
    SelectionItems unselItems;

    SelectionChanger(Selector* o);

    bool Begin();
    void Commit();

    void Clear();
    void Validate();

    bool IsActive() const;

    bool SelectItem(ItemEntry* item, bool bSel);
    void SelectJustThisItem(ItemEntry* item, bool assumeInItemsCollection);

private:

    void ApplyCanSelectMultiple();
    void CreateDeltaSelectionChange(XArray& unselectedItems, XArray& selectedItems);
};

inline bool SelectionChanger::IsActive() const
{
    return active;
}

class SUICORE_API Selector : public ItemsControl
{
public:

    RTTIOfAbstractClass(Selector)

    static DpProperty* IsSelectedProperty;
    static DpProperty* IsSelectionActiveProperty;

    static DpProperty* SelectedIndexProperty;
    static DpProperty* SelectedItemProperty;
    static DpProperty* SelectedValueProperty;

    static RoutedEvent* SelectionChangedEvent;
    static RoutedEvent* SelectedEvent;
    static RoutedEvent* UnselectedEvent;

    static bool StaticInit();

    static void SelectContainer(Element* container, bool val);

    static bool ValidateSelectedIndex(DpObject* d, Object* val);
    static void OnSelectedIndexChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnSelectedItemChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnSelectedValueChanged(DpObject* d, DpPropChangedEventArg* e);

    static void OnSelected(DpObject* sender, RoutedEventArg* e);
    static void OnUnselected(DpObject* sender, RoutedEventArg* e);

    Selector();
    ~Selector();

    bool CanSelectMultiple();

    int GetSelectedIndex();
    void SetSelectedIndex(int itemIndex);

    Object* GetSelectedItem();
    void SetSelectedItem(Object* item);

    void GetSelectedItems(OAutoColl& selItems);
    SelectionChanger* GetSelectionChanger();

    void AddSelectionChanged(SelectionChangedEventHandler* handler);
    void RemoveSelectionChanged(SelectionChangedEventHandler* handler);

public:

    virtual void OnItemsChanged(NotifyCollChangedEventArg* e);
    virtual void OnSelectionChanged(SelectionChangedEventArg* e);

    void SetSelectedHelper(ItemEntry* item, bool selected);
    void NotifyIsSelectedChanged(FrameworkElement* container, bool selected);

protected:

    ItemEntry* GetSelItemEntry();
    void UpdateSelectedItems();
    void InvokeSelectionChanged(XArray* unselectedItems, XArray* selectedItems);

    bool ItemGetIsSelected(ItemEntry* item);
    void ItemSetIsSelected(ItemEntry* item, bool value);

    void PrepareItemContainer(DpObject* container, ItemEntry* item);
    void ClearContainerForItemOverride(DpObject* elem, ItemEntry* item);

    virtual void OnItemsSourceChanged(ItemCollection* oldValue, ItemCollection* newValue);
    virtual void OnClearItem(DpObject* elem, ItemEntry* item);

    virtual void SelectAllImpl();
    virtual void UnselectAllImpl();
    
protected:

    int _selectMode;
    Byte _isSelectPending;
    SelectionItems _selectedItems;
    SelectionChanger* _selectionChanger;

    friend struct SelectionChanger;
};

inline SelectionChanger* Selector::GetSelectionChanger()
{
    return _selectionChanger;
}

}

#endif
