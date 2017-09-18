/////////////////////////////////////////////////////
// UpdateWindow.cpp

#include "stdafx.h"
#include "UpdateWindow.h"

ImplementRTTIOfClass(UpdateAppData, suic::NotifyPropChanged)

UpdateAppWindow::UpdateAppWindow()
{
    _updateData = NULL;
}

UpdateAppWindow::~UpdateAppWindow()
{
    if (NULL != _updateData)
    {
        _updateData->unref();
    }
}

void UpdateAppWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    CenterWindow();

    _updateData = new UpdateAppData();
    _updateData->ref();

    _updateData->SetVersion("2.0.1.19");

    SetDataContext(_updateData);
}

bool UpdateAppWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void UpdateAppWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void UpdateAppWindow::OnClickUpdate(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    // 点击更新按钮，做相应业务处理
}
