/////////////////////////////////////////////////////////////////////////
// ListBoxItem.cpp

#include <Extend/Controls/ListBox.h>
#include <Extend/Controls/ListBoxItem.h>
#include <System/Input/KeyboardNavigation.h>

namespace suic
{

ImplementRTTIOfClass(ListBoxItem, ContentControl)

DpProperty* ListBoxItem::IsSelectedProperty;

RoutedEvent* ListBoxItem::SelectedEvent;
RoutedEvent* ListBoxItem::UnselectedEvent;

void ListBoxItem::StaticInit()
{
    if (NULL == IsSelectedProperty)
    {
        IsMouseOverProperty->OverrideMetadata(RTTIType()
            , DpPropMemory::GetPropMeta(suic::Boolean::False, PropMetadataOptions::AffectsNone, &OnVisualStatePropChanged));
        IsSelectedProperty = Selector::IsSelectedProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, 0, &ListBoxItem::OnIsSelectedChanged));

        KeyboardNavigation::DirectionalNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knContinue)));
        KeyboardNavigation::TabNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knNone)));
        //KeyboardNavigation::DirectionalNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knOnce)));
        //KeyboardNavigation::TabNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knLocal)));
    }
}

void ListBoxItem::OnIsSelectedChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ListBoxItem* container = RTTICast<ListBoxItem>(d);
    bool newValue = e->GetNewValue()->ToBool();

    Selector* parentSelector = container->GetParentSelector();
    if (NULL != parentSelector)
    {
        parentSelector->NotifyIsSelectedChanged(container, newValue);
    }

    container->SetSelectedFlag(ListBoxItem::sfSelected, newValue);
    container->UpdateVisualState(false);
}

//----------------------------------------------------

ListBoxItem::ListBoxItem()
    : _selectedFlag(0)
{
    WriteFlag(CoreFlags::IsForbidInvalidateDC, true);
}

ListBoxItem::~ListBoxItem()
{

}

Selector* ListBoxItem::GetParentSelector()
{
    return RTTICast<Selector>(ItemsControl::ItemsControlFromItemContainer(this));
}

ListBox* ListBoxItem::GetParentListBox()
{
    return RTTICast<ListBox>(GetParentSelector());
}

bool ListBoxItem::GetIsSelected()
{
    return ContainsSelectedFlag(ListBoxItem::sfSelected);
    //return GetValue(IsSelectedProperty)->ToBool();
}

void ListBoxItem::SetIsSelected(bool val)
{
    SetValue(IsSelectedProperty, BOOLTOBOOLEAN(val));
}

void ListBoxItem::OnPreApplyTemplate()
{
    ContentControl::OnPreApplyTemplate();
}

void ListBoxItem::OnInitialized(EventArg* e)
{
    ContentControl::OnInitialized(e);
}

void ListBoxItem::OnMouseEnter(MouseButtonEventArg* e)
{

}
void ListBoxItem::OnMouseLeave(MouseButtonEventArg* e)
{

}

void ListBoxItem::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    if (!e->IsHandled())
    {
        Focus();
        e->SetHandled(true);
        HandleMouseButtonDown(MouseButton::mbLeftButton);
    }

    ContentControl::OnMouseLeftButtonDown(e);
}

void ListBoxItem::OnMouseRightButtonDown(MouseButtonEventArg* e)
{

}

void ListBoxItem::OnSelected(RoutedEventArg* e)
{
    RaiseEvent(e);
}

void ListBoxItem::OnUnselected(RoutedEventArg* e)
{
    RaiseEvent(e);
}

void ListBoxItem::OnVisualParentChanged(DpObject* oldParent)
{

}

void ListBoxItem::HandleIsSelectedChanged(bool newValue, RoutedEventArg* e)
{

}

void ListBoxItem::HandleMouseButtonDown(MouseButton mouseButton)
{
    if (IsFocused())
    {
        ListBox* parentListBox = GetParentListBox();

        if (parentListBox != NULL)
        {
            SetSelectedFlag(ListBoxItem::sfFromMouse, true);
            parentListBox->NotifyListItemClicked(this, mouseButton);
            SetSelectedFlag(ListBoxItem::sfFromMouse, false);
        }
    }
}

}
