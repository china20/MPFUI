
#include <Extend/Controls/TabItem.h>
#include <Extend/Controls/TabControl.h>
#include <Framework/Controls/Selector.h>

#include <System/Tools/EventHelper.h>
#include <System/Input/KeyboardNavigation.h>

namespace suic
{

DpProperty* TabItem::IsSelectedProperty;
DpProperty* TabItem::TabStripPlacementProperty;

ImplementRTTIOfClass(TabItem, HeaderedContentControl)

void TabItem::StaticInit()
{
    if (NULL == IsSelectedProperty)
    {
        Selector::StaticInit();

        IsSelectedProperty = Selector::IsSelectedProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsParentMeasure, PropChangedCallback(&TabItem::OnIsSelectedPropChanged)));
        TabStripPlacementProperty = DpProperty::RegisterReadOnly("TabStripPlacement", RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(Dock::dockTop), 0, PropChangedCallback()));
        TabStripPlacementProperty->SetConvertValueCb(DockConvert::Convert);
        //EventHelper::RegisterClassHandler(RTTIType(), AccessKeyManager.AccessKeyPressedEvent, new AccessKeyPressedEventHandler(TabItem::OnAccessKeyPressed));
        KeyboardNavigation::DirectionalNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knContained)));
        KeyboardNavigation::TabNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knLocal)));
    }
}

TabItem::TabItem()
    : _field(0)
{
}

TabItem::~TabItem()
{

}

int TabItem::CoerceTabStripPlacement(DpObject* d, Object* value)
{
    TabItem* tabItem = RTTICast<TabItem>(d);
    TabControl* tabControlParent = tabItem->GetTabControlParent();
    if (tabControlParent == NULL)
    {
        return value->ToInt();
    }
    return tabControlParent->GetTabStripPlacement();
}

bool TabItem::GetBoolField(BoolField field)
{
    return ((_field & field) != BoolField::DefaultValue);
}

void TabItem::HandleIsSelectedChanged(bool newValue, RoutedEventArg* e)
{
    RaiseEvent(e);
}

/*protected override void TabItem::OnAccessKey(AccessKeyEventArgs e)
{
    this.SetFocus();
}

private static void TabItem::OnAccessKeyPressed(object sender, AccessKeyPressedEventArgs e)
{
    if (!e.Handled && (e.Scope == null))
    {
        TabItem item = sender as TabItem;
        if (e.Target == null)
        {
            e.Target = item;
        }
        else if (!item.IsSelected)
        {
            e.Scope = item;
            e.Handled = true;
        }
    }
}

internal override void TabItem::OnAncestorChanged()
{
    base.CoerceValue(TabStripPlacementProperty);
}*/

void TabItem::OnContentChanged(Object* oldContent, Object* newContent)
{
    HeaderedContentControl::OnContentChanged(oldContent, newContent);

    if (IsSelected())
    {
        TabControl* tabControlParent = GetTabControlParent();
        if (tabControlParent != NULL)
        {
            tabControlParent->SetSelectedContent(newContent);
        }
    }
}

void TabItem::OnContentTemplateChanged(DataTemplate* oldContentTemplate, DataTemplate* newContentTemplate)
{
    HeaderedContentControl::OnContentTemplateChanged(oldContentTemplate, newContentTemplate);
    if (IsSelected())
    {
        TabControl* tabControlParent = GetTabControlParent();
        if (tabControlParent != NULL)
        {
            tabControlParent->SetSelectedContentTemplate(newContentTemplate);
        }
    }
}

void TabItem::OnContentTemplateSelectorChanged(DataTemplateSelector* oldContentTemplateSelector, DataTemplateSelector* newContentTemplateSelector)
{
    HeaderedContentControl::OnContentTemplateSelectorChanged(oldContentTemplateSelector, newContentTemplateSelector);
    if (IsSelected())
    {
        TabControl* tabControlParent = GetTabControlParent();
        if (tabControlParent != NULL)
        {
            tabControlParent->SetSelectedContentTemplateSelector(newContentTemplateSelector);
        }
    }
}

