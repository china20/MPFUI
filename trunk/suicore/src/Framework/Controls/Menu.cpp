// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Menu.cpp

#include <Framework/Controls/Menu.h>
#include <Framework/Controls/Separator.h>
#include <Framework/Controls/MenuitemHelper.h>

#include <System/Windows/CoreTool.h>
#include <System/Windows/RenderTarget.h>

#include <System/Input/MouseDevice.h>

namespace suic
{

ImplementRTTIOfClass(MenuBase, Selector)
ImplementRTTIOfClass(Menu, MenuBase)
ImplementRTTIOfClass(ContextMenu, MenuBase)

MenuBase::MenuBase()
    : _ownerMenu(NULL)
{
}

MenuBase::~MenuBase()
{
}

void MenuBase::Dispose()
{
    Selector::Dispose();
}

MenuBase* MenuBase::GetOwner() const
{
    return _ownerMenu;
}

void MenuBase::SetOwner(MenuBase* owner)
{
    _ownerMenu = owner;
}

void MenuBase::StaticInit()
{
}

bool MenuBase::IsItemItsOwnContainer(Object* item)
{
    return (MenuItem::RTTIType() == item->GetRTTIType());
}

Element* MenuBase::GetContainerForItemOverride()
{
    return new MenuItem();
}

bool MenuBase::BlockRouteEvent() const
{
    return true;
}

suic::Object* MenuBase::NavigatePrevItem(suic::ItemsControl* items)
{
    suic::Object* pItem = NULL;
    suic::Object* pSelItem = GetSelectedItem();

    if (pSelItem == suic::DpProperty::UnsetValue())
    {
        pSelItem = NULL;
    }

    if (items->GetCount() > 0)
    {
        if (NULL == pSelItem)
        {
            pItem = items->GetItem(items->GetCount() - 1);
        }
        else
        {
            for (int i = 0; i < items->GetCount(); ++i)
            {
                suic::Object* temp = items->GetItem(i);

                if (pSelItem == temp)
                {
                    i--;
                    pItem = FindEndToBegin(items, i);
                    break;
                }
            }

            if (NULL == pItem)
            {
                pItem = items->GetItem(items->GetCount() - 1);
            }
        }
    }

    return pItem;
}

Object* MenuBase::FindEndToBegin(suic::ItemsControl* items, int iEnd)
{
    Object* pItem = NULL;
    int iStart = iEnd - items->GetCount();

    for (int i = iEnd; i > iStart; --i)
    {
        int index = (items->GetCount() + i) % items->GetCount();
        Object* temp = items->GetItem(index);

        if (temp->GetRTTIType() != suic::Separator::RTTIType())
        {
            pItem = temp;
            break;
        }
    }

    return pItem;
}

Object* MenuBase::FindBeginToEnd(suic::ItemsControl* items, int iBegin)
{
    int i = 0;
    int iCount = items->GetCount() + iBegin;
    Object* pItem = NULL;

    for (i = iBegin; i < iCount; ++i)
    {
        Object* temp = items->GetItem(i % items->GetCount());
        if (temp->GetRTTIType() != suic::Separator::RTTIType())
        {
            pItem = temp;
            break;
        }
    }

    return pItem;
}

suic::Object* MenuBase::NavigateNextItem(suic::ItemsControl* items)
{
    suic::Object* pItem = NULL;
    suic::Object* pSelItem = GetSelectedItem();

    if (pSelItem == suic::DpProperty::UnsetValue())
    {
        pSelItem = NULL;
    }

    if (items->GetCount() > 0)
    {
        if (NULL == pSelItem)
        {
            pItem = items->GetItem(0);
        }
        else
        {
            int i = 0;

            for (i = 0; i < items->GetCount(); ++i)
            {
                suic::Object* temp = items->GetItem(i);
                if (pSelItem == temp)
                {
                    i++;
                    pItem = FindBeginToEnd(items, i);
                    break;
                }
            }

            if (NULL == pItem)
            {
                pItem = items->GetItem(0);
            }
        }
    }

    return pItem;
}

void MenuBase::OnApplyTemplate()
{
    Selector::OnApplyTemplate();
}

void MenuBase::OnInitialized(EventArg* e)
{
    Selector::OnInitialized(e);
}

void MenuBase::OnRender(Drawing * drawing)
{
    Selector::OnRender(drawing);
}

void MenuBase::OnTextInput(KeyboardEventArg* e)
{
}

void MenuBase::OnPreviewKeyDown(KeyboardEventArg* e)
{
    suic::Object* pItem = NULL;
    suic::MenuItem* pMenuItem = NULL;
    int trackPopupCount = TrackingMenuOp::Ins()->GetTrackPopupCount();
    Selector::OnPreviewKeyDown(e);

    if (GetCount() > 0)
    {
        if (e->GetKey() == suic::Key::kMenu)
        {
            if (suic::DynamicCast<Menu>(this) != NULL)
            {
                e->SetHandled(true);

                pItem = GetSelectedItem();

                if (trackPopupCount > 0)
                {
                    GetMenuPopup()->CloseAllPopup(true);
                }

                // 
                // 选中第一个
                // 
                if (suic::DpProperty::UnsetValue() == pItem)
                {
                    pItem = GetItem(0);
                    pMenuItem = MenuItemFromItem(pItem);
                    pMenuItem->SetIsHighlighted(true);
                }
                else
                {
                    pMenuItem = MenuItemFromItem(pItem);
                    pMenuItem->SetIsHighlighted(false);
                }

                pMenuItem->Focus();
            }
        }
    }
}

void MenuBase::OnKeyDown(KeyboardEventArg* e)
{
    suic::Object* pItem = NULL;
    suic::ItemsControl* items = this;
    suic::MenuItem* pMenuItem = NULL;
    int trackPopupCount = TrackingMenuOp::Ins()->GetTrackPopupCount();
    Popup* popup = TrackingMenuOp::Ins()->GetTopPopup();

    if (GetCount() == 0)
    {
        return;
    }

    // 
    // 子菜单弹出了
    // 
    if (NULL != popup && popup != GetParentPopup())
    {
        pMenuItem = suic::DynamicCast<suic::MenuItem>(popup->GetPlacementTarget());
        items = pMenuItem;
    }

    Selector::OnKeyDown(e);

    if (e->GetKey() == suic::Key::kEscape)
    {
        if (NULL == popup)
        {
            SetSelectedItem(NULL);
            e->SetHandled(true);
            return;
        }
    }
    else if (e->GetKey() == suic::Key::kLeft)
    {
        // 关掉子菜单
        if (NULL != popup)
        {
            GetMenuPopup()->ClosePopup(popup);

            if (NULL != pMenuItem)
            {
                pMenuItem->SetIsHighlighted(true);
            }
            
            if (trackPopupCount == 1)
            {
                if (GetSelectedItem() == GetItem(0))
                {
                    pItem = GetItem(GetCount() - 1);
                }
                else
                {
                    pItem = NavigatePrevItem(this);
                }

                pMenuItem = MenuItemFromItem(pItem);
                pMenuItem->HandleLeftButtonDown(true);
                pMenuItem->SetIsHighlighted(true);
            }

            UpdateLayout();

            return;
        }
        else
        {
            pItem = NavigatePrevItem(items);
        }

        e->SetHandled(true);
    }
    else if (e->GetKey() == suic::Key::kRight)
    {
        if (NULL == popup)
        {
            pItem = NavigateNextItem(items);
        }
        else
        {
            pItem = GetSelectedItem();
        }
        e->SetHandled(true);
    }
    else if (e->GetKey() == suic::Key::kUp)
    {
        pItem = NavigatePrevItem(items);
        e->SetHandled(true);
    }
    else if (e->GetKey() == suic::Key::kDown)
    {
        if (popup == NULL)
        {
            pItem = GetSelectedItem();
            pMenuItem = MenuItemFromItem(pItem);
            if (NULL != pMenuItem)
            {
                pMenuItem->HandleLeftButtonDown(true);               
            }
        }
        else
        {
            pItem = NavigateNextItem(items);
        }
        e->SetHandled(true);
    }
    else if (e->GetKey() == suic::Key::kSpace)
    {
        pMenuItem = MenuItemFromItem(GetSelectedItem());
        if (NULL != pMenuItem)
        {
            pMenuItem->HandleMenuItemClick();
            e->SetHandled(true);
        }

        return;
    }

    if (NULL != pItem && pItem != suic::DpProperty::UnsetValue())
    {
        pMenuItem = MenuItemFromItem(pItem);
        if (NULL != pMenuItem)
        {
            Menu* pMenu = suic::DynamicCast<Menu>(pMenuItem->GetParent());

            if (e->GetKey() == suic::Key::kRight && popup != NULL && (pMenu != NULL || !pMenuItem->IsSubmenu()))
            {
                HandleRightKeyPopup();
            }
            else if (pMenuItem->IsSubmenu())
            {
                if (e->GetKey() == suic::Key::kRight)
                {
                    pMenuItem->HandleMenuItemEnter(true);
                }
            }

            if (NULL != pMenuItem && e->GetKey() != suic::Key::kRight)
            {
                pMenuItem->SetIsHighlighted(true);
            }
        }
    }
    else if (e->GetKey() == suic::Key::kRight)
    {
        HandleRightKeyPopup();
    }

    if (NULL != popup)
    {
        popup->UpdateLayout();
    }
    else
    {
        UpdateLayout();
    }
}

void MenuBase::HandleRightKeyPopup()
{
    suic::Object* pItem = NULL;
    suic::MenuItem* trackMenuItem = NULL;
    trackMenuItem = suic::DynamicCast<suic::MenuItem>(TrackingMenuOp::Ins()->GetTrackingMenuItem());

    if (NULL != trackMenuItem)
    {
        trackMenuItem->SetIsHighlighted(true);

        if (GetSelectedItem() == GetItem(GetCount() - 1))
        {
            pItem = GetItem(0);
        }
        else
        {
            pItem = NavigateNextItem(this);
        }

        trackMenuItem = MenuItemFromItem(pItem);

        GetMenuPopup()->CloseAllPopup(false);
        trackMenuItem->HandleLeftButtonDown(true);
    }
}

MenuItem* MenuBase::MenuItemFromItem(Object* item)
{
    MenuItem* pMenuItem = suic::DynamicCast<MenuItem>(GetContainerFromItem(item));
    return pMenuItem;
}

void MenuBase::OnMouseEnter(MouseButtonEventArg* e)
{
    Selector::OnMouseEnter(e);
    OnMouseMove(e);
}

void MenuBase::OnMouseMove(MouseButtonEventArg* e)
{
    Selector::OnMouseMove(e);

    if (MouseDevice::GetLeftButton() != MouseButtonState::mbMousePress)
    {
        return;
    }

    MenuItem* pOver = NULL;
    Panel* panel = GetItemsHost();
    ElementColl* elemColl = panel->GetChildren();

    for (int i = 0; i < panel->GetVisibleChildrenCount(); ++i)
    {
        MenuItem* pItem = dynamic_cast<MenuItem*>(elemColl->GetAt(i));

        if (pItem)
        {
            if (GetRenderBound(pItem).PointIn(e->GetMousePoint()))
            {
                pOver = pItem;

                break;
            }
        }
    }

    Element* pFocused = CASTTOE(GetFocusedItem());

    if (pOver != pFocused)
    {
        if (pFocused)
        {
            Selector::SelectContainer(pFocused, false);
        }

        if (pOver)
        {
            Selector::SelectContainer(pOver, true);
            pOver->Focus();
        }
    }

    e->SetHandled(true);
}

void MenuBase::OnMouseLeave(MouseButtonEventArg* e)
{
    Selector::OnMouseLeave(e);

    FrameworkElementPtr frame(GetFocusedItem());

    if (frame)
    {
        MouseButtonEventArg ef(NULL, e->GetMousePoint());
        ef.SetSource(frame.get());
        frame->OnMouseLeave(&ef);
    }
}

void MenuBase::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    Selector::OnMouseLeftButtonUp(e);
    return;
}

