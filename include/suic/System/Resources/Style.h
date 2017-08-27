// 华勤科技版权所有 2008-2022
// 
// 文件名：Style.h
// 功  能：元素样式的访问实现。
// 
// 作  者：汪荣
// 时  间：2011-08-02
// 
// ============================================================================

# ifndef _UISTYLE_H_
# define _UISTYLE_H_

#include <System/Resources/ResourceDictionary.h>

namespace suic
{

static const int TargetTypeID = 0x01;
static const int BasedOnID    = 0x02;

class SUICORE_API Style : public DpObject
{
private:

    Style* _basedOn;
    RTTIOfInfo* _targetType;
    ResourceDictionary* _resources; 
    SetterCollection* _setters;
    TriggerCollection* _visualTriggers;

    // 存储经过处理的属性值
    PropValueArray _propValues;
    // 容器依赖的所有属性（可能来自Trigger）
    ContainerDeps _elementDependents;
    // 记录动态资源
    DepRecord* _resDepRecords;

    int _modified;
    int _globalIndex;

    static int StyleInstanceCount;

public:

    RTTIOfClass(Style)

    Style();
    ~Style();

    PropValueArray* GetPropValues();
    ContainerDeps* GetElementDependents();

    /// <summary>
    ///     Style 构造函数
    /// </summary>
    /// <param name="targetType">应用的元素类型</param> 
    Style(RTTIOfInfo* targetType);

    /// <summary>
    ///     Style 构造函数 
    /// </summary>
    /// <param name="targetType">应用的元素类型</param> 
    /// <param name="basedOn">继承的样式</param> 
    Style(RTTIOfInfo* targetType, Style* basedOn);

    int GetHashCode();

    RTTIOfInfo* GetTargetType() const;
    void SetTargetType(RTTIOfInfo* targetType);

    void SetBasedOn(Style* value);
    Style* GetBasedOn();

    TriggerCollection* GetTriggers();
    SetterCollection* GetSetters();

    ResourceDictionary* GetResources();
    void SetResources(ResourceDictionary* value);

    Object* FindRes(const String& resourceKey);
    ResourceDictionary* FindResourceDictionary(const String& resourceKey);

    void AddSetter(Setter* value);

    bool HasResourceReferences();
    void CheckTargetType(Object* element);

    void Seal();

    bool IsSealed();
    void SetModified(int id);
    bool IsModified(int id);
    bool IsBasedOnModified();

    static void AddContainerDependent(DpProperty* dp, bool fromTrigger, Array<ContainerDep>& deps);

private:
    
    void GetUniqueGlobalIndex();
    void ProcessBasedOnStyle();
    void ProcessSelfStyles(Style* style);
    void ProcessSetters(Style* style);
    void ProcessVisualTriggers(Style* style);

    void UpdatePropertyValueList(DpProperty* dp, PropValType valueType, Object* value);
    void CheckForCircularBasedOnReferences();
};

typedef shared<Style> StylePtr;

inline PropValueArray* Style::GetPropValues()
{
    return &_propValues;
}

inline ContainerDeps* Style::GetElementDependents()
{
    return &_elementDependents;
}

inline int Style::GetHashCode()
{
    VerifyAccess(__FILELINE__);
    return _globalIndex; 
}

inline RTTIOfInfo* Style::GetTargetType() const
{
    return _targetType;
}

inline void Style::SetTargetType(RTTIOfInfo* targetType)
{
    _targetType = targetType;
    SetModified(TargetTypeID);
}

inline void Style::SetBasedOn(Style* value)
{
    if (value != this)
    {
        SETREFOBJ(_basedOn, value); 
        SetModified(BasedOnID);
    }
}

inline bool Style::IsSealed()
{ 
    VerifyAccess(__FILELINE__);
    return DpObject::IsSealed();
}

inline void Style::SetModified(int id) 
{ 
    _modified |= id; 
}

inline bool Style::IsModified(int id)
{ 
    return (id & _modified) != 0; 
}

inline bool Style::IsBasedOnModified() 
{ 
    return IsModified(BasedOnID); 
} 

}

#endif
