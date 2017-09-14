
#include <Framework/Controls/ScrollViewer.h>
#include <Framework/Controls/ScrollBar.h>
#include <Framework/Presenter/ScrollContentPresenter.h>

#include <Framework/Animation/AnimationAsSeries.h>

#include <System/Tools/EventHelper.h>
#include <System/Tools/CommandOp.h>
#include <System/Input/KeyboardNavigation.h>

#include <Framework/Controls/Popup.h>
#include <System/Tools/VisualTreeOp.h>

namespace suic
{

ImplementRTTIOfClass(ScrollViewer, ContentControl)

const int ScrollViewer::scScrollLineDelta = 16;
const int ScrollViewer::scMouseWheelDelta = 48;

DpProperty* ScrollViewer::CanContentScrollProperty;
DpProperty* ScrollViewer::SmoothingScrollProperty;
//DpProperty* ScrollViewer::ContentHorizontalOffsetProperty;
//DpProperty* ScrollViewer::ContentVerticalOffsetProperty;
//DpProperty* ScrollViewer::ExtentHeightProperty;
//DpProperty* ScrollViewer::ExtentWidthProperty;
//DpProperty* ScrollViewer::ScrollableWidthProperty;
//DpProperty* ScrollViewer::ScrollableHeightProperty;
//DpProperty* ScrollViewer::ViewportHeightProperty;
//DpProperty* ScrollViewer::ViewportWidthProperty;
DpProperty* ScrollViewer::HorizontalOffsetProperty;
DpProperty* ScrollViewer::HorizontalScrollBarVisibilityProperty;

DpProperty* ScrollViewer::VerticalOffsetProperty;
DpProperty* ScrollViewer::VerticalScrollBarVisibilityProperty;

DpProperty* ScrollViewer::ComputedHorizontalScrollBarVisibilityProperty;
DpProperty* ScrollViewer::ComputedVerticalScrollBarVisibilityProperty;

//DpProperty* ScrollViewer::ComputedHorizontalScrollBarVisibilityProperty;
//DpProperty* ScrollViewer::ComputedVerticalScrollBarVisibilityProperty;

RoutedEvent* ScrollViewer::ScrollChangedEvent;

static String HorizontalScrollBarTemplateName = _U("PART_HorizontalScrollBar"); 
static String VerticalScrollBarTemplateName = _U("PART_VerticalScrollBar");
static String ScrollContentPresenterTemplateName = _U("PART_ScrollContentPresenter");

enum Flags
{
    Unknow = 0,
    InMeasure = 1,
    InChildInvalidateMeasure = 2,
    RequestComputeMeasure = 4,
    HandlesMouseWheelScrolling = 8,
    ForceNextManipulationComplete = 0x10,

    ManipulationBindingsInitialized = 0x20,
    CompleteScrollManipulation = 0x40,
    ScrollChangedDirty = 0x60,
};

AutoObj SBVisibilityConvert::ConvertValue(DpProperty* dp, Object* val)
{
    String strVal(val->ToString());
    if (strVal.Equals(_U("Auto")))
    {
        return Integer::GetPosInt(ScrollBarVisibility::sbvAutoScroll);
    }
    else if (strVal.Equals(_U("Disabled")))
    {
        return Integer::GetPosInt(ScrollBarVisibility::sbvDisabled);
    }
    else if (strVal.Equals(_U("Hidden")))
    {
        return Integer::GetPosInt(ScrollBarVisibility::sbvHidden);
    }
    else if (strVal.Equals(_U("Collapsed")))
    {
        return Integer::GetPosInt(ScrollBarVisibility::sbvCollapsed);
    }
    else
    {
        return Integer::GetPosInt(ScrollBarVisibility::sbvVisible);
    }
}

SBVisibilityConvert* SBVisibilityConvert::Convert = new SBVisibilityConvert();

ScrollViewer::ScrollViewer()
    : _scrollInfo(NULL)
{
    _xSize = 0;
    _ySize = 0;
    _xExtent = 0;
    _yExtent = 0;
    _flags = 0;
    _horiVisible = Visibility::Visible;
    _vertVisible = Visibility::Visible;

    _horiScroll = NULL;
    _vertScroll = NULL;

    WriteDoFlag(ViewFlags::IsClipToBoundsVF, true);
}

ScrollViewer::~ScrollViewer()
{
}

void ScrollViewer::OnVerticalOffsetPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ScrollViewer* scrollView = DynamicCast<ScrollViewer>(d);
    if (scrollView != NULL)
    {
        scrollView->ScrollToVerticalOffset(e->GetNewValue()->ToInt());
        scrollView->RefleshLayout(false);
    }
}

