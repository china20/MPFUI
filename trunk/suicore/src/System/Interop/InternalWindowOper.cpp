
#include <Framework/Controls/Window.h>
#include <System/Windows/Assigner.h>
#include <System/Windows/CoreTool.h>

#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/Thread.h>
#include <System/Tools/HwndHelper.h>

/*--------内部使用头文件--------*/
#include <System/Interop/InternalWindowOper.h>
#include <System/Interop/System.h>
#include <System/Render/RenderManager.h>

#include <System/Windows/HwndSubclass.h>

namespace suic
{

HWND __HwndFromElement(Element* elem)
{
    return HANDLETOHWND(HwndHelper::GetHostHwnd(elem));
}

void __SetParentByLongWithHwnd(HWND parent, HWND child)
{
    if (parent != child)
    {
        SetWindowLong(child, GWLP_HWNDPARENT, (LONG)(LONG_PTR)parent);
    }
}

void __SetParentByLong(Element* parent, Element* child)
{
    HWND hChild = __HwndFromElement(child);
    HWND hParent = __HwndFromElement(parent);
    __SetParentByLongWithHwnd(hParent, hChild);
}

Point __GetCanvasLeftTop(Element* element)
{
    Point point;
    Element* parent = element;

    while (parent)
    {
        point.x += parent->GetVisualOffset().x;
        point.y += parent->GetVisualOffset().y;

        parent = parent->GetUIParent();
    }

    return point;
}

Point __GetScreenPoint(Element* element)
{
    Point point;
    
    VisualHostPtr obj = HwndSubclass::MatchRenderSource(element);

    if (obj)
    {
        point = obj->GetScreenPoint();
    }
    else
    {
        HWND hwnd = __HwndFromElement(element);

        if (::IsWindow(hwnd))
        {
            Rect rect;

            GetWindowRect(hwnd, &rect);
            point.x = rect.left;
            point.y = rect.top;
        }

    }

    return point;
}

Point __ToScreenPoint(Element* element, Point pt)
{
    Point point = __GetScreenPoint(element);

    // point = SystemParameters::TransformFromDevice(point.x, point.y).ToPoint();

    point.x += pt.x;
    point.y += pt.y;

    return point;
}

Point __GetCusorPointFromHwnd(HWND h)
{
    Point pt;
    Rect rc;
    fSize size;

    ::GetCursorPos(&pt);
    ::GetWindowRect(h, &rc);
    pt.x -= rc.left;
    pt.y -= rc.top;

    // size = SystemParameters::TransformFromDevice(fSize(pt.x, pt.y));
    //pt.x = size.cx;
    //pt.y = size.cy;

    return pt;
}

Point __GetCusorPoint(Element* element)
{
    HWND owner = __HwndFromElement(element);
    Rect rect;
    Point pt;

    ::GetCursorPos(&pt);
    ::GetWindowRect(owner, &rect);

    pt.x -= rect.left;
    pt.y -= rect.top;

    return pt;
}

Point __CalcElementAlignPoint(Element* element, int flag)
{
    Point point = __GetCanvasLeftTop(element);

    switch (flag)
    {
    case ElementAlign::eaLeftTop:
        break;
    case ElementAlign::eaRightTop:
        point.x += element->GetRenderSize().cx;
        break;
    case ElementAlign::eaLeftBottom:
        point.y += element->GetRenderSize().cy;
        break;
    case ElementAlign::eaRightBottom:
        point.x += element->GetRenderSize().cx;
        point.y += element->GetRenderSize().cy;
        break;
    }

    return __ToScreenPoint(element, point);
}

}
