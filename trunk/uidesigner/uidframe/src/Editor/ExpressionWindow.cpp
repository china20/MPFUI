/////////////////////////////////////////////////////
// ExpressionWindow.cpp

#include "stdafx.h"
#include <Editor/ExpressionWindow.h>

ExpressionWindow::ExpressionWindow()
    : _fromTemplate(false)
{
}

ExpressionWindow::ExpressionWindow(bool fromTemp)
    : _fromTemplate(fromTemp)
{
}

ExpressionWindow::~ExpressionWindow()
{
}

String ExpressionWindow::GetValue() const
{
    return _value;
}

void ExpressionWindow::SetPreviousValue(const String& val)
{
    _value = val;
}

void ExpressionWindow::OnOkButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::TextBox* txtBox = FindElem<suic::TextBox>(_U("txtVal"));
    if (NULL != txtBox)
    {
        _value = txtBox->GetText();
    }

    _value.Trim();

    if (_value.Empty() || _value.Length() <= 2)
    {
        suic::Toast::Show(_U("表达式为空或不合法,请重新输入!"), 2);
    }
    else if (_value[0] != '{')
    {
        suic::Toast::Show(_U("表达式不合法(缺少'{'),请重新输入!"), 2);
    }
    else if (_value[_value.Length() - 1] != '}')
    {
        suic::Toast::Show(_U("表达式不合法(缺少'}'),请重新输入!"), 2);
    }
    else if (!_fromTemplate && _value.IndexOf(_U("{TemplateBinding")) == 0)
    {
        suic::Toast::Show(_U("不是控件模版，表达式不能是TemplateBinding!"), 2);
    }
    else
    {
        SetDialogResult(0);
        AsyncClose();
    }
}

bool ExpressionWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void ExpressionWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void ExpressionWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
    suic::TextBox* txtBox = FindElem<suic::TextBox>(_U("txtVal"));
    if (NULL != txtBox)
    {
        txtBox->SetText(_value);
    }

    if (!_fromTemplate)
    {
        suic::Element* pTemp = FindName(_U("rTemplateBinding"));
        if (pTemp != NULL)
        {
            pTemp->SetVisibility(suic::Visibility::Collapsed);
        }
    }
    _value = _U("");
    SetDialogResult(-1);
}

void ExpressionWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}
