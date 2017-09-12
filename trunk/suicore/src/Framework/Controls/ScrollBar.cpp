// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// ScrollBar.cpp

#include <Framework/Controls/ScrollBar.h>
#include <Framework/Controls/RepeatButton.h>
#include <Framework/Animation/AnimationAsSeries.h>

#include <System/Tools/EventHelper.h>
#include <System/Tools/CommandOp.h>

#include <math.h>

static const int CSSCROLLTIMER = 258;

namespace suic
{

RoutedCommand* ScrollBar::ScrollHereCommand;
RoutedCommand* ScrollBar::PageUpCommand;
RoutedCommand* ScrollBar::PageDownCommand;
RoutedCommand* ScrollBar::PageLeftCommand;
RoutedCommand* ScrollBar::PageRightCommand;

RoutedCommand* ScrollBar::LineUpCommand;
RoutedCommand* ScrollBar::LineDownCommand;
RoutedCommand* ScrollBar::LineLeftCommand;
RoutedCommand* ScrollBar::LineRightCommand;

RoutedCommand* ScrollBar::ScrollToTopCommand;
RoutedCommand* ScrollBar::ScrollToBottomCommand;

RoutedCommand* ScrollBar::ScrollToLeftCommand;
RoutedCommand* ScrollBar::ScrollToRightCommand;

RoutedCommand* ScrollBar::ScrollToVerticalOffsetCommand;
RoutedCommand* ScrollBar::ScrollToHorizontalOffsetCommand;

DpProperty* ScrollBar::OrientationProperty;
DpProperty* ScrollBar::ViewportSizeProperty;

RoutedEvent* ScrollBar::ScrollEvent;

ImplementRTTIOfClass(ScrollBar, RangeBase)

ScrollBar::ScrollBar() 
    : _isStandalone(true)
    , _canScroll(true)
    , _previousValue(0)
    , _track(NULL)
{
}

ScrollBar::~ScrollBar()
{
}

ScrollEventArg::ScrollEventArg(ScrollEventType scrollEventType, Float newValue)
    : _newValue(newValue)
    , _scrollEventType(scrollEventType)
{
    
}

void ScrollBar::OnOrientationPropChanged(DpObject* d, DpPropChangedEventArg* e)
{

}

void ScrollBar::StaticInit()
{
    if (PageUpCommand == NULL)
    {
        Thumb::StaticInit();
        RangeBase::StaticInit();

        ScrollHereCommand = new RoutedCommand(_T("ScrollHere"), ScrollBar::RTTIType());

        PageUpCommand = new RoutedCommand(_T("PageUp"), ScrollBar::RTTIType());
        PageDownCommand = new RoutedCommand(_T("PageDown"), ScrollBar::RTTIType());
        PageLeftCommand = new RoutedCommand(_T("PageLeft"), ScrollBar::RTTIType());
        PageRightCommand = new RoutedCommand(_T("PageRight"), ScrollBar::RTTIType());

        LineUpCommand = new RoutedCommand(_T("LineUp"), ScrollBar::RTTIType());
        LineDownCommand = new RoutedCommand(_T("LineDown"), ScrollBar::RTTIType());

        LineLeftCommand = new RoutedCommand(_T("LineLeft"), ScrollBar::RTTIType());
        LineRightCommand = new RoutedCommand(_T("LineRight"), ScrollBar::RTTIType());

        ScrollToTopCommand = new RoutedCommand(_T("ScrollToTop"), ScrollBar::RTTIType());
        ScrollToBottomCommand = new RoutedCommand(_T("ScrollToBottom"), ScrollBar::RTTIType());

        ScrollToLeftCommand = new RoutedCommand(_T("ScrollToLeft"), ScrollBar::RTTIType());
        ScrollToRightCommand = new RoutedCommand(_T("ScrollToRight"), ScrollBar::RTTIType());

        ScrollToVerticalOffsetCommand = new RoutedCommand(_T("ScrollToVerticalOffset"), ScrollBar::RTTIType());
        ScrollToHorizontalOffsetCommand = new RoutedCommand(_T("ScrollToHorizontalOffset"), ScrollBar::RTTIType());

        IsMouseOverProperty->OverrideMetadata(ScrollBar::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone, &OnVisualStatePropChanged));

        OrientationProperty = DpProperty::Register(_T("Orientation"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(OrientationBox::VerticalBox, PropMetadataOptions::AffectsMeasure, &ScrollBar::OnOrientationPropChanged));
        OrientationProperty->SetConvertValueCb(OrientationConvert::Convert);

        ViewportSizeProperty = DpProperty::Register(_T("ViewportSize"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsMeasure));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            PageUpCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            PageDownCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            PageLeftCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            PageRightCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            LineUpCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            LineDownCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            LineLeftCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            LineRightCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollToTopCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollToBottomCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollToLeftCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollToRightCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollToVerticalOffsetCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollToHorizontalOffsetCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

        ScrollEvent = EventHelper::RegisterRoutedEvent("Scroll", RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());

        Element::FocusableProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, 0));

