//////////////////////////////////////////////////////////////////////////////
// uiwindowbase.cpp

#include <Framework/Controls/WindowBase.h>

#include <System/Windows/CoreTool.h>
#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Input/MouseDevice.h>
#include <System/Windows/AssignerInvoker.h>
#include <System/Interop/InternalWindowOper.h>
#include <System/Interop/InternalOper.h>

#include <System/Tools/Thread.h>
#include <System/Windows/HwndSubclass.h>
#include <System/Tools/ViewTreeHelper.h>

namespace suic
{

/////////////////////////////////////////////////////////
// CloseAssignerInvoker

class CloseAssignerInvoker : public AssignerInvoker
{
public:

    CloseAssignerInvoker(SysRenderSource* obj)
        : _obj(obj) {}

    void Invoke()
    {
        _obj->Close(true);
    }

protected:

    SysRenderSource* _obj;
};

ImplementTypeofClass(WindowBase, ContentControl)

DpProperty* WindowBase::AllowsTransparencyProperty;
DpProperty* WindowBase::ResizeModeProperty;
DpProperty* WindowBase::ShowInTaskbarProperty;
DpProperty* WindowBase::SizeToContentProperty;
DpProperty* WindowBase::TopmostProperty;
DpProperty* WindowBase::WindowStateProperty;
DpProperty* WindowBase::WindowStyleProperty;
DpProperty* WindowBase::RoundSizeProperty;
DpProperty* WindowBase::TitleProperty;
DpProperty* WindowBase::IconProperty;

Integer* ResizeModeBox::NoResizeBox = Integer::GetPosInt(ResizeMode::NoResize);
Integer* ResizeModeBox::CanMinimizeBox = Integer::GetPosInt(ResizeMode::CanMinimize);
Integer* ResizeModeBox::CanResizeBox = Integer::GetPosInt(ResizeMode::CanResize);
Integer* ResizeModeBox::CanResizeWithGripBox = Integer::GetPosInt(ResizeMode::CanResizeWithGrip);

class ResizeModeConvert
{
public:

    ResizeModeConvert()
    {

    }

    static Object* Convert(DpProperty* dp, Object* prop)
    {
        String val = prop->ToString();

        if (val.Equals(_T("CanMinimize")))
        {
            return ResizeModeBox::CanMinimizeBox;
        }
        else if (val.Equals(_T("CanResize")))
        {
            return ResizeModeBox::CanResizeBox;
        }
        else if (val.Equals(_T("CanResizeWithGrip")))
        {
            return ResizeModeBox::CanResizeWithGripBox;
        }
        else
        {
            return ResizeModeBox::NoResizeBox;
        }
    }
};

WindowBase::WindowBase()
    : _isClosing(false)
    , _owner(NULL)
    , _frame(NULL)
    , _ownerHandle(0)
    , _closeTimes(0)
    , _closeTimerId(0)
{
    SetSizeFrame(Size(2, 2));
}

WindowBase::~WindowBase()
{
    if (_frame)
    {
        _frame->__release();
    }
}

void WindowBase::OnAllowsTransparencyPropChanged(DpObject* d, DpPropChangedEventArg& e)
{
    WindowBase* wndBase = CastElem<WindowBase>(d);
    HWND hwnd = uiHwndFromElement(wndBase);

    if (::IsWindow(hwnd))
    {
        DWORD dwExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);

        if (e.GetNewValue()->ToBool())
        {
            ::SetWindowLong(hwnd, GWL_EXSTYLE, dwExStyle | 0x80000);
        }
        else
        {
            ::SetWindowLong(hwnd, GWL_EXSTYLE, dwExStyle & (~0x80000));
        }

        HwndLayoutFilter::HandleWindowRgn(HwndToHandle(hwnd), wndBase
            , wndBase->GetWidth(), wndBase->GetHeight());

        if (wndBase->IsLoaded())
        {
            wndBase->InvalidateVisual();
        }
    }
}

void WindowBase::OnTitlePropChanged(DpObject* d, DpPropChangedEventArg& e)
{
    WindowBase* pWnd = CastElem<WindowBase>(d);
    String title = e.GetNewValue()->ToString();
    HWND selfhwnd = uiHwndFromElement(pWnd);

    if (IsWindow(selfhwnd))
    {
        SetWindowText(selfhwnd, title.c_str());
    }
}

