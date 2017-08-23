/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

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

    suic::TextBlock* pLink = FindElem<suic::TextBlock>(_U("EditStyleProp"));
    if (NULL != pLink)
    {
        pLink->AddHandler(suic::Hyperlink::ClickEvent, new suic::ClickEventHandler(this, &MainWindow::OnEditStylePropClick));
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

    //
    // 采用代码完成设置
    //
    suic::TextBlock* txt = FindElem<suic::TextBlock>("MyTxtBlock");
    if (txt != NULL)
    {
        txt->GetInlines()->AddText("代码增加Text");
        suic::Run* pRun = new suic::Run("蓝色文本");
        pRun->SetForeground(new suic::SolidColorBrush(suic::Colors::Blue));
        txt->GetInlines()->AddChild(pRun);

        pRun = new suic::Run("字体设置");
        pRun->SetFontSize(16);
        pRun->SetFontWeight(suic::FontWeightStyle::fwExtraBold);
        txt->GetInlines()->AddChild(pRun);
    }
}

void MainWindow::OnEditStylePropClick(suic::Element* sender, suic::RoutedEventArg* e)
{

}

void MainWindow::OnButtonClicked(suic::Element* sender, suic::RoutedEventArg* e)
{
}

void MainWindow::OnHyperlinkClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::Element* pLink = CASTTOE(e->GetOriginalSource());
    if (pLink->GetName().Equals(_U("txtLink")))
    {
        suic::InfoBox::Show(_U("点击了Hyperlink"), _U("提示"));
    }
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
        pBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnButtonClicked));
    }

    suic::TextBlock* txt = FindElem<suic::TextBlock>("txtBind");
    if (NULL != txt)
    {
        txt->SetDataContext(new BindData());
    }

    // 挂接TextBlock里的Hyperlink点击事件
    suic::TextBlock* pLink = FindElem<suic::TextBlock>(_U("myBlk"));
    if (NULL != pLink)
    {
        pLink->AddHandler(suic::Hyperlink::ClickEvent, new suic::ClickEventHandler(this, &MainWindow::OnHyperlinkClick));
    }
}
