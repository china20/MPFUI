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

/// <Summary>
///  用户数据结构实现ICommand接口，可以接收诸如按钮之类的点击命令事件
///  事件（因为按钮的命令事件是可路由的），这样比如ListBox，TreeView等
///  可以自动处理其模版中定义的按钮的点击事件
/// <Summary/>
class NameField : public suic::NotifyPropChanged
{
public:

    BeginMember(NameField, suic::NotifyPropChanged)
        MemberInt(Number)
        MemberString(Name)
        MemberGetObject(Tel)
        MemberObject(SelTel)
        MemberString(Sexy)
        MemberObject(Checked)
        MemberAttributes(Attris)
    EndMember()

    RTTIOfClass(NameField);

    NameField()
    {
        _tel.setAutoDelete(false);
        _checked = suic::Boolean::True;
    }

    NameField(int num, suic::String name)
    {
        _tel.setAutoDelete(false);
        _number = num;
        _propName = name;
        _addr = _U("中国广东深圳");
        _tel.AddItem(new suic::OString(_U("13500013800")));
        _tel.AddItem(new suic::OString(_U("13500013801")));
        _personId = _U("52020298888081231");
        _checked = suic::Boolean::True;
    }

    int GetNumber()
    {
        return _number;
    }

    void SetNumber(int val)
    {
        _number = val;
    }

    suic::String GetName()
    {
        return _propName;
    }

    void SetName(suic::String name)
    {
        _propName = name;
    }

    suic::String GetAddr()
    {
        return _addr;
    }

    void SetAddr(suic::String val)
    {
        _addr = val;
    }

    suic::Object* GetSelTel()
    {
        return _selItem.get();
    }

    void SetSelTel(suic::Object* obj)
    {
        _selItem = obj;
    }

    suic::Object* GetTel()
    {
        return &_tel;
    }

    suic::String GetSexy()
    {
        return _sexy;
    }

    void SetSexy(suic::String val)
    {
        _sexy = val;
        NotifyChanged(_U("Sexy"));
    }

    suic::String GetPersonId()
    {
        return _personId;
    }

    void SetPersonId(suic::String val)
    {
        _personId = val;
    }

    suic::Object* GetChecked()
    {
        return _checked;
    }

    void SetChecked(suic::Object* obj)
    {
        _checked = suic::DynamicCast<suic::Boolean>(obj);
    }

    bool SetAttris(const suic::String& name, suic::Object* obj)
    {
        if (name.Equals(_U("PersonId")))
        {
            return true;
        }
        else if (name.Equals(_U("Addr")))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool GetAttris(const suic::String& name, suic::ObjectPtr& obj)
    {
        if (name.Equals(_U("PersonId")))
        {
            obj = new suic::OString(_personId);
            return true;
        }
        else if (name.Equals(_U("Addr")))
        {
            obj = new suic::OString(_addr);
            return true;
        }
        else
        {
            return false;
        }
    }

    suic::String ToString()
    {
        suic::String strFmt;
        strFmt.Format(_U("Data Binding: %d->%s"), _number, _propName.c_str());
        return strFmt;
    }

private:

    int _number;
    suic::Boolean* _checked;
    suic::String _propName;
    suic::String _addr;
    suic::ObjectPtr _selItem;
    suic::ObservableCollection _tel;
    suic::String _sexy;
    suic::String _personId;
};

class MainWindow : public suic::Window
{
public:

    BeginRoutedEvent(MainWindow, suic::Window)
        MemberRouted(OnResetListButtonClick)
        MemberRouted(OnDeleteListButtonClick)
    EndRoutedEvent()

    MainWindow();
    ~MainWindow();

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnBtnClicked(suic::Element* sender, suic::RoutedEventArg* e);
    void OnItemDbClck(suic::Element* sender, suic::MouseButtonEventArg* e);
    void OnClipListView(suic::Element* sender, suic::RoutedEventArg* e);

    void OnItemSelectionChanged(suic::Element* sender, suic::SelectionChangedEventArg* e);

protected:

    void InsertUserItem(suic::ListView* listView);

    void OnDeleteListButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnResetListButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

    void OnTimer(suic::Object* sender, suic::EventArg* e);

private:

    void RegisterButtonEvent();

private:

    suic::AssignerTimer* _timer;
};

#endif
