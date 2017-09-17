/////////////////////////////////////////////////////////////////////////
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

void ListBox::StaticInit()
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
    ScrollViewer* scrollView = GetScrollHost();

    if (NULL != scrollView && (IsLogicalVertical() || bHori))
    {
        if (e->GetKey() == Key::kSpace || 
            (e->GetKey() == Key::kReturn && KeyboardNavigation::IsAcceptsReturn(this)))
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

        int index = -1;
        int offset = 0;
        int itemSize = 0;

        Object* item = NULL;
        Element* focusElem = NULL;
        ItemEntry* itemb = GetFocusedItem();

        eItemDirection eid = eItemDirection::idCurr;

        AxisDirection dAxis = (IsLogicalVertical() ? AxisDirection::yAxis : AxisDirection::xAxis);

        if (NULL == itemb)
        {
            item = GetSelectedItem();
        }
        else
        {
            item = itemb->GetItem();
        }

        if (e->GetKey() == Key::kUp)
        {
            // 往上滚动一个元素
            if (dAxis == AxisDirection::yAxis)
            {
                eid = eItemDirection::idPrev;
                index = ComputeOffsetFromItem(item, eItemDirection::idPrev, offset, itemSize);
            }
            else
            {
                // 往上滚动一行
                scrollView->LineUp();
            }
        }
        else if (e->GetKey() == Key::kLeft)
        {
            // 往左滚动一个元素
            if (dAxis == AxisDirection::xAxis)
            {
                eid = eItemDirection::idPrev;
                index = ComputeOffsetFromItem(item, eItemDirection::idPrev, offset, itemSize);
            }
            else
            {
                // 往左滚动一行
                scrollView->LineLeft();
            }
        }
        else if (e->GetKey() == Key::kDown)
        {
            // 往下滚动一个元素
            if (dAxis == AxisDirection::yAxis)
            {
                eid = eItemDirection::idNext;
                index = ComputeOffsetFromItem(item, eItemDirection::idNext, offset, itemSize);
            }
            else
            {
                // 往下滚动一行
                scrollView->LineDown();
            }
        }
        else if (e->GetKey() == Key::kRight)
        {
            // 往右滚动一个元素
            if (dAxis == AxisDirection::xAxis)
            {
                eid = eItemDirection::idNext;
                index = ComputeOffsetFromItem(item, eItemDirection::idNext, offset, itemSize);
            }
            else
            {
                // 往右滚动一行
                scrollView->LineRight();
            }
        }

        if (index >= 0)
        {
            itemb = ItemEntryFromIndex(index);
            item = itemb->GetItem();

            if (NULL != itemb)
            {
                focusElem = GetContainerFromItem(item);
                if (NULL == focusElem || !IsOnCurrentPage(focusElem, dAxis, true))
                {
                    //MakeVisible(item, offset, itemSize, false);
                    NavigateToItem(item, offset, itemSize, eItemDirection::idCurr, false);
                    UpdateLayout();
                }
                UpdateFocusItem(itemb);
            }
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

void ListBox::HandleSelectedItem(Object* cHost, MouseButton mouseButton)
{
    ListBoxItem* container = (ListBoxItem*) cHost;
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
