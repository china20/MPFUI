//////////////////////////////////////////////////////////////////////////////
// ListBox.cpp

#include <Extend/Controls/ListBox.h>

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Input/KeyboardNavigation.h>

namespace suic
{

ImplementRTTIOfClass(ListBox, Selector)

//DpProperty* ListBox::SelectedItemsProperty;
DpProperty* ListBox::SelectionModeProperty;

class SelectionModeConvert : public IConvertValue
{
public:

    static SelectionModeConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val)
    {
        String selMode = val->ToString();
        if (selMode.Equals(_U("Multiple")))
        {
            return Integer::GetPosInt(SelectionMode::eMultiple);
        }
        else if (selMode.Equals(_U("Extended")))
        {
            return Integer::GetPosInt(SelectionMode::eExtended);
        }
        else
        {
            return Integer::GetPosInt(SelectionMode::eSingle);
        }
    }

protected:

    SelectionModeConvert() {}
};

SelectionModeConvert* SelectionModeConvert::Convert = new SelectionModeConvert();

bool ListBox::StaticInit()
{
    if (NULL == SelectionModeProperty)
    {
        ListBoxItem::StaticInit();

        SelectionModeProperty = suic::DpProperty::Register(_U("SelectionMode"), RTTIType(), suic::Integer::RTTIType(), 
            suic::DpPropMemory::GetPropMeta(Integer::GetPosInt(SelectionMode::eSingle), suic::PropMetadataOptions::AffectsRender, &ListBox::OnSelectionModePropChanged));
        SelectionModeProperty->SetConvertValueCb(SelectionModeConvert::Convert);
        Control::IsTabStopProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False));
        KeyboardNavigation::DirectionalNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knContained)));
        KeyboardNavigation::TabNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knOnce)));
    }

    return true;
}

void ListBox::OnSelectionModePropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    ListBox* pListBox = RTTICast<ListBox>(d);
    if (NULL != pListBox)
    {
        pListBox->_selectMode = e->GetNewValue()->ToInt();
    }
}

//----------------------------------------------------

ListBox::ListBox()
{
    StaticInit();
}

ListBox::~ListBox()
{
}

SelectionMode ListBox::GetSelectionMode()
{
    return (SelectionMode)_selectMode;
}

void ListBox::SetSelectionMode(SelectionMode val)
{
    SetValue(SelectionModeProperty, Integer::GetPosInt(val));
}

Element* ListBox::GetContainerForItemOverride()
{
    return new ListBoxItem();
}

bool ListBox::IsItemItsOwnContainer(Object* item)
{
    return (item->GetRTTIType() == ListBoxItem::RTTIType());
}

void ListBox::OnKeyDown(KeyboardEventArg* e)
{
    bool bHori = IsLogicalHorizontal();
    if (IsLogicalVertical() || bHori)
    {
        if (e->GetKey() == Key::kSpace || 
            (e->GetKey() == Key::kReturn && KeyboardNavigation::IsAcceptsReturn(this)) )
        {
            ListBoxItem* pItem = RTTICast<ListBoxItem>(e->GetOriginalSource());
            if (NULL != pItem)
            {
                HandleSelectedItem(pItem, MouseButton::mbUnknown);
                e->SetHandled(true);
            }
            return;
        }
        else if (!e->IsArrowKey())
        {
            return;
        }

        int index = 0;
        int offset = 0;
        int itemLen = 0;
        ItemEntry* itemb = GetFocusedItem();
        Object* currentItem = NULL;
        Size itemSize;
        AxisDirection dAxis = (IsLogicalVertical() ? AxisDirection::yAxis : AxisDirection::xAxis);

        if (NULL == itemb)
        {
            currentItem = GetSelectedItem();
        }
        else
        {
            currentItem = itemb->GetItem();
        }

        // 
        // offset为前index个元素的总长度
        //
        index = ComputeOffsetFromItem(currentItem, offset, itemLen);

        if (index < 0)
        {
            e->SetHandled(true);
            ScrollToStart();
            return;
        }

        if ((e->GetKey() == Key::kUp && dAxis == AxisDirection::yAxis) || 
            (e->GetKey() == Key::kLeft && dAxis == AxisDirection::xAxis))
        {
            if (index > 0)
            {
                index -= 1;
                itemb = GetItemsSource()->GetItemEntry(index);
                ReadItemSize(itemb, bHori, index, itemSize);
                // index-1处子项的高度
                itemLen = (e->GetKey() == Key::kUp ? itemSize.cy : itemSize.cx);
                // 前index-1个子项总长度
                offset -= itemLen;
            }
            else
            {
                return;
            }
        }
        else if ((e->GetKey() == Key::kDown && dAxis == AxisDirection::yAxis) || 
            (e->GetKey() == Key::kRight && dAxis == AxisDirection::xAxis))
        {
            if (index < GetCount() - 1)
            {
                offset += itemLen;
                index += 1;
                itemb = GetItemsSource()->GetItemEntry(index);
                ReadItemSize(itemb, bHori, index, itemSize);
                itemLen = (e->GetKey() == Key::kDown ? itemSize.cy : itemSize.cx);
            }
            else
            {
                return;
            }
        }

        ItemEntry* moveItem = ItemEntryFromIndex(index);
        Element* moveElem = GetContainerFromItem(moveItem->GetItem());
        ItemNavigateArg enav(KeyboardDeice::Current(), e->GetModifierKey());

        if (!IsOnCurrentPage(moveElem, dAxis, true))
        {
            NavigateToItem(moveItem->GetItem(), offset, itemLen, &enav, e->GetKey() == Key::kUp || e->GetKey() == Key::kLeft);
        }
        else
        {
            moveElem->Focus();
            UpdateLayout();
        }

        e->SetHandled(true);
    }
}