void ScrollViewer::OnHorizontalOffsetPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ScrollViewer* scrollView = RTTICast<ScrollViewer>(d);
    if (scrollView != NULL)
    {
        scrollView->ScrollToHorizontalOffset(e->GetNewValue()->ToInt());
        scrollView->RefleshLayout(false);
    }
}

void ScrollViewer::StaticInit()
{
    if (NULL == CanContentScrollProperty)
    {
        KeyboardNavigation::StaticInit();
        ContentControl::StaticInit();
        ScrollBar::StaticInit();

        ClipToBoundsProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsParentMeasure, &Element::OnClipToBoundsPropChanged));

        CanContentScrollProperty = DpProperty::RegisterAttached(_U("CanContentScroll"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsArrange));
        SmoothingScrollProperty = DpProperty::Register(_U("SmoothingScroll"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));

        //ContentHorizontalOffsetProperty = DpProperty::Register(_U("ContentHorizontalOffset"), RTTIType(), Integer::RTTIType()
        //    , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsArrange));
        //ContentVerticalOffsetProperty = DpProperty::Register(_U("ContentVerticalOffset"), RTTIType(), Integer::RTTIType()
        //    , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsArrange));
        /*ExtentHeightProperty = DpProperty::Register(_U("ExtentHeight"), RTTIType(), Integer::RTTIType()
        , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsArrange));
        ExtentWidthProperty = DpProperty::Register(_U("ExtentWidth"), RTTIType(), Integer::RTTIType()
        , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsArrange));

        ScrollableWidthProperty = DpProperty::Register(_U("ScrollableWidth"), RTTIType(), OFloat::RTTIType()
        , DpPropMemory::GetPropMeta(OFloat::GetZeroFloat(), PropMetadataOptions::AffectsNone));
        ScrollableHeightProperty = DpProperty::Register(_U("ScrollableHeight"), RTTIType(), OFloat::RTTIType()
        , DpPropMemory::GetPropMeta(OFloat::GetZeroFloat(), PropMetadataOptions::AffectsNone));

        ViewportHeightProperty = DpProperty::RegisterAttached(_U("ViewportHeight"), RTTIType(), Integer::RTTIType()
        , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsArrange));
        ViewportWidthProperty = DpProperty::RegisterAttached(_U("ViewportWidth"), RTTIType(), Integer::RTTIType()
        , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsArrange));*/

        ComputedHorizontalScrollBarVisibilityProperty = DpProperty::RegisterAttached(_U("ComputedHorizontalScrollBarVisibility"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(VisibilityBox::VisibleBox, PropMetadataOptions::AffectsArrange));
        ComputedVerticalScrollBarVisibilityProperty = DpProperty::RegisterAttached(_U("ComputedVerticalScrollBarVisibility"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(VisibilityBox::VisibleBox, PropMetadataOptions::AffectsArrange));

        HorizontalOffsetProperty = DpProperty::Register(_U("HorizontalOffset"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsArrange, &ScrollViewer::OnHorizontalOffsetPropChanged));
        VerticalOffsetProperty = DpProperty::Register(_U("VerticalOffset"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsArrange, &ScrollViewer::OnVerticalOffsetPropChanged));

        HorizontalScrollBarVisibilityProperty = DpProperty::RegisterAttached(_U("HorizontalScrollBarVisibility"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(ScrollBarVisibility::sbvAutoScroll), PropMetadataOptions::AffectsArrange));
        VerticalScrollBarVisibilityProperty = DpProperty::RegisterAttached(_U("VerticalScrollBarVisibility"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(ScrollBarVisibility::sbvAutoScroll), PropMetadataOptions::AffectsArrange));

        HorizontalScrollBarVisibilityProperty->SetConvertValueCb(SBVisibilityConvert::Convert);
        VerticalScrollBarVisibilityProperty->SetConvertValueCb(SBVisibilityConvert::Convert);

        ComputedHorizontalScrollBarVisibilityProperty->SetConvertValueCb(VisibilityConvert::Convert);
        ComputedVerticalScrollBarVisibilityProperty->SetConvertValueCb(VisibilityConvert::Convert);

        ScrollChangedEvent = EventHelper::RegisterRoutedEvent(_U("ScrollChanged"), RoutingStrategy::Bubble, ScrollChangedEventHandler::RTTIType(), RTTIType());

        Control::IsTabStopProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False));
        KeyboardNavigation::DirectionalNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knLocal)));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollBar::ScrollToVerticalOffsetCommand, ExecutedRoutedEventHandler(ScrollViewer::OnScrollCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollBar::ScrollToHorizontalOffsetCommand, ExecutedRoutedEventHandler(ScrollViewer::OnScrollCommand));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollBar::LineUpCommand, ExecutedRoutedEventHandler(ScrollViewer::OnScrollCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollBar::LineDownCommand, ExecutedRoutedEventHandler(ScrollViewer::OnScrollCommand));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollBar::LineLeftCommand, ExecutedRoutedEventHandler(ScrollViewer::OnScrollCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollBar::LineRightCommand, ExecutedRoutedEventHandler(ScrollViewer::OnScrollCommand));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollBar::ScrollToTopCommand, ExecutedRoutedEventHandler(ScrollViewer::OnScrollCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollBar::ScrollToBottomCommand, ExecutedRoutedEventHandler(ScrollViewer::OnScrollCommand));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollBar::ScrollToLeftCommand, ExecutedRoutedEventHandler(ScrollViewer::OnScrollCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            ScrollBar::ScrollToRightCommand, ExecutedRoutedEventHandler(ScrollViewer::OnScrollCommand));
    }
}

