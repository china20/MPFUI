/////////////////////////////////////////////////////
// AddNameWindow.cpp

#include "stdafx.h"
#include <Main/AddNameWindow.h>

AddNameWindow::AddNameWindow(const String& title, const String& name, int wid)
{
    _title = title;
    _name = name;
    _width = wid;
}

AddNameWindow::~AddNameWindow()
{
}

String AddNameWindow::GetName() const
{
    return _value;
}

void AddNameWindow::OnOkButtonClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::TextBox* txtBox = FindElem<suic::TextBox>(_U("txtVal"));
    if (NULL != txtBox)
    {
        _value = txtBox->GetText();
    }
    
    if (_value.Empty())
    {
        suic::InfoBox::Show(_U("名称为空或不合法,请重新输入!"), _U("提示"));
    }
    else
    {
        AsyncClose();
    }
}

bool AddNameWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void AddNameWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void AddNameWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
    suic::TextBlock* txtName = FindElem<suic::TextBlock>(_U("txtName"));
    if (NULL != txtName)
    {
        if (_name.Empty())
        {
            txtName->SetVisibility(Visibility::Collapsed);
        }
        else
        {
            txtName->SetText(_name);
        }
    }
    if (!_title.Empty())
    {
        SetTitle(_title);
    }

    if (_width > 0)
    {
        SetWidth(_width);
    }

    suic::Button* pOkBtn = FindElem<suic::Button>(_U("OkBtn"));
    if (NULL != pOkBtn)
    {
        pOkBtn->AddClick(new suic::ClickEventHandler(this, &AddNameWindow::OnOkButtonClick));
    }
}

void AddNameWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}
