//////////////////////////////////////////////////////////////////////////////
// XamlWalker.cpp

#include "stdafx.h"

#include <Core/XamlWalker.h>
#include <Core/StyleNode.h>
#include <Main/Project.h>
#include <Framework/Controls/Track.h>
#include <Framework/Controls/HoldControl.h>

XamlWalker::XamlWalker(XamlRootItem* root)
{
    _root = root;
    _project = root->GetProject();

    _appRoot = NULL;
    _resRoot = NULL;
    _elemRoot = NULL;

    _mergedDics = NULL;

    _styleNode = NULL;
    _rootTemplate = NULL;
    _propLayout = NULL;

    _styleFromSetter = false;

    setAutoDelete(false);
}

XamlWalker::~XamlWalker()
{
    FREEREFOBJ(_elemRoot);
    FREEREFOBJ(_propLayout);
    FREEREFOBJ(_rootTemplate);
    FREEREFOBJ(_styleNode);
}

//----------------------

void XamlWalker::OnStartWalk(Object* obj)
{
    Application* pApp = RTTICast<Application>(obj);
    ResourceDictionary* pResDic = RTTICast<ResourceDictionary>(obj);

    if (NULL != pApp)
    {
        _root->ResetRootItem(true);
        _appRoot = _root->GetApplicationNode();
    }
    else if (NULL != pResDic)
    {
        _root->ResetRootItem(true);
        _resRoot = _root->GetResDictNode();
        _resRoot->SetResourceDictionary(pResDic);
        _resources.Add(_resRoot);
    }
    else if (NULL != obj && _elemRoot == NULL)
    {
        _root->ResetRootItem(true);
        _elemRoot = _root->GetXamlElementNode();
        _elemRoot->ref();
    }
}

void XamlWalker::OnFinishWalk(Object* obj)
{

}

void XamlWalker::OnStartNode(Object* elem, IXamlNode* node, String data)
{
    
}

void XamlWalker::OnFinishNode(Object* elem, IXamlNode* node, String data)
{

}

void XamlWalker::OnStartApp(Application* app, IXamlNode* node)
{
    SetterCollectionNode* pSetters = _appRoot->GetSetterCollection();

    _appRoot->SetApplication(app);
    _root->GetProject()->SwitchToCurrent();

    pSetters->SetRTTIOfInfo(app->GetRTTIType());
    _setters.Add(pSetters);
}

void XamlWalker::OnFinishApp(Application* app, IXamlNode* node)
{
    _setters.RemoveAt(_setters.GetCount() - 1);
}

void XamlWalker::OnStartAppWindow(Application* app, FrameworkElement* mainWnd)
{
    ResourceUri resPath = app->GetStartupUri();
    XamlRootItem* rootWnd = RTTICast<XamlRootItem>(_project->FindItemFromPath(resPath.GetResourcePath()));

    if (NULL != rootWnd)
    {
        rootWnd->ResetRootItem(true);
        _elemRoot = rootWnd->GetXamlElementNode();
        _elemRoot->ref();
        rootWnd->SetFileName(resPath.GetResourcePath());
        _root->SetMainWindow(rootWnd);
        _root->GetApplicationNode()->SetStartupUri(resPath.ToString());
    }
    else
    {
        throw suic::InvalidValueException(app->GetStartupUri().ToString(), __FILELINE__);
    }
}

void XamlWalker::OnFinishAppWindow(Application* app, FrameworkElement* mainWnd)
{

}

DesignElement* XamlWalker::CreateElementNode(DpObject* d, IXamlNode* node)
{
    DesignElement* elemNode = NULL;
    if (d->GetRTTIType()->InheritFrom(Grid::RTTIType()))
    {
        elemNode = new DesignGrid();
    }
    else
    {
        elemNode = new DesignElement();
    }
    return elemNode;
}

