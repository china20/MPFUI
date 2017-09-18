/////////////////////////////////////////////////////
// SettingWindow.cpp

#include "stdafx.h"
#include "SettingWindow.h"

ImplementRTTIOfClass(SettingData, suic::NotifyPropChanged)

ZipSetWindow::ZipSetWindow()
{
    _settingData = NULL;
}

ZipSetWindow::~ZipSetWindow()
{
    if (NULL != _settingData)
    {
        _settingData->unref();
    }
}

void ZipSetWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    CenterWindow();

    _settingData = new SettingData();
    _settingData->ref();

    SetDataContext(_settingData);
}

bool ZipSetWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void ZipSetWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void ZipSetWindow::OnClickApply(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
}

void ZipSetWindow::OnClickCancel(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    AsyncClose();
}

void ZipSetWindow::OnClickOk(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    AsyncClose();
}
