/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

#include <System/Tools/TreeBuilder.h>
#include <Framework/Controls/DatePicker.h>

ImplementRTTIOfClass(BindingData, suic::NotifyPropChanged)

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

void MainWindow::OnCheckedChanged(suic::Element* sender, suic::RoutedEventArg* e)
{
}

void MainWindow::OnPopupWindow(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::String strPath = _T("ControlDemo/CheckBox/PopupWindow.xml");
    suic::WindowPtr wnd = suic::XamlReader::LoadUri(NULL, NULL, strPath.c_str()).get();
    if (wnd)
    {
        wnd->Show();
        wnd->CenterWindow();
    }
}

void MainWindow::RegisterButtonEvent()
{
    suic::CheckBox* pDate = FindElem<suic::CheckBox>("myDate");
    if (NULL != pDate)
    {
        ;
    }

    suic::Button* pBtn = FindElem<suic::Button>("OpenPop");
    if (NULL != pBtn)
    {
        pBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnPopupWindow));
    }
}
