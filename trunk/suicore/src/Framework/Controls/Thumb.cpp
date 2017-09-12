// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Thumb.cpp

#include <Framework/Controls/Thumb.h>
#include <System/Windows/VisualStates.h>
#include <System/Tools/EventHelper.h>
#include <System/Input/Mouse.h>

namespace suic
{

ImplementRTTIOfClass(Thumb, Control)

RoutedEvent* Thumb::DragStartedEvent;
RoutedEvent* Thumb::DragDeltaEvent;
RoutedEvent* Thumb::DragCompletedEvent;

DpProperty* Thumb::IsDraggingProperty;

Point Thumb::_prevPoint;
Point Thumb::_originPoint;

Thumb::Thumb()
{
}

Thumb::~Thumb()
{
}

void Thumb::StaticInit()
{
    if (IsDraggingProperty == NULL)
    {
        Control::StaticInit();

        IsMouseOverProperty->OverrideMetadata(RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone, &OnVisualStatePropChanged));

        IsDraggingProperty = DpProperty::RegisterAttached(_T("IsDragging"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));

        DragStartedEvent = EventHelper::RegisterRoutedEvent(_T("DragStarted")
            , RoutingStrategy::Bubble, Thumb::RTTIType(), Thumb::RTTIType());
        DragDeltaEvent = EventHelper::RegisterRoutedEvent(_T("DragDelta")
            , suic::RoutingStrategy::Bubble, Thumb::RTTIType(), Thumb::RTTIType());
        DragCompletedEvent = EventHelper::RegisterRoutedEvent(_T("DragCompleted")
            , RoutingStrategy::Bubble, Thumb::RTTIType(), Thumb::RTTIType());
    }
}

void Thumb::SetIsDraging(bool val)
{
    SetValue(IsDraggingProperty, BOOLTOBOOLEAN(val));
}

void Thumb::AddDragStarted(RoutedEventHandler* handler)
{
    AddHandler(DragStartedEvent, handler);
}

void Thumb::AddDragDelta(RoutedEventHandler* handler)
{
    AddHandler(DragDeltaEvent, handler);
}

void Thumb::AddDragCompleted(RoutedEventHandler* handler)
{
    AddHandler(DragCompletedEvent, handler);
}

void Thumb::RemoveDragStarted(RoutedEventHandler handler)
{
    RemoveHandler(DragStartedEvent, &handler);
}

void Thumb::RemoveDragDelta(RoutedEventHandler handler)
{
    RemoveHandler(DragDeltaEvent, &handler);
}

void Thumb::RemoveDragCompleted(RoutedEventHandler handler)
{
    RemoveHandler(DragCompletedEvent, &handler);
}

suic::Size Thumb::OnMeasure(const suic::Size& constraintSize)
{
    suic::Size desiredSize = suic::Control::OnMeasure(constraintSize);
    return desiredSize;
}

void Thumb::OnMouseMove(MouseButtonEventArg* e)
{
    Control::OnMouseMove(e);

    if (MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress)
    {
        suic::Point thumbPos = e->GetMousePoint();

        if (thumbPos.x != _prevPoint.x || thumbPos.y != _prevPoint.y)
        { 
            int horiDelta = thumbPos.x - _prevPoint.x;
            int vertDelta = thumbPos.y - _prevPoint.y;

            int horiChange = thumbPos.x - _originPoint.x;
            int vertChange = thumbPos.y - _originPoint.y;

            _prevPoint = thumbPos;
            e->SetHandled(true);

            // 发出拖动变化事件
            DragDeltaEventArg de(horiChange, vertChange, horiDelta, vertDelta);
            de.SetRoutedEvent(DragDeltaEvent);
            de.OnOverrideOriginalSource(this);
            RaiseEvent(&de); 
        }
    }
    else
    {
        ReleaseCaptureMouse();

        SetIsDraging(false);
        _originPoint.x = 0;
        _originPoint.y = 0; 
    }
}

void Thumb::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    if (!IsDragging())
    {
        e->SetHandled(true);
        // 开始拖动，捕获鼠标控制权
        SetCaptureMouse();

        SetIsDraging(true);

        // 初始化数据
        _prevPoint = e->GetMousePoint();
        _originPoint = _prevPoint;

        // 发出开始拖动事件
        DragStartedEventArg de(_originPoint.x, _originPoint.y);

        de.SetRoutedEvent(DragStartedEvent);
        de.OnOverrideOriginalSource(this);

        RaiseEvent(&de);
    }

    Control::OnMouseLeftButtonDown(e);
}

void Thumb::OnLostMouseCapture(MouseButtonEventArg* e)
{
    Control::OnLostMouseCapture(e);
    if (IsDragging())
    {
        e->SetHandled(true);
        SetIsDraging(false);
        HandleMouseLeftButtonUp(e->GetMousePoint());
    }
}

void Thumb::HandleMouseLeftButtonUp(Point pt)
{
    DragCompletedEventArg de(pt.x - _originPoint.x, pt.y - _originPoint.y, false);

    de.SetRoutedEvent(DragCompletedEvent);
    de.OnOverrideOriginalSource(this);

    RaiseEvent(&de);
}

void Thumb::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    if (IsMouseCaptured() && IsDragging())
    {
        e->SetHandled(true);
        SetIsDraging(false);

        ReleaseCaptureMouse();
        HandleMouseLeftButtonUp(e->GetMousePoint());
    }
    else
    {
        SetIsDraging(false);
    }

    Control::OnMouseLeftButtonUp(e); 
}

}
