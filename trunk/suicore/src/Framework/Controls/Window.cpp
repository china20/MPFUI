//////////////////////////////////////////////////////////////////
// uiwindow.cpp

#include <Framework/Controls/Window.h>
#include <Framework/Controls/Application.h>

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Input/FocusManager.h>
#include <System/Input/KeyboardNavigation.h>

#include <System/Tools/Thread.h>
#include <System/Tools/HwndHelper.h>

#include <System/Windows/CoreTool.h>
#include <System/Windows/AssignerInvoker.h>

#include <System/Graphics/Bitmap.h>
#include <System/Graphics/ImageBrush.h>
#include <System/Graphics/Solidcolorbrush.h>

#include <System/Interop/InternalOper.h>
#include <System/Interop/System.h>

#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>

#include <System/Tools/SSE.h>
#include <System/Tools/IconHelper.h>

#include <System/DragDrop/DragDrop.h>
#include <System/Tools/TreeBuilder.h>

namespace suic
{

enum eCallWindowType
{
    cwtShow,
    cwtShowDialog,
    cwtShowUri,
    cwtShowDialogUri,
    cwtCloseWindow,
};

CancelEventArg::CancelEventArg(bool cancel)
    : _cancel(cancel)
{
}

bool CancelEventArg::IsCancel()
{
    return _cancel;
}

void CancelEventArg::SetCancel(bool cancel)
{
    _cancel = cancel;
}

WindowInvoker::WindowInvoker(Window* sender, int type)
{
    _target = sender;
    _holder = 0;
    _type = type;
    _result = 0;
    if (NULL != _target)
    {
        _target->ref();
    }
}

WindowInvoker::~WindowInvoker()
{
    if (NULL != _target)
    {
        _target->unref();
    }
}

void WindowInvoker::Invoke()
{
    Window* target = _target;
    if (target)
    {
        switch (_type)
        {
        case cwtShow:
            _result = target->ShowInternal(_holder, 0);
            break;

        case cwtShowDialog:
            _result = target->ShowDialogInternal(_holder);
            break;

        case cwtShowUri:
            _result = ShowUri(target, _uri, suic::cwtShowUri, _holder);
            break;

        case cwtShowDialogUri:
            _result = ShowUri(target, _uri, suic::cwtShowDialogUri, _holder);
            break;

        case cwtCloseWindow:
            target->InternalClose(_result != 0);
            break;
        }
    }
}

void WindowInvoker::CloseWindow(Window* wnd, bool bAsync)
{
    if (wnd->GetAssigner()->GetThreadId() != Thread::CurrentThreadId())
    {
        WindowInvoker* invoker = NULL;
        
        invoker = new WindowInvoker(wnd, cwtCloseWindow);
        invoker->ref();
        invoker->_result = bAsync ? 1 : 0;
        invoker->_holder = NULL;
        wnd->GetAssigner()->Invoke(invoker, 0);
        invoker->unref();
    }
    else
    {
        wnd->InternalClose(bAsync);
    }
}

int WindowInvoker::ShowOnThread(Window* wnd, Handle owner, const String& uri, int type, int timeout)
{
    int result = 0;
    WindowInvoker* invoker = new WindowInvoker(wnd, type);
    invoker->ref();
    invoker->_uri = uri;
    invoker->_holder = owner;
    wnd->GetAssigner()->Invoke(invoker, timeout);
    result = invoker->_result;
    invoker->unref();

    return result;
}

int WindowInvoker::Show(Window* wnd, Element* owner)
{
    Handle h = HWNDTOHANDLE(__HwndFromElement(owner));
    if (wnd->GetAssigner()->GetThreadId() != Thread::CurrentThreadId())
    {
        return ShowOnThread(wnd, h, _U(""), suic::cwtShow, 0);
    }
    else
    {
        return wnd->ShowInternal(h, 0);
    }
}

int WindowInvoker::ShowDialog(Window* wnd, Handle owner, int timeout)
{
    if (wnd->GetAssigner()->GetThreadId() != Thread::CurrentThreadId())
    {
        return ShowOnThread(wnd, owner, _U(""), suic::cwtShowDialog, timeout);
    }
    else
    {
        return wnd->ShowDialogInternal(owner);
    }
}

int WindowInvoker::ShowUri(Window* wnd, const String& uri, int type, Handle h)
{
    AutoObj root = Builder::BuildUri(wnd, uri);
    if (root.get() == wnd)
    {
        if (type == suic::cwtShowDialogUri)
        {
            return wnd->ShowDialogInternal(h);
        }
        else
        {
            return wnd->ShowInternal(h, 0);
        }
    }
    else
    {
        return -1;
    }
}

int WindowInvoker::Show(Window* wnd, const String& uri, Element* owner)
{
    Handle h = HWNDTOHANDLE(__HwndFromElement(owner));
    if (wnd->GetAssigner()->GetThreadId() != Thread::CurrentThreadId())
    {
        return ShowOnThread(wnd, h, uri, suic::cwtShowUri, 0);
    }
    else
    {
        return ShowUri(wnd, uri, suic::cwtShowUri, h);
    }
}

int WindowInvoker::ShowDialog(Window* wnd, const String& uri, int timeout)
{
    if (wnd->GetAssigner()->GetThreadId() != Thread::CurrentThreadId())
    {
        return ShowOnThread(wnd, NULL, uri, suic::cwtShowDialogUri, timeout);
    }
    else
    {
        return ShowUri(wnd, uri, suic::cwtShowDialogUri, NULL);
    }
}

/////////////////////////////////////////////////////////
//
class ResizeModeConvert : public IConvertValue
{
public:

    static ResizeModeConvert* Convert;

    ResizeModeConvert()
    {

    }

    AutoObj ConvertValue(DpProperty* dp, Object* prop)
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

ResizeModeConvert* ResizeModeConvert::Convert = new ResizeModeConvert();

class SizeToContentConvert : public IConvertValue
{
public:

    static SizeToContentConvert* Convert;

    SizeToContentConvert()
    {

    }

    AutoObj ConvertValue(DpProperty* dp, Object* prop)
    {
        String val = prop->ToString();

        if (val.Equals(_T("Width")))
        {
            return Integer::GetPosInt(SizeToContent::stcWidth);
        }
        else if (val.Equals(_T("Height")))
        {
            return Integer::GetPosInt(SizeToContent::stcHeight);
        }
        else if (val.Equals(_T("WidthAndHeight")))
        {
            return Integer::GetPosInt(SizeToContent::stcWidthAndHeight);
        }
        else
        {
            return Integer::GetPosInt(SizeToContent::stcManual);
        }
    }
};

SizeToContentConvert* SizeToContentConvert::Convert = new SizeToContentConvert();

class WindowStyleConvert : public IConvertValue
{
public:

    static WindowStyleConvert* Convert;

    WindowStyleConvert()
    {

    }

