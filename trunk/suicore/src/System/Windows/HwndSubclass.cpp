
#include <System/Windows/HwndSubclass.h>
#include <System/Windows/Assigner.h>
#include <System/Windows/HwndObject.h>

#include <System/Windows/RenderTarget.h>
#include <Framework/Controls/Tooltip.h>
#include <Framework/Controls/Window.h>
#include <Framework/Controls/Popup.h>
#include <Framework/Controls/Application.h>

#include <System/Interop/System.h>
#include <System/Interop/InternalOper.h>

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Input/MouseDevice.h>

#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/Array.h>

#include <System/Windows/CoreTool.h>

namespace suic
{

PointerAutoDic<HWND, VisualHost> HwndSubclass::_hwnds;

HwndSubclass::HwndSubclass(const MessageHook& hook)
{
}

void HwndSubclass::DoHwndDestroyMessage(HWND hwnd, SysRenderSource* visual, MessageParam* mp)
{
    if (mp->message == WM_NCDESTROY)
    {
        ElementPtr pRoot = VisualTreeOp::GetVisualRoot(Keyboard::GetFocusedElement());
        ElementPtr pCapRoot = VisualTreeOp::GetVisualRoot(Mouse::GetCaptured());
        ElementPtr pDownRoot = VisualTreeOp::GetVisualRoot(MouseDevice::GetRawMouseDown(MouseButton::mbLeftButton));

        _hwnds.Remove(hwnd);
        //Assigner::Current()->RecycleObject(visual);

        if (visual->GetRootElement() == pRoot)
        {
            Keyboard::Focus(NULL);
        }

        if (visual->GetRootElement() == pCapRoot)
        {
            Mouse::SetCaptured(NULL);
        }

        if (visual->GetRootElement() == pDownRoot)
        {
            MouseDevice::SetRawMouseDown(NULL, MouseButton::mbLeftButton);
        }

        pRoot = VisualTreeOp::GetVisualRoot(Mouse::GetDirectlyOver());

        if (visual->GetRootElement() == pRoot)
        {
            Mouse::ChangeMouseOver(NULL);
        }

        FrameworkElement* rootElem(visual->GetRootElement());

        visual->StopMouseTrack();
        RecursiveUnloaded(rootElem);
        
        rootElem->SetValue(VisualHost::RootSourceProperty, NULL);
    }
}

void HwndSubclass::HandleNcCreate(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    SysRenderSource* visual = NULL;

    LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
    visual = (SysRenderSource*)lpcs->lpCreateParams;
 
    if (NULL != visual)
    {
        visual->SetHwnd(HWNDTOHANDLE(hwnd));
        _hwnds.Add(hwnd, visual);
    }
}

void HwndSubclass::HandleCommand(MessageParam* mp, bool handled)
{
    if (mp->message == WM_COMMAND)
    {
        if (!handled)
        {
            HWND hwnd = ::GetParent(HANDLETOHWND(mp->hwnd));
            if (::IsWindow(hwnd))
            {
                ::PostMessage(hwnd, mp->message, mp->wp, mp->lp);
            }
        }
        handled = true;
        mp->result = CallDefWindowProc(HANDLETOHWND(mp->hwnd), mp->message, mp->wp, mp->lp);
    }
}

LRESULT __stdcall HwndSubclass::DoWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, bool callDef, bool& handled)
{
    LRESULT result = 0;

    if (message == WM_NCCREATE)
    {
        HandleNcCreate(hwnd, wParam, lParam);
    }
    else if (message == WM_ERASEBKGND)
    {
        return 1;
    }
    else if (WM_LBUTTONDOWN == message || WM_RBUTTONDOWN == message)
    {
        ToolTip::Ins()->Close();
    }

    VisualHost* visualHost = NULL;

    MessageParam prevmp = Assigner::Current()->lastmp;
    MessageParam& mp = Assigner::Current()->lastmp;

    mp.message = message;
    mp.hwnd = (Handle)(LONG_PTR)hwnd;
    mp.wp = (Uint32)wParam;
    mp.lp = (Uint32)lParam;
    mp.point = __GetCusorPointFromHwnd(hwnd);
    mp.result = 0;

    if (_hwnds.TryGetValue(hwnd, visualHost))
    {
        AssignerPtr assign(Assigner::Current());
        SysRenderSource* visual(RTTICast<SysRenderSource>(visualHost));
        FrameworkElement* rootElem = visual->GetRootElement();

        visual->ref();
        rootElem->ref();

        if (!handled)
        {
            Window* rootWnd = RTTICast<Window>(rootElem);
            if (NULL != rootWnd)
            {
                handled = rootWnd->OnEvent(rootElem, &mp);
            }
        }

        if (!handled && Application::Current())
        {
            handled = Application::Current()->OnEvent(rootElem, &mp);
        }

        if (handled)
        {
            visual->unref();
            rootElem->unref();
            Assigner::Current()->lastmp = prevmp;
            return mp.result;
        }

        if (WM_KEYDOWN == message && VK_F8 == (int)mp.wp)
        {
            CoreSystem::Ins()->SetDrawDebugLine();
            rootElem->InvalidateVisual();
        }

        bool interrupt = false;

        // 先让整个分配器处理
        if (!assign->DoMessageHook(rootElem, &mp, interrupt) && !interrupt)
        {
            int iCount = assign->GetAssignerFrameCount();
            
            for (int j = iCount - 1; j >= 0; --j)
            {
                AssignerFrame* curf = assign->GetAssignerFrame(j);

                if (curf->IsContinue() && curf->_hooks.GetCount() > 0)
                {
                    // 先处理hook消息
                    handled = curf->_hooks.DoMessageHook(rootElem, &mp, interrupt);
                    
                    //
                    // 主消息循环已经退出（因为这里的窗口宿主已经被清除）
                    //
                    if (handled || interrupt || visual->getRefCount() == 1)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            handled = true;
        }

        int iRefCount = visual->getRefCount();

        if (iRefCount > 1)
        {
            HandleCommand(&mp, handled);

            if (!handled)
            {
                handled = visual->HandleEvent(&mp);
            }
        }

        DoHwndDestroyMessage(hwnd, visual, &mp);

        result = mp.result;
        visual->unref();
        rootElem->unref();
    }
    else
    {
        Debug::Trace(_T("Warning: can't find HwndWrapper, count of hwnd: %d\n"), _hwnds.GetCount());
    }

    if (callDef && !handled)
    {
        result = CallDefWindowProc(hwnd, message, wParam, lParam);
    }

    Assigner::Current()->lastmp = prevmp;

    /*if (message == WM_CREATE)
    {
        ::AnimateWindow(HANDLETOHWND(mp->hwnd), 200, AW_CENTER);
    }*/

    return result;
}

LRESULT __stdcall HwndSubclass::CallWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, bool& handled)
{
    LRESULT result = 0;
    result = DoWndProc(hwnd, message, wParam, lParam, false, handled);
    return result;
}

LRESULT __stdcall HwndSubclass::SubclassWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool handled = false;
    LRESULT result = 0;
    result = DoWndProc(hwnd, message, wParam, lParam, true, handled);
    return result;
}

