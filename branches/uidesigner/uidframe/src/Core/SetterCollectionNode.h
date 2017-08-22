///
/// 文件名：SetterCollectionNode.h
///
/// 功  能：存储Setter
///
/// 作  者：汪荣
///

#ifndef _SETTERCOLLECTIONNODE_H
#define _SETTERCOLLECTIONNODE_H

#include <Core/SetterNode.h>

class SetterCollectionNode;

class SetterSelf : public XamlNodeItem
{
public:

    SetterSelf();
    SetterSelf(SetterCollectionNode* setters);

    RTTIOfClass(SetterSelf)

    String ToString()
    {
        return _U("Layout");
    }

    SetterCollectionNode* GetSetters() const
    {
        return _setters;
    }

private:

    SetterCollectionNode* _setters;
};

class SetterCollectionNode : public XamlNodeItem
{
public:

    SetterCollectionNode();
    ~SetterCollectionNode();

    RTTIOfClass(SetterCollectionNode)

    void Clear();

    void CloneNode(ResNodePtr& obj);
    void CloneSetterCollectionNode(SetterCollectionNode* Other);

    suic::String GetResXml(const String& offset);

    void RemoveSetter(SetterNode* pNode);
    void RemoveSetter(const String& strName);

    void AddSetter(SetterNode* pNode);

    void AddSetterForAdd(SetterNode* pNode);

    void SetRTTIOfInfo(RTTIOfInfo* rttiInfo);
    RTTIOfInfo* GetOwnerType() const;

    SetterNode* GetSetter(int index);
    SetterNode* FindSetter(const String& name);

private:

    RTTIOfInfo* _rttiInfo;
};

#endif