void ScrollViewer::OnScrollCommand(Object* target, ExecutedRoutedEventArg* e)
{
    e->SetHandled(true);

    ScrollViewer* scrollView = RTTICast<ScrollViewer>(target);

    if (e->GetCommand() == ScrollBar::ScrollToVerticalOffsetCommand)
    {
        scrollView->ScrollToVerticalOffset(e->GetParams()->ToInt());
    }
    else if (e->GetCommand() == ScrollBar::ScrollToHorizontalOffsetCommand)
    {
        scrollView->ScrollToHorizontalOffset(e->GetParams()->ToInt());
    }
    else if (e->GetCommand() == ScrollBar::ScrollToLeftCommand)
    {
        scrollView->ScrollToLeft();
    }
    else if (e->GetCommand() == ScrollBar::ScrollToRightCommand)
    {
        scrollView->ScrollToRight();
    }
    else if (e->GetCommand() == ScrollBar::ScrollToTopCommand)
    {
        scrollView->ScrollToTop();
    }
    else if (e->GetCommand() == ScrollBar::ScrollToBottomCommand)
    {
        scrollView->ScrollToBottom();
    }
    else if (e->GetCommand() == ScrollBar::LineLeftCommand)
    {
        scrollView->LineLeft();
    }
    else if (e->GetCommand() == ScrollBar::LineRightCommand)
    {
        scrollView->LineRight();
    }
    else if (e->GetCommand() == ScrollBar::LineUpCommand)
    {
        scrollView->LineUp();
    }
    else if (e->GetCommand() == ScrollBar::LineDownCommand)
    {
        scrollView->LineDown();
    }
    else
    {
        return;
    }

    scrollView->RefleshLayout(false);
}

void ScrollViewer::SetFlagValue(int flag, bool value)
{
    if (value)
    {
        _flags |= flag;
    }
    else
    {
        _flags &= ~flag;
    }
}

bool ScrollViewer::GetInMeasure() const
{
    return ((_flags & Flags::InMeasure) == Flags::InMeasure);
}

void ScrollViewer::SetInMeasure(bool val)
{
    SetFlagValue(Flags::InMeasure, val);
}

bool ScrollViewer::GetInChildInvalidateMeasure() const
{
    return ((_flags & Flags::InChildInvalidateMeasure) == Flags::InChildInvalidateMeasure);
}

void ScrollViewer::SetInChildInvalidateMeasure(bool val)
{
    SetFlagValue(Flags::InChildInvalidateMeasure, val);
}

