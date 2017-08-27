/////////////////////////////////////////////////////
// XamlRootItem.cpp

#include "stdafx.h"
#include "Project.h"
#include "DesignPanel.h"
#include "FilterNode.h"
#include "XamlRootItem.h"
#include "SlnTreeManager.h"

#include <Editor/XamlWalker.h>
#include <Core/XamlRedoUndo.h>
#include <Core/StyleNode.h>

#include <Main/ElementTreeManager.h>

/////////////////////////////////////////////////////
// RootItem

ImplementRTTIOfClass(RootItem, FilterNode)
ImplementRTTIOfClass(FileRootItem, RootItem)
ImplementRTTIOfClass(ElementRootItem, FileRootItem)
ImplementRTTIOfClass(ApplicationRootItem, FileRootItem)
ImplementRTTIOfClass(ResourceDicRootItem, FileRootItem)
ImplementRTTIOfClass(TemplateRootItem, ElementRootItem)

RootItem::RootItem()
    : _objTree(NULL)
{
    _isModified = false;
    _isLoaded = false;
    _project = NULL;
}

RootItem::RootItem(const String& name)
    : FilterNode(name)
    , _project(NULL)
    , _objTree(NULL)
{
    _isModified = false;
    _isLoaded = false;
}

RootItem::~RootItem()
{

}

suic::String RootItem::GetResXml(const String& offset)
{
    return suic::String();
}

String RootItem::ToString()
{
    if (GetKey().Empty())
    {
        if (IsModified())
        {
            return GetName() + _U("*");
        }
        else
        {
            return GetName();
        }
    }
    else
    {
        return ResNode::ToString();
    }
}

bool RootItem::IsModified() const
{
    return _isModified;
}

void RootItem::SetModified(bool val)
{
    if (_isModified != val)
    {
        _isModified = val;
        OnModifiedChanged();
    }
}

Project* RootItem::GetProject() const
{
    if (NULL == _project)
    {
        return FilterNode::GetProject();
    }
    else
    {
        return _project;
    }
}

void RootItem::SetProject(Project* pPrj)
{
    _project = pPrj;
}

String RootItem::GetFileName() const
{
    return GetName();
}

void RootItem::SetFileName(const String& name)
{
    ResourceUri resPath(name);
    String filename = resPath.GetResourcePath();

    for (int i = name.Length() - 1; i >= 0; --i)
    {
        if (name[i] == '\\' || name[i] == '/')
        {
            filename = name.Substring(i + 1);
            SetName(filename);
            return;
        }
    }

    SetName(name);
}

ResourceUri RootItem::GetResourceUri() const
{
    suic::String strPath = GetRelativePath();
    ResourceUri resPath;
    Project* pPrj = GetProject();

    if (pPrj != NULL)
    {
        resPath.SetComponent(pPrj->GetProjectName());
        resPath.SetResourcePath(strPath);
    }
    return resPath;
}

void RootItem::SetObjTreeManager(ObjTreeManager* objTree)
{
    _objTree = objTree;
}

ObjTreeManager* RootItem::GetObjTreeManager() const
{
    return _objTree;
}

bool RootItem::IsLoaded() const
{
    return _isLoaded;
}

void RootItem::Close()
{
    OnClose();
    _isLoaded = false;
    UpdateDisplayName();
}

bool RootItem::Load(bool onlyReadAppRes)
{
    if (!_isLoaded)
    {
        _isLoaded = OnLoad(onlyReadAppRes);
    }
    return _isLoaded;
}

bool RootItem::CheckLoadState()
{
    if (!_isLoaded)
    {
        _isLoaded = OnLoad(false);
    }

    return _isLoaded;
}

void RootItem::ReloadOnMemory()
{
    OnLoadFromXml();
}

bool RootItem::OnLoadFromXml()
{
    return false;
}

void RootItem::SetLoaded()
{
    _isLoaded = true;
}

void RootItem::OnModifiedChanged()
{
    UpdateDisplayName();
}

//========================================================
// ElementRootItem

ElementRootItem::ElementRootItem()
{
    _rootItem = NULL;
    _rootData = NULL;
    _isTemplate = false;
    _refreshDirty = false;
}

ElementRootItem::ElementRootItem(DesignElement* elem)
{
    _rootItem = NULL;
    _rootData = NULL;
    _isTemplate = false;
    _refreshDirty = false;

    UpdateRootElement(elem);
}

