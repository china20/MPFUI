///
/// 文件名：TriggerCollectionNode.h
///
/// 功  能：TriggerCollection资源
///
/// 作  者：汪荣
///

#ifndef _TRIGGERCOLLECTIONNODE_H
#define _TRIGGERCOLLECTIONNODE_H

#include <Core/TriggerNode.h>

class TriggerCollectionNode;

class TriggersNode : public XamlNodeItem
{
public:

    TriggersNode();
    TriggersNode(TriggerCollectionNode* triggers);

    RTTIOfClass(TriggersNode)

    String ToString()
    {
        return _U("Triggers");
    }

    TriggerCollectionNode* GetTriggers() const
    {
        return _triggers;
    }

private:

    TriggerCollectionNode* _triggers;
};

class TriggerCollectionNode : public XamlNodeItem
{
public:

    TriggerCollectionNode();
    ~TriggerCollectionNode();

    RTTIOfClass(TriggerCollectionNode)

    suic::String GetResXml(const String& offset);

    void CloneTriggerCollectionNode(TriggerCollectionNode* Other);

    void Clear();
    void RemoveTrigger(TriggerBaseNode* pNode);
    void AddTriggerNode(TriggerBaseNode* trigger);

    void SetRTTIOfInfo(RTTIOfInfo* rttiInfo);
    RTTIOfInfo* GetOwnerType() const;

    int GetCount();
    TriggerBaseNode* GetTrigger(int index);

    bool IsTriggerEmpty();

    String ToString()
    {
        return _U("Triggers");
    }

private:

    RTTIOfInfo* _rttiInfo;
};

#endif

