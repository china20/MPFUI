/////////////////////////////////////////////////////
// ElementTreeManager.cpp

#include "stdafx.h"
#include "DesignPanel.h"
#include "ElementTreeManager.h"

ObjTreeManager::ObjTreeManager()
{
    _project = NULL;
    _elemTree = NULL;
    _designPanel = NULL;
}

ObjTreeManager::~ObjTreeManager()
{
    FREEREFOBJ(_designPanel);
}

void ObjTreeManager::InitElementTree(FrameworkElement* mainWnd)
{
    _elemTree = mainWnd->FindElem<ElementTree>(_U("ObjTreeView"));
    _designPanel = new DesignPanel();
    _designPanel->ref();

    if (NULL == _elemTree)
    {
        return ;
    }

    _elemTree->InitTreeDoc(this);
    _designPanel->SetTreeDoc(this);

    DataTemplate* dataTemp = suic::RTTICast<DataTemplate>(mainWnd->FindRes(_U("ElemItemTemplate")));
    if (NULL != dataTemp)
    {
        _elemTree->SetItemTemplate(dataTemp);
    }

    BindTreeEvent();

    _ctrlWindow.SetSelectNotify(SelectNotifyEventHandler(this, &ObjTreeManager::OnControlNotify));
}

void ObjTreeManager::SetTargetElement(DesignElement* resElem, ElementRootItem* elem)
{
    if (_elemTree == NULL)
    {
        return;
    }

    if (elem == NULL)
    {
        _elemTree->GetItemsSource()->Clear();
        _designPanel->InitEditPanelInfo(elem);
        _designPanel->SwitchRootElement(resElem, elem);
    }
    else if (elem->GetRootElement()->GetUIElement() != NULL && 
        elem != _designPanel->GetRootElement())
    {
        _elemTree->GetItemsSource()->Clear();
        _elemTree->GetItemsSource()->AddItem(elem->GetRootElement());

        elem->SetObjTreeManager(this);

        // 设置默认资源文件名称
        _designPanel->InitEditPanelInfo(elem);
        _designPanel->SwitchRootElement(resElem, elem);

        if (!elem->GetRootElement()->GetUIElement()->IsInitialized())
        {
            elem->GetRootElement()->GetUIElement()->RecursiveInit();
        }
        _elemTree->SetSelectedItem(elem->GetRootElement());
    }

    _designPanel->RefleshDesignPanel();
    _elemTree->UpdateLayout();
}

void ObjTreeManager::UpdateElementTree()
{
    if (_elemTree != NULL)
    {
        _elemTree->UpdateLayout();
    }
}

void ObjTreeManager::SetSelectedElement(Object* elem)
{
    DesignElement* delem = suic::DynamicCast<DesignElement>(elem);
    if (NULL != _elemTree && NULL != delem)
    {
        _elemTree->SetSelectedItem(delem);
        _elemTree->ScrollIntoView(delem, true);
        _elemTree->UpdateLayout();
    }
}

void ObjTreeManager::SetMouseOverElement(Object* elem)
{
    DesignElement* delem = suic::DynamicCast<DesignElement>(elem);
    if (NULL != _elemTree && NULL != delem)
    {
        ElementTreeItem* overElem = suic::DynamicCast<ElementTreeItem>(_elemTree->GetContainerFromItem(elem));
        _elemTree->SetActiveMouseOver(overElem);
    }
    else
    {
        _elemTree->SetActiveMouseOver(NULL);
    }
}

void ObjTreeManager::OnControlNotify(DesignMode mode, const suic::String& name)
{
    _designPanel->SetDesignMode(mode, name);
}

void ObjTreeManager::OnTreeMouseEnter(suic::Element* sender, suic::MouseButtonEventArg* e)
{
    suic::ElementPtr elem = suic::MouseDevice::GetMouseOver();
    DesignElement* overItem = suic::DynamicCast<DesignElement>(elem->GetDataContext());
    if (overItem)
    {
        _designPanel->SetMouseOverElement(overItem, false);
    }
}

void ObjTreeManager::BindTreeEvent()
{
    _elemTree->AddMouseMove(new suic::MouseButtonEventHandler(this, &ObjTreeManager::OnTreeMouseEnter), true);
}