bool ScrollViewer::GetRequestComputeMeasure() const
{
    return ((_flags & Flags::RequestComputeMeasure) == Flags::RequestComputeMeasure);
}

void ScrollViewer::SetRequestComputeMeasure(bool val)
{
    SetFlagValue(Flags::RequestComputeMeasure, val);
}

ScrollBarVisibility ScrollViewer::GetHorizontalScrollBarVisibility()
{
    return (ScrollBarVisibility)(GetValue(HorizontalScrollBarVisibilityProperty)->ToInt());
}

void ScrollViewer::SetHorizontalScrollBarVisibility(ScrollBarVisibility val)
{
    SetValue(HorizontalScrollBarVisibilityProperty, new Integer(val));
}

ScrollBarVisibility ScrollViewer::GetVerticalScrollBarVisibility()
{
    return (ScrollBarVisibility)(GetValue(VerticalScrollBarVisibilityProperty)->ToInt());
}

void ScrollViewer::SetVerticalScrollBarVisibility(ScrollBarVisibility val)
{
    SetValue(VerticalScrollBarVisibilityProperty, new Integer(val));
}

void ScrollViewer::AddScrollChanged(ScrollChangedEventHandler* ev)
{
    AddHandler(ScrollChangedEvent, ev);
}

void ScrollViewer::RemoveScrollChanged(ScrollChangedEventHandler* ev)
{
    RemoveHandler(ScrollChangedEvent, ev);
}

IScrollInfo* ScrollViewer::GetScrollInfo()
{
    return _scrollInfo;
}

void ScrollViewer::SetScrollInfo(IScrollInfo* scrollInfo)
{
    _scrollInfo = scrollInfo;
    if (NULL != _scrollInfo)
    {
        bool hDisableScroll = GetHorizontalScrollBarVisibility() == ScrollBarVisibility::sbvDisabled;
        bool vDisableScroll = GetVerticalScrollBarVisibility() == ScrollBarVisibility::sbvDisabled;
        scrollInfo->SetCanHorizontalScroll(!hDisableScroll); 
        scrollInfo->SetCanVerticalScroll(!vDisableScroll);
    }
}

void ScrollViewer::SetScrollChangedDirty(bool val)
{
    if (val)
    {
        _flags |= ScrollChangedDirty;
    }
    else
    {
        _flags &= ~ScrollChangedDirty;
    }
}

bool ScrollViewer::IsScrollChangedDirty() const
{
    return (_flags & ScrollChangedDirty) == ScrollChangedDirty;
}

void ScrollViewer::RaiseScrollChanged()
{
    if (IsScrollChangedDirty())
    {
        SetScrollChangedDirty(false);

        if (NULL != _scrollInfo)
        {
            Float horz = GetHorizontalOffset();
            Float vert = GetVerticalOffset();
            Float viewX = GetViewportWidth();
            Float viewY = GetViewportHeight();
            Float extentX = GetExtentWidth();
            Float extentY = GetExtentHeight();

            ScrollChangedEventArg e(horz, vert, viewX, viewY, extentX, extentY);
            e.SetRoutedEvent(ScrollChangedEvent);
            e.SetSource(this);
            e.OnOverrideOriginalSource(this);

            OnScrollChanged(&e);
        }
    }
}

bool ScrollViewer::CanContentScroll()
{
    return GetValue(CanContentScrollProperty)->ToBool();
}

bool ScrollViewer::GetSmoothingScroll()
{
    return GetValue(SmoothingScrollProperty)->ToBool();
}

void ScrollViewer::SetSmoothingScroll(bool val)
{
    SetValue(SmoothingScrollProperty, BOOLTOBOOLEAN(val));
}

int ScrollViewer::GetScrollableHeight()
{
    return max(0, GetExtentHeight() - GetViewportHeight());
}

int ScrollViewer::GetScrollableWidth()
{
    return max(0, GetExtentWidth() - GetViewportWidth());
}

int ScrollViewer::GetViewportHeight()
{
    return _ySize;
}

int ScrollViewer::GetViewportWidth()
{
    return _xSize;
}

int ScrollViewer::GetHorizontalOffset()
{
    if (NULL == _scrollInfo)
    {
        return 0;
    }
    else
    {
        return _scrollInfo->GetComputeHorizontalOffset();
    }
}

