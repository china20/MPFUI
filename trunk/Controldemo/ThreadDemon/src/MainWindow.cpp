/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

#include <Framework/Controls/DatePicker.h>

ImplementRTTIOfClass(BindData, suic::NotifyPropChanged)

MainWindow::MainWindow()
{
    _thread = NULL;
    _reflesh = NULL;
}

MainWindow::~MainWindow()
{
}

bool MainWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void MainWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void MainWindow::OnInitialized(suic::EventArg* e)
{
    suic::Window::OnInitialized(e);
}

void MainWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    //
    // 窗口居中
    //
    CenterWindow();

    //
    // 注册按钮事件处理
    //
    RegisterButtonEvent();
}

void MainWindow::OnSelectedDateChanged(suic::Element* sender, suic::RoutedEventArg* e)
{
    suic::DatePicker* datePick = suic::DynamicCast<suic::DatePicker>(sender);
    if (NULL != datePick)
    {
        suic::DateTime dt = datePick->GetDate();
    }
}

void MainWindow::OnThreadButtonClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    if (sender->GetName().Equals(_U("showDialogBtn")))
    {
        TestThread* testThr = new TestThread(sender, TestThread::ctShowDialog);
        testThr->Start();
    }
    else if (sender->GetName().Equals(_U("showBtn")))
    {
        TestThread* testThr = new TestThread(sender, TestThread::ctShow);
        testThr->Start();
    }
    else if (sender->GetName().Equals(_U("visibilityBtn")))
    {
        TestThread* testThr = new TestThread(FindElem<suic::ListBox>(_U("listBox")), TestThread::ctSetVisibility);
        testThr->Start();
    }
    else if (sender->GetName().Equals(_U("notifyChangedBtn")))
    {
        TestThread* testThr = new TestThread(FindElem<suic::ListBox>(_U("listBox")), TestThread::ctNotifyChanged);
        //testThr->Start();

        testThr->ref();
        testThr->Run();
        testThr->unref();
    }
}

void MainWindow::OnResetButtonClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("listBox"));
    if (NULL != pListBox)
    {
        pListBox->GetItemsSource()->Clear();
        pListBox->GetItemsSource()->AddItem(new BindData(_U("wang rong")));
        pListBox->GetItemsSource()->AddItem(new BindData(_U("Sharpui")));
        pListBox->UpdateLayout();
    }
}

void MainWindow::RegisterButtonEvent()
{
    suic::Button* pReset = FindElem<suic::Button>("resetBtn");
    if (NULL != pReset)
    {
        pReset->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnResetButtonClick));
    }

    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("listBox"));
    if (NULL != pListBox)
    {
        _reflesh = new suic::InvokeProxy(pListBox);
        _reflesh->ref();
        _thread = new MyThread(_reflesh);
        _thread->ref();

        _reflesh->Invoker += suic::InvokerHandler(_thread, &MyThread::OnInvoker);
        _thread->Start();
        pListBox->GetItemsSource()->AddItem(new BindData(_U("wang rong")));
        pListBox->GetItemsSource()->AddItem(new BindData(_U("Sharpui")));
    }

    suic::Button* showDialog = FindElem<suic::Button>(_U("showDialogBtn"));
    if (NULL != showDialog)
    {
        showDialog->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnThreadButtonClick));
    }

    suic::Button* showBtn = FindElem<suic::Button>(_U("showBtn"));
    if (NULL != showBtn)
    {
        showBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnThreadButtonClick));
    }

    suic::Button* visibilityBtn = FindElem<suic::Button>(_U("visibilityBtn"));
    if (NULL != visibilityBtn)
    {
        visibilityBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnThreadButtonClick));
    }

    suic::Button* notifyChangedBtn = FindElem<suic::Button>(_U("notifyChangedBtn"));
    if (NULL != notifyChangedBtn)
    {
        notifyChangedBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnThreadButtonClick));
    }
}
