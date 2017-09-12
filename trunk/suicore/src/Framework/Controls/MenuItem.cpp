// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// MenuItem.cpp

#include <Framework/Controls/Menu.h>
#include <Framework/Controls/Separator.h>
#include <Framework/Controls/menuitemhelper.h>

#include <System/Windows/CommandSource.h>
#include <System/Windows/CoreTool.h>
#include <System/Input/MouseDevice.h>
#include <System/Tools/EventHelper.h>
#include <System/Tools/HwndHelper.h>
#include <System/Tools/VisualTreeOp.h>

namespace suic
{

ImplementRTTIOfClass(MenuItem, HeaderedItemsControl)

DpProperty* MenuItem::IsSubmenuOpenProperty;
DpProperty* MenuItem::IsCheckedProperty;
DpProperty* MenuItem::IsCheckableProperty;
DpProperty* MenuItem::IsPressedProperty;
DpProperty* MenuItem::IsHighlightedProperty;

DpProperty* MenuItem::RoleProperty;
DpProperty* MenuItem::IconProperty;

Integer* MenuItemRoleBox::TopLevelItemBox = new Integer(MenuItemRole::TopLevelItem, false);
Integer* MenuItemRoleBox::TopLevelHeaderBox = new Integer(MenuItemRole::TopLevelHeader, false);
Integer* MenuItemRoleBox::SubmenuItemBox = new Integer(MenuItemRole::SubmenuItem, false);
Integer* MenuItemRoleBox::SubmenuHeaderBox = new Integer(MenuItemRole::SubmenuHeader, false);

RoutedEvent* MenuItem::ClickEvent;
RoutedEvent* MenuItem::CheckedEvent;
RoutedEvent* MenuItem::UncheckedEvent;

RoutedEvent* MenuItem::SubmenuOpenedEvent;
RoutedEvent* MenuItem::SubmenuClosedEvent;

class MenuItemRoleConvert : public IConvertValue
{
public:

    MenuItemRoleConvert()
    {

    }

    AutoObj ConvertValue(DpProperty* dp, Object* prop)
    {
        String val = prop->ToString();

        if (val.Equals(_T("TopLevelHeader")))
        {
            return MenuItemRoleBox::TopLevelHeaderBox;
        }
        else if (val.Equals(_T("SubmenuItem")))
        {
            return MenuItemRoleBox::SubmenuItemBox;
        }
        else if (val.Equals(_T("SubmenuHeader")))
        {
            return MenuItemRoleBox::SubmenuHeaderBox;
        }
        else
        {
            return MenuItemRoleBox::TopLevelItemBox;
        }
    }

    String ToString(DpProperty* dp, Object* prop)
    {
        String strVal;
        int iVal = prop->ToInt();

        switch (iVal)
        {
        case MenuItemRole::TopLevelHeader:
            strVal = _U("TopLevelHeader");
            break;

        case MenuItemRole::SubmenuItem:
            strVal = _U("SubmenuItem");
            break;

        case MenuItemRole::SubmenuHeader:
            strVal = _U("SubmenuHeader");
            break;

        default:

            strVal = _U("TopLevelItem");
        }

        return strVal;
    }
};

MenuItem::MenuItem()
{
    _isSeparator = false;
    _commandSource = NULL;
}

MenuItem::~MenuItem()
{
    if (_commandSource)
    {
        delete _commandSource;
    }
}

void MenuItem::Dispose()
{
    if (_submenuPopup)
    {
        _submenuPopup = NULL;
    }

    if (_currentSelection)
    {
        _currentSelection = NULL;
    }

    HeaderedItemsControl::Dispose();
}

ICommandSource* MenuItem::GetCommandSource()
{
    if (!_commandSource)
    {
        _commandSource = new CommandSource();
        _commandSource->SetCommandTarget(this);
    }

    return _commandSource;
}

void MenuItem::OnIsCheckedPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    MenuItem* pItem = DynamicCast<MenuItem>(d);
    if (NULL != pItem)
    {
        if (e->GetNewValue()->ToBool())
        {
            RoutedEventArg re(pItem, CheckedEvent);
            pItem->OnChecked(&re);
        }
        else
        {
            RoutedEventArg re(pItem, UncheckedEvent);
            pItem->OnUnchecked(&re);
        }
    }
}