void XamlWalker::OnStartElement(DpObject* d, IXamlNode* node, bool bFromElem)
{
    SetterCollectionNode* pSetters = NULL;
    Element* elem = CASTTOE(d);

    if (NULL != d)
    {
        if (d->GetRTTIType() == suic::HoldControl::RTTIType())
        {
            DesignHoldElement* pHold = new DesignHoldElement(node);
            _setters.Add(NULL);
            _elems.Add(pHold);
            return;
        }

        DesignHelper::SetInDesignMode(d);

        if (elem != NULL)
        {
            if (_elems.GetCount() == 0 && bFromElem)
            {
                _elems.Add(_elemRoot);
                _elemRoot->SetNodeElement(d);
                pSetters = _elemRoot->GetSetterCollection();
            }
            else
            {
                // 来自于元素
                if (bFromElem || _propLayout->GetNodeElement() != d)
                {
                    DesignElement* pElem = CreateElementNode(d, node);

                    pElem->ref();
                    pElem->SetNodeElement(d);
                    DesignElement* pParent = _elems[_elems.GetCount() - 1];

                    pSetters = pElem->GetSetterCollection();
                    pParent->AddXamlElement(pElem);
                    _elems.Add(pElem);
                    pElem->unref();
                }
                // 否则作为资源或元素属性
                else
                {
                    _elems.Add(_propLayout);
                    pSetters = _propLayout->GetSetterCollection();
                }
            }

            pSetters->SetRTTIOfInfo(elem->GetRTTIType());
            _setters.Add(pSetters);
        }
        else
        {
            if (_propLayout->GetNodeElement() == d)
            {
                _elems.Add(_propLayout);
                pSetters = _propLayout->GetSetterCollection();
            }
            else
            {
                DesignElement* pElem = CreateElementNode(d, node);

                pElem->ref();
                pElem->SetNodeElement(d);
                DesignElement* pParent = _elems[_elems.GetCount() - 1];

                pSetters = pElem->GetSetterCollection();
                pParent->AddXamlElement(pElem);
                _elems.Add(pElem);
                pElem->unref();
            }

            pSetters->SetRTTIOfInfo(d->GetRTTIType());
            _setters.Add(pSetters);
        }
    }
    else
    {
        _setters.Add(NULL);
        _elems.Add(NULL);
    }
}

void XamlWalker::OnFinishElement(DpObject* d, IXamlNode* node, bool bFromElem)
{
    int iCount = _elems.GetCount();
    DesignElement* pElem = _elems[iCount - 1];

    if (NULL != pElem)
    {
        pElem->OnFinishReadElement(d);
    }

    _setters.RemoveAt(_setters.GetCount() - 1);
    _elems.RemoveAt(iCount - 1);
}

void XamlWalker::OnStartReadLayout(Object* elem, IXamlNode* node)
{
    if (NULL != _propLayout)
    {
        _propLayout->unref();
    }

    _propLayout = new DesignElement();
    _propLayout->ref();
    _propLayout->SetNodeElement(DynamicCast<DpObject>(elem));
}

void XamlWalker::OnFinishReadLayout(Object* elem, const ResourceItem& res, IXamlNode* node)
{

}

void XamlWalker::OnStartTemplate(FrameworkTemplate* ft, IXamlNode* node)
{
    if (NULL != _rootTemplate)
    {
        _rootTemplate->unref();
    }

    _rootTemplate = new TemplateRootItem(ft);
    _rootTemplate->ref();
}

void XamlWalker::OnFinishTemplate(FrameworkTemplate* ft, IXamlNode* node)
{

}

void XamlWalker::OnStartControlTemplateTrigger(FrameworkTemplate* ft, IXamlNode* node)
{
    if (NULL != _rootTemplate)
    {
        _rootTemplate->GetTriggerCollection()->SetRTTIOfInfo(ft->GetTargetType());
    }
}

void XamlWalker::OnFinishControlTemplateTrigger(FrameworkTemplate* ft, IXamlNode* node)
{

}

void XamlWalker::OnStartTemplateElement(FEFactory* fef, IXamlNode* node)
{
    DesignElement* elemNode = NULL;
    if (fef->GetType()->InheritFrom(Grid::RTTIType()))
    {
        elemNode = new DesignGrid(fef);
    }
    else if (fef->GetType()->InheritFrom(Track::RTTIType()))
    {
        elemNode = new DesignTrack(fef);
    }
    else
    {
        elemNode = new DesignElement(fef);
    }

    SetterCollectionNode* pSetters = elemNode->GetSetterCollection();
    pSetters->SetRTTIOfInfo(fef->GetType());
    _setters.Add(pSetters);

    if (_rootTemplate->GetRootTemplateElement() == NULL)
    {
        _rootTemplate->SetRootElement(elemNode);
        _elems.Add(elemNode);
    }
    else
    {
        elemNode->ref();
        DesignElement* pParent = _elems[_elems.GetCount() - 1];
        suic::IXamlNode* pp = dynamic_cast<suic::IXamlNode*>(pParent);
        if (pp != NULL)
        {
            pp = NULL;
        }
        pParent->AddXamlElement(elemNode);

        _elems.Add(elemNode);
        elemNode->unref();
    }
}

