
#include <System/Tools/VisualTreeOp.h>
#include <System/Windows/CoreTool.h>

#include <System/Input/Keyboard.h>
#include <System/Input/Mouse.h>
#include <System/Input/MouseDevice.h>

#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>

#include <System/Interop/InternalWindowOper.h>
#include <System/Interop/InternalOper.h>
#include <System/Render/RenderManager.h>

namespace suic
{

bool VisualTreeOp::IsInVisualTree(Element* child, const Element* root)
{
    Element* tmp = child;
    while (tmp != NULL)
    {
        if (tmp == root)
        {
            return true;
        }
        tmp = tmp->GetUIParent();
    }
    return false;
}

Rect GetRenderBound(Element* pElem)
{
    Point pt(pElem->PointToScreen(Point()));
    return Rect(pt.x, pt.y, pElem->GetActualWidth(), pElem->GetActualHeight());
}

void InvalidateWindow(Element*)
{
}

static bool __IsHoldVisualHost(FrameworkElement* rootElement, VisualHost** visualHost)
{
    Popup* popup = RTTICast<Popup>(rootElement);
    *visualHost = VisualHost::GetVisualHost(popup);
    return ((*visualHost) != NULL || NULL != popup);
}

Element* VisualTreeOp::GetVisualRoot(DpObject* d)
{
    Element* pParent = RTTICast<Element>(d);
    Element* pPrev = NULL;

    while (pParent)
    {
        if (pParent->BlockVisualState())
        {
            break;
        }
        pPrev = pParent;
        pParent = pParent->GetUIParent();
        if (NULL == pParent && CASTTOFE(pPrev))
        {
            pParent = CASTTOFE(pPrev)->GetParent();
        }
    }

    return pPrev;
}

bool VisualTreeOp::IsAllowsTransparency(DpObject* elem)
{
    HWND hwnd = __HwndFromElement(RTTICast<Element>(elem));
    DWORD exStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
    bool bLayered = (exStyle & WS_EX_LAYERED) != 0;
    return bLayered;
}

Element* VisualTreeOp::GetParent(DpObject* child)
{
    DpObject* pParent = NULL;
    Element* elem = RTTICast<Element>(child);

    if (NULL != elem)
    {
        elem = elem->GetUIParent();
    }
    else
    {
        pParent = child;
        while (pParent != NULL)
        {
            pParent = pParent->GetInheritParent();
            elem = RTTICast<Element>(pParent);

            if (NULL != elem)
            {
                break;
            }
        }
    }

    return elem;
}

bool VisualTreeOp::IsAncestorOf(Element* ancestor, Element* child)
{
    if (ancestor == NULL)
    {
        return false;
    }
    else
    {
        return ancestor->IsAncestorOf(child);
    }
}

Element* VisualTreeOp::FindName(Element* root, const String& name)
{
    if (name.Equals(root->GetName()))
    {
        return root;
    }

    int iCount = root->GetVisualChildrenCount();
    for (int i = 0; i < iCount; ++i)
    {
        Element* elem(root->GetVisualChild(i));
        if (elem)
        {
            Element* ret(VisualTreeOp::FindName(elem, name));
            if (ret)
            {
                return ret;
            }
        }
    }
    return NULL;
}

//=========================================================

Point PointTransform::GetCursorPointOfRoot(Element* elem)
{
    HWND owner = __HwndFromElement(elem);
    Rect rect;
    Point pt;
    ::GetWindowRect(owner, &rect);
    ::GetCursorPos(&pt);
    pt.x -= rect.left;
    pt.y -= rect.top;
    return pt;
}

bool PointTransform::PointInWindow(Element* elem, Point* point)
{
    Point pt;
    Rect rect;
    if (NULL == point)
    {
        GetCursorPos(&pt);
    }
    else
    {
        pt = *point;
    }

    HWND hwnd = __HwndFromElement(elem);
    if (hwnd)
    {
        GetWindowRect(hwnd, &rect);
        return rect.PointIn(pt);
    }
    else
    {
        return false;
    }
}

bool PointTransform::PointInElement(Element* elem, Point* point)
{
    Point pt;
    Rect rect;
    Rect rcWnd;
    HWND hwnd = __HwndFromElement(elem);

    GetCursorPos(&pt);

    if (hwnd != WindowFromPoint(pt))
    {
        return false;
    }

    if (NULL == point)
    {
        GetWindowRect(hwnd, &rcWnd);
        pt.x -= rcWnd.left;
        pt.y -= rcWnd.top;
    }
    else
    {
        pt = *point;
    }

    rect = GetRenderBound(elem);

    return rect.PointIn(pt);
}

Point PointTransform::GetRootScreenPos(Element* elem)
{
    HWND owner = __HwndFromElement(elem);
    Rect rect;
    Point pt;

    ::GetWindowRect(owner, &rect);

    pt.x = rect.left;
    pt.y = rect.top;

    return pt;
}

Point PointTransform::GetElementScreenPos(Element* elem)
{
    HWND owner = __HwndFromElement(elem);
    Rect rect;
    Point pt = elem->PointToScreen(Point());

    ::GetWindowRect(owner, &rect);

    pt.x += rect.left;
    pt.y += rect.top;

    return pt;
}

}