    AutoObj ConvertValue(DpProperty* dp, Object* prop)
    {
        String val = prop->ToString();

        if (val.Equals(_T("SingleBorderWindow")))
        {
            return WindowStyleBox::SingleBorderWindowBox;
        }
        else if (val.Equals(_T("ThreeDBorderWindow")))
        {
            return WindowStyleBox::ThreeDBorderWindowBox;
        }
        else if (val.Equals(_T("ToolWindow")))
        {
            return WindowStyleBox::ToolWindowBox;
        }
        else
        {
            return WindowStyleBox::NoneBox;
        }
    }
};

WindowStyleConvert* WindowStyleConvert::Convert = new WindowStyleConvert();

static HWND HwndFromRootWindow(Window* pWnd)
{
    HWND self = NULL;
    VisualHostPtr obj = VisualHost::GetVisualHost(pWnd);
    if (obj && obj->GetRootElement() == pWnd)
    {
        self = HANDLETOHWND(obj->GetHandle());
    }
    return self;
}

ImplementRTTIOfClass(Window, ContentControl)
ImplementRTTIOfClass(ResizeGrip, FrameworkElement)

Window::Window()
    : _dialogResult(0)
    , _frame(NULL)
    , _bigIcon(NULL)
    , _smallIcon(NULL)
    , _ownerHandle(0)
    , _windowFlag(0)
{
    _sizeToContent = SizeToContent::stcManual;
    SetSizeFrame(Size(2, 2));
}

Window::~Window()
{
    if (_bigIcon != 0)
    {
        ::DestroyIcon((HICON)(DWORD_PTR)_bigIcon);
    }
    if (_smallIcon != 0)
    {
        ::DestroyIcon((HICON)(DWORD_PTR)_smallIcon);
    }

    if (_frame)
    {
        _frame->unref();
    }
}

DpProperty* Window::IsActiveProperty;
DpProperty* Window::ShowActivatedProperty;

DpProperty* Window::AllowsTransparencyProperty;
DpProperty* Window::ResizeModeProperty;
DpProperty* Window::CanMovingProperty;
DpProperty* Window::ShowInTaskbarProperty;
DpProperty* Window::SizeToContentProperty;
DpProperty* Window::TopmostProperty;
DpProperty* Window::CaptionHeightProperty;
DpProperty* Window::WindowStateProperty;
DpProperty* Window::WindowStyleProperty;
DpProperty* Window::RoundSizeProperty;
DpProperty* Window::LocationProperty;
DpProperty* Window::TitleProperty;
DpProperty* Window::IconProperty;

Integer* ResizeModeBox::NoResizeBox = Integer::GetPosInt(ResizeMode::NoResize);
Integer* ResizeModeBox::CanMinimizeBox = Integer::GetPosInt(ResizeMode::CanMinimize);
Integer* ResizeModeBox::CanResizeBox = Integer::GetPosInt(ResizeMode::CanResize);
Integer* ResizeModeBox::CanResizeWithGripBox = Integer::GetPosInt(ResizeMode::CanResizeWithGrip);

Integer* WindowStateBox::NormalBox = Integer::GetPosInt(WindowState::wsNormal);
Integer* WindowStateBox::MinimizedBox = Integer::GetPosInt(WindowState::wsMinimized);
Integer* WindowStateBox::MaximizedBox = Integer::GetPosInt(WindowState::wsMaximized);

Integer* WindowStyleBox::NoneBox = Integer::GetPosInt(WindowStyle::wsNone);
Integer* WindowStyleBox::SingleBorderWindowBox = Integer::GetPosInt(WindowStyle::wsSingleBorderWindow);
Integer* WindowStyleBox::ThreeDBorderWindowBox = Integer::GetPosInt(WindowStyle::wsThreeDBorderWindow);
Integer* WindowStyleBox::ToolWindowBox = Integer::GetPosInt(WindowStyle::wsToolWindow);

Integer* WindowStateBox::From(int index)
{
    switch (index)
    {
    case WindowState::wsMinimized:
        return WindowStateBox::MinimizedBox;

    case WindowState::wsMaximized:
        return WindowStateBox::MaximizedBox;

    default:
        return WindowStateBox::NormalBox;
    }
}

Integer* WindowStyleBox::From(int index)
{
    switch (index)
    {
    case WindowStyle::wsSingleBorderWindow:
        return WindowStyleBox::SingleBorderWindowBox;

    case WindowStyle::wsThreeDBorderWindow:
        return WindowStyleBox::ThreeDBorderWindowBox;

    case WindowStyle::wsToolWindow:
        return WindowStyleBox::ToolWindowBox;

    default:
        return WindowStyleBox::NoneBox;
    }
}

Integer* ResizeModeBox::From(int index)
{
    switch (index)
    {
    case ResizeMode::CanMinimize:
        return ResizeModeBox::CanMinimizeBox;

    case ResizeMode::CanResize:
        return ResizeModeBox::CanResizeBox;

    case ResizeMode::CanResizeWithGrip:
        return ResizeModeBox::CanResizeWithGripBox;

    default:
        return ResizeModeBox::NoResizeBox;
    }
}

suic::FnEventDic* __stdcall Element::ClassEventInfo()
{
    return NULL;
}

void Window::OnAllowsTransparencyPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    pWnd->OnAllowsTransparencyChanged(e->GetOldValue()->ToBool(), e->GetNewValue()->ToBool());
}

void Window::OnIconPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    if (pWnd != NULL)
    {
        ImageSource* oldVal = RTTICast<ImageSource>(e->GetOldValue());
        ImageSource* newVal = RTTICast<ImageSource>(e->GetNewValue());
        pWnd->OnIconChanged(oldVal, newVal);
    }
}

void Window::OnWidthPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    suic::FrameworkElement::OnWidthPropChanged(d, e);
    if (pWnd != NULL)
    {
        pWnd->OnWidthChanged(e->GetOldValue()->ToInt(), e->GetNewValue()->ToInt());
    }
}

void Window::OnHeightPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    suic::FrameworkElement::OnHeightPropChanged(d, e);
    if (pWnd != NULL)
    {
        pWnd->OnHeightChanged(e->GetOldValue()->ToInt(), e->GetNewValue()->ToInt());
    }
}

void Window::OnMinWidthPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    if (pWnd != NULL)
    {
        pWnd->OnMinWidthChanged(e->GetOldValue()->ToInt(), e->GetNewValue()->ToInt());
    }
}

void Window::OnMinHeightPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    if (pWnd != NULL)
    {
        pWnd->OnMinHeightChanged(e->GetOldValue()->ToInt(), e->GetNewValue()->ToInt());
    }
}

void Window::OnMaxWidthPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    if (pWnd != NULL)
    {
        pWnd->OnMaxWidthChanged(e->GetOldValue()->ToInt(), e->GetNewValue()->ToInt());
    }
}

void Window::OnMaxHeightPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    if (pWnd != NULL)
    {
        pWnd->OnMaxHeightChanged(e->GetOldValue()->ToInt(), e->GetNewValue()->ToInt());
    }
}

void Window::OnWindowStatePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    if (NULL != pWnd)
    {
        Uint32 uState = e->GetNewValue()->ToInt();
        pWnd->OnWindowStateChanged(e->GetOldValue()->ToInt(), uState);
    }
}

void Window::OnAllowDropPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    if (NULL != pWnd)
    {
        pWnd->OnAllowDropChanged(e->GetOldValue()->ToBool(), e->GetNewValue()->ToBool());
    }
}

void Window::OnVisibilityPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    if (NULL != pWnd)
    {
        pWnd->OnVisibilityChanged((Visibility)e->GetOldValue()->ToInt(), 
            (Visibility)e->GetNewValue()->ToInt());
    }
    Element::OnVisibilityPropChanged(d, e);
}

void Window::OnWindowStylePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    if (NULL != pWnd)
    {
        Uint32 uStyle = e->GetNewValue()->ToInt();
        pWnd->OnWindowStyleChanged(e->GetOldValue()->ToInt(), uStyle);
    }
}

void Window::OnLocationPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    Point oldVal = OPoint::FromObj(e->GetOldValue());
    Point newVal = OPoint::FromObj(e->GetNewValue());

    if (NULL != pWnd)
    {
        pWnd->OnLocationChanged(oldVal, newVal);
    }
}

void Window::OnTopMostPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    bool topmost = e->GetNewValue()->ToBool();
    pWnd->OnTopMostChanged(e->GetOldValue()->ToBool(), topmost);
}

void Window::OnShowInTaskbarPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    bool val = e->GetNewValue()->ToBool();
    pWnd->OnShowInTaskbarChanged(e->GetOldValue()->ToBool(), val);
}

void Window::OnTitlePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    String title = e->GetNewValue()->ToString();
    pWnd->OnTitleChanged(e->GetOldValue()->ToString(), title);
}

void Window::OnResizeModePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    ResizeMode newVal = (ResizeMode)e->GetNewValue()->ToInt();

    if (NULL != pWnd)
    {
        pWnd->OnResizeModeChanged((ResizeMode)e->GetOldValue()->ToInt(), newVal);
    }
}

void Window::OnSizeToContentPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Window* pWnd = RTTICast<Window>(d);
    if (NULL != pWnd)
    {
        pWnd->OnSizeToContentChanged(e->GetOldValue()->ToInt(), e->GetNewValue()->ToInt());
    }
}

