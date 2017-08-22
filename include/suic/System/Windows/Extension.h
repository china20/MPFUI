// 华勤科技版权所有 2010-2012
// 
// 文件名：Extension.h
// 功  能：抽象资源表达式。
// 
// 作  者：MPF开发组
// 时  间：2011-05-01
// 
// ============================================================================

#ifndef _UIEXTENSION_H_
#define _UIEXTENSION_H_

#include <System/Windows/DpObject.h>
#include <System/Windows/DpProperty.h>

namespace suic
{

enum ExtensionType
{
    etStaticRes,
    etDynamicRes,
    etTemplateBind,
};

class ResourceDictionary;

class SUICORE_API Extension : public Object
{
public:

    RTTIOfClass(Extension)

    Extension();
    Extension(int type);
    ~Extension();

    int GetType() const;

    virtual void ProvideValue(DpObject* d, DpProperty* dp, ObjectPtr& obj);

private:

    int _type;
};

class SUICORE_API TemplateBindingExtension : public Extension
{
public:

    RTTIOfClass(TemplateBindingExtension)

    TemplateBindingExtension();
    TemplateBindingExtension(DpProperty* prop);
    ~TemplateBindingExtension();

    DpProperty* GetProp() const;

    void ProvideValue(DpProperty* dp, ObjectPtr& obj);
    void ProvideValue(DpObject* d, DpProperty* dp, ObjectPtr& obj);
    
private:

    DpProperty* _prop;
};

inline DpProperty* TemplateBindingExtension::GetProp() const
{
    return _prop;
}


} 

#endif
