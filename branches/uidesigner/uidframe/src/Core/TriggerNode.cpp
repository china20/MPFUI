
#include "stdafx.h"

#include <Core/TriggerNode.h>
#include <Core/TypeManager.h>

ImplementRTTIOfClass(TriggerBaseNode, XamlNodeItem)
//ImplementRTTIOfClass(TriggerNode, TriggerBaseNode)
ImplementRTTIOfClass(TriggerNode, TriggerBaseNode)

TriggerBaseNode::TriggerBaseNode()
{
    _rttiInfo = NULL;
    _setterCollNode = new SetterCollectionNode();
    _setterCollNode->ref();
}

TriggerBaseNode::~TriggerBaseNode()
{
    _setterCollNode->unref();
}

SetterCollectionNode* TriggerBaseNode::GetSetterCollection()
{
    return _setterCollNode;
}


/*TriggerNode::TriggerNode()
{
    _value = NULL;
}

TriggerNode::~TriggerNode()
{
    FREEREFOBJ(_value);
}*/

void TriggerBaseNode::SetRTTIOfInfo(RTTIOfInfo* rttiInfo)
{
    _rttiInfo = rttiInfo;
    GetSetterCollection()->SetRTTIOfInfo(_rttiInfo);
}

/*void TriggerNode::SetValue(DpProperty* dp, Object* val)
{
    _value = new SetterNode(dp->GetName(), dp, val);
}

void TriggerNode::SetSetterNode(SetterNode* pNode)
{
    SETREFOBJ(_value, pNode);
}*/

void TriggerBaseNode::AddSetter(SetterNode* setter)
{
    GetSetterCollection()->AddSetter(setter);
}

/*suic::String TriggerNode::GetResXml(const String& offset)
{
    suic::String strXml;
    DpItem* dpItem = DpManager::Ins()->FindDpItem(_value->GetName());
    suic::String strVal = dpItem->ValueToString(_value->GetResNode());
    suic::DpProperty* dp = _value->GetProperty();

    strXml = offset + _U("<Trigger Property=\"");
    if (dp->IsAttached() && !GetType()->InheritFrom(dp->GetOwnerType()))
    {
        strXml += dp->GetOwnerType()->typeName;
        strXml += _U(".");
    }
    strXml += _value->GetName();
    strXml += _U("\" Value=\"");
    strXml += strVal;
    strXml += _U("\">\n");

    strXml += GetSetterCollection()->GetResXml(offset + ResNode::OFFSET1);

    strXml += offset + _U("</Trigger>\n");

    return strXml;
}*/

TriggerNode::TriggerNode()
{
    _conds = new SetterCollectionNode();
    _conds->ref();
}

TriggerNode::~TriggerNode()
{
    _conds->unref();
}

String TriggerNode::ToString()
{
    if (_conds->GetCount() == 0)
    {
        return _U("Invalid Value");
    }
    else if (_conds->GetCount() == 1)
    {
        return _conds->GetSetter(0)->GetName();
    }
    else
    {
        return _conds->GetSetter(0)->GetName() + _U(" ...");
    }
}

void TriggerNode::SetArrayCondition(ArrayCondition* conds)
{
    for (int i = 0; i < conds->GetCount(); ++i)
    {
        Condition& cond = conds->GetItem(i);
        AddValue(cond.prop, cond.GetValue());
    }
}

void TriggerNode::AddValue(DpProperty* dp, Object* val)
{
    _conds->AddItem(new SetterNode(dp->GetName(), dp, val));
}

void TriggerNode::AddSetterNode(SetterNode* pNode)
{
    _conds->AddItem(pNode);
}

void TriggerNode::RemoveTrigger(const String& name)
{
    for (int i = 0; i < _conds->GetCount(); ++i)
    {
        SetterNode* cond = _conds->GetSetter(i);
        if (cond->GetName().Equals(name))
        {
            _conds->RemoveItemAt(i);
            break;
        }
    }
}

SetterNode* TriggerNode::FindTrigger(const String& name)
{
    for (int i = 0; i < _conds->GetCount(); ++i)
    {
        SetterNode* cond = _conds->GetSetter(i);
        if (cond->GetName().Equals(name))
        {
            return cond;            
        }
    }
    return NULL;
}

SetterCollectionNode* TriggerNode::GetContSetterCollection() const
{
    return _conds;
}

int TriggerNode::GetCount() const
{
    return _conds->GetCount();
}

SetterNode* TriggerNode::GetTrigger(int index) const
{
    return _conds->GetSetter(index);
}

suic::String TriggerNode::GetTriggerResXml(const String& offset)
{
    suic::String strXml;
    SetterNode* cond = _conds->GetSetter(0);
    DpItem* dpItem = DpManager::Ins()->FindDpItem(cond->GetName());
    suic::String strVal = dpItem->ValueToString(cond->GetResNode());
    suic::DpProperty* dp = cond->GetProperty();

    strXml = offset + _U("<Trigger Property=\"");
    if (dp->IsAttached() && !GetType()->InheritFrom(dp->GetOwnerType()))
    {
        strXml += dp->GetOwnerType()->typeName;
        strXml += _U(".");
    }
    strXml += cond->GetName();
    strXml += _U("\" Value=\"");
    strXml += strVal;
    strXml += _U("\">\n");

    strXml += GetSetterCollection()->GetResXml(offset + ResNode::OFFSET1);

    strXml += offset + _U("</Trigger>\n");

    return strXml;
}

TriggerBaseNode* TriggerNode::CloneNode()
{
    TriggerNode* pTrigger = new TriggerNode();

    pTrigger->ref();
    CloneTriggerNode(pTrigger);
    return pTrigger;
}

void TriggerNode::CloneTriggerNode(TriggerNode* pTrigger)
{
    pTrigger->_rttiInfo = _rttiInfo;
    _setterCollNode->CloneSetterCollectionNode(pTrigger->_setterCollNode);
    _conds->CloneSetterCollectionNode(pTrigger->_conds);
}

suic::String TriggerNode::GetResXml(const String& offset)
{
    suic::String strXml;
    suic::String strCond;

    if (_conds->GetCount() == 0)
    {
        return strXml;
    }
    else if (_conds->GetCount() == 1)
    {
        return GetTriggerResXml(offset);
    }

    strXml = offset + _U("<MultiTrigger>\n");
    strXml += offset + ResNode::OFFSET1 + _U("<MultiTrigger.Conditions>\n");
    
    for (int i = 0; i < _conds->GetCount(); ++i)
    {
        SetterNode* cond = _conds->GetSetter(i);
        DpItem* dpItem = DpManager::Ins()->FindDpItem(cond->GetName());
        ResNodePtr resNode = cond->GetResNode();
        suic::String strVal = dpItem->ValueToString(resNode.get());
        suic::DpProperty* dp = cond->GetProperty();

        if (!strVal.Empty())
        {
            strCond += offset + ResNode::OFFSET2 + _U("<Condition Property=\"");

            if (dp->IsAttached() && !GetType()->InheritFrom(dp->GetOwnerType()))
            {
                strCond += dp->GetOwnerType()->typeName;
                strCond += _U(".");
            }

            strCond += cond->GetName();
            strCond += _U("\" Value=\"");
            strCond += strVal;
            strCond += _U("\"/>\n");
        }
        else
        {
            // tbl
        }
    }

    strXml += strCond;
    strXml += offset + ResNode::OFFSET1 + _U("</MultiTrigger.Conditions>\n");
    strXml += GetSetterCollection()->GetResXml(offset + ResNode::OFFSET1);

    strXml += offset + _U("</MultiTrigger>\n");

    return strXml;
}
