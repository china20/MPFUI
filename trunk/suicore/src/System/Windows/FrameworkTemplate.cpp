// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
//
// ======================================================================

#include <System/Tools/Debug.h>

#include <System/Resources/XamlWalker.h>
#include <System/Resources/XamlNodeImp.h>

#include <System/Windows/FrameworkElement.h>
#include <System/Windows/FrameworkTemplate.h>
#include <System/Windows/FrameworkElementFactory.h>

#include <System/Tools/StyleHelper.h>

#include <Framework/Controls/DataTemplate.h>
#include <Framework/Controls/ControlTemplate.h>

namespace suic 
{

ImplementRTTIOfClass(FrameworkTemplate, DpObject)

FrameworkTemplate::FrameworkTemplate()
{
    _resources = NULL;
    _templateRoot = NULL; 
    _templateSeal = false;
    _templateChildCount = 1;

    _bindDepRecords = NULL;
    _resDepRecords = NULL;
    _trgDepRecords = NULL;
}

FrameworkTemplate::~FrameworkTemplate()
{
    Dispose();
}

void FrameworkTemplate::Dispose()
{
    if (NULL != _resDepRecords)
    {
        delete _resDepRecords;
        _resDepRecords = NULL;
    }

    if (NULL != _bindDepRecords)
    {
        delete _bindDepRecords;
        _bindDepRecords = NULL;
    }

    if (NULL != _trgDepRecords)
    {
        delete _trgDepRecords;
        _trgDepRecords = NULL;
    }

    if (_templateRoot)
    {
        _templateRoot->ClearChildren();
        delete _templateRoot;
        _templateRoot = NULL;
    }
}

void FrameworkTemplate::SetVisualTree(FEFactory* val) 
{
    VerifyAccess(__FILELINE__);

    CheckSealed();
    ValidateVisualTree(val); 

    _templateRoot = val;

    if (_templateRoot)
    {
        _templateRoot->_childIndex = 1;
        _templateRoot->_frameworkTemplate = this;
    }
}

RTTIOfInfo* FrameworkTemplate::GetTargetType()
{
    return NULL;
}

TriggerCollection* FrameworkTemplate::GetTriggers()
{
    return NULL;
}

ResourceDictionary* FrameworkTemplate::GetResources()
{
    VerifyAccess(__FILELINE__); 

    if (_resources == NULL) 
    {
        _resources = new ResourceDictionary();
    } 

    if (IsSealed())
    { 
        _resources->SetIsReadOnly(true);
    }

    return _resources; 
}

void FrameworkTemplate::SetResources(ResourceDictionary* val) 
{ 
    VerifyAccess(__FILELINE__); 

    if (IsSealed())
    {
        throw InvalidOperationException(_U("FrameworkTemplate::SetResources"), _T("Object is sealed!"), __FILELINE__); 
    }

    _resources = val; 
}

Element* FrameworkTemplate::FindName(const String& name, FrameworkElement* templatedParent)
{
    VerifyAccess(__FILELINE__);

    if (templatedParent == NULL) 
    {
        throw ArgumentNullException(_U("templatedParent"), __FILELINE__); 
    } 

    if (this != templatedParent->GetTemplateInternal()) 
    {
        throw InvalidOperationException(_U("FrameworkTemplate::FindName"), __FILELINE__);
    }

    Element* elem = NULL;
    XArray* childDep = StyleHelper::TemplatedBindField->GetValue(templatedParent);

    if (NULL != childDep)
    {
        int index = ChildIndexFromChildName(name);
        if (index > 0)
        {
            elem = RTTICast<Element>(childDep->GetItem(index - 1));
        }
    }

    return elem;
}

void FrameworkTemplate::Seal()
{
    if (!IsSealed())
    {
        VerifyAccess(__FILELINE__);
        DpObject::Seal();

        if (_templateRoot)
        {
            _templateRoot->Seal();
        }
    }
}

void FrameworkTemplate::SetResourceReferenceState() 
{ 
    Debug::Assert(!IsSealed(), _U("call this method before template is sealed"));

    // StyleHelper::SortResourceDependents(ResourceDependents);
}

void FrameworkTemplate::SetTargetType(RTTIOfInfo* targetType)
{
}

void FrameworkTemplate::ValidateTemplatedParent(FrameworkElement* templatedParent) 
{
}

void FrameworkTemplate::ProcessTemplateBeforeSeal()
{
}

AutoObj FrameworkTemplate::LoadContent(DpObject* container)
{
    AutoObj rootElem;
    if (GetVisualTree())
    {
        WalkTemplate walk(GetVisualTree(), _templateChildCount);
        rootElem = walk.InstantiateTree(container, container);
        SealTemplateBinding();
    }

    return rootElem; 
}

AutoObj FrameworkTemplate::InstantiateTree(FrameworkElement* container)
{
    AutoObj rootElem;
    if (GetVisualTree())
    {
        WalkTemplate walk(GetVisualTree(), _templateChildCount);
        rootElem = walk.InstantiateTree(container, container);
        if (rootElem.get() != NULL)
        {
            SealTemplateBinding();
            StyleHelper::UpdateTemplateInvalidations(container);
        }
    }

    return rootElem; 
}

bool FrameworkTemplate::BuildVisualTree(FrameworkElement* container) 
{ 
    return false;
}

void FrameworkTemplate::CheckSealed() 
{ 
    if (IsSealed())
    { 
        throw InvalidOperationException(_U("CannotChangeAfterSealed"), __FILELINE__);
    }
}

bool FrameworkTemplate::ApplyTemplateContent(FrameworkElement* container)
{
    ValidateTemplatedParent(container);

    bool visualsCreated = false; 

    return visualsCreated; 
}

void FrameworkTemplate::ValidateVisualTree(FEFactory* val)
{

}

int FrameworkTemplate::ChildIndexFromChildName(const String& name)
{
    ChildNameIndex index;

    if (_childNameMappingIndex.TryGetValue(name, index))
    {
        return index.index;
    }
    else
    {
        return 0;
    }
}

ChildNameIndex FrameworkTemplate::ChildNameIndexFromChildName(const String& name)
{
    ChildNameIndex index;
    if (!_childNameMappingIndex.TryGetValue(name, index))
    {
        index.index = 0;
        index.typecls = NULL;
    }
    
    return index;
}

}
