///
/// 文件名：XamlRootElement.h
///
/// 功  能：定义资源节点基类
///
/// 作  者：汪荣
///

#ifndef _XAMLROOTELEMENT_H
#define _XAMLROOTELEMENT_H

#include <Main/FilterNode.h>

#include <Core/RedoUndoManager.h>
#include <Core/XamlElementNode.h>
#include <Core/TriggerCollectionNode.h>

class Project;
class XamlNodeItem;
class SlnTreeManager;

class XamlContentControl : public DesignElement
{
public:

    XamlContentControl();
    virtual ~XamlContentControl();

protected:

    bool OnAddElement(suic::FrameworkElement* elem);
    bool OnInsertElement(int index, suic::FrameworkElement* elem);
    bool OnRemoveElement(suic::FrameworkElement* elem);
};

class XamlPanel : public DesignElement
{
public:

    XamlPanel();
    virtual ~XamlPanel();

protected:

    bool OnAddElement(suic::FrameworkElement* elem);
    bool OnInsertElement(int index, suic::FrameworkElement* elem);
    bool OnRemoveElement(suic::FrameworkElement* elem);
};

class BaseRedoUndo : public IRedoUndo
{
public:

    BaseRedoUndo(RootItem* rootElem);
    ~BaseRedoUndo();

    void SetFocusedElement(DesignElement* elem);

protected:

    void UpdateLayout();
    SlnTreeManager* GetSlnTreeManager() const;

    RootItemData* GetRootItemData();

protected:

    RootItem* _rootElem;
};

class AddElementRedoUndo : public BaseRedoUndo
{
public:

    AddElementRedoUndo(RootItem* rootElem, DesignElement* pParent, DesignElement* pChild);
    ~AddElementRedoUndo();

    virtual bool Redo();
    virtual void Undo();

protected:

    DesignElement* _parent;
    DesignElement* _child;
};

class AddContentRedoUndo : public BaseRedoUndo
{
public:

    AddContentRedoUndo(RootItem* rootElem, DesignContent* pParent, DesignElement* pChild);
    ~AddContentRedoUndo();

    virtual bool Redo();
    virtual void Undo();

protected:

    DesignContent* _parent;
    DesignElement* _child;
    Object* _oldChild;
};

class DeleteElementRedoUndo : public AddElementRedoUndo
{
public:

    DeleteElementRedoUndo(RootItem* rootElem, DesignElement* pParent, DesignElement* pChild);
    ~DeleteElementRedoUndo();

    virtual bool Redo();
    virtual void Undo();

protected:

    void HandleContentControl();
};

class ReplacePanelRedoUndo : public BaseRedoUndo
{
public:

    ReplacePanelRedoUndo(RootItem* rootElem, DesignElement* pElem, suic::Panel* pPanel);
    ~ReplacePanelRedoUndo();

    virtual bool Redo();
    virtual void Undo();

private:

    DesignElement* _elem;
    suic::Panel* _oldPanel;
    suic::Panel* _newPanel;
};

class SetElementContentRedoUndo : public BaseRedoUndo
{
public:

    SetElementContentRedoUndo(RootItem* rootElem, DesignElement* pElem, SetterNode* pSetter);
    ~SetElementContentRedoUndo();

    virtual bool Redo();
    virtual void Undo();

    void UpdatePropertyWindow();

private:
    
    DpProperty* _dp;

    ObjectPtr _oldVal;
    ObjectPtr _newVal;

    SetterNode* _setterNode;

    DesignElement* _elem;
    DesignElement* _content;
};

#endif
