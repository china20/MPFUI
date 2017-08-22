///
/// 文件名：XamlElementNode.h
///
/// 功  能：定义资源节点基类
///
/// 作  者：汪荣
///

#ifndef _XAMLELEMENTNODE_H
#define _XAMLELEMENTNODE_H

#include <Core/HoldItem.h>
#include <Core/SetterCollectionNode.h>
#include <Core/TriggerCollectionNode.h>
#include <Core/ResourceDictionaryNode.h>

class DesignElement;
typedef shared<DesignElement> DesignElementPtr;

/// <Summary>
///  处理元素的资源节点，比如Button等
/// </Summary>
class DesignElement : public ResNode
{
public:

    BeginMember(DesignElement, ResNode)
        MemberGetObject(ElementIcon)
        MemberGetString(DisplayName)
        MemberBool(ShowElement)
        MemberBool(LockElement)
    EndMember()

    RTTIOfClass(DesignElement)

    DesignElement();
    DesignElement(bool isTemplate);
    virtual ~DesignElement();

    // 已经调用ref
    virtual void CloneNode(ResNodePtr& obj);

    virtual suic::String GetName();
    virtual suic::String GetTypeName();

    suic::String GetNodeName();
    void SetNodeName(const suic::String& val);

    void SetTypeInfo(suic::RTTIOfInfo* typeInfo);
    suic::RTTIOfInfo* GetTypeInfo() const;

    Object* GetElementIcon();

    bool GetShowElement();
    void SetShowElement(bool val);

    bool GetLockElement();
    void SetLockElement(bool val);

    String GetDisplayName();
    void SetDisplayName(String name);

    bool IsLeafItem()
    {
        if (GetCount() > 0)
        {
            return false;
        }
        return true;
    }

    bool IsModified() const;
    void SetModified(bool val);

    bool IsRoot() const;
    void SetIsRoot(bool val);

    bool IsTemplate() const;
    void SetIsTemplate(bool val);

    bool IsCanReplacePanel();

    void NotifyShowChanged();
    void NotifyLockedChanged();

    String ToString();

    virtual bool IsTemplateTaget() const;

    virtual bool IsSingleValue();
    virtual void SetValue(suic::Object* val);
    virtual suic::Object* GetValue();
    virtual suic::String GetSingleXml();
    virtual suic::String GetResXml(const String& offset);
    virtual String GetChildXml(const String& offset);

    virtual String GetElementName();
    virtual bool NeedNotifyChanged() const;

    void GetElementSetterXml(const String& offset, int iCurrSerial, String& strProp, String& strChildProp);

    DesignElement* GetXamlElement(int index);

    void SetNodeElement(suic::DpObject* elem);
    suic::DpObject* GetNodeElement() const;

    suic::FrameworkElement* GetUIElement() const;

    DesignElement* GetParentXamlElement() const;
    suic::FrameworkElement* GetParentUIElement() const;

    void AddSetter(const String& name, DpProperty* dp, Object* val);
    void AddResource(const String& key, Object* res);

    SetterCollectionNode* GetSetterCollection();
    ResourceDictionaryNode* GetResourceDictionary();

    void SetResourceDictionary(ResourceDictionaryNode* val);

    bool CreateElement(DesignElementPtr& elem, const String& strPath, const String& clsName);
    void SetElementAsTemplate(DesignElement* elem);

    bool InsertElement(int index, DesignElement* elem);
    bool MoveElement(DesignElement* elem, DesignElement* destination);

    void SwapElement(DesignElement* src, DesignElement* dest);

    int IndexOf(DesignElement* pElem);
    int IndexOf(suic::Point point, suic::Rect& rcElem);

    void Clear();
    void ClearChildren();

    // 刷新元素表达式资源
    void RefreshDesignElement();
    // 创建控件元素
    void InstantiateDesignElement(DesignElement* parent, FrameworkElement* templatedParent, bool fromTempParent);
    void InitElementProps(bool bOnlyExt);

    suic::Object* FindElemRes(const suic::String& resKey);

    virtual void OnInstantiateDesignElement(suic::FrameworkElement* fe);
    virtual void UpdateElementProperty(DpProperty* dp, Object* newVal);
    virtual void NotifyChildElementSet(DesignElement* child);

    void AddXamlChild(DesignElement* elem);

    virtual void OnFinishReadChildElement(DesignElement* child);
    virtual void AddXamlElement(DesignElement* elem);
    virtual bool AddElement(DesignElement* elem);
    virtual bool RemoveElement(DesignElement* elem);

