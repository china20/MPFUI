
#include <System/Windows/ScrollInfo.h>
#include <System/Windows/FrameworkElement.h>
#include <Framework/Controls/ScrollViewer.h>

namespace suic
{

ScrollInfo::ScrollInfo()
    : _owner(NULL)
{
    //__setAutoDelete(false);
}

ScrollInfo::~ScrollInfo()
{
}

void ScrollData::SetComputedOffset(const Point& offset)
{
    computedOffset = offset;
}

int ScrollInfo::GetComputeHorizontalOffset()
{
    return _scrollData.computedOffset.x;
}

int ScrollInfo::GetComputeVerticalOffset()
{
    return _scrollData.computedOffset.y;
}

int ScrollInfo::GetHorizontalOffset() const
{
    return _scrollData.offset.x;
}

int ScrollInfo::GetVerticalOffset() const
{
    return _scrollData.offset.y;
}

void ScrollInfo::SetHorizontalOffset(int offset)
{
    if (_scrollData.offset.x != offset)
    {
        _scrollData.offset.x = offset;
    }
}

void ScrollInfo::SetVerticalOffset(int offset)
{
    if (_scrollData.offset.y != offset)
    {
        _scrollData.offset.y = offset;
    }
}

bool ScrollInfo::CanHorizontalScroll() const
{
    return _scrollData.allowHorizontal;
}

bool ScrollInfo::CanVerticalScroll() const
{
    return _scrollData.allowVertical;
}

void ScrollInfo::SetCanHorizontalScroll(bool val)
{
    _scrollData.allowHorizontal = val;
}
void ScrollInfo::SetCanVerticalScroll(bool val)
{
    _scrollData.allowVertical = val;
}

void ScrollInfo::EnableHorizontalScroll(bool val)
{
    _scrollData.allowHorizontal = val;
}

void ScrollInfo::EnableVerticalScroll(bool val)
{
    _scrollData.allowVertical = val;
}

int ScrollInfo::GetExtentHeight() const
{
    return _scrollData.extent.cy;
}

int ScrollInfo::GetExtentWidth() const
{
    return _scrollData.extent.cx;
}

int ScrollInfo::GetViewportHeight() const
{
    return _scrollData.viewport.cy;
}

int ScrollInfo::GetViewportWidth() const
{
    return _scrollData.viewport.cx;
}

ScrollData* ScrollInfo::GetScrollData()
{
    return &_scrollData;
}

const ScrollData* ScrollInfo::GetScrollData() const
{
    return &_scrollData;
}

void ScrollInfo::SetOwner(FrameworkElement* owner)
{
    _owner = owner;
}

void ScrollInfo::LineDown()
{
    if (_scrollData.scrollOwner != NULL)
    {
        int iPos = GetComputeVerticalOffset() + ScrollViewer::scScrollLineDelta;
        _scrollData.scrollOwner->SetVerticalOffset(iPos);
    }
}

void ScrollInfo::LineLeft()
{
    if (_scrollData.scrollOwner != NULL)
    {
        int iPos = GetComputeHorizontalOffset() - ScrollViewer::scScrollLineDelta;
        _scrollData.scrollOwner->SetHorizontalOffset(iPos);
    }
}

void ScrollInfo::LineRight()
{
    if (_scrollData.scrollOwner != NULL)
    {
        int iPos = GetComputeHorizontalOffset() + ScrollViewer::scScrollLineDelta;
        _scrollData.scrollOwner->SetHorizontalOffset(iPos);
    }
}

void ScrollInfo::LineUp()
{
    if (_scrollData.scrollOwner != NULL)
    {
        int iPos = GetComputeVerticalOffset() - ScrollViewer::scScrollLineDelta;
        _scrollData.scrollOwner->SetVerticalOffset(iPos);
    }
}

void ScrollInfo::MouseWheelDown()
{
    if (_scrollData.scrollOwner != NULL)
    {
        int iPos = GetComputeVerticalOffset() + ScrollViewer::scMouseWheelDelta;
        _scrollData.scrollOwner->SetVerticalOffset(iPos);
    }
}

void ScrollInfo::MouseWheelLeft()
{
    if (_scrollData.scrollOwner != NULL)
    {
        int iPos = GetComputeHorizontalOffset() - ScrollViewer::scMouseWheelDelta;
        _scrollData.scrollOwner->SetHorizontalOffset(iPos);
    }
}

void ScrollInfo::MouseWheelRight()
{
    if (_scrollData.scrollOwner != NULL)
    {
        int iPos = GetComputeHorizontalOffset() + ScrollViewer::scMouseWheelDelta;
        _scrollData.scrollOwner->SetHorizontalOffset(iPos);
    }
}

void ScrollInfo::MouseWheelUp()
{
    if (_scrollData.scrollOwner != NULL)
    {
        int iPos = GetComputeVerticalOffset() - ScrollViewer::scMouseWheelDelta;
        _scrollData.scrollOwner->SetVerticalOffset(iPos);
    }
}

void ScrollInfo::PageDown()
{
    if (_scrollData.scrollOwner != NULL)
    {
        int iPos = GetComputeVerticalOffset() + GetViewportHeight();
        _scrollData.scrollOwner->SetVerticalOffset(iPos);
    }
}

void ScrollInfo::PageLeft()
{
    if (_scrollData.scrollOwner != NULL)
    {
        int iPos = GetComputeHorizontalOffset() - GetViewportWidth();
        _scrollData.scrollOwner->SetHorizontalOffset(iPos);
    }
}

void ScrollInfo::PageRight()
{
    if (_scrollData.scrollOwner != NULL)
    {
        int iPos = GetComputeHorizontalOffset() + GetViewportWidth();
        _scrollData.scrollOwner->SetHorizontalOffset(iPos);
    }
}

void ScrollInfo::PageUp()
{
    if (_scrollData.scrollOwner != NULL)
    {
        int iPos = GetComputeVerticalOffset() - GetViewportHeight();
        _scrollData.scrollOwner->SetVerticalOffset(iPos);
    }
}

Rect ScrollInfo::MakeVisible(Element* visual, Rect rect)
{
    if (MakeVisibleCb.safe_bool())
    {
        return MakeVisibleCb(visual, rect);
    }
    else if (_owner)
    {
        return _owner->MakeVisible(visual, rect);
    }
    else
    {
        return Rect();
    }
}

ScrollViewer* ScrollInfo::GetScrollOwner() const
{
    return _scrollData.scrollOwner;
}

void ScrollInfo::SetScrollOwner(ScrollViewer* val)
{
    _scrollData.scrollOwner = val;
}

}