void XamlWalker::OnFinishTemplateElement(FEFactory* fef, IXamlNode* node)
{
    DesignElement* elemNode = _elems[_elems.GetCount() - 1];

    elemNode->OnFinishReadElement(NULL);
    _setters.RemoveAt(_setters.GetCount() - 1);
    _elems.RemoveAt(_elems.GetCount() - 1);
}

void XamlWalker::OnReadTemplateElementCommand(FEFactory* fef, const String& val, IXamlNode* node)
{
    DesignElement* elemNode = _elems[_elems.GetCount() - 1];
    SetterCollectionNode* pSetters = elemNode->GetSetterCollection();
    pSetters->AddSetter(new SetterNode(_U("Command"), NULL, new OString(val)));
}

void XamlWalker::OnStartResDic(ResourceDictionary* res, IXamlNode* node, Byte bMerged)
{
    // 是元素本身的资源
    if (_elems.GetCount() > 0)
    {
        DesignElement* pElem = _elems[_elems.GetCount() - 1];
        ResourceDictionaryNode* resNode = pElem->GetResourceDictionary();

        _resources.Add(resNode);
        resNode->SetResourceDictionary(res);
    }
    // Application资源
    else if (_appRoot != NULL)
    {
        ResourceDictionaryNode* resNode = _appRoot->GetResourceDictionary();
        _resources.Add(resNode);
        resNode->SetResourceDictionary(res);
    }
    else if (_resRoot != NULL)
    {
        _resources.Add(_resRoot);
    }
}

void XamlWalker::OnFinishResDic(ResourceDictionary* res, IXamlNode* node, Byte bMerged)
{
    _resources.RemoveAt(_resources.GetCount() - 1);
}

// 这种情况必须是元素资源节点
void XamlWalker::OnStartRefResDic(ResourceDictionary* res, IXamlNode* node, Byte bMerged)
{
    if (_resRoot != NULL)
    {
        throw suic::InvalidValueException(_U("Resource error, must resource of element"), __FILELINE__);
    }

    ResourceDictionaryNode* resNode = _resources[_resources.GetCount() - 1];
    resNode->SetResourceDictionary(res);
}

void XamlWalker::OnFinishRefResDic(ResourceDictionary* res, IXamlNode* node, Byte bMerged)
{
    //_resources.RemoveAt(_resources.GetCount() - 1);
}

void XamlWalker::OnStartResDicSource(const String& strSource, ResourceDictionary* res, IXamlNode* node, Byte bMerged)
{
    ResourceDictionaryNode* resNode = _resources[_resources.GetCount() - 1];
    ResourceDictionaryNode* resSource = NULL;//new ResourceDictionaryNode();

    String strUri = suic::ResourceUri(strSource).GetResourcePath();
    XamlRootItem* resElem = RTTICast<XamlRootItem>(_project->FindItemFromPath(strUri));

    if (NULL != resElem)
    {
        resElem->ResetRootItem(true);
        resSource = resElem->GetResDictNode();
        resElem->SetFileName(strUri);

        // 记录资源Source
        resNode->SetSourceResource(resElem);
        resNode->SetResourceSourceUri(strSource);
        _resources.Add(resSource);
        resSource->SetResourceDictionary(res);
    }
    else
    {
        throw suic::InvalidValueException(strUri, __FILELINE__);
    }
}

void XamlWalker::OnFinishResDicSource(const String& strSource, ResourceDictionary* res, IXamlNode* node, Byte bMerged)
{
    _resources.RemoveAt(_resources.GetCount() - 1);
}

void XamlWalker::OnStartMergedDictionaries(ResourceDictionary* res, IXamlNode* node)
{
    ResourceDictionaryNode* resNode = _resources[_resources.GetCount() - 1];
    _mergedDics = new MergedDictionariesNode();
    resNode->AddMergedDictionaries(_mergedDics);
}

void XamlWalker::OnFinishMergedDictionaries(ResourceDictionary* res, IXamlNode* node)
{
    _mergedDics = NULL;
}

void XamlWalker::OnStartMergedResDic(ResourceDictionary* res, ResourceDictionary* mergedRes, IXamlNode* node)
{
    ResourceDictionaryNode* resMerged = new ResourceDictionaryNode();

    _mergedDics->AddMergedResource(resMerged);
    _resources.Add(resMerged);
    resMerged->SetResourceDictionary(mergedRes);
}

void XamlWalker::OnFinishMergedResDic(ResourceDictionary* res, ResourceDictionary* mergedRes, IXamlNode* node)
{
    _resources.RemoveAt(_resources.GetCount() - 1);
}