void Window::StaticInit()
{
    if (IsActiveProperty == NULL)
    {
        KeyboardNavigation::StaticInit();
        ContentControl::StaticInit();

        IsActiveProperty = DpProperty::Register(_T("IsActive"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));
        ShowActivatedProperty = DpProperty::Register(_T("ShowActivated"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));

        FocusableProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsNone));

        WidthProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(new Integer(-1), PropMetadataOptions::AffectsMeasure, &Window::OnWidthPropChanged));
        HeightProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(new Integer(-1), PropMetadataOptions::AffectsMeasure, &Window::OnHeightPropChanged));
        MinWidthProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(new Integer(0), PropMetadataOptions::AffectsMeasure, &Window::OnMinWidthPropChanged));
        MinHeightProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(new Integer(0), PropMetadataOptions::AffectsMeasure, &Window::OnMinHeightPropChanged));
        MaxWidthProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(new Integer(-1), PropMetadataOptions::AffectsMeasure, &Window::OnMaxWidthPropChanged));
        MaxHeightProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(new Integer(-1), PropMetadataOptions::AffectsMeasure, &Window::OnMaxHeightPropChanged));

        //---------------------------------------------------------------

        AllowsTransparencyProperty = DpProperty::Register(_T("AllowsTransparency"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender, &Window::OnAllowsTransparencyPropChanged));

        ResizeModeProperty = DpProperty::Register(_T("ResizeMode"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(ResizeModeBox::NoResizeBox, PropMetadataOptions::AffectsNone, &Window::OnResizeModePropChanged));

        CanMovingProperty = DpProperty::Register(_T("CanMoving"), RTTIType(), Boolean::RTTIType(), 
            DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsNone));

        ShowInTaskbarProperty = DpProperty::Register(_T("ShowInTaskbar"), RTTIType(), Boolean::RTTIType(), 
            DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsNone, &Window::OnShowInTaskbarPropChanged));
        SizeToContentProperty = DpProperty::Register(_T("SizeToContent"), RTTIType(), Integer::RTTIType(), 
            DpPropMemory::GetPropMeta(Integer::GetPosInt(SizeToContent::stcManual), PropMetadataOptions::AffectsMeasure, &Window::OnSizeToContentPropChanged));
        TopmostProperty = DpProperty::Register(_T("Topmost"), RTTIType(), Boolean::RTTIType(), 
            DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone, &Window::OnTopMostPropChanged));

        RoundSizeProperty = DpProperty::Register(_T("RoundSize"), RTTIType(), OSize::RTTIType()
            , DpPropMemory::GetPropMeta(OSize::GetEmpty(), PropMetadataOptions::AffectsNone));
        IconProperty = DpProperty::Register(_T("Icon"), RTTIType(), ImageSource::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &Window::OnIconPropChanged));
        TitleProperty = DpProperty::Register(_T("Title"), RTTIType(), OString::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &Window::OnTitlePropChanged));

        WindowStateProperty = DpProperty::Register(_T("WindowState"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(WindowStateBox::NormalBox, PropMetadataOptions::AffectsRender, &Window::OnWindowStatePropChanged));
        WindowStyleProperty = DpProperty::Register(_T("WindowStyle"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(WindowStyleBox::SingleBorderWindowBox, PropMetadataOptions::AffectsRender, &Window::OnWindowStylePropChanged));

        LocationProperty = DpProperty::Register(_T("Location"), RTTIType(), OPoint::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &Window::OnLocationPropChanged));
        CaptionHeightProperty = DpProperty::Register(_T("CaptionHeight"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(new Integer(25)));
        FocusManager::IsFocusScopeProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsNone));

        SizeToContentProperty->SetConvertValueCb(SizeToContentConvert::Convert);
        ResizeModeProperty->SetConvertValueCb(ResizeModeConvert::Convert);
        WindowStateProperty->SetConvertValueCb(WindowStateConvert::Convert);
        WindowStyleProperty->SetConvertValueCb(WindowStyleConvert::Convert);

        //ClipToBoundsProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsNone));
        VisibilityProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(VisibilityBox::VisibleBox, PropMetadataOptions::AffectsRender, &Window::OnVisibilityPropChanged));
        IsTabStopProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        AllowDropProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone, &Window::OnAllowDropPropChanged));

        KeyboardNavigation::DirectionalNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knCycle), PropMetadataOptions::AffectsNone)); 
        KeyboardNavigation::TabNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knCycle), PropMetadataOptions::AffectsNone)); 
        KeyboardNavigation::ControlTabNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knCycle), PropMetadataOptions::AffectsNone));
    }
}

void Window::OnAllowsTransparencyChanged(bool oldVal, bool newVal)
{
    HWND hwnd = HwndFromRootWindow(this);
    if (::IsWindow(hwnd))
    {
        DWORD dwExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);

        if (newVal)
        {
            ::SetWindowLong(hwnd, GWL_EXSTYLE, dwExStyle | 0x80000);
        }
        else
        {
            ::SetWindowLong(hwnd, GWL_EXSTYLE, dwExStyle & (~0x80000));
        }

        int w = SystemParameters::TransformXToDevice(GetWidth());
        int h = SystemParameters::TransformXToDevice(GetHeight());

        HwndObject::HandleWindowRgn(HWNDTOHANDLE(hwnd), this, w, h);

        if (IsLoaded())
        {
            InvalidateVisual();
        }
    }
}

void Window::OnIconChanged(ImageSource* oldVal, ImageSource* newVal)
{
    UpdateIcon();
}

void Window::OnWidthChanged(int oldVal, int newVal)
{
    if (!ReadFlag(CoreFlags::IsSizeModing))
    {
        UpdateWindowSize();
    }
}

void Window::OnHeightChanged(int oldVal, int newVal)
{
    if (!ReadFlag(CoreFlags::IsSizeModing))
    {
        UpdateWindowSize();
    }
}

void Window::OnMinWidthChanged(int oldVal, int newVal)
{
    UpdateMinMaxInfo();

    HWND self = HwndFromRootWindow(this);
    int iWid = newVal;

    if (iWid > GetWidth() && ::IsWindow(self))
    {
        SetWidth(iWid);
    }
}

void Window::OnMinHeightChanged(int oldVal, int newVal)
{
    UpdateMinMaxInfo();

    HWND self = HwndFromRootWindow(this);
    int iHei = newVal;

    if (iHei > GetHeight() && ::IsWindow(self))
    {
        SetHeight(iHei);
    }
}

void Window::OnMaxWidthChanged(int oldVal, int newVal)
{
    UpdateMinMaxInfo();

    HWND self = HwndFromRootWindow(this);
    int iWid = newVal;

    if (iWid < GetWidth() && ::IsWindow(self))
    {
        SetWidth(iWid);
    }
}

void Window::OnMaxHeightChanged(int oldVal, int newVal)
{
    UpdateMinMaxInfo();

    HWND self = HwndFromRootWindow(this);
    int iHei = newVal;

    if (iHei < GetHeight() && ::IsWindow(self))
    {
        SetHeight(iHei);
    }
}

void Window::OnWindowStateChanged(Uint32 oldVal, Uint32 newVal)
{
    Uint32 uState = newVal;
    HWND self = HwndFromRootWindow(this);
    if (::IsWindow(self))
    {
        Uint32 uStyle = ::GetWindowLongPtr(self, GWL_STYLE);
        uStyle &= ~(WS_MAXIMIZE | WS_MINIMIZE);

        if (uState == WindowState::wsMaximized)
        {
            uStyle |= WS_MAXIMIZE;
            HwndHelper::MaximizeWindow(this);
        }
        else if (uState == WindowState::wsMinimized)
        {
            uStyle |= WS_MINIMIZE;
            HwndHelper::MinimizeWindow(this);
        }
        else if (uState == WindowState::wsNormal)
        {
            HwndHelper::RestoreWindow(this);
        }

        //::SetWindowLongPtr(self, GWL_STYLE, uStyle);
        //pWnd->InvalidateVisual();
    }

    if (StateChanged)
    {
        StateChanged(this, &EventArg::Empty);
    }
}

void Window::OnAllowDropChanged(bool oldVal, bool newVal)
{
    HWND self = HwndFromRootWindow(this);
    if (::IsWindow(self))
    {
        Uint32 uPrevStyle = ::GetWindowLongPtr(self, GWL_EXSTYLE);

        if (newVal)
        {
            uPrevStyle |= WS_EX_ACCEPTFILES;
        }
        else
        {
            uPrevStyle &= ~WS_EX_ACCEPTFILES;
        }

        ::SetWindowLongPtr(self, GWL_EXSTYLE, uPrevStyle);
        DragDrop::RegisterDropTarget(self);
    }
}

void Window::OnVisibilityChanged(Visibility oldVal, Visibility newVal)
{
    HWND self = HwndFromRootWindow(this);
    if (::IsWindow(self))
    {
        if (newVal == Visibility::Visible)
        {
            ::ShowWindow(self, SW_SHOW);
        }
        else
        {
            if (_frame != NULL)
            {
                _frame->SetContinue(false);
            }
            ::ShowWindow(self, SW_HIDE);
        }
    }
}

