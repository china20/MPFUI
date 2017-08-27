// 华勤科技版权所有 2008-2022
// 
// 文件名：PropertyValue.h
// 功  能：元素样式的访问实现。
// 
// 作  者：汪荣
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UIPROPERTYVALUE_H_
#define _UIPROPERTYVALUE_H_

#include <System/Windows/DpObject.h>
#include <System/Windows/Expression.h>
#include <System/Windows/DeferredRef.h>

#include <System/Tools/Collection.h>

namespace suic
{

enum PropValType
{
    pvtSet,
    pvtTrigger,          // 静态资源绑定
    pvtPropTriggerRes,   // 动态资源绑定
    pvtTemplateBinding,  // 使用TemplateBinding绑定
    pvtResource,         // 资源
};

enum ValueLookupType
{
    vltSimple,
    vltTrigger,
    vltPropTriggerRes,
    vltTemplateBinding,
    vltResource,
};

struct TriggerCondition
{
    DpProperty* prop;
    Object* value;
    String sourceName;

    TriggerCondition()
    {
        prop = NULL;
        value = NULL;
    }
};

struct SUICORE_API PropValue
{
    // 值的来源
    PropValType valueType;
    // 触发条件
    TriggerCondition conditions;
    // 子元素的名称
    String childName;
    // 属性
    DpProperty* prop;

    Object* GetValue();

    PropValue();
    PropValue(const PropValue& Other);
    PropValue(Object* val);
    ~PropValue();

    PropValue& operator=(const PropValue& Other);

    void SetValue(Object* val);

private:

    // 属性值
    Object* value;
};

struct ContainerDep
{
    DpProperty* prop;
    bool fromTrigger;

    ContainerDep()
    {
        prop = NULL;
        fromTrigger = false;
    }

    ContainerDep(const ContainerDep& Other)
    {
        prop = Other.prop;
        fromTrigger = Other.fromTrigger;
    }

    Uint32 GetHashCode()
    {
        if (prop == NULL)
        {
            return 0;
        }
        else if (fromTrigger)
        {
            return (0x80000000 | (prop->GetIndex()));
        }
        else
        {
            prop->GetIndex();
        }
    }
};

struct TriggerDep
{
    DpProperty* prop;
    int childIndex;

    TriggerDep()
    {
        prop = NULL;
        childIndex = 0;
    }

    TriggerDep(DpProperty* dp, int index)
    {
        prop = dp;
        childIndex = index;
    }

    Uint32 GetHashCode()
    {
        if (prop == NULL)
        {
            return 0;
        }
        else
        {
            return ((childIndex << 16) | (prop->GetIndex()));
        }
    }
};

struct ChildPropDep
{
    int Index;
    String Name;
    DpProperty* Prop;

    ChildPropDep() 
        : Index(0), Prop(NULL) 
    {}
};

struct SUICORE_API DpDependent : public Object
{
    DpDependent();
    DpDependent(DpObject* o, DpProperty* p, Expression* e);

    RTTIOfClass(DpDependent)

    bool IsValid();

    DpObject* GetDO();
    DpProperty* GetDP();
    Expression* GetExpr();

    bool Equals(Object* o);

    friend bool operator==(DpDependent& first, DpDependent& second);
    friend bool operator!=(DpDependent& first, DpDependent& second);

    int GetHashCode();

private:

    DpProperty* prop;
    WeakRef d;
    WeakRef expr;
};

typedef Array<PropValue*> PropValueArray;
typedef Array<TriggerDep> TriggerDeps;
typedef Array<ContainerDep> ContainerDeps;
typedef Array<ChildPropDep> ChildPropDepList;

typedef ValueDic<Uint32, TriggerDep> DicTriggerDependents;

static PropValueArray EmptyArrayProps;
static ContainerDeps EmptyArrayContainerDeps;

}

#endif
