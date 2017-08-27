/////////////////////////////////////////////////////
// ControlAssetsWindow.cpp

#include "stdafx.h"

#include <Main/DesignPanel.h>
#include <Core/TypeManager.h>
#include <Core/StyleNode.h>

#include <Editor/ControlAssetsWindow.h>

#include <Framework/Controls/Rectangle.h>
#include <Framework/Controls/UniformGrid.h>

ControlAssetsWindow::ControlAssetsWindow(IControlWindow* ctrlWnd)
{
    _controlWnd = ctrlWnd;
}

ControlAssetsWindow::~ControlAssetsWindow()
{
}

void ControlAssetsWindow::RefleshControls()
{
    TreeView* pTree = FindElem<TreeView>(_U("CtrlsType"));
    ListBox* pList = FindElem<ListBox>(_U("Controls"));
    if (NULL != pList)
    {
        Element* selItem = RTTICast<Element>(pTree->GetSelectedItem());
        if (NULL == selItem)
        {
            TypePool::GetTypeItems(pList->GetItemsSource());
        }
        else
        {
            TypePool::GetTypeItems(pList->GetItemsSource(), selItem->GetName());
        }
    }
}

void ControlAssetsWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);

    TreeView* pTree = FindElem<TreeView>(_U("CtrlsType"));
    if (NULL != pTree)
    {
        pTree->AddSelectedItemChanged(new suic::RoutedPropChangedEventHandler(this, &ControlAssetsWindow::OnSelectedItemChanged));
    }

    ListBox* pList = FindElem<ListBox>(_U("Controls"));
    if (NULL != pList)
    {
        pList->AddSelectionChanged(new SelectionChangedEventHandler(this, &ControlAssetsWindow::OnSelControlsChanged));
        pList->AddPreviewMouseDoubleClick(new MouseButtonEventHandler(this, &ControlAssetsWindow::OnPreviewMouseDoubleClick));
    }

    RefleshControls();
}

void ControlAssetsWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}

void ControlAssetsWindow::OnPreviewMouseDoubleClick(Element* sender, MouseButtonEventArg* e)
{
    e->SetHandled(true);

    if (e->GetMouseButton() != suic::MouseButton::mbLeftButton)
    {
        return;
    }

    ListBox* pList = FindElem<ListBox>(_U("Controls"));
    if (NULL != pList && NULL != _controlWnd)
    {
        String strName = pList->GetSelectedItem()->ToString();
        _controlWnd->UpdateMoreElement(strName, true);
    }

    AsyncClose();
}

void ControlAssetsWindow::OnSelControlsChanged(Element* sender, SelectionChangedEventArg* e)
{
    e->SetHandled(true);
}

void ControlAssetsWindow::OnSelectedItemChanged(suic::Element* sender, suic::RoutedPropChangedEventArg* e)
{
    e->SetHandled(true);
    RefleshControls();
}