/////////////////////////////////////////////////////
// ChildWindow.cpp

#include "stdafx.h"
#include "ChildWindow.h"

ChildWindow::ChildWindow()
    : _openWnd(NULL)
{
}

ChildWindow::~ChildWindow()
{
    if (NULL != _openWnd)
    {
        _openWnd->unref();
    }
}

bool ChildWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void ChildWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void ChildWindow::OnInitialized(suic::EventArg* e)
{
    suic::Window::OnInitialized(e);
}

void ChildWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    //
    // 窗口居中
    //
    CenterWindow(0);

    //
    // 注册按钮事件处理
    //
    RegisterButtonEvent();
}

void ChildWindow::OnButtonClicked(suic::Element* sender, suic::RoutedEventArg* e)
{
    if (sender->GetName().Equals("OpenWnd"))
    {
        if (NULL == _openWnd)
        {
            _openWnd = new suic::Window();
            _openWnd->ref();
            suic::Builder::BuildUri(_openWnd, "ControlDemo/WindowDemon/ChildWindow.xml");
        }

        _openWnd->ShowInTaskBar(false);
        _openWnd->Show();
    }
    else if (sender->GetName().Equals("BindBtn"))
    {
    }
}

void ChildWindow::RegisterButtonEvent()
{
    suic::ButtonBase* pBtn = NULL;

    // 按名称找到名为NorBtn的按钮对象
    pBtn = FindElem<suic::ButtonBase>("OpenWnd");
    if (NULL != pBtn)
    {
        // 把我们定义的点击处理函数挂接到按钮的点击事件
        pBtn->AddClick(new suic::ClickEventHandler(this, &ChildWindow::OnButtonClicked));
    }
}
