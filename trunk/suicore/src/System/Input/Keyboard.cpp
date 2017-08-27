/////////////////////////////////////////////////////////////////////////
// Keyboard.cpp

#include <System/Input/Keyboard.h>
#include <System/Input/InputDevice.h>
#include <System/Tools/VisualTreeOp.h>

#include <System/Interop/InternalOper.h>
#include <System/Windows/HwndSubclass.h>
#include <System/Windows/InheritStateProperty.h>

namespace suic
{

/////////////////////////////////////////////////////////
//
void DoInitResourceByEvent(Element* pElem);

KeyboardDeice::KeyboardDeice()
{
    _visualHost = NULL;

    setAutoDelete(false);
}

KeyboardDeice* KeyboardDeice::Current()
{
    static KeyboardDeice _ins;
    return &_ins;
}

VisualHost* KeyboardDeice::GetVisualHost()
{
    return _visualHost;
}

void KeyboardDeice::SetVisualHost(VisualHost* pHost)
{
    _visualHost = pHost;
}

Keyboard::Keyboard()
{
}

Keyboard::~Keyboard()
{
}

WeakRef Keyboard::_focus;
bool Keyboard::_pending = false;

Element* Keyboard::GetFocusedElement()
{
    Element* focus = _focus.GetElement<Element>();
    return focus;
}

Element* Keyboard::Focus(Element* elem)
{
    if (elem && (!elem->IsVisible() || !elem->IsFocusable()))
    {
        return _focus.GetElement<Element>();
    }
    else if (_pending) 
    {
        return _focus.GetElement<Element>();
    }

    Element* focusableElem = elem;
    Element* oldElem = _focus.GetElement<Element>();

    _pending = true;

    if (oldElem)
    {
        oldElem->ref();
    }

    /*if (elem != NULL)
    {
        while (focusableElem != NULL)
        {
            if (focusableElem->BlockVisualState())
            {
                focusableElem = NULL;
                break;
            }

            if (focusableElem->IsFocusable())
            {
                break;
            }

            focusableElem = focusableElem->GetUIParent();
        }
    }*/
    
    if (oldElem != focusableElem)
    {
        _focus = focusableElem;

        if (NULL != oldElem)
        {
            KeyboardFocusEventArg e(oldElem, oldElem, focusableElem);
            e.SetRoutedEvent(Element::PreLostKeyboardFocusEvent);
            oldElem->RaiseEvent(&e);
        }

        if (focusableElem)
        {
            KeyboardFocusEventArg e(focusableElem, oldElem, focusableElem);
            e.SetRoutedEvent(Element::PreGotKeyboardFocusEvent);
            focusableElem->RaiseEvent(&e);
        }

        FocusWithinProperty focusWithinProp;
        focusWithinProp.OnOriginalValueChanged(oldElem, focusableElem);

        if (oldElem)
        {
            KeyboardFocusEventArg e(oldElem, oldElem, focusableElem);
            oldElem->ClearValue(Element::IsKeyboardFocusedProperty);

            e.SetRoutedEvent(Element::LostKeyboardFocusEvent);
            oldElem->RaiseEvent(&e);
        }

        if (focusableElem)
        {
            KeyboardFocusEventArg e(focusableElem, oldElem, focusableElem);
  
            focusableElem->SetValue(Element::IsKeyboardFocusedProperty, Boolean::True);
            e.SetRoutedEvent(Element::GotKeyboardFocusEvent);
            focusableElem->RaiseEvent(&e);
        }
    }

    if (oldElem)
    {
        oldElem->unref();
    }

    _pending = false;

    return focusableElem;
}

bool Keyboard::IsFocusable(Element* elem)
{
    return elem->IsFocusable();
}

bool Keyboard::IsFocusedWithin(Element* element)
{
    return element->ReadFlag(CoreFlags::IsKeyboardFocusWithinCache);
}

bool Keyboard::IsKeyDown(int iKey)
{
    return (::GetKeyState(iKey) < 0);
}

Uint16 Keyboard::GetModifiers()
{
    Uint16 uState = 0;

    if (IsKeyDown(Key::kLeftAlt) || IsKeyDown(Key::kRightAlt))
    {
        uState |= ModifierKey::mkAlt;
    }

    if (IsKeyDown(Key::kLeftCtrl) || IsKeyDown(Key::kRightCtrl))
    {
        uState |= ModifierKey::mkControl;
    }

    if (IsKeyDown(Key::kLeftShift) || IsKeyDown(Key::kRightShift))
    {
        uState |= ModifierKey::mkShift;
    }

    if (IsKeyDown(Key::kLeftWin) || IsKeyDown(Key::kRightWin))
    {
        uState |= ModifierKey::mkWindows;
    }

    return uState;
}

bool Keyboard::IsModifierKey(int val)
{
    return ((Keyboard::GetModifiers() & val) == val);
}

bool Keyboard::IsAltDown()
{
    return IsModifierKey(ModifierKey::mkAlt);
}

bool Keyboard::IsShiftDown()
{
    return IsModifierKey(ModifierKey::mkShift);
}

bool Keyboard::IsControlDown()
{
    return IsModifierKey(ModifierKey::mkControl);
}

bool Keyboard::IsWinDown()
{
    return IsModifierKey(ModifierKey::mkWindows);
}

}