void MenuBase::OnSelectionChanged(SelectionChangedEventArg* e)
{
    Element* container = NULL;

    e->SetHandled(true);

    if (e->GetRemovedItems()->GetCount() > 0)
    {
        container = GetContainerFromItem(e->GetRemovedItems()->GetItem(0));
        if (NULL != container)
        {
            MenuItem::SetIsHighlighted(container, false);
            UpdateLayout();
        }
    }
}

void MenuBase::OnClearItem(DpObject* elem, ItemEntry* item)
{
    Selector::OnClearItem(elem, item);
    MenuItem* pMenuItem = RTTICast<MenuItem>(elem);
    if (NULL != pMenuItem && pMenuItem->IsHighlighted())
    {
        GetSelectionChanger()->Begin();
        pMenuItem->ClearValue(MenuItem::IsHighlightedProperty);
        GetSelectionChanger()->Clear();
    }
}

void MenuBase::PrepareContainerForItemOverride(DpObject* elem, ItemEntry* item)
{
    Selector::PrepareContainerForItemOverride(elem, item);

    if (elem->GetRTTIType() != suic::Separator::RTTIType() && 
        item->GetItem() == GetSelectedItem())
    {
        MenuItem::SetIsHighlighted(elem, true);
    }
    else
    {
        MenuItem::SetIsHighlighted(elem, false);
    }
}

