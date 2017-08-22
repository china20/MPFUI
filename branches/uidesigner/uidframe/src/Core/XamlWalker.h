
#ifndef _XAMLNODEWALKER_H_
#define _XAMLNODEWALKER_H_

#include <Core/StyleNode.h>
#include <Main/XamlRootItem.h>

#include <System/Graphics/Resource.h>
#include <System/Resources/IXamlWalker.h>

class XamlElementNodeArray : public Array<DesignElement*>
{
public:

    XamlElementNodeArray()
    {

    }

    ~XamlElementNodeArray()
    {
        Clear();
    }

    void Add(DesignElement* elem)
    {
        if (NULL != elem)
        {
            elem->ref();
        }
        Array::Add(elem);
    }

    void RemoveAt(int index)
    {
        DesignElement* elem = GetItem(index);
        if (NULL != elem)
        {
            elem->unref();
        }
        Array::RemoveAt(index);
    }

    void Clear()
    {
        for (int i = 0; i < GetCount(); ++i)
        {
            DesignElement* elem = GetItem(i);
            if (NULL != elem)
            {
                elem->unref();
            }
        }
        Array::Clear();
    }
};

/// <summary>
///  资源布局遍历对象，界面引擎读取资源布局时回调IXamlWalker，然后
///  我们保存对应的节点信息
/// </summary>
class XamlWalker : public suic::IXamlWalker
{
public:

    XamlWalker(XamlRootItem* root);
    ~XamlWalker();

    virtual void OnStartWalk(Object* obj);
    virtual void OnFinishWalk(Object* obj);

    virtual void OnStartNode(Object* elem, IXamlNode* node, String data);
    virtual void OnFinishNode(Object* elem, IXamlNode* node, String data);

    virtual void OnStartApp(Application* app, IXamlNode* node);
    virtual void OnFinishApp(Application* app, IXamlNode* node);

    virtual void OnStartAppWindow(Application* app, FrameworkElement* mainWnd);
    virtual void OnFinishAppWindow(Application* app, FrameworkElement* mainWnd);

    virtual void OnStartElement(DpObject* d, IXamlNode* node, bool bFromElem);
    virtual void OnFinishElement(DpObject* d, IXamlNode* node, bool bFromElem);

    virtual void OnStartReadLayout(Object* elem, IXamlNode* node);
    virtual void OnFinishReadLayout(Object* elem, const ResourceItem& res, IXamlNode* node);

    //------------------------------------------------------------

    void OnStartTemplate(FrameworkTemplate* ft, IXamlNode* node);
    void OnFinishTemplate(FrameworkTemplate* ft, IXamlNode* node);
    void OnStartControlTemplateTrigger(FrameworkTemplate* ft, IXamlNode* node);
    void OnFinishControlTemplateTrigger(FrameworkTemplate* ft, IXamlNode* node);

    void OnStartTemplateElement(FEFactory* fef, IXamlNode* node);
    void OnFinishTemplateElement(FEFactory* fef, IXamlNode* node);

    void OnReadTemplateElementCommand(FEFactory* fef, const String& val, IXamlNode* node);

    //------------------------------------------------------------

    virtual void OnStartResDic(ResourceDictionary* res, IXamlNode* node, Byte bMerged);
    virtual void OnFinishResDic(ResourceDictionary* res, IXamlNode* node, Byte bMerged);

    virtual void OnStartRefResDic(ResourceDictionary* res, IXamlNode* node, Byte bMerged);
    virtual void OnFinishRefResDic(ResourceDictionary* res, IXamlNode* node, Byte bMerged);

    virtual void OnStartResDicSource(const String& strSource, ResourceDictionary* res, IXamlNode* node, Byte bMerged);
    virtual void OnFinishResDicSource(const String& strSource, ResourceDictionary* res, IXamlNode* node, Byte bMerged);

    virtual void OnStartMergedDictionaries(ResourceDictionary* res, IXamlNode* node);
    virtual void OnFinishMergedDictionaries(ResourceDictionary* res, IXamlNode* node);

    virtual void OnStartMergedResDic(ResourceDictionary* res, ResourceDictionary* mergedRes, IXamlNode* node);
    virtual void OnFinishMergedResDic(ResourceDictionary* res, ResourceDictionary* mergedRes, IXamlNode* node);

    virtual void OnStartStyle(ResourceDictionary* res, const String& key, Style* sty, IXamlNode* node);
    virtual void OnFinishStyle(ResourceDictionary* res, const String& key, Style* sty, IXamlNode* node);

    virtual void OnStartTrigger(ControlTemplate* temp, Trigger* trg, IXamlNode* node);
    virtual void OnFinishTrigger(ControlTemplate* temp, Trigger* trg, IXamlNode* node);

    virtual void OnStartMultiTrigger(ControlTemplate* temp, MultiTrigger* trg, IXamlNode* node);
    virtual void OnFinishMultiTrigger(ControlTemplate* temp, MultiTrigger* trg, IXamlNode* node);

    virtual void OnStartReadSetter(Object* owner, WalkerFrom eFrom, const String& nodeName, DpProperty* dp);

    virtual void OnReadSetter(Object* owner, WalkerFrom eFrom, const String& targetName, const String& name, const String& val, DpProperty* dp, const ResourceItem& res);
    virtual void OnReadRes(ResourceDictionary* res, WalkerFrom eFrom, const String& key, const ResourceItem& resItem, IXamlNode* node);

protected:

    DesignElement* CreateElementNode(DpObject* d, IXamlNode* node);

protected:

    Project* _project;
    XamlRootItem* _root;

    DesignElement* _elemRoot;
    ApplicationNode* _appRoot;
    ResourceDictionaryNode* _resRoot;

    bool _elemAsRoot;
    bool _styleFromSetter;

    StyleNode* _styleNode;
    TriggerNode* _trigger;

    TemplateRootItem* _rootTemplate;
    DesignElement* _propLayout;

    MergedDictionariesNode* _mergedDics;

    XamlElementNodeArray _elems;
    Array<SetterCollectionNode*> _setters;
    Array<ResourceDictionaryNode*> _resources;
};

/// <summary>
///  模版遍历对象，界面引擎读取模版节点时回调ITempXamlWalker，然后
///  我们保存对应的节点信息
/// </summary>
class TemplateXamlWalker : public suic::ITempXamlWalker
{
public:

    TemplateXamlWalker(TemplateRootItem* root);
    ~TemplateXamlWalker();

    bool InstantiateTree(FrameworkElement* parent);

    virtual void OnStartWalk(FrameworkTemplate* ft);
    virtual void OnFinishWalk(FrameworkTemplate* ft);

    virtual void OnStartWalkElement(DpObject* elem, FEFactory* fef);
    virtual void OnEndWalkElement(DpObject* elem, FEFactory* fef);

protected:

    bool SetElementOnTemplate(DesignElement* tempRoot, DpObject* elem, FEFactory* fef);

private:

    TemplateRootItem* _root;
};

#endif
