/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

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

void MainWindow::OnButtonClicked(suic::Element* sender, suic::RoutedEventArg* e)
{
    if (sender->GetName().Equals("NorBtn"))
    {
        suic::Toast::Show(_U("点击了普通按钮"), 0);
    }
    else if (sender->GetName().Equals("BindBtn"))
    {
        suic::Toast::Show(_U("点击了绑定按钮"), 0);
    }

    e->SetHandled(true);
}

void MainWindow::RegisterButtonEvent()
{
    suic::ButtonBase* pBtn = NULL;

    pBtn = FindElem<suic::ButtonBase>("NorBtn");
    if (NULL != pBtn)
    {
        pBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnButtonClicked));
    }

    pBtn = FindElem<suic::ButtonBase>("BindBtn");
    if (NULL != pBtn)
    {
        ButtonData* btnData = new ButtonData();
        //
        // 把我们定义的数据绑定到按钮上
        //
        pBtn->SetContent(btnData);
        pBtn->SetDataContext(btnData);

        //pBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnButtonClicked));
    }
}
