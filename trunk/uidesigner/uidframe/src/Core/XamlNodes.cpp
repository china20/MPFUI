
#include "stdafx.h"

#include <Core/XamlNodes.h>

#include <Framework/Controls/Canvas.h>
#include <Framework/Controls/UniformGrid.h>
#include <Framework/Controls/VisualizeStackPanel.h>

PropertyId::PropertyId(const String& name, DpProperty* dp)
    : _name(name)
    , _prop(dp)
{
}

PropertyId::~PropertyId()
{
}

DpProperty* PropertyId::GetTargetType()
{
    return _prop;
}

String PropertyId::GetName()
{
    return _name;
}

String PropertyId::GetUniqueName()
{
    return _name;
}

PropElement::PropElement(RTTIOfInfo* info, RTTIOfInfo* rttiInfo)
{
    _desingInfo = info;
    _rttiInfo = rttiInfo;
}

IPropertyId* PropElement::Find(const String& name)
{
    IPropertyId* pProp = NULL;
    _props.TryGetValue(name, pProp);
    return pProp;
}

IPropertyId* PropElement::RegisterProperty(const String& name, DpProperty* dp)
{
    IPropertyId* pProp = NULL;
    _props.TryGetValue(name, pProp);
    if (NULL == pProp)
    {
        pProp = new PropertyId(name, dp);
        _props.Add(name, pProp);
    }
    return pProp;
}
