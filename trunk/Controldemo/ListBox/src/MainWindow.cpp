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
    suic::DatePicker* datePick = suic::RTTICast<suic::DatePicker>(sender);
    if (NULL != datePick)
    {
        suic::DateTime dt = datePick->GetDate();
    }
}

void MainWindow::OnResetButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("listBox"));
    if (NULL != pListBox)
    {
        pListBox->GetItemsSource()->Clear();
        //pListBox->GetItemsSource()->AddItem(new BindData(_U("ListBox演示")));
        //pListBox->GetItemsSource()->AddItem(new BindData(_U("MPF界面引擎")));
        //pListBox->UpdateLayout();
    }
}

void MainWindow::OnStopButtonClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    if (NULL != _thread)
    {
        suic::Button* stopBtn = suic::RTTICast<suic::Button>(sender);
        if (_thread->IsAlive())
        {
            _thread->Stop();
            _thread->Join();
            stopBtn->AddText(_U("开始刷新"));
        }
        else
        {
            _thread->Start();
            stopBtn->AddText(_U("停止刷新"));
        }
    }
}

void MainWindow::OnDeleteButtonClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("listBox"));

    if (NULL != pListBox)
    {
        suic::OAutoColl selItems;
        pListBox->GetSelectedItems(selItems);
        for (int i = 0; i < selItems.GetCount(); ++i)
        {
            pListBox->GetItemsSource()->RemoveItem(selItems.GetItem(i));
        }
    }
}

void MainWindow::OnSelModeButtonClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::String name(sender->GetName());
    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("listBox"));

    if (NULL != pListBox)
    {
        if (name.Equals(_U("rSingle")))
        {
            pListBox->SetSelectionMode(suic::SelectionMode::eSingle);
        }
        else if (name.Equals(_U("rMultiple")))
        {
            pListBox->SetSelectionMode(suic::SelectionMode::eMultiple);
        }
        else if (name.Equals(_U("rExtended")))
        {
            pListBox->SetSelectionMode(suic::SelectionMode::eExtended);
        }
    }
}

void MainWindow::RegisterButtonEvent()
{
    suic::Button* pReset = FindElem<suic::Button>("resetBtn");
    if (NULL != pReset)
    {
        //pReset->AddClick(new suic::RoutedEventHandler(this, &MainWindow::OnResetButtonClick));
    }

    suic::Button* stopBtn = FindElem<suic::Button>("stopBtn");
    if (NULL != stopBtn)
    {
        stopBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnStopButtonClick));
    }

    suic::Button* delBtn = FindElem<suic::Button>("delBtn");
    if (NULL != delBtn)
    {
        delBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnDeleteButtonClick));
    }

    
    suic::RadioButton* rBtn = FindElem<suic::RadioButton>("rSingle");
    if (NULL != rBtn)
    {
        rBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnSelModeButtonClick));
    }
    rBtn = FindElem<suic::RadioButton>("rMultiple");
    if (NULL != rBtn)
    {
        rBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnSelModeButtonClick));
    }
    rBtn = FindElem<suic::RadioButton>("rExtended");
    if (NULL != rBtn)
    {
        rBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnSelModeButtonClick));
    }

    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("listBox"));
    if (NULL != pListBox)
    {
        _reflesh = new suic::InvokeProxy(pListBox);
        _reflesh->ref();
        _thread = new MyThread(_reflesh);
        _thread->ref();

        _reflesh->Invoker += suic::InvokerHandler(_thread, &MyThread::OnInvoker);
        //_thread->Start();
        pListBox->GetItemsSource()->AddItem(new BindData(_U("ListBox演示")));
        pListBox->GetItemsSource()->AddItem(new BindData(_U("MPF界面引擎")));
    }
    
}
