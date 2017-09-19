/////////////////////////////////////////////////////////////////////////
// Selector.cpp


#include <Framework/Controls/Selector.h>
#include <System/Windows/Binding.h>
#include <System/Windows/EventManager.h>
#include <System/Tools/EventHelper.h>

namespace suic
{

ImplementRTTIOfClass(Selector, ItemsControl)

DpProperty* Selector::IsSelectedProperty;
DpProperty* Selector::IsSelectionActiveProperty;
DpProperty* Selector::SelectedIndexProperty;
DpProperty* Selector::SelectedItemProperty;
DpProperty* Selector::SelectedValueProperty;

RoutedEvent* Selector::SelectionChangedEvent;
RoutedEvent* Selector::SelectedEvent;
RoutedEvent* Selector::UnselectedEvent;

//--------------------------------------------------------

SelectionItems::SelectionItems()
{
}

SelectionItems::~SelectionItems()
{
    Clear();
}

int SelectionItems::GetCount() const
{
    return items.GetCount();
}

void SelectionItems::Add(ItemEntry* item)
{
    items.Add(item);
    item->ref();
    indexes.Add(item->GetItem(), items.GetCount() - 1);
}

bool SelectionItems::Remove(ItemEntry* item)
{
    return RemoveItem(item->GetItem());
}

int SelectionItems::IndexOf(Object* item)
{
    int index = -1;
    for (int i = 0; i < items.GetCount(); ++i)
    {
        if (items[i]->GetItem() == item)
        {
            index = i;
            break;
        }
    }
    return index;
}

bool SelectionItems::RemoveItem(Object* item)
{
    int index = IndexOf(item);
    if (index != -1)
    {
        items[index]->unref();
        indexes.Remove(item);
        items.RemoveAt(index);
        return true;
    }
    return false;
}

bool SelectionItems::Contains(ItemEntry* item)
{
    return indexes.ContainsKey(item->GetItem());
}

void SelectionItems::Clear()
{
    for (int i = 0; i < items.GetCount(); ++i)
    {
        items[i]->unref();
    }
    indexes.Clear();
    items.Clear();
}

ItemEntry* SelectionItems::operator[](int index)
{
    return items.GetItem(index);
}

//--------------------------------------------------------

SelectionChanger::SelectionChanger(Selector* o)
{
    owner = o;
    active = false;
}

SelectionChanger::~SelectionChanger()
{
    Clear();
}

bool SelectionChanger::Begin()
{
    if (active)
    {
        return false;
    }
    else
    {
        active = true;
        selItems.Clear();
        unselItems.Clear();
        return true;
    }
}

void SelectionChanger::CreateDeltaSelectionChange(XArray& unselectedItems, XArray& selectedItems)
{
    int i = 0;
    ItemEntry* item = NULL;

    if ((unselItems.GetCount() > 0) || (selItems.GetCount() > 0))
    {
        // 
        // 从已选择项删除需要撤销的选项
        //
        for (i = 0; i < unselItems.GetCount(); ++i)
        {
            item = unselItems.GetItem(i);
            owner->ItemSetIsSelected(item, false);

            if (owner->_selectedItems.Contains(item))
            {
                owner->_selectedItems.Remove(item);
                unselectedItems.Add(item->GetItem());
            }
        }

        // 
        // 把选择项加到选择项集合列表
        //
        for (i = 0; i < selItems.GetCount(); ++i)
        {
            item = selItems.GetItem(i);
            owner->ItemSetIsSelected(item, true);
            if (!owner->_selectedItems.Contains(item))
            {
                owner->_selectedItems.Add(item);
                selectedItems.Add(item->GetItem());
            }
        }
    }
}

void SelectionChanger::ApplyCanSelectMultiple()
{
    if (!owner->CanSelectMultiple())
    {
        if (selItems.GetCount() == 1)
        {
            unselItems.Clear();
            for (int i = 0; i < owner->_selectedItems.GetCount(); ++i)
            {
                // 记录未选择项
                ItemEntry* unselItem = owner->_selectedItems.GetItem(i);
                unselItems.Add(unselItem);
            }
        }
        else if ((owner->_selectedItems.GetCount() > 1) && 
            (owner->_selectedItems.GetCount() != (unselItems.GetCount() + 1)))
        {
            unselItems.Clear();

            for (int i = 1; i < owner->_selectedItems.GetCount(); ++i)
            {
                ItemEntry* item = owner->_selectedItems.GetItem(i);
                unselItems.Add(item);
            }
        }
    }
}

void SelectionChanger::Commit()
{
    XArray unselectedItems;
    XArray selectedItems;

    unselectedItems.setAutoDelete(false);
    selectedItems.setAutoDelete(false);

    // 检测多选逻辑
    ApplyCanSelectMultiple();
    CreateDeltaSelectionChange(unselectedItems, selectedItems);
    owner->UpdateSelectedItems();

    Clear();

    if ((unselectedItems.GetCount() > 0) || (selectedItems.GetCount() > 0))
    {
        owner->InvokeSelectionChanged(&unselectedItems, &selectedItems);
        //owner->UpdateLayout();
    }
}

void SelectionChanger::Clear()
{
    active = false;

    if (selItems.GetCount() > 0)
    {
        selItems.Clear();
    }
    if (unselItems.GetCount() > 0)
    {
        unselItems.Clear();
    }
}

void SelectionChanger::Validate()
{
    Begin();
    Commit();
}

bool SelectionChanger::SelectItem(ItemEntry* item, bool bSel)
{
    ItemEntry* ditem = item;

    if (bSel)
    {
        // 选择项不在未选择集合里
        if (!unselItems.Remove(ditem))
        {
            // 已经选择
            if (owner->_selectedItems.Contains(ditem))
            {
                return false;
            }

            // 防止重复选择
            if (selItems.Contains(ditem))
            {
                return false;
            }

            // 如果是单选，取消之前的所有选择
            if (!owner->CanSelectMultiple())
            {
                selItems.Clear();
            }

            selItems.Add(ditem);
        }
    }
    else
    {
        // 未在选择集合里
        if (!selItems.Remove(ditem))
        {
            // 一定是在已选择集合里才能做未选择操作
            if (!owner->_selectedItems.Contains(ditem))
            {
                return false;
            }

            // 已经加到未选择集合里
            if (unselItems.Contains(ditem))
            {
                return false;
            }

            unselItems.Add(ditem);
        }
    }

    return true;
}

void SelectionChanger::SelectJustThisItem(ItemEntry* item, bool assumeInItemsCollection)
{
    Begin();

    try
    {
        bool flag = false;
        for (int i = owner->_selectedItems.GetCount() - 1; i >= 0; i--)
        {
            ItemEntry* pEntry = owner->_selectedItems[i];
            if (item->GetItem() != pEntry->GetItem())
            {
                SelectItem(pEntry, false);
            }
            else
            {
                flag = true;
            }
        }
        if ((!flag && (item != NULL)))
        {
            SelectItem(item, true);
        }
    }
    catch (...)
    {
        ;
    }

    Commit();
}

//--------------------------------------------------------

bool Selector::ValidateSelectedIndex(DpObject* d, Object* val)
{
    if (val->ToInt() < -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Selector::StaticInit()
{
    if (NULL == IsSelectedProperty)
    {
        IsSelectedProperty = DpProperty::Register(_T("IsSelected"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));
        IsSelectionActiveProperty = DpProperty::Register(_T("IsSelectionActive"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));
        SelectedIndexProperty = DpProperty::Register(_T("SelectedIndex"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetNegInt(-1), PropMetadataOptions::AffectsRender, &Selector::OnSelectedIndexChanged)
            , Selector::ValidateSelectedIndex);
        SelectedItemProperty = DpProperty::Register(_T("SelectedItem"), RTTIType(), Object::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender, &Selector::OnSelectedItemChanged));
        SelectedValueProperty = DpProperty::Register(_T("SelectedValue"), RTTIType(), Object::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender));
        //SelectedItemsProperty = DpProperty::Register(_T("SelectedItems"), RTTIType(), XArray::RTTIType()
        //    , DpPropMemory::GetPropMeta(new XArray(), PropMetadataOptions::AffectsNone));

        SelectedEvent = EventHelper::RegisterRoutedEvent(_U("Selected"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
        SelectionChangedEvent = EventHelper::RegisterRoutedEvent(_U("SelectionChanged"), RoutingStrategy::Bubble, SelectionChangedEventHandler::RTTIType(), RTTIType());
        UnselectedEvent = EventHelper::RegisterRoutedEvent(_U("Unselected"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());

        EventHelper::RegisterClassHandler(RTTIType(), SelectedEvent, new RoutedEventHandler(&Selector::OnSelected), true);
        EventHelper::RegisterClassHandler(RTTIType(), UnselectedEvent, new RoutedEventHandler(&Selector::OnUnselected), true);
    }
}

void Selector::SelectContainer(Element* container, bool val)
{
    container->SetValue(IsSelectedProperty, BOOLTOBOOLEAN(val));
}

void Selector::InvokeSelectionChanged(XArray* unselectedItems, XArray* selectedItems)
{
    SelectionChangedEventArg e(unselectedItems, selectedItems);
    e.SetSource(this);
    e.SetRoutedEvent(SelectionChangedEvent);
    OnSelectionChanged(&e);
}

ItemEntry* Selector::GetSelItemEntry()
{
    ItemEntry* pItem = NULL;
    Object* selItem = GetSelectedItem();
    if (selItem != NULL)
    {
        int index = _selectedItems.IndexOf(selItem);
        pItem = _selectedItems.GetItem(index);
    }
    return pItem;
}

void Selector::GetSelectedItems(OAutoColl& selItems)
{
    for (int j = 0; j < _selectedItems.GetCount(); ++j)
    {
        Object* realItem = _selectedItems.GetItem(j)->GetItem();
        selItems.AddItem(realItem);
    }
}

void Selector::SelectAllImpl()
{

}

void Selector::UnselectAllImpl()
{

}

void Selector::UpdateSelectedItems()
{
    int iCount = _selectedItems.GetCount();
    if (iCount > 0)
    {
        ItemEntry* selItem = _selectedItems.GetItem(iCount - 1);
        int selIndex = GetItemsSource()->IndexOf(selItem->GetItem());
        SetSelectedIndex(selIndex);
        SetSelectedItem(selItem->GetItem());
    }
    else
    {
        SetSelectedIndex(-1);
        SetSelectedItem(NULL);
    }
}

void Selector::AddSelectionChanged(SelectionChangedEventHandler* handler)
{
    AddHandler(SelectionChangedEvent, handler);
}

void Selector::RemoveSelectionChanged(SelectionChangedEventHandler* handler)
{
    RemoveHandler(SelectionChangedEvent, handler);
}

void Selector::NotifyIsSelectedChanged(FrameworkElement* container, bool selected)
{
    if (!GetSelectionChanger()->IsActive())
    {
        if (container != NULL)
        {
            if (container != DpProperty::UnsetValue())
            {
                ItemEntry* itemb = container->GetItemEntry();
                if (NULL == itemb)
                {
                    itemb = ItemEntryFromItem(container);
                }
                SetSelectedHelper(itemb, selected);
            }
        }
    }
}

void Selector::SetSelectedHelper(ItemEntry* item, bool selected)
{
    if (NULL != item)
    {
        SelectionChanger* selChanger = GetSelectionChanger();
        if (selChanger->Begin())
        {
            if (selChanger->SelectItem(item, selected))
            {
                selChanger->Commit();
            }
            else
            {
                selChanger->Clear();
            }
        }
    }
}

void Selector::OnSelected(DpObject* sender, RoutedEventArg* e)
{
    Selector* selector = RTTICast<Selector>(sender);
    FrameworkElement* item = RTTICast<FrameworkElement>(e->GetOriginalSource());
    selector->NotifyIsSelectedChanged(item, true);
}

void Selector::OnUnselected(DpObject* sender, RoutedEventArg* e)
{
    Selector* selector = RTTICast<Selector>(sender);
    FrameworkElement* item = RTTICast<FrameworkElement>(e->GetOriginalSource());
    selector->NotifyIsSelectedChanged(item, false);
}

void Selector::OnSelectedIndexChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Selector* selector = RTTICast<Selector>(d);

    //
    // 直接调用SetSelectedIndex方法导致的选中
    // 我们需要触发选中变化通知并记录选中的项
    //
    if (selector && 
        selector->_isSelectPending == SelectPendingType::eSelectUnknown && 
        !selector->GetSelectionChanger()->IsActive())
    {
        bool bSel = true;
        int index = e->GetNewValue()->ToInt();

        selector->_isSelectPending = SelectPendingType::eSelectInvoke;

        // 取消之前的item
        if (index < 0 || index >= selector->GetCount())
        {
            bSel = false;
            index = e->GetOldValue()->ToInt();
        }
        if (index >= 0 && index < selector->GetCount())
        {
            ItemEntry* pItem = selector->ItemEntryFromIndex(index);
            selector->SetSelectedHelper(pItem, bSel);
        }

        selector->_isSelectPending = SelectPendingType::eSelectUnknown;
    }
}

void Selector::OnSelectedItemChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Selector* selector = RTTICast<Selector>(d);

    //
    // 直接调用SetSelectedIndex方法导致的选中
    // 我们需要触发选中变化通知并记录选中的项
    //
    if (selector && 
        selector->_isSelectPending == SelectPendingType::eSelectUnknown && 
        !selector->GetSelectionChanger()->IsActive())
    {
        selector->_isSelectPending = SelectPendingType::eSelectInvoke;

        // 如果itemb为NULL，可能设置了选择项为NULL
        int index = -1;
        ItemEntry* itemb = NULL;
        bool bSel = true;
        index = selector->GetItemsSource()->IndexOf(e->GetNewValue());
        
        if (index < 0)
        {
            bSel = false;
            index = selector->GetItemsSource()->IndexOf(e->GetOldValue());
        }

        itemb = selector->ItemEntryFromIndex(index);

        if (NULL != itemb)
        {
            selector->SetSelectedHelper(itemb, bSel);
        }

        selector->_isSelectPending = SelectPendingType::eSelectUnknown;
    }
}

void Selector::OnSelectedValueChanged(DpObject* d, DpPropChangedEventArg* e)
{

}

//----------------------------------------------------

Selector::Selector()
{
    _isSelectPending = SelectPendingType::eSelectUnknown;
    _selectMode = SelectionMode::eSingle;
    _selectionChanger = new SelectionChanger(this);
}

Selector::~Selector()
{
    if (_selectionChanger)
    {
        delete _selectionChanger;
    }
}

bool Selector::CanSelectMultiple()
{   
    return _selectMode != SelectionMode::eSingle;
}

int Selector::GetSelectedIndex()
{
    return GetValue(SelectedIndexProperty)->ToInt();
}

void Selector::SetSelectedIndex(int val)
{
    SetValue(SelectedIndexProperty, new Integer(val));
}

Object* Selector::GetSelectedItem()
{
    return GetValue(SelectedItemProperty);
}

void Selector::SetSelectedItem(Object* val)
{
    SetValue(SelectedItemProperty, val);
}

bool Selector::ItemGetIsSelected(ItemEntry* item)
{
    return _selectedItems.Contains(item);
}

void Selector::ItemSetIsSelected(ItemEntry* item, bool bSel)
{
    if (NULL != item)
    {
        if (IsItemItsOwnContainer(item->GetItem()))
        {
            SelectContainer(CASTTOE(item->GetItem()), bSel);
        }
        else
        {
            Element* container = GetContainerFromItem(item->GetItem());
            if (NULL != container)
            {
                SelectContainer(container, bSel);
            }
        }
    }
}

void Selector::PrepareItemContainer(DpObject* container, ItemEntry* item)
{
    ItemsControl::PrepareItemContainer(container, item);
    Element* currElem = RTTICast<Element>(container);
    if (NULL != currElem)
    {
        if (_selectedItems.IndexOf(item->GetItem()) != -1)
        {
            SelectContainer(currElem, true);
        }
        else
        {
            currElem->ClearValue(IsSelectedProperty);
        }
    }
}

void Selector::ClearContainerForItemOverride(DpObject* elem, ItemEntry* item)
{
    ItemsControl::ClearContainerForItemOverride(elem, item);
    if (item != NULL && !IsItemItsOwnContainer(item->GetItem()))
    {
        OnClearItem(elem, item);
    }
}

void Selector::OnClearItem(DpObject* elem, ItemEntry* item)
{
    GetSelectionChanger()->Begin();
    elem->ClearValue(IsSelectedProperty);
    GetSelectionChanger()->Clear();
}

void Selector::OnItemsSourceChanged(ItemCollection* oldValue, ItemCollection* newValue)
{
    ItemsControl::OnItemsSourceChanged(oldValue, newValue);
    if (GetSelectionChanger()->Begin())
    {
        GetSelectionChanger()->Commit();
    }
}

void Selector::OnItemsChanged(NotifyCollChangedEventArg* e)
{
    if ((e->GetAction() == NotifyCollectionChangedAction::Reset) || 
        ((e->GetAction() == NotifyCollectionChangedAction::Add) && (e->GetNewIndex() == 0)))
    {
    }
    
    ItemEntry* item = e->GetNewItem();
    SelectionChanger* selChanger = GetSelectionChanger();

    ItemsControl::OnItemsChanged(e);

    switch (e->GetAction())
    {
    case NotifyCollectionChangedAction::Add:
        if (selChanger->Begin())
        {
            ItemEntry* t = e->GetNewItem();
            if (IsItemItsOwnContainer(t->GetItem()))
            {
                Element* container = RTTICast<Element>(t->GetItem());
                if (container->GetValue(IsSelectedProperty)->ToBool())
                {
                    selChanger->SelectItem(t, true);
                }
            }
            selChanger->Commit();
        }
        return;

    case NotifyCollectionChangedAction::Remove:
        break;

    case NotifyCollectionChangedAction::Replace:
        break;

    case NotifyCollectionChangedAction::Move:
        selChanger->Validate();
        return;

    case NotifyCollectionChangedAction::Reset:

        if (selChanger->Begin())
        {
            for (int i = 0; i < _selectedItems.GetCount(); i++)
            {
                ItemEntry* selItem = _selectedItems.GetItem(i);
                selChanger->SelectItem(selItem, false);
            }
            selChanger->Commit();
        }
        return;

    default:
        return;
    }

    if (selChanger->Begin())
    {
        ItemEntry* t = e->GetOldItem();
        if (_selectedItems.Contains(t))
        {
            selChanger->SelectItem(t, false);
        }
        selChanger->Commit();
    }
}

void Selector::OnSelectionChanged(SelectionChangedEventArg* e)
{
    RaiseEvent(e);
}

}
