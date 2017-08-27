// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////////
// ScrollBar.cpp

#include "stdafx.h"

#include <sui/thumb.h>
#include <sui/scrollbar.h>
#include <sui/repeatbutton.h>
#include <sui/stackpanel.h>
#include <sui/stacklayout.h>
#include <suic/render/uirender.h>

#include <math.h>

static const int CSSCROLLTIMER = 258;

namespace ui
{

suic::RoutedCommand* ScrollBar::PageUpCommand = NULL;
suic::RoutedCommand* ScrollBar::PageDownCommand = NULL;
suic::RoutedCommand* ScrollBar::PageLeftCommand = NULL;
suic::RoutedCommand* ScrollBar::PageRightCommand = NULL;

suic::RoutedCommand* ScrollBar::LineUpCommand = NULL;
suic::RoutedCommand* ScrollBar::LineDownCommand = NULL;
suic::RoutedCommand* ScrollBar::LineLeftCommand = NULL;
suic::RoutedCommand* ScrollBar::LineRightCommand = NULL;

suic::RoutedCommand* ScrollBar::ScrollStartCommand = NULL;
suic::RoutedCommand* ScrollBar::ScrollEndCommand = NULL;

suic::Rect ScrollBar::_rcTrack;
suic::Rect ScrollBar::_rcTrackStart;
suic::Point ScrollBar::_ptTrackStart;

ImplementTypeofClass(ScrollBar, ui::RangeBase)

ScrollBar::ScrollBar() 
    : _iStep(16)
    , _iVisualSize(0)
    , _iLogicSize(0)
    , _iScrollPos(0)
    , _iDelta(0)
    , _iTimer(0)
    , _iSpeed(20)
{
    _scroInfo.iVisualPos = 0;
    _scroInfo.iLogicPos = 0;
    _scroInfo._iTrackPos = 0;
    _scroInfo.iMax = 0;
    _scroInfo.iPage = 0;

    const int nxHScroll = 16;
    const int nxVScroll = 16;

    SetWidth(nxHScroll);
    SetHeight(nxVScroll);
}

ScrollBar::~ScrollBar()
{
}

bool ScrollBar::StaticInit()
{
    PageUpCommand = new suic::RoutedCommand(_T("PageUp"), ScrollBar::ClassType());
    PageDownCommand = new suic::RoutedCommand(_T("PageDown"), ScrollBar::ClassType());
    PageLeftCommand = new suic::RoutedCommand(_T("PageLeft"), ScrollBar::ClassType());
    PageRightCommand = new suic::RoutedCommand(_T("PageRight"), ScrollBar::ClassType());

    LineUpCommand = new suic::RoutedCommand(_T("LineUp"), ScrollBar::ClassType());
    LineDownCommand = new suic::RoutedCommand(_T("LineDown"), ScrollBar::ClassType());

    LineLeftCommand = new suic::RoutedCommand(_T("LineLeft"), ScrollBar::ClassType());
    LineRightCommand = new suic::RoutedCommand(_T("LineRight"), ScrollBar::ClassType());

    ScrollStartCommand = new suic::RoutedCommand(_T("ScrollStart"), ScrollBar::ClassType());
    ScrollEndCommand = new suic::RoutedCommand(_T("ScrollEnd"), ScrollBar::ClassType());

    IsMouseOverProperty->OverrideMetadata(ScrollBar::ClassType()
        , new suic::PropertyMetadata(suic::Boolean::False, &OnVisualStatePropChanged));

    suic::CommandHelper::RegisterClassCommandHandler(ClassType(),
        PageUpCommand, suic::ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
    suic::CommandHelper::RegisterClassCommandHandler(ClassType(),
        PageDownCommand, suic::ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

    suic::CommandHelper::RegisterClassCommandHandler(ClassType(),
        PageLeftCommand, suic::ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
    suic::CommandHelper::RegisterClassCommandHandler(ClassType(),
        PageRightCommand, suic::ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

    suic::CommandHelper::RegisterClassCommandHandler(ClassType(),
        LineUpCommand, suic::ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
    suic::CommandHelper::RegisterClassCommandHandler(ClassType(),
        LineDownCommand, suic::ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

    suic::CommandHelper::RegisterClassCommandHandler(ClassType(),
        LineLeftCommand, suic::ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
    suic::CommandHelper::RegisterClassCommandHandler(ClassType(),
        LineRightCommand, suic::ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

    suic::CommandHelper::RegisterClassCommandHandler(ClassType(),
        ScrollStartCommand, suic::ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));
    suic::CommandHelper::RegisterClassCommandHandler(ClassType(),
        ScrollEndCommand, suic::ExecutedRoutedEventHandler(ScrollBar::OnScrollCommand));

    return true;
}

void ScrollBar::OnScrollCommand(suic::Element* sender, suic::ExecutedRoutedEventArg& e)
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

bool ScrollBar::IgnoreVerticalStyle() const
{
    return false;
}

int ScrollBar::GetScrollPos() const
{
    return _scroInfo.iVisualPos;
}

int ScrollBar::GetScrollPage() const
{
    return _scroInfo.iPage;
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
    _scroInfo.iVisualPos = iPos;

    int iMin = 0;
    int iMaxPos = max(0, (_scroInfo.iMax - iMin + 1 - _scroInfo.iPage));

    if (_scroInfo.iVisualPos < iMin)
    {
        _scroInfo.iVisualPos = iMin;
    }

    _scroInfo.iVisualPos = min(iMaxPos, _scroInfo.iVisualPos);
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

    _scroInfo.iMax = (int)((double)_iLogicSize + 0.5);
    _scroInfo.iPage = (int)((double)_iVisualSize + 0.5);

    if (_iLogicSize <= _iVisualSize)
    {
        _scroInfo.iVisualPos = 0;
    }

    ScrollTo(_scroInfo.iVisualPos, false);
}

void ScrollBar::ScrollToBegin(bool bRepaint)
{
    if (_scroInfo.iVisualPos != 0)
    {
        _scroInfo.iVisualPos = 0;
        DoScroll(bRepaint);
    }
}

void ScrollBar::ScrollTo(int nNewPos, bool bRepaint)
{
    int iMin = 0;
    int iMaxPos = max(0, (_scroInfo.iMax - iMin + 1 - _scroInfo.iPage));

    if (nNewPos < iMin)
    {
        nNewPos = iMin;
    }

    int iOldPos = _scroInfo.iVisualPos;

    _scroInfo.iVisualPos = min(iMaxPos, nNewPos);

    if (iOldPos != _scroInfo.iVisualPos)
    {
        InvalidateArrange();
    }

    DoScroll(false, bRepaint);
}

void ScrollBar::ScrollToEnd(bool bRepaint)
{
    int iMin = 0;
    int iMaxPos = max(0, (_scroInfo.iMax - iMin + 1 - _scroInfo.iPage));

    if (_scroInfo.iVisualPos != iMaxPos)
    {
        _scroInfo.iVisualPos = iMaxPos;
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

suic::Rect ScrollBar::GetScrollThumbRect(bool bThumb)
{
    int iMin = 0;
    suic::Rect rectThumb;
    suic::Element* scroll(FindName(_T("PART_Scroll")));
    suic::FrameElement* one(CastElem<suic::FrameElement>(FindName(_T("PART_One"))));
    suic::FrameElement* two(CastElem<suic::FrameElement>(FindName(_T("PART_Two"))));
    suic::FrameElement* thumb(CastElem<suic::FrameElement>(FindTempName(_T("Thumb"))));

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
            nThumbPos = ::MulDiv(nClientSize, _scroInfo.iVisualPos - iMin, nMax - iMin);
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

void ScrollBar::OnSetCursor(suic::CursorEventArg& e)
{
    e.SetHandled(true);
}

void ScrollBar::OnMouseWheel(suic::MouseWheelEventArg& e)
{
    e.SetHandled(true);
}

void ScrollBar::OnApplyTemplate()
{
    suic::FrameElementPtr temp(FindTemplate(this, GetElementType()));

    if (temp)
    {
        suic::FrameElement* thumb(FindElem<suic::FrameElement>(temp, _T("PART_Thumb")));

        if (thumb)
        {
            SetTempName(thumb, _T("Thumb"));
            if (GetOrientation() == CoreFlags::Horizontal)
            {
                thumb->AddHandler(MouseMoveEvent, new suic::MouseEventHandler(this, &ScrollBar::OnHorzTrack), true);
            }
            else
            {
                thumb->AddHandler(MouseMoveEvent, new suic::MouseEventHandler(this, &ScrollBar::OnVertTrack), true);
            }

            thumb->AddHandler(MouseLButtonDownEvent, new suic::MouseEventHandler(this, &ScrollBar::OnBeginTrack), true);
            thumb->AddHandler(MouseLButtonUpEvent, new suic::MouseEventHandler(this, &ScrollBar::OnEndTrack), true);
        }

        SetViewElem(temp);
    }

    SetFocusable(false);
}

suic::Size ScrollBar::OnArrange(const suic::Size& arrangeSize)
{
    if (_elemView)
    {
        _elemView->Arrange(suic::Rect(0, 0, arrangeSize.cx, arrangeSize.cy));
    }

    suic::Element* scroll(FindName(_T("PART_Scroll")));

    if (scroll)
    {
        GetScrollThumbRect();
        scroll->InvalidateArrange();
    }

    return arrangeSize;
}

void ScrollBar::OnBeginTrack(suic::Element* sender, suic::MouseEventArg& e)
{
    suic::Element* scroll(FindTempName(_T("Thumb")));
    suic::Point pt(scroll->PointToScreen(suic::Point()));
    _rcTrackStart.SetRect(pt.x, pt.y, scroll->GetActualWidth(), scroll->GetActualHeight());

    _rcTrack = _rcTrackStart;
    _scroInfo._iTrackPos = _scroInfo.iVisualPos;
    _ptTrackStart = e.MousePoint();

    KillTimer(_iTimer);
}

void ScrollBar::OnEndTrack(suic::Element* sender, suic::MouseEventArg& e)
{
    _rcTrack.SetEmpty();
}

int ScrollBar::PositionFromThumb(int thumb, const suic::Size & szClamp)
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

void ScrollBar::OnHorzTrack(suic::Element* sender, suic::MouseEventArg& e)
{
    if (!_rcTrack.Empty())
    {
        int iMin = 0;
        suic::Element* scroll(FindName(_T("PART_Scroll")));

        suic::Point ptWindow = e.MousePoint();
        suic::Point ptSelf = scroll->PointToScreen(suic::Point());

        suic::Rect rect(ptSelf.x, ptSelf.y, scroll->GetRenderSize().cx, scroll->GetRenderSize().cy);
 
        suic::Size szClamp(0, 0);
		int nThumbLength = 0;

        szClamp.cx = rect.left;
        szClamp.cy = rect.right;

        nThumbLength = _rcTrackStart.Width();

        szClamp.cy -= nThumbLength;

        suic::Point ptOffset(ptWindow.x - _ptTrackStart.x, ptWindow.y - _ptTrackStart.y);
        suic::Rect rectNew(_rcTrackStart);

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
            int nPosNew = PositionFromThumb(_rcTrack.left - szClamp.cx, szClamp);

			if (_scroInfo.iVisualPos != nPosNew)
            {
                _scroInfo.iVisualPos = nPosNew;
                DoScroll(true);
            }
        }
    }
}

void ScrollBar::OnVertTrack(suic::Element* sender, suic::MouseEventArg& e)
{
    if (!_rcTrack.Empty())
    {
        int iMin = 0;
        suic::Element* scroll(FindName(_T("PART_Scroll")));

        suic::Point ptWindow = e.MousePoint();
        suic::Point ptSelf = scroll->PointToScreen(suic::Point());

        suic::Rect rect(ptSelf.x, ptSelf.y, scroll->GetRenderSize().cx, scroll->GetRenderSize().cy);
 
        suic::Size szClamp(0, 0);
		int nThumbLength = 0;

        szClamp.cx = rect.top;
        szClamp.cy = rect.bottom;
        nThumbLength = _rcTrackStart.Height();

		szClamp.cy -= nThumbLength;

        suic::Point ptOffset(ptWindow.x - _ptTrackStart.x, ptWindow.y - _ptTrackStart.y);
        suic::Rect rectNew(_rcTrackStart);

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
            int nPosNew = PositionFromThumb(_rcTrack.top - szClamp.cx, szClamp);

			if (_scroInfo.iVisualPos != nPosNew)
            {
                _scroInfo.iVisualPos = nPosNew;
                DoScroll(true);
            }
        }
    }
}

void ScrollBar::LineUp()
{
    if (_scroInfo.iVisualPos > 0)
    {
        _scroInfo.iVisualPos -= _iStep;
        if (_scroInfo.iVisualPos < 0)
        {
            _scroInfo.iVisualPos = 0;
        }
        DoScroll();
    }
}

void ScrollBar::LineDown()
{
    int iMin = 0;
    int iMaxPos = _scroInfo.iMax - iMin + 1 - _scroInfo.iPage;

    if (_scroInfo.iVisualPos < iMaxPos)
    {
        _scroInfo.iVisualPos += _iStep;
        if (_scroInfo.iVisualPos > iMaxPos)
        {
            _scroInfo.iVisualPos = iMaxPos;
        }
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
    if (_scroInfo.iVisualPos <= 0)
    {
        return;
    }

    _scroInfo.iVisualPos -= _scroInfo.iPage;

    if (_scroInfo.iVisualPos < 0)
    {
        _scroInfo.iVisualPos = 0;
    }

    DoScroll();
}

void ScrollBar::PageDown()
{
    int iMin = 0;
    int iMaxPos = _scroInfo.iMax - iMin + 1 - _scroInfo.iPage;

    if (_scroInfo.iVisualPos >= iMaxPos)
    {
        return;
    }

    _scroInfo.iVisualPos += _scroInfo.iPage;

    if (_scroInfo.iVisualPos > iMaxPos)
    {
        _scroInfo.iVisualPos = iMaxPos;
    }

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
        ui::ScrollEventArg scrollEvt(_scroInfo.iVisualPos);

        Scroll(this, scrollEvt);
        __setAutoDelete(bAutoDel);
    }
}

void ScrollBar::OnTimeout(int id, suic::Object* data)
{
    if (_iTimer == id)
    {
        KillTimer(_iTimer);

        int iMin = 0;
        int iCurPos = GetScrollPos();

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

    ScrollTo(GetScrollPos() + _iDelta, false);
}

}