void Window::OnWindowStyleChanged(Uint32 oldVal, Uint32 newVal)
{
    HWND self = HwndFromRootWindow(this);
    if (::IsWindow(self))
    {
        Uint32 uPrevStyle = ::GetWindowLongPtr(self, GWL_EXSTYLE);
        Uint32 uStyle = newVal;

        uPrevStyle &= ~(WS_EX_TOPMOST);

        if (uStyle & WindowStyle::wsToolWindow)
        {
            uPrevStyle |= WS_EX_TOPMOST;
        }

        ::SetWindowLongPtr(self, GWL_EXSTYLE, uPrevStyle);
    }
}

void Window::OnLocationChanged(Point oldVal, Point newVal)
{
    _location = newVal;

    HWND hwnd = HwndFromRootWindow(this);
    if (::IsWindow(hwnd))
    {
        Int32 flag = SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER;
        CoreSystem::SetWndPos(hwnd, NULL, _location.x, _location.y, 0, 0, flag);
    }
}

void Window::OnTopMostChanged(bool oldVal, bool newVal)
{
    HWND self = HwndFromRootWindow(this);
    if (::IsWindow(self))
    {
        CoreSystem::SetWndPos(self,(newVal ? HWND_TOPMOST : HWND_NOTOPMOST)
            , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
    }
}

void Window::OnShowInTaskbarChanged(bool oldVal, bool newVal)
{
    HWND self = HwndFromRootWindow(this);

    if (::IsWindow(self))
    {
        DWORD dwStyleEx = GetWindowLong(self, GWL_EXSTYLE);

        if (newVal)
        {
            dwStyleEx &= ~WS_EX_APPWINDOW;
            dwStyleEx |= WS_EX_TOOLWINDOW;
            SetWindowLong(self, GWL_EXSTYLE, dwStyleEx);
        }
        else
        {
            dwStyleEx &= ~WS_EX_TOOLWINDOW;
            dwStyleEx |= WS_EX_APPWINDOW;
            SetWindowLong(self, GWL_EXSTYLE, dwStyleEx);
        }
    }
}

void Window::OnTitleChanged(String oldTitle, String newTitle)
{
    HWND selfhwnd = HwndFromRootWindow(this);
    if (IsWindow(selfhwnd))
    {
        SetWindowText(selfhwnd, newTitle.c_str());
    }
}

void Window::OnResizeModeChanged(ResizeMode oldVal, ResizeMode newVal)
{
    ResizeMode mode = newVal;

    HWND selfhwnd = HwndFromRootWindow(this);

    if (IsWindow(selfhwnd))
    {
        Uint32 uStyle = ::GetWindowLongPtr(selfhwnd, GWL_STYLE);
        if (mode == ResizeMode::CanResize || mode == ResizeMode::CanResizeWithGrip)
        {
            ::SetWindowLongPtr(selfhwnd, GWL_STYLE, uStyle | WS_SIZEBOX);
        }
        else
        {
            ::SetWindowLongPtr(selfhwnd, GWL_STYLE, uStyle & ~WS_SIZEBOX);
        }
    }
}

void Window::OnSizeToContentChanged(int oldVal, int newVal)
{
    _sizeToContent = newVal;
    VisualHost* pHost = RTTICast<VisualHost>(GetValue(VisualHost::RootSourceProperty));
    if (pHost != NULL)
    {
        pHost->GetRenderInfo()->SetSizeToContent(newVal);
    }
}

void Window::CreateWindowStyle(Uint32& iStyle, Uint32& iStyleEx)
{
    iStyle &= -12582913;
    iStyleEx &= -641;
    switch (GetWindowStyle())
    {
    case WindowStyle::wsNone:
        iStyle &= -12582913;
        return;

    case WindowStyle::wsSingleBorderWindow:
        iStyle |= 0xc00000;
        return;

    case WindowStyle::wsThreeDBorderWindow:
        iStyle |= 0xc00000;
        iStyleEx |= 0x200;
        return;

    case WindowStyle::wsToolWindow:
        iStyle |= 0xc00000;
        iStyleEx |= 0x80;
        return;
    }
}

bool Window::OnFilterMessage(Object* sender, MessageParam* mp, bool& interrupt)
{
    bool handled = false;
    ElementPtr rootElement(sender);
    HWND hwnd = (HWND)(LONG_PTR)mp->hwnd;

    if (mp->message == WM_CLOSE)
    {
        // 正在关闭中，直接返回，防止重复调用
        if (ContainFlag(wfIsClosing))
        {
            return true;
        }

        CancelEventArg ce(false);
        Application* pApp = Application::Current();

        // 关闭中
        SetWndFlag(wfIsClosing, true);
        OnClosing(&ce);

        // 调用外部注册的通知事件
        if (Closing)
        {
            Closing(this, &ce);
        }

        // 关闭取消
        if (!ce.IsCancel())
        {
            if (_frame)
            {
                HWND hwnd = __HwndFromElement(this);
                CoreSystem::Ins()->EnableThreadWindowAndErase(hwnd);
                if (::IsWindow(HANDLETOHWND(_dialogPreviousActiveHandle)))
                {
                    ::SetFocus(HANDLETOHWND(_dialogPreviousActiveHandle));
                    ::SetActiveWindow(HANDLETOHWND(_dialogPreviousActiveHandle));
                }
            }

            //::AnimateWindow(hwnd, 200, AW_CENTER|AW_HIDE);
            EventArg e;
            OnClosed(&e);

            ::SendMessage(hwnd, WM_SETICON, 0, (LPARAM)NULL);
            ::SendMessage(hwnd, WM_SETICON, 1, (LPARAM)NULL);

            if (Closed)
            {
                Closed(this, &e);
            }

            if (NULL != pApp && pApp->GetMainWindow() == this)
            {
                ::PostQuitMessage(0);
            }
        }
        else
        {
            handled = true;
        }

        SetWndFlag(wfIsClosing, false);
    }
    else if (mp->message == WM_NCDESTROY)
    {
        if (GetAssignerFrame())
        {
            GetAssignerFrame()->SetContinue(false);
        }

        RemoveWindowFilter(this, MessageHook());
    }

    return handled;
}

void Window::Dispose()
{
    SetWndFlag(wfDisposed, true);
    ContentControl::Dispose();
}

bool Window::IsDisposed()
{
    return ContainFlag(wfDisposed);
}

ResizeMode Window::GetResizeMode()
{
    return (ResizeMode)GetValue(ResizeModeProperty)->ToInt();
}

void Window::SetResizeMode(ResizeMode val)
{
    SetValue(ResizeModeProperty, ResizeModeBox::From(val));
}

int Window::Show(const String& uri, Element* owner)
{
    return WindowInvoker::Show(this, uri, owner);
}

int Window::ShowDialog(const String& uri)
{
    return WindowInvoker::ShowDialog(this, uri, 0);
}

Handle Window::ShowChild(Handle parent)
{
    Handle hwnd = 0;
    if (0 != parent)
    {
        ShowInternal(0, parent);
        hwnd = HWNDTOHANDLE(__HwndFromElement(this));
    }
    return hwnd;
}

Handle Window::ShowChild(const String& uri, Handle parent)
{
    suic::AutoObj elem;
    elem = suic::Builder::BuildUri(this, uri);

    if (elem.get() == this)
    {
        return ShowChild(parent);
    }
    else
    {
        return 0;
    }
}

int Window::Show(Element* owner)
{
    return WindowInvoker::Show(this, owner);
}

int Window::ShowInternal(Handle owner, Handle parent)
{
    if (GetUIParent())
    {
        return 0;
    }
    else
    {
        HWND selfhwnd = __HwndFromElement(this);
        bool isChild = false;
        bool bCreateSucc = false;
        Int32 flag = SWP_NOSIZE | /*SWP_NOOWNERZORDER | SWP_NOZORDER | */SWP_SHOWWINDOW | SWP_NOMOVE | SWP_FRAMECHANGED;

        isChild = ::IsWindow(HANDLETOHWND(parent)) ? true : false;

        if (!selfhwnd || !::IsWindow(selfhwnd))
        {
            bCreateSucc = CreateWindowDuringShow(parent);
        }
        else if (IsLoaded())
        {
            SetVisibility(Visibility::Visible);
            return 0;
        }

        bool isShow = false;

        if (bCreateSucc)
        {
            selfhwnd = __HwndFromElement(this);
            SetClassLong(selfhwnd, GWL_STYLE, GetClassLong(selfhwnd, GWL_STYLE) | 0x00020000);

            isShow = IsVisible();

            HWND howner = HANDLETOHWND(GetOwnerHandle());
            if (NULL == howner && owner != NULL)
            {
                howner = HANDLETOHWND(owner);
            }

            if (::IsWindow(howner) && !isChild)
            {
                __SetParentByLongWithHwnd(howner, selfhwnd);
            }

            if (!isShow)
            {
                flag &= ~SWP_SHOWWINDOW;
            }
        }

        /*HANDLE curproc = GetCurrentProcess();
        ::SetProcessWorkingSetSize(curproc, -1, -1);
        ::CloseHandle(curproc);*/

        if (isShow && !OnShowingWindow())
        {
            if (!isChild)
            {
                if (GetWindowState() == WindowState::wsMaximized)
                {
                    ::ShowWindow(selfhwnd, SW_SHOWMAXIMIZED);
                }
                else if (GetWindowState() == WindowState::wsMinimized)
                {
                    ::ShowWindow(selfhwnd, SW_MINIMIZE);
                }
                else
                {
                    if (IsTopMost())
                    {
                        CoreSystem::SetWndPos(selfhwnd, HWND_TOPMOST, 0, 0, 0, 0, flag);
                    }
                    else
                    {
                        CoreSystem::SetWndPos(selfhwnd, NULL, 0, 0, 0, 0, flag);
                    }
                }
            }

            OnShowWindow();
        }

        if (IsFocusable())
        {
            ::SetFocus(selfhwnd);
        }

        if (!isChild)
        {
            Activate();
        }

        //UpdateLayout();
        //Assigner::Current()->GetTimeManager()->Tick();

        return 1;
    }
}

int Window::Show()
{
    return Show(NULL);
}

bool Window::IsShowInTaskBar()
{
    return GetValue(ShowInTaskbarProperty)->ToBool();
}

void Window::ShowInTaskBar(bool val)
{
    SetValue(ShowInTaskbarProperty, BOOLTOBOOLEAN(val));
}

int Window::ShowDialog()
{
    return WindowInvoker::ShowDialog(this, NULL, 0);
}

int Window::ShowDialogInternal(Handle owner)
{
    if (GetUIParent())
    {
        return 0;
    }

    HWND hOwner = HANDLETOHWND(owner);
    HWND selfhwnd = __HwndFromElement(this);
    bool isCreateSucc = false;
    Int32 flag = SWP_NOSIZE | /*SWP_NOOWNERZORDER | SWP_NOZORDER | */SWP_SHOWWINDOW | SWP_NOMOVE | SWP_FRAMECHANGED;

    _dialogPreviousActiveHandle = (Handle)(LONG_PTR)HwndSubclass::GetTopActiveHwnd();

    if (NULL == _dialogPreviousActiveHandle)
    {
        _dialogPreviousActiveHandle = (Handle)(LONG_PTR)::GetActiveWindow();
    }

    HWND hActiveWnd = (HWND)(LONG_PTR)_dialogPreviousActiveHandle;

    if (!IsInitialized())
    {
        RecursiveInit();
    }

    LONG lStyle = (LONG)GetWindowLongPtr(hActiveWnd, GWL_STYLE);
    LONG lExStyle = (LONG)GetWindowLongPtr(hActiveWnd, GWL_EXSTYLE);

    bool isChild = (lStyle & WS_CHILDWINDOW) != 0;

    if ((lExStyle & WS_EX_TOPMOST) != 0)
    {
        SetTopMost(true);
    }

    HWND hActiveParent = ::GetParent(hActiveWnd);
    while (NULL != hActiveParent)
    {
        hActiveWnd = hActiveParent;
        hActiveParent = ::GetParent(hActiveParent);
    }

    Rect rcwnd;
    ::GetWindowRect(hActiveWnd, &rcwnd);

    //
    // 找到线程内所有的窗口对象
    //
    HwndItems* pItem = NULL;
    BOOL bOwnerEnabled = TRUE;

    if (!::IsWindow(selfhwnd))
    {
        isCreateSucc = CreateWindowDuringShow(0);
    }
    else
    {
        SetVisibility(Visibility::Visible);
    }

    selfhwnd = __HwndFromElement(this);
    _frame = new AssignerFrame();
    _frame->ref();

    if (IsFocusable())
    {
        ::SetFocus(selfhwnd);
    }

    Activate();

    if (NULL == hOwner || !::IsWindow(hOwner))
    {
        pItem = CoreSystem::Ins()->SearchCurrentThreadWindow();
    }
    else
    {
        bOwnerEnabled = ::IsWindowEnabled(hOwner);
        ::EnableWindow(hOwner, FALSE);
    }

    if (NULL != pItem)
    {
        CoreSystem::Ins()->AddhwndItem(selfhwnd, pItem);
    }

    if (NULL != hOwner && ::IsWindowVisible(hOwner))
    {
        __SetParentByLongWithHwnd(hOwner, selfhwnd); 
    }
    else if (GetOwnerHandle())
    {
        __SetParentByLongWithHwnd((HWND)(LONG_PTR)GetOwnerHandle(), selfhwnd); 
    }
    else
    {
        __SetParentByLongWithHwnd(hActiveWnd, selfhwnd); 
    }

    if (!OnShowingWindow())
    {
        DWORD dwStyleEx = GetWindowLong(selfhwnd, GWL_EXSTYLE);

        dwStyleEx &= ~WS_EX_APPWINDOW;
        dwStyleEx |= WS_EX_TOOLWINDOW;
        SetWindowLong(selfhwnd, GWL_EXSTYLE, dwStyleEx);

        if (GetWindowState() == WindowState::wsMaximized)
        {
            ::ShowWindow(selfhwnd, SW_SHOWMAXIMIZED);
        }
        else
        {
            CoreSystem::SetWndPos(selfhwnd, NULL, 0, 0, 0, 0, flag);
        }

        OnShowWindow();
        CenterWindow(owner);
    }

    Assigner::Current()->PushMessageLoop(_frame);

    if (NULL != pItem)
    {
        CoreSystem::Ins()->EnableThreadWindowAndErase(selfhwnd);
    }
    else
    {
        ::EnableWindow(hOwner, bOwnerEnabled);
    }

    if (::IsWindow(hActiveWnd))
    {
        ::SetFocus(hActiveWnd);
        ::SetActiveWindow(hActiveWnd);
    }

    _dialogPreviousActiveHandle = 0;
    _frame->unref();
    _frame = NULL;

    return _dialogResult;
}

bool Window::IsCanMoving()
{
    return GetValue(CanMovingProperty)->ToBool();
}

void Window::SetCanMoving(bool val)
{
    SetValue(CanMovingProperty, BOOLTOBOOLEAN(val));
}

Int32 Window::GetDialogResult()
{
    return _dialogResult;
}

void Window::SetDialogResult(Int32 val)
{
    _dialogResult = val;
}

void Window::Hide()
{
    SetVisibility(Visibility::Hidden);
}

bool Window::HitTestCore(HitTestCtx* hitResult, bool onlySelf)
{
    Rect rect(GetCanvasOffset(), GetRenderSize());
    rect.Deflate(GetSizeFrame());
    return rect.PointIn(hitResult->hitPoint);
}

void Window::InvalidateForce()
{
    Element::InvalidateForce();
}

void Window::Activate()
{
    HWND selfhwnd = __HwndFromElement(this);

    if (::IsWindow(selfhwnd))
    {
        ::SetForegroundWindow(selfhwnd);
        InvalidateVisual();
    }
}

bool Window::StartDragMove()
{
    if (!IsCanMoving())
    {
        return false;
    }

    if (!HwndHelper::IsWindowMaximize(this))
    {
        HWND hwnd = HwndFromRootWindow(this);

        GetWindowRect(hwnd, &_startpos);
        GetCursorPos(&_startdragpos);

        if (0)
        {
            SetCaptureMouse();
        }
        else
        {
            PostMessage(hwnd, WM_SYSCOMMAND, 0xF012/*SC_MOVE*/, 0); 
            SendMessage(hwnd, WM_LBUTTONUP, 0, 0);
        }

        return true;
    }
    else
    {
        return false;
    }
}

void Window::DragMove()
{
    if (HwndHelper::IsWindowMaximize(this))
    {
        return ;
    }

    if (MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress && IsMouseCaptured())
    {
        Rect curRect(_startpos);
        Point loc;
        GetCursorPos(&loc);

        if (0)
        {
            HWND hwnd = HwndFromRootWindow(this);
            curRect.Offset(loc.x - _startdragpos.x, loc.y - _startdragpos.y);
            // CoreSystem::SetWndPos(hwnd, NULL, curRect.left, curRect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER /*| SWP_NOOWNERZORDER*/);
        }
    }
}

void Window::EndDragMove()
{
    if (IsMouseCaptured() && (0))
    {
        ReleaseCaptureMouse();
    }
}

bool Window::AllowsFullScreen()
{
    return ContainFlag(wfFullScreen);
}

void Window::SetAllowsFullScreen(bool val)
{
    if (AllowsFullScreen() != val)
    {
        SetWndFlag(wfFullScreen, val);

        HWND hwnd = HwndFromRootWindow(this);

        if (NULL != hwnd)
        {
            Uint32 flag = SWP_ASYNCWINDOWPOS | SWP_NOZORDER | SWP_NOCOPYBITS | SWP_NOACTIVATE;

            if (val)
            {
                WINDOWPLACEMENT wndpl; 
                wndpl.length = sizeof(WINDOWPLACEMENT); 
                MonitorInfo mi = Environment::GetMonitorBoundByWindow(hwnd);

                GetWindowPlacement(hwnd, &wndpl);
                _prevPos = wndpl.rcNormalPosition;

                if (_prevPos.Empty())
                {
                    ::GetWindowRect(hwnd, _prevPos);
                }

                //::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, szFull.Width(), szFull.Height(), flag);
                
                wndpl.flags = 0; 
                wndpl.showCmd = SW_SHOWNORMAL; 
                wndpl.rcNormalPosition = mi.rcMonitor; 
                ::SetWindowPlacement(hwnd, &wndpl);
            }
            else
            {
                ::SetWindowPos(hwnd, NULL, _prevPos.left, _prevPos.top, _prevPos.Width(), _prevPos.Height(), flag);
            }
        }
    }
}

void Window::OnPreviewGotKeyboardFocus(KeyboardFocusEventArg* e)
{
    if (GetUIParent() == NULL)
    {
        HWND hwnd = HwndFromRootWindow(this);
        if (NULL != hwnd && IsFocusable())
        {
            ::SetFocus(hwnd);
        }
    }
}

suic::FnEventDic* Element::GetEventInfo()
{
    return ClassEventInfo();
}

void Window::OnClosed(EventArg* e)
{
}

void Window::OnClosing(CancelEventArg* e)
{
    
}

void Window::OnActivated(EventArg* e)
{
    SetWndFlag(wfIsClosing, false);

    if (!ContainFlag(wfIsActivate))
    {
        SetWndFlag(wfIsActivate, true);
    }
}

void Window::OnDeactivated(EventArg* e)
{
    if (ContainFlag(wfIsActivate))
    {
        SetWndFlag(wfIsActivate, false);
    }
}

bool Window::OnShowingWindow()
{
    return false;
}

void Window::OnShowWindow()
{

}

void Window::OnHideWindow()
{
}

bool Window::OnEvent(Object* sender, MessageParam* mp)
{
    return false;
}

bool Window::OnBuild(IXamlNode* pNode, ObjectPtr& obj)
{
    return false;
}

void Window::OnConnect(IXamlNode* pNode, Object* target)
{
    ;
}

void Window::OnMouseMove(MouseButtonEventArg* e)
{
    ContentControl::OnMouseMove(e);

    if (MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress && IsMouseCaptured())
    {
        DragMove();
        e->SetHandled(true);
    }
}

void Window::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    ContentControl::OnMouseLeftButtonDown(e);
    Rect rect(0, 0, GetActualWidth(), GetCaptionHeight());
    HWND hwnd = HwndFromRootWindow(this);

    if (AllowsFullScreen() && !GetUIParent())
    {
        e->SetHandled(true);
        return;
    }

    if (e->GetClickCount() % 2 == 0 && rect.PointIn(e->GetMousePoint()) && 
        (GetWindowStyle() != WindowStyle::wsNone) && ::IsWindow(hwnd))
    {
        e->SetHandled(true);

        if (GetWindowState() != WindowState::wsMaximized)
        {
            SetWindowState(WindowState::wsMaximized);
        }
        else if (GetWindowState() == WindowState::wsMaximized)
        {
            SetWindowState(WindowState::wsNormal);
        }

        return;
    }

    if (!GetUIParent())
    {
        Rect rcWnd(GetRenderRect());
        rcWnd.Deflate(GetSizeFrame());

        if (rcWnd.PointIn(e->GetMousePoint()) && StartDragMove())
        {
            e->SetHandled(true);
        }
    }
}