int ScrollViewer::GetVerticalOffset()
{
    if (NULL == _scrollInfo)
    {
        return 0;
    }
    else
    {
        return _scrollInfo->GetComputeVerticalOffset();
    }
}

void ScrollViewer::SetHorizontalOffset(int val)
{
    CoerceHorizontalPos(val);
    SetValue(HorizontalOffsetProperty, new Integer(val));
}

void ScrollViewer::SetVerticalOffset(int val)
{
    CoerceVerticalPos(val);
    SetValue(VerticalOffsetProperty, new Integer(val));
}

void ScrollViewer::ScrollToHorizontalOffset(int iPos)
{
    iPos = max(0, iPos);

    if (NULL != _scrollInfo)
    {
        _scrollInfo->SetHorizontalOffset(iPos);
    }

    if (_horiScroll)
    {
        _horiScroll->SetValue(iPos);
        SetScrollChangedDirty(true);
    }
}

void ScrollViewer::ScrollToVerticalOffset(int iPos)
{
    iPos = max(0, iPos);

    if (NULL != _scrollInfo)
    {
        _scrollInfo->SetVerticalOffset(iPos);
    }

    if (_vertScroll)
    {
        _vertScroll->SetValue(iPos);
        SetScrollChangedDirty(true);
    }
}

void ScrollViewer::CoerceHorizontalPos(int& iPos)
{
    iPos = max(0, iPos);
    iPos = min(iPos, GetScrollableWidth());
}

void ScrollViewer::CoerceVerticalPos(int& iPos)
{
    iPos = max(0, iPos);
    iPos = min(iPos, GetScrollableHeight());
}

void ScrollViewer::SetHoriScrollBarVisibility(Visibility val)
{
    _horiVisible = val;
    if (_horiScroll)
    {
        _horiScroll->SetVisibility(val);
    }
}

void ScrollViewer::SetVertScrollBarVisibility(Visibility val)
{
    _vertVisible = val;
    if (_vertScroll)
    {
        _vertScroll->SetVisibility(val);
    }
}

void ScrollViewer::PageDown()
{
    if (_scrollInfo != NULL)
    {
        _scrollInfo->PageDown();
    }
}

void ScrollViewer::PageLeft()
{
    if (_scrollInfo != NULL)
    {
        _scrollInfo->PageLeft();
    }
}

void ScrollViewer::PageRight()
{
    if (_scrollInfo != NULL)
    {
        _scrollInfo->PageRight();
    }
}

void ScrollViewer::PageUp()
{
    if (_scrollInfo != NULL)
    {
        _scrollInfo->PageUp();
    }
}

void ScrollViewer::LineUp()
{
    if (_scrollInfo != NULL)
    {
        _scrollInfo->LineUp();
    }
}

void ScrollViewer::LineDown()
{
    if (_scrollInfo != NULL)
    {
        _scrollInfo->LineDown();
    }
}

void ScrollViewer::LineLeft()
{
    if (_scrollInfo != NULL)
    {
        _scrollInfo->LineLeft();
    }
}

void ScrollViewer::LineRight()
{
    if (_scrollInfo != NULL)
    {
        _scrollInfo->LineRight();
    }
}

void ScrollViewer::ScrollToEnd(AxisDirection axis)
{
    int iHori = 0;
    int iVert = 0;

    if (NULL != _scrollInfo)
    {
        bool byAxis = (axis == AxisDirection::xyAxis || axis == AxisDirection::yAxis);
        bool bxAxis = (axis == AxisDirection::xyAxis || axis == AxisDirection::xAxis);

        for (int i = 0; i < 9; ++i)
        {
            iHori = GetExtentWidth();
            iVert = GetExtentHeight();

            if (byAxis)
            {
                ScrollToVerticalOffset(iVert);
            }

            if (bxAxis)
            {
                ScrollToHorizontalOffset(iHori);
            }

            InvalidateMeasure();
            UpdateArrange();

            if ((!bxAxis || iHori == GetExtentWidth()) && (!byAxis || iVert == GetExtentHeight()))
            {
                break;
            }
        }

        RefleshLayout(true);
    }
}

