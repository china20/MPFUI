// 华勤科技版权所有 2010-2011
// 
// 文件名：EditRootPanel.h
// 功  能：编辑根界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _EDITROOTPANEL_H_
#define _EDITROOTPANEL_H_

#include <Core/TypeManager.h>
#include <main/CommonInterface.h>

class DesignPanel;
class EditRootPanel;
class SetterEditor;

struct PropInfo
{
    DpItem* dpItem;
    suic::String name;
    RTTIOfInfo* target;
    SetterEditor* valEditor;
};

class NameEditor : public suic::TextBox
{
public:

    RTTIOfClass(NameEditor)

    NameEditor()
    {
        _elem = NULL;
    }

    void SetTargetElement(DesignElement* delem, EditRootPanel* editPanel);

    void OnLostFocus(suic::RoutedEventArg* e)
    {
        suic::TextBox::OnLostFocus(e);
        //UpdateName();
    }

    void OnKeyDown(suic::KeyboardEventArg* e)
    {
        suic::TextBox::OnKeyDown(e);
        if (e->GetKey() == Key::kReturn)
        {
            //UpdateName();
        }
    }

    void OnTextInput(suic::KeyboardEventArg* e)
    {
        suic::TextBox::OnTextInput(e);
        //UpdateName();
    }

    void OnTextChanged(TextChangedEventArg* e)
    {
        suic::TextBox::OnTextChanged(e);
        UpdateName();
        e->SetHandled(true);
    }

    void UpdateName();

private:

    DesignElement* _elem;
    EditRootPanel* _editPanel;
};

class Project;
class ElementRootItem;

class EditRootPanel : public Grid
{
public:

    RTTIOfClass(EditRootPanel)

    EditRootPanel();
    ~EditRootPanel();

    void UpdateSetter(SetterNode* pSetter, bool needNotifyChanged);
    void UpdateResource(ResNode* setter);
    void UpdateSetterToUI(SetterNode* pSetter);

    void SwitchCurrentElement(DesignElement* resElem, DesignElement* delem);
    void EnableDesignWithElementLock(DesignElement* delem);

    void SetDesignPanel(DesignPanel* design);
    void UpdateDesignPanel(DesignElement* target);

    void SetRootItem(RootItem* rootItem);
    void SetDesignRootItem(RootItem* rootItem);

    void InitEditPanelInfo(RootItem* rootElem, DesignPanel* design);

    //void SetTemplateParent(RTTIOfInfo* typeInfo);
    //RTTIOfInfo* GetTemplateParentType();

    SetterNode* CreateSetterNode(const suic::String& prop);
    suic::Object* FindElemRes(const suic::String& resKey);

    void SetModified(bool bModified);
    void InitPropsInfo(Element* parent);

    // 设置当前资源节点
    void SetCurrentResNode(ResNode* resNode)
    {
        _currentResNode = resNode;
    }

    ResourceDictionaryNode* GetResourceDicNode() const
    {
        ResourceDicRootItem* resRootItem = RTTICast<ResourceDicRootItem>(GetRootItem());
        if (NULL == resRootItem)
        {
            return NULL;
        }
        return resRootItem->GetResourceDicNode();
    }

    ResNode* GetCurrentResNode() const
    {
        return _currentResNode;
    }

    Project* GetProject() const
    {
        return _project;
    }

    RootItem* GetRootItem() const
    {
        return _designRoot;
    }

    TemplateRootItem* GetTemplateRoot() const
    {
        return RTTICast<TemplateRootItem>(GetRootItem());
    }

    String GetResName() const
    {
        return _resName;
    }

    String GetResDir() const
    {
        return _resDir;
    }

private:

    void OnInitialized(EventArg* e);
    void OnEditGridDefinitions(suic::Element* sender, suic::RoutedEventArg* e);
    void OnTimer(suic::Object* sender, suic::EventArg* e);

private:

    //RTTIOfInfo* _tempParent;
    // 当前编辑的元素
    DesignElement* _editElem;

    // 当前编辑的根元素
    RootItem* _designRoot;
    // 文件元素对象
    RootItem* _rootItem;

    DesignPanel* _designPanel;
    ResNode* _currentResNode;

    suic::AssignerTimer* _timer;

    bool _isSettingElement;

    Project* _project;
    suic::Array<PropInfo*> _props;

    String _resName;
    String _resDir;
};

#endif
