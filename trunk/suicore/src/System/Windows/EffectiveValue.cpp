// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

#include <System/Types/Structure.h>

#include <System/Windows/DpProperty.h>
#include <System/Windows/Exception.h>
#include <System/Windows/Expression.h>
#include <System/Windows/DeferredRef.h>
#include <System/Windows/EffectiveValue.h>

#include <System/Windows/MemberVisitor.h>

namespace suic
{

ImplementRTTIOfClass(ReviseValue, Object)

EntryIndex::EntryIndex()
{
    _store = 0;
}

EntryIndex::EntryIndex(Uint32 index)
{
    _store = index | 0x80000000;
}

EntryIndex::EntryIndex(Uint32 index, bool found)
{
    _store = index & 0x7fffffff;
    if (found)
    {
        _store |= 0x80000000;
    }
}

bool EntryIndex::IsFound()
{
    return ((_store & 0x80000000) != 0);
}

Uint32 EntryIndex::GetIndex()
{
    return (_store & 0x7fffffff);
}

ReviseValue::ReviseValue()
{
    _baseValue = NULL;
    _coercedValue = NULL;
    _animatedValue = NULL; 
    _expressionValue = NULL;
}

ReviseValue::~ReviseValue()
{
    FREEREFOBJ(_baseValue);
    FREEREFOBJ(_coercedValue);
    FREEREFOBJ(_animatedValue);
    FREEREFOBJ(_expressionValue);
}

void ReviseValue::SetBaseValue(Object* val)
{
    SETREFOBJ(_baseValue, val);

    if (DynamicCast<ReviseValue>(val))
    {
        throw InvalidValueException(_U("BaseValue can not be ReviseValue"), __FILELINE__);
    }
}

void ReviseValue::SetExprValue(Object* val)
{ 
    SETREFOBJ(_expressionValue, val);
}

void ReviseValue::SetAnimatedValue(Object* val)
{
    SETREFOBJ(_animatedValue, val);
}

EffectiveValue::EffectiveValue()
    : _prop(NULL)
    , _propIndex(0)
    , _value(DpProperty::UnsetValue())
    , _source(BaseValueType::btUnknown)
    , _meta(NULL)
{
    _value->ref();
}

EffectiveValue::EffectiveValue(const EffectiveValue& Other)
{
    _meta = Other._meta;
    _prop = Other._prop;
    _source = Other._source;
    _value = Other._value;
    _propIndex = Other._propIndex;
    _value->ref();
}

EffectiveValue::EffectiveValue(DpProperty* prop, int bvType)
{
    SetPropertyAndIndex(prop, 0);
    _value = DpProperty::UnsetValue();
    _source = bvType;
    _meta = NULL;
    _value->ref();

#ifdef _DEBUG
    if (prop == NULL)
    {
        prop = NULL;
    }
#endif
}

EffectiveValue::EffectiveValue(DpProperty* dp, Object* val, int bvType)
{
#ifdef _DEBUG
    if (val == NULL)
    {
        throw;
    }
    if (dp == NULL)
    {
        dp = NULL;
    }
#endif
    SetPropertyAndIndex(dp, 0);
    _value = val;
    _source = bvType;//BaseValueType::btDefault;
    _meta = NULL;

    if (NULL == _value)
    {
        _value = DpProperty::UnsetValue();
    }

    _value->ref();
}

EffectiveValue::~EffectiveValue()
{
    _value->unref();
}

void EffectiveValue::Reset()
{
    _value->unref();
    _value = DpProperty::UnsetValue();
    _source = BaseValueType::btUnknown;
    _meta = NULL;
    _prop = NULL;
    _propIndex = 0;
    _value->ref();
}

void EffectiveValue::ResetEV(DpProperty* dp, Object* val, int bvType)
{
#ifdef _DEBUG
    if (val == NULL)
    {
        throw;
    }
    if (dp == NULL)
    {
        dp = NULL;
    }
#endif
    SetPropertyAndIndex(dp, 0);

    _value->unref();

    if (NULL == val)
    {
        _value = DpProperty::UnsetValue();
    }
    else
    {
        _value = val;
    }

    _source = bvType;
    _meta = NULL;
    _value->ref();
}

EffectiveValue& EffectiveValue::operator=(const EffectiveValue& Other)
{
    if (&Other != this)
    {
        _prop = Other._prop;
        _propIndex = Other._propIndex;
        _source = Other._source;
        _meta = Other._meta;
        SETREFOBJ(_value, Other._value)
    }

    return *this;
}

Object* EffectiveValue::GetValue() const
{
    return _value;
}

Object* EffectiveValue::GetLocalValue()
{
    if (GetBaseValueType() == BaseValueType::btLocal)
    {
        if (!HasRevise())
        {
            return _value;
        }
        else
        {
            ReviseValue* revise(dynamic_cast<ReviseValue*>(_value));
            return revise->GetBaseValue();
        }
    }
    else
    {
        return DpProperty::UnsetValue();
    }
}

void EffectiveValue::SetLocalValue(Object* val)
{
    if (!HasRevise())
    {
        SETREFOBJ(_value, val);
    }
    else
    {
        ReviseValue* reviseValue(DynamicCast<ReviseValue>(_value));
        reviseValue->SetBaseValue(val);
    }
}

void EffectiveValue::SetValue(Object* val)
{
#ifdef _DEBUG
    if (val == NULL)
    {
        throw InvalidOperationException(_U("Effective value is not null"), __FILELINE__);
    }
#endif
    if (_value != val)
    {
        SETREFOBJ(_value, val);
        //SetIsDeferredReference(DynamicCast<DeferredRef>(val) != NULL);
    }
}

Object* EffectiveValue::GetFlattenedValue(RequestFlags requests)
{
    Object* flatVal = NULL;
    //
    // 值没有任何修改
    //
    if (!HasRevise())    
    {
        return GetValue();
    }

    ReviseValue* reviseValue = NULL;

    if (IsCoerced())
    {
        reviseValue = GetReviseValue();

        if ((requests & RequestFlags::rfCoercionBaseValue) == RequestFlags::rfFullyResolved || IsCoercedWithCurrentValue())
        {
            flatVal = reviseValue->GetCoercedValue();
        }
        else if (IsAnimated() && ((requests & RequestFlags::rfAnimationBaseValue) == RequestFlags::rfFullyResolved))
        {
            flatVal = reviseValue->GetAnimatedValue();
        }
        else if (IsExpression())
        {
            flatVal = reviseValue->GetExprValue();
        }
        else
        {
            flatVal = reviseValue->GetBaseValue();
        }
    }
    else if (IsAnimated())
    {
        reviseValue = GetReviseValue();

        if ((requests & RequestFlags::rfAnimationBaseValue) == RequestFlags::rfFullyResolved)
        {
            flatVal = reviseValue->GetAnimatedValue();
        }
        else if (IsExpression())
        {
            flatVal = reviseValue->GetExprValue();
        }
        else
        {
            flatVal = reviseValue->GetBaseValue();
        }
    }
    else if (IsExpression())
    {
        flatVal = GetReviseValue()->GetExprValue();
    }
    else
    {
        flatVal = GetValue();
    }

    return flatVal;
}

void EffectiveValue::FindFlattenedEntry(RequestFlags requests, EffectiveValue& entry)
{
    entry.SetBaseValueType(GetBaseValueType());
    entry._prop = _prop;
    entry._propIndex = _propIndex;
    entry._meta = _meta;
    entry.SetValue(GetFlattenedValue(requests));

    //
    // 值没有任何修改
    //
    /*if (((short)(_source & (ValueSourceType::ModifiersMask))) == 0)
    {
        return *this;
    }

    EffectiveValue entry;

    entry.SetBaseValueType(GetBaseValueType());
    entry._prop = _prop;
    entry._propIndex = _propIndex;
    //entry.SetIsDeferredReference(IsDeferredReference());

    ReviseValue* reviseValue = NULL;

    if (IsCoerced())
    {
        reviseValue = GetReviseValue();

        if ((requests & RequestFlags::CoercionBaseValue) == RequestFlags::FullyResolved || IsCoercedWithCurrentValue())
        {
            entry.SetValue(reviseValue->GetCoercedValue());
            return entry;
        }

        if (IsAnimated() && ((requests & RequestFlags::AnimationBaseValue) == RequestFlags::FullyResolved))
        {
            entry.SetValue(reviseValue->GetAnimatedValue());
            return entry;
        }

        if (IsExpression())
        {
            entry.SetValue(reviseValue->GetExprValue());
            return entry;
        }

        entry.SetValue(reviseValue->GetBaseValue());
        return entry;
    }

    if (IsAnimated())
    {
        reviseValue = GetReviseValue();

        if ((requests & RequestFlags::AnimationBaseValue) == RequestFlags::FullyResolved)
        {
            entry.SetValue(reviseValue->GetAnimatedValue());
            return entry;
        }

        if (IsExpression())
        {
            entry.SetValue(reviseValue->GetExprValue());
            return entry;
        }

        entry.SetValue(reviseValue->GetBaseValue());

        return entry;
    }

    if (IsExpression())
    {
        reviseValue = GetReviseValue();
        entry.SetValue(reviseValue->GetExprValue());

        return entry;
    }

    entry.SetValue(GetValue());*/

    //return entry;
}

void EffectiveValue::ResetValue(Object* val, bool flag)
{
    SETREFOBJ(_value, val);
    _source = BaseValueType::btLocal;
}

ReviseValue* EffectiveValue::EnsureReviseValue() 
{
    ReviseValue* reviseValue = NULL;

    if (!_value)
    {
        reviseValue = new ReviseValue();
        _value = reviseValue;
        reviseValue->ref();
    }
    else 
    {
        reviseValue = dynamic_cast<ReviseValue*>(_value); 

        if (!reviseValue)
        {
            reviseValue = new ReviseValue();
            reviseValue->ref();
            reviseValue->SetBaseValue(_value);
            _value->unref();
            _value = reviseValue;
        } 
    }

    return reviseValue;
}

void EffectiveValue::ResetDefaultEV(DpProperty* dp, Object* value)
{
    ResetEV(dp, value, BaseValueType::btDefault);
}

ReviseValue* EffectiveValue::GetReviseValue()
{
    if (_value->GetRTTIType() == ReviseValue::RTTIType())
    {
        return (ReviseValue*)(_value);
    }
    else
    {
        return NULL;
    }
}

Object* EffectiveValue::GetBaseValue()
{
    if (HasRevise())
    {
        return GetReviseValue()->GetBaseValue();
    }
    else
    {
        return DpProperty::UnsetValue();
    }
}

void EffectiveValue::SetExprValue(Object* val, Object* baseVal)
{
    ReviseValue* reviseValue = EnsureReviseValue();
    reviseValue->SetExprValue(val);

    SetIsExpression(true);
    //SetIsDeferredReference(DynamicCast<DeferredRef>(val) != NULL);
}

void EffectiveValue::SetAnimatedValue(Object* val, Object* baseVal)
{
    ReviseValue* reviseValue = EnsureReviseValue();
    reviseValue->SetAnimatedValue(val);

    SetIsAnimated(true);
    //SetIsDeferredReference(DynamicCast<DeferredRef>(val) != NULL);
}

void EffectiveValue::SetCoercedValue(Object* val, bool coerceWithCurrentValue)
{
    EnsureReviseValue()->SetCoercedValue(val);
    SetIsCoerced(true);
    SetIsCoercedWithCurrentValue(coerceWithCurrentValue);

    if (coerceWithCurrentValue)
    {
        SetIsDeferredReference(DynamicCast<DeferredRef>(val) != NULL);
    }
    else
    {
        SetIsDeferredReference(false);
    }
}

void EffectiveValue::SetCurrentValue(Object* val)
{
    if (val)
    {
        if (!HasRevise())
        {
            SETREFOBJ(_value, val);
        }
        else
        {
            ReviseValue* reviseValue = EnsureReviseValue();
            reviseValue->SetCoercedValue(val);
            _source |= ValueSourceType::IsCoerced | ValueSourceType::IsCoercedWithCurrentValue;
        }
    }
}

void EffectiveValue::ComputeIsDeferred()
{
    /*bool flag = false;

    if (!HasRevise())
    {
        flag = DynamicCast<DeferredRef>(_value) != NULL;
    }
    else
    {
        ReviseValue* reviseValue = DynamicCast<ReviseValue>(_value);
        if (IsCoercedWithCurrentValue())
        {
            flag = DynamicCast<DeferredRef>(reviseValue->GetCoercedValue()) != NULL;
        }
        else if (IsExpression())
        {
            flag = DynamicCast<DeferredRef>(reviseValue->GetExprValue()) != NULL;
        }
    }

    SetIsDeferredReference(false);*/
}


void EffectiveValue::CheckReviseValue(ReviseValue* reviseValue)
{
    if (!HasRevise())
    {
        _value = reviseValue->GetBaseValue();
        ADDREFOBJ(_value);
        reviseValue->unref();
    }
}

void EffectiveValue::ResetCoercedValue()
{
    if (IsCoerced())
    {
        ReviseValue* reviseValue = GetReviseValue();
        reviseValue->SetCoercedValue(NULL);
        SetIsCoerced(false);

        if (!HasRevise())
        {
            CheckReviseValue(reviseValue);
        }
        else
        {
            ComputeIsDeferred();
        }
    }
}

void EffectiveValue::ResetAnimatedValue()
{
    if (IsAnimated())
    {
        ReviseValue* reviseValue = GetReviseValue();
        reviseValue->SetAnimatedValue(NULL);
        SetIsAnimated(false);

        if (!HasRevise())
        {
            CheckReviseValue(reviseValue);
        }
        else
        {
            ComputeIsDeferred();
        }
    }
}

void EffectiveValue::SetAnimationBaseValue(Object* animationBaseValue)
{
    if (!HasRevise())
    {
        SETREFOBJ(_value, animationBaseValue);
    }
    else
    {
        ReviseValue* reviseValue = GetReviseValue();
        if (IsExpression())
        {
            reviseValue->SetExprValue(animationBaseValue);
        }
        else
        {
            reviseValue->SetBaseValue(animationBaseValue);
        }
        
        ComputeIsDeferred();
    }
}

void EffectiveValue::Clear()
{
    _source = 0;
    _prop = NULL;
    SETREFOBJ(_value, DpProperty::UnsetValue());
}

}
