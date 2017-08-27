/////////////////////////////////////////////////////////////////////////
// uimousedevice.cpp

#include <System/Input/MouseDevice.h>
#include <System/Types/Const.h>
#include <System/Types/Micro.h>
#include <System/Input/Mouse.h>
#include <System/Tools/VisualTreeOp.h>

/*--------内部使用头文件--------*/
#include <System/Interop/InternalOper.h>
#include <System/Interop/InternalWindowOper.h>

namespace suic
{

MouseDevice::MouseDevice()
{
    setAutoDelete(false);

    _visualHost = NULL;
    _clickCount = 0;
    _lastClickTime = 0;
    _lastButton = MouseButton::mbUnknown;
    _doubleClickTime = ::GetDoubleClickTime();
}

static WeakRef _rawMouseOver;
static WeakRef _mouseOver;
static WeakRef _mouseCapture;

static WeakRef _rawLMouseDown;
static WeakRef _rawRMouseDown;
static WeakRef _rawMMouseDown;

static WeakRef _lMouseDown;
static WeakRef _rMouseDown;
static WeakRef _mMouseDown;

void MouseDevice::Clear()
{
    _rawMouseOver = NULL;
    _mouseOver = NULL;
    _mouseCapture = NULL;
    _rawLMouseDown = NULL;
    _rawRMouseDown = NULL;
    _rawMMouseDown = NULL;
}

MouseDevice* MouseDevice::Current()
{
    static MouseDevice _ins;
    return &_ins;
}

VisualHost* MouseDevice::GetVisualHost()
{
    return _visualHost;
}

void MouseDevice::SetVisualHost(VisualHost* pHost)
{
    _visualHost = pHost;
}

Element* MouseDevice::GetRawMouseOver()
{
    return dynamic_cast<Element*>(_rawMouseOver.GetTarget());
}
void MouseDevice::SetRawMouseOver(Element* val)
{
    _rawMouseOver = val;
}

Element* MouseDevice::GetMouseOver()
{
    return dynamic_cast<Element*>(_mouseOver.GetTarget());
}
void MouseDevice::SetMouseOver(Element* val)
{
    if (val != _mouseOver)
    {
        _mouseOver = val;
    }
}

void MouseDevice::ClearMouseOver(bool bCheck)
{
    if (bCheck)
    {
        Mouse::ChangeMouseOver(NULL);
    }
    else
    {
        _mouseOver = NULL;
    }
}

Element* MouseDevice::GetRawMouseDown(Int32 mouseType)
{
    switch (mouseType)
    {
    case MouseButton::mbLeftButton:
        return _rawLMouseDown.GetElement<Element>();

    case MouseButton::mbRightButton:
        return _rawRMouseDown.GetElement<Element>();

    case MouseButton::mbMiddleButton:
        return _rawMMouseDown.GetElement<Element>();

    default:
        return NULL;
    }
}

void MouseDevice::SetRawMouseDown(Element* val, Int32 mouseType)
{
    switch (mouseType)
    {
    case MouseButton::mbLeftButton:
        _rawLMouseDown = val;

    case MouseButton::mbRightButton:
        _rawRMouseDown = val;

    case MouseButton::mbMiddleButton:
        _rawMMouseDown = val;

    default:
        ;
    }
}

Element* MouseDevice::GetMouseDown(Int32 mouseType)
{
    switch (mouseType)
    {
    case MouseButton::mbLeftButton:
        return _lMouseDown.GetElement<Element>();

    case MouseButton::mbRightButton:
        return _rMouseDown.GetElement<Element>();

    case MouseButton::mbMiddleButton:
        return _mMouseDown.GetElement<Element>();

    default:
        return NULL;
    }
}

void MouseDevice::SetMouseDown(Element* val, Int32 mouseType)
{
    switch (mouseType)
    {
    case MouseButton::mbLeftButton:
        _lMouseDown = val;

    case MouseButton::mbRightButton:
        _rMouseDown = val;

    case MouseButton::mbMiddleButton:
        _mMouseDown = val;

    default:
        ;
    }
}

Element* MouseDevice::GetMouseCapture()
{
    if (_mouseCapture.GetTarget())
    {
        /*HWND hwnd = __GetHwndFromElement(_mouseCapture);
        if (GetCapture() != hwnd)
        {
            SetMouseCapture(NULL);
        }*/
    }

    return dynamic_cast<Element*>(_mouseCapture.GetTarget());
}
void MouseDevice::SetMouseCapture(Element* val)
{
    _mouseCapture = val;
}

Point MouseDevice::GetPrevClick()
{
    return _prevClick;
}
void MouseDevice::SetPrevClick(Point val)
{
    _prevClick = val;
}

Point MouseDevice::GetLastClick()
{
    return _lastClick;
}
void MouseDevice::SetLastClick(Point val)
{
    _lastClick = val;
}

Uint32 MouseDevice::GetClickCount()
{
    return _clickCount;
}
void MouseDevice::SetClickCount(Uint32 val)
{
    _clickCount = val;
}

Uint32 MouseDevice::GetLastClickTime()
{
    return _lastClickTime;
}
void MouseDevice::SetLastClickTime(Uint32 val)
{
    _lastClickTime = val;
}

Int32 MouseDevice::GetLastButton()
{
    return _lastButton;
}
void MouseDevice::SetLastButton(Int32 val)
{
    _lastButton = val;
}

Int32 MouseDevice::GetDoubleDeltaTime()
{
    return _doubleClickTime;
}

MouseButtonState MouseDevice::GetButtonState(int mouseButton)
{
    int iVirtKey = mouseButton;

    /*switch (mouseButton)
    {
    case MouseButton::mbLeftButton:
        iVirtKey |= VK_LBUTTON;
        break;

    case MouseButton::mbRightButton:
        iVirtKey |= VK_RBUTTON;
        break;

    case MouseButton::mbMiddleButton:
        iVirtKey |= VK_MBUTTON;
        break;

    default:
        return MouseButtonState::mbMouseRelease;
    }*/

    if (::GetKeyState(iVirtKey) < 0)
    {
        return MouseButtonState::mbMousePress;
    }
    else
    {
        return MouseButtonState::mbMouseRelease;
    }
}

MouseButtonState MouseDevice::GetLeftButton()
{
    return GetButtonState(MouseButton::mbLeftButton);
}

MouseButtonState MouseDevice::GetRightButton()
{
    return GetButtonState(MouseButton::mbRightButton);
}

MouseButtonState MouseDevice::GetMiddleButton()
{
    return GetButtonState(MouseButton::mbMiddleButton);
}

MouseButtonState MouseDevice::GetXButton1()
{
    return GetButtonState(MouseButton::mbXBUTTON1);
}

MouseButtonState MouseDevice::GetXButton2()
{
    return GetButtonState(MouseButton::mbXBUTTON2);
}

}
