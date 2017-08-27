
#ifndef _PROPERTYMANAGER_H_
#define _PROPERTYMANAGER_H_

#include <Core/XamlRootElement.h>

class IValueEditor
{
public:

    virtual ~IValueEditor() {}

    virtual void SetSetterNode(SetterNode* setter) = 0;
    virtual SetterNode* GetSetterNode(bool bCreate) = 0;
    virtual void Reset() = 0;
    virtual bool IsResetPending() const = 0;
    virtual void NotifySetterChanged() = 0;
};

class PropItem
{
public:

    // 属性名称
    suic::String name;
    // 属性默认值
    suic::ObjectPtr defVal;
    // 属性类型
    suic::DpProperty* prop;
    // 属性所属对象
    suic::RTTIOfInfo* owner;

    IValueEditor* elemBase;

    PropItem(const suic::String& n, suic::ObjectPtr val, suic::DpProperty* t, suic::RTTIOfInfo* o);
};

class PropManager
{
public:

    static PropManager* Ins();

    int GetCount() const;
    PropItem* GetPropItem(int index) const;
    PropItem* FindProp(const suic::String& name);

private:

    PropManager();

    void InitProps();

    static suic::Array<PropItem*> _props;
};

#endif