LRESULT HwndSubclass::CallDefWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProc(hwnd, message, wParam, lParam);
}

void HwndSubclass::Close()
{
}

void HwndSubclass::SubclassNativeWnd(HWND hwnd)
{
}

void HwndSubclass::UnsubclassNativeWnd(HWND hwnd)
{
}

void HwndSubclass::InvalidateAllWindows(Rect* lprc)
{
    Collection<VisualHost*> vals;

    _hwnds.GetValues(vals);
 
    for (int i = 0; i < vals.GetCount(); ++i)
    {
        VisualHost* visualHost = vals.GetItem(i);

        if (NULL == lprc)
        {
            visualHost->Invalidate(lprc, false);
        }
        else
        {
            Rect rect;
            Rect uprect(*lprc);

            GetWindowRect(HANDLETOHWND(visualHost->GetHandle()), &rect);

            uprect.Intersect(&rect);
            uprect.Offset(-rect.left, -rect.top);

            if (!uprect.Empty())
            {
                visualHost->Invalidate(NULL, false);
            }
        }
    }
}

bool HwndSubclass::HasHwnds()
{
    int iSize = _hwnds.GetCount();
    return (iSize > 0);
}

Window* HwndSubclass::GetTopActiveWnd()
{
    Window* pActive = NULL;
    Collection<HWND> keys;

    _hwnds.GetKeys(keys);

    for (int i = 0; i < keys.GetCount(); ++i)
    {
        HWND hwnd = keys.GetItem(i);

        if (::IsWindow(hwnd) && ::IsWindowVisible(hwnd) && 
            (::GetForegroundWindow() == hwnd || ::GetFocus() == hwnd))
        {
            VisualHost* pHost = NULL;
            _hwnds.TryGetValue(hwnd, pHost);

            if (pHost != NULL)
            {
                pActive = DynamicCast<Window>(pHost->GetRootElement());
            }

            break;
        }
    }

    return pActive;
}

