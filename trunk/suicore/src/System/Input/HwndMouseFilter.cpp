/////////////////////////////////////////////////////////////////////////
// hwndmousefilter.cpp

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Input/MouseDevice.h>
#include <System/Input/FocusManager.h>
#include <System/Input/HwndMouseFilter.h>
#include <System/Input/KeyboardNavigation.h>

#include <System/Tools/HwndHelper.h>
#include <System/Tools/VisualTreeOp.h>

#include <System/Windows/CoreTool.h>
#include <System/Windows/EventBuilder.h>
#include <System/Windows/HwndSubclass.h>

#include <System/Interop/System.h>
#include <System/Interop/InternalOper.h>

#include <System/DragDrop/DragDrop.h>
#include <System/DragDrop/DataObject.h>

//#include <Framework/Controls/TextBox.h>

namespace suic
{

/////////////////////////////////////////////////////////
//
HwndMouseFilter::HwndMouseFilter(HwndObject* objPtr)
    : _hwndObj(objPtr)
{
}

HwndMouseFilter::~HwndMouseFilter()
{
}

//================================================================

void HwndMouseFilter::HandleMouseCapture(Element* rootElement, MessageParam* mp)
{
    Element* capPtr = Mouse::GetCaptured();
    Rect rc(GetRenderBound(capPtr));

    if (MouseDevice::GetMouseOver() != capPtr)
    {
        MouseDevice::SetMouseOver(capPtr);
    }

    MouseButtonEventArg me(capPtr, mp->point);
    me.SetRoutedEvent(Element::MouseMoveEvent);
    capPtr->RaiseEvent(&me);
}

void HwndMouseFilter::SetMouseInputDevice()
{
    FocusManager::SetCurrentInputDevice(MouseDevice::Current());
    KeyboardNavigation::Current()->HideFocusVisual();
}

bool HwndMouseFilter::OnFilterMessage(Object* sender, MessageParam* mp)
{
    Element* rootElement  = CASTTOE(sender);
    bool handled = true;

    switch (mp->message)
    {
    case WM_SETCURSOR:
        handled = Process_WM_SETCURSOR(rootElement, mp);
        break;

    case WM_NCHITTEST:
        handled = Process_WM_NCHITTEST(rootElement, mp);
        break;

    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
        Process_WM_MOUSEMOVE(rootElement, mp);
        break;

    case WM_MOUSEACTIVATE:
        break;

    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONDBLCLK:    
        SetMouseInputDevice();
        handled = Process_WM_LBUTTONDOWN(rootElement, mp);
        break;

    case WM_LBUTTONDOWN:
        SetMouseInputDevice();
        handled = Process_WM_LBUTTONDOWN(rootElement, mp);
        break;

    case WM_MBUTTONDOWN:
        SetMouseInputDevice();
        handled = Process_WM_MBUTTONDOWN(rootElement, mp);
        break;

    case WM_MBUTTONUP:
        handled = Process_WM_MBUTTONUP(rootElement, mp);
        break;

    case WM_LBUTTONDBLCLK:
        handled = Process_WM_LBUTTONDOWN(rootElement, mp);
        break;

    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
        suic::Debug::Trace(_U("WM_LBUTTONUP...\n"));
        Process_WM_LBUTTONUP(rootElement, mp);
        break;

    case WM_RBUTTONDOWN:
        suic::Debug::Trace(_U("WM_RBUTTONDOWN...\n"));
        SetMouseInputDevice();
        Process_WM_RBUTTONDOWN(rootElement, mp);
        //handled = false;
        break;

    case WM_RBUTTONDBLCLK:
        SetMouseInputDevice();
        Process_WM_RBUTTONDOWN(rootElement, mp);
        handled = true;
        break;

    case WM_RBUTTONUP:
        suic::Debug::Trace(_U("WM_RBUTTONUP...\n"));
        Process_WM_RBUTTONUP(rootElement, mp);
        handled = false;
        break;

    case WM_MOUSELEAVE:
        Process_WM_MOUSELEAVE(rootElement, mp);
        break;

    case WM_MOUSEWHEEL:
        SetMouseInputDevice();
        Process_WM_MOUSEWHEEL(rootElement, mp);
        handled = true;
        break;

    case WM_CONTEXTMENU:
        Process_WM_CONTEXTMENU(rootElement, mp);
        handled = true;
        break;

    case WM_CAPTURECHANGED:
        {   
            SetMouseInputDevice();
            Element* capElem(Mouse::GetCaptured());

            // 做capture处理
            if (capElem)
            {
                if (rootElement != VisualTreeOp::GetVisualRoot(capElem))
                {
                    Mouse::SetCaptured(NULL);
                }
            }
            break;
        }

    case WM_DROPFILES:
        handled = Process_WM_DROPFILES(rootElement, mp);
        break;

    default:
        handled = false;
    }

    return handled;
}

void HwndMouseFilter::Process_WM_MOUSEMOVE(Element* rootElement, MessageParam* mp)
{
    ElementPtr capElem(Mouse::GetCaptured());

    // 
    // 做capture处理
    //
    if (capElem.get() != NULL)
    {
        Element* prev(Mouse::GetDirectlyOver());
        if (prev && prev != capElem.get())
        {
            Mouse::ChangeMouseOver(capElem.get());
        }

        HandleMouseCapture(rootElement, mp);
    }
    else 
    { 
        _hwndObj->StartMouseTrack();

        HitTestCtx hitRes(mp->point);
        rootElement->HitTest(&hitRes);
        MouseButtonEventArg e(hitRes.hitRes.get(), mp->point);

        if (hitRes.hitRes != MouseDevice::GetMouseOver())
        {
            Mouse::ChangeMouseOver(hitRes.hitRes.get());
        }
        else if (NULL != hitRes.hitRes.get())
        {
            e.SetRoutedEvent(Element::PreMouseMoveEvent);
            hitRes.hitRes->RaiseEvent(&e);

            e.SetRoutedEvent(Element::MouseMoveEvent);
            hitRes.hitRes->RaiseEvent(&e);
        }

        //Process_MouseEvent(hitRes.hitResult.get(), mp);
    }
}

void HwndMouseFilter::Process_WM_MOUSELEAVE(Element* rootElement, MessageParam* mp)
{
    Element* ePtr(Mouse::GetDirectlyOver());
    MouseButtonEventArg me(NULL, mp->point);
    Element* eOld = VisualTreeOp::GetVisualRoot(ePtr);

    if (ePtr && eOld == rootElement && ePtr != Mouse::GetCaptured())
    {
        Mouse::SetCaptured(NULL);
        Mouse::ChangeMouseOver(NULL);

        me.SetRoutedEvent(Element::MouseLeaveEvent);
        ePtr->RaiseEvent(&me);
    }

    _hwndObj->StopMouseTrack();
}

void HwndMouseFilter::Process_WM_MOUSEWHEEL(Element* rootElement, MessageParam* mp)
{
    Point pt = mp->point;
    Element* root(rootElement);
    Popup* popup(Assigner::Current()->GetTopTrackPopup());

    KeyboardNavigation::Current()->HideFocusVisual();

    if (popup != NULL && popup->GetPopupRoot())
    {
        root = popup->GetPopupRoot();
        pt = __GetCusorPoint(root);

        if (!root->HitTestMouse(pt))
        {
            return;
        }
    }

    MouseWheelEventArg e(pt, mp->timestamp, (short)HIWORD(mp->wp));
    HitTestCtx hitRes(pt);
    root->HitTest(&hitRes);

    if (NULL != hitRes.hitRes.get())
    {
        e.SetSource(hitRes.hitRes.get());
        e.OnOverrideOriginalSource(hitRes.hitRes.get());

        e.SetRoutedEvent(Element::PreMouseWheelEvent);
        hitRes.hitRes->RaiseEvent(&e);

        e.SetRoutedEvent(Element::MouseWheelEvent);
        hitRes.hitRes->RaiseEvent(&e);
    }
}

bool HwndMouseFilter::Process_WM_DROPFILES(Element* rootElement, MessageParam* mp)
{
    HDROP hdrop = (HDROP)mp->wp;
    int iFileCount = ::DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 512);

