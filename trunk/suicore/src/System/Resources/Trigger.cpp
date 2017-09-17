// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
//
// ======================================================================

#include <System/Resources/Trigger.h>
#include <System/Resources/StaticResource.h>
#include <System/Windows/ResourceReferenceExpression.h>

namespace suic
{

ImplementRTTIOfClass(TriggerBase, DpObject)
ImplementRTTIOfClass(Trigger, TriggerBase)
ImplementRTTIOfClass(MultiTrigger, TriggerBase)

Trigger::Trigger()
    : _setters(NULL)
    , _value(NULL)
    , _prop(NULL)
{
}

Trigger::~Trigger()
{
    if (_setters)
    {
        delete _setters;
    }
    if (_value)
    {
        _value->unref();
    }
}

void Trigger::SetProperty(DpProperty* val)
{
    VerifyAccess(__FILELINE__);

    if (IsSealed())
    { 
        throw InvalidOperationException(_U(""), _U("CannotChangeAfterSealed"), __FILELINE__);
    } 

    _prop = val;
} 

void Trigger::SetValue(Object* val)
{
    VerifyAccess(__FILELINE__); 

    if (IsSealed()) 
    { 
        throw InvalidOperationException(_U(""), _U("CannotChangeAfterSealed"), __FILELINE__);
    } 

    SETREFOBJ(_value, val); 
}

void Trigger::SetSourceName(String val)
{ 
    VerifyAccess(__FILELINE__); 

    if (IsSealed())
    { 
        throw InvalidOperationException(_U("SourceName"), _U("CannotChangeAfterSealed"), __FILELINE__);
    }
    _sourceName = val; 
}

SetterCollection* Trigger::GetSetters() 
{
    VerifyAccess(__FILELINE__); 

    if (_setters == NULL)
    {
        _setters = new SetterCollection(); 
    }
    return _setters; 
}

void Trigger::AddSetter(Setter* value) 
{
    VerifyAccess(__FILELINE__);
    GetSetters()->AddSetter(value);
}

void Trigger::Seal()
{
    VerifyAccess(__FILELINE__);

    if (!IsSealed())
    {
        if ((_prop != NULL) && !_prop->IsValidValue(this, _value))
        {
            throw InvalidOperationException(_T("InvalidPropertyValue"), __FILELINE__);
        }

        ProcessSettersCollection(_setters);
    }

    DpObject::Seal();
}

TriggerBase::TriggerBase()
{

}

TriggerBase::~TriggerBase()
{
    for (int i = 0; i < _propValues.Length(); ++i)
    {
        delete _propValues[i];
    }
}

void TriggerBase::ProcessSettersCollection(SetterCollection* setters)
{
    if (setters != NULL)
    {
        setters->Seal();
        for (int i = 0; i < setters->GetCount(); i++)
        {
            Setter* setter = setters->GetSetter(i);
            if (setter == NULL)
            {
                throw InvalidOperationException(_U("VisualTriggerSettersIncludeUnsupportedSetterType"), __FILELINE__);
            }

            DpProperty* dp = setter->GetProp();
            Object* val = setter->GetRawValue();
            String targetName = setter->GetTargetName();

            if (targetName.Empty())
            {
                //targetName = "~Self";
            }
  
            RRExpression* extension = DynamicCast<RRExpression>(val);

            if (extension == NULL)
            {
                AddToPropertyValues(targetName, dp, val, PropValType::pvtTrigger);
            }
            else
            {
                AddToPropertyValues(targetName, dp, extension, PropValType::pvtPropTriggerRes);
            }
        }
    }
}

void TriggerBase::Seal()
{
    DpObject::Seal();
}

void TriggerBase::AddToPropertyValues(String childName, DpProperty* dp, Object* value, PropValType valueType)
{
    PropValue* propVal = new PropValue();

    propVal->valueType = valueType;
    propVal->childName = childName;
    propVal->prop = dp;
    propVal->SetValue(value);

    _propValues.Add(propVal);
}

SetterCollection* TriggerBase::GetSetters()
{
    return NULL;
}

bool TriggerBase::IsMatch(DpObject* d)
{
    return false;
}

bool TriggerBase::CheckProperty(DpProperty* dp)
{
    return false;
}

void TriggerBase::MatchValue(DpObject* d, DpProperty* dp, Object* oldVal, Object* newVal, bool& bOld, bool& bNew)
{
}

bool Trigger::IsMatch(DpObject* d)
{
    Object* val = d->GetValue(GetProp());
    return GetValue()->Equals(val);
}

bool Trigger::CheckProperty(DpProperty* dp)
{
    return GetProp() == dp;
}

void Trigger::MatchValue(DpObject* d, DpProperty* dp, Object* oldVal, Object* newVal, bool& bOld, bool& bNew)
{
    if (dp == _prop)
    {
        bOld = (_value == oldVal || _value->Equals(oldVal));
        bNew = (_value == newVal || _value->Equals(newVal));
    }
    else
    {
        Object* val = d->GetValue(_prop);
        bOld = false;
        bNew = (val == _value || _value->Equals(val));
    }
}

//===============================================================
//
MultiTrigger::MultiTrigger()
    : _setters(NULL)
    , _oper(ConditionOper::coAnd)
{

}

MultiTrigger::~MultiTrigger()
{
    if (_setters)
    {
        delete _setters;
    }
}

SetterCollection* MultiTrigger::GetSetters()
{
    if (NULL == _setters)
    {
        _setters = new SetterCollection();
    }

    return _setters;
}

void MultiTrigger::SetConditionOper(MultiTrigger::ConditionOper oper)
{
    if (!IsSealed())
    {
        _oper = oper;
    }
}

MultiTrigger::ConditionOper MultiTrigger::GetConditionOper() const
{
    return _oper;
}

ArrayCondition* MultiTrigger::GetConditions()
{
    return &_conds;
}

bool MultiTrigger::IsMatch(DpObject* d)
{
    Array<Condition, true>* arrCond = GetConditions();

    for (int i = 0; i < arrCond->Length(); ++i)
    {
        Condition& cond = arrCond->GetItem(i);
        Object* val = d->GetValue(cond.prop);

        if (!cond.GetValue()->Equals(val))
        {
            return false;
        }
    }

    return true;
}

bool MultiTrigger::CheckProperty(DpProperty* dp)
{
    ArrayCondition* arrCond = GetConditions();

    for (int i = 0; i < arrCond->Length(); ++i)
    {
        Condition& cond = arrCond->GetItem(i);

        if (cond.prop == dp)
        {
            return true;
        }
    }
    return false;
}

void MultiTrigger::MatchValue(DpObject* d, DpProperty* dp, Object* oldVal, Object* newVal, bool& bOld, bool& bNew)
{
    bool bFind = false;
    bOld = false;
    bNew = true;

    for (int i = 0; i < _conds.Length(); ++i)
    {
        Condition& cond = _conds[i];
        Object* condVal = cond.GetValue();

        if (dp == cond.prop)
        {
            bOld = condVal->Equals(oldVal);

            if (!bFind)
            {
                bNew = condVal->Equals(newVal);
                if (bNew && _oper == ConditionOper::coOr)
                {
                    break;
                }
            }
        }
        else if (!bFind)
        {
            Object* val = d->GetValue(_conds[i].prop);

            if (!condVal->Equals(val))
            {
                bOld = false;
                bNew = false;
                if (_oper == ConditionOper::coAnd)
                {
                    return;
                }
            }
            else if (_oper == ConditionOper::coOr)
            {
                bNew = true;
                bFind = true;
            }
        }
    }
}

void MultiTrigger::Seal()
{
    if (!TriggerBase::IsSealed())
    {
        ProcessSettersCollection(_setters);
        TriggerBase::Seal();
    }
}

}
