// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// MenuItemHelper.cpp

#include <Framework/Controls/MenuItemHelper.h>
#include <Framework/Controls/Menu.h>

#include <System/Windows/CoreTool.h>
#include <System/Windows/RenderTarget.h>

#include <System/Tools/HwndHelper.h>
#include <System/Input/MouseDevice.h>

namespace suic
{

TrackingMenuOp* TrackingMenuOp::Ins()
{
    static TrackingMenuOp ins;
    return &ins;
}

TrackingMenuOp::TrackingMenuOp()
    : _mouseCapture(false)
    , _popup(NULL)
    , _trackingMenu(NULL)
    , _trackingMenuItem(NULL)
    , Object(false)
{
}

void TrackingMenuOp::SetTrackingMenu(Popup* popup, Element* menu)
{
    _mouseCapture = false;
    _trackingMenu = menu;
    _popups.Clear();

    if (NULL != popup)
    {
        _popups.Add(popup);
    }
}

void TrackingMenuOp::TrackMenuPopup(Popup* popup, Element* item, Element* menu)
{
    CloseAllPopup(false);

    _popup = popup;
    _trackingMenu = menu;
    _trackingMenuItem = item;
    AddPopupMenuItem(popup);
    _popup->Closing += EventHandler(this, &TrackingMenuOp::OnRootPopupClosing);
    popup->TrackingPopup(MessageHook(this, &TrackingMenuOp::OnFilterMessage));
    
    _trackingMenu = NULL;
    _popup = NULL;
    _popups.Clear();
}

void TrackingMenuOp::TrackContextMenu(Popup* popup, Element* menu, int x, int y)
{
    CloseAllPopup(false);

    _popup = popup;
    _trackingMenu = menu;
    _trackingMenuItem = NULL;

    AddPopupMenuItem(popup);
    _popup->GetPositionInfo()->x = SystemParameters::TransformXToDevice(x);
    _popup->GetPositionInfo()->y = SystemParameters::TransformYToDevice(y);
    _popup->Closing += EventHandler(this, &TrackingMenuOp::OnRootPopupClosing);

    popup->TrackingPopup(MessageHook(this, &TrackingMenuOp::OnFilterMessage));

    _trackingMenu = NULL;
    _popup = NULL;

    _popups.Clear();
}

void TrackingMenuOp::TrackSubMenuPopup(Popup* popup, Element* menu)
{
    AddPopupMenuItem(popup);
    popup->SetIsOpen(true);
}

bool TrackingMenuOp::HandleMouseDown(Handle hwnd, bool& bOverTree)
{
    bool bOver = false;
    bool handled = false;
    Element* focusMenuItem = HitTestPopup(hwnd, bOver, bOverTree);

    if (!bOver)
    {
        CloseAllPopup(true);
    }
    else
    {
        handled = true;
    }

    return handled;
}

bool TrackingMenuOp::HandleMouseUp(Handle hwnd, bool& bOverTree)
{
    bool bOver = false;
    bool handled = true;
    Element* focusMenuItem = HitTestPopup(hwnd, bOver, bOverTree);

    if (!focusMenuItem || !bOver)
    {
        CloseAllPopup(true);
    }
    else
    {
        handled = false;
    }

    return handled;
}

MenuItem* TrackingMenuOp::GetMenuItem(Element* elem)
{
    MenuItem* pMenu = NULL;
    Element* parent(elem);

    while (parent)
    {
        pMenu = DynamicCast<MenuItem>(parent);
        if (pMenu)
        {
            break;
        }
        parent = parent->GetUIParent();
    }

    return pMenu;
}

bool TrackingMenuOp::HandleMouseMove(MessageParam* mp, bool& bOverTree)
{
    bool bOver = false;
    bool handled = false;

    Element* hotMenu = HitTestPopup(mp->hwnd, bOver, bOverTree);

    if (!bOver)
    {
        handled = true;
        HwndHelper::HandleDefMessage(mp);
    }

    return handled;
}

bool TrackingMenuOp::IsMouseOverOwer()
{
    // 当前顶层菜单窗口
    Element* ower(_trackingMenu);

    if (ower)
    {
        Rect rect(Point(), ower->GetRenderSize());
        Point point = GetCursorPoint(ower);

        point = ower->PointFromScreen(point);

        if (rect.PointIn(point))
        {
            return true;
        }
    }

    return false;
}

void TrackingMenuOp::OnRootPopupClosing(Object* source, EventArg* e)
{
    if (NULL != _popup)
    {
        _popup->Closing -= EventHandler(this, &TrackingMenuOp::OnRootPopupClosing);
    }

    int iEnd = _popups.GetCount() - 1;

    while (iEnd >= 1)
    {
        Popup* pop = DynamicCast<Popup>(_popups.GetItem(iEnd));

        if (pop)
        {
            pop->AsyncClose();
        }

        _popups.RemoveAt(iEnd);
        --iEnd;
    }
}

bool TrackingMenuOp::OnFilterMessage(Object* sender, MessageParam* mp, bool& interrupt)
{
    ElementPtr rootElement(sender);
    bool handled = false;
    HWND hwnd = HANDLETOHWND(mp->hwnd);

    switch (mp->message)
    {
    case WM_NCLBUTTONDOWN:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        if (HandleMouseDown(mp->hwnd, interrupt))
        {
        }
        break;
    case WM_NCRBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
        if (HandleMouseDown(mp->hwnd, interrupt))
        {
            handled = true;
            HwndHelper::HandleDefMessage(mp);
        }
        break;
    case WM_LBUTTONUP:
    //case WM_RBUTTONUP:
    //case WM_CONTEXTMENU:
        if (HandleMouseUp(mp->hwnd, interrupt))
        {
            handled = true;
            HwndHelper::HandleDefMessage(mp);
        }
        //mp.gonext = HandleMouseUp(mp->hwnd);
        break;
    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
        handled = HandleMouseMove(mp, interrupt);
        break;

    case WM_MOUSEACTIVATE:
        mp->result = MA_NOACTIVATE;
        handled = true;
        break;

    case WM_KEYDOWN:
        if ((int)mp->wp == Key::kEscape)
        {
            CloseTopPopup();
            interrupt = true;
            handled = true;
        }
        break;

    case WM_CLOSE:
        ClosePopup(rootElement.get());
        break;

    case WM_NCACTIVATE:
        {
            ;
        }
        break;
    }

    return handled;
}

void TrackingMenuOp::CloseTopPopup()
{
    int iCount = _popups.GetCount();
    if (iCount == 0)
    {
        return;
    }

    Popup* pop = DynamicCast<Popup>(_popups.GetItem(iCount - 1));
    if (pop)
    {
        pop->AsyncClose();
    }

    _popups.RemoveAt(iCount - 1);
}

void TrackingMenuOp::ClosePopup(Element* elem)
{
    int i = 0;
    int start = -1;

    for (i = _popups.GetCount() - 1; i >= 0; --i)
    {
        Popup* pop = DynamicCast<Popup>(_popups.GetItem(i));

        if (pop == elem)
        {
            start = i;
            break;
        }
    }

    if (start < 0)
    {
        return;
    }

    int iEnd = _popups.GetCount() - 1;

    while (iEnd >= start)
    {
        Popup* pop = DynamicCast<Popup>(_popups.GetItem(iEnd));

        if (pop)
        {
            pop->AsyncClose();
        }

        _popups.RemoveAt(iEnd);

        --iEnd;
    }
}

void TrackingMenuOp::HideAllPopup()
{
    for (int i = _popups.GetCount() -1; i >= 0; --i)
    {
        Popup* pop = DynamicCast<Popup>(_popups.GetItem(i));

        if (pop)
        {
            pop->Hide();
        }
    }
}

void TrackingMenuOp::CloseAllPopup(bool bAsync)
{
    for (int i = _popups.GetCount() -1; i >= 0; --i)
    {
        Popup* pop = DynamicCast<Popup>(_popups.GetItem(i));

        if (pop)
        {
            if (bAsync)
            {
                pop->AsyncClose();
            }
            else
            {
                pop->Close();
            }
        }
    }

    _popups.Clear();
}

Element* TrackingMenuOp::HitTestPopup(Handle hwnd, bool& bOver, bool& bOverTree)
{
    Popup* overPopup = NULL;

    bOver = false;
    bOverTree = false;

    // 找到当前鼠标所在的菜单窗口
    for (int i = 0; i < _popups.GetCount(); ++i)
    {
        Popup* pop = RTTICast<Popup>(_popups.GetItem(i));
        Rect rcpop;
        Point pt;
        PopupRoot* popupRoot = pop->GetPopupRoot();

        if (popupRoot == NULL)
        {
            continue;
        }

        rcpop = Rect(Point(), popupRoot->GetRenderSize());


        pt = GetCursorPoint(popupRoot);

        if (rcpop.PointIn(pt) && PointInWindow(popupRoot))
        {
            overPopup = pop;
            bOverTree = true;
            break;
        }
    }

    // 没找到，查看是否在主菜单里面
    if (!overPopup)
    {
        if (IsMouseOverOwer())
        {
            bOver = true;
            return _trackingMenu;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        bOver = true;
        return overPopup;//->GetPlacementTarget();
    }
}

bool TrackingMenuOp::IsMouseCapture() const
{        
    return true;
    for (int i = 0; i < _popups.GetCount(); ++i)
    {
        PopupPtr pop(_popups.GetItem(i));
        VisualHostPtr pHost(VisualHost::GetVisualHost(pop.get()));

        if (!pHost)
        {
            return false;
        }

        HWND hwnd = HANDLETOHWND(pHost->GetHandle());

        if (GetCapture() == hwnd)
        {
            return true;
        }
    }
    return _mouseCapture;
}

void TrackingMenuOp::SetMouseCapture(bool value)
{
}

void TrackingMenuOp::AddPopupMenuItem(Popup* popup)
{
    _popups.Add(popup);
}

}
