// 华勤科技版权所有 2010-2011
// 
// 文件名：PropertyWindow.h
// 功  能：主界面窗口。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _PROPERTYWINDOW_H_
#define _PROPERTYWINDOW_H_

#include <Core/TypeManager.h>
#include "CommonInterface.h"

class DesignPanel;
class SetterEditor;
class EditRootPanel;

class PropertyWindow : public suic::Window
{
public:

    PropertyWindow();
    ~PropertyWindow();

    void OnLoaded(suic::LoadedEventArg* e);

protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);
};

class PropertyWindowOp : public IPropertyWindow
{
public:

    PropertyWindowOp();
    ~PropertyWindowOp();

    void InitEditRootPanel(Element* rootPanel, Element* parent);
    suic::FrameworkElement* GetPropertyWindow(suic::Element* parent);

    void UpdateModified();

    void SetDesignPanel(DesignPanel* design);
    void InitEditPanelInfo(ElementRootItem* rootElem, DesignPanel* design);
    void SetCurrentElement(DesignElement* resElem, DesignElement* delem);

    void UpdateSetter(SetterNode* setter, bool needNotifyChanged);
    void UpdateResource(ResNode* setter);

    void SetSetterNotify(SetterNotifyEventHandler h);

private:

    DesignPanel* _designPanel;
    EditRootPanel* _editPanel;
    PropertyWindow* _propWindow;
};

#endif
