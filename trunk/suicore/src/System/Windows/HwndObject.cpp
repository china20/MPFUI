
#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>

#include <Framework/Animation/Storyboard.h>

#include <System/Render/RenderManager.h>
#include <System/Interop/InternalOper.h>
#include <System/Interop/System.h>

#include <System/Tools/Thread.h>
#include <System/Tools/HwndHelper.h>
#include <System/Tools/VisualTreeOp.h>

#include <System/Windows/CoreTool.h>

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>

#include <System/Input/HwndMouseFilter.h>
#include <System/Input/HwndKeyboardFilter.h>

#include "mmsystem.h"
#include <System/Input/Dwmapi.h>

//#pragma comment(lib, "winmm.lib")

namespace suic
{

//ImplementRTTIOfClass(RenderInfo, Object)

RenderInfo::RenderInfo()
    : _needRender(true)
    , _hwndObj(NULL)
    , _mouseFilter(NULL)
    , _keyboardFilter(NULL)
    , _drawDebugLine(false)
    , _isRendering(false)
{
}

RenderInfo::~RenderInfo()
{
    if (NULL != _hwndObj)
    {
        _hwndObj->Clear();
        delete _hwndObj;
    }
    _dib.Clear();
}

void RenderInfo::InitHwndObject(HwndObject* obj)
{
    if (!_hwndObj)
    {
        _hwndObj = obj;
        _mouseFilter.SetHwndObject(_hwndObj);
        _keyboardFilter.SetHwndObject(_hwndObj);
    }
}

HwndObject::HwndObject(HwndParam hp)
    : _isActivate(false)
    , _mouseTracking(false)
    , _needRender(true)
    , _immtimerid(0)
    , _rootElement(NULL)
    , _isInSizeMode(false)
    , _isWindowInMinimizeState(false)
    , _myOwnUpdate(false)
    , _sizeToContent(SizeToContent::stcManual)
    , Object(false)
{
    _hwnd = new HwndAdapter(hp);
    _hwnd->ref();
}

HwndObject::HwndObject()
    : _isActivate(false)
    , _mouseTracking(false)
    , _needRender(true)
    , _isInSizeMode(false)
    , _immtimerid(0)
    , _rootElement(NULL)
    , _hwnd(NULL)
    , _isWindowInMinimizeState(false)
    , _myOwnUpdate(false)
    , _sizeToContent(SizeToContent::stcManual)
    , Object(false)
{
}

HwndObject::~HwndObject()
{
    Clear();
}

void HwndObject::Clear()
{
    FREEREFOBJ(_rootElement);
    FREEREFOBJ(_hwnd);
}

static Mutex system_lock;

Handle HwndObject::GetHandle()
{
    if (!_hwnd)
    {
        return 0;
    }
    else
    {
        return _hwnd->GetHwnd();
    }
}

bool HwndObject::Invalidate(const Rect* lprc, bool bForce)
{
    VisualHostPtr obj(VisualHost::GetVisualHost(GetRootElement()));
    if (obj.get())
    {
        obj->Invalidate(lprc, true);
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////
// HwndObject

HwndObject* HwndObject::GetHwndObject(Element* elem)
{
    VisualHost* obj(VisualHost::GetVisualHost(elem));
    if (obj == NULL)
    {
        return NULL;
    }
    else
    {
        RenderInfo* rInfo(obj->GetRenderInfo());
        return rInfo->GetHwndObject();
    }
}

RenderInfo* HwndObject::GetRenderInfo(Element* elem)
{
    VisualHost* obj(VisualHost::GetVisualHost(elem));
    if (obj == NULL)
    {
        return NULL;
    }
    else
    {
        return obj->GetRenderInfo();
    }
}

Size HwndObject::GetSizeFromHwnd()
{
    Rect rect;
    HWND hwnd = HANDLETOHWND(GetHandle());
    ::GetWindowRect(hwnd, &rect);
    return rect.ToSize();
}

void HwndObject::SetRootElement(Element* element)
{
    if (element != _rootElement)
    {
        Window* pWindow = RTTICast<Window>(element);
        if (NULL != pWindow)
        {
            SetSizeToContent(pWindow->GetSizeToContent());
        }

        SETREFOBJ(_rootElement, element);
        _rootElement->LayoutUpdated.Clear();
        _rootElement->LayoutUpdated += suic::EventHandler(this, &HwndObject::OnLayoutUpdated);
    }
}

void HwndObject::DisableSizeToContent()
{ 
    if (GetSizeToContent() != SizeToContent::stcManual)
    {
        _sizeToContent = SizeToContent::stcManual;

        Size sizeHwnd = GetSizeFromHwnd();
        Rect rect(0, 0, sizeHwnd.cx, sizeHwnd.cy);

        _rootElement->Measure(sizeHwnd); 
        _rootElement->Arrange(rect);

        RefleshUIShow(NULL, false);

        /*if (SizeToContentChanged != null)
        { 
            SizeToContentChanged(this, EventArgs.Empty);
        }*/
    }

    _rootElement->InvalidateVisual();
}

bool HwndObject::EnableSizeToContent()
{
    Window* pRoot = RTTICast<Window>(GetRootElement());
    return false;
}

void HwndObject::OnLayoutUpdated(Object* sender, EventArg* e)
{
    if (_rootElement != NULL)
    {
        Size renderSize = _rootElement->GetRenderSize();
        if (_previousSize != renderSize)
        {
            _previousSize = renderSize;
            if ((GetSizeToContent() != SizeToContent::stcManual) && 
                !IsWindowInMinimizeState())
            {
                Resize(renderSize);
            }
        }
    }
}

int HwndObject::GetSizeToContent() const
{
    return _sizeToContent;
}

void HwndObject::SetSizeToContent(int val)
{
    if (_sizeToContent != val)
    {
        _sizeToContent = val;
        SetLayoutSize();
    }
}

void HwndObject::Resize(Size newSize)
{
    try
    {
        _myOwnUpdate = true;
        if (IsUsable())
        {
            int flags = SWP_NOMOVE | SWP_SHOWWINDOW | SWP_FRAMECHANGED | SWP_NOCOPYBITS | SWP_NOZORDER | SWP_NOOWNERZORDER; 

            HWND hwnd = HANDLETOHWND(GetHandle());
            //bool bForce = _rootElement->ReadFlag(CoreFlags::IsSizeModing);
            CoreSystem::SetWndPos(hwnd, NULL, 0, 0, newSize.Width(), newSize.Height(), 0x16);
            //RefleshUIShow(bForce);
        }
    }
    catch (...)
    {
        
    }

    _myOwnUpdate = false;
}

void HwndObject::SetLayoutSize()
{
    if (_rootElement != NULL)
    {
        Size availableSize;

        if (GetSizeToContent() == SizeToContent::stcWidthAndHeight)
        {
            availableSize = Size(Numeric::MeasureInt, Numeric::MeasureInt);
            _rootElement->Measure(availableSize);
            availableSize = _rootElement->GetDesiredSize();
        }
        else
        {
            availableSize = GetSizeFromHwnd();
            if (GetSizeToContent() == SizeToContent::stcWidth)
            {
                availableSize.cx = Numeric::MeasureInt;
            }
            else if (GetSizeToContent() == SizeToContent::stcHeight)
            {
                availableSize.cy = Numeric::MeasureInt;
            }

            _rootElement->Measure(availableSize);

            if (GetSizeToContent() == SizeToContent::stcWidth)
            {
                availableSize.cx = _rootElement->GetDesiredSize().Width();
            }
            else if (GetSizeToContent() == SizeToContent::stcHeight)
            {
                availableSize.cy = _rootElement->GetDesiredSize().Height();
            }
        }

        Rect finalRect = Rect(Point(), availableSize);

        _rootElement->Arrange(finalRect);

        HandleWindowRgn(GetHandle(), _rootElement, availableSize.cx, availableSize.cy);

        _rootElement->InvalidateVisual();
    }
}

void HwndObject::RefleshUIShow(Rect* lpRect, bool bForce)
{
    Rect rect(Point(), GetSizeFromHwnd());

    if (NULL != lpRect)
    {
        rect.Union(lpRect);
    }
    
    // 
    // 刷新界面显示
    //
    VisualHost* pHost = VisualHost::GetVisualHost(_rootElement);

    if (NULL != pHost)
    {
        pHost->SetNeedRender(true);
        pHost->Invalidate(&rect, bForce);
    }
}

void HwndObject::InitializeElements(Element* element)
{
    if (!element->IsInitialized())
    {
        // 开始初始化
        element->BeginInit();

        for (int i = 0; i < element->GetVisualChildrenCount(); ++i)
        {
            InitializeElements(element->GetVisualChild(i));
        }

        element->EndInit();
    }
}

bool HwndObject::Process_WM_SYSCOMMAND(Element* rootElement, MessageParam* mp)
{
    Int32 sysCommand = mp->wp & 0xFFF0;

    CallDefWindowProc(mp);
    rootElement->WriteFlag(CoreFlags::IsSysPending, true);

    if (sysCommand == SC_RESTORE)
    {
        HandleWindowRgn(GetHandle(), rootElement, rootElement->GetActualWidth(), rootElement->GetActualHeight());
        if (HwndHelper::IsWindowMaximize(rootElement))
        {
            rootElement->SetValue(Window::WindowStateProperty, WindowStateBox::MaximizedBox);
        }
        else
        {
            rootElement->SetValue(Window::WindowStateProperty, WindowStateBox::NormalBox);
        }
    }
    else if (sysCommand == SC_MAXIMIZE)
    {
        rootElement->SetValue(Window::WindowStateProperty, WindowStateBox::MaximizedBox);
    }
    else if (sysCommand == SC_MINIMIZE)
    {
        rootElement->SetValue(Window::WindowStateProperty, WindowStateBox::MinimizedBox);
    }

    if (sysCommand == SC_SIZE || sysCommand == SC_MAXIMIZE)
    {
        DisableSizeToContent();
    }

    rootElement->WriteFlag(CoreFlags::IsSysPending, false);

    return true;
}

bool HwndObject::Process_WM_CREATE(Element* rootElement, MessageParam* mp)
{
    if (rootElement != NULL)
    {
        ElementPtr focused;
        DWORD dwTime = Environment::GetSystemTick();

        CallDefWindowProc(mp);

        SetLayoutSize();

        InitializeElements(rootElement);
        RecursiveLoadedWithFocused(rootElement, focused);

        if (focused.get() != NULL)
        {
            Keyboard::Focus(focused.get());
        }

        // ::SetTimer(HANDLETOHWND(HANDLETOHWND(mp->hwnd)), WM_INVALIDATERENDER, 10, NULL);

        return true;
    }
    else
    {
        return false;
    }
}

bool HwndObject::Process_WM_SIZE(Element* rootElement, MessageParam* mp)
{
    int maxFlag = (int)mp->wp;
    int x = LOWORD(mp->lp);
    int y = HIWORD(mp->lp);
    MonitorInfo mi = Environment::GetMonitorBoundByWindow(HANDLETOHWND(mp->hwnd));

    if (SIZE_MAXIMIZED == maxFlag)
    {
        x = _maxSize.x; // UIMIN(x, mi.rcMonitor.Width());
        y = _maxSize.y; // UIMIN(y, mi.rcMonitor.Height());
    }

    Size realSize(x, y);
    Rect finalRect(Point(), realSize);
    FrameworkElementPtr main(rootElement);

    SetWindowInMinimizeState((maxFlag == SIZE_MINIMIZED) ? true : false);

    if (!IsOwnUpdating() && 
        !IsWindowInMinimizeState() && 
        GetSizeToContent() != SizeToContent::stcWidthAndHeight)
    {
        Size prevSize = _previousSize;
        MeasureData* measureData = rootElement->GetMeasureData();

        if (GetSizeToContent() == SizeToContent::stcWidth)
        {
            realSize.cx = Numeric::MeasureInt;
            measureData->SetAvailableSize(Size(mi.rcMonitor.Width(), realSize.cy));
        }
        else if (GetSizeToContent() == SizeToContent::stcHeight)
        {
            realSize.cy = Numeric::MeasureInt;
            measureData->SetAvailableSize(Size(realSize.cx, mi.rcMonitor.Height()));
        }
        else
        {
            measureData->SetAvailableSize(realSize);
        }

        measureData->SetViewPort(Rect(Point(), measureData->GetAvailableSize()));

        main->InvalidateMeasure(realSize);

        if (GetSizeToContent() == SizeToContent::stcWidth)
        {
            realSize.cx = main->GetDesiredSize().Width();
        }
        else if (GetSizeToContent() == SizeToContent::stcHeight)
        {
            realSize.cy = main->GetDesiredSize().Height();
        }

        finalRect.right = realSize.cx;
        finalRect.bottom = realSize.cy;

        measureData->SetAvailableSize(realSize);
        measureData->SetViewPort(finalRect);

        main->ResetCanvasOffset();
        main->InvalidateArrange(finalRect);
    }

    RefleshUIShow(finalRect, false);

    return true;
}

bool HwndObject::Process_WM_WINDOWPOSCHANGING(Element* rootElement, MessageParam* mp)
{
    LPWINDOWPOS lpwndpos = (LPWINDOWPOS)(ULONG_PTR)mp->lp;

    Rect rc(Point(), rootElement->GetRenderSize());

    if ((lpwndpos->flags & SWP_NOSIZE) == 0 
        && (rc.Width() != lpwndpos->cx || rc.Height() != lpwndpos->cy))
    {
        rc.left = lpwndpos->x;
        rc.top = lpwndpos->y;
        rc.right = rc.left + lpwndpos->cx;
        rc.bottom = rc.top + lpwndpos->cy;

        HandleWindowRgn(GetHandle(), rootElement, rc.Width(), rc.Height());
    }

    return true;
}

void HwndObject::HandleWindowRgn(Handle hd, Element* fe, int w, int h)
{
    Size roundSize(0, 0);
    Object* obj = fe->GetValue(Window::RoundSizeProperty);

    if (obj)
    {
        roundSize = OSize::FromObj(obj);
    }

    if (roundSize.cx > 0 && roundSize.cy > 0)
    {
        HWND hwnd = HANDLETOHWND(hd);
        Size szRound(roundSize.cx, roundSize.cy);
        Rect rcElem(Point(), Size(w, h));
        DWORD dwStyle = ::GetWindowLongPtr(hwnd, GWL_STYLE);

        szRound.cx = SystemParameters::TransformXToDevice(szRound.cx);
        szRound.cy = SystemParameters::TransformYToDevice(szRound.cy);

        if ((dwStyle & WS_CHILD) == 0)
        {
            rcElem.Offset(-rcElem.left, -rcElem.top);
            HRGN hRgn = ::CreateRoundRectRgn(rcElem.left, rcElem.top
                , rcElem.right + 1, rcElem.bottom + 1, szRound.cx, szRound.cy);
            SetWindowRgn(hwnd, hRgn, TRUE);
            ::DeleteObject(hRgn);
        }
    }
}

void HwndObject::StartMouseTrack()
{
    if (!_mouseTracking && GetHandle())
    {
        TRACKMOUSEEVENT _tme = {sizeof(TRACKMOUSEEVENT)};

        _tme.hwndTrack = HANDLETOHWND(GetHandle()); 
        _tme.dwFlags = TME_LEAVE;

        TrackMouseEvent(&_tme);

        _mouseTracking = true;
    }
}

void HwndObject::StopMouseTrack()
{
    if (_mouseTracking && GetHandle())
    {
        TRACKMOUSEEVENT _tme = {sizeof(TRACKMOUSEEVENT)};

        _tme.hwndTrack = HANDLETOHWND(GetHandle()); 
        _tme.dwFlags = TME_CANCEL | TME_LEAVE;

        TrackMouseEvent(&_tme);

        _mouseTracking = false;

        if (GetRootElement() != ToolTip::Ins())
        {
            ToolTip::Ins()->Close();
        }
    }
}

bool HwndObject::Process_WM_GETMINMAXINFO(Element* rootElement, MessageParam* mp)
{
    Window* pWnd = RTTICast<Window>(rootElement);

    if (NULL != pWnd)
    {
        Rect rcArea;
        MonitorInfo mi;
        PMINMAXINFO mInfo = (PMINMAXINFO)(ULONG_PTR)mp->lp;
        Rect rcmgr = SystemParameters::TransformToDevice(pWnd->GetBorderThickness());

        mi = Environment::GetMonitorBoundByWindow(HANDLETOHWND(mp->hwnd));

        rcArea = mi.rcWork;
        rcArea.Offset(-rcArea.left, -rcArea.top);

        Size szMin(pWnd->GetMinWidth(), pWnd->GetMinHeight());
        Size szMax(pWnd->GetMaxWidth(), pWnd->GetMaxHeight());

        szMin = SystemParameters::TransformToDevice(szMin);
        szMax = SystemParameters::TransformToDevice(szMax);

        mInfo->ptMaxPosition.x = -rcmgr.left + rcArea.left;
        mInfo->ptMaxPosition.y = -rcmgr.top + rcArea.top;

        if (szMax.cx > rcArea.Width())
        {
            szMax.cx = rcArea.Width();
        }

        if (szMax.cy > rcArea.Height())
        {
            szMax.cy = rcArea.Height();
        }

        if (szMax.cx > 2)
        {
            mInfo->ptMaxSize.x = szMax.cx;
        }
        else
        {
            mInfo->ptMaxSize.x = rcArea.Width() + (rcmgr.left + rcmgr.right);
        }

        if (szMax.cy > 2)
        {
            mInfo->ptMaxSize.y = szMax.cy;
        }
        else
        {
            mInfo->ptMaxSize.y = rcArea.Height() + rcmgr.top + rcmgr.bottom;
        }

        _maxSize = mInfo->ptMaxSize;

        mInfo->ptMaxTrackSize = mInfo->ptMaxSize;

        if (szMin.cx > 2)
        {
            mInfo->ptMinTrackSize.x = szMin.cx;
        }

        if (szMin.cy > 2)
        {
            mInfo->ptMinTrackSize.y = szMin.cy;
        }
    }
    return true;
}

bool HwndObject::Process_WM_TIMER(Element* rootElement, MessageParam* mp)
{
    HWND hwnd = HANDLETOHWND(mp->hwnd);

    if (WM_INVALIDATERENDER == (int)mp->wp)
    {
        //VisualHost* pHost = VisualHost::GetVisualHost(rootElement);
        //::KillTimer(hwnd, 5);
        //Debug::Trace(_U("kill WM_INVALIDATERENDER timer\n"));
        //return true;
    }

    return false;
}

void HwndObject::Process_WM_DESTROY(Element* rootElement, MessageParam* mp)
{
}

void HwndObject::Process_WM_NCDESTROY(Element* rootElement, MessageParam* mp)
{
}

bool HwndObject::Process_WM_ACTIVATEAPP(Element* rootElement, MessageParam* mp)
{
    bool bInActive = (bool)(mp->wp != 0);
    int threadid = (int)mp->lp;

    if (bInActive)
    {
        if (_isActivate)
        {
            if (Mouse::GetDirectlyOver())
            {                      
                MouseButtonEventArg me(NULL, mp->point);
                Mouse::GetDirectlyOver()->OnMouseLeave(&me);
                Mouse::ChangeMouseOver(NULL);
            }

            Mouse::SetCaptured(NULL);
            _isActivate = false;
        }
    }
    else if (!_isActivate)
    {
        _isActivate = true;
    }

    mp->lp = 0;

    return true;
}

bool HwndObject::Process_WM_NCACTIVATE(Element* rootElement, MessageParam* mp)
{
    if (!rootElement)
    {
        return false;
    }

    bool handled = true;
    WindowPtr wnd(rootElement);
    EventArg e;

    if (mp->wp == 0)
    {
        mp->result = 1;

        if (wnd)
        {
            Point point;
            Popup* pop(Assigner::Current()->GetTopTrackPopup());

            ::GetCursorPos(&point);
            HWND ptHwnd = ::WindowFromPoint(point);

            while (pop != NULL)
            {
                pop->ref();
                pop->Close();

                Assigner::Current()->RemoveTrackPopup(pop);
                pop->unref();

                pop = Assigner::Current()->GetTopTrackPopup();
            }

            wnd->OnDeactivated(&e);
        }
    }
    else
    {
        mp->result = 0;
        if (wnd)
        {
            wnd->OnActivated(&e);
        }
    }

    //if (Environment::GetOSType() < OSType::eOSVista)
    if (::IsIconic(HANDLETOHWND(GetHandle())))
    {
        //CallDefWindowProc(mp);
        return false;
    }

    return true;
}

bool HwndObject::Process_WM_ACTIVATE(Element* rootElement, MessageParam* mp)
{
    bool bInActive = LOWORD(mp->wp) == WA_INACTIVE;

    if (bInActive)
    {
        //
        // 删除本窗口鼠标和焦点的元素
        //
        Element* prevOver = Mouse::GetDirectlyOver();
        Element* overPtr = VisualTreeOp::GetVisualRoot(prevOver);
        Element* capPtr = VisualTreeOp::GetVisualRoot(Mouse::GetCaptured());

        if (overPtr == rootElement)
        {
            MouseButtonEventArg me(NULL, mp->point);
            Mouse::ChangeMouseOver(NULL);
        }

        if (capPtr == rootElement)
        {
            Mouse::SetCaptured(NULL);
        }

        mp->result = 0;

        //InvalidateRect(HANDLETOHWND(GetHandle()), NULL, FALSE);
    }
    else
    {
        mp->result = 1;
        
        if (Environment::GetOSType() >= OSType::osVista)
        {
            Rect margins;

            margins.left = -1;
            margins.right = -1;
            margins.bottom = -1;
            margins.top = -1;
            //UIDwmApi::ExtendFrameIntoClientArea(HANDLETOHWND(mp->hwnd), margins);
            //UIDwmApi::EnableBlurBehindWindow(HANDLETOHWND(mp->hwnd), true);
        }

        //InvalidateRect(HANDLETOHWND(GetHandle()), NULL, FALSE);
    }

    //SetWindowPos(HANDLETOHWND(mp->hwnd), NULL, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOOWNERZORDER|SWP_NOMOVE|SWP_NOSIZE);

    return false;
}

bool HwndObject::OnRenderFilterMessage(Element* rootElement, MessageParam* mp)
{
    bool handled = false;

    switch (mp->message)
    {
    case WM_ERASEBKGND:
        mp->result = 1;
        return true;

    case WM_PRINT:
    case WM_PRINTCLIENT:
        mp->result = 1;
        return true;

    case WM_SYSCOMMAND: 
        handled = Process_WM_SYSCOMMAND(rootElement, mp);
        break;

    case WM_DESTROY:
        Process_WM_DESTROY(rootElement, mp);
        break;

    case WM_SIZING:
        handled = true;
        DisableSizeToContent();
        break;

    case WM_NCCALCSIZE:
        handled = true;
        if ((BOOL)mp->wp)
        {
            NCCALCSIZE_PARAMS* pCalcsize = (NCCALCSIZE_PARAMS*)(DWORD_PTR)mp->lp;
            Rect rect0 = pCalcsize->rgrc[0];
            Rect rect1 = pCalcsize->rgrc[1];
            mp->result = 0;
            pCalcsize->rgrc[1] = rect0;
            pCalcsize->rgrc[2] = rect1;
        }
        break;

    case WM_NCPAINT:
        handled = true;
        break;

    case WM_DEVICECHANGE:
    case WM_POWERBROADCAST:
        rootElement->InvalidateVisual();
        break;

    case WM_GETMINMAXINFO:
        handled = Process_WM_GETMINMAXINFO(rootElement, mp);
        break;

    /*case WM_DWMCOMPOSITIONCHANGED:
        break;*/
    }

    return handled;
}

void HwndObject::CallDefWindowProc(MessageParam* mp)
{
    mp->result = ::DefWindowProc(HANDLETOHWND(GetHandle()), mp->message, mp->wp, mp->lp);
}

//================================================================
// RenderInfo

void RenderInfo::RegisterHook(Object* obj, MessageHook hook)
{
    _hooks.RegisterHook(obj, hook);
}

void RenderInfo::RemmoveHook(Object* obj, MessageHook hook)
{
    if (!hook.safe_bool())
    {
        _hooks.RemoveHookFromObject(obj);
    }
    else
    {
        _hooks.RegisterHook(obj, hook);
    }
}

bool RenderInfo::OnHookFilterMessage(Element* rootElement, MessageParam* mp, bool& interrupt)
{
    return _hooks.DoMessageHook(rootElement, mp, interrupt);
}

bool RenderInfo::OnFilterMessage(Element* rootElement, MessageParam* mp)
{
    bool handled = false;
    bool interrupt = false;

    //switch (mp.message)
    //{
    //    case WM_NCPAINT:
    //    //case WM_NCACTIVATE:
    //    // WM_NCUAHDRAWCAPTION
    //    case 0x00AE:	
    //    // WM_NCUAHDRAWFRAME
    //    case 0x00AF:
    //        mp->result = WM_PAINT;
    //        return true;
    //}

    // 先处理 hooks 
    handled = OnHookFilterMessage(rootElement, mp, interrupt);

    // 处理绘制
    if (!handled)
    {
        handled = _hwndObj->OnRenderFilterMessage(rootElement, mp);
    }

    // 处理鼠标
    if (!handled)
    {
        handled = _mouseFilter.OnFilterMessage(rootElement, mp);
        /*if (mp.message >= WM_MOUSEFIRST && mp.message <= WM_MOUSELAST)
        {
            return handled;
        }*/
    }

    // 处理键盘
    if (!handled)
    {
        handled = _keyboardFilter.OnFilterMessage(rootElement, mp);
    }

    if (!handled)
    {
        handled = _hwndObj->OnPublicFilterMessage(rootElement, mp);
    }

    return handled;
}

void HwndObject::Close(bool bAsync)
{
    _hwnd->Close(bAsync);
}

bool HwndObject::OnPublicFilterMessage(Element* rootElement, MessageParam* mp)
{
    bool handled = false;

    switch (mp->message)
    {
    case WM_ACTIVATEAPP:
        handled = Process_WM_ACTIVATEAPP(rootElement, mp);
        break;

    case WM_ACTIVATE:
        handled = Process_WM_ACTIVATE(rootElement, mp);
        break;

    case WM_NCACTIVATE:
        handled = Process_WM_NCACTIVATE(rootElement, mp);
        break;

    case 0x02A2:
        handled = true;
        break;

    case WM_STYLECHANGED:
        handled = true;
        break;

    case WM_SIZE:
        handled = Process_WM_SIZE(rootElement, mp);
        break;

    case WM_WINDOWPOSCHANGED:
        {
            LPWINDOWPOS lpwndpos = (LPWINDOWPOS)(ULONG_PTR)mp->lp;
            if (lpwndpos != NULL)
            {
                lpwndpos = NULL;
            }
        }
        break;

    case WM_WINDOWPOSCHANGING:
        handled = Process_WM_WINDOWPOSCHANGING(rootElement, mp);
        break;

    /*case WM_SETTEXT:
        handled = true;
        break;*/

    case WM_TIMER:
        handled = Process_WM_TIMER(rootElement, mp);
        break;

    case WM_CREATE:
        handled = Process_WM_CREATE(rootElement, mp);
        break;

    case WM_DESTROY:
        Process_WM_ACTIVATE(rootElement, mp);
        Process_WM_DESTROY(rootElement, mp);
        break;

    case WM_NCDESTROY:
        Process_WM_NCDESTROY(rootElement, mp);
        break;

    case WM_ENTERSIZEMOVE:
        {
            SizeModeChangedEventArg e(_rootElement->GetRenderSize(), false);
            _rootElement->OnRenderSizeModeChanged(&e);
            _rootElement->WriteFlag(CoreFlags::IsSizeModing, true);
        }
        break;

    case WM_EXITSIZEMOVE:
        {
            SizeModeChangedEventArg e(_rootElement->GetRenderSize(), true);
            _rootElement->OnRenderSizeModeChanged(&e);
            _rootElement->WriteFlag(CoreFlags::IsSizeModing, false);
        }
        break;
    }

    return handled;
}

bool HwndObject::IsUsable()
{
    return IsWindow(HANDLETOHWND(GetHandle())) ? true : false;
}

/////////////////////////////////////////////////////////////////
//

ImplementRTTIOfClass(SysRenderSource, VisualHost)

SysRenderSource::SysRenderSource(HwndParam hp)
{
    RenderInfo* pInfo(GetRenderInfo());
    pInfo->InitHwndObject(new HwndObject(hp));
}

SysRenderSource::SysRenderSource()
{
}

SysRenderSource::~SysRenderSource()
{
}

Handle SysRenderSource::GetHandle()
{
    return GetHwndObject()->GetHandle();
}

bool SysRenderSource::Initialize(Element* rootElement)
{
    SetRootElement(CASTTOFE(rootElement));
    GetHwndObject()->SetRootElement(CASTTOFE(rootElement));

    if (GetHwndObject()->GetHwndAdapter()->BuildHwnd(this))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void SysRenderSource::SetHwnd(Handle hwnd)
{
    GetHwndObject()->GetHwndAdapter()->SetHwnd(hwnd);
}

FrameworkElement* SysRenderSource::GetRootElement()
{
    HwndObject* hwndObj = GetHwndObject();
    if (NULL == hwndObj) 
    {
        return NULL;
    }
    else
    {
        return CASTTOFE(hwndObj->GetRootElement());
    }
}

void SysRenderSource::StopMouseTrack()
{
    GetHwndObject()->StartMouseTrack();
}

void SysRenderSource::StartMouseTrack()
{
    GetHwndObject()->StopMouseTrack();
}

void SysRenderSource::Close(bool bAsync)
{
    GetHwndObject()->Close(bAsync);
}

Point SysRenderSource::GetScreenPoint()
{
    HWND hwnd = HANDLETOHWND(GetHandle());
    Rect rect;

    GetWindowRect(hwnd, &rect);
    // rect = SystemParameters::TransformFromDevice(rect.TofRect()).ToRect();

    return Point(rect.left, rect.top);
}

}
