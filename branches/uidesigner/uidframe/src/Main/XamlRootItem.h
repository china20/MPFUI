
#ifndef _XAMLROOTITEM_H_
#define _XAMLROOTITEM_H_

#include <Core/ResNode.h>
#include <Core/RedoUndoManager.h>
#include <Core/XamlElementNode.h>
#include <Core/TriggerCollectionNode.h>

#include <Main/FilterNode.h>

using namespace suic;

class Project;

struct RootItemData
{
    DesignElement* root;
    DesignElement* hot;
    RedoUndoManager ruMana;

    Array<DesignElement*> sels;

    RootItemData()
    {
        root = NULL;
        hot = NULL;
    }

    bool ContainsSelected(DesignElement* focused)
    {
        for (int i = 0; i < GetSelectedCount(); ++i)
        {
            if (GetSelectedElement(i) == focused)
            {
                return true;
            }
        }
        return false;
    }

    void ClearSelectedElements()
    {
        sels.Clear();
    }

    void AddSelected(DesignElement* focused)
    {
        sels.Add(focused);
    }

    void SetFocused(DesignElement* focused)
    {
        sels.Clear();
        sels.Add(focused);
    }

    int GetSelectedCount() const
    {
        return sels.GetCount();
    }

    DesignElement* GetSelectedElement(int index)
    {
        return sels.GetItem(index);
    }
};

class ObjTreeManager;

/// <Summary>
///  根对象基类（比如布局文件、图像、资源文件等保存在本地的文件）
/// </Summary>
class RootItem : public FilterNode
{
public:

    RTTIOfAbstractClass(RootItem)

    BeginMember(RootItem, FilterNode)
        MemberGetInt(ShowItem)
    EndMember()

    bool IsLeafItem()
    {
        return true;
    }

    int GetShowItem()
    {
        return Visibility::Collapsed;
    }

    RootItem();
    RootItem(const String& name);
    virtual ~RootItem();

    suic::String GetResXml(const String& offset);

    String ToString();

    bool IsModified() const;
    void SetModified(bool val);

    Project* GetProject() const;
    void SetProject(Project* pPrj);

    String GetFileName() const;
    void SetFileName(const String& name);

    ResourceUri GetResourceUri() const;

    void SetObjTreeManager(ObjTreeManager* objTree);
    ObjTreeManager* GetObjTreeManager() const;

    bool IsLoaded() const;

    void Close();
    bool Load(bool onlyReadAppRes);
    bool CheckLoadState();

    void SetLoaded();
    void ReloadOnMemory();

    virtual void Reset() = 0;
    virtual bool Save() = 0;
    virtual bool SaveAs(const String& strPath) = 0;

protected:

    virtual bool OnLoad(bool onlyReadAppRes) = 0;
    virtual void OnClose() = 0;
    virtual bool OnLoadFromXml();

    virtual void OnModifiedChanged();

protected:

    bool _isModified;
    bool _isLoaded;

    Project* _project;
    ObjTreeManager* _objTree;

    friend class XamlLoader;
};

class FileRootItem : public RootItem
{
public:

    RTTIOfAbstractClass(FileRootItem)
};

/// <Summary>
///  管理布局文件、资源文件（Application、ResourceDictionary、Window等文件）
/// </Summary>
class ElementRootItem : public FileRootItem
{
public:

    RTTIOfClass(ElementRootItem)

    ElementRootItem();
    ElementRootItem(DesignElement* root);
    virtual ~ElementRootItem();

    static DesignElement* HitTestElement(DesignElement* elem, Point pt);
    static SetterNode* SetElementValue(DesignElement* elem, DpProperty* dp, Object* val);

    void UpdateRootElement(DesignElement* data);

    void SetStartElement();
    void RemoveStartElement();

    // 根文件是否是一个模版文件
    bool IsTemplate() const;

    suic::String GetResXml(const String& offset);

    RootItemData* GetRootItemData() const;

    void Refresh();
    void SetRefreshFlag(bool val);

    void Reset();
    void ResetRootItem(bool bSetLoaded);
    virtual DesignElement* GetRootElement();

    virtual bool Save();
    virtual bool SaveAs(const String& strPath);

