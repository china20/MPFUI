
#include <Framework/Controls/ToggleButton.h>

#include <System/Input/Mouse.h>
#include <System/Tools/EventHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>
#include <System/Windows/CoreTool.h>

namespace suic
{

ImplementRTTIOfClass(ToggleButton, ButtonBase)

RoutedEvent* ToggleButton::CheckedEvent;
RoutedEvent* ToggleButton::UncheckedEvent;
RoutedEvent* ToggleButton::IndeterminateEvent;

DpProperty* ToggleButton::IsThreeStateProperty;
DpProperty* ToggleButton::IsCheckedProperty;

ToggleButton::ToggleButton()
{
}

ToggleButton::~ToggleButton()
{
}

void ToggleButton::StaticInit()
{
    if (IsCheckedProperty == NULL)
    {
        ButtonBase::StaticInit();
        IsCheckedProperty = DpProperty::Register(_T("IsChecked"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender, &ToggleButton::OnIsCheckedChanged));
        IsThreeStateProperty = DpProperty::Register(_T("IsThreeState"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));

        CheckedEvent = EventHelper::RegisterRoutedEvent(_U("Checked"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
        UncheckedEvent = EventHelper::RegisterRoutedEvent(_U("Unchecked"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
        IndeterminateEvent = EventHelper::RegisterRoutedEvent(_U("Indeterminate"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
    }
}

void ToggleButton::AddCheckedEvent(RoutedEventHandler* h)
{
    AddHandler(CheckedEvent, h, false);
}

void ToggleButton::RemoveCheckedEvent(RoutedEventHandler* h)
{
    RemoveHandler(CheckedEvent, h);
}

void ToggleButton::AddUncheckedEvent(RoutedEventHandler* h)
{
    AddHandler(UncheckedEvent, h, false);
}

void ToggleButton::RemoveUncheckedEvent(RoutedEventHandler* h)
{
    RemoveHandler(UncheckedEvent, h);
}

void ToggleButton::AddIndeterminateEvent(RoutedEventHandler* h)
{
    AddHandler(IndeterminateEvent, h, false);
}

void ToggleButton::RemoveIndeterminateEvent(RoutedEventHandler* h)
{
    RemoveHandler(IndeterminateEvent, h);
}

void ToggleButton::OnIsCheckedChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ToggleButton* elem = RTTICast<ToggleButton>(d);
    Boolean* oldValue = RTTICast<Boolean>(e->GetOldValue());
    Boolean* newValue = RTTICast<Boolean>(e->GetNewValue());

    if (Boolean::True == newValue)
    {
        RoutedEventArg e(elem, CheckedEvent);
        elem->OnChecked(&e);
    }
    else if (Boolean::False == newValue)
    {
        RoutedEventArg e(elem, UncheckedEvent);
        elem->OnUnchecked(&e);
    }
    else
    {
        RoutedEventArg e(elem, IndeterminateEvent);
        elem->OnIndeterminate(&e);
    }

    elem->UpdateVisualState(false);
}

void ToggleButton::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    e->SetHandled(true);
    Focus();

    if (MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress)
    {
        SetCaptureMouse();
        if (!IsPressed())
        {
            SetIsPressed(true);
        }
    }

    ContentControl::OnMouseLeftButtonDown(e);
}

void ToggleButton::OnChecked(RoutedEventArg* e)
{
    ButtonBase::RaiseEvent(e);
}

void ToggleButton::OnUnchecked(RoutedEventArg* e)
{
    ButtonBase::RaiseEvent(e);
}

void ToggleButton::OnIndeterminate(RoutedEventArg* e)
{
    ButtonBase::RaiseEvent(e);
}

void ToggleButton::OnToggle()
{
    Boolean* isChecked = GetChecked();
    Boolean* nullable = Boolean::Nullable;

    if (isChecked == Boolean::True)
    {
        nullable = IsThreeState() ? Boolean::Nullable : Boolean::False;
    }
    else
    {
        if (!IsThreeState())
        {
            nullable = Boolean::True;
        }
        else
        {
            if (isChecked != Boolean::Nullable)
            {
                nullable = Boolean::True;
            }
            else
            {
                nullable = Boolean::False;
            }
        }
    }

    SetValue(IsCheckedProperty, nullable);
}

void ToggleButton::OnClick()
{
    OnToggle();
    ButtonBase::OnClick();
}

Boolean* ToggleButton::GetChecked()
{
    Boolean* bl = RTTICast<Boolean>(GetValue(IsCheckedProperty));
    return bl;
}

bool ToggleButton::IsIndeterminate()
{
    return GetChecked() == Boolean::Nullable;
}

bool ToggleButton::IsChecked()
{
    return GetChecked() == Boolean::True;
}

void ToggleButton::SetChecked(Boolean* val)
{
    if (val != NULL)
    {
        SetValue(IsCheckedProperty, val);
    }
}

}
