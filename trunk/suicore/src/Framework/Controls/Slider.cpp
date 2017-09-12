// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Slider.cpp

#include <stdio.h>
#include <System/Tools/Math.h>
#include <System/Tools/EventHelper.h>

#include <Framework/Controls/Canvas.h>
#include <Framework/Controls/Slider.h>
#include <Framework/Controls/RepeatButton.h>

namespace suic
{

ImplementRTTIOfClass(Slider, RangeBase)

static String TrackName = "PART_Track";
static String SelectionRangeElementName = "PART_SelectionRange";

DpProperty* Slider::OrientationProperty;
DpProperty* Slider::AutoToolTipPlacementProperty;
DpProperty* Slider::AutoToolTipPrecisionProperty;
DpProperty* Slider::DelayProperty;
DpProperty* Slider::IntervalProperty;
DpProperty* Slider::IsDirectionReversedProperty;
DpProperty* Slider::IsMoveToPointEnabledProperty;
DpProperty* Slider::IsSelectionRangeEnabledProperty;
DpProperty* Slider::IsSnapToTickEnabledProperty;

DpProperty* Slider::SelectionEndProperty;
DpProperty* Slider::SelectionStartProperty;

DpProperty* Slider::TickFrequencyProperty;
DpProperty* Slider::TickPlacementProperty;
DpProperty* Slider::TicksProperty;

Slider::Slider()
    : _previousValue(0)
    , _track(NULL)
    , _hideThumb(true)
    , _autoToolTip(NULL)
    , _selectionRangeElement(NULL)
{
}

Slider::~Slider()
{
}

Object* Slider::CoerceSelectionEnd(DpObject* d, Object* val)
{
    Slider* slider = RTTICast<Slider>(d);
    if (NULL != slider)
    {
        int num = val->ToInt();
        int selectionStart = (int)slider->GetSelectionStart();
        int maximum = slider->GetMaximum();

        if (num < selectionStart)
        {
            return new Integer(selectionStart);
        }

        if (num > maximum)
        {
            return new Integer(maximum);
        }
    }

    return val;

}

Object* Slider::CoerceSelectionStart(DpObject* d, Object* val)
{
    Slider* slider = RTTICast<Slider>(d);
    if (NULL != slider)
    {
        int num = val->ToInt();
        int minimum = slider->GetMinimum();
        int maximum = slider->GetMaximum();

        if (num < minimum)
        {
            return new Integer(minimum);
        }

        if (num > maximum)
        {
            return new Integer(maximum);
        }
    }
    return val;

}

void Slider::OnSelectionEndChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Slider* elem = RTTICast<Slider>(d);
    if (NULL != elem)
    {
        elem->UpdateSelectionRangeElementPositionAndSize();
    }
}

void Slider::OnSelectionStartChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Slider* slider = RTTICast<Slider>(d);

    if (NULL != slider)
    {
        Float oldValue = (Float)e->GetOldValue()->ToFloat();
        Float newValue = (Float)e->GetNewValue()->ToFloat();
        slider->CoerceValue(SelectionEndProperty);
        slider->UpdateSelectionRangeElementPositionAndSize();
    }
}

void Slider::OnThumbDragCompletedEvent(DpObject* sender, RoutedEventArg* e)
{
    Slider* slider = RTTICast<Slider>(sender);

    if (NULL != slider)
    {
        DragCompletedEventArg* dce = (DragCompletedEventArg*)e;
        slider->OnThumbDragCompleted(dce);
    }
}

void Slider::OnThumbDragDeltaEvent(DpObject* sender, RoutedEventArg* e)
{
    Slider* slider = RTTICast<Slider>(sender);

    if (NULL != slider)
    {
        DragDeltaEventArg* dde = (DragDeltaEventArg*)e;
        slider->OnThumbDragDelta(dde);
    }
}

void Slider::OnThumbDragStartedEvent(DpObject* sender, RoutedEventArg* e)
{
    Slider* slider = RTTICast<Slider>(sender);

    if (NULL != slider)
    {
        DragStartedEventArg* dse = (DragStartedEventArg*)e;
        slider->OnThumbDragStarted(dse);
    }
}

