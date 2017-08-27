// 华勤科技版权所有 2010-2022
// 
// 文件名：MainWindow.h
// 功  能：主界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

//
// <Summary>
//  演示按钮的Command和数据绑定功能；
//  Command功能允许用户定义的数据接收并处理按钮的点击命令
//  数据绑定功能演示了按钮的内容根据用户数据变化而变化。
// </Summary>
//
class ButtonData : public suic::ICommand
                 , public suic::NotifyPropChanged
{
public:

    BeginMember(ButtonData, suic::NotifyPropChanged)
        MemberString(ButtonName)
    EndMember()

    RTTIOfClass(ButtonData)

    ButtonData()
    {
        _btnName = "点击绑定业务数据";
    }

    bool CanExecute(suic::Object* target, suic::Object* param)
    {
        return true;
    }

    void Execute(suic::Object* target, suic::Object* param)
    {
        SetButtonName("绑定到业务数据");
    }

    suic::String GetButtonName()
    {
        return _btnName;
    }

    void SetButtonName(suic::String val)
    {
        _btnName = val;
        NotifyChanged("ButtonName");
    }

    suic::String ToString()
    {
        return _btnName;
    }

private:

    suic::String _btnName;
};

class MainWindow : public suic::Window
{
public:

    MainWindow();
    ~MainWindow();

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnButtonClicked(suic::Element* sender, suic::RoutedEventArg* e);
 
protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    void RegisterButtonEvent();
};

#endif