void Window::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    ContentControl::OnMouseLeftButtonUp(e);

    if (IsMouseCaptured())
    {
        EndDragMove();
        e->SetHandled(true);
    }
}

void Window::UpdateMeasureSize(const Size& availableSize, Size& measureSize)
{
    if (GetUIParent() == NULL)
    {
        UpdateMinMaxInfo(availableSize.cx == Numeric::MeasureInt ? -1 : availableSize.cx, 
            availableSize.cy == Numeric::MeasureInt ? -1 : availableSize.cy);
    }
    ContentControl::UpdateMeasureSize(availableSize, measureSize);
}

void Window::OnArrange(const Size& arrangeBounds)
{
    if (GetUIParent() == NULL && IsValid())
    {
        Element* pChild = GetVisualChild(0);
        if (pChild != NULL)
        {
            Rect finalRect(arrangeBounds);
            finalRect.right = max(min(_mmInfo.maxWidth, arrangeBounds.cx), _mmInfo.minWidth);
            finalRect.bottom = max(min(_mmInfo.maxHeight, arrangeBounds.cy), _mmInfo.minHeight);

            pChild->Arrange(finalRect);
        }
    }
    else
    {
        ContentControl::OnArrange(arrangeBounds);
    }
}

Size Window::OnMeasure(const Size& availableSize)
{
    Size constraint = availableSize;
    if (GetUIParent() == NULL && IsValid())
    {
        Element* pChild = GetVisualChild(0);
        if (pChild != NULL)
        {
            Size size = availableSize;
            GetMeasureData()->SetViewPort(Rect(Point(), size));
            MeasureChild(pChild, size);
            constraint = pChild->GetDesiredSize();
        }
    }
    else
    {
        constraint = ContentControl::OnMeasure(availableSize);
    }

    return constraint;
}

