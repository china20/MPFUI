/////////////////////////////////////////////////////////////////////////
// uimouse.cpp

#include <System/Input/Mouse.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Windows/CoreTool.h>

#include <System/Interop/System.h>
#include <System/Interop/InternalOper.h>

#include <System/Windows/HwndSubclass.h>
#include <System/Windows/InheritStateProperty.h>

namespace suic
{

Mouse::Mouse()
{
}

Mouse::~Mouse()
{
}

//----------------------------------------------------------------
//

Point Mouse::GetPosition(Element* element)
{
    return Point();
}

void Mouse::ClippingCursor(const Rect* rect)
{
    ::ClipCursor(rect);
}

Color Mouse::ColorFromPoint(const Point* point)
{
    Point pt;
    if (point == NULL)
    {
        ::GetCursorPos(&pt);
    }
    else
    {
        pt = *point;
    }

    HDC hdc = ::GetWindowDC(NULL);
    COLORREF clr = ::GetPixel(hdc, pt.x, pt.y);
    ::ReleaseDC(NULL, hdc);

    return Color::FromRgb(clr);
}

MouseButton Mouse::GetMouseButton()
{
    if (MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress)
    {
        return MouseButton::mbLeftButton;
    }
    else if (MouseDevice::GetRightButton() == MouseButtonState::mbMousePress)
    {
        return MouseButton::mbRightButton;
    }
    else if (MouseDevice::GetMiddleButton() == MouseButtonState::mbMousePress)
    {
        return MouseButton::mbMiddleButton;
    }
    else
    {
        return MouseButton::mbUnknown;
    }
}

Element* Mouse::SetCaptured(Element* element)
{
    Element* oldElem = MouseDevice::GetMouseCapture();

    if (oldElem != element)
    {
        MouseDevice::SetMouseCapture(element);

        if (element)
        {
            VisualHostPtr obj = HwndSubclass::MatchRenderSource(element);

            if (obj)
            {
                obj->SetMouseCapture();
            }
        }
        else if (oldElem)
        {
            VisualHostPtr obj = HwndSubclass::MatchRenderSource(oldElem);

            if (obj)
            {
                obj->ReleaseMouseCapture();
            }
        }

        Point point;
        MouseCaptureProperty mouseCaptureProp;
        mouseCaptureProp.OnOriginalValueChanged(oldElem, element);

        if (oldElem)
        {
            Point point = __GetCusorPoint(oldElem);
            MouseButtonEventArg e(oldElem, point);
            FrameworkElementPtr(oldElem)->UpdateVisualState(true);
            e.SetRoutedEvent(Element::LostMouseCaptureEvent);
            oldElem->RaiseEvent(&e);
        }

        if (element)
        {
            Point point = __GetCusorPoint(element);
            MouseButtonEventArg e(element, point);
            FrameworkElementPtr(element)->UpdateVisualState(true);
            e.SetRoutedEvent(Element::GotMouseCaptureEvent);
            element->RaiseEvent(&e);
        }
    }

    return oldElem;
}

Element* Mouse::GetCaptured()
{
    Element* cap = MouseDevice::GetMouseCapture();
    return cap;
}

Element* Mouse::GetDirectlyOver()
{
    if (!MouseDevice::GetRawMouseOver())
    {
        return MouseDevice::GetMouseOver();
    }
    else
    {
        return MouseDevice::GetRawMouseOver();
    }
}

Element* Mouse::HitTestElement(Element* elem, Point point)
{
    Element* rootElement = VisualTreeOp::GetVisualRoot(elem);
    HitTestCtx hitRes(point);
    rootElement->HitTest(&hitRes);
    return hitRes.hitRes.get();
}

void Mouse::ChangeMouseOver(Element* newElem)
{
    Element* oldElem = MouseDevice::GetMouseOver();
    MouseDevice::SetMouseOver(newElem);

    MouseOverProperty mouseOverProp;
    mouseOverProp.OnOriginalValueChanged(oldElem, newElem);
}

bool Mouse::IsMouseCaptureWithin(Element* element)
{
    return element->ReadFlag(CoreFlags::IsMouseCapturedWithinCache);
}

void Mouse::UpdateCursor()
{
}

}
