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

void MainWindow::OnClickPrevImage(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    DriverItem* pItem = NULL;

    e->SetHandled(true);
    pItem = dynamic_cast<DriverItem*>(_tvDrivers->GetSelectedItem());

    if (NULL != pItem)
    {
        pItem->PrevImage();
        ShowImage(pItem);
    }
}

void MainWindow::OnClickNextImage(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    DriverItem* pItem = NULL;

    e->SetHandled(true);
    pItem = dynamic_cast<DriverItem*>(_tvDrivers->GetSelectedItem());

    if (NULL != pItem)
    {
        pItem->NextImage();
        ShowImage(pItem);
    }
}

void MainWindow::OnTreeViewSelectedItemChanged(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    DriverItem* pItem = NULL;

    e->SetHandled(true);
    pItem = dynamic_cast<DriverItem*>(_tvDrivers->GetSelectedItem());

    ShowImage(pItem);
}

void MainWindow::ShowImage(DriverItem* pItem)
{
    if (NULL != pItem && pItem->GetPicCount() > 0)
    {
        suic::Image* pImg = FindElem<suic::Image>("imgShow");
        suic::ImageSource* pImgSrc = pImg->GetSource();

        if (NULL == pImgSrc)
        {
            pImgSrc = new suic::ImageSource();
            pImg->SetSource(pImgSrc);
        }

        pImgSrc->SetUri(pItem->GetPicPath());

        UpdateLayout();
    }
}

void MainWindow::OnRefleshTimer(suic::Object* sender, suic::EventArg* e)
{
    ;
}