void MenuItem::StaticInit()
{
    if (NULL == IsSubmenuOpenProperty)
    {
        HeaderedItemsControl::StaticInit();

        IsSubmenuOpenProperty = DpProperty::Register(_T("IsSubmenuOpen"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        IsCheckedProperty = DpProperty::Register(_T("IsChecked"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender, &MenuItem::OnIsCheckablePropChanged));
        IsCheckableProperty = DpProperty::Register(_T("IsCheckable"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone, &MenuItem::OnIsCheckablePropChanged));
        IsPressedProperty = DpProperty::Register(_T("IsPressed"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        IsHighlightedProperty = DpProperty::Register(_T("IsHighlighted"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));

        RoleProperty = DpProperty::Register(_T("Role"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(MenuItemRoleBox::TopLevelItemBox, PropMetadataOptions::AffectsNone));

        RoleProperty->SetConvertValueCb(new MenuItemRoleConvert());
        IconProperty = DpProperty::Register(_T("Icon"), RTTIType(), ImageSource::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));

        ClickEvent = EventHelper::RegisterRoutedEvent(_U("Click"), RoutingStrategy::Bubble, ClickEventHandler::RTTIType(), RTTIType());
        CheckedEvent = EventHelper::RegisterRoutedEvent(_U("Checked"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
        UncheckedEvent = EventHelper::RegisterRoutedEvent(_U("Unchecked"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());

        SubmenuOpenedEvent = EventHelper::RegisterRoutedEvent(_U("SubmenuOpened"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
        SubmenuClosedEvent = EventHelper::RegisterRoutedEvent(_U("SubmenuClosed"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
    }
}

void MenuItem::OnIsCheckablePropChanged(DpObject* target, DpPropChangedEventArg* e)
{
    MenuItem* pItem = DynamicCast<MenuItem>(target);
    pItem->UpdateRole();
}

bool MenuItem::IsSubmenuOpen()
{
    if (!_submenuPopup)
    {
        return false;
    }
    else
    {
        return _submenuPopup->IsOpen();
    }
}

void MenuItem::UpdateRole() 
{
    Integer* roleType = NULL;

    if (!IsCheckable() && HasItems()) 
    {
        if (DynamicCast<Menu>(GetParent())) 
        { 
            roleType = MenuItemRoleBox::TopLevelHeaderBox;
        }
        else
        {
            roleType = MenuItemRoleBox::SubmenuHeaderBox;
        } 
    }
    else 
    { 
        if (DynamicCast<Menu>(GetParent())) 
        { 
            roleType = MenuItemRoleBox::TopLevelItemBox;
        }
        else
        { 
            roleType = MenuItemRoleBox::SubmenuItemBox;
        } 
    } 

    SetValue(RoleProperty, roleType); 
}

bool MenuItem::IsSubmenu()
{
    return HasItems();
}

Element* MenuItem::GetContainerForItemOverride()
{
    return new MenuItem();
}

bool MenuItem::IsItemItsOwnContainer(Object* item)
{
    return (MenuItem::RTTIType() == item->GetRTTIType());
}

/////////////////////////////////////////////////////////
// 菜单项事件
//

void MenuItem::OnApplyTemplate()
{
    HeaderedItemsControl::OnApplyTemplate();
    _submenuPopup = GetTemplateChild(_U("PART_Popup"));
    if (_submenuPopup)
    {
        _submenuPopup->SetPlacementTarget(this);
        _submenuPopup->Closed += EventHandler(this, &MenuItem::OnPopupClosed);

        MenuItem* parent = DynamicCast<MenuItem>(GetParent());
        if (NULL != parent)
        {
            _submenuPopup->SetParentPopup(parent->_submenuPopup.get());
        }
    }
}

Element* MenuItem::GetTargetElement()
{
    Element* targetElem = NULL;

    if (NULL != GetMenuPopup() && NULL != GetMenuPopup()->GetRootPopup())
    {
        targetElem = GetMenuPopup()->GetRootPopup()->GetPlacementTarget();
    }

    if (NULL == targetElem)
    {
        targetElem = Keyboard::GetFocusedElement(); 
    }

    return targetElem;
}

void MenuItem::OnClick()
{
    Object* pItem = GetContainerItem();
    Element* targetElem = GetTargetElement();

    if (NULL != targetElem)
    {
        suic::ClickEventArg e(targetElem, ClickEvent);
        e.OnOverrideOriginalSource(this);
        targetElem->RaiseEvent(&e);
    }
}

void MenuItem::OnPopupClosed(Object* source, EventArg* e)
{
    SetValue(IsSubmenuOpenProperty, Boolean::False);
}

void MenuItem::OnSubmenuOpened(RoutedEventArg* e)
{
    RaiseEvent(e);
}

void MenuItem::OnSubmenuClosed(RoutedEventArg* e)
{
    RaiseEvent(e);
}

void MenuItem::OnChecked(RoutedEventArg* e)
{
    RaiseEvent(e);
}

void MenuItem::OnUnchecked(RoutedEventArg* e)
{
    RaiseEvent(e);
}

///////////////////////////////////////////////////////////////////////////////////////
// MenuItem
//

void MenuItem::OnInitialized(EventArg* e)
{
    HeaderedItemsControl::OnInitialized(e);
    UpdateRole();
}

void MenuItem::OnItemsChanged(NotifyCollChangedEventArg* e)
{
    UpdateRole();
    HeaderedItemsControl::OnItemsChanged(e);
}

void MenuItem::OnVisualParentChanged(Element* oldParent)
{
    UpdateRole();
    HeaderedItemsControl::OnVisualParentChanged(oldParent);
}

bool MenuItem::IsSelected()
{
    if (_submenuPopup && _submenuPopup->IsOpen())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MenuItem::SetIsSelected(bool val)
{

}

void MenuItem::CloseChildSubmenu()
{
}

void MenuItem::CloseSubmenu()
{
    if (_submenuPopup && _submenuPopup->IsOpen())
    {
        CloseChildSubmenu();
        _submenuPopup->Close();

        if (IsSelected())
        {
            SetIsSelected(false);
            return;
        }
        UpdateVisualState(true);
    }
}

void MenuItem::PrepareHeaderedItemsControl(ItemEntry* item, ItemsControl* parentItemsControl)
{
    Separator* pSep = RTTICast<Separator>(item->GetItem());

    if (NULL != pSep)
    {
        _isSeparator = true;
    }
    else
    {
        _isSeparator = false;

        if (HeaderIsItem())
        {
            SetDataContext(item->GetItem());
        }
    }

    HeaderedItemsControl::PrepareHeaderedItemsControl(item, parentItemsControl);
}

void MenuItem::OnMouseEnter(MouseButtonEventArg* e)
{
    if (!IsEnabled())
    {
        return;
    }

    e->SetHandled(true);

    if (!_isSeparator)
    {
        SetValue(IsHighlightedProperty, Boolean::True);
        
        ItemsControl* itemsCtrl = DynamicCast<ItemsControl>(GetParent());
        Panel* itemsHost = NULL;

        if (NULL != itemsCtrl)
        {
            itemsHost = itemsCtrl->GetItemsHost();
        }

        Debug::Trace(_U("MenuItem::OnMouseEnter -> %ld\n"), (DWORD)(LONG_PTR)itemsHost);

        if (NULL!= itemsHost)
        {
            for (int i = 0; i < itemsHost->GetVisibleChildrenCount(); ++i)
            {
                MenuItem* menuItem = DynamicCast<MenuItem>(itemsHost->GetChildren()->GetAt(i));

                if (menuItem->IsSubmenuOpen())
                {
                    if (menuItem == this)
                    {
                        return;
                    }

                    // 关掉子菜单
                    GetMenuPopup()->ClosePopup(menuItem->_submenuPopup.get());

                    break;
                }
            }
        }

        if (IsSubmenu())
        {
            if (GetMenuPopup()->IsInTracking())
            {
                Menu* menu = DynamicCast<Menu>(GetParent());

                SetValue(IsSubmenuOpenProperty, Boolean::True);

                if (NULL != _submenuPopup)
                {
                    if (NULL != menu)
                    {
                        _submenuPopup->SetPlacement(PlacementMode::pmBottom);
                        GetMenuPopup()->TrackMenuPopup(_submenuPopup.get(), this, GetParent());
                    }
                    else
                    {
                        _submenuPopup->SetPlacement(PlacementMode::pmRight);
                        GetMenuPopup()->TrackSubMenuPopup(_submenuPopup.get(), this);
                    }
                }
            }
        }
        else
        {
            Selector::SelectContainer(this, true);
        }
    }
    HeaderedItemsControl::OnMouseEnter(e);
}

void MenuItem::OnMouseLeave(MouseButtonEventArg* e)
{
    MenuItem* overItem = DynamicCast<MenuItem>(e->GetOriginalSource());
    VisualTreeOp::GetVisualRoot(this);

    SetValue(IsHighlightedProperty, Boolean::False);

    //
    // 如果是子菜单弹出并且在同一个菜单元素内，则关闭
    //
    if (IsSubmenuOpen() && overItem && overItem->GetParent() == GetParent())
    {
     }

    Selector::SelectContainer(this, false);
    InvalidateVisual();

    e->SetHandled(true);
    HeaderedItemsControl::OnMouseLeave(e);
}

void MenuItem::HandleLeftButtonDown(MouseButtonEventArg* e)
{
    e->SetHandled(true);
    e->SetSource(this);

    if (_isSeparator)
    {
        ;
    }
    // 如果子菜单已经弹出并且已经打开，则关闭
    else if (GetMenuPopup()->IsInTracking() && 
        GetMenuPopup()->GetTrackingMenuItem() == this)
    {
        GetMenuPopup()->ClosePopup(_submenuPopup.get());
    }
    else
    {
        if (IsSubmenu())
        {
            Menu* menu = RTTICast<Menu>(GetParent());
            if (menu != NULL && NULL != _submenuPopup)
            {
                SetValue(IsSubmenuOpenProperty, Boolean::True);
                GetMenuPopup()->TrackMenuPopup(_submenuPopup.get(), this, menu);
            }
        }
        else if (IsEnabled())/* if (!GetItemEntry()->HasFlag(ItemEntry::fDisabled))*/
        {
            // 单击菜单
            if (IsCheckable())
            {
                SetIsChecked(!IsChecked());
            }
        }
    }
}

void MenuItem::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    Element::OnMouseLeftButtonDown(e);

    if (!e->IsHandled())
    {
        InvalidateVisual();
        HandleLeftButtonDown(e);
    }
}

void MenuItem::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    HeaderedItemsControl::OnMouseLeftButtonUp(e);

    if (_isSeparator)
    {
        ;
    }
    else if (GetMenuPopup()->IsInTracking() && 
        GetMenuPopup()->GetTrackingMenuItem() == this)
    {
        ;
    }
    else if (!IsSubmenu() && IsEnabled()/*!GetItemEntry()->HasFlag(ItemEntry::fDisabled)*/)
    {
        Object* target = this;
        ICommand* pCmd = DynamicCast<ICommand>(GetContainerItem());

        if (NULL == pCmd)
        {
            target = GetTargetElement();
            pCmd = GetCommandSource()->GetCommand();
            if (NULL == target)
            {
                target = this;
            }
        }

        GetMenuPopup()->CloseAllPopup(false);

        OnClick();

        if (pCmd)
        {
            if (pCmd->CanExecute(target, NULL))
            {
                pCmd->Execute(target, NULL);
            }
        }
    }
}

}
