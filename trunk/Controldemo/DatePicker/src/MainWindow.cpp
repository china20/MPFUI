/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

#include <Framework/Controls/DatePicker.h>

ImplementRTTIOfClass(ButtonData, suic::NotifyPropChanged)

MainWindow::MainWindow()
{
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
    Object* res = FindRes(_U("Calendar"));
    if (res != NULL)
    {
        res = NULL;
    }
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

void MainWindow::OnSelectedDateChanged(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    suic::DatePicker* datePick = suic::RTTICast<suic::DatePicker>(sender);
    if (NULL != datePick)
    {
        suic::DateTime dt = datePick->GetDate();
    }
}

void MainWindow::RegisterButtonEvent()
{
    suic::DatePicker* pDate = FindElem<suic::DatePicker>("myDate");
    if (NULL != pDate)
    {
        pDate->AddSelectedDateChanged(new suic::RoutedEventHandler(this, &MainWindow::OnSelectedDateChanged));
    }
}
