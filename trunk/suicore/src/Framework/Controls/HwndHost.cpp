// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// HwndHost.cpp

#include <Framework/Controls/HwndHost.h>

#include <System/Interop/System.h>
#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>

namespace suic
{

//////////////////////////////////////////////////////////////
// HwndHost
//

ImplementRTTIOfClass(HwndHost, FrameworkElement)

HwndHost::HwndHost()
    : _handle(0)
{
}

HwndHost::~HwndHost()
{
}

void HwndHost::OnInitialized(EventArg* e)
{
    FrameworkElement::OnInitialized(e);
}

void HwndHost::OnLoaded(LoadedEventArg* e)
{
    FrameworkElement::OnLoaded(e);

    HWND hwnd = HANDLETOHWND(_handle);

    if (::IsWindow(hwnd))
    {
        VisualHost* pHost = RTTICast<VisualHost>(e->GetTarget());
        if (NULL != pHost)
        {
            HWND hParent = HANDLETOHWND(pHost->GetHandle());
            if (::IsWindow(hParent))
            {
                ::SetParent(hwnd, hParent);
            }
        }
    }
}

void HwndHost::OnUnloaded(LoadedEventArg* e)
{
    FrameworkElement::OnUnloaded(e);

    HWND hwnd = HANDLETOHWND(_handle);

    if (::IsWindow(hwnd))
    {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }

    HwndSubclass::RemoveHwnd(_handle);

    _handle = 0;
}

void HwndHost::OnRender(Drawing * drawing)
{
    HWND hwnd = HANDLETOHWND(_handle);

    if (::IsWindow(hwnd))
    {
        UpdateWindowPos();
    }
}

void HwndHost::OnRenderSizeChanged(SizeChangedInfo& e)
{
    FrameworkElement::OnRenderSizeChanged(e);
    UpdateWindowPos();
}

void HwndHost::BuildWindow(Handle hwndParent)
{
    if (_handle == 0)
    {
        HwndParam hp;

        Point pt = PointToScreen(Point());

        hp.x = pt.x;
        hp.y = pt.y;
        hp.width = GetWidth();
        hp.height = GetHeight();

        hp.parent = hwndParent;

        hp.style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
        hp.exstyle = 0;
        hp.state = 0;

        if (GetHitTestFilterAction() != HitTestFilterAction::Continue)
        {
            hp.exstyle |= WS_EX_TRANSPARENT;
        }

        SysRenderSource* obj = new SysRenderSource(hp);
        VisualHostPtr pHost = obj;

        obj->Initialize(this);

        _handle = obj->GetHandle();
    }
}

void HwndHost::OnWindowPositionChanged(Rect rcBox)
{
    Uint32 flag = SWP_ASYNCWINDOWPOS | SWP_NOZORDER | SWP_NOCOPYBITS | SWP_NOACTIVATE;
    HWND hwnd = HANDLETOHWND(_handle);
    ::SetWindowPos(hwnd, NULL, rcBox.left, rcBox.top, rcBox.Width(), rcBox.Height(), flag);
}

void HwndHost::OnIsVisibleChanged(IsVisibleChangedEventArg* e)
{
    HWND hwnd = HANDLETOHWND(_handle);
    if (::IsWindow(hwnd))
    {
        if (e->GetValue())
        {
            ShowWindowAsync(hwnd, 8);
        }
        else
        {
            ShowWindowAsync(hwnd, 0);
        }
    }
}

void HwndHost::UpdateWindowPos()
{
    HWND hwnd = HANDLETOHWND(_handle);
    HWND host = ::GetParent(hwnd);

    if (::IsWindow(hwnd))
    {
        Rect rcChild;
        Rect rcParent;
        Point pt = PointToScreen(Point());

        ::GetWindowRect(host, &rcParent);
        ::GetWindowRect(hwnd, &rcChild);

        Size szReal(GetActualWidth(), GetActualHeight());
        Size szOri(rcChild.Width(), rcChild.Height());
        Point ptOri(rcChild.left - rcParent.left, rcChild.top - rcParent.top);

        if (ptOri != pt || szReal != szOri)
        {
            OnWindowPositionChanged(Rect(pt, szReal));
        }
        
        if (IsVisible())
        {
            ShowWindowAsync(hwnd, 5);
        }
        else
        {
            ShowWindowAsync(hwnd, 0);
        }
    }
}

void HwndHost::Sethandle(Handle h)
{
    _handle = h;
    UpdateWindowPos();
}

}
