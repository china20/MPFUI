/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

MainWindow::MainWindow()
{
    _timer = new suic::AssignerTimer();
    _timer->ref();

    _driverGrp = new DriverGroup();
    _driverGrp->ref();

    //_driverLevel = new DriverLevel();
    //_driverLevel->ref();
}

MainWindow::~MainWindow()
{
    _timer->unref();
    _driverGrp->unref();
    //_driverLevel->unref();
}

void MainWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    CenterWindow();

    _timer->SetTick(suic::EventHandler(this, &MainWindow::OnRefleshTimer));
    _timer->SetInterval(1000);
    //_timer->Start();

    _tvDrivers = FindElem<suic::TreeView>("tvDriver");
    _tvDrivers->SetItemsSource(_driverGrp);

    _driverGrp->InitRootItems();

    /*_driverLevel->InitDrivers();

    for (int i = 0; _driverLevel->GetCount(); ++i)
    {
        _driverGrp->AddItem(_driverLevel->GetItem(i));
    }*/
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
    // _processInfo.RefleshProcessInfo();
}