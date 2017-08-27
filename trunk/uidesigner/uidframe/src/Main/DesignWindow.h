// 华勤科技版权所有 2010-2011
// 
// 文件名：DesignWindow.h
// 功  能：主界面窗口。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _DESIGNWINDOW_H_
#define _DESIGNWINDOW_H_

class DesignWindow : public suic::Window
{
public:

    DesignWindow();
    ~DesignWindow();

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

protected:

    // 创建自己的控件对象
    suic::ObjectPtr OnBuild(suic::IXamlNode* pNode);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);
};

class DesignWindowOp
{
public:

    suic::FrameworkElement* GetDesignWindow();
};

#endif