ElementRootItem::~ElementRootItem()
{
    if (NULL != _rootItem)
    {
        _rootItem->unref();
    }

    if (NULL != _rootData)
    {
        delete _rootData;
    }
}

SetterNode* ElementRootItem::SetElementValue(DesignElement* elem, DpProperty* dp, Object* val)
{
    ObjectPtr cacheVal = val;

    if (NULL != elem)
    {
        SetterCollectionNode* pSetters = elem->GetSetterCollection();
        SetterNode* pSetter = pSetters->FindSetter(dp->GetName());

        if (NULL == pSetter)
        {
            pSetter = new SetterNode(dp->GetName(), dp, val);
            pSetters->AddSetter(pSetter);
        }
        else
        {
            pSetter->SetValue(val);
        }
        elem->GetUIElement()->SetValue(dp, val);
        return pSetter;
    }
    else
    {
        return NULL;
    }
}

void ElementRootItem::SetStartElement()
{
    if (GetProject() != NULL)
    {
        ApplicationRootItem* appRootItem = GetProject()->GetApplicationRootItem();

        if (NULL != appRootItem && appRootItem->GetMainRootItem() != this)
        {
            appRootItem->SetMainRootItem(this, true);
        }
    }
}

void ElementRootItem::RemoveStartElement()
{
    if (GetProject() != NULL)
    {
        ApplicationRootItem* appRootItem = GetProject()->GetApplicationRootItem();

        if (NULL != appRootItem && appRootItem->GetMainRootItem() == this)
        {
            appRootItem->SetMainRootItem(NULL, true);
        }
    }
}

void ElementRootItem::UpdateRootElement(DesignElement* elem)
{
    if (elem != _rootItem)
    {
        if (NULL != _rootItem)
        {
            _rootItem->unref();
        }

        _rootItem = elem;

        if (NULL != _rootItem)
        {
            _rootItem->ref();
        }

        if (NULL != _rootData)
        {
            delete _rootData;
            _rootData = NULL;
        }

        if (NULL != _rootItem)
        {
            _rootData = new RootItemData();
            _isTemplate = _rootItem->IsTemplate();
        }
    }
}

bool ElementRootItem::IsTemplate() const
{
    return _isTemplate;
}

suic::String ElementRootItem::GetResXml(const String& offset)
{
    suic::String strXml;
    if (_rootItem != NULL)
    {
        strXml = _rootItem->GetResXml(offset);
    }
    return strXml;
}

bool ElementRootItem::IsTemplateRoot()
{
    return false;
}

/*void XamlRootItem::SetElementRootItem(Object* rootItem)
{
    if (_rootItem != NULL)
    {
        _rootItem->unref();
    }

    _rootItem = rootItem;

    if (NULL != _rootItem)
    {
        // 设置已经load标志
        SetLoaded();

        _rootItem->ref();

        if (RTTICast<XamlElementNode>(_rootItem) != NULL)
        {
            if (NULL != _rootData)
            {
                delete _rootData;
            }
            _rootData = new RootItemData();
        }
    }
}*/

FrameworkElement* ElementRootItem::GetUIElement()
{
    if (NULL == GetRootElement())
    {
        return NULL;
    }
    return GetRootElement()->GetUIElement();
}

DesignElement* ElementRootItem::GetHotElement() const
{
    return _rootData->hot;
}

DesignElement* ElementRootItem::GetFocusElement() const
{
    if (_rootData->GetSelectedCount() > 0)
    {
        return _rootData->GetSelectedElement(0);
    }
    return NULL;
}

void ElementRootItem::SetHotElement(DesignElement* elem)
{
    if (NULL != _rootData)
    {
        _rootData->hot = elem;
    }
}

void ElementRootItem::SetFocusElement(DesignElement* elem)
{
    if (NULL != _rootData)
    {
        _rootData->SetFocused(elem);
    }
}

RootItemData* ElementRootItem::GetRootItemData() const
{
    return _rootData;
}

void ElementRootItem::Refresh()
{
    if (_refreshDirty)
    {
        FrameworkElement* fe = GetUIElement();
        if (NULL != fe)
        {
            Element* pParent = fe->GetUIParent();
            if (NULL != pParent)
            {
                pParent->RemoveVisualChild(fe);
                GetRootElement()->RefreshDesignElement();
                pParent->AddVisualChild(fe);
            }
            else
            {
                GetRootElement()->RefreshDesignElement();
            }
        }
        _refreshDirty = false;
    }
}

void ElementRootItem::SetRefreshFlag(bool val)
{
    _refreshDirty = true;
}

