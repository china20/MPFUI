//////////////////////////////////////////////////////////////////////////////
// ReloadXamlWalker.cpp

#include "stdafx.h"

#include <Core/ReloadXamlWalker.h>
#include <Main/Project.h>

ReloadXamlWalker::ReloadXamlWalker(XamlRootItem* root)
{
    _root = root;
    _project = root->GetProject();
    _elemRoot = NULL;
    setAutoDelete(false);
}

ReloadXamlWalker::~ReloadXamlWalker()
{
}

//----------------------

void ReloadXamlWalker::OnStartWalk(Object* obj)
{
    Application* pApp = RTTICast<Application>(obj);
    ResourceDictionary* pResDic = RTTICast<ResourceDictionary>(obj);

    if (NULL != pApp)
    {
    }
    else if (NULL != pResDic)
    {
    }
    else if (NULL != obj && _elemRoot == NULL)
    {
        _elemRoot = _root->GetXamlElementNode();
        _elems.Add(_elemRoot);
    }
}

void ReloadXamlWalker::OnFinishWalk(Object* obj)
{

}

void ReloadXamlWalker::OnStartNode(Object* elem, IXamlNode* node, String data)
{
    
}

void ReloadXamlWalker::OnFinishNode(Object* elem, IXamlNode* node, String data)
{

}

void ReloadXamlWalker::OnStartApp(Application* app, IXamlNode* node)
{
}

void ReloadXamlWalker::OnFinishApp(Application* app, IXamlNode* node)
{
}

void ReloadXamlWalker::OnStartAppWindow(Application* app, FrameworkElement* mainWnd)
{
}

void ReloadXamlWalker::OnFinishAppWindow(Application* app, FrameworkElement* mainWnd)
{

}

void ReloadXamlWalker::OnStartElement(DpObject* d, IXamlNode* node, bool bFromElem)
{
}

void ReloadXamlWalker::OnFinishElement(DpObject* d, IXamlNode* node, bool bFromElem)
{
    int iCount = _elems.GetCount();
}

void ReloadXamlWalker::OnStartReadLayout(Object* elem, IXamlNode* node)
{
}

void ReloadXamlWalker::OnFinishReadLayout(Object* elem, const ResourceItem& res, IXamlNode* node)
{
}

void ReloadXamlWalker::OnStartTemplate(FrameworkTemplate* ft, IXamlNode* node)
{
}

void ReloadXamlWalker::OnFinishTemplate(FrameworkTemplate* ft, IXamlNode* node)
{

}

void ReloadXamlWalker::OnStartControlTemplateTrigger(FrameworkTemplate* ft, IXamlNode* node)
{
}

void ReloadXamlWalker::OnFinishControlTemplateTrigger(FrameworkTemplate* ft, IXamlNode* node)
{

}

void ReloadXamlWalker::OnStartTemplateElement(FEFactory* fef, IXamlNode* node)
{
}

void ReloadXamlWalker::OnFinishTemplateElement(FEFactory* fef, IXamlNode* node)
{
}

void ReloadXamlWalker::OnReadTemplateElementCommand(FEFactory* fef, const String& val, IXamlNode* node)
{
}

void ReloadXamlWalker::OnStartResDic(ResourceDictionary* res, IXamlNode* node, Byte bMerged)
{
}

void ReloadXamlWalker::OnFinishResDic(ResourceDictionary* res, IXamlNode* node, Byte bMerged)
{
}

// 这种情况必须是元素资源节点
void ReloadXamlWalker::OnStartRefResDic(ResourceDictionary* res, IXamlNode* node, Byte bMerged)
{
}

void ReloadXamlWalker::OnFinishRefResDic(ResourceDictionary* res, IXamlNode* node, Byte bMerged)
{
}

void ReloadXamlWalker::OnStartResDicSource(const String& strSource, ResourceDictionary* res, IXamlNode* node, Byte bMerged)
{
}

void ReloadXamlWalker::OnFinishResDicSource(const String& strSource, ResourceDictionary* res, IXamlNode* node, Byte bMerged)
{
}

void ReloadXamlWalker::OnStartMergedDictionaries(ResourceDictionary* res, IXamlNode* node)
{
}

void ReloadXamlWalker::OnFinishMergedDictionaries(ResourceDictionary* res, IXamlNode* node)
{
}

void ReloadXamlWalker::OnStartMergedResDic(ResourceDictionary* res, ResourceDictionary* mergedRes, IXamlNode* node)
{
}

void ReloadXamlWalker::OnFinishMergedResDic(ResourceDictionary* res, ResourceDictionary* mergedRes, IXamlNode* node)
{
}

void ReloadXamlWalker::OnStartStyle(ResourceDictionary* res, const String& key, Style* sty, IXamlNode* node)
{
}

void ReloadXamlWalker::OnFinishStyle(ResourceDictionary* res, const String& key, Style* sty, IXamlNode* node)
{
}

void ReloadXamlWalker::OnStartTrigger(ControlTemplate* temp, Trigger* trg, IXamlNode* node)
{
}

void ReloadXamlWalker::OnFinishTrigger(ControlTemplate* temp, Trigger* trg, IXamlNode* node)
{
}

void ReloadXamlWalker::OnStartMultiTrigger(ControlTemplate* temp, MultiTrigger* trg, IXamlNode* node)
{
}

void ReloadXamlWalker::OnFinishMultiTrigger(ControlTemplate* temp, MultiTrigger* trg, IXamlNode* node)
{
}

void ReloadXamlWalker::OnStartReadSetter(Object* owner, WalkerFrom eFrom, const String& nodeName, DpProperty* dp)
{
}

void ReloadXamlWalker::OnReadSetter(Object* owner, WalkerFrom eFrom, const String& targetName, const String& name, const String& val, DpProperty* dp, const ResourceItem& resItem)
{
}

void ReloadXamlWalker::OnReadRes(ResourceDictionary* res, WalkerFrom eFrom, const String& key, const ResourceItem& resItem, IXamlNode* node)
{
}
