/////////////////////////////////////////////////////
// UnzipWindow.cpp

#include "stdafx.h"
#include "UnzipWindow.h"

ImplementRTTIOfClass(UnzipData, suic::NotifyPropChanged)

UnzipWindow::UnzipWindow()
{
    _unzipData = NULL;
}

UnzipWindow::~UnzipWindow()
{
    if (NULL != _unzipData)
    {
        _unzipData->unref();
    }
}

void UnzipWindow::UpdateWindowTitle()
{
    SetTitle(suic::String().Format(_U("解压文件：%s"), _unzipData->GetUnzipFile().c_str()));
}

void UnzipWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    CenterWindow();

    _unzipData = new UnzipData();
    _unzipData->ref();

    _unzipData->SetUnzipProgress(20);
    _unzipData->SetUnzipFile("c:/test.tar");

    UpdateWindowTitle();

    SetDataContext(_unzipData);
}

bool UnzipWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void UnzipWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void UnzipWindow::OnClickChangeDir(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // 点击更新目录按钮，做相应业务处理

    e->SetHandled(true);

    suic::FileBrowser fb;

    if (fb.Open(NULL, _U("ZIP Files(*.*)\0*.*\0\0")))
    {
        _unzipData->SetUnzipFile(fb.GetFilePath());
        UpdateWindowTitle();
    }
}

void UnzipWindow::OnClickUnzip(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // 点击立即解压按钮，做相应业务处理

    e->SetHandled(true);
    _unzipData->DoUnzip();

    //InvalidateDataProp(_unzipData);
}

void UnzipWindow::OnClickPause(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // 点击暂停按钮，做相应业务处理

    e->SetHandled(true);
}

void UnzipWindow::OnClickCancel(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // 点击取消按钮，做相应业务处理

    e->SetHandled(true);
    AsyncClose();
}
