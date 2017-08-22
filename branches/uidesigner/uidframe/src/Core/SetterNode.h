///
/// 文件名：SetterNode.h
///
/// 功  能：定义Setter资源
///
/// 作  者：汪荣
///

#ifndef _SETTERNODE_H
#define _SETTERNODE_H

#include <Core/ResNode.h>
#include <Core/CoreDefines.h>
#include <Core/XamlSceneNode.h>

struct DpItem;

class SetterNode : public XamlNodeItem
{
public:

    RTTIOfClass(SetterNode)
;
    SetterNode();
    SetterNode(const String& name, DpProperty* dp, Object* val);

    virtual ~SetterNode();
    void CloneSetterNode(SetterNode* obj);

    DpProperty* UpdateProperty(RTTIOfInfo* owner);

    bool IsLeafItem();
    String GetName() const;

    String GetTargetName() const;
    void SetTargetName(const String& name);

    DpProperty* GetProperty() const;
    Object* GetValue();

    ePropertyType GetFromTrigger() const;
    void SetFromTriggerProp(ePropertyType val);
    ResNode* GetResNode();
    Object* GetOldValue();
    void UpdateOldValue(Object* val);
    void SetResNode(ResNode* resNode);
    void SetValue(Object* val);
    String ToString();

    DpItem* GetDpItem();
    bool IsExtension();

private:

    String _name;
    String _targetName;
    ePropertyType _fromTrigger;
    DpProperty* _prop;
    ResNodePtr _value;
    ObjectPtr _oldValue;
    DpItem* _dpItem;
};

inline bool SetterNode::IsLeafItem()
{
    return true;
}

inline String SetterNode::GetName() const
{
    return _name;
}

inline String SetterNode::GetTargetName() const
{
    return _targetName;
}

inline void SetterNode::SetTargetName(const String& name)
{
    _targetName = name;
}

inline DpProperty* SetterNode::GetProperty() const
{
    return _prop;
}

inline Object* SetterNode::GetValue()
{
    if (NULL == GetResNode())
    {
        return NULL;
    }
    else
    {
        return GetResNode()->GetValue();
    }
}

inline ePropertyType SetterNode::GetFromTrigger() const
{
    return _fromTrigger;
}

inline void SetterNode::SetFromTriggerProp(ePropertyType val)
{
    _fromTrigger = val;
}

inline ResNode* SetterNode::GetResNode()
{
    return _value.get();
}

inline Object* SetterNode::GetOldValue()
{
    return _oldValue.get();
}

inline void SetterNode::UpdateOldValue(Object* val)
{
    _oldValue = val;
}

inline void SetterNode::SetResNode(ResNode* resNode)
{
    _value = resNode;
}

inline void SetterNode::SetValue(Object* val)
{
    ResNode::CreateResNode(val, _value);
}

inline String SetterNode::ToString() 
{
    return _name;
}

class UndefinedSetterNode : public SetterNode
{
public:

    UndefinedSetterNode(const String& name, const String& val)
        : SetterNode(name, NULL, new OString(val))
    {
    }
};

#endif