    if (iFileCount > 0)
    {
        String* fileDrag = new String[iFileCount];

        for (int i = 0; i < iFileCount; ++i)
        {
            Char szFilename[512] = {0};

            ::DragQueryFile(hdrop, i, &szFilename[0], 512);
            fileDrag[i] = szFilename;
        }

        DataStore* dataStore = new DataStore(new FileDragInfo(fileDrag, iFileCount));
        HitTestCtx hitRes(mp->point);
        rootElement->HitTest(&hitRes);
        suic::DragEventArg e(dataStore, suic::DragDropKeyStates::eDragKeyNone, 
            suic::DragDropEffects::eDragNone, hitRes.hitRes.get(), 
            mp->point, suic::eDropType::dtFiles);

        e.SetRoutedEvent(suic::DragDrop::PreviewDropEvent);
        hitRes.hitRes->RaiseEvent(&e);

        e.SetRoutedEvent(suic::DragDrop::DropEvent);
        hitRes.hitRes->RaiseEvent(&e);

        ::DragFinish(hdrop);

        return true;
    }
    else
    {
        return false;
    }
}

void HwndMouseFilter::Process_WM_CONTEXTMENU(Element* rootElement, MessageParam* mp)
{
    //CloseTrackingPopup();

    HitTestCtx hitRes(mp->point);
    rootElement->HitTest(&hitRes);
    ContextMenuEventArg e(hitRes.hitRes.get(), mp->point.x, mp->point.y);

    if (hitRes.hitRes && hitRes.hitRes->IsEnabled())
    {
        e.SetRoutedEvent(Element::ContextMenuEvent);
        hitRes.hitRes->RaiseEvent(&e);
    }
}