        EventHelper::RegisterClassHandler(RTTIType(), Thumb::DragStartedEvent, new RoutedEventHandler(ScrollBar::OnPropThumbDragStarted), false);
        EventHelper::RegisterClassHandler(RTTIType(), Thumb::DragDeltaEvent, new RoutedEventHandler(ScrollBar::OnPropThumbDragDelta), false);
        EventHelper::RegisterClassHandler(RTTIType(), Thumb::DragCompletedEvent, new RoutedEventHandler(ScrollBar::OnPropThumbDragCompleted), false);

        RangeBase::MaximumProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(new OFloat(100.0f), 0, PropChangedCallback(ScrollBar::OnViewChanged)));
        RangeBase::MinimumProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(OFloat::GetZeroFloat(), 0, PropChangedCallback(ScrollBar::OnViewChanged)));
    }
}

bool ScrollBar::IsValidOrientation(Object* o)
{
    Orientation orientation = (Orientation)o->ToInt();
    if (orientation != Orientation::Horizontal)
    {
        return (orientation == Orientation::Vertical);
    }
    else
    {
        return true;
    }
}

void ScrollBar::OnScrollCommand(Object* target, ExecutedRoutedEventArg* e)
{
    ScrollBar* bar = DynamicCast<ScrollBar>(target);
    ICommand* cmd = e->GetCommand();

    if (cmd == ScrollHereCommand)
    {
        bar->ScrollToLastMousePoint();
    }

    if (bar->IsStandalone())
    {
        if (bar->GetOrientation() == Orientation::Vertical)
        {
            if (cmd == LineUpCommand)
            {
                bar->LineUp();
            }
            else if (cmd == LineDownCommand)
            {
                bar->LineDown();
            }
            else if (cmd == PageUpCommand)
            {
                bar->PageUp();
            }
            else if (cmd == PageDownCommand)
            {
                bar->PageDown();
            }
            else if (cmd == ScrollToTopCommand)
            {
                bar->ScrollToTop();
            }
            else if (cmd == ScrollToBottomCommand)
            {
                bar->ScrollToBottom();
            }
        }
        else 
        {
            if (cmd == LineLeftCommand)
            {
                bar->LineLeft();
            }
            else if (cmd == LineRightCommand)
            {
                bar->LineRight();
            }
            else if (cmd == PageLeftCommand)
            {
                bar->PageLeft();
            }
            else if (cmd == PageRightCommand)
            {
                bar->PageRight();
            }
            else if (cmd == ScrollToLeftCommand)
            {
                bar->ScrollToLeftEnd();
            }
            else if (cmd == ScrollToRightCommand)
            {
                bar->ScrollToRightEnd();
            }
        }
    }
}

void ScrollBar::OnPropThumbDragCompleted(DpObject* sender, RoutedEventArg* e)
{
    ScrollBar* bar = DynamicCast<ScrollBar>(sender);
    DragCompletedEventArg* dce = (DragCompletedEventArg*)e;

    bar->OnThumbDragCompleted(dce);
}

void ScrollBar::OnPropThumbDragDelta(DpObject* sender, RoutedEventArg* e)
{
    ScrollBar* bar = DynamicCast<ScrollBar>(sender);

    if (bar != NULL)
    {
        DragDeltaEventArg* dde = (DragDeltaEventArg*)e;
        bar->UpdateValue(dde->GetHorizontalChange(), dde->GetVerticalChange());
    }
}

void ScrollBar::OnPropThumbDragStarted(DpObject* sender, RoutedEventArg* e)
{
    ScrollBar* bar = DynamicCast<ScrollBar>(sender);

    if (bar != NULL)
    {
        bar->_hasScrolled = false;
        bar->_previousValue = bar->GetValue();
    }
}

void ScrollBar::OnViewChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ScrollBar* bar = DynamicCast<ScrollBar>(d);
    Float dMax = bar->GetMaximum();
    Float dMin = bar->GetMinimum();
    bool flag = FloatUtil::GreaterThan(dMax, dMin);

    if (flag != bar->_canScroll)
    {
        bar->_canScroll = flag;
        bar->CoerceValue(Element::IsEnabledProperty);
    }
}

