
#ifndef _UIMEMBERVISITOR_H_
#define _UIMEMBERVISITOR_H_

#include <System/Windows/EffectiveValue.h>
#include <System/Windows/FrameworkElement.h>
#include <System/Tools/StyleHelper.h>

namespace suic
{
    
class MemberVisitor
{
public:

    static bool GetMember(Object* obj, String name, ObjectPtr& val)
    {
        return obj->GetMember(name, val);
    }

    static bool SetMember(Object* obj, String name, Object* val)
    {
        return obj->SetMember(name, val);
    }

    static void SetUIParent(Element* parent, Element* child)
    {
        child->_parent = parent;
    }

    static void SetInheritParent(DpObject* parent, DpObject* child)
    {
        child->_contextStorage = parent;
    }

    static void SetParent(Element* parent, FrameworkElement* child)
    {
        child->_parent = parent;
    }

    static void SetTemplateParent(DpObject* parent, FrameworkElement* child)
    {
        child->_templatedParent = parent;
    }

    static void SetValue(DpObject* d, DpProperty* dp, Object* value)
    {
        d->InternalSetValue(dp, value);
    }

    static void SetValue(EffectiveValue* d, Object* value)
    {
        d->SetValue(value);
    }

    static Object* GetValue(EffectiveValue* d)
    {
        return d->GetValue();
    }

    static void SetExprValue(EffectiveValue* d, Object* val, Object* baseValue)
    {
        d->SetExprValue(val, baseValue);
    }

    static void SetAnimatedValue(EffectiveValue* d, Object* val, Object* baseValue)
    {
        d->SetAnimatedValue(val, baseValue);
    }

    static void SetMetadata(EffectiveValue* d, PropMetadata* metadata)
    {
        d->SetMetadata(metadata);
    }

    static ReviseValue* GetReviseValue(EffectiveValue* d)
    {
        return d->GetReviseValue();
    }

    static void ResetAnimatedValue(EffectiveValue* d)
    {
        d->ResetAnimatedValue();
    }

    static void ResetCoercedValue(EffectiveValue* d)
    {
        d->ResetCoercedValue();
    }

    static void SetBaseValueType(EffectiveValue* d, BaseValueType val)
    {
        d->SetBaseValueType(val);
    }

    static void SetPersistId(Element* elem, int id)
    {
        elem->_persistId = id;
    }

    static EntryIndex FindEntryIndex(DpObject* d, int target)
    {
        return d->FindEntryIndex(target);
    }

    static PropMetadata* FindMetadata(DpObject* d, EntryIndex& entryi, DpProperty* dp)
    {
        return d->FindMetadata(entryi, dp);
    }

    static bool GetRawEV(DpObject* d, DpProperty* dp, EffectiveValue& entry)
    {
        return d->GetRawEV(dp, entry);
    }

    static int GetBaseValueSourceByEntryIndex(DpObject* d, EntryIndex& entryi)
    {
        return d->GetBaseValueSourceByEntryIndex(entryi);
    }

    static void SetEV(DpObject* d, EntryIndex entryi, DpProperty* dp, int iTarget, PropMetadata* metadata, Object* val, BaseValueType source)
    {
        d->SetEV(entryi, dp, iTarget, metadata, val, source);
    }

    static void RemoveEV(DpObject* d, EntryIndex entryi, DpProperty* dp)
    {
        if (entryi.IsFound())
        {
            d->RemoveEV(entryi.GetIndex(), dp);
        }
    }

    static Object* GetLocalValue(DpObject* d, Uint32 index)
    {
        return d->GetLocalValue(index);
    }

    static Object* GetFlattenedValue(EffectiveValue* d, RequestFlags val)
    {
        return d->GetFlattenedValue(val);
    }

    static int UpdateEV(DpObject* d, EntryIndex entryi, DpProperty* dp, EffectiveValue& oldEntry, EffectiveValue& newEntry, bool coerceWithDefer, bool coerceWithCurr, OperationType operType)
    {
        return d->UpdateEV(entryi, dp, oldEntry, newEntry, coerceWithDefer, coerceWithCurr, operType);
    }

    static void SetInDesignMode(DpObject* d, bool val)
    {
        if (val)
        {
            d->_viewflag |= 1;
        }
        else
        {
            d->_viewflag &= (~1);
        }
    }

    static bool GetInDesignMode(DpObject* d)
    {
        return ((1 & d->_viewflag) != 0);
    }

    static void SetTemplateCache(FrameworkElement* fe, FrameworkTemplate* newTemplate)
    {
        fe->SetTemplateCache(newTemplate);
        StyleHelper::DoTemplateInvalidations(fe, NULL);
    }
};

}

#endif