DesignElement* ElementRootItem::GetRootElement()
{
    if (NULL == _rootItem)
    {
        UpdateRootElement(new DesignElement());
    }
    return _rootItem;
}

void ElementRootItem::Reset()
{
    if (NULL != _rootItem)
    {
        _rootItem->unref();
        _rootItem = NULL;
    }

    if (NULL != _rootData)
    {
        delete _rootData;
        _rootData = NULL;
    }

    _isLoaded = false;
    _isModified = false;
}

void ElementRootItem::ResetRootItem(bool bSetLoaded)
{
    Close();

    if (bSetLoaded)
    {
        SetLoaded();
    }
}

bool ElementRootItem::Save()
{
    if (Utils::CheckUseTimeout(_U("这是Beta版本,请重新启动!")))
    {
        return false;
    }

    if (GetRootElement() == NULL || !IsLoaded())
    {
        suic::Toast::Show(_U("保存失败"), 3);
        return false;
    }
    else
    {
        String strXml;

        strXml += _U("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
        strXml += GetResXml(_U(""));

        FileWriter fs;
        String strFile = GetFullPath();

        if (fs.Open(strFile))
        {
            fs.Write(strXml);
            fs.Close();
            SetModified(false);

            Project* pPrj = GetProject();
            if (NULL != pPrj)
            {
                pPrj->BackupRootItem(this);
            }
        }

        suic::Toast::Show(_U("保存成功"), 3);

        return true;
    }
}

bool ElementRootItem::SaveAs(const String& strPath)
{
    return true;
}

bool ElementRootItem::OnLoad(bool onlyReadAppRes)
{
    String strPath = GetFullPath();
    XamlLoader xamlLoader;

    xamlLoader.LoadElementRootXaml(this, strPath);
    return GetRootElement() != NULL;
}

bool ElementRootItem::OnLoadFromXml()
{
    String strName = GetFileName();
    Project* pPrj = GetProject();

    if (NULL != pPrj)
    {
        suic::Mulstr strXml = GetResXml(_U("")).c_str();
        XamlLoader xamlLoader;
        bool isCurrent = pPrj->EqualCurrentDesignRootElement(this);

        OnClose();

        strXml.EncodeUtf8();
        xamlLoader.LoadElementRootXamlFromMemory(this, strXml.c_str(), strXml.Length());

        if (isCurrent)
        {
            pPrj->SwitchRootElement(NULL, this);
        }

        return GetRootElement() != NULL;
    }
    else
    {
        return false;
    }
}

void ElementRootItem::OnClose()
{
    Project* pPrj = GetProject();

    if (NULL != pPrj)
    {
        pPrj->CheckAndCloseRootElement(this);
    }

    if (_rootData != NULL)
    {
        _rootData->ruMana.Clear();
        _rootData->ClearSelectedElements();
        _rootData->hot = NULL;
    }

    Reset();
}

//====================================================
// 布局元素函数

void ElementRootItem::UpdateDesignWindow()
{
    DesignPanel* designPanel = GetProject()->GetSlnTreeManager()->GetObjTreeManager()->GetDesignPanel();
    designPanel->RefleshDesignPanel();
}

void ElementRootItem::AddChileElement(DesignElement* pParent, DesignElement* pChild, bool bCallRedo)
{
    if (_rootData != NULL)
    {
        BaseRedoUndo* pRedoUndo = NULL;
        DesignContent* pCont = RTTICast<DesignContent>(pParent);

        if (NULL != pCont)
        {
            pRedoUndo = new AddContentRedoUndo(this, pCont, pChild);
        }
        else
        {
            pRedoUndo = new AddElementRedoUndo(this, pParent, pChild);
        }

        if (!bCallRedo || pRedoUndo->Redo())
        {
            _rootData->ruMana.AddRedoUndo(pRedoUndo);
            SetModified(true);
        }
        else
        {
            delete pRedoUndo;
        }
    }
}

void ElementRootItem::ReplacePanel(DesignElement* pElem, suic::Panel* pPanel)
{
    if (_rootData != NULL)
    {
        ReplacePanelRedoUndo* pRedoUndo = new ReplacePanelRedoUndo(this, pElem, pPanel);
        if (pRedoUndo->Redo())
        {
            _rootData->ruMana.AddRedoUndo(pRedoUndo);
            SetModified(true);
        }
        else
        {
            delete pRedoUndo;
        }
    }
}

void ElementRootItem::RemoveChileElement(DesignElement* pParent, DesignElement* pChild)
{
    if (_rootData != NULL && !pParent->GetLockElement())
    {
        DeleteElementRedoUndo* pRedoUndo = new DeleteElementRedoUndo(this, pParent, pChild);
        if (pRedoUndo->Redo())
        {
            _rootData->ruMana.AddRedoUndo(pRedoUndo);
            SetModified(true);
        }
        else
        {
            delete pRedoUndo;
        }
    }
}

bool ElementRootItem::SetElementSetter(DesignElement* pElem, SetterNode* pSetter, bool fromProp)
{
    TemplateRootItem* tempElemNode = RTTICast<TemplateRootItem>(pElem);

    if (pElem->GetLockElement())
    {
        return false;
    }

    if (_rootData != NULL)
    {
        suic::FrameworkElement* fe = pElem->GetUIElement();

        DpProperty* dp = pSetter->UpdateProperty(fe->GetRTTIType());
        ObjectPtr newVal = pSetter->GetValue();
        ObjectPtr oldVal = pSetter->GetOldValue();

        suic::Extension* expr = suic::RTTICast<suic::Extension>(newVal.get());

        if (dp != suic::FrameworkElement::StyleProperty && 
            dp != suic::Control::TemplateProperty)
        {
            if (newVal.get() == oldVal.get() || 
                (newVal.get() != NULL && oldVal.get() != NULL && 
                newVal->Equals(oldVal.get())))
            {
                return false;
            }
        }

        SetterNode* oldSetter = pElem->GetSetterCollection()->FindSetter(pSetter->GetName());
        if (NULL == oldSetter)
        {
            pElem->GetSetterCollection()->AddSetter(pSetter);
        }

        if (dp == suic::FrameworkElement::StyleProperty)
        {
            StyleNode* style = RTTICast<StyleNode>(pSetter->GetResNode());
            if (NULL != style)
            {
                style->ParseStyleValue(GetProject(), pElem->GetUIElement(), newVal);
            }
        }
        else if (dp->GetPropType()->InheritFrom(FrameworkTemplate::RTTIType()))
        {
            TemplateRootItem* pTempNode = RTTICast<TemplateRootItem>(pSetter->GetResNode());
            if (NULL != pTempNode)
            {
                pTempNode->ParseTemplateValue(GetProject(), pElem->GetUIElement(), newVal);
            }
        }

        pElem->UpdateElementProperty(pSetter->GetProperty(), newVal.get());

        if (fe->GetRTTIType()->InheritFrom(ContentControl::RTTIType()) && 
            dp == ContentControl::ContentProperty)
        {
            SetElementContentRedoUndo* pRedoUndo = new SetElementContentRedoUndo(this, pElem, pSetter);
            _rootData->ruMana.AddRedoUndo(pRedoUndo);
            pElem->ClearChildren();
        }

        pSetter->UpdateOldValue(newVal.get());

        UpdateDesignWindow();
        SetModified(true);

    }

    return (tempElemNode == NULL);
}

DesignElement* ElementRootItem::HitTestElement(DesignElement* parent, Point pt)
{
    DesignElement* hitEle = NULL;

    for (int i = parent->GetCount() - 1; i >= 0; --i)
    {
        DesignElement* ele = parent->GetXamlElement(i);
        FrameworkElement* fe = ele->GetUIElement();

        if (NULL != fe && ele->GetShowElement())
        {
            bool bInElem = false;
            Rect rect = GetRenderBound(fe);
            Rect rcBounds = fe->GetDescendantBounds();

            if (fe->IsClipToBounds())
            {
                bInElem = rect.PointIn(pt);
            }
            else
            {
                bInElem = rcBounds.PointIn(pt);
            }
            if (bInElem)
            {
                hitEle = HitTestElement(ele, pt);
                if (hitEle != NULL)
                {
                    break;
                }
            }
        }
    }

    return hitEle;
}

DesignElement* ElementRootItem::HitTestElement(Point pt)
{
    Array<DesignElement*> eles;
    DesignElement* hitEle = NULL;
    DesignElement* rootEle = RTTICast<DesignElement>(_rootItem);

    eles.Resize(6);
    eles.Add(rootEle);

    while (eles.GetCount() > 0)
    {
        hitEle = eles[eles.GetCount() - 1];
        eles.RemoveAt(eles.GetCount() - 1);

        if (hitEle->GetUIElement()->GetRTTIType()->InheritFrom(suic::TextBlock::RTTIType()))
        {
            break;
        }

        for (int i = hitEle->GetCount() - 1; i >= 0; --i)
        {
            DesignElement* ele = hitEle->GetXamlElement(i);
            FrameworkElement* fe = ele->GetUIElement();

            if (NULL != fe && ele->GetShowElement())
            {
                bool bInElem = false;
                Rect rect = GetRenderBound(fe);
                Rect rcBounds = fe->GetDescendantBounds();
                if (fe->IsClipToBounds())
                {
                    bInElem = rect.PointIn(pt);
                }
                else
                {
                    bInElem = rcBounds.PointIn(pt);
                }
                if (bInElem)
                {
                    eles.Add(ele);
                    break;
                }
            }
        }
    }

    Rect rect = GetRenderBound(hitEle->GetUIElement());
    if (rect.PointIn(pt))
    {
        return hitEle;
    }
    else
    {
        return NULL;
    }
}

//==================================================
// ApplicationRootItem

ApplicationRootItem::ApplicationRootItem()
    : _app(NULL)
{
}

ApplicationRootItem::~ApplicationRootItem()
{
    Clear();
}

void ApplicationRootItem::Clear()
{
    if (NULL != _app)
    {
        _app->unref();
        _app = NULL;
    }

    _isLoaded = false;
    _isModified = false;
}

void ApplicationRootItem::Reset()
{
    Clear();
}

void ApplicationRootItem::ClearOnlyRes()
{
    if (NULL != _app)
    {
        _app->Clear();
    }

    _isLoaded = false;
    _isModified = false;
}

ApplicationNode* ApplicationRootItem::GetApplicationNode()
{
    if (NULL == _app)
    {
        _app = new ApplicationNode();
        _app->ref();
    }
    return _app;
}

ElementRootItem* ApplicationRootItem::GetMainRootItem() const
{
    if (_app != NULL)
    {
        return _app->GetMainRootItem();
    }
    return NULL;
}

void ApplicationRootItem::SetMainRootItem(ElementRootItem* pRootItem, bool bModified)
{
    Project* pPrj = GetProject();

    if (_app != NULL && NULL != pPrj)
    {
        _app->SetMainRootItem(pRootItem);
        if (bModified)
        {
            SetModified(true);
            pPrj->GetSlnTreeManager()->GetProjectTree()->SetSelectedItem(this);
        }
    }
}

suic::String ApplicationRootItem::GetResXml(const String& offset)
{
    suic::String strXml;
    if (_app != NULL)
    {
        strXml = _app->GetResXml(_U(""), offset);
    }
    return strXml;
}

bool ApplicationRootItem::InternalAppOpen(const String& strPath, const String& strFilename, bool onlyReadAppRes)
{
    XamlLoader xamlLoader;
    ApplicationNode* appNode = GetApplicationNode();

    xamlLoader.LoadAppRootXaml(this, strPath, onlyReadAppRes);

    if (appNode != NULL)
    {
        FileDir::DupCreateDir(strPath);
        return true;
    }
    else
    {
        return false;
    }
}

bool ApplicationRootItem::OnLoadApp(bool onlyReadAppRes)
{
    String strPath = GetFullPath();
    Project* pPrj = GetProject();
    String prjDir = pPrj->GetProjectDir();
    String strFilename = GetFileName();

    suic::ThemeOp::AddDir(pPrj->GetProjectName(), prjDir);

    return InternalAppOpen(strPath, strFilename, onlyReadAppRes);
}

bool ApplicationRootItem::OnLoad(bool onlyReadAppRes)
{
    return OnLoadApp(onlyReadAppRes);
}

void ApplicationRootItem::OnClose()
{
    Reset();
}

bool ApplicationRootItem::Save()
{
    if (Utils::CheckUseTimeout(_U("这是Beta版本,请重新启动!")))
    {
        return false;
    }

    if (GetApplicationNode() == NULL || !IsLoaded())
    {
        return false;
    }
    else
    {
        String strXml;

        strXml += _U("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
        strXml += GetResXml(_U(""));

        FileWriter fs;
        String strFile = GetFullPath();

        if (fs.Open(strFile))
        {
            fs.Write(strXml);
            fs.Close();
            SetModified(false);
        }

        return true;
    }
}

bool ApplicationRootItem::SaveAs(const String& strPath)
{
    return false;
}

//==================================================
// ResourceDicRootItem

ResourceDicRootItem::ResourceDicRootItem()
    : _resDic(NULL)
{
}

ResourceDicRootItem::~ResourceDicRootItem()
{
    Clear();
}

void ResourceDicRootItem::Clear()
{
    if (NULL != _resDic)
    {
        _resDic->unref();
        _resDic = NULL;
    }

    _isLoaded = false;
    _isModified = false;
}

void ResourceDicRootItem::Reset()
{
    Clear();
}

ResourceDictionaryNode* ResourceDicRootItem::GetResourceDicNode()
{
    if (NULL == _resDic)
    {
        _resDic = new ResourceDictionaryNode();
        _resDic->ref();
    }
    return _resDic;
}

suic::String ResourceDicRootItem::GetResXml(const String& offset)
{
    suic::String strXml;
    if (_resDic != NULL)
    {
        strXml = _resDic->GetResXml(offset);
    }
    return strXml;
}

bool ResourceDicRootItem::OnLoad(bool onlyReadRes)
{
    String strPath = GetFullPath();
    XamlLoader xamlLoader;

    xamlLoader.LoadResourceDicRootXaml(this, strPath);
    return GetResourceDicNode() != NULL;
}

void ResourceDicRootItem::OnClose()
{
    Reset();
}

bool ResourceDicRootItem::Save()
{
    if (Utils::CheckUseTimeout(_U("这是Beta版本,请重新启动!")))
    {
        return false;
    }

    if (GetResourceDicNode() == NULL || !IsLoaded())
    {
        return false;
    }
    else
    {
        String strXml;

        strXml += _U("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
        strXml += GetResXml(_U(""));

        FileWriter fs;
        String strFile = GetFullPath();

        if (fs.Open(strFile))
        {
            fs.Write(strXml);
            fs.Close();
            SetModified(false);
        }

        return true;
    }
}

bool ResourceDicRootItem::SaveAs(const String& strPath)
{
    return false;
}

//==================================================
// TemplateRootItem

TemplateRootItem::TemplateRootItem()
{
    _initTemplate = false;
    _isControlTemplate = true;

    _typeInfo = NULL;
    _targetType = NULL;

    _trgColl = new TriggerCollectionNode();
    _trgColl->ref();

    _ftRoot = NULL;
}

TemplateRootItem::~TemplateRootItem()
{
    FREEREFOBJ(_trgColl);
    if (NULL != _ftRoot)
    {
        _ftRoot->unref();
    }
}

void TemplateRootItem::CloneNode(ResNodePtr& obj)
{
    ResNodePtr elemNode;
    TemplateRootItem* tempRoot = new TemplateRootItem();
    DesignElement* elemRoot = GetTemplateRootElement();

    obj = tempRoot;

    _trgColl->CloneTriggerCollectionNode(tempRoot->_trgColl);

    tempRoot->_initTemplate = _initTemplate;
    tempRoot->_isControlTemplate = _isControlTemplate;

    tempRoot->_typeInfo = _typeInfo;
    tempRoot->_targetType = _targetType;

    if (elemRoot != NULL)
    {
        elemRoot->CloneNode(elemNode);
        tempRoot->SetRootElement(RTTICast<DesignElement>(elemNode.get()));
    }
}

void TemplateRootItem::ParseTemplateValue(Project* pPrj, FrameworkElement* fe, ObjectPtr& resVal)
{
    suic::String strXml = GetResXml(_U(""));
    IXamlDoc* xamlDoc = XamlTools::CreateXamlDoc();
    if (NULL != xamlDoc)
    {
        XamlWalker walker(pPrj);
        IXamlNode* pRoot = NULL;
        suic::Mulstr strData(strXml.c_str());

        xamlDoc->LoadMemory((suic::Byte*)strData.c_str(), strData.Length());
        pRoot = xamlDoc->GetRootNode();
        if (NULL != pRoot)
        {
            ResourceItem resItem;
            walker.ReadTemplateRes(GetTargetType(), NULL, pRoot, resItem);
            resVal = RTTICast<FrameworkTemplate>(resItem.res.get());
        }
        xamlDoc->unref();
    }
}

String TemplateRootItem::GetResXml(const String& offset)
{
    String strXml;
    String strLayout;
    String strTrigger;

    if (!HasContent())
    {
        return strXml;
    }

    DesignElement* rootElem = GetTemplateRootElement();

    strLayout = rootElem->GetResXml(offset + ResNode::OFFSET1);

    strTrigger = GetTriggerCollection()->GetResXml(offset + ResNode::OFFSET2);

    strXml += offset + _U("<");
    strXml += GetTemplateType()->typeName;

    if (!GetKey().Empty())
    {
        strXml += _U(" x:Key=\"");
        strXml += GetKey();
        strXml += _U("\"");
    }

    if (GetTargetType() != NULL)
    {
        strXml += _U(" TargetType=\"");
        strXml += GetTargetType()->typeName;
        strXml += _U("\"");
    }

    strXml += _U(">\n");

    strXml += strLayout;
    if (!strTrigger.Empty() && GetTemplateType() != NULL && 
        GetTemplateType()->InheritFrom(ControlTemplate::RTTIType()))
    {
        strXml += offset + ResNode::OFFSET1 + _U("<ControlTemplate.Triggers>\n");
        strXml += strTrigger;
        strXml += offset + ResNode::OFFSET1 + _U("</ControlTemplate.Triggers>\n");
    }

    strXml += offset + _U("</");
    strXml += GetTemplateType()->typeName;
    strXml += _U(">\n");

    return strXml;
}

bool TemplateRootItem::HasContent()
{
    if (NULL == GetTemplateRootElement() || GetTemplateType() == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

suic::Object* TemplateRootItem::GetValue()
{
    return NULL;
}

RTTIOfInfo* TemplateRootItem::GetValueRTTIType()
{
    return _typeInfo;
}

void TemplateRootItem::CheckTemplateInstantiate()
{
    DesignTemplateRoot* pRoot = GetDesignTemplateRoot();
    DesignElement* pTempRoot = pRoot->GetRootElement();
    if (NULL != pTempRoot && NULL == pTempRoot->GetUIElement())
    {
        if (GetTargetType() != NULL)
        {
            pTempRoot->InstantiateDesignElement(pRoot, pRoot->GetUIElement(), true);
        }
        else
        {
            pTempRoot->InstantiateDesignElement(pRoot, pRoot->GetUIElement(), true);
        }
    }
}

suic::RTTIOfInfo* TemplateRootItem::GetTargetType() const
{
    return _targetType;
}

suic::RTTIOfInfo* TemplateRootItem::GetTemplateType() const
{
    return _typeInfo;
}

void TemplateRootItem::SetTemplateType(suic::RTTIOfInfo* typeInfo)
{
    _typeInfo = typeInfo;
}

void TemplateRootItem::SetTargetType(suic::RTTIOfInfo* typeInfo)
{
    _targetType = typeInfo;
    _trgColl->SetRTTIOfInfo(_targetType);
}

void TemplateRootItem::SetIsControlTemplate(bool v)
{
    _isControlTemplate = v;
}

bool TemplateRootItem::IsControlTemplate() const
{
    return _isControlTemplate;
}

bool TemplateRootItem::IsInitTemplate() const
{
    return _initTemplate;
}

void TemplateRootItem::SealInitTemplate()
{
    _initTemplate = true;
}

DesignElement* TemplateRootItem::GetRootElement()
{
    if (_rootItem == NULL)
    {
        UpdateRootElement(new DesignTemplateRoot(NULL, GetTargetType()));
    }
    return _rootItem;
}

DesignTemplateRoot* TemplateRootItem::GetDesignTemplateRoot()
{
    return dynamic_cast<DesignTemplateRoot*>(GetRootElement());
}

void TemplateRootItem::SetRootElement(DesignElement* pElem)
{
    DesignTemplateRoot* dRoot = GetDesignTemplateRoot();
    if (NULL == dRoot)
    {
        UpdateRootElement(new DesignTemplateRoot(pElem, GetTargetType()));
    }
    else if (pElem != dRoot->GetRootElement())
    {
        pElem->SetIsTemplate(true);
        if (pElem->GetUIElement() != NULL)
        {
            dRoot->AddElement(pElem);
        }
        else
        {
            dRoot->Clear();
            dRoot->AddXamlElement(pElem);
        }
    }
}

DesignElement* TemplateRootItem::GetTemplateRootElement()
{
    DesignTemplateRoot* dRoot = GetDesignTemplateRoot();
    if (NULL != dRoot)
    {
        return dRoot->GetRootElement();
    }
    return NULL;
}

TriggerCollectionNode* TemplateRootItem::GetTriggerCollection()
{
    if (_trgColl->GetOwnerType() == NULL)
    {
        _trgColl->SetRTTIOfInfo(GetTargetType());
    }
    return _trgColl;
}

void TemplateRootItem::GetChildAsNamed(suic::ItemCollection* itemColl)
{
    ComboBoxItem* cmbItem = new ComboBoxItem();

    cmbItem->SetContent(new OString(_U("TemplateParent")));
    itemColl->AddItem(cmbItem);
    GetChildAsNamed(GetRootElement(), itemColl, true);
}

void TemplateRootItem::GetChildAsNamed(DesignElement* pNode, suic::ItemCollection* itemColl, bool bSkip)
{
    String strName;
    if (pNode->GetUIElement() != NULL)
    {
        strName = pNode->GetUIElement()->GetName();
    }

    if (!bSkip && !strName.Empty())
    {
        itemColl->AddItem(pNode);
    }

    for (int i = 0; i < pNode->GetCount(); ++i)
    {
        DesignElement* pChild = RTTICast<DesignElement>(pNode->GetItem(i));
        if (NULL != pChild)
        {
            GetChildAsNamed(pChild, itemColl, false);
        }
    }
}

//=========================================================
//

bool XamlLoader::LoadAppRootXaml(ApplicationRootItem* root, const String& strPath, bool onlyReadAppRes)
{
    bool bSucc = false;
    String strErr;
    Project* pPrj = root->GetProject();
    XamlWalker walker(pPrj);

    if (!onlyReadAppRes)
    {
        root->Reset();
    }
    else
    {
        root->ClearOnlyRes();
    }

    walker.SetOnlyReadAppRes(onlyReadAppRes);

    try
    {
        bSucc = walker.WalkUri(root->GetApplicationNode(), NULL, strPath);
    }
    catch (suic::Exception& e)
    {
        strErr = e.GetErrorMsg();
    }

    if (!strErr.Empty() || NULL == root->GetApplicationNode()->GetApplication())
    {
        //suic::InfoBox::Show(strErr, _U("错误"));
        return false;
    }
    else
    {
        return bSucc;
    }
}

bool XamlLoader::LoadElementRootXaml(ElementRootItem* root, const String& strPath)
{
    bool bSucc = false;
    String strErr;
    Project* pPrj = root->GetProject();
    XamlWalker walker(pPrj);

    root->Reset();

    try
    {
        bSucc = walker.WalkUri(NULL, NULL, strPath);
        root->UpdateRootElement(walker.GetDesignElement());
    }
    catch (suic::Exception& e)
    {
        strErr = e.GetErrorMsg();
    }

    if (!strErr.Empty() || NULL == root->GetUIElement())
    {
        //suic::InfoBox::Show(strErr, _U("错误"));
        return false;
    }
    else
    {
        return bSucc;
    }
}

bool XamlLoader::LoadResourceDicRootXaml(ResourceDicRootItem* root, const String& strPath)
{
    bool bSucc = false;
    String strErr;
    Project* pPrj = root->GetProject();
    XamlWalker walker(pPrj);

    root->Reset();

    try
    {
        bSucc = walker.WalkUri(root->GetResourceDicNode(), NULL, strPath);
    }
    catch (suic::Exception& e)
    {
        strErr = e.GetErrorMsg();
    }

    if (!strErr.Empty() || NULL == root->GetResourceDicNode()->GetResourceDictionary())
    {
        //suic::InfoBox::Show(strErr, _U("错误"));
        return false;
    }
    else
    {
        return bSucc;
    }
}

bool XamlLoader::LoadElementRootXamlFromMemory(ElementRootItem* root, const char* data, int size)
{
    bool bSucc = false;
    String strErr;
    Project* pPrj = root->GetProject();
    XamlWalker walker(pPrj);

    root->Reset();

    try
    {
        bSucc = walker.WalkMemory(NULL, NULL, data, size);
        root->UpdateRootElement(walker.GetDesignElement());
    }
    catch (suic::Exception& e)
    {
        strErr = e.GetErrorMsg();
    }

    if (!strErr.Empty())
    {
        //suic::InfoBox::Show(strErr, _U("错误"));
        return false;
    }
    else
    {
        return bSucc;
    }
}

bool XamlLoader::LoadResourceDicRootXamlFromMemory(ResourceDicRootItem* root, const char* data, int size)
{
    bool bSucc = false;
    String strErr;
    Project* pPrj = root->GetProject();
    XamlWalker walker(pPrj);

    root->Reset();

    try
    {
        bSucc = walker.WalkMemory(root->GetResourceDicNode(), NULL, data, size);
    }
    catch (suic::Exception& e)
    {
        strErr = e.GetErrorMsg();
    }

    if (!strErr.Empty())
    {
        //suic::InfoBox::Show(strErr, _U("错误"));
        return false;
    }
    else
    {
        return bSucc;
    }
}
