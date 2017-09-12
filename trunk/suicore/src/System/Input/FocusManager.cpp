/////////////////////////////////////////////////////////////////////////
// FocusManager.cpp

#include <System/Input/FocusManager.h>
#include <System/Input/Keyboard.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Windows/WeakRefObj.h>

namespace suic
{

DpProperty* FocusManager::IsFocusScopeProperty;
DpProperty* FocusManager::FocusedElementProperty;
InputDevice* FocusManager::_inputDevice;

FocusManager::FocusManager()
{
    setAutoDelete(false);
}

void FocusManager::StaticInit()
{
    if (NULL == IsFocusScopeProperty)
    {
        IsFocusScopeProperty = DpProperty::RegisterAttached(_T("IsFocusScope")
            , RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        FocusedElementProperty = DpProperty::RegisterAttached(_T("FocusedElement"), RTTIType(), Element::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &FocusManager::OnFocusedElementChanged));
    }
}

InputDevice* FocusManager::GetCurrentInputDevice()
{
    return _inputDevice;
}

void FocusManager::SetCurrentInputDevice(InputDevice* inputDevice)
{
    _inputDevice = inputDevice;
}

static DpObject* __GetFocusScope(DpObject* d)
{
    if (d == NULL)
    {
        return NULL;
    }

    if (!(d->GetValue(FocusManager::IsFocusScopeProperty)->ToBool()))
    {
        Element* elem = RTTICast<Element>(d);

        if (elem != NULL && !elem->BlockVisualState())
        {
            Element* parent = elem->GetUIParent();
            if (parent != NULL)
            {
                return FocusManager::GetFocusScope(parent);
            }
        }
    }

    return d;

}

static DpObject* GetRoot(DpObject* d)
{
    if (d == NULL)
    {
        return NULL;
    }

    DpObject* rootObj = NULL;
    Element* reference = RTTICast<Element>(d);

    while (reference != NULL)
    {
        if (reference->BlockVisualState())
        {
            break;
        }

        rootObj = reference;
        reference = VisualTreeOp::GetParent(reference);
    }

    return rootObj;
}

bool FocusManager::GetIsFocusScope(DpObject* d)
{
    return d->GetValue(IsFocusScopeProperty)->ToBool();
}

DpObject* FocusManager::GetFocusScope(DpObject* d)
{
    if (d == NULL)
    {
        return NULL;
    }
    else
    {
        return __GetFocusScope(d);
    }
}

static Element* _GetElement(Object* obj)
{
    Element* v = RTTICast<Element>(obj);
    return v;
}

Element* FocusManager::GetFocusedElement(DpObject* d)
{
    return RTTICast<Element>(GetFocusedElement(d, false));
}

Element* FocusManager::GetFocusedElement(DpObject* d, bool validate)
{
    Element* v = _GetElement(d->GetValue(FocusedElementProperty));

    if (validate && (v != NULL))
    {
        if (VisualTreeOp::GetVisualRoot(d) != VisualTreeOp::GetVisualRoot(v))
        {
            SetFocusedElement(d, NULL);
            v = NULL;
        }
    }

    return v;
}

void FocusManager::SetFocusedElement(DpObject* d, Element* val)
{
    if (d != val)
    {
        d->SetValue(FocusedElementProperty, val);
    }
}

void FocusManager::SetIsFocusScope(DpObject* d, bool val)
{
    d->SetValue(IsFocusScopeProperty, BOOLTOBOOLEAN(val));
}

void FocusManager::OnFocusedElementChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Element* oldValue = _GetElement(e->GetOldValue());
    Element* newValue = _GetElement(e->GetNewValue());

    if (oldValue != NULL)
    {
        oldValue->ClearValue(Element::IsFocusedProperty);
    }

    if (newValue != NULL)
    {
        DpObject* focusedElement = Keyboard::GetFocusedElement();
        newValue->SetValue(Element::IsFocusedProperty, Boolean::True);
        DpObject* fElem = Keyboard::GetFocusedElement();

        if (((focusedElement == fElem) && (newValue != fElem)) && 
            focusedElement != fElem && 
            ((fElem == NULL) || (GetRoot(newValue) == GetRoot(fElem))))
        {
            Keyboard::Focus(newValue);
        }
    }
}

}