void ScrollViewer::ScrollToHome(AxisDirection axis)
{
    int iHori = 0;
    int iVert = 0;

    if (NULL != _scrollInfo)
    {
        bool byAxis = (axis == AxisDirection::xyAxis || axis == AxisDirection::yAxis);
        bool bxAxis = (axis == AxisDirection::xyAxis || axis == AxisDirection::xAxis);

        if (byAxis)
        {
            ScrollToVerticalOffset(iVert);
        }

        if (bxAxis)
        {
            ScrollToHorizontalOffset(iHori);
        }

        RefleshLayout(true);
    }
}

void ScrollViewer::ScrollToLeft()
{
    ScrollToHome(AxisDirection::xAxis);
}

void ScrollViewer::ScrollToRight()
{
    ScrollToEnd(AxisDirection::xAxis);
}

void ScrollViewer::ScrollToBottom()
{
    ScrollToEnd(AxisDirection::yAxis);
}

void ScrollViewer::ScrollToTop()
{
    ScrollToHome(AxisDirection::yAxis);
}

void ScrollViewer::RefleshLayout(bool bOnlyRender)
{
    Element* fte = RTTICast<Element>(GetTemplatedParent());

    if (fte)
    {
        PopupRoot* pPopup = RTTICast<PopupRoot>(VisualTreeOp::GetVisualRoot(this));

        if (pPopup != NULL)
        {
            fte = pPopup;
        }
    }
    else
    {
        fte = this;
    }

    static DWORD dwTime = 0;
    DWORD currTime = Environment::GetSystemTick();

    dwTime = currTime;
    if (fte)
    {
        if (bOnlyRender)
        {
            fte->InvalidateArrange();
            fte->InvalidateVisual();
        }
        else
        {
            fte->UpdateLayout();
        }
    }
}

void ScrollViewer::BindToTemplatedParent(DpProperty* property)
{
}

Size ScrollViewer::OnMeasure(const Size& constraint)
{
    IScrollInfo* scrollInfo = GetScrollInfo();
    Point offset;
    Size desiredSize;
    int iHori = 0;
    int iVert = 0;

    SetRequestComputeMeasure(false);

    desiredSize = RecalcMeasure(constraint);
    scrollInfo = GetScrollInfo();

    offset.x = max(0, _xExtent - _xSize);
    offset.y = max(0, _yExtent - _ySize);

    if (NULL != scrollInfo)
    {
        iHori = scrollInfo->GetHorizontalOffset();
        iVert = scrollInfo->GetVerticalOffset();
    }

    if (GetRequestComputeMeasure() || (offset.x < iHori || offset.y < iVert))
    {
        ScrollToHorizontalOffset(min(offset.x, scrollInfo->GetHorizontalOffset()));
        ScrollToVerticalOffset(min(offset.y, scrollInfo->GetVerticalOffset()));
        desiredSize = RecalcMeasure(constraint);
    }

    SetRequestComputeMeasure(false);

    return desiredSize;
}