Handle HwndSubclass::GetTopActiveHwnd()
{
    HWND hActive = NULL;
    Collection<HWND> keys;

    _hwnds.GetKeys(keys);

    for (int i = 0; i < keys.GetCount(); ++i)
    {
        HWND hwnd = keys.GetItem(i);
        if (::IsWindow(hwnd) && ::IsWindowVisible(hwnd) && 
            (::GetForegroundWindow() == hwnd || ::GetFocus() == hwnd))
        {
            hActive = hwnd;
            break;
        }
    }

    return HWNDTOHANDLE(hActive);
}

Element* HwndSubclass::LookRootElement(String name)
{
    HWND hActive = NULL;
    Collection<VisualHost*> vals;

    _hwnds.GetValues(vals);

    for (int i = 0; i < vals.GetCount(); ++i)
    {
        VisualHost* pHost = vals.GetItem(i);
        if (pHost && pHost->GetRootElement() && 
            pHost->GetRootElement()->GetName().Equals(name))
        {
            return pHost->GetRootElement();
        }
    }

    return NULL;
}

VisualHost* HwndSubclass::MatchRenderSource(Element* elem)
{
    VisualHost* obj = NULL;

    if (elem == NULL)
    {
        return NULL;
    }

    obj = VisualHost::GetVisualHost(elem);

    if (obj == NULL && elem->IsLoaded())
    {
        Window* pWnd = RTTICast<Window>(elem);
        Popup* pPopup = RTTICast<Popup>(elem);
        ToolTip* pToolTip = RTTICast<ToolTip>(elem);
        if (NULL == pWnd && NULL == pPopup && NULL == pToolTip && elem->GetUIParent() != NULL)
        {
            ArgumentNullException e(_U("HwndResource is null, may be can not find root element!"), __FILELINE__);
        }
    }
    return obj;
}

VisualHost* HwndSubclass::FindRenderTarget(HWND hwnd)
{
    VisualHost* visualHost = NULL;
    _hwnds.TryGetValue(hwnd, visualHost);
    return visualHost;
}

void HwndSubclass::RemoveHwnd(Handle h)
{
    _hwnds.Remove(HANDLETOHWND(h));
}

SysRenderSource* HwndSubclass::MatchHwndObject(ElementPtr elem)
{
    return dynamic_cast<SysRenderSource*>(MatchRenderSource(elem.get()));
}

void HwndSubclass::UpdateResources()
{
    Collection<VisualHost*> vals;

    _hwnds.GetValues(vals);

    for (int i = 0; i < vals.GetCount(); ++i)
    {
        VisualHost* visualHost = vals.GetItem(i);
        FrameworkElement* frame = visualHost->GetRootElement();
        if (frame)
        {
            frame->InvalidateProp(FrameworkElement::StyleProperty);
            frame->UpdateLayout();
        }
    }
}

void HwndSubclass::InvalidateResourceReference()
{
    Collection<VisualHost*> vals;

    _hwnds.GetValues(vals);

    for (int i = 0; i < vals.GetCount(); ++i)
    {
        VisualHost* visualHost = vals.GetItem(i);
        FrameworkElement* frame = visualHost->GetRootElement();
        if (frame)
        {
            frame->InvalidateResourceReference(false);
            frame->UpdateLayout();
        }
    }
}

void HwndSubclass::RenderVisualHost(VisualHost* visualHost)
{
    RenderInfo* pInfo(visualHost->GetRenderInfo());
    Rect rect;
    HWND hwnd = HANDLETOHWND(visualHost->GetHandle());

    /*if (pInfo->PopClip(rect) && !::IsIconic(hwnd))
    {
        if (NULL != hwnd && ::IsWindow(hwnd))
        {
            visualHost->GetRootElement()->GetAssigner()->GetTimeManager()->Tick();

            DWORD exStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
            bool bLayered = false;//(exStyle & WS_EX_LAYERED) != 0;
            if (bLayered) 
            {
                visualHost->OnRender(&rect);
            }
            else
            {
                ::RedrawWindow(hwnd, &rect, NULL, RDW_INVALIDATE|RDW_INTERNALPAINT);
            }
        }
    }*/
}

void HwndSubclass::RenderHwnd(HWND hwnd)
{
    VisualHost* visualHost = FindRenderTarget(hwnd);
    if (NULL != visualHost)
    {
        RenderVisualHost(visualHost);
    }
}

void HwndSubclass::Render()
{
    Collection<VisualHost*> vals;
    _hwnds.GetValues(vals);

    for (int i = 0; i < vals.GetCount(); ++i)
    {
        VisualHost* visualHost = vals.GetItem(i);
        RenderVisualHost(visualHost);
    }
}

}