void Slider::OnMouseLeftButtonDownThunk(Element* sender, MouseButtonEventArg* e)
{
    if (e->GetMouseButton() == MouseButton::mbLeftButton)
    {
        Slider* slider = RTTICast<Slider>(sender);
        if (!slider->IsKeyboardFocusWithin())
        {
            e->SetHandled(slider->Focus() || e->IsHandled());
        }
    }
}

void Slider::StaticInit()
{
    if (NULL == OrientationProperty)
    {
        Thumb::StaticInit();
        RangeBase::StaticInit();

        OrientationProperty = DpProperty::Register(_T("Orientation"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(OrientationBox::HorizontalBox, PropMetadataOptions::AffectsMeasure));
        OrientationProperty->SetConvertValueCb(OrientationConvert::Convert);

        IsDirectionReversedProperty = DpProperty::Register("IsDirectionReversed", RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False));

        DelayProperty = RepeatButton::DelayProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(new Integer(500)));
        IntervalProperty = RepeatButton::IntervalProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(new Integer(300)));
        //AutoToolTipPlacementProperty = DpProperty::Register("AutoToolTipPlacement", typeof(AutoToolTipPlacement), typeof(Slider), new FrameworkPropertyMetadata(AutoToolTipPlacement.None), new ValidateValueCallback(Slider.IsValidAutoToolTipPlacement));
        //AutoToolTipPrecisionProperty = DpProperty::Register("AutoToolTipPrecision", typeof(int), typeof(Slider), new FrameworkPropertyMetadata(0), new ValidateValueCallback(Slider.IsValidAutoToolTipPrecision));

        IsSnapToTickEnabledProperty = DpProperty::Register("IsSnapToTickEnabled", RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False));
        TickPlacementProperty = DpProperty::Register("TickPlacement", RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt()));
        TickFrequencyProperty = DpProperty::Register("TickFrequency", RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(1.0)));
        TicksProperty = DpProperty::Register("Ticks", RTTIType(), IntAutoArray::RTTIType(), DpPropMemory::GetPropMeta(NULL));
        IsSelectionRangeEnabledProperty = DpProperty::Register("IsSelectionRangeEnabled", RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False));
        SelectionStartProperty = DpProperty::Register("SelectionStart", RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(0), PropMetadataOptions::AffectsNone, &Slider::OnSelectionStartChanged, &Slider::CoerceSelectionStart));
        SelectionEndProperty = DpProperty::Register("SelectionEnd", RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(0), PropMetadataOptions::AffectsNone, &Slider::OnSelectionEndChanged, &Slider::CoerceSelectionEnd));
        IsMoveToPointEnabledProperty = DpProperty::Register("IsMoveToPointEnabled", RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False));

        TicksProperty->SetConvertValueCb(IntAutoArrayConvert::Convert);
        
        RangeBase::MaximumProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(new OFloat(10.0f), PropMetadataOptions::AffectsMeasure, &RangeBase::OnMaximumPropChanged));

        EventHelper::RegisterClassHandler(RTTIType(), Thumb::DragStartedEvent, new RoutedEventHandler(&Slider::OnThumbDragStartedEvent), false);
        EventHelper::RegisterClassHandler(RTTIType(), Thumb::DragDeltaEvent, new RoutedEventHandler(&Slider::OnThumbDragDeltaEvent), false);
        EventHelper::RegisterClassHandler(RTTIType(), Thumb::DragCompletedEvent, new RoutedEventHandler(&Slider::OnThumbDragCompletedEvent), false);
        EventHelper::RegisterClassHandler(RTTIType(), MouseDownEvent, new MouseButtonEventHandler(&Slider::OnMouseLeftButtonDownThunk), true);
    }
}

Track* Slider::GetTrack() const
{
    return _track;
}

Orientation Slider::GetOrientation()
{
    return (Orientation)DpObject::GetValue(OrientationProperty)->ToInt();
}

void Slider::SetOrientation(Orientation val)
{
    DpObject::SetValue(OrientationProperty, OrientationBox::From(val));
}

void Slider::OnApplyTemplate()
{
    RangeBase::OnApplyTemplate();
    _selectionRangeElement = RTTICast<FrameworkElement>(GetTemplateChild(SelectionRangeElementName));
    _track = RTTICast<Track>(GetTemplateChild(TrackName));

    if (_autoToolTip != NULL)
    {
        _autoToolTip->SetPlacementTarget((_track != NULL) ? _track->GetThumb() : NULL);
    }
}