void XamlWalker::OnStartStyle(ResourceDictionary* res, const String& key, Style* sty, IXamlNode* node)
{
    if (NULL != _styleNode)
    {
        _styleNode->unref();
        _styleNode = NULL;
    }

    _styleFromSetter = false;

    _styleNode = new StyleNode();
    _styleNode->ref();
    _styleNode->SetStyle(key, sty);
    _setters.Add(_styleNode->GetSetterCollection());
}

void XamlWalker::OnFinishStyle(ResourceDictionary* res, const String& key, Style* sty, IXamlNode* node)
{
    ResourceDictionaryNode* resNode = NULL;

    _styleNode->SetStyle(key, sty);

    if (!_styleFromSetter)
    {
        resNode = _resources[_resources.GetCount() - 1];
        if (NULL != _styleNode)
        {
            resNode->AddResItem(key, _styleNode);
        }
    }

    // 这可能是用户自定义的控件样式
    if (sty->GetTargetType() == NULL)
    {
        ;
    }

    _setters.RemoveAt(_setters.GetCount() - 1);
    _styleNode->unref();
    _styleNode = NULL;
}

void XamlWalker::OnStartTrigger(ControlTemplate* temp, Trigger* trg, IXamlNode* node)
{
    _trigger = new TriggerNode();
    _trigger->ref();

    if (NULL == temp)
    {
        _styleNode->AddTrigger(_trigger);
    }
    else
    {
        _rootTemplate->GetTriggerCollection()->AddTriggerNode(_trigger);
    }
    _setters.Add(_trigger->GetSetterCollection());
}

void XamlWalker::OnFinishTrigger(ControlTemplate* temp, Trigger* trg, IXamlNode* node)
{
    _trigger->AddValue(trg->GetProp(), trg->GetValue());

    _setters.RemoveAt(_setters.GetCount() - 1);
    _trigger->unref();
    _trigger = NULL;
}

void XamlWalker::OnStartMultiTrigger(ControlTemplate* temp, MultiTrigger* trg, IXamlNode* node)
{
    _trigger = new TriggerNode();
    _trigger->ref();

    if (NULL == temp)
    {
        _styleNode->AddTrigger(_trigger);
    }
    else
    {
        _rootTemplate->GetTriggerCollection()->AddTriggerNode(_trigger);
    }

    _setters.Add(_trigger->GetSetterCollection());
}

void XamlWalker::OnFinishMultiTrigger(ControlTemplate* temp, MultiTrigger* trg, IXamlNode* node)
{
    _trigger->SetArrayCondition(trg->GetConditions());
    _setters.RemoveAt(_setters.GetCount() - 1);
    _trigger->unref();
    _trigger = NULL;
}

void XamlWalker::OnStartReadSetter(Object* owner, WalkerFrom eFrom, const String& nodeName, DpProperty* dp)
{
    if (dp == suic::FrameworkElement::StyleProperty)
    {
        _styleFromSetter = true;
        if (NULL != _styleNode)
        {
            _styleNode->unref();
            _styleNode = NULL;
        }

        _styleNode = new StyleNode();
        _styleNode->ref();
        _setters.Add(_styleNode->GetSetterCollection());
    }
}

void XamlWalker::OnReadSetter(Object* owner, WalkerFrom eFrom, const String& targetName, const String& name, const String& val, DpProperty* dp, const ResourceItem& resItem)
{
    bool bFromStyleProp = false;
    SetterNode* pSetter = NULL;
    SetterCollectionNode* pSetters = NULL;

    if (dp == suic::FrameworkElement::StyleProperty && _styleFromSetter)
    {
        bFromStyleProp = true;
        _setters.RemoveAt(_setters.GetCount() - 1);
    }

    pSetters = _setters[_setters.GetCount() - 1];

    if (bFromStyleProp)
    {
        suic::Style* setterStyle = RTTICast<suic::Style>(resItem.res.get());
        _styleNode->SetStyle(_U(""), setterStyle);
        pSetter = new SetterNode(name, dp, _styleNode);

        _styleFromSetter = false;
        _styleNode->unref();
        _styleNode = NULL;
    }
    else if (resItem.type == Resource::ResType::resLayout)
    {
        if (_propLayout->GetNodeElement() == resItem.res.get())
        {
            pSetter = new SetterNode(name, dp, _propLayout);
        }
        else
        {
            suic::InvalidValueException e(_U("Invalidate layout property"), __FILELINE__);
        }
    }
    else if (resItem.type == Resource::ResType::resFrameworkTemplate)
    {
        if (_rootTemplate->GetFrameworkTemplate() == resItem.res.get())
        {
            pSetter = new SetterNode(name, dp, _rootTemplate);
        }
        else
        {
            suic::InvalidValueException e(_U("Invalidate FrameworkTemplate"), __FILELINE__);
        }
    }
    else if (dp == suic::Image::SourceProperty)
    {
        suic::Bitmap* bmp = RTTICast<suic::Bitmap>(resItem.res.get());
        if (NULL != bmp)
        {
            SourceResNode* sourceRes = new SourceResNode(bmp);
            pSetter = new SetterNode(name, dp, sourceRes);
            sourceRes->GetResPath()->Parse(val);
        }
        else
        {
            pSetter = new SetterNode(name, dp, resItem.res.get());
        }
    }
    else
    {
        pSetter = new SetterNode(name, dp, resItem.res.get());
    }

    if (NULL != pSetter)
    {
        pSetter->ref();
        pSetters->AddSetter(pSetter);
        pSetter->SetTargetName(targetName);
        pSetter->unref();
    }
}

