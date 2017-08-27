
#include <System/Windows/Extension.h>
#include <System/Windows/FrameworkElement.h>
#include <System/Windows/TemplateBindingExpression.h>

#include <System/Resources/StaticResource.h>
#include <System/Resources/DynamicResource.h>
#include <System/Windows/ResourceReferenceExpression.h>

#include <System/Tools/DesignHelper.h>
#include <System/Tools/LogicalTreeOp.h>

#include <Framework/Controls/Application.h>

namespace suic
{

ImplementRTTIOfClass(Extension, Object)
ImplementRTTIOfClass(TemplateBindingExtension, Extension)
ImplementRTTIOfClass(SRExtension, Extension)
ImplementRTTIOfClass(DRExtension, Extension)

Extension::Extension()
    : _type(0)
{

}

Extension::Extension(int type)
    : _type(type)
{
}

Extension::~Extension()
{

}

int Extension::GetType() const
{
    return _type;
}

void Extension::ProvideValue(DpObject* d, DpProperty* dp, ObjectPtr& obj)
{
    obj = DpProperty::UnsetValue();
}

TemplateBindingExtension::TemplateBindingExtension()
    : Extension(ExtensionType::etTemplateBind)
{

}

TemplateBindingExtension::TemplateBindingExtension(DpProperty* prop)
    : _prop(prop)
    , Extension(ExtensionType::etTemplateBind)
{

}

TemplateBindingExtension::~TemplateBindingExtension()
{

}

void TemplateBindingExtension::ProvideValue(DpProperty* dp, ObjectPtr& obj)
{
    obj = new TBExpression(this);
}

void TemplateBindingExtension::ProvideValue(DpObject* d, DpProperty* dp, ObjectPtr& obj)
{
    if (NULL != _prop)
    {
        obj = new TBExpression(this);
    }
    else
    {
        obj = DpProperty::UnsetValue();
    }

    /*FrameworkElement* fe = RTTICast<FrameworkElement>(d);

    if (NULL != fe && NULL != _prop)
    {
        FrameworkElement* tfe = RTTICast<FrameworkElement>(fe->GetTemplatedParent());
        if (NULL == tfe)
        {
            obj = DpProperty::UnsetValue();
            return;
        }

        _prop = DpProperty::Lookup(_prop->GetName(), tfe->GetRTTIType());

        if (_prop == NULL)//DpProperty::Lookup(_prop->GetName(), tfe->GetClassType()) != _prop)
        {
            obj = DpProperty::UnsetValue();
            return;
        }

        obj = new TBExpression(this);
    }
    else
    {
        obj = DpProperty::UnsetValue();
    }*/
}

SRExtension::SRExtension()
    : Extension(ExtensionType::etStaticRes)
{

}

SRExtension::SRExtension(String resourceKey)
    : _resourceKey(resourceKey)
    , Extension(ExtensionType::etStaticRes)
{

}

SRExtension::~SRExtension()
{

}

//ObjectPtr StaticResourceExtension::ProvideValue(ResourceDictionary* resDic, DpProperty* dp)
//{
//    String resKey(GetResourceKey());
//    ObjectPtr retVal = DpProperty::UnsetValue();
//
//    if (resDic != NULL)
//    {
//        retVal = resDic->Lookup(resKey);
//    }
//    return retVal;
//}

void SRExtension::ProvideValue(DpObject* d, DpProperty* dp, ObjectPtr& obj)
{
    String resKey(GetResourceKey());
    FrameworkElement* fe = RTTICast<FrameworkElement>(d);
    ResourceDictionary* sharedRes = NULL;

    if (NULL == fe && NULL != d)
    {
        fe = LogicalTreeOp::GetParent(d);
    }

    if (NULL != fe)
    {
        obj = fe->FindRes(NULL, resKey);
    }
    else
    {
        obj = DpProperty::UnsetValue();
    }
}

DRExtension::DRExtension()
    : Extension(ExtensionType::etDynamicRes)
{

}

DRExtension::DRExtension(String resourceKey)
    : _resourceKey(resourceKey)
    , Extension(ExtensionType::etDynamicRes)
{

}

DRExtension::~DRExtension()
{

}

void DRExtension::ProvideValue(DpObject* d, DpProperty* dp, ObjectPtr& obj)
{
    obj = new RRExpression(GetResourceKey());
}

}
