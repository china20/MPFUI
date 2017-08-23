// 华勤科技版权所有 2010-2011
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

class GroupData : public suic::ViewGroup
{
public:

    BeginMember(GroupData, suic::ViewGroup)
        MemberGetString(Name)
    EndMember()

    GroupData()
    {

    }

    GroupData(suic::String name)
    {
        _name = name;
    }

    bool IsBottomLevel()
    {
        return false;
    }

    suic::String GetName()
    {
        return _name;
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

    BeginRoutedEvent(MainWindow, suic::Window)
        MemberRouted(OnResetButtonClick)
    EndRoutedEvent()

    MainWindow();
    ~MainWindow();

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnSelectedDateChanged(suic::Element* sender, suic::RoutedEventArg* e);
    void OnResetButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnStopButtonClick(suic::Element* sender, suic::RoutedEventArg* e);
    void OnDeleteButtonClick(suic::Element* sender, suic::RoutedEventArg* e);

    void OnSelModeButtonClick(suic::Element* sender, suic::RoutedEventArg* e);
 
protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    void RegisterButtonEvent();
    GroupData* NewGroupData(suic::String name);
};

#endif