void Window::OnLoaded(LoadedEventArg* e)
{
    ContentControl::OnLoaded(e);

    if (!GetUIParent())
    {
        VisualHostPtr pHost(e->GetTarget());
        HWND hwnd = HANDLETOHWND(pHost->GetHandle());

        if (::IsWindow(hwnd))
        {
            SetWindowText(hwnd, GetTitle().c_str());

            DWORD dwExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);

            if (AllowsTransparency())
            {
                ::SetWindowLong(hwnd, GWL_EXSTYLE, dwExStyle | 0x80000);
            }
            else
            {
                ::SetWindowLong(hwnd, GWL_EXSTYLE, dwExStyle & (~0x80000));
            }

            ::UpdateWindow(hwnd);
        }

        //
        // 全屏显示
        //
        if (AllowsFullScreen())
        {
            SetAllowsFullScreen(true);
        }
    }
}

void Window::OnRenderSizeModeChanged(SizeModeChangedEventArg* e)
{
    ContentControl::OnRenderSizeModeChanged(e);
}

ResizeGrip::ResizeGrip()
{

}

ResizeGrip::~ResizeGrip()
{

}

Size ResizeGrip::OnMeasure(const Size& availableSize)
{
    Size size;
    return size;
}

void ResizeGrip::OnHitTest(HitResultEventArg* e)
{
    if (GetHitTestFilterAction() == HitTestFilterAction::Continue)
    {
        e->GetHitResult().HitTestCode(HTBOTTOMRIGHT);
        e->SetHandled(true);
    }
}

void ResizeGrip::OnMouseMove(MouseButtonEventArg* e)
{    
    e->SetHandled(true);
}

void ResizeGrip::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    HwndHelper::HandleLastMessage();
    e->SetHandled(true);
}

void ResizeGrip::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

