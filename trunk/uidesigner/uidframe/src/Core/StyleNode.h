///
/// 文件名：StyleNode.h
///
/// 功  能：定义Style资源节点
///
/// 作  者：汪荣
///

#ifndef _STYLENODE_H
#define _STYLENODE_H

#include <Core/SetterCollectionNode.h>
#include <Core/TriggerCollectionNode.h>

class Project;

class StyleNode : public ResNode
{
public:

    StyleNode();
    ~StyleNode();

    BeginMember(StyleNode, ResNode)
        MemberGetObject(ItemsSource)
    EndMember()

    RTTIOfClass(StyleNode)

    SetterCollectionNode* GetSetterCollection();
    TriggerCollectionNode* GetTriggerCollection();

    void ParseStyleValue(Project* pPrj, FrameworkElement* fe, ObjectPtr& resVal);

    suic::Object* GetItemsSource()
    {
        return NULL;
    }

    bool IsLeafItem()
    {
        return true;
    }

    virtual void CloneNode(ResNodePtr& obj);

    virtual bool IsSingleValue();
    virtual void SetValue(suic::Object* val);
    virtual suic::Object* GetValue();
    virtual suic::String GetSingleXml();
    virtual suic::String GetResXml(const String& offset);

    virtual RTTIOfInfo* GetValueRTTIType()
    {
        return Style::RTTIType();
    }

    bool HasContent() const;

    void SetStyle(const suic::String& key, suic::Style* style);

    void SetSetter(SetterNode* setter);
    void AddTrigger(TriggerBaseNode* trigger);

    RTTIOfInfo* GetTargetType() const
    {
        return _target;
    }

    void SetTargetType(RTTIOfInfo* rttiInfo);

    String ToString()
    {
        return GetKey() + _U("(Style)");
    }

private:

    void CheckRTTIOfInfo();

private:

    suic::String _targetType;
    suic::RTTIOfInfo* _target;
    SetterCollectionNode* _setterCollNode;
    TriggerCollectionNode* _trgCollNode;
};

#endif
