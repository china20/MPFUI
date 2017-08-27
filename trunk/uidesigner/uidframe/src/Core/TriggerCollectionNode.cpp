
#include "stdafx.h"

#include <Core/TriggerCollectionNode.h>

ImplementRTTIOfClass(TriggersNode, XamlNodeItem)
ImplementRTTIOfClass(TriggerCollectionNode, XamlNodeItem)

TriggersNode::TriggersNode()
{
    _triggers = NULL;
}

TriggersNode::TriggersNode(TriggerCollectionNode* triggers)
{
    _triggers = triggers;
}

TriggerCollectionNode::TriggerCollectionNode()
{
    _rttiInfo = NULL;
}

TriggerCollectionNode::~TriggerCollectionNode()
{
    Clear();
}

void TriggerCollectionNode::SetRTTIOfInfo(RTTIOfInfo* rttiInfo)
{
    _rttiInfo = rttiInfo;
    for (int i = 0; i < GetCount(); ++i)
    {
        TriggerBaseNode* pTrigger = GetTrigger(i);
        pTrigger->SetRTTIOfInfo(_rttiInfo);
    }
}

RTTIOfInfo* TriggerCollectionNode::GetOwnerType() const
{
    return _rttiInfo;
}

void TriggerCollectionNode::Clear()
{
    XamlNodeItem::Clear();
}

void TriggerCollectionNode::CloneTriggerCollectionNode(TriggerCollectionNode* pTrgColl)
{
    pTrgColl->_rttiInfo = _rttiInfo;
    for (int i = 0; i < GetCount(); ++i)
    {
        ResNodePtr newNode;
        TriggerBaseNode* pTrg = GetTrigger(i);
        TriggerBaseNode* pClone = pTrg->CloneNode();

        pTrgColl->AddTriggerNode(pClone);
        pClone->unref();
    }
}

suic::String TriggerCollectionNode::GetResXml(const String& offset)
{
    suic::String strXml;

    for (int i = 0; i < GetCount(); ++i)
    {
        TriggerBaseNode* pTrigger = GetTrigger(i);
        strXml += pTrigger->GetResXml(offset);
    }

    return strXml;
}

void TriggerCollectionNode::RemoveTrigger(TriggerBaseNode* pNode)
{
    for (int i = 0; i < GetCount(); ++i)
    {
        if (GetItem(i) == pNode)
        {
            this->RemoveItemAt(i);
            break;
        }
    }
}

void TriggerCollectionNode::AddTriggerNode(TriggerBaseNode* trigger)
{
    trigger->SetRTTIOfInfo(_rttiInfo);
    AddItem(trigger);
}

int TriggerCollectionNode::GetCount()
{
    return XamlNodeItem::GetCount();
}

TriggerBaseNode* TriggerCollectionNode::GetTrigger(int index)
{
    return RTTICast<TriggerBaseNode>(GetItem(index));
}

bool TriggerCollectionNode::IsTriggerEmpty()
{
    for (int i = 0; i < GetCount(); ++i)
    {
        TriggerBaseNode* trgNode = GetTrigger(i);
        if (trgNode != NULL && trgNode->GetCount() > 0)
        {
            return false;
        }
    }

    return true;
}