void ListBox::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

void ListBox::NotifyListItemMouseDragged(ListBoxItem* listItem)
{
}

void ListBox::SetAnchorItem(Object* item)
{
    _anchorItem = item;
}

Object* ListBox::GetAnchorItem()
{
    return _anchorItem.GetTarget();
}

void ListBox::UpdateAnchorAndActionItem(ListBoxItem* listItem)
{
    ItemEntry* itemb = listItem->GetItemEntry();
    if (NULL == itemb || itemb->GetItem() == DpProperty::UnsetValue())
    {
        SetAnchorItem(NULL);
    }
    else
    {
        SetAnchorItem(itemb->GetItem());
    }
    KeyboardNavigation::SetTabOnceActiveElement(this, listItem);
}

void ListBox::HandleSelectedItem(ListBoxItem* container, MouseButton mouseButton)
{
    ItemEntry* itemb = container->GetItemEntry();

    switch (GetSelectionMode())
    {
    case SelectionMode::eSingle:
        if (container->GetIsSelected())
        {
            if (Keyboard::IsControlDown())
            {
                container->SetCurrentValue(Selector::IsSelectedProperty, Boolean::False);
            }
            break;
        }

        container->SetCurrentValue(Selector::IsSelectedProperty, Boolean::True);

        break;

    case SelectionMode::eMultiple:
        MakeToggleSelection(container);
        return;

    case SelectionMode::eExtended:
        if (mouseButton != MouseButton::mbLeftButton && mouseButton != MouseButton::mbUnknown)
        {
            if ((mouseButton == MouseButton::mbRightButton) && 
                (!Keyboard::IsShiftDown() && !Keyboard::IsControlDown()))
            {
                if (container->GetIsSelected())
                {
                    UpdateAnchorAndActionItem(container);
                    return;
                }
                MakeSingleSelection(container);
            }
            return;
        }

        if ((Keyboard::GetModifiers() & (ModifierKey::mkShift | ModifierKey::mkControl)) != (ModifierKey::mkShift | ModifierKey::mkControl))
        {
            if (Keyboard::IsControlDown())
            {
                MakeToggleSelection(container);
                return;
            }

            if (Keyboard::IsShiftDown())
            {
                MakeAnchorSelection(container, true);
                return;
            }
            MakeSingleSelection(container);
            return;
        }
        MakeAnchorSelection(container, false);
        return;

    default:
        return;
    }
}

void ListBox::NotifyListItemClicked(ListBoxItem* container, MouseButton mouseButton)
{
    if ((mouseButton == MouseButton::mbLeftButton) && (Mouse::GetCaptured() != this))
    {
        //Mouse::SetCaptured(this);
        //SetInitialMousePosition();
    }

    HandleSelectedItem(container, mouseButton);

    //RoutedEventArg e(container, ItemClickEvent);
    //RaiseEvent(&e);
}

void ListBox::MakeToggleSelection(ListBoxItem* item)
{
    ItemEntry* itemb = item->GetItemEntry();
    bool flag = !item->GetIsSelected();
    item->SetValue(Selector::IsSelectedProperty, BOOLTOBOOLEAN(flag));
    UpdateAnchorAndActionItem(item);
}