Size ScrollViewer::RecalcMeasure(const Size& constraint)
{
    SetInChildInvalidateMeasure(false);

    IScrollInfo* scrollInfo = GetScrollInfo();
    Element* element = NULL;
    ScrollBarVisibility vertScrollBarVisible = GetVerticalScrollBarVisibility();
    ScrollBarVisibility horiScrollBarVisible = GetHorizontalScrollBarVisibility();

    if (GetVisualChildrenCount() > 0)
    {
        element = GetVisualChild(0);
    }

    if (element == NULL)
    {
        return Size();
    }

    SetInMeasure(true);

    element->WriteFlag(CoreFlags::IsInvalidateLocked, true);
    //_horiScroll->WriteFlag(CoreFlags::IsInvalidateLocked, true);
    //_vertScroll->WriteFlag(CoreFlags::IsInvalidateLocked, true);

    bool vertAuto = vertScrollBarVisible == ScrollBarVisibility::sbvAutoScroll;
    bool horiAuto = horiScrollBarVisible == ScrollBarVisibility::sbvAutoScroll;
    bool hDisableScroll = horiScrollBarVisible == ScrollBarVisibility::sbvDisabled;
    bool vDisableScroll = vertScrollBarVisible == ScrollBarVisibility::sbvDisabled;

    Visibility vertVisible = Visibility::Visible;
    Visibility horiVisible = Visibility::Visible;

    switch (vertScrollBarVisible)
    {
    case ScrollBarVisibility::sbvVisible:
        vertVisible = Visibility::Visible;
        break;

    case ScrollBarVisibility::sbvHidden:
        vertVisible = Visibility::Hidden;
        break;

    default:
        vertVisible = Visibility::Collapsed;
    }

    switch (horiScrollBarVisible)
    {
    case ScrollBarVisibility::sbvVisible:
        horiVisible = Visibility::Visible;
        break;

    case ScrollBarVisibility::sbvHidden:
        horiVisible = Visibility::Hidden;
        break;

    default:
        horiVisible = Visibility::Collapsed;
    }

    if (_vertVisible != vertVisible)
    {
        SetVertScrollBarVisibility(vertVisible);
    }

    if (_horiVisible != horiVisible)
    {
        SetHoriScrollBarVisibility(horiVisible);
    }

    if (scrollInfo != NULL) 
    {
        scrollInfo->SetCanHorizontalScroll(!hDisableScroll); 
        scrollInfo->SetCanVerticalScroll(!vDisableScroll);
    }

    MeasureChild(element, constraint);
    scrollInfo = GetScrollInfo();

    // 如果需要自动计算滚动条
    // 需要判断逻辑大小是否超过可视范围，如果超过需要显示滚动条
    if ((scrollInfo != NULL) && (horiAuto || vertAuto))
    {
        bool horiGreat = horiAuto && scrollInfo->GetExtentWidth() > scrollInfo->GetViewportWidth();
        bool vertGreat = vertAuto && scrollInfo->GetExtentHeight() > scrollInfo->GetViewportHeight();

        if (horiGreat && (_horiVisible != Visibility::Visible))
        {
            SetHoriScrollBarVisibility(Visibility::Visible);
        }

        if (vertGreat && (_vertVisible != Visibility::Visible))
        {
            SetVertScrollBarVisibility(Visibility::Visible);
        }

        // 滚动条需要显示，重新计算
        if (horiGreat || vertGreat)
        {
            SetInChildInvalidateMeasure(true);
            element->Measure(constraint);
        }

        // 垂直和水平滚动条都需要自动计算，而且垂直和水平之中有一个
        // 逻辑大小大于可视区域范围，则需要调整滚动条
        if ((horiAuto && vertAuto) && (horiGreat != vertGreat))
        {
            bool recalcHori = !horiGreat && (scrollInfo->GetExtentWidth() > scrollInfo->GetViewportWidth());
            bool recalcVert = !vertGreat && (scrollInfo->GetExtentHeight() > scrollInfo->GetViewportHeight());

            if (recalcHori)
            {
                if (_horiVisible != Visibility::Visible)
                {
                    SetHoriScrollBarVisibility(Visibility::Visible);
                }
            }
            else if (recalcVert && (_vertVisible != Visibility::Visible))
            {
                SetVertScrollBarVisibility(Visibility::Visible);
            }

            if (recalcHori || recalcVert)
            {
                SetInChildInvalidateMeasure(true);
                MeasureChild(element, constraint);
            }
        }
    }

    SetInMeasure(false);
    SetInChildInvalidateMeasure(false);

    return element->GetDesiredSize();
}

void ScrollViewer::OnArrange(const Size& arrangeSize)
{
    ContentControl::OnArrange(arrangeSize);
    RaiseScrollChanged();
}

void ScrollViewer::OnApplyTemplate()
{
    ContentControl::OnApplyTemplate();

    // 从模板找到水平滚动条
    _horiScroll = DynamicCast<ScrollBar>(GetTemplateChild("PART_HorizontalScrollBar"));

    if (_horiScroll != NULL)
    {
        _horiScroll->SetIsStandalone(false);
    }

    _vertScroll = DynamicCast<ScrollBar>(GetTemplateChild("PART_VerticalScrollBar"));

    if (_vertScroll != NULL)
    {
        _vertScroll->SetIsStandalone(false);
    }
}

void ScrollViewer::OnPreApplyTemplate()
{
    ContentControl::OnPreApplyTemplate();

    if (GetTemplatedParent() != NULL)
    {
        //BindToTemplatedParent(HorizontalScrollBarVisibilityProperty);
        //BindToTemplatedParent(VerticalScrollBarVisibilityProperty);
        //BindToTemplatedParent(CanContentScrollProperty);
    }
}

