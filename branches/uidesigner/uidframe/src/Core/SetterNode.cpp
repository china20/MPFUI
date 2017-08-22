/////////////////////////////////////////////////////
// SetterNode.cpp

#include "stdafx.h"

#include <Core/SetterNode.h>
#include <Core/TypeManager.h>

ImplementRTTIOfClass(SetterNode, XamlNodeItem)

SetterNode::SetterNode()
{
    _prop = NULL;
    _dpItem = NULL;
    _fromTrigger = ePropertyType::ePropStyle;
}

SetterNode::SetterNode(const String& name, DpProperty* dp, Object* val)
{
    int iDotPos = name.IndexOf(_U("."));
    if (-1 == iDotPos || dp == NULL)
    {
        _name = name;
    }
    else
    {
        _name = name.Substring(iDotPos + 1);
    }

    _prop = dp;
    _fromTrigger = ePropertyType::ePropStyle;
    ResNode::CreateResNode(val, _value);
    _dpItem = NULL;
    _oldValue = val;
}

SetterNode::~SetterNode()
{
}

void SetterNode::CloneSetterNode(SetterNode* pSetter)
{
    pSetter->_name = _name;
    pSetter->_targetName = _targetName;
    pSetter->_prop = _prop;

    if (_value.get() != NULL)
    {
        _value->CloneNode(pSetter->_value);
        pSetter->_oldValue = _value->GetValue();
    }
    pSetter->_dpItem = _dpItem;
}

DpProperty* SetterNode::UpdateProperty(RTTIOfInfo* owner)
{
    if (_prop != NULL && !_prop->IsAttached() && !owner->InheritFrom(_prop->GetOwnerType()))
    {
        _prop = DpManager::Ins()->FindDp(_prop->GetName(), owner, false);
    }

    return _prop;
}

DpItem* SetterNode::GetDpItem()
{
    if (_dpItem == NULL)
    {
        if (GetProperty() != NULL)
        {
            _dpItem = DpManager::Ins()->FindDpItem(GetProperty());
        }
        else
        {
            _dpItem = DpManager::Ins()->FindDpItem(GetName());
        }
    }
    return _dpItem;
}

bool SetterNode::IsExtension()
{
    suic::Extension* expr = suic::RTTICast<suic::Extension>(GetValue());
    return (NULL != expr);
}
