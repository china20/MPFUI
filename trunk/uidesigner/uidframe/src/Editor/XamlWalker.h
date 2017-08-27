// 华勤科技版权所有 2010-2011
// 
// 文件名：XamlWalker.h
// 功  能：读取资源。
// 
// 作  者：MPF开发组
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UIDXAMLWALKER_H_
#define _UIDXAMLWALKER_H_

#include <System/Tools/ResourceParser.h>
#include <System/Tools/CollectionX.h>
#include <System/Windows/FrameworkElementFactory.h>

#include <Framework/Controls/Grid.h>
#include <Framework/Controls/Track.h>
#include <Framework/Controls/Application.h>

#include <Core/XamlElementNode.h>
#include <Core/TriggerCollectionNode.h>
#include <Core/ResourceDictionaryNode.h>

class Project;

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

struct DResourceItem
{
    String dKey;
    ObjectPtr dItem;
    ResourceItem resItem;
};

struct DSetterColl
{
    SetterCollection* setColl;
    SetterCollectionNode* dSetColl;
};

struct DTriggerColl
{
    TriggerCollection* trgColl;
    TriggerCollectionNode* dTrgColl;
};

struct DResourceDic
{
    ResourceDictionary* resDic;
    ResourceDictionaryNode* dResDic;
};

class XamlWalker
{
public:

    XamlWalker(Project* pPrj);
    ~XamlWalker();

    static DesignElement* CreateDesignElement(suic::RTTIOfInfo* typeInfo);

    void SetOnlyReadAppRes(bool val)
    {
        _onlyReadAppRes = val;
    }

    suic::Object* GetRoot()
    {
        return _root.get();
    }

    DesignElement* GetDesignElement()
    {
        return _dElem;
    }

    ResourceDictionaryNode* GetDesignResDic()
    {
        return _dResDic;
    }

    bool WalkUri(Object* dObj, Object* component, const String& strUri);
    bool WalkMemory(Object* dObj, Object* component, const char* strXml, int size);

    bool ReadTemplateRes(RTTIOfInfo* targetInfo, suic::FrameworkElement* fe, IXamlNode* pNode, ResourceItem& resItem);
    bool ReadStyleRes(suic::FrameworkElement* fe, IXamlNode* pNode, ResourceItem& resItem);

private:

    Project* GetProject() const;

    bool HandleXamlDoc(Object* dObj, Object* component);

    void WalkUserXamlElement(DesignElement* parent, ReadContext& rParentCtx, IXamlNode* pNode);
    void WalkXamlElement(DesignElement* dElem, suic::FrameworkElement* fe, ReadContext& rCtx, IXamlNode* pNode);

    void ReadXamlProperties(SetterCollectionNode* dSetColl, ReadContext& rCtx, IXamlNode* pNode, Array<ExtItem>* extProps);

    void ReadResourceDictionaryFromNode(IXamlNode* pNode);

    void ReadXamlApplication(ApplicationNode* dApp, suic::Application* app, IXamlNode* pNode);
    void ReadElementResource(ResourceDictionaryNode* dResNode, suic::FrameworkElement* fe, IXamlNode* pNode);

    void ReadMergedDictionaries(suic::FrameworkElement* fe, DResourceDic* resDic, IXamlNode* pNode);
    void ReadBaseMetaResource(suic::RTTIOfInfo* trInfo, suic::FrameworkElement* fe, const suic::String& strName, IXamlNode* pNode, DResourceItem& resItem, bool fromTemp);
    void ReadMetaResource(suic::RTTIOfInfo* trInfo, suic::FrameworkElement* fe, const suic::String& strName, IXamlNode* pNode, DResourceItem& resItem, bool fromTemp);

    void ReadXamlResource(suic::FrameworkElement* fe, DResourceDic* resDic, IXamlNode* pNode, bool fromElem);

    void ReadMergedResourceDictionaryFromUri(DResourceDic* resDic, const suic::String& strSource);
    bool ReadResourceDictionaryFromUri(const suic::String& strSource, DResourceDic* resDic);

    suic::String ParseKey(const String& strType);
    suic::DpProperty* ParseProp(suic::RTTIOfInfo* rInfo, const String& strProp);
    suic::Object* DoStaticResource(suic::FrameworkElement* fe, ResourceItem& resItem);

    suic::Object* FindRes(suic::FrameworkElement* elem, const suic::String& name);

    //--------------------------------------------------

    void WalkTemplateElement(DesignElement* dElem, suic::FrameworkElement* fe, IXamlNode* pNode, suic::FEFactory* feFactory);
    void WalkTemplateElementProperties(SetterCollectionNode* dSetColl, suic::FrameworkElement* fe, IXamlNode* pNode, suic::FEFactory* feFactory);
    bool ReadTemplateResource(suic::RTTIOfInfo* trInfo, suic::RTTIOfInfo* targetInfo, suic::FrameworkElement* fe, IXamlNode* pNode, DResourceItem& dResItem);

    //--------------------------------------------------

    void ReadStyle(suic::FrameworkElement* fe, IXamlNode* pNode, DResourceItem& resItem);

    void ReadSetter(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, DSetterColl* setterColl, IXamlNode* pNode, FrameworkTemplate* ft);

    void ReadTriggers(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, DTriggerColl* trgColl, IXamlNode* pNode, FrameworkTemplate* ft);
    void ReadTrigger(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, DTriggerColl* trgColl, IXamlNode* pNode, FrameworkTemplate* ft);
    void ReadMultiTrigger(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, DTriggerColl* trgColl, IXamlNode* pNode, FrameworkTemplate* ft);
    void ReadMultiTriggerConditions(RTTIOfInfo* tcls, TriggerNode* dTrg, MultiTrigger* pTrg, IXamlNode* node, FrameworkTemplate* ft);

    DResourceDic ParseElementResource(ResourceDictionaryNode* dResDic, IXamlNode* pNode, String& strSource);

private:

    bool OnReadingTrack(DesignTrack* dTrack, Track* pTrack, IXamlNode* node);

    bool OnReadingGrid(DesignGrid* dGrid, Grid* pGrid, IXamlNode* node);
    void ReadGridRowDefs(DesignGrid* dGrid, Grid* pGrid, IXamlNode* pNode);
    void ReadGridColumnDefs(DesignGrid* dGrid, Grid* pGrid, IXamlNode* pNode);

    void SetInDesignMode(DpObject* d);

private:

    bool _inDesignMode;
    bool _onlyReadAppRes;

    Project* _project;

    suic::Application* _app;
    suic::Window* _mainWnd;
    suic::ObjectPtr _root;
    suic::ResourceDictionary* _resDic;

    DesignElement* _dElem;
    ApplicationNode* _dApp;
    ResourceDictionaryNode* _dResDic;

    IXamlDoc* _xamlDoc;
};

class WalkTemplate
{
public:

    WalkTemplate(FEFactory* feRoot, int iCount);
    AutoObj InstantiateTree(DpObject* container, DpObject* parent, FrameworkElement* logicalParent);

private:

    bool InstantiateChildren(FEFactory* fef, DpObject* container, DpObject* parent, FrameworkElement* logicalParent, FrameworkElement* elem);

    void WalkGrid(FEFactory* fef, FrameworkElement* elem);
    void WalkTrack(FEFactory* child, FrameworkElement* elem);

private:

    XArray* _bindColl;
    int _count;
    FEFactory* _rootTemplate;
};

}

#endif
