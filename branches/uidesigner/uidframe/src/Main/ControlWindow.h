// 华勤科技版权所有 2010-2011
// 
// 文件名：ControlWindow.h
// 功  能：主界面窗口。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _CONTROLWINDOW_H_
#define _CONTROLWINDOW_H_

#include "CommonInterface.h"
#include <Editor/ControlAssetsWindow.h>

class ControlWindow : public suic::Window
{
public:

    ControlWindow();
    ~ControlWindow();

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);
};

class ControlWindowOp : public IControlWindow
{
public:

    ControlWindowOp();
    virtual ~ControlWindowOp();

    suic::FrameworkElement* GetControlWindow(suic::Element* parent);
    // 根据控件类型名称生成控件默认布局
    suic::FrameworkElementPtr GetControlSample(suic::Object* parent, String strType);

    void RestoreSelectMode();
    void SetSelectNotify(SelectNotifyEventHandler selEvent);

    suic::RadioButton* GetMoreElement();
    void UpdateMoreElement(const String& name, bool setChecked);

    void OnCheckedChanged(suic::DpObject* sender, suic::RoutedEventArg* e);

    void OnMoreExpanderClick(suic::Element* sender, suic::RoutedEventArg* e);

private:

    ControlWindow* _ctrlWindow;
    ControlAssetsWindow * _ctrlAssetsWnd;
    suic::RadioButton* _moreElement;
    SelectNotifyEventHandler _selectEvent;
};

#endif
