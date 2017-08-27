// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////////
// ScrollBar.cpp

#include <Framework/Controls/ScrollBar.h>
#include <Framework/Controls/RepeatButton.h>
#include <System/Tools/CommandHelper.h>

#include <math.h>

static const int CSSCROLLTIMER = 258;

namespace suic
{

RoutedCommand* ScrollBar::PageUpCommand = NULL;
RoutedCommand* ScrollBar::PageDownCommand = NULL;
RoutedCommand* ScrollBar::PageLeftCommand = NULL;
RoutedCommand* ScrollBar::PageRightCommand = NULL;

RoutedCommand* ScrollBar::LineUpCommand = NULL;
RoutedCommand* ScrollBar::LineDownCommand = NULL;
RoutedCommand* ScrollBar::LineLeftCommand = NULL;
RoutedCommand* ScrollBar::LineRightCommand = NULL;

RoutedCommand* ScrollBar::ScrollStartCommand = NULL;
RoutedCommand* ScrollBar::ScrollEndCommand = NULL;

DpProperty* ScrollBar::OrientationProperty;

Rect ScrollBar::_rcTrack;
Rect ScrollBar::_rcTrackStart;
Point ScrollBar::_ptTrackStart;

ImplementTypeofClass(ScrollBar, RangeBase)

ScrollBar::ScrollBar() 
    : _iStep(1)
    , _iVisualSize(0)
    , _iLogicSize(0)
    , _iScrollPos(0)
    , _iDelta(0)
    , _iTimer(0)
    , _iSpeed(20)
    , _isStandalone(true)
{
    _scroInfo._iTrackPos = 0;
    _scroInfo.iMax = 0;
    _scroInfo.iPage = 0;

    const int nxHScroll = 16;
    const int nxVScroll = 16;

    bool del = __setAutoDelete(false);
    SetWidth(nxHScroll);
    SetHeight(nxVScroll);
    __setAutoDelete(del);
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::OnOrientationPropChanged(DpObject* d, DpPropChangedEventArg& e)
{

}

bool ScrollBar::StaticInit()
{
    if (IsStaticInit())
    {
        return true;
    }

    PageUpCommand = new RoutedCommand(_T("PageUp"), ScrollBar::ClassType());
    PageDownCommand = new RoutedCommand(_T("PageDown"), ScrollBar::ClassType());
    PageLeftCommand = new RoutedCommand(_T("PageLeft"), ScrollBar::ClassType());
    PageRightCommand = new RoutedCommand(_T("PageRight"), ScrollBar::ClassType());

    LineUpCommand = new RoutedCommand(_T("LineUp"), ScrollBar::ClassType());
    LineDownCommand = new RoutedCommand(_T("LineDown"), ScrollBar::ClassType());

    LineLeftCommand = new RoutedCommand(_T("LineLeft"), ScrollBar::ClassType());
    LineRightCommand = new RoutedCommand(_T("LineRight"), ScrollBar::ClassType());

    ScrollStartCommand = new RoutedCommand(_T("ScrollStart"), ScrollBar::ClassType());
    ScrollEndCommand = new RoutedCommand(_T("ScrollEnd"), ScrollBar::ClassType());

    IsMouseOverProperty->OverrideMetadata(ScrollBar::ClassType()
        , new PropMetadata(Boolean::False, PropMetadataOptions::AffectsNone, &OnVisualStatePropChanged));

    OrientationProperty = DpProperty::Register(_T("Orientation"), ClassType(), Integer::ClassType()
        , new PropMetadata(new Integer(CoreFlags::Vertical), PropMetadataOptions::AffectsMeasure, &ScrollBar::OnOrientationPropChanged));
    OrientationProperty->SetConvertValueCb(&OrientationConvert::Convert);

    CommandHelper::RegisterClassCommandHandler(ClassType(),
        PageUpCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
    CommandHelper::RegisterClassCommandHandler(ClassType(),
        PageDownCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

    CommandHelper::RegisterClassCommandHandler(ClassType(),
        PageLeftCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
    CommandHelper::RegisterClassCommandHandler(ClassType(),
        PageRightCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

    CommandHelper::RegisterClassCommandHandler(ClassType(),
        LineUpCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
    CommandHelper::RegisterClassCommandHandler(ClassType(),
        LineDownCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

    CommandHelper::RegisterClassCommandHandler(ClassType(),
        LineLeftCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
    CommandHelper::RegisterClassCommandHandler(ClassType(),
        LineRightCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

    CommandHelper::RegisterClassCommandHandler(ClassType(),
        ScrollStartCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
    CommandHelper::RegisterClassCommandHandler(ClassType(),
        ScrollEndCommand, ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

    return true;
}

void ScrollBar::OnScrollCommand(Element* sender, ExecutedRoutedEventArg& e)
{
    ScrollBar* scroll(CastElem<ScrollBar>(sender));

    if (e.GetCommand() == PageUpCommand)
    {
        scroll->PageUp();
    }
    else if (e.GetCommand() == PageDownCommand)
    {
        scroll->PageDown();
    }
    else if (e.GetCommand() == PageLeftCommand)
    {
        scroll->PageUp();
    }
    else if (e.GetCommand() == PageRightCommand)
    {
        scroll->PageDown();
    }
    else if (e.GetCommand() == LineUpCommand)
    {
        scroll->LineUp();
    }
    else if (e.GetCommand() == LineDownCommand)
    {
        scroll->LineDown();
    }
    else if (e.GetCommand() == LineLeftCommand)
    {
        scroll->LineLeft();
    }
    else if (e.GetCommand() == LineRightCommand)
    {
        scroll->LineRight();
    }
    else if (e.GetCommand() == ScrollStartCommand)
    {
        scroll->ScrollToBegin();
    }
    else if (e.GetCommand() == ScrollEndCommand)
    {
        scroll->ScrollToEnd();
    }

    e.SetHandled(true);
}

int ScrollBar::GetMaxScrollPos() const
{
    int iMaxPos = max(0, (_scroInfo.iMax - GetMinScrollPos() + 1 - _scroInfo.iPage));
    return iMaxPos;
}

int ScrollBar::GetMinScrollPos() const
{
    return 0;
}

void ScrollBar::SetScrollPos(int iPos)
{
    int iVisualPos = iPos;

    int iMin = 0;
    int iMaxPos = max(0, (_scroInfo.iMax - iMin + 1 - _scroInfo.iPage));

    if (iVisualPos < iMin)
    {
        iVisualPos = iMin;
    }

    iVisualPos = min(iMaxPos, iVisualPos);
    RangeBase::SetValue((double)iVisualPos);
}

void ScrollBar::UpdateScrollBar()
{
    InvalidateArrange();
    InvalidateVisual();
}

void ScrollBar::SetScrollStep(int iStep)
{
    _iStep = iStep;
}

int ScrollBar::GetScrollStep() const
{
    return _iStep;
}

void ScrollBar::CalcScrollPos()
{
    if (_iVisualSize <= 0)
    {
        _iVisualSize = 0;
    }

    int iVisualPos = (int)RangeBase::GetValue();

    _scroInfo.iMax = (int)((double)_iLogicSize + 0.5);
    _scroInfo.iPage = (int)((double)_iVisualSize + 0.5);

    if (_iLogicSize <= _iVisualSize)
    {
        iVisualPos = 0;
        RangeBase::SetValue(0);
    }

    ScrollTo(iVisualPos, false);
}

void ScrollBar::ScrollToBegin(bool bRepaint)
{
    RangeBase::SetValue(0);
    DoScroll(bRepaint);
}

void ScrollBar::ScrollTo(int nNewPos, bool bRepaint)
{
    int iMin = 0;
    int iMaxPos = max(0, (_scroInfo.iMax - iMin + 1 - _scroInfo.iPage));

    if (nNewPos < iMin)
    {
        nNewPos = iMin;
    }

    int iOldPos = (int)RangeBase::GetValue();

    int iVisualPos = min(iMaxPos, nNewPos);

    if (iOldPos != iVisualPos)
    {
        RangeBase::SetValue((double)iVisualPos);
        InvalidateArrange();
    }

    DoScroll(false, bRepaint);
}

void ScrollBar::ScrollToEnd(bool bRepaint)
{
    int iMin = 0;
    int iMaxPos = max(0, (_scroInfo.iMax - iMin + 1 - _scroInfo.iPage));

    if ((int)RangeBase::GetValue() != iMaxPos)
    {
        RangeBase::SetValue((double)iMaxPos);
        DoScroll(bRepaint);
    }
}

void ScrollBar::SetScrollInfo(int iVisual, int iLogic, bool bRepaint)
{
    UpdateScrollInfo(iVisual, iLogic, true);
    if (bRepaint)
    {
        InvalidateVisual();
    }
}

void ScrollBar::UpdateScrollInfo(int iVisual, int iLogic, bool bCalc)
{
    _iVisualSize = iVisual;
    _iLogicSize = iLogic;

    CalcScrollPos();

    if (bCalc)
    {
        GetScrollThumbRect();
    }
}

void ScrollBar::UpdateScrollVisual(int iVisual, bool bRepaint)
{
    if (_iVisualSize != iVisual)
    {
        _iVisualSize = iVisual;
    }

    CalcScrollPos();

    if (bRepaint)
    {
        InvalidateVisual();
    }
}

void ScrollBar::UpdateScrollLogic(int iLogic, bool bRepaint)
{
    if (_iLogicSize != iLogic)
    {
        _iLogicSize = iLogic;
    }

    CalcScrollPos();

    if (bRepaint)
    {
        InvalidateVisual();
    }
}

void ScrollBar::AddScrollLogic(int iLogic, bool bRepaint)
{
    UpdateScrollLogic(_iLogicSize + iLogic, bRepaint);
}

const int ScrollMin_Const = 4;
const int ScrollThumbMin_Const = 10;

Rect ScrollBar::GetScrollThumbRect(bool bThumb)
{
    int iMin = 0;
    Rect rectThumb;

    Element* scroll(GetTemplateChild(_T("PART_Scroll")));
    FrameworkElement* one(CastElem<FrameworkElement>(GetTemplateChild(_T("PART_One"))));
    FrameworkElement* two(CastElem<FrameworkElement>(GetTemplateChild(_T("PART_Two"))));
    FrameworkElement* thumb(CastElem<FrameworkElement>(GetTemplateChild(_T("PART_Track"))));

    // 滚动条大小尺寸
    int nClientSize = 0;
    int nThumbSize  = 0;

    if (GetOrientation() == CoreFlags::Horizontal)
    {
        nThumbSize = scroll->GetActualHeight();
        nClientSize = scroll->GetActualWidth();

        one->SetWidth(0);
        two->SetWidth(0);
        thumb->SetWidth(0);
    }
    else
    {
        // 得到滑块的尺寸
        nThumbSize = scroll->GetActualWidth();
        nClientSize = scroll->GetActualHeight();

        one->SetHeight(0);
        two->SetHeight(0);
        thumb->SetHeight(0);
    }

    if (nClientSize <= ScrollMin_Const)
    {
        return rectThumb;
    }

    int nThumbPos = 0;

    if (_scroInfo.iPage != 0)
    {
        nThumbSize = max(::MulDiv(nClientSize, _scroInfo.iPage, _scroInfo.iMax - iMin + 1), ScrollThumbMin_Const);
    }

    // 滑块为0或大于客户区，直接返回，不做处理
    if (nClientSize < nThumbSize || nThumbSize == 0)
    {
        Enable(false);
        return rectThumb;
    }
    else
    {
        // 计算位置去除滑块尺寸
        nClientSize -= nThumbSize;
        int nMax = _scroInfo.iMax - max(_scroInfo.iPage - 1, 0);

        if (iMin < nMax)
        {
            nThumbPos = ::MulDiv(nClientSize, (int)RangeBase::GetValue() - iMin, nMax - iMin);
        }
    }

    if (GetOrientation() == CoreFlags::Horizontal)
    {
        one->SetWidth(nThumbPos);
        two->SetWidth(scroll->GetActualWidth() - nThumbPos - nThumbSize);
        thumb->SetWidth(nThumbSize);
    }
    else
    {
        one->SetHeight(nThumbPos);
        two->SetHeight(scroll->GetActualHeight() - nThumbPos - nThumbSize);
        thumb->SetHeight(nThumbSize);
    }

    scroll->InvalidateMeasure();

    Enable(true);

    return rectThumb;
}

///////////////////////////////////////////////////
// 事件函数

void ScrollBar::OnSetCursor(CursorEventArg& e)
{
    e.SetHandled(true);
}

void ScrollBar::OnMouseWheel(MouseWheelEventArg& e)
{
    e.SetHandled(true);
}

void ScrollBar::OnApplyTemplate()
{
    RangeBase::OnApplyTemplate();
    SetFocusable(false);
}

Size ScrollBar::OnArrange(const Size& arrangeSize)
{
    if (GetTemplateChild())
    {
        GetTemplateChild()->Arrange(Rect(0, 0, arrangeSize.cx, arrangeSize.cy));
    }

    Element* scroll(GetTemplateChild(_T("PART_Scroll")));

    if (scroll)
    {
        GetScrollThumbRect();
        scroll->InvalidateArrange();
    }

    return arrangeSize;
}

void ScrollBar::OnBeginTrack(Element* sender, MouseButtonEventArg& e)
{
    Element* scroll(GetTemplateChild(_T("PART_Track")));
    Point pt(scroll->PointToScreen(Point()));
    _rcTrackStart.SetRect(pt.x, pt.y, scroll->GetActualWidth(), scroll->GetActualHeight());

    _rcTrack = _rcTrackStart;
    _scroInfo._iTrackPos = RangeBase::GetValue();
    _ptTrackStart = e.MousePoint();

    KillTimer(_iTimer);
}

void ScrollBar::OnEndTrack(Element* sender, MouseButtonEventArg& e)
{
    _rcTrack.SetEmpty();
}

int ScrollBar::PositionFromThumb(int thumb, const Size & szClamp)
{
    const int iRange1 = szClamp.cy - szClamp.cx;
    const int iMin = 0;
    const int iMax = _scroInfo.iMax - (_scroInfo.iPage - 1);
    const int iRange2 = iMax - iMin;

    if (iRange2 < 0)
    {
        return 0;
    }

    int iPos = iMin + 
        (int)(((double)thumb) * ((double)iRange2) / (double)(iRange1) + 0.5);

    if (iPos < iMin)
    {
        iPos = iMin;
    }

    if (iMax < iPos)
    {
        iPos = iMax;
    }

    return iPos;
}

void ScrollBar::OnHorzTrack(Element* sender, MouseButtonEventArg& e)
{
    if (!_rcTrack.Empty())
    {
        int iMin = 0;
        Element* scroll(GetTemplateChild(_T("PART_Scroll")));

        Point ptWindow = e.MousePoint();
        Point ptSelf = scroll->PointToScreen(Point());

        Rect rect(ptSelf.x, ptSelf.y, scroll->GetRenderSize().cx, scroll->GetRenderSize().cy);
 
        Size szClamp(0, 0);
		int nThumbLength = 0;

        szClamp.cx = rect.left;
        szClamp.cy = rect.right;

        nThumbLength = _rcTrackStart.Width();

        szClamp.cy -= nThumbLength;

        Point ptOffset(ptWindow.x - _ptTrackStart.x, ptWindow.y - _ptTrackStart.y);
        Rect rectNew(_rcTrackStart);

        if (abs(ptOffset.y) < 150)
        {
            rectNew.Offset(ptOffset.x, 0);

            if (rectNew.left < szClamp.cx)
            {
                rectNew.left = szClamp.cx;
                rectNew.right = rectNew.left + nThumbLength;
            }
            else if (szClamp.cy < rectNew.left)
            {
                rectNew.left = szClamp.cy;
                rectNew.right = rectNew.left + nThumbLength;
            }
        }

        if (!rectNew.Equal(&_rcTrack))
		{
			_rcTrack = rectNew;
            int iPosNew = PositionFromThumb(_rcTrack.left - szClamp.cx, szClamp);

            if ((int)RangeBase::GetValue() != iPosNew)
            {
                RangeBase::SetValue((double)iPosNew);
                DoScroll(true);
            }
        }
    }
}

void ScrollBar::OnVertTrack(Element* sender, MouseButtonEventArg& e)
{
    if (!_rcTrack.Empty())
    {
        int iMin = 0;
        Element* scroll(GetTemplateChild(_T("PART_Scroll")));

        Point ptWindow = e.MousePoint();
        Point ptSelf = scroll->PointToScreen(Point());

        Rect rect(ptSelf.x, ptSelf.y, scroll->GetRenderSize().cx, scroll->GetRenderSize().cy);
 
        Size szClamp(0, 0);
		int nThumbLength = 0;

        szClamp.cx = rect.top;
        szClamp.cy = rect.bottom;
        nThumbLength = _rcTrackStart.Height();

		szClamp.cy -= nThumbLength;

        Point ptOffset(ptWindow.x - _ptTrackStart.x, ptWindow.y - _ptTrackStart.y);
        Rect rectNew(_rcTrackStart);

        if (abs(ptOffset.x) < 150)
        {
            rectNew.Offset(0, ptOffset.y);

            if (rectNew.top < szClamp.cx)
            {
                rectNew.top = szClamp.cx;
                rectNew.bottom = rectNew.top + nThumbLength;
            }
            else if (szClamp.cy < rectNew.top)
            {
                rectNew.top = szClamp.cy;
                rectNew.bottom = rectNew.top + nThumbLength;
            }
        }

        if (!rectNew.Equal(&_rcTrack))
		{
			_rcTrack = rectNew;
            int iPosNew = PositionFromThumb(_rcTrack.top - szClamp.cx, szClamp);

            if ((int)RangeBase::GetValue() != iPosNew)
            {
                RangeBase::SetValue((double)iPosNew);
                DoScroll(true);
            }
        }
    }
}

void ScrollBar::LineUp()
{
    int iVisualPos = (int)RangeBase::GetValue();

    if (iVisualPos > 0)
    {
        iVisualPos -= _iStep;
        if (iVisualPos < 0)
        {
            iVisualPos = 0;
        }

        RangeBase::SetValue((double)iVisualPos);
        DoScroll();
    }
}

void ScrollBar::LineDown()
{
    int iMin = 0;
    int iVisualPos = (int)RangeBase::GetValue();
    int iMaxPos = _scroInfo.iMax - iMin + 1 - _scroInfo.iPage;

    if (iVisualPos < iMaxPos)
    {
        iVisualPos += _iStep;
        if (iVisualPos > iMaxPos)
        {
            iVisualPos = iMaxPos;
        }

        RangeBase::SetValue((double)iVisualPos);
        DoScroll();
    }
}

void ScrollBar::LineLeft()
{
    LineUp();
}

void ScrollBar::LineRight()
{
    LineDown();
}

void ScrollBar::PageUp()
{
    int iVisualPos = (int)RangeBase::GetValue();

    if (iVisualPos <= 0)
    {
        return;
    }

    iVisualPos -= _scroInfo.iPage;

    if (iVisualPos < 0)
    {
        iVisualPos = 0;
    }

    RangeBase::SetValue((double)iVisualPos);

    DoScroll();
}

void ScrollBar::PageDown()
{
    int iMin = 0;
    int iVisualPos = (int)RangeBase::GetValue();
    int iMaxPos = _scroInfo.iMax - iMin + 1 - _scroInfo.iPage;

    if (iVisualPos >= iMaxPos)
    {
        return;
    }

    iVisualPos += _scroInfo.iPage;

    if (iVisualPos > iMaxPos)
    {
        iVisualPos = iMaxPos;
    }

    RangeBase::SetValue((double)iVisualPos);

    DoScroll();
}

void ScrollBar::DoScroll(bool bUpdate, bool bExitTimer)
{
    if (bExitTimer)
    {
        KillTimer(_iTimer);
    }

    if (bUpdate)
    {
        UpdateScrollBar();
    }

    if (Scroll)
    {
        bool bAutoDel = __setAutoDelete(false);
        ScrollEventArg scrollEvt((int)RangeBase::GetValue());

        Scroll(this, scrollEvt);
        __setAutoDelete(bAutoDel);
    }
}

void ScrollBar::OnTimeout(int id, Object* data)
{
    if (_iTimer == id)
    {
        KillTimer(_iTimer);

        int iMin = 0;
        int iCurPos = (int)RangeBase::GetValue();

        if (iCurPos != _iScrollPos)
        {
            _iSpeed += 2;
            _iTimer = SetTimer(_iSpeed);

            int iMaxPos = GetMaxScrollPos();

            if (_iScrollPos > iMaxPos)
            {
                _iScrollPos = iMaxPos;
            }

            if (_iScrollPos < iMin)
            {
                _iScrollPos = iMin;
            }

            iCurPos += _iDelta;
            if ((_iDelta > 0 && iCurPos >= _iScrollPos) 
                || (_iDelta < 0 && iCurPos <= _iScrollPos))
            {
                KillTimer(_iTimer);
                iCurPos = _iScrollPos;
            }

            ScrollTo(iCurPos, false);
        }
        else
        {
            KillTimer(_iTimer);
        }
    }
}

void ScrollBar::DoScrollWithAnimate(int iNewPos, int iDelta, int iSpeed)
{
    _iSpeed = iSpeed;
    _iDelta = iDelta;
    _iScrollPos = iNewPos;

    if (_iTimer <= 0)
    {
        KillTimer(_iTimer);
        _iTimer = SetTimer(_iSpeed);
    }

    ScrollTo((int)RangeBase::GetValue() + _iDelta, false);
}

}
