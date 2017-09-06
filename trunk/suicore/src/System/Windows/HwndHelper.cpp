
#include <System/Tools/VisualTreeOp.h>
#include <Framework/Animation/Storyboard.h>

#include <System/Windows/CoreTool.h>
#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>

#include <System/Input/Mouse.h>
#include <System/Tools/HwndHelper.h>
#include <System/Interop/System.h>
#include <System/Interop/InternalWindowOper.h>

namespace suic
{

bool IsMouseInElement(Element* element)
{
    Point point;

    ::GetCursorPos(&point);

    HWND hwnd = __HwndFromElement(element);

    if (hwnd != WindowFromPoint(point))
    {
        return false;
    }

    Rect rect = GetRenderBound(element);
    Rect wndrc;

    GetWindowRect(hwnd, &wndrc);

    point.x -= wndrc.left;
    point.y -= wndrc.top;

    if (!rect.PointIn(point)/* || !PointInWindow(element)*/)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool HwndHelper::IsAllowsTransparency(Element* d)
{
    return VisualTreeOp::IsAllowsTransparency(d);
}

Handle HwndHelper::GetHostHwnd(Element* elem)
{
    VisualHost* obj = VisualHost::GetVisualHost(elem);

    if (!obj)
    {
        return NULL;
    }
    else
    {
        return obj->GetHandle();
    }
}

Handle HwndHelper::GetActiveHandle()
{
    return HwndSubclass::GetTopActiveHwnd();
}

Window* HwndHelper::GetActiveWnd()
{
    return HwndSubclass::GetTopActiveWnd();
}

void HwndHelper::DragMoveWindow(Element* elem)
{
    if (MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress)
    {
        HWND hwnd = __HwndFromElement(elem);

        if (!IsWindowMinimize(elem) && !IsWindowMaximize(elem))
        {
            SendMessage(hwnd, WM_SYSCOMMAND, (WPARAM)SC_MOVE, 0); 
            SendMessage(hwnd, WM_LBUTTONUP, 0, 0);
        }
    }
}

Point PointTransform::GetCursorPoint()
{
    Point pt;

    GetCursorPos(&pt);

    return pt;
}

void HwndHelper::InvalidateAllWidows()
{

}

void HwndHelper::HandleDefMessage(MessageParam* mp)
{
    mp->result = (Uint32)::DefWindowProc(HANDLETOHWND(mp->hwnd), mp->message, mp->wp, mp->lp);
}

void HwndHelper::HandleLastMessage()
{    
    const MessageParam& mp = Assigner::Current()->lastmp;    
    ::DefWindowProc(HANDLETOHWND(mp.hwnd), mp.message, mp.wp, mp.lp);
}

static void SetWndPlacement(HWND hwnd, int nCode, Rect border)
{
    WINDOWPLACEMENT wndpl;  
    GetWindowPlacement(hwnd, &wndpl); // 获取当前窗口状态
    wndpl.showCmd = nCode;     

    if (SW_SHOWMAXIMIZED == nCode)
    {
        Rect rcWnd;
        MonitorInfo mi;
        mi = Environment::GetMonitorBoundByWindow(hwnd);

        rcWnd = mi.rcWork;
        border = SystemParameters::TransformToDevice(border.TofRect()).ToRect();

        rcWnd.left -= border.left;
        rcWnd.top -= border.top;
        rcWnd += border.left + border.right;
        rcWnd.bottom += border.top + border.bottom;

        ::SetWindowPos(hwnd, NULL, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);

        return;
    }

    SetWindowPlacement(hwnd, &wndpl);
}

void HwndHelper::SetWindowTitle(Element* elem, String title)
{
    HWND hwnd = __HwndFromElement(elem);
    if (::IsWindow(hwnd))
    {
        SetWindowText(hwnd, title.c_str());
    }
}

void HwndHelper::MaximizeWindow(Element* elem)
{
    if (elem->ReadFlag(CoreFlags::IsSysPending))
    {
        return;
    }

    POINT pt; 
    HWND hwnd = __HwndFromElement(elem);

    if (!::IsZoomed(hwnd))
    {
        ::GetCursorPos(&pt);
        ::PostMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, MAKELPARAM(pt.x, pt.y));
    }
}

void HwndHelper::RestoreWindow(Element* elem)
{
    if (elem->ReadFlag(CoreFlags::IsSysPending))
    {
        return;
    }

    POINT pt; 
    HWND hwnd = __HwndFromElement(elem);

    if (::IsIconic(hwnd) || ::IsZoomed(hwnd))
    {
        ::GetCursorPos(&pt);
        ::PostMessage(hwnd, WM_SYSCOMMAND, 
            SC_RESTORE, MAKELPARAM(pt.x, pt.y));
    }
}

void HwndHelper::MinimizeWindow(Element* elem)
{
    if (elem->ReadFlag(CoreFlags::IsSysPending))
    {
        return;
    }

    HWND hwnd = __HwndFromElement(elem);
    POINT pt;

    if (!::IsIconic(hwnd))
    {
        ::GetCursorPos(&pt);
        ::PostMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, MAKELPARAM(pt.x, pt.y));
    }
}

void uiEnableWindow(Element* elem, bool enable)
{
    HWND hwnd = __HwndFromElement(elem);
    if (enable)
    {
        ::EnableWindow(hwnd, TRUE);
    }
    else
    {
        ::EnableWindow(hwnd, FALSE);
    }
}

void HwndHelper::CloseWindow(Element* elem, bool bAsync)
{
    HWND hwnd = __HwndFromElement(elem);

    // 关掉主窗口
    if (::IsWindow(hwnd))
    {
        if (bAsync)
        {
            ::PostMessage(hwnd, WM_CLOSE, 0, 0);
        }
        else
        {
            ::SendMessage(hwnd, WM_CLOSE, 0, 0);
        }
    }
    else
    {
        ;
    }
}

bool HwndHelper::IsWindowMinimize(Element* elem)
{
    HWND hwnd = __HwndFromElement(elem);
    DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
    return (dwStyle & WS_MINIMIZE) == WS_MINIMIZE;
}

bool HwndHelper::IsWindowMaximize(Element* elem)
{
    HWND hwnd = __HwndFromElement(elem);
    DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
    return (dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE;
}

bool HwndHelper::IsWindowVisible(Element* elem)
{
    HWND hwnd = __HwndFromElement(elem);
    return ::IsWindowVisible(hwnd) ? true : false;
}

bool HwndHelper::IsWindowEnable(Element* elem)
{
    HWND hwnd = __HwndFromElement(elem);
    return ::IsWindowEnabled(hwnd) ? true : false;
}

}