    //============================= 布局操作函数
    void UpdateDesignWindow();
    void AddChileElement(DesignElement* pParent, DesignElement* pChild, bool bCallRedo=true);
    void ReplacePanel(DesignElement* pElem, suic::Panel* pPanel);
    void RemoveChileElement(DesignElement* pParent, DesignElement* pChild);
    bool SetElementSetter(DesignElement* pElem, SetterNode* pSetter, bool fromProp);

    //============================= 设计面板辅助操作函数
    DesignElement* HitTestElement(Point pt);

    FrameworkElement* GetUIElement();

    DesignElement* GetHotElement() const;
    DesignElement* GetFocusElement() const;

    void SetHotElement(DesignElement* elem);
    void SetFocusElement(DesignElement* elem);

    virtual bool IsTemplateRoot();

protected:

    virtual bool OnLoad(bool onlyReadAppRes);
    virtual void OnClose();
    virtual bool OnLoadFromXml();

protected:

    bool OnLoadApp(bool onlyReadAppRes);
    bool InternalAppOpen(const String& strPath, const String& strFilename, bool onlyReadAppRes);

protected:

    bool _isTemplate;
    bool _refreshDirty;
    DesignElement* _rootItem;
    RootItemData* _rootData;
};

class ApplicationRootItem : public FileRootItem
{
public:

    RTTIOfClass(ApplicationRootItem)

    ApplicationRootItem();
    virtual ~ApplicationRootItem();

    suic::String GetResXml(const String& offset);

    void Reset();
    void ClearOnlyRes();

    virtual bool Save();
    virtual bool SaveAs(const String& strPath);

    ApplicationNode* GetApplicationNode();
    ElementRootItem* GetMainRootItem() const;
    void SetMainRootItem(ElementRootItem* pRootItem, bool bModified);

protected:

    virtual bool OnLoad(bool onlyReadAppRes);
    virtual void OnClose();

private:

    void Clear();
    bool OnLoadApp(bool onlyReadAppRes);
    bool InternalAppOpen(const String& strPath, const String& strFilename, bool onlyReadAppRes);

private:

    ApplicationNode* _app;
};

class ResourceDicRootItem : public FileRootItem
{
public:

    RTTIOfClass(ResourceDicRootItem)

    ResourceDicRootItem();
    virtual ~ResourceDicRootItem();

    suic::String GetResXml(const String& offset);

    void Reset();
    virtual bool Save();
    virtual bool SaveAs(const String& strPath);

    ResourceDictionaryNode* GetResourceDicNode();

protected:

    virtual bool OnLoad(bool onlyReadAppRes);
    virtual void OnClose();

private:

    void Clear();

private:

    ResourceDictionaryNode* _resDic;
};

class DesignTemplateRoot : public DesignElement
{
public:

    DesignTemplateRoot(DesignElement* pChild, RTTIOfInfo* targetInfo)
    {
        FrameworkElement* pCont = NULL;
        
        _targetInfo = targetInfo;

        if (NULL != targetInfo)
        {
            pCont = RTTICast<FrameworkElement>(targetInfo->Create());
        }
        else
        {
            pCont = RTTICast<FrameworkElement>(ContentControl::RTTIType()->Create());
        }

        if (NULL != pCont)
        {
            DesignHelper::SetTemplateDesignMode(pCont);
        }

        SetNodeElement(pCont);
        SetIsTemplate(true);

        if (pChild != NULL)
        {
            pChild->SetIsTemplate(true);
            AddXamlElement(pChild);

            if (NULL != pChild->GetUIElement())
            {
                if (NULL != targetInfo)
                {
                    DesignHelper::SetElementParent(pCont, pChild->GetUIElement());
                }
                pCont->SetTemplateChild(pChild->GetUIElement());
            }
        }
    }

    DesignElement* GetRootElement()
    {
        if (GetCount() > 0)
        {
            return GetXamlElement(0);
        }
        return NULL;
    }
    
    void ReplaceRootElement(suic::FrameworkElement* pRoot)
    {
        if (NULL != GetRootElement())
        {
            GetRootElement()->SetNodeElement(pRoot);
            SetRootChild(GetUIElement(), pRoot);
        }
    }

