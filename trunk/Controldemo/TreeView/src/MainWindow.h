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

class TreeGroup : public suic::HierarchicalItem
{
public:

    BeginMember(TreeGroup, suic::HierarchicalItem)
        MemberString(Name)
        MemberObject(IsChecked)
    EndMember()

    TreeGroup()
    {
        _isChecked = suic::Boolean::False;
    }

    suic::String GetName()
    {
        return _name;
    }

    void SetName(suic::String name)
    {
        _name = name;
    }

    suic::Object* GetIsChecked()
    {
        return _isChecked;
    }

    void SetIsChecked(suic::Object* val)
    {
        _isChecked = suic::DynamicCast<suic::Boolean>(val);
        NotifyChanged(L"IsChecked");
    }

    suic::String ToString()
    {
        return _name;
    }

    void AddItem(suic::Object* item)
    {
        GetChildren()->AddItem(item);
    }

private:

    suic::String _name;
    suic::Boolean* _isChecked;
};

class TreeGuoJia : public suic::NotifyPropChanged
{
public:

    BeginMember(TreeGuoJia, suic::NotifyPropChanged)
        MemberString(Name)
        MemberObject(IsChecked)
    EndMember()

    TreeGuoJia()
    {
        _isChecked = suic::Boolean::False;
    }

    TreeGuoJia(suic::String name)
    {
        _isChecked = suic::Boolean::False;
        _name = name;
    }

    suic::String GetName()
    {
        return _name;
    }

    void SetName(suic::String name)
    {
        _name = name;
    }

    suic::Object* GetIsChecked()
    {
        return _isChecked;
    }

    void SetIsChecked(suic::Object* val)
    {
        _isChecked = suic::DynamicCast<suic::Boolean>(val);
    }

    suic::String ToString()
    {
        return _name;
    }

private:

    suic::String _name;
    suic::Boolean* _isChecked;
};

class MainWindow : public suic::Window
{
public:

    MainWindow();
    ~MainWindow();

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnButtonClicked(suic::Element* sender, suic::RoutedEventArg* e);
    void OnTreeItemSelectedChanged(suic::Element* sender, suic::RoutedPropChangedEventArg* e);
 
protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    void RegisterButtonEvent();
};

#endif