void HwndMouseFilter::BeginMouseDown(int iDownFlag)
{
    // 单击
    if (MouseDevice::Current()->GetClickCount() <= 1)
    {
        suic::CoreSystem::Ins()->SetMouseDown(false);
    }
    else
    {
        suic::CoreSystem::Ins()->SetMouseDown(true);
    }
}

void HwndMouseFilter::EndMouseDown(int iDownFlag)
{
    if (!suic::CoreSystem::Ins()->IsSetMouseDown()) 
    {
        suic::MouseDevice::SetMouseDown(NULL, iDownFlag);
        suic::CoreSystem::Ins()->SetMouseDown(true);
    }
}

void HwndMouseFilter::HandMouseDown(Element* mouse, Point point, Int32 mouseType)
{
    MouseDevice::SetRawMouseDown(mouse, mouseType);

    const int xClickDelta = GetSystemMetrics(SM_CXDOUBLECLK);
    const int yClickDelta = GetSystemMetrics(SM_CYDOUBLECLK);

    // 计算鼠标偏移
    int xOff = abs(point.x - MouseDevice::Current()->GetLastClick().x);
    int yOff = abs(point.y - MouseDevice::Current()->GetLastClick().y);

    Int32 timeStamp = Environment::GetSystemTick();
    Int32 iClickCount = MouseDevice::Current()->GetClickCount();
    // 计算鼠标单击次数
    Int32 timeSpan = timeStamp - MouseDevice::Current()->GetLastClickTime();

    if (timeSpan < MouseDevice::Current()->GetDoubleDeltaTime()
        && MouseDevice::Current()->GetLastButton() == mouseType
        && xOff < xClickDelta && yOff < yClickDelta)
    {
        ++iClickCount;
    }
    else
    {
        iClickCount = 1;
    }
    
    if (iClickCount == 1)
    {
        MouseDevice::Current()->SetLastButton(mouseType);
        MouseDevice::Current()->SetLastClickTime(timeStamp);
        MouseDevice::Current()->SetLastClick(point);
    }

    MouseDevice::Current()->SetClickCount(iClickCount);
}