Popup* MenuBase::GetParentPopup()
{
    return NULL;
}

void Menu::StaticInit()
{
}

Menu::Menu()
{
}

Menu::~Menu()
{
}

void Menu::OnLoaded(LoadedEventArg* e)
{
    VisualHost* target = NULL;
    
    target = suic::DynamicCast<VisualHost>(e->GetTarget());

    if (NULL != target)
    {
        target->_mainMenu = this;
    }
}

DpProperty* ContextMenu::IsOpenProperty;
DpProperty* ContextMenu::PlacementProperty;
DpProperty* ContextMenu::PlacementRectangleProperty;
DpProperty* ContextMenu::PlacementTargetProperty;
DpProperty* ContextMenu::VerticalOffsetProperty;
DpProperty* ContextMenu::HorizontalOffsetProperty;

void ContextMenu::OnIsOpenPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ContextMenu* menu = RTTICast<ContextMenu>(d);
    if (e->GetNewValue()->ToBool())
    {
        if (menu->_parentPopup == NULL)
        {
            menu->HookupParentPopup();
        }
        TrackingMenuOp::Ins()->TrackContextMenu(menu->_parentPopup, menu, 0, 0);
    }
    else
    {
        //menu->ClosingMenu();
    }
}

void ContextMenu::StaticInit()
{
    if (NULL == IsOpenProperty)
    {
        Popup::StaticInit();

        IsOpenProperty = Popup::IsOpenProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, 0, &ContextMenu::OnIsOpenPropChanged));
        PlacementProperty = Popup::PlacementProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(PlacementModeBox::MousePointBox, 0));
        PlacementRectangleProperty = Popup::PlacementRectangleProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(new ORect(), 0));
        PlacementTargetProperty = Popup::PlacementTargetProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(NULL, 0));
        VerticalOffsetProperty = Popup::VerticalOffsetProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), 0));
        HorizontalOffsetProperty = Popup::HorizontalOffsetProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), 0));
    }
}