void WindowBase::OnResizeModePropChanged(DpObject* d, DpPropChangedEventArg& e)
{
    WindowBase* pWnd = CastElem<WindowBase>(d);
    ResizeMode mode = (ResizeMode)e.GetNewValue()->ToInt();

    if (mode == ResizeMode::CanResize || mode == ResizeMode::CanResizeWithGrip)
    {
        pWnd->SetWindowStyle(pWnd->GetWindowStyle() | WS_SIZEBOX);
    }
    else
    {
        pWnd->SetWindowStyle(pWnd->GetWindowStyle() & ~WS_SIZEBOX);
    }
}

bool WindowBase::StaticInit()
{
    if (IsStaticInit())
    {
        return true;
    }

    AllowsTransparencyProperty = DpProperty::Register(_T("AllowsTransparency")
        , ClassType(), Boolean::ClassType()
        , new PropMetadata(Boolean::False, PropMetadataOptions::AffectsRender, &WindowBase::OnAllowsTransparencyPropChanged));

    ResizeModeProperty = DpProperty::Register(_T("ResizeMode"), ClassType(), Integer::ClassType()
        , new PropMetadata(ResizeModeBox::NoResizeBox, PropMetadataOptions::AffectsNone, &WindowBase::OnResizeModePropChanged));
    ShowInTaskbarProperty = DpProperty::Register(_T("ShowInTaskbar")
        , ClassType(), Boolean::ClassType(), new PropMetadata(Boolean::False, PropMetadataOptions::AffectsNone));
    SizeToContentProperty = DpProperty::Register(_T("SizeToContent")
        , ClassType(), Boolean::ClassType(), new PropMetadata(Boolean::False, PropMetadataOptions::AffectsNone));
    TopmostProperty = DpProperty::Register(_T("Topmost")
        , ClassType(), Boolean::ClassType(), new PropMetadata(Boolean::False, PropMetadataOptions::AffectsNone));

    RoundSizeProperty = DpProperty::Register(_T("RoundSize"), ClassType(), OSize::ClassType()
        , new PropMetadata(OSize::GetEmptySize(), PropMetadataOptions::AffectsNone));
    IconProperty = DpProperty::Register(_T("Icon"), ClassType(), Bitmap::ClassType()
        , new PropMetadata(NULL, PropMetadataOptions::AffectsNone));
    TitleProperty = DpProperty::Register(_T("Title"), ClassType(), OString::ClassType()
        , new PropMetadata(new OString(_U("Sharpui Window")), PropMetadataOptions::AffectsNone, &WindowBase::OnTitlePropChanged));

    WindowStateProperty = DpProperty::Register(_T("WindowState"), ClassType(), Integer::ClassType()
        , new PropMetadata(new Integer(WS_MINIMIZEBOX | WS_MAXIMIZEBOX), PropMetadataOptions::AffectsRender));
    WindowStyleProperty = DpProperty::Register(_T("WindowStyle"), ClassType(), Integer::ClassType()
        , new PropMetadata(new Integer(WS_SYSMENU | WS_CLIPCHILDREN | WS_TABSTOP), PropMetadataOptions::AffectsRender));

    ResizeModeProperty->SetConvertValueCb(&ResizeModeConvert::Convert);
    FocusableProperty->OverrideMetadata(ClassType(), new PropMetadata(Boolean::True, PropMetadataOptions::AffectsNone));

    return true;
}

void WindowBase::SetTitle(String val)
{
    SetValue(TitleProperty, new OString(val));
}

bool WindowBase::OnFilterMessage(Object* sender, MessageParam& mp)
{
    bool handled = false;

    if (mp.message == WM_SHOWWINDOW)
    {
        if ((BOOL)mp.wp)
        {
            ::AnimateWindow(HandleToHwnd(mp.hwnd), 2000, AW_CENTER);
            OnShowWindow();
        }
        else
        {
            OnHideWindow();
        }
    }

    return handled;
}

void WindowBase::Dispose()
{
    if (_frame)
    {
        _frame->Dispose();
        _frame->__release();
        _frame = NULL;
    }

    ContentControl::Dispose();
}

bool WindowBase::IsClosing()
{
    return _isClosing;
}

