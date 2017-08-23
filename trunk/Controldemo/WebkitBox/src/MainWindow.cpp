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
    }
    else if (sender->GetName().Equals("BindBtn"))
    {
    }
}

void MainWindow::RegisterButtonEvent()
{
    suic::ButtonBase* pBtn = NULL;

    // 按名称找到名为NorBtn的按钮对象
    pBtn = FindElem<suic::ButtonBase>("NorBtn");
    if (NULL != pBtn)
    {
        // 把我们定义的点击处理函数挂接到按钮的点击事件
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

        // 设置数据上下文，这样我们可以直接修改业务数据来动态改变绑定到界面上的内容
        pBtn->SetDataContext(btnData);
    }
}