    virtual bool AddElement(DesignElement* elem)
    {
        Clear();
        AddXamlElement(elem);
        elem->SetIsTemplate(true);

        FrameworkElement* pCont = GetUIElement();
        FrameworkElement* feElem = elem->GetUIElement();

        SetRootChild(pCont, feElem);
        
        return true;
    }

    virtual bool RemoveElement(DesignElement* elem)
    {
        FrameworkElement* pCont = GetUIElement();
        if (GetCount() == 1 && GetXamlElement(0) == elem)
        {
            Clear();
            pCont->SetTemplateChild(NULL);
        }
        return true;
    }

private:
    
    void SetRootChild(FrameworkElement* pCont, FrameworkElement* feElem)
    {
        if (NULL != feElem)
        {
            if (NULL != _targetInfo)
            {
                DesignHelper::SetElementParent(pCont, feElem);
            }

            pCont->SetTemplateChild(feElem);
        }
        else
        {
            pCont->SetTemplateChild(NULL);
        }
    }

private:

    RTTIOfInfo* _targetInfo;
};

//=========================================
// 
// 管理模版元素
class TemplateRootItem : public ElementRootItem
{
public:

    RTTIOfClass(TemplateRootItem)

    TemplateRootItem();
    ~TemplateRootItem();

    void CloneNode(ResNodePtr& obj);
    void ParseTemplateValue(Project* pPrj, FrameworkElement* fe, ObjectPtr& resVal);

    virtual String GetResXml(const String& offset);

    String ToString();
    suic::Object* GetValue();
    RTTIOfInfo* GetValueRTTIType();

    bool HasContent();

    void CheckTemplateInstantiate();

    suic::RTTIOfInfo* GetTargetType() const;
    suic::RTTIOfInfo* GetTemplateType() const;

    void SetTemplateType(suic::RTTIOfInfo* typeInfo);
    void SetTargetType(suic::RTTIOfInfo* typeInfo);

    void SetIsControlTemplate(bool v);
    bool IsControlTemplate() const;
    bool IsInitTemplate() const;
    void SealInitTemplate();

    DesignElement* GetRootElement();
    void SetRootElement(DesignElement* pElem);

    DesignElement* GetTemplateRootElement();
    TriggerCollectionNode* GetTriggerCollection();

    // 取得已经命名的子元素
    void GetChildAsNamed(suic::ItemCollection* itemColl);

    bool IsTemplateRoot()
    {
        return true;
    }

protected:

    DesignTemplateRoot* GetDesignTemplateRoot();
    void GetChildAsNamed(DesignElement* pNode, suic::ItemCollection* itemColl, bool bSkip);

private:

    bool _initTemplate;
    bool _isControlTemplate;

    RTTIOfInfo* _typeInfo;
    RTTIOfInfo* _targetType;

    TriggerCollectionNode* _trgColl;

    FrameworkTemplate* _ftRoot;
};

inline String TemplateRootItem::ToString()
{
    return ResNode::ToString();
}

class ImageRootItem : public RootItem
{
public:

    RTTIOfClass(ImageRootItem)

    BeginMember(ImageRootItem, RootItem)
        MemberGetInt(ShowItem)
    EndMember()

    ImageRootItem();

    int GetShowItem()
    {
        return Visibility::Visible;
    }

    virtual void Reset()
    {

    }

    virtual String GetRootType() const;
    virtual bool Save();
    virtual bool SaveAs(const String& strPath);

protected:

    virtual bool OnLoad(bool onlyReadAppRes);
    virtual void OnClose();
    virtual void OnClear();
};

// 读取文件

class XamlLoader
{
public:

    bool LoadAppRootXaml(ApplicationRootItem* root, const String& strPath, bool onlyReadAppRes);
    bool LoadElementRootXaml(ElementRootItem* root, const String& strPath);
    bool LoadResourceDicRootXaml(ResourceDicRootItem* root, const String& strPath);

    bool LoadElementRootXamlFromMemory(ElementRootItem* root, const char* data, int size);
    bool LoadResourceDicRootXamlFromMemory(ResourceDicRootItem* root, const char* data, int size);
};

#endif
