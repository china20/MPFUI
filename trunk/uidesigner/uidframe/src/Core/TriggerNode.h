///
/// 文件名：TriggerNode.h
///
/// 功  能：Trigger资源
///
/// 作  者：汪荣
///

#ifndef _TRIGGERNODE_H
#define _TRIGGERNODE_H

#include "SetterCollectionNode.h"

class TriggerBaseNode : public XamlNodeItem
{
public:

    TriggerBaseNode();
    ~TriggerBaseNode();

    BeginMember(TriggerBaseNode, XamlNodeItem)
        MemberGetObject(ItemsSource)
    EndMember()

    RTTIOfAbstractClass(TriggerBaseNode)

    virtual String GetResXml(const String& offset) = 0;
    virtual TriggerBaseNode* CloneNode() = 0;

    suic::Object* GetItemsSource()
    {
        return NULL;
    }

    bool IsLeafItem()
    {
        return true;
    }

    RTTIOfInfo* GetType() const
    {
        return _rttiInfo;
    }
    void SetRTTIOfInfo(RTTIOfInfo* rttiInfo);

    void AddSetter(SetterNode* setter);
    SetterCollectionNode* GetSetterCollection();

protected:

    RTTIOfInfo* _rttiInfo;
    SetterCollectionNode* _setterCollNode;
};

class TriggerNode : public TriggerBaseNode
{
public:

    TriggerNode();
    ~TriggerNode();

    RTTIOfClass(TriggerNode)

    suic::String GetResXml(const String& offset);

    TriggerBaseNode* CloneNode();
    void CloneTriggerNode(TriggerNode* pTrigger);

    String ToString();

    int GetCount() const;
    SetterNode* GetTrigger(int index) const;

    void SetArrayCondition(ArrayCondition* conds);
    void AddValue(DpProperty* dp, Object* val);
    void AddSetterNode(SetterNode* pNode);
    void RemoveTrigger(const String& name);

    SetterNode* FindTrigger(const String& name);
    SetterCollectionNode* GetContSetterCollection() const;

private:

    suic::String GetTriggerResXml(const String& offset);

private:

    SetterCollectionNode* _conds;
};

#endif
