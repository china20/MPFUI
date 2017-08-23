/////////////////////////////////////////////////////
// SettingWindow.cpp

#include "stdafx.h"
#include "SettingWindow.h"

SettingWindow::SettingWindow()
    : _openWnd(NULL)
{
}

SettingWindow::~SettingWindow()
{
    if (NULL != _openWnd)
    {
        _openWnd->unref();
    }
}

bool SettingWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void SettingWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void SettingWindow::OnInitialized(suic::EventArg* e)
{
    suic::Window::OnInitialized(e);

    //
    // 注册按钮事件处理
    //
    RegisterButtonEvent();
}

void SettingWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    //
    // 窗口居中
    //
    CenterWindow(0);
}

void SettingWindow::OnButtonClicked(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    if (NULL == _openWnd)
    {
        _openWnd = new suic::Window();
        _openWnd->ref();
        suic::Builder::BuildUri(_openWnd, "ControlDemo/MFCDemon/ShowWindow.xml");
    }
    _openWnd->ShowDialog();
}

void SettingWindow::RegisterButtonEvent()
{
}