void Slider::OnDecreaseLarge()
{
    MoveToNextTick(-RangeBase::GetLargeChange());
}

void Slider::OnDecreaseSmall()
{
    MoveToNextTick(-RangeBase::GetSmallChange());
}

void Slider::OnArrange(const Size& arrangeSize)
{
    RangeBase::OnArrange(arrangeSize);
    UpdateSelectionRangeElementPositionAndSize();
}

int Slider::GetInterval()
{
    return DpObject::GetValue(IntervalProperty)->ToInt();
}

bool Slider::IsDirectionReversed()
{
    return DpObject::GetValue(IsDirectionReversedProperty)->ToBool();
}

void Slider::SetIsDirectionReversed(bool val)
{
    DpObject::SetValue(IsDirectionReversedProperty, BOOLTOBOOLEAN(val));
}

bool Slider::IsMoveToPointEnabled()
{
    return DpObject::GetValue(IsMoveToPointEnabledProperty)->ToBool();
}

void Slider::SetIsMoveToPointEnabled(bool val)
{
    DpObject::SetValue(IsMoveToPointEnabledProperty, BOOLTOBOOLEAN(val));
}

bool Slider::IsSnapToTickEnabled()
{
    return DpObject::GetValue(IsSnapToTickEnabledProperty)->ToBool();
}

void Slider::SetIsSnapToTickEnabled(bool val)
{
    DpObject::SetValue(IsSnapToTickEnabledProperty, BOOLTOBOOLEAN(val));
}

bool Slider::IsSelectionRangeEnabled()
{
    return DpObject::GetValue(IsSelectionRangeEnabledProperty)->ToBool();
}

void Slider::SetIsSelectionRangeEnabled(bool val)
{
    DpObject::SetValue(IsSelectionRangeEnabledProperty, BOOLTOBOOLEAN(val));
}

void Slider::SetInterval(int val)
{
    DpObject::SetValue(IntervalProperty, new Integer(val));
}

Float Slider::GetSelectionEnd()
{
    return DpObject::GetValue(SelectionEndProperty)->ToFloat();
}

void Slider::SetSelectionEnd(Float val)
{
    DpObject::SetValue(SelectionEndProperty, new OFloat(val));
}

Float Slider::GetSelectionStart()
{
    return DpObject::GetValue(SelectionStartProperty)->ToFloat();
}

void Slider::SetSelectionStart(Float val)
{
    DpObject::SetValue(SelectionStartProperty, new OFloat(val));
}

Float Slider::GetTickFrequency()
{
    return DpObject::GetValue(TickFrequencyProperty)->ToFloat();
}

void Slider::SetTickFrequency(Float val)
{
    DpObject::SetValue(TickFrequencyProperty, new OFloat(val));
}

IntAutoArray* Slider::GetTicks()
{
    return DynamicCast<IntAutoArray>(DpObject::GetValue(TicksProperty));
}

void Slider::SetTicks(IntAutoArray* val)
{
    DpObject::SetValue(TicksProperty, val);
}

int Slider::GetTickPlacement()
{
    return DpObject::GetValue(TickPlacementProperty)->ToInt();
}

void Slider::SetTickPlacement(int val)
{
    DpObject::SetValue(TickPlacementProperty, new Integer(val));
}

Float Slider::SnapToTick(Float value)
{
    if (IsSnapToTickEnabled())
    {
        Float minimum = RangeBase::GetMinimum();
        Float maximum = RangeBase::GetMaximum();
        IntAutoArray* ticks = GetTicks();

        if ((ticks != NULL) && (ticks->GetCount() > 0))
        {
            for (int i = 0; i < ticks->GetCount(); i++)
            {
                int num4 = ticks->GetItem(i);
                if (num4 == value)
                {
                    return value;
                }

                if ((num4 < value) && (num4 > minimum))
                {
                    minimum = num4;
                }
                else if ((num4 > value) && (num4 < maximum))
                {
                    maximum = num4;
                }
            }
        }
        else if (FloatUtil::GreaterThan(GetTickFrequency(), 0.0))
        {
            minimum = RangeBase::GetMinimum() + (Math::Round((Float)((value - RangeBase::GetMinimum()) / GetTickFrequency())) * GetTickFrequency());
            maximum = min(RangeBase::GetMaximum(), minimum + GetTickFrequency());
        }

        value = (value >= (int)(minimum + maximum) * 0.5) ? maximum : minimum;
    }

    return value;
}

