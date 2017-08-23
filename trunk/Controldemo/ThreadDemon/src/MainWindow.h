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
        MemberGetObject(Icon)
        MemberString(Name)
    EndMember()

    RTTIOfClass(BindData)

    BindData()
    {
        _name = "Bind Button";
        _icon = NULL;
    }

    BindData(suic::String name)
    {
        _name = name;
        _icon = NULL;
    }

    suic::Object* GetIcon()
    {
        return _icon;
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

    suic::Brush* _icon;
    suic::String _name;
};

class TestThread : public suic::Thread
{
public:

    enum eCallType
    {
        ctShow,
        ctShowDialog,
        ctSetVisibility,
        ctNotifyChanged,
    };

    TestThread(suic::Element* sender, int callType)
        : _sender(sender)
        , _callType(callType)
    {
    }

    void Run()
    {
        switch (_callType)
        {
        case eCallType::ctShow:
            {
                suic::WindowPtr wndDialog = new suic::Window();
                wndDialog->Show(_U("ControlDemo/ThreadDemon/ThreadWindow.xml"), NULL);
            }
            break;

        case eCallType::ctShowDialog:
            {
                suic::WindowPtr wndDialog = new suic::Window();
                wndDialog->ShowDialog(_U("ControlDemo/ThreadDemon/ThreadWindow.xml"));
            }
            break;

        case eCallType::ctSetVisibility:
            if (_sender->IsVisible())
            {
                _sender->SetVisibility(suic::Visibility::Hidden);
            }
            else
            {
                _sender->SetVisibility(suic::Visibility::Visible);
            }
            break;

        case eCallType::ctNotifyChanged:
            {
                suic::ListBox* pListBox = suic::RTTICast<suic::ListBox>(_sender);
                if (NULL != pListBox && pListBox->GetCount() > 1)
                {
                    BindData* pData = suic::RTTICast<BindData>(pListBox->GetItem(1));
                    pData->SetName(_U("NotifyCanged演示（调用SetName动态改变值）"));
                }
            }
            break;
        }
    }

protected:

    suic::Int32 _callType;
    suic::Element* _sender;
};

class MyThread : public suic::Thread
{
public:

    MyThread(suic::InvokeProxy* h)
    {
        _handler = h;
        _bExit = false;
    }

    void Run()
    {
        for (;;)
        {
            if (_bExit) 
            {
                break;
            }
            _handler->PostInvoker(0, NULL);
            Sleep(500);
        }
    }

    void OnInvoker(suic::Object* sender, suic::InvokerArg* e)
    {
        suic::String text;
        static int i = 0;
        text.Format(_U("多线程刷新Invoker_%d"), ++i);
        suic::ListBox* listBox = (suic::ListBox*)_handler->GetTarget();

        if (listBox->GetItemsSource()->GetCount() > 5)
        {
            _bExit = true;
            return;
            listBox->GetItemsSource()->Clear();
        }

        listBox->AddChild(new BindData(text));
        listBox->ScrollToEnd();
    }

protected:

    bool _bExit;
    suic::InvokeProxy* _handler;
};

class MainWindow : public suic::Window
{
public:

    MainWindow();
    ~MainWindow();

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnSelectedDateChanged(suic::Element* sender, suic::RoutedEventArg* e);

    void OnResetButtonClick(suic::Element* sender, suic::RoutedEventArg* e);
    void OnThreadButtonClick(suic::Element* sender, suic::RoutedEventArg* e);
 
protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    void RegisterButtonEvent();
    MyThread* _thread;
    suic::InvokeProxy* _reflesh;
};

#endif