void Window::OnHitTest(HitResultEventArg* e)
{
    HWND hwnd = HwndFromRootWindow(this);

    if (AllowsFullScreen())
    {
        e->SetHandled(true);
        e->GetHitResult().HitTestCode(CoreFlags::HitClient);
        return;
    }
    
    if (::IsWindow(hwnd) && 
        Mouse::GetCaptured() == NULL && 
        GetHitTestFilterAction() == HitTestFilterAction::Continue)
    {
        HitResult& ret = e->GetHitResult();
        Point pt = ret.HitTestPoint();

        ret.HitTestCode(CoreFlags::HitValid);

        if (GetResizeMode() != ResizeMode::NoResize)
        {
            Rect rcWnd(GetRenderRect());

            int nXF = GetSizeFrame().Width();
            int nYF = GetSizeFrame().Height();

            Rect rcT(0, 0, rcWnd.Width(), nYF);
            Rect rcL(0, nYF, nXF, rcWnd.Height() - nYF * 2);
            Rect rcR(rcWnd.Width() - nXF, nYF, nXF, rcWnd.Height() - nYF * 2);
            Rect rcB(0, rcWnd.Height() - nYF, rcWnd.Width(), nYF);

            if (rcT.PointIn(pt))
            {
                if (pt.x <= nXF)
                {
                    ret.HitTestCode(HTTOPLEFT);
                }
                else if (pt.x >= (rcT.right - nXF))
                {
                    ret.HitTestCode(HTTOPRIGHT);
                }
                else
                {
                    ret.HitTestCode(HTTOP);
                }
            }
            else if (rcL.PointIn(pt))
            {
                if (pt.y <= rcL.top - nYF)
                {
                    ret.HitTestCode(HTTOPLEFT);
                }
                else if (pt.y >= (rcL.bottom - nYF))
                {
                    ret.HitTestCode(HTBOTTOMLEFT);
                }
                else
                {
                    ret.HitTestCode(HTLEFT);
                }
            }
            else if (rcR.PointIn(pt))
            {
                if (pt.y <= rcR.top - nYF)
                {
                    ret.HitTestCode(HTTOPLEFT);
                }
                else if (pt.y >= (rcR.bottom - nYF))
                {
                    ret.HitTestCode(HTBOTTOMRIGHT);
                }
                else
                {
                    ret.HitTestCode(HTRIGHT);
                }
            }
            else if (rcB.PointIn(pt))
            {
                if (pt.x <= nXF)
                {
                    ret.HitTestCode(HTBOTTOMLEFT);
                }
                else if (pt.x >= (rcB.right - nXF))
                {
                    ret.HitTestCode(HTBOTTOMRIGHT);
                }
                else
                {
                    ret.HitTestCode(HTBOTTOM);
                }
            }
        }

        e->SetHandled(ret.HitTestCode() != CoreFlags::HitValid);
    }
}

void Window::OnRenderSizeChanged(SizeChangedInfo& e)
{
}

void Window::OnKeyDown(KeyboardEventArg* e)
{
    //
    // Esc键压下，关闭窗口
    //
    if (e->GetKey() == Key::kEscape && !GetUIParent())
    {
        Close();
        e->SetHandled(true);
    }
}