void Slider::UpdateSelectionRangeElementPositionAndSize()
{
    Size renderSize;
    Size thumbSize;

    if ((GetTrack() != NULL) && !FloatUtil::LessThan(GetSelectionEnd(), GetSelectionStart()))
    {
        renderSize = GetTrack()->GetRenderSize();
        thumbSize = (GetTrack()->GetThumb() != NULL) ? GetTrack()->GetThumb()->GetRenderSize() : Size();
        Float numLength = RangeBase::GetMaximum() - RangeBase::GetMinimum();
        FrameworkElement* selectionRangeElement = _selectionRangeElement;

        if (selectionRangeElement != NULL)
        {
            Float num2 = 0.0f;

            if (GetOrientation() == Orientation::Horizontal)
            {
                if ((numLength == 0) || (renderSize.Width() == thumbSize.Width()))
                {
                    num2 = 0.0f;
                }
                else
                {
                    num2 = Math::Max(0.0f, ((renderSize.Width() - thumbSize.Width()) / (Float)numLength));
                }

                selectionRangeElement->SetWidth((GetSelectionEnd() - GetSelectionStart()) * num2);

                if (IsDirectionReversed())
                {
                    Canvas::SetLeft(selectionRangeElement, (thumbSize.Width() * 0.5) + (UFMAX((RangeBase::GetMaximum() - GetSelectionEnd()), 0.0f) * num2));
                }
                else
                {
                    Canvas::SetLeft(selectionRangeElement, (thumbSize.Width() * 0.5) + (UFMAX((GetSelectionStart() - RangeBase::GetMinimum()), 0.0f) * num2));
                }
            }
            else
            {
                if ((numLength == 0) || (renderSize.Height() == thumbSize.Height()))
                {
                    num2 = 0;
                }
                else
                {
                    num2 = max(0.0f, ((renderSize.Height() - thumbSize.Height()) / (Float)numLength));
                }

                selectionRangeElement->SetHeight((GetSelectionEnd() - GetSelectionStart()) * num2);
                
                if (IsDirectionReversed())
                {
                    Canvas::SetTop(selectionRangeElement, (thumbSize.Height() * 0.5) + (UFMAX((GetSelectionStart() - RangeBase::GetMinimum()), 0.0f) * num2));
                }
                else
                {
                    Canvas::SetTop(selectionRangeElement, (thumbSize.Height() * 0.5) + (UFMAX((RangeBase::GetMaximum() - GetSelectionEnd()), 0.0f) * num2));
                }
            }
        }
    }
}

void Slider::UpdateValue(int value)
{
    Float num = SnapToTick(value);

    if (!FloatUtil::AreClose(num, GetValue()))
    {
        if (FloatUtil::GreaterThan(num, GetMaximum()))
        {
            num = GetMaximum();
        }

        if (FloatUtil::GreaterThan(GetMinimum(), num))
        {
            num = GetMinimum();
        }

        DpObject::SetValue(ValueProperty, new OFloat(num));
    }
}

void Slider::OnValueChanged(Float oldValue, Float newValue)
{
    RangeBase::OnValueChanged(oldValue, newValue);
    UpdateSelectionRangeElementPositionAndSize();
}

void Slider::OnMaximizeValue()
{
    SetCurrentValue(RangeBase::ValueProperty, new OFloat(RangeBase::GetMaximum()));
}

void Slider::OnMaximumChanged(Float oldMaximum, Float newMaximum)
{
    CoerceValue(SelectionStartProperty);
    CoerceValue(SelectionEndProperty);
}

void Slider::OnMinimizeValue()
{
    SetCurrentValue(RangeBase::ValueProperty, new OFloat(RangeBase::GetMinimum()));
}

void Slider::OnMinimumChanged(Float oldMinimum, Float newMinimum)
{
    CoerceValue(SelectionStartProperty);
}

void Slider::OnThumbDragCompleted(DragCompletedEventArg* e)
{
    e->SetHandled(true);
}

void Slider::OnThumbDragDelta(DragDeltaEventArg* e)
{
    Thumb* originalSource = DynamicCast<Thumb>(e->GetOriginalSource());

    if ((GetTrack() != NULL) && (originalSource == GetTrack()->GetThumb()))
    {
        int o = _previousValue + GetTrack()->GetValueFromDistance(e->GetHorizontalChange(), e->GetVerticalChange());
        if (o >= 0)
        {
            UpdateValue(o);
        }
    }
    e->SetHandled(true);
}