void ListBox::MakeSingleSelection(ListBoxItem* item)
{
    if (ItemsControl::ItemsControlFromItemContainer(item) == this)
    {
        ItemEntry* itemb = item->GetItemEntry();
        GetSelectionChanger()->SelectJustThisItem(itemb, true);
        item->Focus();
        UpdateAnchorAndActionItem(item);
    }
}

void ListBox::MakeAnchorSelection(ListBoxItem* actionItem, bool clearCurrent)
{
    if (GetAnchorItem() == NULL)
    {
        int selCount = _selectedItems.GetCount();
        if (selCount > 0)
        {
            SetAnchorItem(_selectedItems[selCount - 1]->GetItem());
        }
        else
        {
            SetAnchorItem(GetItem(0));
        }

        if (GetAnchorItem() == NULL)
        {
            return;
        }
    }

    SelectionChanger* selChanger = GetSelectionChanger();
    ItemCollection* itemsColl = GetItemsSource();
    int iBegin = itemsColl->IndexOf(actionItem->GetContainerItem());
    int iEnd = itemsColl->IndexOf(GetAnchorItem());
    ItemEntry* itemb = actionItem->GetItemEntry();

    if (iBegin < 0)
    {
        iBegin = 0;
    }

    if (iBegin > iEnd)
    {
        int iTmp = iBegin;
        iBegin = iEnd;
        iEnd = iTmp;
    }

    if (iEnd >= GetCount())
    {
        iEnd = GetCount() - 1;
    }

    bool flag = false;
    if (!selChanger->IsActive())
    {
        flag = true;
        selChanger->Begin();
    }

    if (clearCurrent)
    {
        for (int j = 0; j < _selectedItems.GetCount(); j++)
        {
            ItemEntry* item = _selectedItems[j];
            int index = itemsColl->IndexOf(item->GetItem());
            if ((index < iBegin) || (iEnd < index))
            {
                selChanger->SelectItem(item, false);
            }
        }
    }

    for (int i = iBegin; i <= iEnd; i++)
    {
        selChanger->SelectItem(itemsColl->GetItemEntry(i), true);
    }
    
    if (flag)
    {
        selChanger->Commit();
    }
}

void ListBox::ScrollToStart()
{
    if (HasItems())
    {
        Object* item = GetItemsSource()->GetItem(0);
        ItemNavigateArg e(MouseDevice::Current(), ModifierKey::mkNone);
        NavigateToItem(item, 0, 0, &e, true);
    }
}

void ListBox::ScrollToEnd()
{
    ItemNavigateArg e(MouseDevice::Current(), ModifierKey::mkNone);
    NavigateToEnd(&e);
}

void ListBox::ScrollIntoView(Object* item)
{
    if (HasItems())
    {
        ItemNavigateArg e(MouseDevice::Current(), ModifierKey::mkNone);
        NavigateToItem(item, -1, 0, &e, true);
    }
}

void ListBox::SelectAll()
{
    SelectAllImpl();
}

void ListBox::UnselectAll()
{
    UnselectAllImpl();
}

void ListBox::SelectAllImpl()
{
    GetSelectionChanger()->Begin();

    for (int i = 0; i < GetCount(); ++i)
    {
        ItemEntry* obj = ItemEntryFromIndex(i);
        GetSelectionChanger()->SelectItem(obj, true);
    }

    GetSelectionChanger()->Commit();
}

void ListBox::UnselectAllImpl()
{
    GetSelectionChanger()->Begin();

    for (int i = 0; i < _selectedItems.GetCount(); ++i)
    {
        ItemEntry* obj = _selectedItems.GetItem(i);
        GetSelectionChanger()->SelectItem(obj, false);
    }

    GetSelectionChanger()->Commit();
}

void ListBox::OnClearItem(DpObject* elem, ItemEntry* item)
{
    Selector::OnClearItem(elem, item);
    ListBoxItem* pListItem = RTTICast<ListBoxItem>(elem);
    if (NULL != pListItem && pListItem->GetIsSelected())
    {
        GetSelectionChanger()->Begin();
        pListItem->ClearValue(IsSelectedProperty);
        GetSelectionChanger()->Clear();
    }
}

void ListBox::PrepareContainerForItemOverride(DpObject* elem, ItemEntry* item)
{
    Selector::PrepareContainerForItemOverride(elem, item);

    if (IsKeyboardFocusWithin() && item != NULL && GetFocusedItem() == item)
    {
        ListBoxItem* pListItem = RTTICast<ListBoxItem>(elem);
        pListItem->Focus();
    }
}

}