void ScrollViewer::OnScrollChanged(ScrollChangedEventArg* e)
{
    RaiseEvent(e);
}

void ScrollViewer::OnRequestBringIntoView(RequestBringIntoViewEventArg* e)
{
    Element* child = NULL;
    FrameworkElement* fe = NULL;

    e->SetHandled(true);

    if (GetVisualChildrenCount() > 0)
    {
        child = this->GetVisualChild(0);
        fe = (FrameworkElement*)e->GetOriginalSource();
    }
}

void ScrollViewer::ScrollToVerticalOffsetByAnimation(int iStart, int iOffset, int duration, IEasingFunction* func)
{
    ObjectPtr cacheObj = func;

    if (NULL != _scrollInfo)
    {
        if (iStart < 0)
        {
            iStart = _scrollInfo->GetComputeVerticalOffset();
        }

        if (duration < 10)
        {
            duration = 200;
        }

        int iEnd = iStart + iOffset;

        Int32Animation* intAni = NULL;

        CoerceVerticalPos(iStart);
        CoerceVerticalPos(iEnd);

        if (iEnd != iStart)
        {
            intAni = new Int32Animation(iStart, iEnd, Duration(duration), FillBehavior::fbHoldEnd);
            intAni->SetDecelerationRatio(1);
            if (func != NULL)
            {
                intAni->SetEasingFunction(func);
            }
            BeginAnimation(VerticalOffsetProperty, intAni);
        }
    }
}

void ScrollViewer::OnMouseWheel(MouseWheelEventArg* e)
{
    if (!e->IsHandled() && NULL != _scrollInfo)
    {
        int iExtentHei = _scrollInfo->GetExtentHeight();
        int iViewportHei = _scrollInfo->GetViewportHeight();
        ScrollBarVisibility vertScrollBarVisible = GetVerticalScrollBarVisibility();

        int iDuration = 500;
        int iDeta = e->GetDelta() * -1 / WHEEL_DELTA;

        if (ScrollBarVisibility::sbvDisabled != vertScrollBarVisible && iExtentHei > iViewportHei)
        {
            if (iDeta > 0)
            {
                if (GetSmoothingScroll())
                {
                    ScrollToVerticalOffsetByAnimation(-1, GetViewportHeight() * 0.8, iDuration, NULL);
                }
                else
                {
                    _scrollInfo->MouseWheelDown();
                }
            }
            else
            {
                if (GetSmoothingScroll())
                {
                    ScrollToVerticalOffsetByAnimation(-1, GetViewportHeight() * -0.8, iDuration, NULL);
                }
                else
                {
                    _scrollInfo->MouseWheelUp();
                }
            }

            UpdateLayout();
            e->SetHandled(true);
        }
    }
}

void ScrollViewer::DoLayoutUpdated(Object* sender, EventArg* e)
{
}

void ScrollViewer::InvalidateScrollInfo(bool bRaised)
{
    bool _needRaised = false;
    IScrollInfo* scrollInfo = GetScrollInfo();

    if (NULL != scrollInfo)
    {
        if (_xSize != scrollInfo->GetViewportWidth() || _xExtent != scrollInfo->GetExtentWidth())
        {
            _xSize = scrollInfo->GetViewportWidth();
            _xExtent = scrollInfo->GetExtentWidth();

            if (_horiScroll)
            {
                _horiScroll->SetViewportSize(GetViewportWidth());
                _horiScroll->SetMaximum(GetScrollableWidth());
            }

            _needRaised = true;
        }
        
        if (_ySize != scrollInfo->GetViewportHeight() || _yExtent != scrollInfo->GetExtentHeight())
        {
            _ySize = scrollInfo->GetViewportHeight();
            _yExtent = scrollInfo->GetExtentHeight();

            if (_vertScroll)
            {
                _vertScroll->SetViewportSize(GetViewportHeight());
                _vertScroll->SetMaximum(GetScrollableHeight());
            }

            _needRaised = true;
        }

        if (bRaised)
        {
            SetScrollChangedDirty(true);
            RaiseScrollChanged();
        }

        //SetScrollChangedDirty(true);
    }
}

}
