/////////////////////////////////////////////////////
// ResSelectorWindow.cpp

#include "stdafx.h"

#include <Main/DesignPanel.h>
#include <Editor/ResSelectorWindow.h>
#include <Core/StyleNode.h>

#include <Framework/Controls/Rectangle.h>
#include <Framework/Controls/UniformGrid.h>

ResSelectorWindow::ResSelectorWindow()
{
    _isDbClicked = false;
}

ResSelectorWindow::~ResSelectorWindow()
{
}

void ResSelectorWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
    _isDbClicked = false;

    ListBox* pList = FindElem<ListBox>(_U("resList"));
    if (NULL != pList)
    {
        pList->SetItemsSource(ResTypePool::GetTypeItems());
        pList->AddSelectionChanged(new SelectionChangedEventHandler(this, &ResSelectorWindow::OnSelectedChanged));
        pList->AddPreviewMouseDoubleClick(new MouseButtonEventHandler(this, &ResSelectorWindow::OnDbListClick));
    }

    ListBox* pCtrlList = FindElem<ListBox>(_U("ctrlList"));
    if (NULL != pCtrlList)
    {
        TypePool::GetTypeItems(pCtrlList->GetItemsSource());
        pCtrlList->AddPreviewMouseDoubleClick(new MouseButtonEventHandler(this, &ResSelectorWindow::OnCtrlListDbClick));
    }
}

void ResSelectorWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}

void ResSelectorWindow::OnSelectedChanged(suic::Element* sender, suic::SelectionChangedEventArg* e)
{
    if (e->GetAddedItems()->GetCount() == 1)
    {
        e->SetHandled(true);
        ListBox* pList = FindElem<ListBox>(_U("ctrlList"));

        suic::Element* fromType = FindName(_U("fromType"));
        UniformGrid* pPanel = FindElem<UniformGrid>(_U("gridPanel"));

        ResTypeItem* resItem = dynamic_cast<ResTypeItem*>(e->GetAddedItems()->GetItem(0));
        int iTargetType = IsCanClose(resItem);
        if (iTargetType > 0)
        {
            pPanel->SetColumns(2);
            pList->SetVisibility(Visibility::Visible);
        }
        else
        {
            pPanel->SetColumns(1);
            pList->SetVisibility(Visibility::Collapsed);
        }

        // Style»òControlTemplate
        if (iTargetType == 1)
        {
            pPanel->SetMargin(suic::Rect(5, 5, 0, 27));
            fromType->SetVisibility(suic::Visibility::Visible);
        }
        else
        {
            pPanel->SetMargin(suic::Rect(5, 5, 0, 0));
            fromType->SetVisibility(suic::Visibility::Collapsed);
        }

        pPanel->GetParent()->UpdateLayout();
    }
}

void ResSelectorWindow::OnDbListClick(Element* sender, MouseButtonEventArg* e)
{
    if (e->GetMouseButton() != suic::MouseButton::mbLeftButton)
    {
        return;
    }

    e->SetHandled(true);

    if (IsCanClose(GetCurrentSelItem()) <= 0)
    {
        _isDbClicked = true;
        AsyncClose();
    }
}

void ResSelectorWindow::OnCtrlListDbClick(Element* sender, MouseButtonEventArg* e)
{
    if (e->GetMouseButton() != suic::MouseButton::mbLeftButton)
    {
        return;
    }

    e->SetHandled(true);

    _isDbClicked = true;
    AsyncClose();
}

bool ResSelectorWindow::IsFromTypeChecked()
{
    suic::CheckBox* fromType = FindElem<suic::CheckBox>(_U("fromType"));
    if (NULL != fromType)
    {
        return (fromType->IsChecked());
    }
    else
    {
        return false;
    }
}

ResTypeItem* ResSelectorWindow::GetSelectedItem()
{
    if (_isDbClicked)
    {
        return GetCurrentSelItem();
    }
    else
    {
        return NULL;
    }
}

TypeItem* ResSelectorWindow::GetSelectedControl()
{
    if (_isDbClicked)
    {
        TypeItem* typeItem = NULL;
        ListBox* pList = FindElem<ListBox>(_U("ctrlList"));
        typeItem = dynamic_cast<TypeItem*>(pList->GetSelectedItem());
        return typeItem;
    }
    else
    {
        return NULL;
    }
}

ResTypeItem* ResSelectorWindow::GetCurrentSelItem()
{
    ListBox* pList = FindElem<ListBox>(_U("resList"));
    if (NULL != pList)
    {
        return dynamic_cast<ResTypeItem*>(pList->GetSelectedItem());
    }
    else
    {
        return NULL;
    }
}

int ResSelectorWindow::IsCanClose(ResTypeItem* resItem)
{
    RTTIOfInfo* rttiInfo = resItem->GetType();
    if (rttiInfo->InheritFrom(ControlTemplate::RTTIType()) || 
        rttiInfo->InheritFrom(Style::RTTIType()))
    {
        return 1;
    }
    else if (rttiInfo->InheritFrom(Element::RTTIType()))
    {
        return 2;
    }
    else
    {
        return 0;
    }
}
