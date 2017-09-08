/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

MainWindow::MainWindow()
{
    _timer = new suic::AssignerTimer();
    _timer->ref();
}

MainWindow::~MainWindow()
{
    _timer->unref();
}

void MainWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    CenterWindow();

    _timer->SetTick(suic::EventHandler(this, &MainWindow::OnRefleshTimer));
    _timer->SetInterval(1000);
    _timer->Start();

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

void MainWindow::OnRefleshTimer(suic::Object* sender, suic::EventArg* e)
{
    _processInfo.RefleshProcessInfo();
}