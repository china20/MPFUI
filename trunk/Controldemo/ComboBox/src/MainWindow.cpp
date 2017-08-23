/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

#include <Framework/Controls/DatePicker.h>

ImplementRTTIOfClass(ComboBoxData, suic::NotifyPropChanged)

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

void MainWindow::OnSelItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e)
{
    suic::InfoBox::Show(_U("MainWindow::OnSelItemChanged"), _U("提示"));
}

void MainWindow::OnComboBoxTextChanged(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    suic::TextBlock* pTb = FindElem<suic::TextBlock>(_U("txtChanged"));
    suic::TextEditor* pText = suic::RTTICast<suic::TextEditor>(e->GetOriginalSource());
    if (NULL != pTb && NULL != pText)
    {
        pTb->SetText(pText->GetText());
    }
    //suic::InfoBox::Show(_U("MainWindow::OnComboBoxTextChanged"), _U("提示"));
}

void MainWindow::OnSwitchBtnClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    suic::ComboBox* cmbBox = FindElem<suic::ComboBox>(_U("myCmbOnlyRead"));
    if (NULL != cmbBox)
    {
        cmbBox->GetItemsSource()->Clear();
        cmbBox->AddText(_U("北京"));
        cmbBox->AddText(_U("贵阳"));
        cmbBox->AddText(_U("天津"));
        cmbBox->SetSelectedIndex(1);
    }
}

void MainWindow::RegisterButtonEvent()
{
    suic::ComboBox* cmb = FindElem<suic::ComboBox>("myCmb");
    if (NULL != cmb)
    {
        cmb->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &MainWindow::OnSelItemChanged));
        cmb->GetTextBoxSite()->AddTextChanged(new suic::RoutedEventHandler(this, &MainWindow::OnComboBoxTextChanged));
        cmb->AddChild(new suic::OString("386666951"));
        cmb->AddChild(new suic::OString("11232312321"));
    }

    cmb = FindElem<suic::ComboBox>("myCmbBind");
    if (NULL != cmb)
    {
        cmb->SetDataContext(new ComboBoxData());
    }

    cmb = FindElem<suic::ComboBox>("myCmbOnlyRead");
    if (NULL != cmb)
    {
        cmb->AddChild(new suic::OString("386666951"));
        cmb->AddChild(new suic::OString("11232312321"));
    }

    suic::Button* btn = FindElem<suic::Button>(_U("SwitchBtn"));
    if (NULL != btn)
    {
        btn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnSwitchBtnClick));
    }
}