void ScrollBar::OnApplyTemplate()
{
    RangeBase::OnApplyTemplate();
    _track = DynamicCast<Track>(GetTemplateChild("PART_Track"));
}

void ScrollBar::ScrollToPosByAnimation(DpProperty* dp, int iStart, int iOffset, int duration, IEasingFunction* func)
{
    ObjectPtr cacheObj = func;

    if (iStart < 0)
    {
        iStart = GetValue();
    }

    if (duration < 10)
    {
        duration = 200;
    }

    int iEnd = iStart + iOffset;

    Int32Animation* intAni = NULL;

    iStart = UFMAX(GetMinimum(), iStart);
    iStart = UFMIN(iStart, GetMaximum());
    iEnd = UFMAX(GetMinimum(), iEnd);
    iEnd = UFMIN(iEnd, GetMaximum());

    if (iEnd != iStart)
    {
        intAni = new Int32Animation(iStart, iEnd, Duration(duration), FillBehavior::fbHoldEnd);
        intAni->SetDecelerationRatio(1);
        if (func != NULL)
        {
            intAni->SetEasingFunction(func);
        }
        BeginAnimation(dp, intAni);
    }
}

void ScrollBar::OnMouseWheel(MouseWheelEventArg* e)
{
    if (IsStandalone() && IsEnabled())
    {
        int iViewport = 0;
        if (GetOrientation() == Orientation::Horizontal)
        {
            iViewport = GetActualWidth();
        }
        else
        {
            iViewport = GetActualHeight();
        }

        if (e->GetDelta() < 0)
        {
            ScrollToPosByAnimation(ValueProperty, -1, iViewport, 500, NULL);
        }
        else
        {
            ScrollToPosByAnimation(ValueProperty, -1, -iViewport, 500, NULL);
        }

        UpdateLayout();

        e->SetHandled(true);
    }
}

void ScrollBar::OnThumbDragCompleted(DragCompletedEventArg* e)
{
    if (_hasScrolled)
    {
        FinishDrag();
        RaiseScrollEvent(ScrollEventType::EndScroll);
    }
}

void ScrollBar::OnPreviewMouseLeftButtonDown(MouseButtonEventArg* e)
{
    _thumbOffset = Point();

    if (((GetTrack() != NULL) && GetTrack()->IsMouseOver()))
    {
        Thumb* thumb = GetTrack()->GetThumb();
        Point position = GetTrack()->PointToScreen(Point());
        Point point1 = position;
        
        // 计算当前鼠标和Track开始位置的距离
        position.x = e->GetMousePoint().x - position.x;
        position.y = e->GetMousePoint().y - position.y;
 
        // 计算出当前位置
        int o = GetTrack()->GetValueFromPoint(position);

        if (o >= 0 && !thumb->IsMouseOver())
        {
            ChangeValue(o, false);
        }

        if ((thumb != NULL) && thumb->IsMouseOver())
        {
            Point point2 = thumb->PointToScreen(Point());
            Point point3(thumb->GetActualWidth() * 0.5, thumb->GetActualHeight() * 0.5);

            point2.x = e->GetMousePoint().x - point2.x;
            point2.y = e->GetMousePoint().y - point2.y;

            _thumbOffset.x = point2.x - point3.x;
            _thumbOffset.y = point2.y - point3.y;
        }
        else
        {
            e->SetHandled(true);
        }
    }

    RangeBase::OnPreviewMouseLeftButtonDown(e);
}

void ScrollBar::OnPreviewMouseRightButtonUp(MouseButtonEventArg* e)
{
    if (GetTrack() != NULL)
    {
        _latestRightButtonClickPoint = e->GetMousePoint();
    }
    else
    {
        _latestRightButtonClickPoint = Point(-1.0, -1.0);
    }
    
    RangeBase::OnPreviewMouseRightButtonUp(e);
}

void ScrollBar::RaiseScrollEvent(ScrollEventType scrollEventType)
{
    ScrollEventArg e(scrollEventType, GetValue());

    e.SetSource(this);
    e.SetRoutedEvent(ScrollEvent);

    RaiseEvent(&e);
}

void ScrollBar::AddScroll(RoutedEventHandler* arg)
{

}

void ScrollBar::RemoveScroll(RoutedEventHandler* arg)
{

}

void ScrollBar::ScrollToLastMousePoint()
{
    Point point(-1.0, -1.0);

    if ((GetTrack() != NULL) && (_latestRightButtonClickPoint != point))
    {
        int o = GetTrack()->GetValueFromPoint(_latestRightButtonClickPoint);

        if (o >= 0)
        {
            ChangeValue(o, false);
            _latestRightButtonClickPoint = point;
            RaiseScrollEvent(ScrollEventType::ThumbPosition);
        }
    }
}

