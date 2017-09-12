
#include <System/Windows/CommandSource.h>
#include <Framework/Controls/ButtonBase.h>

#include <System/Tools/EventHelper.h>
#include <System/Tools/CommandOp.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>

#include <System/Input/Mouse.h>
#include <System/Input/KeyboardNavigation.h>

#include <Framework/Animation/AnimationAsSeries.h>

namespace suic
{

ImplementRTTIOfClass(ButtonBase, ContentControl)

DpProperty* ButtonBase::IsPressedProperty;
RoutedEvent* ButtonBase::ClickEvent;

ButtonBase::ButtonBase()
    : _animationType(0)
{
    _commandSource = NULL;
    _timer = new AssignerTimer();
    _timer->ref();
}

ButtonBase::~ButtonBase()
{
    if (_commandSource)
    {
        delete _commandSource;
    }
    _timer->unref();
}

void ButtonBase::StaticInit()
{
    if (IsPressedProperty == NULL)
    {
        ContentControl::StaticInit();
        ClickEvent = EventHelper::RegisterRoutedEvent(_U("Click"), RoutingStrategy::Bubble, ClickEventHandler::RTTIType(), RTTIType());

        IsPressedProperty = DpProperty::RegisterReadOnly(_T("IsPressed"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone, &ButtonBase::OnIsPressedPropChanged));
        IsMouseOverProperty->OverrideMetadata(RTTIType()
            , DpPropMemory::GetPropMeta(suic::Boolean::False, PropMetadataOptions::AffectsNone, &OnVisualStatePropChanged));

        KeyboardNavigation::AcceptsReturnProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::True));
    }
}

void ButtonBase::AddClick(ClickEventHandler* handler)
{
    AddHandler(ClickEvent, handler);
}

void ButtonBase::RemoveClick(ClickEventHandler* handler)
{
    RemoveHandler(ClickEvent, handler);
}

ICommandSource* ButtonBase::GetCommandSource()
{
    if (!_commandSource)
    {
        _commandSource = new CommandSource();
        _commandSource->SetCommandTarget(this);
    }

    return _commandSource;
}

void ButtonBase::OnCheckTimer(Object* sender, EventArg* e)
{
    if (!IsLoaded())
    {
        _timer->SetEnabled(false);
    }
    else if (MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress)
    {
        HandleIsMouseOverChanged();
    }
    else
    {
        _timer->SetEnabled(false);
        SetIsPressed(false);
    }
}

void ButtonBase::OnIsPressedPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ButtonBase* btn = RTTICast<ButtonBase>(d);
    btn->OnIsPressedChanged(e);
}

void ButtonBase::OnIsPressedChanged(DpPropChangedEventArg* e)
{
    Control::OnVisualStatePropChanged(this, e);
}

void ButtonBase::OnKeyDown(KeyboardEventArg* e)
{
    bool bOk = e->GetKey() == Key::kSpace;
    if (KeyboardNavigation::IsAcceptsReturn(this) && e->GetKey() == Key::kReturn)
    {
        bOk = true;
    }

    if (bOk)
    {
        SetIsPressed(true);
        e->SetHandled(true);
    }
}

void ButtonBase::OnKeyUp(KeyboardEventArg* e)
{
    ContentControl::OnKeyUp(e);

    bool bOk = e->GetKey() == Key::kSpace;
    if (KeyboardNavigation::IsAcceptsReturn(this) && e->GetKey() == Key::kReturn)
    {
        bOk = true;
    }

    if (bOk)
    {
        if (MouseDevice::GetLeftButton() == MouseButtonState::mbMouseRelease)
        {
            bool flag = IsPressed();

            if (flag)
            {
                SetIsPressed(false);
                OnClick();
            }
        }
        else if (IsMouseCaptured())
        {
            ;
        }

        e->SetHandled(true);
    }
}

void ButtonBase::OnTextInput(suic::KeyboardEventArg* e)
{
    if (e->GetKey() == Key::kSpace)
    {
        e->SetHandled(true);
    }
}

void ButtonBase::OnMouseEnter(MouseButtonEventArg* e)
{
    ContentControl::OnMouseEnter(e);
    if (HandleIsMouseOverChanged())
    {
        e->SetHandled(true);
    }
}

void ButtonBase::OnMouseLeave(MouseButtonEventArg* e)
{
    ContentControl::OnMouseLeave(e);
    if (HandleIsMouseOverChanged())
    {
        e->SetHandled(true);
    }
}

void ButtonBase::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    e->SetHandled(true);

    Focus();

    if ((e->GetClickCount() % 2) != 0 && 
        MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress)
    {
        SetCaptureMouse();
        if (!IsPressed())
        {
            SetIsPressed(true);
        }
    }

    ContentControl::OnMouseLeftButtonDown(e);
}

void ButtonBase::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    ReleaseCaptureMouse();
    e->SetHandled(true);

    //
    // 只有鼠标位于元素上才出发Click事件
    //
    if (IsPressed() && HitTestMouse(e->GetMousePoint()))
    {
        OnClick();
    }

    SetIsPressed(false);

    ContentControl::OnMouseLeftButtonUp(e);
}

void ButtonBase::OnClick()
{
    ClickEventArg e(this, ClickEvent);
    RaiseEvent(&e);
    CommandOp::ExecuteCommand(GetCommandSource());
}

void ButtonBase::SetIsPressed(bool pressed)
{
    if (pressed)
    {
        SetValue(IsPressedProperty, BOOLTOBOOLEAN(pressed));
    }
    else
    {
        ClearValue(IsPressedProperty);
    }
}

bool ButtonBase::HandleIsMouseOverChanged()
{
    if (MouseDevice::GetLeftButton() != MouseButtonState::mbMousePress)
    {
        return false;
    }

    if (IsMouseOver())
    {
        SetIsPressed(true);
    }
    else
    {
        SetIsPressed(false);
    }

    return true;
}

}
