// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
//
// ======================================================================

#include <System/Resources/Style.h>
#include <System/Resources/ResourceDictionary.h>
#include <System/Resources/StaticResource.h>
#include <System/Windows/ResourceReferenceExpression.h>
#include <System/Windows/FrameworkElement.h>

//#include <Framework/Controls/TextBox.h>
#include <Framework/Controls/TextBlock.h>

namespace suic
{

int Style::StyleInstanceCount = 0;

ImplementRTTIOfClass(Style, DpObject)

Style::Style()
    : _setters(NULL)
    , _basedOn(NULL)
    , _targetType(NULL)
    , _resources(NULL)
    , _visualTriggers(NULL)
    , _modified(0)
    , _globalIndex(0)
    , _resDepRecords(NULL)
{
    GetUniqueGlobalIndex(); 
}

Style::Style(RTTIOfInfo* targetType)
    : _setters(NULL)
    , _basedOn(NULL)
    , _targetType(targetType)
    , _resources(NULL)
    , _visualTriggers(NULL)
    , _modified(0)
    , _globalIndex(0)
    , _resDepRecords(NULL)
{ 
    SetTargetType(targetType);
    GetUniqueGlobalIndex(); 
}

Style::Style(RTTIOfInfo* targetType, Style* basedOn)
    : _setters(NULL)
    , _basedOn(NULL)
    , _targetType(targetType)
    , _resources(NULL)
    , _resDepRecords(NULL)
    , _visualTriggers(NULL)
    , _modified(0)
    , _globalIndex(0)
{ 
    SetTargetType(targetType);
    SetBasedOn(basedOn);

    GetUniqueGlobalIndex(); 
}

Style::~Style()
{
    if (_setters)
    {
        delete _setters;
    }
    if (_visualTriggers)
    {
        delete _visualTriggers;
    }
    if (_resources)
    {
        delete _resources;
    }

    FREEREFOBJ(_basedOn);
}

TriggerCollection* Style::GetTriggers()
{ 
    VerifyAccess(__FILELINE__);

    if (_visualTriggers == NULL) 
    { 
        _visualTriggers = new TriggerCollection();

        if (IsSealed())
        { 
            _visualTriggers->Seal();
        } 
    }

    return _visualTriggers;
}

SetterCollection* Style::GetSetters()
{ 
    VerifyAccess(__FILELINE__);

    if (_setters == NULL) 
    { 
        _setters = new SetterCollection();

        if (IsSealed())
        { 
            _setters->Seal();
        } 
    }
    return _setters;
}

ResourceDictionary* Style::GetResources()
{ 
    VerifyAccess(__FILELINE__); 

    if (_resources == NULL) 
    { 
        _resources = new ResourceDictionary();
        
        if (IsSealed()) 
        { 
            _resources->SetIsReadOnly(true);
        } 
    }
    return _resources;
}

void Style::SetResources(ResourceDictionary* value)
{
    VerifyAccess(__FILELINE__); 

    if (IsSealed()) 
    {
        throw InvalidOperationException(_U("has sealed!"), __FILELINE__);
    }

    _resources = value;
}

Object* Style::FindRes(const String& resourceKey)
{
    Object* retVal = DpProperty::UnsetValue();
    if (_resources != NULL)
    {
        retVal = _resources->Lookup(resourceKey); 
    }

    if (retVal == DpProperty::UnsetValue() && _basedOn != NULL)
    {
        retVal = _basedOn->FindRes(resourceKey);
    }

    return retVal;
}

ResourceDictionary* Style::FindResourceDictionary(const String& resourceKey) 
{
    if (_resources != NULL && _resources->Contains(resourceKey)) 
    {
        return _resources; 
    }
    else if (_basedOn != NULL)
    {
        return _basedOn->FindResourceDictionary(resourceKey); 
    }
    else
    {
        return NULL; 
    }
}

void Style::AddSetter(Setter* value)
{ 
    VerifyAccess(__FILELINE__); 

    if (value == NULL)
    { 
        throw new ArgumentNullException(_U("value"), __FILELINE__);
    }

    GetSetters()->AddSetter(value);
}

bool Style::HasResourceReferences()
{
    return false;//GetResourceDependents()->GetCount() > 0; 
}

void Style::CheckTargetType(Object* element)
{
    if (NULL != GetTargetType())
    {
        RTTIOfInfo* c = element->GetRTTIType();
        if (/*c != TextBlock::RTTIType() && 
            c != UIEdit::RTTIType() && */
            !c->InheritFrom(GetTargetType()))
        {
            throw InvalidOperationException(_U("StyleTargetTypeMismatchWithElement"), __FILELINE__);
        }
    }
}

void Style::GetUniqueGlobalIndex() 
{ 
    StyleInstanceCount++;
    _globalIndex = StyleInstanceCount;
}

Style* Style::GetBasedOn()
{
    return _basedOn;
}

void Style::CheckForCircularBasedOnReferences()
{
    /*Stack stack(10);

    for (Style* style = this; style != NULL; style = style->GetBasedOn())
    {
        if (stack.Contains(style))
        {
            throw InvalidOperationException(_U("StyleBasedOnHasLoop"));
        }
        stack.Push(style);
    }*/
}

void Style::Seal()
{
    VerifyAccess(__FILELINE__);

    if (!IsSealed())
    {
        if (_targetType == NULL)
        {
            throw InvalidOperationException(_U("NullPropertyIllegal"), __FILELINE__);
        }

        if (_setters != NULL)
        {
            _setters->Seal();
        }

        if (_visualTriggers != NULL)
        {
            _visualTriggers->Seal();
        }

        CheckForCircularBasedOnReferences();

        if (_basedOn != NULL)
        {
            _basedOn->Seal();
        }

        if (_resources != NULL)
        {
            _resources->SetIsReadOnly(true);
        }

        ProcessSetters(this);
        ProcessSelfStyles(this);
        ProcessVisualTriggers(this);

        DpObject::Seal();
 
        //DetachFromDispatcher();
    }
}

void Style::ProcessBasedOnStyle()
{
    if (NULL == _basedOn)
    {
        return;
    }

    for (int i = 0; i < _basedOn->GetSetters()->GetCount(); i++)
    {
        Setter* setter = _basedOn->GetSetters()->GetSetter(i);
        if (setter != NULL && NULL == GetSetters()->GetSetter(setter->GetProp()))
        {
            GetSetters()->AddSetter(setter);
        }
    }

    for (int j = 0; j < _basedOn->GetTriggers()->GetCount(); ++j)
    {
        TriggerBase* trigger = _basedOn->GetTriggers()->GetTrigger(j);
        if (trigger != NULL)
        {
            GetTriggers()->AddTrigger(trigger);
        }
    }
}

void Style::ProcessSelfStyles(Style* style)
{
    if (style != NULL)
    {
        ProcessSelfStyles(style->_basedOn);
        for (int i = 0; i < style->_propValues.Length(); i++)
        {
            PropValue* propValue = style->_propValues[i];
            AddContainerDependent(propValue->prop, false, _elementDependents);
        }
    }
}

void Style::ProcessSetters(Style* style)
{
    if (style != NULL)
    {
        style->GetSetters()->Seal();
        if (style == this)
        {
            for (int i = 0; i < style->GetSetters()->GetCount(); i++)
            {
                Setter* setter = style->GetSetters()->GetSetter(i);
                if (setter != NULL)
                {
                    if (!setter->GetTargetName().Empty())
                    {
                        throw InvalidOperationException(_U("SetterOnStyleNotAllowedToHaveTarget"), __FILELINE__);
                    }

                    RRExpression* val = RTTICast<RRExpression>(setter->GetRawValue());
                    if (val == NULL)
                    {
                        UpdatePropertyValueList(setter->GetProp(), PropValType::pvtSet, setter->GetRawValue());
                    }
                    else
                    {
                        UpdatePropertyValueList(setter->GetProp(), PropValType::pvtResource, val);
                    }
                }
            }
        }

        ProcessSetters(style->_basedOn);
    }
}

void Style::ProcessVisualTriggers(Style* style)
{
    if (style != NULL)
    {
        ProcessVisualTriggers(style->_basedOn);

        if (style->_visualTriggers != NULL)
        {
            int count = style->_visualTriggers->GetCount();
            for (int i = 0; i < count; i++)
            {
                TriggerBase* trigger = style->_visualTriggers->GetTrigger(i);

                for (int j = 0; j < trigger->_propValues.Length(); j++)
                {
                    PropValue* propValue = trigger->_propValues[j];
                    if (propValue->childName.Equals(_U("~Self")))
                    {
                        throw InvalidOperationException(_U("StyleTriggersCannotTargetTheTemplate"), __FILELINE__);
                    }
     
                    AddContainerDependent(propValue->prop, true, _elementDependents);
                }
            }
        }
    }
}

void Style::UpdatePropertyValueList(DpProperty* dp, PropValType valueType, Object* value)
{
    int num = -1;

    if (dp != NULL)
    {
        for (int i = 0; i < _propValues.Length(); i++)
        {
            if (_propValues[i]->prop == dp)
            {
                num = i;
                break;
            }
        }

        if (num >= 0)
        {
            PropValue* propVal = _propValues[num];
            propVal->valueType = valueType;
            propVal->SetValue(value);
        }
        else
        {
            PropValue* propVal = new PropValue();

            propVal->valueType = valueType;
            propVal->childName = _U("~Self");
            propVal->prop = dp;
            propVal->SetValue(value);
            _propValues.Add(propVal);
        }
    }
}

void Style::AddContainerDependent(DpProperty* dp, bool fromTrigger, Array<ContainerDep, false>& deps)
{
    if (dp != NULL)
    {
        ContainerDep dependent;
        for (int i = 0; i < deps.Length(); i++)
        {
            dependent = deps[i];
            if (dp == dependent.prop)
            {
                deps[i].fromTrigger |= fromTrigger;
                return;
            }
        }

        dependent.prop = dp;
        dependent.fromTrigger = fromTrigger;
        deps.Add(dependent);
    }
}

}