void ScrollBar::ChangeValue(int newValue, bool defer)
{
    newValue = min(max(newValue, GetMinimum()), GetMaximum());

    if (IsStandalone())
    {
        SetValue(newValue);
    }
    else
    {
        Element* commandTarget = GetCommandTarget();
        RoutedCommand* command = NULL;
        bool flag = GetOrientation() == Orientation::Horizontal;
        
        if (command == NULL)
        {
            ObjectPtr param = new Integer(newValue);
            command = flag ? ScrollToHorizontalOffsetCommand : ScrollToVerticalOffsetCommand;
            if (command->CanExecute(commandTarget, param.get()))
            {
                command->Execute(commandTarget, param.get());
            }
        }
    }
}

void ScrollBar::ToLargeDecrement()
{
    int num = max(GetValue() - GetLargeChange(), GetMinimum());

    if (GetValue() != num)
    {
        SetValue(num);
        RaiseScrollEvent(ScrollEventType::LargeDecrement);
    }
}

void ScrollBar::ToLargeIncrement()
{
    int num = min(GetValue() + GetLargeChange(), GetMaximum());
    if (GetValue() != num)
    {
        SetValue(num);
        RaiseScrollEvent(ScrollEventType::LargeIncrement);
    }
}

void ScrollBar::LineDown()
{
    ToSmallIncrement();
}

void ScrollBar::LineLeft()
{
    ToSmallDecrement();
}

void ScrollBar::LineRight()
{
    ToSmallIncrement();
}

void ScrollBar::LineUp()
{
    ToSmallDecrement();
}

void ScrollBar::PageDown()
{
    ToLargeIncrement();
}

void ScrollBar::PageLeft()
{
    ToLargeDecrement();
}

void ScrollBar::PageRight()
{
    ToLargeIncrement();
}

void ScrollBar::PageUp()
{
    ToLargeDecrement();
}

void ScrollBar::ScrollToBottom()
{
    ToMaximum();
}

void ScrollBar::ScrollToLeftEnd()
{
    ToMinimum();
}

void ScrollBar::ScrollToRightEnd()
{
    ToMaximum();
}

void ScrollBar::ScrollToTop()
{
    ToMinimum();
}

void ScrollBar::ToSmallDecrement()
{
    int num = max(GetValue() - GetSmallChange(), GetMinimum());
    if (GetValue() != num)
    {
        SetValue(num);
        RaiseScrollEvent(ScrollEventType::SmallDecrement);
    }
}

void ScrollBar::ToSmallIncrement()
{
    int num = min(GetValue() + GetSmallChange(), GetMaximum());
    if (GetValue() != num)
    {
        SetValue(num);
        RaiseScrollEvent(ScrollEventType::SmallIncrement);
    }
}

void ScrollBar::ToMaximum()
{
    if (!FloatUtil::AreClose(GetValue(), GetMaximum()))
    {
        SetValue(GetMaximum());
        RaiseScrollEvent(ScrollEventType::Last);
    }
}

void ScrollBar::ToMinimum()
{
    if (!FloatUtil::AreClose(GetValue(), GetMinimum()))
    {
        SetValue(GetMinimum());
        RaiseScrollEvent(ScrollEventType::First);
    }
}

void ScrollBar::UpdateValue(int horizontalDragDelta, int verticalDragDelta)
{
    if (GetTrack() != NULL)
    {
        // 根据可视大小计算出逻辑大小
        int o = GetTrack()->GetValueFromDistance(horizontalDragDelta, verticalDragDelta);

        if (o != 0)
        {
            int num4;
            int num2 = _previousValue;
            int num3 = num2 + o;
            if (GetOrientation() == Orientation::Horizontal)
            {
                num4 = abs(verticalDragDelta);
            }
            else
            {
                num4 = abs(horizontalDragDelta);
            }
            if (num4 > 150)
            {
                num3 = _previousValue;
            }

            num3 = UFMIN(num3, GetMaximum());

            if (num2 != num3)
            {
                _hasScrolled = true;
                ChangeValue(num3, true);
                RaiseScrollEvent(ScrollEventType::ThumbTrack);
            }
        }
    }
}

void ScrollBar::FinishDrag()
{
    /*Float parameter = GetValue();
    Element* commandTarget = GetCommandTarget();
    RoutedCommand* command = (GetOrientation() == Orientation::Horizontal) ? DeferScrollToHorizontalOffsetCommand : DeferScrollToVerticalOffsetCommand;
    
    if (command->CanExecute(parameter, commandTarget))
    {
        ChangeValue(parameter, false);
    }*/
}

}
