/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

#include <Framework/Controls/DatePicker.h>

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
}

void MainWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    //
    // 窗口居中
    //
    CenterWindow();
}

void MainWindow::OnClickButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::InfoBox::Show("事件配置测试", "提示");
}

void MainWindow::RegisterButtonEvent()
{
    
}