ContextMenu::ContextMenu()
    : _parentPopup(NULL)
{
}

ContextMenu::~ContextMenu()
{
    if (_parentPopup)
    {
        _parentPopup->unref();
    }
}

bool ContextMenu::IsOpen()
{
    return GetValue(IsOpenProperty)->ToBool();
}

void ContextMenu::SetIsOpen(bool val)
{
    SetValue(IsOpenProperty, BOOLTOBOOLEAN(val));
}

PlacementMode ContextMenu::GetPlacement()
{
    return (PlacementMode)GetValue(PlacementProperty)->ToInt();
}

void ContextMenu::SetPlacement(PlacementMode val)
{
    SetValue(PlacementProperty, new Integer(val));
}

Rect ContextMenu::GetPlacementRectangle()
{
    return ORect::FromObj(GetValue(PlacementRectangleProperty));
}

void ContextMenu::GetPlacementRectangle(Rect val)
{
    SetValue(PlacementRectangleProperty, new ORect(val));
}

Element* ContextMenu::GetPlacementTarget()
{
    return DynamicCast<Element>(GetValue(PlacementTargetProperty));
}

void ContextMenu::SetPlacementTarget(Element* target)
{
    SetValue(PlacementTargetProperty, target);
}

int ContextMenu::GetVerticalOffset()
{
    return GetValue(VerticalOffsetProperty)->ToInt();
}

void ContextMenu::SetVerticalOffset(int val)
{
    SetValue(VerticalOffsetProperty, new Integer(val));
}

int ContextMenu::GetHorizontalOffset()
{
    return GetValue(HorizontalOffsetProperty)->ToInt();
}

void ContextMenu::SetHorizontalOffset(int val)
{
    SetValue(HorizontalOffsetProperty, new Integer(val));
}

void ContextMenu::TrackContextMenu()
{
    SetIsOpen(true);
}

void ContextMenu::CloseContextMenu()
{
    GetMenuPopup()->CloseAllPopup(false);
}

void ContextMenu::OnPopupOpened(Object* sender, EventArg* e)
{
    SetSelectedItem(NULL);
}

void ContextMenu::OnPopupClosed(Object* sender, EventArg* e)
{
}

Popup* ContextMenu::GetParentPopup()
{
    HookupParentPopup();
    return _parentPopup;
}

void ContextMenu::HookupParentPopup()
{
    if (_parentPopup == NULL)
    {
        _parentPopup = new Popup();
        _parentPopup->ref();

        _parentPopup->SetAllowsTransparency(true);
        _parentPopup->Opened += EventHandler(this, &ContextMenu::OnPopupOpened);
        _parentPopup->Closed += EventHandler(this, &ContextMenu::OnPopupClosed);
        Popup::CreateRootPopup(_parentPopup, this);
    }
}

}
