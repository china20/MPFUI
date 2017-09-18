/////////////////////////////////////////////////////
// ZipWindow.cpp

#include "stdafx.h"
#include "ZipWindow.h"

ImplementRTTIOfClass(ZipData, suic::NotifyPropChanged)

ZipWindow::ZipWindow()
{
    _zipData = NULL;
    _zipThread = NULL;
    _reflesh = NULL;
    _driverGrp = new DriverGroup();
    _driverGrp->ref();
}

ZipWindow::~ZipWindow()
{
    if (NULL != _zipData)
    {
        _zipData->unref();
    }

    if (NULL != _zipThread)
    {
        _zipThread->unref();
    }

    if (NULL != _reflesh)
    {
        _reflesh->unref();
    }

    _driverGrp->unref();
}

void ZipWindow::UpdateWindowTitle()
{
    SetTitle(suic::String().Format(_U("压缩文件：%s"), _zipData->GetZipFile().c_str()));
}

void ZipWindow::OnInvoker(suic::Object* sender, suic::InvokerArg* e)
{
    InvalidateDataProp(_zipData);
}

void ZipWindow::StopZip()
{
    if (NULL != _zipThread)
    {
        _zipThread->Stop();
        _zipThread->Join();
        _zipThread->unref();

        _zipThread = NULL;
    }
}

void ZipWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    CenterWindow();

    _driverGrp->InitRootItems();

    _zipData = new ZipData();
    _zipData->ref();

    _zipData->SetMinBulk(false);
    _zipData->SetMostSpeed(true);
    _zipData->SetZipProgress(20);
    _zipData->SetZipFile("c:/test.tar");

    UpdateWindowTitle();

    SetDataContext(_zipData);

    _reflesh = new suic::InvokeProxy(this);
    _reflesh->ref();
    _reflesh->Invoker += suic::InvokerHandler(this, &ZipWindow::OnInvoker);
}

bool ZipWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void ZipWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void ZipWindow::OnClickChangeDir(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // 点击更新目录按钮，做相应业务处理

    e->SetHandled(true);

    /*suic::DirBrowser db;
    if (db.Open())
    {
        _zipData->SetZipFile(db.GetDir());
        UpdateWindowTitle();
    }*/
    _zipData->SetZipFile(_selectedDir);
    UpdateWindowTitle();
}

void ZipWindow::OnClickZip(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // 点击立即压缩，做相应业务处理

    e->SetHandled(true);

    if (_zipData->GetMostSpeed())
    {
        _zipData->DoZip();

        StopZip();

        _zipThread = new ZipThread(_zipData, _reflesh);
        _zipThread->ref();
        _zipThread->Start();
    }
}

void ZipWindow::OnClickPause(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // 点击暂停按钮，做相应业务处理

    e->SetHandled(true);
}

void ZipWindow::OnClickCancel(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // 点击取消按钮，做相应业务处理

    e->SetHandled(true);
    AsyncClose();
}

void ZipWindow::OnCheckedDown(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    DriverItem* pItem = NULL;
    suic::TreeView* pTreeView = NULL;
    suic::Popup* pPopup = NULL;
    suic::ToggleButton* pTB = NULL;
    
    pPopup = new suic::Popup();
    pPopup->ref();

    pTB = suic::DynamicCast<suic::ToggleButton>(sender);

    pTreeView = new suic::TreeView();
    pTreeView->SetWidth(pTB->GetParent()->GetParent()->GetActualWidth());
    pTreeView->SetHeight(160);

    pTreeView->SetItemsSource(_driverGrp);
  
    //suic::InfoBox::Show(_U("正在压缩文件，是否确定退出 ？"), _U("提示"), suic::InfoBox::IB_YesNo);

    pPopup->SetPlacementTarget(pTB->GetParent()->GetParent());
    pPopup->SetPlacement(suic::PlacementMode::pmBottom);

    pPopup->SetChild(pTreeView);
    pPopup->TrackingPopup();

    pItem = suic::DynamicCast<DriverItem>(pTreeView->GetSelectedItem());

    if (NULL != pItem)
    {
        _selectedDir = pItem->GetFilePath();
    }

    pTB->SetChecked(suic::Boolean::False);
    pPopup->unref();
}

void ZipWindow::OnClosing(suic::CancelEventArg* e)
{
    if (_zipThread != NULL && _zipThread->IsAlive())
    {
        int iResult = suic::InfoBox::Show(_U("正在压缩文件，是否确定退出 ？"), _U("提示"), suic::InfoBox::IB_YesNo);
        if (iResult == suic::InfoBox::Yes)
        {
            StopZip();
        }
        else
        {
            e->SetCancel(true);
        }
    }
}