bool HwndMouseFilter::Process_WM_MBUTTONDOWN(Element* rootElement, MessageParam* mp)
{
    ToolTip::Ins()->Close();
    FocusManager::SetCurrentInputDevice(MouseDevice::Current());

    HitTestCtx hitRes(mp->point);
    rootElement->HitTest(&hitRes);

    HandMouseDown(hitRes.hitRes.get(), mp->point, MouseButton::mbMiddleButton);
    BeginMouseDown(MouseButton::mbMiddleButton);

    if (!hitRes.hitRes->IsEnabled())
    {
        MouseDevice::SetRawMouseDown(NULL, MouseButton::mbMiddleButton);
    }
    else
    {
        MouseButtonEventArg e(hitRes.hitRes.get(), mp->point);

        e.SetMouseButton(MouseButton::mbMiddleButton);
        e.SetRoutedEvent(Element::PreMouseDownEvent);
        hitRes.hitRes->RaiseEvent(&e);

        e.SetRoutedEvent(Element::MouseDownEvent);
        hitRes.hitRes->RaiseEvent(&e);
    }

    EndMouseDown(MouseButton::mbMiddleButton);

    return true;
}

bool HwndMouseFilter::Process_WM_MBUTTONUP(Element* rootElement, MessageParam* mp)
{
    ElementPtr down(MouseDevice::GetRawMouseDown(MouseButton::mbMiddleButton));
    MouseDevice::SetRawMouseDown(NULL, MouseButton::mbMiddleButton);

    if (down)
    {
        MouseButtonEventArg e(down.get(), mp->point);

        e.SetRoutedEvent(Element::MouseUpEvent);
        e.SetMouseButton(MouseButton::mbMiddleButton);
        down->RaiseEvent(&e);
    }

    return true;
}

bool HwndMouseFilter::Process_WM_LBUTTONDOWN(Element* rootElement, MessageParam* mp)
{
    ToolTip::Ins()->Close();

    HitTestCtx hitRes(mp->point);
    rootElement->HitTest(&hitRes);
    ElementPtr prevRoot = VisualTreeOp::GetVisualRoot(Mouse::GetDirectlyOver());

    HandMouseDown(hitRes.hitRes.get(), mp->point, MouseButton::mbLeftButton);
    BeginMouseDown(MouseButton::mbLeftButton);

    if (!hitRes.hitRes)
    {
        MouseDevice::SetRawMouseDown(NULL, MouseButton::mbLeftButton);
        return false;
    }

    if (!hitRes.hitRes->IsEnabled())
    {
        MouseDevice::SetRawMouseDown(NULL, MouseButton::mbLeftButton);
        return true;
    }

    MouseButtonEventArg e(hitRes.hitRes.get(), mp->point);

    e.SetMouseButton(MouseButton::mbLeftButton);
    e.SetRoutedEvent(Element::PreMouseDownEvent);
    hitRes.hitRes->RaiseEvent(&e);

    e.SetRoutedEvent(Element::MouseDownEvent);
    hitRes.hitRes->RaiseEvent(&e);

    Element* capElem = Mouse::GetCaptured();

    if (capElem != NULL)
    {
        MouseDevice::SetRawMouseDown(capElem, MouseButton::mbLeftButton);
    }

    EndMouseDown(MouseButton::mbLeftButton);

    return e.IsHandled();
}

void HwndMouseFilter::Process_WM_LBUTTONUP(Element* rootElement, MessageParam* mp)
{
    ElementPtr down(MouseDevice::GetRawMouseDown(MouseButton::mbLeftButton));
    MouseDevice::SetRawMouseDown(NULL, MouseButton::mbLeftButton);

    if (down)
    {
        MouseButtonEventArg e(down.get(), mp->point);

        e.SetRoutedEvent(Element::MouseUpEvent);
        e.SetMouseButton(MouseButton::mbLeftButton);
        down->RaiseEvent(&e);
    }
}

