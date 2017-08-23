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

class ComboBoxData : public suic::NotifyPropChanged
{
public:

    BeginMember(ComboBoxData, suic::NotifyPropChanged)
        MemberString(Name)
    EndMember()

    RTTIOfClass(ComboBoxData)

    ComboBoxData()
    {
        _name = "combobox";
    }

    suic::String GetName()
    {
        return _name;
    }

    void SetName(suic::String val)
    {
        _name = val;
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

    void OnSelItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e);
    void OnComboBoxTextChanged(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnSwitchBtnClick(suic::Element* sender, suic::RoutedEventArg* e);

protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    void RegisterButtonEvent();
};

#endif

