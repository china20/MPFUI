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

    //
    // 注册按钮事件处理
    //
    RegisterButtonEvent();
}

void MainWindow::OnSelectedDateChanged(suic::Element* sender, suic::RoutedEventArg* e)
{
}

void MainWindow::OnResetButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
}

void MainWindow::OnStopButtonClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
}

void MainWindow::OnDeleteButtonClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
}

void MainWindow::OnSelModeButtonClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
}

GroupData* MainWindow::NewGroupData(suic::String name)
{
    GroupData* group = new GroupData(name);
    group->GetItems()->AddItem(new suic::OString(_U("Sub Group 1")));
    group->GetItems()->AddItem(new suic::OString(_U("Sub Group 2")));
    group->GetItems()->AddItem(new suic::OString(_U("Sub Group 3")));
    group->GetItems()->AddItem(new suic::OString(_U("Sub Group 4")));
    group->GetItems()->AddItem(new suic::OString(_U("Sub Group 5")));
    return group;
}

void MainWindow::RegisterButtonEvent()
{
    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("listBox"));
    if (NULL != pListBox)
    {
        GroupData* group = NewGroupData(_U("一级分组 1"));
        GroupData* subGroup = new GroupData(_U("二级分组 1-1"));

        subGroup->GetItems()->AddItem(new suic::OString(_U("Sub Group 1-1")));
        subGroup->GetItems()->AddItem(new suic::OString(_U("Sub Group 1-2")));
        subGroup->GetItems()->AddItem(new suic::OString(_U("Sub Group 1-3")));

        group->GetItems()->AddItem(subGroup);
        pListBox->AddChild(group);
        pListBox->AddChild(NewGroupData(_U("一级分组 2")));
        pListBox->AddChild(NewGroupData(_U("一级分组 3")));
    }
    
}