    // 使用其它面板替换
    virtual bool ReplacePanel(suic::Panel* pPanel);
    virtual void CopyTo(DesignElement* Other);

    bool AddUIElement(FrameworkElement* elem);

protected:

    virtual bool OnAddElement(suic::FrameworkElement* elem);
    virtual bool OnInsertElement(int index, suic::FrameworkElement* elem);
    virtual bool OnMoveElement(suic::FrameworkElement* elem, suic::FrameworkElement* destination);
    virtual bool OnRemoveElement(suic::FrameworkElement* elem);

    void RemoveXamlChild(DesignElement* elem);

private:

    bool _isModified;
    bool _isRoot;
    bool _isTemplate;

    suic::String _nodeName;
    
    suic::DpObject* _element;
    suic::RTTIOfInfo* _typeInfo;

    // 元素本身的属性（local值）
    SetterCollectionNode* _setterColl;
    // 元素资源
    ResourceDictionaryNode* _resDic;
};

class DesignContent : public DesignElement
{
public:

    RTTIOfClass(DesignContent)

    DesignContent();
    ~DesignContent();

    Object* GetContent();
    virtual void UpdateElementProperty(DpProperty* dp, Object* newVal);

    virtual void AddXamlElement(DesignElement* elem);
    virtual bool RemoveElement(DesignElement* elem);
};

/// <Summary>
///  处理Grid控件节点，主要是处理其行列信息
/// </Summary>
class DesignGrid : public DesignElement
{
public:

    RTTIOfClass(DesignGrid)

    DesignGrid();
    ~DesignGrid();

    virtual void CloneNode(ResNodePtr& obj);
    void AddXamlElement(DesignElement* elem);

    void SetRowMinHeight(int index, Float val);
    void SetRowMaxHeight(int index, Float val);
    void SetRowHeight(int index, suic::Byte type, suic::Float val);

    void SetColumnMinWidth(int index, Float val);
    void SetColumnMaxWidth(int index, Float val);
    void SetColumnWidth(int index, suic::Byte type, suic::Float val);

    SetterCollectionNode* AddRow();
    SetterCollectionNode* AddColumn();

    void RemoveRowAt(int index);
    void RemoveColumnAt(int index);

    int GetRowCount();
    SetterCollectionNode* GetRow(int index);

    int GetColumnCount();
    SetterCollectionNode* GetColumn(int index);

protected:

    suic::Grid* GetOwner();

    void InitRowDefs();
    void InitColumnDefs();

    SetterCollectionNode* GetRow(int index, bool bAdd);
    SetterCollectionNode* GetColumn(int index, bool bAdd);

    String GetGridSizeXml(SetterCollectionNode* setterColl);
    String GetChildXml(const String& offset);

    void OnInstantiateDesignElement(suic::FrameworkElement* fe);
    void OnFinishReadChildElement(DesignElement* child);

    AutoColl<SetterCollectionNode> _rowDefs;
    AutoColl<SetterCollectionNode> _colDefs;

    DesignElement* _rowTmpDefs;
    DesignElement* _colTmpDefs;
};

/// <Summary>
///  处理Track控件节点
/// </Summary>
class DesignTrack : public DesignElement
{
public:

    RTTIOfClass(DesignTrack)

    DesignTrack();
    ~DesignTrack();

    virtual void CloneNode(ResNodePtr& obj);
    void AddXamlElement(DesignElement* elem);

    void SetThumb(DesignElement* elem);
    void SetDecreaseButton(DesignElement* elem);
    void SetIncreaseButton(DesignElement* elem);

protected:

    void OnFinishReadChildElement(DesignElement* child);
    DesignElement* AddTrackChild(DesignElement* pNode);
    String GetChildXml(const String& offset);

    DesignElement* _drb;
    DesignElement* _irb;
    DesignElement* _thumb;
};

/// <Summary>
///  处理用户自定义节点（这种情况主要是用户没有把自定义控件注册到设计器中）
/// </Summary>
class UserDesignElement : public DesignElement
{
public:

    BeginMember(UserDesignElement, DesignElement)
        MemberBool(LockElement)
    EndMember()

    RTTIOfClass(UserDesignElement)

    UserDesignElement();
    UserDesignElement(suic::IXamlNode* pNode);
    ~UserDesignElement();

    bool GetLockElement();
    void SetLockElement(bool val);

    bool IsLocked() const;
    suic::String GetTypeName();

    String ToString();

protected:

    void Clear();
    String GetResXml(const String& offset);

private:

    HoldItem _holdItem;
};

#endif
