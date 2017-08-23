// 华勤科技版权所有 2010-2011
// 
// 文件名：MainWindow.h
// 功  能：主界面窗口。
// 
// 作  者：Sharpui开发组
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
class BindData : public suic::NotifyPropChanged
{
public:

    BeginMember(BindData, suic::NotifyPropChanged)
        MemberString(Name)
    EndMember()

    BindData()
    {
        _name = "Bind TextBlock";
    }

    suic::String GetName()
    {
        return _name;
    }

    void SetName(suic::String val)
    {
        _name = val;
        NotifyChanged("Name");
    }

    suic::String ToString()
    {
        return _name;
    }

private:

    suic::String _name;
};

class MainWindow : public suic::Window
{
public:

    MainWindow();
    ~MainWindow();

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnButtonClicked(suic::Element* sender, suic::RoutedEventArg* e);
    void OnEditStylePropClick(suic::Element* sender, suic::RoutedEventArg* e);

    void OnHyperlinkClick(suic::Element* sender, suic::RoutedEventArg* e);
 
protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    void RegisterButtonEvent();
};

#endif