bool Window::CreateWindowDuringShow(suic::Handle hParent)
{
    HWND hwParent = HANDLETOHWND(hParent);
    bool isChild = ::IsWindow(hwParent) ? true : false;

    if (GetUIParent())
    {
        return true;
    }
    else
    {
        HwndParam hp;
        WindowStyle uStyle = GetWindowStyle();
        WindowState uState = GetWindowState();
        int oriWid = GetWidth();
        int oriHei = GetHeight();

        hp.x = SystemParameters::TransformXToDevice(_location.x);
        hp.y = SystemParameters::TransformYToDevice(_location.y);
        hp.width = CoercedWidth(-1);
        hp.height = CoercedHeight(-1);
        hp.parent = hParent;

        if (!isChild)
        {
            //CreateWindowStyle(hp.style, hp.exstyle);

            if (oriWid <= 0 && oriHei <= 0)
            {
                SetSizeToContent(SizeToContent::stcWidthAndHeight);
            }
            else if (GetSizeToContent() != SizeToContent::stcWidthAndHeight)
            {
                if (oriWid <= 0)
                {
                    SetSizeToContent(SizeToContent::stcWidth);
                }
                else if (oriHei <= 0)
                {
                    SetSizeToContent(SizeToContent::stcHeight);
                }
            }

            if (uStyle != WindowStyle::wsNone)
            {
                hp.state |= WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
            }
            else
            {
                hp.style &= ~WS_MAXIMIZEBOX;
            }

            if (uStyle == WindowStyle::wsToolWindow || IsTopMost())
            {
                hp.exstyle |= WS_EX_TOPMOST;
                if (uStyle == WindowStyle::wsToolWindow)
                {
                    hp.exstyle |= WS_EX_TOOLWINDOW;
                }
            }

            if (uState == WindowState::wsMaximized)
            {
                hp.state |= WS_MAXIMIZE;
            }
            else if (uState == WindowState::wsMinimized)
            {
                hp.state |= WS_MINIMIZE;
            }

            if (GetResizeMode() != ResizeMode::NoResize)
            {
                hp.state |= WS_SIZEBOX;
            }

            if (IsShowInTaskBar())
            {
                hp.exstyle &= ~WS_EX_TOOLWINDOW;
                hp.exstyle |= WS_EX_APPWINDOW;
            }
            else
            {
                hp.exstyle |= WS_EX_TOOLWINDOW;
                hp.exstyle &= ~WS_EX_APPWINDOW;
            }

            if (AllowsTransparency())
            {
                hp.exstyle |= WS_EX_LAYERED;
            }
        }
        else
        {
            hp.style = WS_CHILDWINDOW | WS_TABSTOP | WS_VISIBLE;
            hp.exstyle = WS_EX_NOPARENTNOTIFY;
        }

        if (IsAllowDrop())
        {
            hp.exstyle |= WS_EX_ACCEPTFILES;
        }

        SysRenderSource* obj = new SysRenderSource(hp);

        obj->ref();

        if (!obj->Initialize(this))
        {
            obj->unref();
            return false;
        }
        else
        {
            obj->unref();

            UpdateIcon();

            //
            // 去掉窗口边框
            //
            HWND selfhwnd = __HwndFromElement(this);
            DWORD dwStyle = GetWindowLongPtr(selfhwnd, GWL_EXSTYLE);
            HWND ownHwnd = HANDLETOHWND(GetOwnerHandle());

            if (NULL != ownHwnd && NULL != GetOwner())
            {
                ownHwnd = __HwndFromElement(GetOwner());
            }

            dwStyle &= ~WS_EX_WINDOWEDGE;
            ::SetWindowLongPtr(selfhwnd, GWL_EXSTYLE, dwStyle);
            dwStyle = GetWindowLongPtr(selfhwnd, GWL_EXSTYLE);

            if (IsAllowDrop())
            {
                DragDrop::RegisterDropTarget(selfhwnd);
            }

            if (ownHwnd != NULL && !isChild)
            {
                SetWindowLong(selfhwnd,  GWLP_HWNDPARENT, HWNDTOHANDLE(ownHwnd)); 
            }

            if (!isChild)
            {
                // 设置窗口标题
                SetWindowText(selfhwnd, GetTitle().c_str());
                CoreSystem::SetWndPos(selfhwnd,(IsTopMost() ? HWND_TOPMOST : NULL)
                    , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
            }

            InvalidateVisual();

            RegisterWindowFilter(this, MessageHook(this, &Window::OnFilterMessage));
            return true;
        }
    }
}

bool Window::ContainFlag(Uint32 key)
{
    if ((_windowFlag & key) == key)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Window::SetWndFlag(Uint32 key, bool bAdd)
{
    if (bAdd)
    {
        _windowFlag |= key;
    }
    else
    {
        _windowFlag &= ~key;
    }
}

//----------------------------------------------------------

Handle Window::GetOwnerHandle()
{
    return _ownerHandle;
}

Size Window::GetSizeFrame()
{
    return _sizeFrame;
}

void Window::SetSizeFrame(Size size)
{
    _sizeFrame = size;
}

ImageSource* Window::GetIcon()
{
    return DynamicCast<ImageSource>(GetValue(IconProperty));
}

void Window::SetIcon(ImageSource* val)
{
    SetValue(IconProperty, val);
}

String Window::GetTitle()
{
    return GetValue(TitleProperty)->ToString();
}

void Window::SetTitle(String val)
{
    SetValue(TitleProperty, new OString(val));
}

int Window::GetCaptionHeight()
{
    return GetValue(CaptionHeightProperty)->ToInt();
}

void Window::SetCaptionHeight(int val)
{
    SetValue(CaptionHeightProperty, new Integer(val));
}

bool Window::IsClosing()
{
    return ContainFlag(wfIsClosing);
}

bool Window::IsClosed()
{
    HWND selfhwnd = __HwndFromElement(this);

    if (!::IsWindow(selfhwnd))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Window::IsValid()
{
    HWND selfhwnd = __HwndFromElement(this);

    if (::IsWindow(selfhwnd))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Window::SetPos(int x, int y)
{
    SetLocation(Point(x, y));
}

void Window::SetLocation(Point pt)
{
    SetValue(LocationProperty, new OPoint(pt));
}

void Window::SetCloseTime(int elapse)
{

}

Point Window::GetLocation()
{
    HWND selfhwnd = __HwndFromElement(this);
    if (::IsWindow(selfhwnd))
    {
        Rect rect;
        Point location;
        ::GetWindowRect(selfhwnd, &rect);
        location.x = rect.left;
        location.y = rect.top;
        if (_location != location)
        {
            _location = location;
        }
    }
    return _location;
}

WindowState Window::GetWindowState()
{
    return (WindowState)GetValue(WindowStateProperty)->ToInt();
}

void Window::SetWindowState(WindowState val)
{
    SetValue(WindowStateProperty, WindowStateBox::From(val));
}

WindowStyle Window::GetWindowStyle()
{
    return (WindowStyle)GetValue(WindowStyleProperty)->ToInt();
}

void Window::SetWindowStyle(WindowStyle val)
{
    SetValue(WindowStyleProperty, new Integer((int)val));
}

void Window::SetOwnerHandle(Handle ownerHandle)
{
    if (_ownerHandle == ownerHandle && !_ownerHandle)
    {
        return; 
    }

    _ownerHandle = ownerHandle;

    HWND hwnd = __HwndFromElement(this);

    if (hwnd && ::IsWindow(hwnd))
    {
        SetWindowLong(hwnd,  GWLP_HWNDPARENT, ownerHandle); 
    }
}

bool Window::AllowsTransparency()
{
    return GetValue(AllowsTransparencyProperty)->ToBool();
}

void Window::SetAllowsTransparency(bool val)
{
    SetValue(AllowsTransparencyProperty, BOOLTOBOOLEAN(val));
}

int Window::GetSizeToContent()
{
    return _sizeToContent;//GetValue(SizeToContentProperty)->ToInt();
}

void Window::SetSizeToContent(int val)
{
    if (val >= SizeToContent::stcManual && val <= SizeToContent::stcWidthAndHeight)
    {
        SetValue(SizeToContentProperty, Integer::GetPosInt(val));
    }
}

Size Window::UpdateSizeOnContent()
{
    Size size;
    /*if (!_isPending && GetSizeToContent())
    {
        _isPending = true;
        Element* child = GetVisualChild(0);
        if (NULL != child)
        {
            Size measureSize(Numeric::MeasureMaxInt, Numeric::MeasureMaxInt);
            MeasureChild(child, measureSize);
            size = child->GetDesiredSize();
            UpdateMinMaxInfo(size.Width(), size.Height());
            SetWidth(CoercedWidth(size.Width()));
            SetHeight(CoercedWidth(size.Height()));
        }
        _isPending = false;
    }*/
    return size;
}

void Window::SetPopupCapture()
{
    HWND selfhwnd = __HwndFromElement(this);

    if (::IsWindow(selfhwnd) && GetCapture() != selfhwnd)
    {
        ::SetCapture(selfhwnd);
    }
}

FrameworkElement* Window::GetOwner() 
{ 
    return _owner.GetElement<FrameworkElement>(); 
}

void Window::SetOwner(FrameworkElement* owner) 
{
    if (this != owner)
    {
        _owner = owner; 
    }
}

void Window::SetOwnerParent()
{
    if (GetOwner())
    {
        __SetParentByLong(GetOwner(), this); 
    }
}

void Window::UpdateWindowSize()
{
    /*HWND self = HwndFromRootWindow(this);
    if (::IsWindow(self))
    {
        Rect rect;
        int realWid = CoercedWidth(-1);
        int realHei = CoercedHeight(-1);
        ::GetWindowRect(self, &rect);

        if (rect.Width() != realWid || rect.Height() != realHei)
        {
            InvalidateVisual();
            SetWindowPos(self, NULL, 0, 0, realWid, realHei, 
                SWP_NOZORDER | SWP_NOMOVE | SWP_FRAMECHANGED);
        }
    }*/
}

void Window::UpdateIcon()
{
    HWND hwnd = __HwndFromElement(this);
    ImageSource* hIcon = GetIcon();

    if (NULL != hIcon)
    {
        Bitmap* bmp = hIcon->GetBitmap();

        if (NULL != _smallIcon)
        {
            ::DestroyIcon((HICON)(DWORD_PTR)_smallIcon);
        }
        if (NULL != _bigIcon)
        {
            ::DestroyIcon((HICON)(DWORD_PTR)_bigIcon);
        }
        _smallIcon = (Handle)IconHelper::CreateHIconFromBitmap(bmp, true);
        _bigIcon = (Handle)IconHelper::CreateHIconFromBitmap(bmp, false);
    }

    if (::IsWindow(hwnd))
    {
        ::SendMessage(hwnd, WM_SETICON, 0, (LPARAM)_smallIcon);
        ::SendMessage(hwnd, WM_SETICON, 1, (LPARAM)_bigIcon);
    }
}

void Window::Resize(int cx, int cy)
{
    HWND hwnd = __HwndFromElement(this);
    if (::IsWindow(hwnd))
    {
        CoreSystem::SetWndPos(hwnd, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
    }
}

void Window::SetMove(int x, int y, int cx, int cy)
{
    HWND hwnd = __HwndFromElement(this);
    if (::IsWindow(hwnd))
    {
        CoreSystem::SetWndPos(hwnd, NULL, x, y, cx, cy, SWP_NOZORDER);
    }
    SetPos(x, y);
}

Rect Window::GetWndRect()
{
    Rect rect;
    HWND hwnd = __HwndFromElement(this);

    if (::IsWindow(hwnd))
    {
        ::GetWindowRect(hwnd, &rect);
    }

    return rect;
}

void Window::Close()
{
    WindowInvoker::CloseWindow(this, false);
}

void Window::AsyncClose()
{
    WindowInvoker::CloseWindow(this, true);
}

bool Window::IsTopMost()
{
    return GetValue(TopmostProperty)->ToBool();
}

void Window::SetTopMost(bool topmost)
{
    SetValue(TopmostProperty, BOOLTOBOOLEAN(topmost));
}

void Window::CenterWindow(Handle hActive)
{
    if (!GetUIParent())
    {
        Rect rcSelf;
        Rect rcOwner;

        HWND self = __HwndFromElement(this);
        HWND hwnd = ::GetDesktopWindow();
        HWND hwndOwner = HANDLETOHWND(hActive);
        MonitorInfo mi = Environment::GetMonitorBoundByPoint(NULL);

        rcOwner = mi.rcWork;
        ::GetWindowRect(self, &rcSelf);

        if (hwndOwner == NULL)
        {
            //hwndOwner = HANDLETOHWND(HwndHelper::GetActiveHandle());
            hwndOwner = (HWND)(DWORD_PTR)GetWindowLong(self, GWLP_HWNDPARENT);
            if (NULL == hwndOwner)
            {
                if (NULL != GetOwner())
                {
                    hwndOwner = __HwndFromElement(GetOwner());
                }

                if (NULL == hwndOwner)
                {
                    hwndOwner = HANDLETOHWND(_ownerHandle);
                }
            }
        }

        if (NULL == hwndOwner)
        {
            hwndOwner = HANDLETOHWND(HwndSubclass::GetTopActiveHwnd());
        }

        if (::IsWindow(hwndOwner) && ::IsWindowVisible(hwndOwner) && hwndOwner != self)
        {
            //mi = Environment::GetMonitorBoundByWindow(hwndOwner);
            //rcOwner = mi.rcWork;
            ::GetWindowRect(hwndOwner, rcOwner);
        }

        int x = rcOwner.left + (Float) (rcOwner.Width() - rcSelf.Width()) / 2.0f;
        int y = rcOwner.top + (Float) (rcOwner.Height() - rcSelf.Height()) / 2.0f;

        int flags = SWP_NOZORDER | SWP_NOSIZE; 

        ::SetWindowPos(self, 0, x, y, 0, 0, flags);

        Activate();
    }
}

void Window::InternalClose(bool async)
{
    if (!ContainFlag(wfIsClosing))
    {
        HWND hwnd = __HwndFromElement(this);

        if (async)
        {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
        }
        else
        {
            SendMessage(hwnd, WM_CLOSE, 0, 0);
        }
    }
}

AssignerFrame* Window::GetAssignerFrame()
{
    return _frame;
}

}