bool WindowBase::IsClosed()
{
    HWND selfhwnd = uiHwndFromElement(this);

    if (!::IsWindow(selfhwnd))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool WindowBase::IsValid()
{
    HWND selfhwnd = uiHwndFromElement(this);

    if (::IsWindow(selfhwnd))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void WindowBase::SetCloseTimer(int times, int elapse)
{
    if (times > 0 && elapse > 10)
    {
        _closeTimes = times;
        if (_closeTimerId > 0)
        {
            //KillTimer(_closeTimerId);
        }
        //_closeTimerId = SetTimer(elapse);
    }
    else if (_closeTimerId > 0)
    {
        //KillTimer(_closeTimerId);
        _closeTimes = 0;
    }
}

int WindowBase::GetCloseTimes()
{
    return _closeTimes;
}

int WindowBase::GetCloseTimerId()
{
    return _closeTimerId;
}

void WindowBase::SetPos(int x, int y)
{
    SetLocation(Point(x, y));
}

void WindowBase::SetLocation(Point pt)
{
    _location = pt;
    
    HWND hwnd = uiHwndFromElement(this);
    if (::IsWindow(hwnd))
    {
        ::SetWindowPos(hwnd, NULL, _location.x, _location.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
}

Point WindowBase::GetLocation()
{
    HWND selfhwnd = uiHwndFromElement(this);

    if (::IsWindow(selfhwnd))
    {
        Rect rect;
        ::GetWindowRect(selfhwnd, &rect);
        _location.x = rect.left;
        _location.y = rect.top;
    }
    
    return _location;
}

inline void WindowBase::SetWindowStyle(Uint32 val)
{
    SetValue(WindowStyleProperty, new Integer((int)val));
}

inline void WindowBase::SetWindowState(Uint32 val)
{
    SetValue(WindowStateProperty, new Integer((int)val));
}

inline Uint32 WindowBase::GetWindowStyle()
{
    return (Uint32)GetValue(WindowStyleProperty)->ToInt();
}

inline Uint32 WindowBase::GetWindowState()
{
    return (Uint32)GetValue(WindowStateProperty)->ToInt();
}

void WindowBase::SetOwnerHandle(Handle ownerHandle)
{
    if (_ownerHandle == ownerHandle && !_ownerHandle)
    {
        return; 
    }

    _ownerHandle = ownerHandle;

    HWND hwnd = uiHwndFromElement(this);

    if (hwnd && ::IsWindow(hwnd))
    {
        SetWindowLong(hwnd,  GWLP_HWNDPARENT, ownerHandle); 
    }

    //NativeMethods.WndProc newWndProc = new NativeMethods.WndProc(SubclassWndProc);
    //IntPtr oldWndProc = UnsafeNativeMethods.GetWindowLongPtr(new HandleSt(this,hwnd), NativeMethods.GWL_WNDPROC); 
    //HookWindowProc(hwnd, newWndProc, oldWndProc);
}

bool WindowBase::AllowsTransparency()
{
    return GetValue(AllowsTransparencyProperty)->ToBool();
}

void WindowBase::SetAllowsTransparency(bool val)
{
    if (AllowsTransparency() != val)
    {
        SetValue(AllowsTransparencyProperty, BoolToBoolean(val));
    }
}

bool WindowBase::GetSizeToContent()
{
    return GetValue(SizeToContentProperty)->ToBool();
}

void WindowBase::SetSizeToContent(bool val)
{
    if (AllowsTransparency() != val)
    {
        SetValue(SizeToContentProperty, BoolToBoolean(val));
    }
}

void WindowBase::SetPopupCapture()
{
    HWND selfhwnd = uiHwndFromElement(this);

    if (::IsWindow(selfhwnd) && GetCapture() != selfhwnd)
    {
        ::SetCapture(selfhwnd);
    }
}

inline FrameworkElement* WindowBase::GetOwner() 
{ 
    return _owner; 
}

inline void WindowBase::SetOwner(FrameworkElement* owner) 
{
    if (this != owner)
    {
        _owner = owner; 
    }
}

WindowBase* WindowBase::Clone()
{
    WindowBase* pObj(CastElem<WindowBase>(ContentControl::Clone()));
    return pObj;
}

void WindowBase::ReleasePopupCapture()
{
    HWND selfhwnd = uiHwndFromElement(this);

    if (::IsWindow(selfhwnd) && GetCapture() == selfhwnd)
    {
        ::ReleaseCapture();
    }
}

void WindowBase::SetOwnerParent()
{
    if (GetOwner())
    {
        __SetParentByLong(GetOwner(), this); 
    }
}

void WindowBase::Resize(int cx, int cy)
{
    HWND hwnd = uiHwndFromElement(this);
    if (::IsWindow(hwnd))
    {
        ::SetWindowPos(hwnd, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
    }
}

void WindowBase::SetMove(int x, int y, int cx, int cy)
{
    HWND hwnd = uiHwndFromElement(this);
    if (::IsWindow(hwnd))
    {
        ::SetWindowPos(hwnd, NULL, x, y, cx, cy, SWP_NOZORDER);
    }

    _location.x = x;
    _location.y = y;
}

Rect WindowBase::GetWndRect()
{
    Rect rect;
    HWND hwnd = uiHwndFromElement(this);

    if (::IsWindow(hwnd))
    {
        ::GetWindowRect(hwnd, &rect);
    }

    return rect;
}

void WindowBase::Hide()
{

}

void WindowBase::Close()
{
    InternalClose(false);
}

void WindowBase::AsyncClose()
{
    InternalClose(true);
}

void WindowBase::SetTopMost(bool topmost)
{
    HWND self = uiHwndFromElement(this);
    if (::IsWindow(self))
    {
        SetWindowPos(self,(topmost ? HWND_TOPMOST : HWND_NOTOPMOST)
            , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
    }
}

void WindowBase::CenterWindow()
{
    if (!GetUIParent())
    {
        bool bDesk = true;

        HWND self = uiHwndFromElement(this);
        HWND hwnd = GetDesktopWindow();
        HWND hwndOwner = (HWND)(DWORD_PTR)GetWindowLong(self, GWLP_HWNDPARENT);

        if (::IsWindow(hwndOwner))
        {
            bDesk = false;
            hwnd = hwndOwner;
        }

        Rect rcOwner;
        Rect rcSelf;

        ::GetWindowRect(hwnd, &rcOwner);
        ::GetWindowRect(self, &rcSelf);

        int x = rcOwner.left + (rcOwner.Width() - rcSelf.Width()) / 2;
        int y = rcOwner.top + (rcOwner.Height() - rcSelf.Height()) / 2;

        int flags = SWP_NOZORDER | SWP_NOSIZE; 

        SetWindowPos(self, 0, x, y, 0, 0, flags);
    }
}

void WindowBase::InternalClose(bool async)
{
    if (IsValid())
    {
        SysRenderSource* obj = HwndSubclass::MatchHwndObject(this);

        if (obj)
        {
            if (GetAssigner()->GetThreadId() == UIThread::CurrentThreadId())
            {
                obj->Close(async);
            }
            else
            {
                if (async)
                {
                    GetAssigner()->BeginInvoke(new CloseAssignerInvoker(obj));
                }
                else
                {
                    GetAssigner()->Invoke(new CloseAssignerInvoker(obj), -1);
                }
            }
        }
    }
}

AssignerFrame* WindowBase::GetAssignerFrame()
{
    return _frame;
}

void WindowBase::SetAttribute(const String& key, Object* val)
{
    if (key.Equals(_T("SizeFrame")))
    {
        SetSizeFrame(val->ToSize());
    }
    else if (key.Equals(_T("Maximize")))
    {
        if (val->ToBool())
        {
            SetWindowStyle(GetWindowStyle() | WS_MAXIMIZE);
        }
        else
        {
            SetWindowStyle(GetWindowStyle() & ~WS_MAXIMIZE);
        }
    }
    else if (key.Equals(_T("Minimize")))
    {
        if (val->ToBool())
        {
            SetWindowStyle(GetWindowStyle() | WS_MINIMIZE);
        }
        else
        {
            SetWindowStyle(GetWindowStyle() & ~WS_MINIMIZE);
        }
    }
    else if (key.Equals(_T("MaximizeBox")))
    {
        if (val->ToBool())
        {
            SetWindowState(GetWindowState() | WS_MAXIMIZEBOX);
        }
        else
        {
            SetWindowState(GetWindowState() & ~WS_MAXIMIZEBOX);
        }
    }
    else if (key.Equals(_T("MinimizeBox")))
    {
        if (val->ToBool())
        {
            SetWindowState(GetWindowState() | WS_MINIMIZEBOX);
        }
        else
        {
            SetWindowState(GetWindowState() & ~WS_MINIMIZEBOX);
        }
    }
    else
    {
        ContentControl::SetAttribute(key, val);
    }
}

void WindowBase::OnTimeout(int id, Object* data)
{
    if (id == _closeTimerId)
    {
        --_closeTimes;
        if (_closeTimes <= 0)
        {
            //KillTimer(_closeTimerId);
            _closeTimes = 0;
            AsyncClose();
        }
    }
    else
    {
        //ContentControl::OnTimeout(id, data);
    }
}

void WindowBase::OnClosing(CancelEventArg& e)
{
    //KillTimer(_closeTimerId);
}

void WindowBase::OnClosed(EventArg& e)
{
}

}
