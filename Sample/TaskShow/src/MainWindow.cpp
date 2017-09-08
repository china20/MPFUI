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

void MainWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    CenterWindow();

    _processInfo.RefleshProcessInfo();

    _listProcess = FindElem<suic::ListView>("listProcess");
    _listProcess->SetItemsSource(_processInfo.GetProcessColl());

    //_listProcess->SetItemsPanel(new suic::ItemsPanelTemplate(new suic::FEFactory(suic::WrapPanel::RTTIType())));
}


bool MainWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void MainWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}
