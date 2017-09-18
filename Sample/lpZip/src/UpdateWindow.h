// 华勤科技版权所有 2010-2022
// 
// 文件名：UpdateWindow.h
// 功  能：软件更新界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2017-09-16
// 
// ============================================================================

#ifndef _UPDATEWINDOW_H_
#define _UPDATEWINDOW_H_

class UpdateAppData : public  suic::NotifyPropChanged
{
public:

    BeginMember(UpdateAppData, suic::NotifyPropChanged)
        MemberString(Version)
    EndMember()

    RTTIOfClass(UpdateAppData)

    DefineString(Version);
};

class UpdateAppWindow : public suic::Window
{
public:

    // 事件映射
    BeginRoutedEvent(UpdateAppWindow, suic::Window)
        MemberRouted(OnClickUpdate)
    EndRoutedEvent()

    UpdateAppWindow();
    ~UpdateAppWindow();

private:

    void UpdateWindowTitle();

private:

    void OnClickUpdate(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    void OnLoaded(suic::LoadedEventArg* e);

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    UpdateAppData* _updateData;
};

#endif
