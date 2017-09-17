
#include <Framework/Controls/Control.h>
#include <Framework/Controls/ControlTemplate.h>
#include <Framework/Controls/Application.h>

namespace suic 
{

ImplementRTTIOfClass(ControlTemplate, FrameworkTemplate)

ControlTemplate::ControlTemplate()
    : _targetType(NULL)
    , _triggers(NULL)
    , _resources(NULL)
{
} 

ControlTemplate::ControlTemplate(RTTIOfInfo* targetType) 
{
    ValidateTargetType(targetType, _U("targetType"));
    _targetType = targetType;
    _triggers = NULL;
    _resources = NULL;
}

ControlTemplate::~ControlTemplate()
{
    if (_triggers)
    {
        delete _triggers;
    }

    if (_resources)
    {
        _resources->unref();
    }
}

TriggerCollection* ControlTemplate::GetTriggers()
{
    return _triggers;
}

TriggerCollection* ControlTemplate::GetTriggersAndCreate()
{
    if (NULL == _triggers)
    {
        _triggers = new TriggerCollection();
    }
    return _triggers;
}

ResourceDictionary* ControlTemplate::GetResources()
{
    if (NULL == _resources)
    {
        _resources = new ResourceDictionary();
        _resources->ref();
    }
    return _resources;
}

void ControlTemplate::ValidateTargetType(RTTIOfInfo* targetType, const String& argName)
{ 
    if (targetType == NULL)
    {
        throw ArgumentNullException(argName, __FILELINE__);
    } 
    if (!targetType->InheritFrom(Control::RTTIType())) 
    {
        throw ArgumentException(targetType->typeName, _U("InvalidControlTemplateTargetType"), __FILELINE__);
    }
}

void ControlTemplate::Seal()
{
    if (!IsSealed())
    {
        ProcessVisualTriggers();
        FrameworkTemplate::Seal();
    }
}

void ControlTemplate::AddContainerDependent(DpProperty* dp, bool fromTrigger, Array<ContainerDep, false>& deps)
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

void ControlTemplate::ProcessVisualTriggers()
{
    if (NULL != _triggers)
    {
        int count = _triggers->GetCount();

        for (int i = 0; i < count; i++)
        {
            TriggerBase* trigger = _triggers->GetTrigger(i);

            for (int j = 0; j < trigger->_propValues.Length(); j++)
            {
                PropValue* propValue = trigger->_propValues[j];
                if (propValue->childName.Equals(_U("~Self")))
                {
                    throw InvalidOperationException(_U("ControlTemplateTriggersCannotTargetTheTemplate"), __FILELINE__);
                }

                AddContainerDependent(propValue->prop, true, _elementDependents);
            }
        }
    }
}

}
