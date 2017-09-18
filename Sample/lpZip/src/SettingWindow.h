// 华勤科技版权所有 2010-2022
// 
// 文件名：SettingWindow.h
// 功  能：软件设置界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2017-09-16
// 
// ============================================================================

#ifndef _SETTINGWINDOW_H_
#define _SETTINGWINDOW_H_

class SettingData : public  suic::NotifyPropChanged
{
public:

    BeginMember(SettingData, suic::NotifyPropChanged)
        MemberString(Version)
    EndMember()

    RTTIOfClass(SettingData)

    DefineString(Version);
};

class ZipSetWindow : public suic::Window
{
public:

    // 事件映射
    BeginRoutedEvent(ZipSetWindow, suic::Window)
        MemberRouted(OnClickApply)
        MemberRouted(OnClickCancel)
        MemberRouted(OnClickOk)
    EndRoutedEvent()

    ZipSetWindow();
    ~ZipSetWindow();

private:

    void OnClickApply(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickCancel(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickOk(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    void OnLoaded(suic::LoadedEventArg* e);

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    SettingData* _settingData;
};

#endif