void XamlWalker::OnReadRes(ResourceDictionary* res, WalkerFrom eFrom, const String& key, const ResourceItem& resItem, IXamlNode* node)
{
    ResourceDictionaryNode* resNode = _resources[_resources.GetCount() - 1];

    if (resItem.type == Resource::ResType::resLayout)
    {
        if (_propLayout->GetNodeElement() == resItem.res.get())
        {
            resNode->AddResItem(key, _propLayout);
        }
        else
        {
            throw suic::InvalidValueException(_U("Invalidate layout property"), __FILELINE__);
        }
    }
    else if (resItem.type == Resource::ResType::resStyle)
    {
    }
    else if (resItem.type == Resource::ResType::resFrameworkTemplate)
    {
        if (_rootTemplate->GetFrameworkTemplate() == resItem.res.get())
        {
            resNode->AddResItem(key, _rootTemplate);
        }
        else
        {
            throw suic::InvalidValueException(_U("Invalidate FrameworkTemplate"), __FILELINE__);
        }
    }
    else
    {
        if (resItem.res.get()->GetRTTIType()->InheritFrom(suic::Bitmap::RTTIType()))
        {
            suic::Bitmap* pBmp = suic::RTTICast<suic::Bitmap>(resItem.res.get());
            suic::String strVal = node->FindAttri(_U("Source"));
            resNode->AddResItem(key, new BitmapResNode(pBmp, strVal));
        }
        else if (resItem.res.get()->GetRTTIType()->InheritFrom(suic::Effect::RTTIType()))
        {
            resNode->AddResItem(key, new BaseResNode(resItem.res.get()));
        }
        else
        {
            resNode->AddResItem(key, resItem.res.get());
        }
    }
}

//=================================================
// TemplateXamlWalker

TemplateXamlWalker::TemplateXamlWalker(TemplateRootItem* root)
{
    _root = root;
}

TemplateXamlWalker::~TemplateXamlWalker()
{
}

bool TemplateXamlWalker::InstantiateTree(FrameworkElement* parent)
{
    DesignElement* xamlRoot = _root->GetRootElement();
    if (xamlRoot->GetUIElement() == NULL)
    {
        _root->GetFrameworkTemplate()->InstantiateTree(parent);
    }
    return true;
}

void TemplateXamlWalker::OnStartWalk(FrameworkTemplate* ft)
{

}

void TemplateXamlWalker::OnFinishWalk(FrameworkTemplate* ft)
{
    //suic::DesignHelper::RegisterTemplateXamlWalker(NULL, 0);
}

void TemplateXamlWalker::OnStartWalkElement(DpObject* d, FEFactory* fef)
{
    DesignElement* tempRoot = _root->GetRootTemplateElement();

    DesignHelper::SetInDesignMode(d);
    if (NULL != tempRoot)
    {
        SetElementOnTemplate(tempRoot, d, fef);
    }
}

void TemplateXamlWalker::OnEndWalkElement(DpObject* elem, FEFactory* fef)
{

}

bool TemplateXamlWalker::SetElementOnTemplate(DesignElement* tempRoot, DpObject* elem, FEFactory* fef)
{
    if (tempRoot->GetFrameworkElementFactory() == fef)
    {
        tempRoot->SetNodeElement(elem);
        return true;
    }

    for (int i = 0; i < tempRoot->GetCount(); ++i)
    {
        DesignElement* child = RTTICast<DesignElement>(tempRoot->GetItem(i));
        if (SetElementOnTemplate(child, elem, fef))
        {
            return true;
        }
    }
    return false;
}
