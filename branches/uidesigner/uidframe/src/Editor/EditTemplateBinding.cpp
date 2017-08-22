/////////////////////////////////////////////////////
// EditTemplateBinding.cpp

#include "stdafx.h"

#include <Main/DesignPanel.h>
#include <Editor/EditTemplateBinding.h>

EditTemplateBindingWindow::EditTemplateBindingWindow(RTTIOfInfo* rttiInfo, DpProperty* dp)
{
    _rttiInfo = rttiInfo;
    _dp = dp;
}

EditTemplateBindingWindow::~EditTemplateBindingWindow()
{
}

String EditTemplateBindingWindow::GetBindingProp() const
{
    return _bindingProp;
}

void EditTemplateBindingWindow::OnButtonClick(Element* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
    if (sender->GetName().Equals(_U("OkBtn")))
    {
         suic::TextBox* bindProp = FindElem<suic::TextBox>(_U("bindProp"));
        if (NULL != bindProp)
        {
            _bindingProp = bindProp->GetText();
        }
    }
    else
    {
        _bindingProp = _U("");
    }

    AsyncClose();
}

void EditTemplateBindingWindow::OnItemSelectionChanged(Element* sender, SelectionChangedEventArg* e)
{
    if (e->GetAddedItems()->GetCount() > 0)
    {
        suic::TextBox* bindProp = FindElem<suic::TextBox>(_U("bindProp"));
        if (NULL != bindProp)
        {
            bindProp->SetText(e->GetAddedItems()->GetItem(0)->ToString());
        }
    }
}

void EditTemplateBindingWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
    suic::Button* pBtn = NULL;
    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("propList"));
    if (NULL != _rttiInfo && _dp != NULL && NULL != pListBox)
    {
        ItemCollection* itemColl = pListBox->GetItemsSource();
        DpManager::Ins()->FillPropByDp(itemColl, _rttiInfo, _dp);

        pListBox->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &EditTemplateBindingWindow::OnItemSelectionChanged));

        pBtn = FindElem<suic::Button>(_U("CancelBtn"));
        if (NULL != pBtn)
        {
            pBtn->AddClick(new ClickEventHandler(this, &EditTemplateBindingWindow::OnButtonClick));
        }

        pBtn = FindElem<suic::Button>(_U("OkBtn"));
        if (NULL != pBtn)
        {
            pBtn->AddClick(new ClickEventHandler(this, &EditTemplateBindingWindow::OnButtonClick));
        }

        suic::TextBox* targetProp = FindElem<suic::TextBox>(_U("targetProp"));
        if (NULL != targetProp)
        {
            targetProp->SetText(_dp->GetName());
        }

        if (itemColl->GetCount() > 0)
        {
            pListBox->SetSelectedIndex(0);
        }
    }
}

void EditTemplateBindingWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}