bool HwndMouseFilter::Process_WM_RBUTTONDOWN(Element* rootElement, MessageParam* mp)
{
    HitTestCtx hitRes(mp->point);
    rootElement->HitTest(&hitRes);
    MouseButtonEventArg e(hitRes.hitRes.get(), mp->point);
    String mouseupFlag;

    HandMouseDown(hitRes.hitRes.get(), mp->point, MouseButton::mbRightButton);
    BeginMouseDown(MouseButton::mbRightButton);

    if (hitRes.hitRes && hitRes.hitRes->IsEnabled())
    {
        HandMouseDown(hitRes.hitRes.get(), mp->point, MouseButton::mbRightButton);

        e.SetMouseButton(MouseButton::mbRightButton);

        mouseupFlag.Format(_T("%d_%d")
            , Environment::GetSystemTick(), (Int32)(LONG_PTR)hitRes.hitRes.get());
        mouseupFlag = CoreSystem::Ins()->SetRMouseupFlag(mouseupFlag);

        e.SetRoutedEvent(Element::PreMouseDownEvent);
        hitRes.hitRes->RaiseEvent(&e);

        e.SetRoutedEvent(Element::MouseDownEvent);
        hitRes.hitRes->RaiseEvent(&e);

        CoreSystem::Ins()->SetRMouseupFlag(_T(""));
    }
    else
    {
        MouseDevice::SetRawMouseDown(NULL, MouseButton::mbRightButton);
    }

    EndMouseDown(MouseButton::mbRightButton);

    return e.IsHandled();
}

void HwndMouseFilter::Process_WM_RBUTTONUP(Element* rootElement, MessageParam* mp)
{
    FrameworkElementPtr down = MouseDevice::GetRawMouseDown(MouseButton::mbRightButton); 
    MouseButtonEventArg eu(Mouse::GetDirectlyOver(), mp->point);

    MouseDevice::SetRawMouseDown(NULL, MouseButton::mbRightButton);

    if (down && CoreSystem::Ins()->GetRMouseupFlag().Empty())
    {
        ContextMenuEventArg ecm(down.get(), 0, 0);

        eu.SetMouseButton(MouseButton::mbRightButton);

        eu.SetRoutedEvent(Element::PreMouseUpEvent);
        down->RaiseEvent(&eu);

        eu.SetRoutedEvent(Element::MouseUpEvent);
        down->RaiseEvent(&eu);
    }
}

bool HwndMouseFilter::Process_WM_NCHITTEST(Element* rootElement, MessageParam* mp)
{
    mp->result = HTCLIENT;

    if (HwndHelper::IsWindowMaximize(rootElement))
    {
        return false;
    }

    // 先测试主窗口
    HitResultEventArg ehr(rootElement, mp->point);
    HitResult& pHr = ehr.GetHitResult();

    rootElement->OnHitTest(&ehr);

    if (pHr.HitTestCode() == CoreFlags::HitValid)
    {
        HitTestCtx hitRes(mp->point);
        rootElement->HitTest(&hitRes);

        if (hitRes.hitRes && rootElement != hitRes.hitRes.get())
        {
            hitRes.hitRes->OnHitTest(&ehr);
        }
        else
        {
            pHr.HitTestCode(HTCLIENT);
        }
    }

    mp->result = pHr.HitTestCode();
    return ehr.IsHandled();
}

bool HwndMouseFilter::Process_WM_SETCURSOR(Element* rootElement, MessageParam* mp)
{
    bool handled = false;
    CursorEventArg e(mp->point);
    Element* cur = Mouse::GetCaptured();
    
    if (!cur)
    {
        HitTestCtx hitRes(mp->point);
        rootElement->HitTest(&hitRes);
        cur = hitRes.hitRes.get();
    }

    if (!cur)
    {
        return handled;
    }

    e.SetRoutedEvent(Element::PreCursorSetEvent);
    cur->RaiseEvent(&e);

    if (!e.IsHandled())
    {
        e.SetRoutedEvent(Element::CursorSetEvent);
        cur->RaiseEvent(&e);
    }

    if (e.IsHandled())
    {
        if (!e.IsIgnore())
        {
            handled = true;
            mp->result = 1;
            OCursor* cursor(e.GetCursor());
            if (!cursor)
            {
                OCursor::Arrow->SetCursor();
            }
            else
            {
                cursor->SetCursor();
            }
        }
    }
    /*else
    {
        OCursor::ArrowCursor->SetCursor();
    }*/

    /*if (!handled)
    {
        HWND hwnd = HANDLETOHWND(mp->hwnd);
        handled = true;
        DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);

        ::SetWindowLong(hwnd, GWL_STYLE, dwStyle&(~WS_SYSMENU));
        _hwndObj->CallDefWindowProc(mp);
        ::SetWindowLong(hwnd, GWL_STYLE, dwStyle);
    }*/

    return handled;
}

}
