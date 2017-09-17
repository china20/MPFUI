// 华勤科技版权所有 2008-2022
// 
// 文件名：XamlWalker.h
// 功  能：读取资源。
// 
// 作  者：MPF开发组
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UIXAMLWALKER_H_
#define _UIXAMLWALKER_H_

#include <System/Tools/ResourceParser.h>

#include <Framework/Controls/Grid.h>
#include <Framework/Controls/Track.h>
#include <Framework/Controls/Application.h>

namespace suic 
{

struct ReadContext
{
    suic::DpObject* d;    
    suic::IAddChild* ac;
    suic::FrameworkElement* fe;
};

struct ExtItem
{
    DpProperty* dp;
    Extension* ext;
    Resource::ResType type;
};

class XamlWalker
{
public:

    XamlWalker();
    ~XamlWalker();

    suic::Object* GetRoot()
    {
        return _root.get();
    }

    bool WalkUri(Element* parent, Object* component, const String& strUri);
	bool WalkMemory(Element* parent, Object* component, const Byte* data, int size);
    bool WalkNode(Element* parent, Object* component, IXamlNode* pNode);

private:

    void CreateElement(IXamlNode* pNode, const String& name, ObjectPtr& child);
	void WalkXamlElement(suic::FrameworkElement* fe, ReadContext& rCtx, IXamlNode* pNode);

    void ReadXamlProperties(ReadContext& rCtx, IXamlNode* pNode, Array<ExtItem, false>& extProps);

    void ReadResourceDictionaryFromNode(IXamlNode* pNode);

	void ReadXamlApplication(suic::Application* app, IXamlNode* pNode);
    void ReadElementResource(suic::FrameworkElement* fe, IXamlNode* pNode);

    void ReadMergedDictionaries(suic::FrameworkElement* fe, suic::ResourceDictionary* resDic, IXamlNode* pNode);

    void ReadBaseMetaResource(suic::RTTIOfInfo* trInfo, suic::FrameworkElement* fe, const suic::String& strName, IXamlNode* pNode, ResourceItem& resItem);
	void ReadMetaResource(suic::RTTIOfInfo* trInfo, suic::FrameworkElement* fe, const suic::String& strName, IXamlNode* pNode, ResourceItem& resItem, String& strKey, bool fromTemp);

    void ReadXamlResource(suic::FrameworkElement* fe, suic::ResourceDictionary* resDic, IXamlNode* pNode, bool fromElem);

	void ReadMergedResourceDictionaryFromUri(suic::ResourceDictionary* resDic, const suic::String& strSource);
    bool ReadResourceDictionaryFromUri(const suic::String& strSource, suic::ResourceDictionary* resDic);

    suic::String ParseKey(const String& strType);
    suic::DpProperty* ParseProp(suic::RTTIOfInfo* rInfo, const String& strProp);
    suic::Object* DoStaticResource(suic::FrameworkElement* fe, ResourceItem& resItem);

    suic::Object* FindRes(suic::FrameworkElement* elem, const suic::String& name);

    //--------------------------------------------------

    void WalkTemplateElement(suic::FrameworkElement* fe, IXamlNode* pNode, suic::FEFactory* feFactory);
    void WalkTemplateElementProperties(suic::FrameworkElement* fe, IXamlNode* pNode, suic::FEFactory* feFactory);
    void ReadTemplateResource(suic::RTTIOfInfo* trInfo, suic::FrameworkElement* fe, IXamlNode* pNode, ResourceItem& resItem, bool fromControlTemp);

    //--------------------------------------------------

    void ReadStyle(suic::FrameworkElement* fe, IXamlNode* pNode, ResourceItem& resItem, String& strKey);

    void ReadTriggers(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, suic::TriggerCollection* trgColl, IXamlNode* pNode, FrameworkTemplate* ft);
    void ReadSetter(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, suic::SetterCollection* setterColl, IXamlNode* pNode, FrameworkTemplate* ft);

    void ReadTrigger(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, suic::TriggerCollection* trgColl, IXamlNode* pNode, FrameworkTemplate* ft);
    void ReadMultiTrigger(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, suic::TriggerCollection* trgColl, IXamlNode* pNode, FrameworkTemplate* ft);
    void ReadMultiTriggerConditions(RTTIOfInfo* tcls, MultiTrigger* pTrg, IXamlNode* node, FrameworkTemplate* ft);

    ResourceDictionary* ParseElementResource(IXamlNode* pNode, String& strSource);

private:

    suic::Application* _app;
    suic::Window* _mainWnd;
    suic::ObjectPtr _root;
    suic::ResourceDictionary* _resDic;

    IXamlDoc* _xamlDoc;
};

class WalkTemplate
{
public:

    WalkTemplate(FEFactory* feRoot, int iCount);
    AutoObj InstantiateTree(DpObject* container, DpObject* parent);

private:

    bool InstantiateChildren(FEFactory* fef, DpObject* container, DpObject* parent, FrameworkElement* elem);
    
    void WalkGrid(FEFactory* fef, FrameworkElement* elem);
    void WalkTrack(FEFactory* child, FrameworkElement* elem);

private:

    XArray* _bindColl;
    int _count;
    FEFactory* _rootTemplate;
};

}

#endif
