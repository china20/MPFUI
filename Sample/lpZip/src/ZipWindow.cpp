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
}

void ZipWindow::UpdateWindowTitle()
{
    SetTitle(suic::String().Format(_U("压缩文件：%s"), _zipData->GetZipFileDir().c_str()));
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

    _zipData = new ZipData();
    _zipData->ref();

    _zipData->SetMinBulk(false);
    _zipData->SetMostSpeed(true);
    _zipData->SetZipProgress(20);
    _zipData->SetZipFileDir("c:/test/unzip");

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

    suic::FileBrowser fb;

    if (fb.Open(NULL, _U("ZIP Files(*.*)\0*.*\0\0")))
    {
        _zipData->SetZipFile(fb.GetFilePath());
        UpdateWindowTitle();
    }
}

void ZipWindow::OnClickZip(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // 点击立即压缩，做相应业务处理

    e->SetHandled(true);

    suic::String strZipPath = _zipData->GetZipFile();

    strZipPath.Trim();

    if (strZipPath.Empty())
    {
        suic::InfoBox::Show(_U("压缩目标文件路径为空，请重新输入！"), _U("提示"));
        return ;
    }

    _zipData->DoZip();

    StopZip();

    _zipThread = new ZipThread(_zipData, _reflesh);
    _zipThread->ref();
    _zipThread->Start();
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