void TabItem::OnIsSelectedPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TabItem* container = RTTICast<TabItem>(d);
    bool newValue = e->GetNewValue()->ToBool();
    TabControl* tabControlParent = container->GetTabControlParent();

    if (NULL != tabControlParent)
    {
        tabControlParent->NotifyIsSelectedChanged(container, newValue);
    }

    if (newValue)
    {
        RoutedEventArg re(container, Selector::SelectedEvent);
        container->OnSelected(&re);
    }
    else
    {
        RoutedEventArg re(container, Selector::UnselectedEvent);
        container->OnSelected(&re);
    }

    if (newValue)
    {
        //Binding* binding = new Binding("Margin");
        //binding->SetSource(container);
        //BindingOperations.SetBinding(container, KeyboardNavigation.DirectionalNavigationMarginProperty, binding);
    }
    else
    {
        //BindingOperations.ClearBinding(container, KeyboardNavigation.DirectionalNavigationMarginProperty);
    }

    container->UpdateVisualState(false);
}

void TabItem::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    if (((e->GetSource() == this) || !IsSelected()) && SetFocus())
    {
        e->SetHandled(true);
    }
    HeaderedContentControl::OnMouseLeftButtonDown(e);
}

void TabItem::OnPreviewGotKeyboardFocus(KeyboardFocusEventArg* e)
{
    HeaderedContentControl::OnPreviewGotKeyboardFocus(e);

    if ((!e->IsHandled() && (e->GetNewFocus() == this)) && (!IsSelected() && (GetTabControlParent() != NULL)))
    {
        HeaderedContentControl::SetCurrentValue(IsSelectedProperty, Boolean::True);
        if (e->GetOldFocus() != Keyboard::GetFocusedElement())
        {
            e->SetHandled(true);
        }
        else if (GetBoolField(BoolField::SetFocusOnContent))
        {
            TabControl* tabControlParent = GetTabControlParent();
            if (tabControlParent != NULL)
            {
                ContentPresenter* selectedContentPresenter = tabControlParent->GetSelectedContentPresenter();
                if (selectedContentPresenter != NULL)
                {
                    tabControlParent->UpdateLayout();
                    if (selectedContentPresenter->MoveFocus(FocusNavDirection::fndFirst))
                    {
                        e->SetHandled(true);
                    }
                }
            }
        }
    }
}

void TabItem::OnSelected(RoutedEventArg* e)
{
    HandleIsSelectedChanged(true, e);
}

void TabItem::OnUnselected(RoutedEventArg* e)
{
    HandleIsSelectedChanged(false, e);
}

void TabItem::SetBoolField(BoolField field, bool val)
{
    if (val)
    {
        _field |= field;
    }
    else
    {
        _field &= ~field;
    }
}

bool TabItem::SetFocus()
{
    bool flag = false;

    if (!GetBoolField(BoolField::SettingFocus))
    {
        TabItem* focusedElement = RTTICast<TabItem>(Keyboard::GetFocusedElement());
        bool flag2 = ((focusedElement == this) || (focusedElement == NULL)) || (focusedElement->GetTabControlParent() != GetTabControlParent());
        
        SetBoolField(BoolField::SettingFocus, true);
        SetBoolField(BoolField::SetFocusOnContent, flag2);

        try
        {
            flag = HeaderedContentControl::Focus() || flag2;
        }
        catch (...)
        {
            
        }

        SetBoolField(BoolField::SettingFocus, false);
        SetBoolField(BoolField::SetFocusOnContent, false);
    }
    return flag;
}

bool TabItem::IsSelected()
{
    return GetValue(IsSelectedProperty)->ToBool();
}

void TabItem::SetSelected(bool val)
{
    SetValue(IsSelectedProperty, BOOLTOBOOLEAN(val));
}

TabControl* TabItem::GetTabControlParent()
{
    return RTTICast<TabControl>(ItemsControl::ItemsControlFromItemContainer(this));
}

Dock TabItem::GetTabStripPlacement()
{
    return (Dock)GetValue(TabStripPlacementProperty)->ToInt();
}

}
