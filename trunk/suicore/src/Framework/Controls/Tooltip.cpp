/////////////////////////////////////////////////////////////////////////
// uitooltip.cpp

#include <Framework/Controls/Tooltip.h>
#include <System/Interop/InternalOper.h>
#include <System/Interop/System.h>

namespace suic
{

ImplementRTTIOfClass(ToolTip, ContentControl)

DpProperty* ToolTip::IsOpenProperty;
DpProperty* ToolTip::PlacementProperty;
DpProperty* ToolTip::PlacementRectangleProperty;
DpProperty* ToolTip::PlacementTargetProperty;
DpProperty* ToolTip::VerticalOffsetProperty;
DpProperty* ToolTip::HorizontalOffsetProperty;

ToolTip* ToolTip::_toolTip;

ToolTip::ToolTip()
{
    _owner = NULL;
    _popup = NULL;
    _timer = NULL;
}

ToolTip::~ToolTip()
{
    if (_popup)
    {
        _popup->unref();
    }
}

void ToolTip::StaticInit()
{
    if (NULL == IsOpenProperty)
    {
        Popup::StaticInit();
        ContentControl::StaticInit();

        IsOpenProperty = Popup::IsOpenProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, 0));
        PlacementProperty = Popup::PlacementProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(PlacementModeBox::MousePointBox, 0));
        PlacementRectangleProperty = Popup::PlacementRectangleProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(new ORect(), 0));
        PlacementTargetProperty = Popup::PlacementTargetProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(NULL, 0));
        VerticalOffsetProperty = Popup::VerticalOffsetProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), 0));
        HorizontalOffsetProperty = Popup::HorizontalOffsetProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), 0));
    }
}

ToolTip* ToolTip::Ins()
{
    if (NULL == _toolTip)
    {
        _toolTip = new ToolTip();
        _toolTip->ref();
    }
    return _toolTip;
}

void ToolTip::Hide()
{
    if (NULL != _popup)
    {
        _popup->Hide();
    }
}

void ToolTip::Close()
{
    if (NULL != _popup)
    {
        _popup->Hide();
    }
}

void ToolTip::SetOwner(Element* owner)
{
    _owner = owner;
}

Element* ToolTip::GetOwner() const
{
    return _owner;
}

void ToolTip::Show(Point pt)
{
    if (NULL == _popup)
    {
        _popup = new Popup();
        _popup->ref();

        _popup->Opening += OpeningEventHandler(this, &ToolTip::OnPopupOpening);
        _popup->Opened += EventHandler(this, &ToolTip::OnPopupOpened);
        _popup->Closed += EventHandler(this, &ToolTip::OnPopupClosed);
        _popup->Closing += EventHandler(this, &ToolTip::OnPopupClosing);
    }

    SetWidth(Numeric::NanInt32);
    SetHeight(Numeric::NanInt32);

    _popup->SetChild(this);
    _popup->SetPlacement(PlacementMode::pmCustom);
    //_popup->SetPlacementTarget(_owner);

    _popup->GetPositionInfo()->x = SystemParameters::TransformXToDevice(pt.x);
    _popup->GetPositionInfo()->y = SystemParameters::TransformYToDevice(pt.y);

    if (_popup->IsOpen())
    {
        _popup->InvalidatePopupRoot();
    }
    else
    {
        _popup->SetIsOpen(true);
    }

    if (!_popup->IsOpen())
    {
        Clear();
    }
    else
    {
    }
}

PlacementMode ToolTip::GetPlacement()
{
    return (PlacementMode)GetValue(PlacementProperty)->ToInt();
}

void ToolTip::SetPlacement(PlacementMode val)
{
    SetValue(PlacementProperty, new Integer(val));
}

Rect ToolTip::GetPlacementRectangle()
{
    return ORect::FromObj(GetValue(PlacementRectangleProperty));
}

void ToolTip::GetPlacementRectangle(Rect val)
{
    SetValue(PlacementRectangleProperty, new ORect(val));
}

Element* ToolTip::GetPlacementTarget()
{
    return RTTICast<Element>(GetValue(PlacementTargetProperty));
}

void ToolTip::SetPlacementTarget(Element* target)
{
    SetValue(PlacementTargetProperty, target);
}

int ToolTip::GetVerticalOffset()
{
    return GetValue(VerticalOffsetProperty)->ToInt();
}

void ToolTip::SetVerticalOffset(int val)
{
    SetValue(VerticalOffsetProperty, new Integer(val));
}

int ToolTip::GetHorizontalOffset()
{
    return GetValue(HorizontalOffsetProperty)->ToInt();
}

void ToolTip::SetHorizontalOffset(int val)
{
    SetValue(HorizontalOffsetProperty, new Integer(val));
}

void ToolTip::SetToolTip(String tooltip)
{
    SetContent(new OString(tooltip));
}

void ToolTip::Clear()
{
    SetContent(NULL);
    SetOwner(NULL);

    if (NULL != _popup)
    {
        PopupRoot* popupRoot = _popup->GetPopupRoot();

        if (popupRoot)
        {
            popupRoot->SetChild(NULL);
            popupRoot->SetWidth(Numeric::NanInt32);
            popupRoot->SetHeight(Numeric::NanInt32);
        }

        _popup->SetIsOpen(false);
        _popup->SetChild(NULL);
        _popup->SetPlacementTarget(NULL);
        _popup->SetWidth(Numeric::NanInt32);
        _popup->SetHeight(Numeric::NanInt32);
    }
}

void ToolTip::OnHitTest(HitResultEventArg* e)
{
}

void ToolTip::OnPopupClosed(Object* sender, EventArg* e)
{
    Clear();
}

void ToolTip::OnPopupOpened(Object* sender, EventArg* e)
{

}

void ToolTip::OnPopupClosing(Object* sender, EventArg* e)
{
    ToolTipEventArg te(false, NULL);
    Element* target = GetPlacementTarget();
    if (NULL != target)
    {
        target->RaiseEvent(&te);
    }

    if (!te.IsHandled())
    {
        CancelEventArg* cee = (CancelEventArg*)e;
        cee->SetCancel(true);
        Hide();
        Clear();
    }
}

void ToolTip::OnPopupOpening(Object* sender, OpeningEventArg* e)
{
    ToolTipEventArg te(true, e->GetHwndParam());
    Element* target = GetPlacementTarget();
    if (NULL != target)
    {
        target->RaiseEvent(&te);
    }
}

void ToolTip::OnClosedTimer(Object* sender, EventArg* e)
{
    Close();
}

void ToolTip::OnShowWindow()
{
}

void ToolTip::OnContentChanged(Object* oldContent, Object* newContent)
{
    ContentControl::OnContentChanged(oldContent, newContent);
}

}
