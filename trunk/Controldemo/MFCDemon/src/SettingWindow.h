// 华勤科技版权所有 2010-2011
// 
// 文件名：MainWindow.h
// 功  能：主界面窗口。
// 
// 作  者：Sharpui开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _SETTINGWINDOW_H_
#define _SETTINGWINDOW_H_

class SettingWindow : public suic::Window
{
public:

    BeginRoutedEvent(SettingWindow, suic::Window)
        MemberRouted(OnButtonClicked)
    EndRoutedEvent()

    SettingWindow();
    ~SettingWindow();

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnButtonClicked(suic::DpObject* sender, suic::RoutedEventArg* e);
 
protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    void RegisterButtonEvent();

private:

    Window* _openWnd;
};

#endif

