/////////////////////////////////////////////////////////////////////////
// hwndkeyboardfilter.cpp

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Input/FocusManager.h>
#include <System/Input/HwndKeyboardFilter.h>
#include <System/Input/KeyboardNavigation.h>

#include <System/Windows/CoreTool.h>

#include <System/Interop/InternalOper.h>

#include <System/Tools/CoreHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/AcceleratorOp.h>

#include <Framework/Controls/Menu.h>

namespace suic
{

HwndKeyboardFilter::HwndKeyboardFilter(HwndObject* objPtr)
    : _hwndObj(objPtr)
    , _calledFocus(false)
{
}

HwndKeyboardFilter::~HwndKeyboardFilter()
{
}

Element* HwndKeyboardFilter::GetFocusedElement(Element* rootElement)
{
    Element* focused(Keyboard::GetFocusedElement());
    //
    // 如果当前焦点元素的根元素和接收键盘的窗口不是同一个
    // 则需要调整当前焦点元素
    //
    if (VisualTreeOp::GetVisualRoot(focused) != rootElement)
    {
        focused = FocusManager::GetFocusedElement(rootElement);
    }
    return focused;
}

Element* HwndKeyboardFilter::GetKeyboardRoot(Element* rootElement)
{
    Element* pRoot = rootElement;
    Popup* popup(Assigner::Current()->GetTopTrackPopup());
    if (popup != NULL)
    {
        pRoot = popup;
    }
    return pRoot;
}

bool HwndKeyboardFilter::Process_WM_KEYDOWN(Element* rootElement, MessageParam* mp)
{
    FocusManager::SetCurrentInputDevice(KeyboardDeice::Current());

    Element* pRoot = GetKeyboardRoot(rootElement);
    ElementPtr focused;
    Element* currElem = NULL;
    bool handled = false;
    Popup* popup(Assigner::Current()->GetTopTrackPopup());

    if ((int)mp->wp == VK_MENU)
    {
        /*SHORT shVal = ::GetAsyncKeyState(VK_LMENU);
        if (shVal != 0)
        {
            mp->wp = suic::Key::kLeftAlt;
        }
        else
        {
            shVal = ::GetAsyncKeyState(VK_RMENU);
            if (shVal != 0)
            {
                mp->wp = suic::Key::kRightAlt;
            }
        }*/

        VisualHost* pHost = VisualHost::GetVisualHost(rootElement);
        if (NULL != pHost && NULL != pHost->GetMainMenu())
        {
            KeyboardEventArg ke(rootElement, (int)mp->wp, CoreHelper::ToKeyFlags(mp->lp));
            pHost->GetMainMenu()->OnPreviewKeyDown(&ke);
            if (ke.IsHandled())
            {
                return true;
            }
        }
    }

    if (NULL == popup)
    {
        if (pRoot != rootElement)
        {
            handled = KeyboardNavigation::Current()->ProcessInput(pRoot, mp);
        }

        focused = GetFocusedElement(pRoot);
        currElem = focused.get();

        if (NULL == currElem)
        {
            currElem = pRoot;
        }
    }
    else
    {
        currElem = popup;
    }

    KeyboardEventArg ke(currElem, (int)mp->wp, CoreHelper::ToKeyFlags(mp->lp));

    if (!handled && currElem != NULL)
    {
        ke.SetRoutedEvent(Element::PreKeyDownEvent);
        currElem->RaiseEvent(&ke);

        ke.SetRoutedEvent(Element::KeyDownEvent);
        currElem->RaiseEvent(&ke);

        handled = ke.IsHandled();
    }

    if (!handled && pRoot == rootElement)
    {
        KeyboardNavigation::Current()->ProcessInput(focused.get(), mp);
    }

    return true;
}

bool HwndKeyboardFilter::Process_WM_KEYUP(Element* rootElement, MessageParam* mp)
{
    Element* pRoot = GetKeyboardRoot(rootElement);
    PopupPtr popup(Assigner::Current()->GetTopTrackPopup());
    Element* focused = NULL;

    if (pRoot != rootElement)
    {
        focused = pRoot;
    }
    else
    {
        focused = GetFocusedElement(pRoot);
    }

    if (focused)
    {
        KeyboardEventArg ke(focused, (int)mp->wp, CoreHelper::ToKeyFlags(mp->lp));

        ke.SetRoutedEvent(Element::PreKeyUpEvent);
        focused->RaiseEvent(&ke);

        ke.SetRoutedEvent(Element::KeyUpEvent);
        focused->RaiseEvent(&ke);
    }

    return true;
}

void HwndKeyboardFilter::Process_WM_KILLFOCUS(Element* rootElement, MessageParam* mp)
{
    Element* focused(Keyboard::GetFocusedElement());

    // 当前失去焦点窗体是同一个
    Keyboard::Focus(NULL);

    Element* prevOver = Mouse::GetDirectlyOver();
    Element* overRoot = VisualTreeOp::GetVisualRoot(prevOver);
    Element* capRoot = VisualTreeOp::GetVisualRoot(Mouse::GetCaptured());

    if (overRoot == rootElement)
    {
        Mouse::ChangeMouseOver(NULL);
    }

    if (capRoot == rootElement)
    {
        Mouse::SetCaptured(NULL);
    }
}

void HwndKeyboardFilter::Process_WM_SETFOCUS(Element* rootElement, MessageParam* mp)
{
    Element* fElem = NULL;
    if (FocusManager::GetIsFocusScope(rootElement))
    {
        fElem = FocusManager::GetFocusedElement(rootElement);
    }

    if (!fElem)
    {
        fElem = GetFocusedElement(rootElement);
    }

    if (!fElem && rootElement->IsFocusable())
    {
        fElem = rootElement;
    }

    Keyboard::Focus(fElem);
    //InvalidateRect(HANDLETOHWND(_hwndObj->GetHandle()), NULL, FALSE);
}

void HwndKeyboardFilter::Process_WM_HOTKEY(Element* rootElement, MessageParam* mp)
{
    UINT uModifiers = (UINT)LOWORD(mp->lp);
    UINT uVirtKey = (UINT)HIWORD(mp->lp);

    if (uModifiers == MOD_CONTROL && uVirtKey == 'D')
    {
        ;
    }

    //
    //RegisterHotKey(NULL, 1001, MOD_CONTROL, 'M');
    //UnregisterHotKey(NULL, 1001);
}

bool HwndKeyboardFilter::OnFilterMessage(Object* sender, MessageParam* mp)
{
    ElementPtr rootElement(sender);
    bool handled = false;
    Element* focusElem = Keyboard::GetFocusedElement();

    /*if (mp->message >= WM_KEYFIRST && mp->message <= WM_KEYLAST)
    {
    MessageEventArg kbe(mp);

    focusElem->ref();
    focusElem->OnKeyboardEvent(&kbe);
    focusElem->unref();

    handled = kbe.IsHandled();

    if (handled)
    {
    return handled;
    }
    }*/

    switch (mp->message)
    {
    case WM_CHAR:
    case WM_SYSCHAR:
        {
            KeyboardEventArg ke(focusElem, (int)mp->wp, CoreHelper::ToKeyFlags(mp->lp));

            AcceleratorOp::ProcessTextInput(rootElement.get(), &ke);

            if (NULL != focusElem)
            {
                focusElem->ref();

                ke.SetRoutedEvent(Element::PreTextInputEvent);
                focusElem->RaiseEvent(&ke);

                ke.SetRoutedEvent(Element::TextInputEvent);
                focusElem->RaiseEvent(&ke);

                focusElem->unref();
            }

            handled = true;
        }
        break;

    case WM_COMMAND:
        {
            handled = false;
        }
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        handled = Process_WM_KEYDOWN(rootElement.get(), mp);
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        handled = Process_WM_KEYUP(rootElement.get(), mp);
        break;

    case WM_KILLFOCUS:
        Process_WM_KILLFOCUS(rootElement.get(), mp);
        break;

    case WM_SETFOCUS:
        Process_WM_SETFOCUS(rootElement.get(), mp);
        break;

    case WM_HOTKEY:
        Process_WM_HOTKEY(rootElement.get(), mp);
        handled = true;
        break;

    case WM_IME_STARTCOMPOSITION:
    case WM_IME_ENDCOMPOSITION:
    case WM_IME_COMPOSITION:
    case WM_IME_CHAR:
        if (NULL != focusElem)
        {
            MessageEventArg ke(mp);

            focusElem->ref();
            focusElem->OnImeEvent(&ke);
            focusElem->unref();

            handled = ke.IsHandled();
        }
        break;

    default:
        ;
    }

    return handled;
}

}
