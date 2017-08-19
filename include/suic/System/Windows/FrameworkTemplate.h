// 华勤科技版权所有 2010-2011
// 
// 文件名：FrameworkTemplate.h
// 功  能：实现控件模板。
// 
// 作  者：MPF开发组
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UIFRAMEWORKTEMPLATE_H_
#define _UIFRAMEWORKTEMPLATE_H_

#include <System/Resources/ResourceDictionary.h>
#include <System/Windows/FrameworkElementFactory.h>
#include <System/Tools/Collection.h>

namespace suic 
{

class Element;
class FrameworkElement;

struct ChildNameIndex
{
    int index;
    RTTIOfInfo* typecls;
};

class SUICORE_API FrameworkTemplate : public DpObject
{
public:

    RTTIOfClass(FrameworkTemplate)

    FrameworkTemplate();
    virtual ~FrameworkTemplate();

    void Dispose();

    bool IsSealed();

    AutoObj LoadContent(DpObject* container);
    AutoObj InstantiateTree(FrameworkElement* container);

    FEFactory* GetVisualTree();
    void SetVisualTree(FEFactory* val);

    ResourceDictionary* GetResources();
    void SetResources(ResourceDictionary* val);

    Element* FindName(const String& name, FrameworkElement* templatedParent);

    void Seal();
    void SetResourceReferenceState();

    void SealTemplateBinding();
    bool IsTemplateBindingSealed() const;

    ChildNameIndex ChildNameIndexFromChildName(const String& name);
    int ChildIndexFromChildName(const String& name);

    ContainerDeps* GetElementDeps();
    DepRecord* GetResourceRecord();
    BindDepRecord* GetBindDepRecord();

    TriggerDepRecords* GetTrgDepRecords();
    TriggerDepRecord* GetTrgDepRecord(int index);

public:

    virtual void SetTargetType(RTTIOfInfo* targetType);
    virtual void ValidateTemplatedParent(FrameworkElement* templatedParent);
    virtual void ProcessTemplateBeforeSeal();
    virtual bool BuildVisualTree(FrameworkElement* container);

    virtual RTTIOfInfo* GetTargetType();
    virtual TriggerCollection* GetTriggers();

protected:

    void CheckSealed();

    bool ApplyTemplateContent(FrameworkElement* container);
    void ValidateVisualTree(FEFactory* val);

protected:

    bool _templateSeal;
    int _templateChildCount;

    FEFactory* _templateRoot; 
    ResourceDictionary* _resources;

    // 容器依赖的所有属性（可能来自Trigger）
    ContainerDeps _elementDependents;
    
    // 记录动态资源
    DepRecord* _resDepRecords;
    // 记录绑定(Binding)
    BindDepRecord* _bindDepRecords;
    TriggerDepRecords* _trgDepRecords;

    StringDic<ChildNameIndex>  _childNameMappingIndex;

    friend class FEFactory;
};

typedef shared<FrameworkTemplate> FrameworkTemplatePtr;

inline bool FrameworkTemplate::IsSealed()
{
    VerifyAccess(__FILELINE__); 
    return DpObject::IsSealed();
}

inline ContainerDeps* FrameworkTemplate::GetElementDeps()
{
    return &_elementDependents;
}

inline DepRecord* FrameworkTemplate::GetResourceRecord()
{
    if (NULL == _resDepRecords)
    {
        _resDepRecords = new DepRecord();
    }
    return _resDepRecords;
}

inline BindDepRecord* FrameworkTemplate::GetBindDepRecord()
{
    if (NULL == _bindDepRecords)
    {
        _bindDepRecords = new BindDepRecord();
    }
    return _bindDepRecords;
}

inline TriggerDepRecords* FrameworkTemplate::GetTrgDepRecords()
{
    return _trgDepRecords;
}

inline TriggerDepRecord* FrameworkTemplate::GetTrgDepRecord(int index)
{
    if (NULL == GetTrgDepRecords())
    {
        return NULL;
    }
    else
    {
        return GetTrgDepRecords()->GetTrgDepRecord(index);
    }
}

inline bool FrameworkTemplate::IsTemplateBindingSealed() const
{
    return _templateSeal;
}

inline void FrameworkTemplate::SealTemplateBinding()
{
    _templateSeal = true;
}

inline FEFactory* FrameworkTemplate::GetVisualTree()
{
    VerifyAccess(__FILELINE__);
    return _templateRoot; 
}

}

#endif