void Slider::OnThumbDragStarted(DragStartedEventArg* e)
{
    _previousValue = GetValue();
    e->SetHandled(true);
}

void Slider::OnPreviewMouseLeftButtonDown(MouseButtonEventArg* e)
{
    if ((IsMoveToPointEnabled() && 
        (GetTrack() != NULL)) && 
        ((GetTrack()->GetThumb() != NULL) && 
        !GetTrack()->GetThumb()->IsMouseOver()))
    {
        Point position = GetTrack()->PointToScreen(Point());
        int o = 0;

        // 计算当前鼠标和Track开始位置的距离
        position.x = e->GetMousePoint().x - position.x;
        position.y = e->GetMousePoint().y - position.y;

        o = GetTrack()->GetValueFromPoint(position);

        if (o >= 0)
        {
            UpdateValue(o);
        }
        e->SetHandled(true);
    }

    RangeBase::OnPreviewMouseLeftButtonDown(e);
}

void Slider::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    RangeBase::OnMouseLeftButtonDown(e);

    e->SetHandled(true);

    /*const Size& thumbSize = _thumbBtn->GetDesiredSize();
    Point pt = PointToScreen(Point());

    if (GetOrientation() == Orientation::Horizontal) 
    {
        Float dx = (Float)(e.MousePoint().x - pt.x);
        Float pos = dx - thumbSize.cx / 2;
        HandleHorizontal(pos);
    }
    else
    {
        Float dy = (Float)(e.MousePoint().y - pt.y);
        Float pos = dy - thumbSize.cy / 2;
        HandleVertical(pos);
    }

    SetFocus();*/ 
}

void Slider::OnKeyDown(KeyboardEventArg* e)
{
    if (GetOrientation() == Orientation::Horizontal) 
    {
        if (e->GetKey() == Key::kLeft)
        {
            if (GetValue() > GetMinimum())
            {
                SetValue((int)GetValue() - 1);
            }
        }
        else if (e->GetKey() == Key::kRight)
        {
            if (GetValue() < GetMaximum())
            {
                SetValue((int)GetValue() + 1);
            }
        }
    }
    else
    {
        if (e->GetKey() == Key::kUp)
        {
            if (GetValue() > GetMinimum())
            {
                SetValue((int)GetValue() - 1);
            }
        }
        else if (e->GetKey() == Key::kDown)
        {
            if (GetValue() < GetMaximum())
            {
                SetValue((int)GetValue() + 1);
            }
        }
    }
}

void Slider::OnMouseEnter(MouseButtonEventArg* e)
{
}

void Slider::OnMouseLeave(MouseButtonEventArg* e)
{
}

//=====================================================
//

void Slider::MoveToNextTick(Float direction)
{
    if (direction != 0.0)
    {
        int num = RangeBase::GetValue();
        int num2 = SnapToTick(max(RangeBase::GetMinimum(), min(RangeBase::GetMaximum(), num + direction)));
        bool flag = direction > 0.0;

        if (((num2 == num) && (!flag || (num != RangeBase::GetMaximum()))) && (flag || (num != RangeBase::GetMinimum())))
        {
            IntAutoArray* ticks = GetTicks();

            if ((ticks != NULL) && (ticks->Length() > 0))
            {
                for (int i = 0; i < ticks->Length(); i++)
                {
                    int num4 = ticks->GetItem(i);
                    if (((flag && num4 > num) && ((num4 < num2) || (num2 == num))) || 
                        ((!flag && (num4 < num)) && ((num4 > num2) || (num2 == num))))
                    {
                        num2 = num4;
                    }
                }
            }
            else if (FloatUtil::GreaterThan(GetTickFrequency(), 0.0))
            {
                Float num5 = Math::Round((Float)((num - RangeBase::GetMinimum()) / GetTickFrequency()));
                
                if (flag)
                {
                    num5++;
                }
                else
                {
                    num5--;
                }

                num2 = (int)(RangeBase::GetMinimum() + (num5 * GetTickFrequency()));
            }
        }

        if (num2 != num)
        {
            SetCurrentValue(RangeBase::ValueProperty, new Integer(num2));
        }
    }
}

}
