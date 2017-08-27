
#include <System/Tools/AcceleratorOp.h>
#include <System/Tools/HwndHelper.h>
#include <System/Windows/CoreTool.h>

#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>
#include <System/Interop/InternalWindowOper.h>

namespace suic
{

static StringDic<AcceleratorOp::ArrayElement*> g_keyElems;

AccessKeyEventArg::AccessKeyEventArg(Uint16 modifier, const String& key, bool isMultiple)
    : _modifier(modifier)
    , _key(key)
    , _isMultiple(isMultiple)
{

}

String AccessKeyEventArg::GetKey() const
{
    return _key;
}

bool AccessKeyEventArg::IsMultiple() const
{
    return _isMultiple;
}

Uint16 AccessKeyEventArg::GetModifier() const
{
    return _modifier;
}

AcceleratorOp::AcceleratorOp()
{
}

void AcceleratorOp::Register(Uint16 modifier, const String& key, Element* elem)
{
    ArrayElement* arrElems = NULL;
    String strKey = NormalizeKey(modifier, key);
    if (!g_keyElems.TryGetValue(strKey, arrElems))
    {
        arrElems = new ArrayElement();
        g_keyElems.Add(strKey, arrElems);
    }

    if (!PurgeDead(arrElems, elem))
    {
        arrElems->Add(suic::WeakRef(elem));
    }
}

void AcceleratorOp::Unregister(Uint16 modifier, const String& key, Element* elem)
{
    ArrayElement* arrElems = NULL;
    String strKey = NormalizeKey(modifier, key);
    if (g_keyElems.TryGetValue(strKey, arrElems))
    {
        for (int i = 0; i < arrElems->GetCount(); ++i)
        {
            WeakRef& weakElem = arrElems->GetItem(i);
            if (weakElem.GetTarget() == elem)
            {
                arrElems->RemoveAt(i);
                break;
            }
        }

        if (arrElems->GetCount() == 0)
        {
            g_keyElems.Remove(strKey);
        }
    }

    PurgeDead(arrElems, elem);
}

bool AcceleratorOp::ProcessKeyDown(Element* rootElem, KeyboardEventArg* e)
{
    return false;
}

bool AcceleratorOp::ProcessTextInput(Element* rootElem, KeyboardEventArg* e)
{
    if (e->GetModifierKey() == 0)
    {
        return false;
    }

    String key;
    String strKey;
    
    key.Format(_U("%c"), e->GetKey() + 96);
    strKey = NormalizeKey(e->GetModifierKey(), key);

    Element* targetElem = NULL;
    ArrayElement* arrElems = NULL;

    if (g_keyElems.TryGetValue(strKey, arrElems))
    {
        for (int i = 0; i < arrElems->GetCount(); ++i)
        {
            Element* currElem = arrElems->GetItem(i).GetElement<Element>();
            if (currElem != NULL && rootElem->IsAncestorOf(currElem))
            {
                targetElem = currElem;
                targetElem->ref();
                break;
            }
        }
    }

    if (targetElem != NULL)
    {
        AccessKeyEventArg ae(e->GetModifierKey(), key, false);
        targetElem->OnAccessKey(&ae);
        targetElem->unref();
    }

    return false;
}

String AcceleratorOp::NormalizeKey(Uint16 modifier, const String& key)
{
    String name;
    name.Format(_U("%s_%d"), key.c_str(), modifier);
    return name;
}

bool AcceleratorOp::PurgeDead(ArrayElement* elems, Element* elem)
{
    bool bExist = false;

    for (int i = 0; i < elems->GetCount(); ++i)
    {
        WeakRef& weakElem = elems->GetItem(i);
        if (!weakElem.IsAlive())
        {
            elems->RemoveAt(i);
            --i;
            continue;
        }

        if (weakElem.GetTarget() == elem)
        {
            bExist = true;
        }
    }

    return bExist;
}

}
