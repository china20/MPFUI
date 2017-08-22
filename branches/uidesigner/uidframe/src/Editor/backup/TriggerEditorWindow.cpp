/////////////////////////////////////////////////////
// TriggerEditorWindow.cpp

#include "stdafx.h"

#include <Main/DesignPanel.h>
#include <Editor/ThemeEditorWindow.h>
#include <Editor/TriggerEditorWindow.h>
#include <Core/StyleNode.h>

#include <Framework/Controls/Rectangle.h>

TriggerEditorWindow::TriggerEditorWindow(SetterCollectionNode* setterColl, TriggerCollectionNode* trgs, ThemeEditorWindow* owner)
{
    _setterColl = setterColl;
    _themeWnd = owner;
    _triggers = trgs;
    _prevSetterEditor = NULL;
    _propTreeView = NULL;
    _currentTrigger = NULL;

    _addProp = NULL;
    _delProp = NULL;
    _propCmb = NULL;
}   

TriggerEditorWindow::~TriggerEditorWindow()
{
}

void TriggerEditorWindow::SetSetterColl(SetterCollectionNode* setterColl)
{
    _setterColl = setterColl;
}

void TriggerEditorWindow::SetTriggerCollectionNode(TriggerCollectionNode* trgs)
{
    _triggers = trgs;
}

TreeView* TriggerEditorWindow::GetTreeView()
{
    if (NULL == _propTreeView)
    {
        _propTreeView = FindElem<suic::TreeView>(_U("SettersTree"));
    }
    return _propTreeView;
}

void TriggerEditorWindow::OnEditStylePropClick(Element* sender, RoutedEventArg* e)
{
    if (NULL != _setterColl)
    {
        _themeWnd->ShowStylePropEditorWindow(_setterColl);
    }
}

void TriggerEditorWindow::Reflesh()
{
    Element* editPanel = FindName(_U("EditStyleGrid"));

    if (NULL != _setterColl)
    {
        editPanel->SetVisibility(Visibility::Visible);
    }
    else
    {
        editPanel->SetVisibility(Visibility::Collapsed);
    }

    if (_triggers != NULL)
    {
        TreeView* pTree = GetTreeView();
        if (NULL != pTree)
        {
            ItemCollection* itemColl = RTTICast<ItemCollection>(_triggers->GetItemsSource());
            pTree->SetItemsSource(itemColl);
        }
    }
}

void TriggerEditorWindow::OnDbClickTree(Element* sender, MouseButtonEventArg* e)
{
    Element* clickElem = RTTICast<Element>(e->GetOriginalSource());
    if (NULL != clickElem)
    {
        Object* item = clickElem->GetDataContext();
        TriggerBaseNode* trgNode = RTTICast<TriggerBaseNode>(item);
        if (NULL != trgNode)
        {
            SetterCollectionNode* setterColl = trgNode->GetSetterCollection();
            _themeWnd->ShowTemplatePropEditorWindow(setterColl);
        }
    }
}

void TriggerEditorWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
    suic::TextBlock* pLink = FindElem<suic::TextBlock>(_U("EditStyleProp"));
    if (NULL != pLink)
    {
        pLink->AddHandler(suic::Hyperlink::ClickEvent, new RoutedEventHandler(this, &TriggerEditorWindow::OnEditStylePropClick));
    }

    suic::Button* pAddTrigger = FindElem<suic::Button>(_U("AddTriggerBtn"));
    if (NULL != pAddTrigger)
    {
        pAddTrigger->AddClick(new RoutedEventHandler(this, &TriggerEditorWindow::OnAddTriggerButton));
    }

    suic::Button* pAddMultiTrigger = FindElem<suic::Button>(_U("AddMultiTriggerBtn"));
    if (NULL != pAddMultiTrigger)
    {
        pAddMultiTrigger->AddClick(new RoutedEventHandler(this, &TriggerEditorWindow::OnAddMultiTriggerButton));
    }


    //-----------------------------------------------------------------------

    suic::Button* pOpRes = FindElem<suic::Button>(_U("DelTriggerBtn"));
    if (NULL != pOpRes)
    {
        pOpRes->AddClick(new RoutedEventHandler(this, &TriggerEditorWindow::OnOpTriggerButton));
    }

    pOpRes = FindElem<suic::Button>(_U("UpTriggerBtn"));
    if (NULL != pOpRes)
    {
        pOpRes->AddClick(new RoutedEventHandler(this, &TriggerEditorWindow::OnOpTriggerButton));
    }

    pOpRes = FindElem<suic::Button>(_U("DownTriggerBtn"));
    if (NULL != pOpRes)
    {
        pOpRes->AddClick(new RoutedEventHandler(this, &TriggerEditorWindow::OnOpTriggerButton));
    }

    //---------------------------------------------------------------------

    _addProp = FindElem<suic::Button>(_U("AddProp"));
    if (NULL != _addProp)
    {
        _addProp->AddClick(new RoutedEventHandler(this, &TriggerEditorWindow::OnAddPropButton));
    }

    _delProp = FindElem<suic::Button>(_U("DelProp"));
    if (NULL != _delProp)
    {
        _delProp->AddClick(new RoutedEventHandler(this, &TriggerEditorWindow::OnDeletePropButton));
    }

    GetTreeView()->AddPreviewMouseDoubleClick(new MouseButtonEventHandler(this, &TriggerEditorWindow::OnDbClickTree));
    _propCmb = FindElem<ComboBox>(_U("propName"));
    _propCmb->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &TriggerEditorWindow::OnSelItemChanged));
}

void TriggerEditorWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}

//======================================================
//

SetterNode* TriggerEditorWindow::HandlePropItem(SetterEditor* setterEditor, DpItem* dpItem, SetterNode* pSetter)
{
    SetterNode* pSetterNode = pSetter;
    suic::RTTIOfInfo* ownerRtti = _triggers->GetOwnerType();
    DpProperty* dp = DpManager::Ins()->FindDp(dpItem->name, ownerRtti);//propItem->MapProperty(ownerRtti);
    MultiTriggerNode* triggerNode = RTTICast<MultiTriggerNode>(_currentTrigger);

    if (NULL == pSetter)
    {
        _currentTrigger->SetRTTIOfInfo(ownerRtti);
    }

    // ±à¼­MultiTrigger
    if (NULL != triggerNode)
    {
        _delProp->Enable(false);
        _addProp->Enable(true);

        if (NULL == pSetterNode)
        {
            pSetterNode = new SetterNode(dpItem->name, dp, NULL);
            triggerNode->AddSetterNode(pSetterNode);

            if (triggerNode->GetCount() == 1)
            {
                _triggers->AddItem(triggerNode);
            }
        }

        if (triggerNode->GetCount() > 1)
        {
            _delProp->Enable(true);
        }
    }
    else
    {
        _delProp->Enable(false);
        _addProp->Enable(false);

        TriggerNode* trgNode = RTTICast<TriggerNode>(_currentTrigger);

        if (NULL == pSetterNode)
        {
            _triggers->AddItem(_currentTrigger);
            pSetterNode = new SetterNode(dpItem->name, dp, NULL);
            trgNode->SetSetterNode(pSetterNode);
        }
    }

    return pSetterNode;
}

void TriggerEditorWindow::OnOpTriggerButton(Element* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
    String strName = sender->GetName();
    suic::TreeView* pTreeView = GetTreeView();
    Object* selObj = pTreeView->GetSelectedItem();
    ItemCollection* itemsColl = RTTICast<ItemCollection>(_triggers->GetItemsSource());
    int selIndex = itemsColl->IndexOf(selObj);

    if (strName.Equals(_U("DelTriggerBtn")))
    {
        itemsColl->RemoveItem(selObj);
        if (selIndex >= itemsColl->GetCount())
        {
            selIndex = itemsColl->GetCount() - 1;
        }
        
        if (selIndex >= 0)
        {
            selObj = itemsColl->GetItem(selIndex);
            pTreeView->SetSelectedItem(selObj);
        }
    }
    else if (strName.Equals(_U("UpTriggerBtn")))
    {
        if (selIndex > 0)
        {
            itemsColl->SwapItem(selIndex, selIndex - 1);
        }
    }
    else if (strName.Equals(_U("DownTriggerBtn")))
    {
        if (selIndex < itemsColl->GetCount() - 1)
        {
            itemsColl->SwapItem(selIndex, selIndex + 1);
        }
    }

    pTreeView->ScrollIntoView(selObj, false);
    pTreeView->UpdateLayout();
}

void TriggerEditorWindow::OnAddTriggerButton(Element* sender, RoutedEventArg* e)
{
    PropSelectorWindow* propWnd = new PropSelectorWindow(NULL, _triggers->GetOwnerType());
    propWnd->ref();

    if (OpenPropSelectorWindow(sender, propWnd, PlacementMode::pmRight))
    {
        DpItem* dpItem = propWnd->GetSelectedItem();
        if (NULL != dpItem)
        {
            _currentTrigger = new TriggerNode();
            _currentTrigger->ref();
            _propCmb->GetItemsSource()->Clear();

            SwitchToProperty(dpItem, NULL);

            _propCmb->AddChild(new OString(dpItem->name));
            _propCmb->SetSelectedIndex(0);
            _currentTrigger->unref();
        }
    }

    propWnd->unref();
    e->SetHandled(true);
}

void TriggerEditorWindow::OnAddMultiTriggerButton(Element* sender, RoutedEventArg* e)
{
    PropSelectorWindow* propWnd = new PropSelectorWindow(NULL, _triggers->GetOwnerType());
    propWnd->ref();

    if (OpenPropSelectorWindow(sender, propWnd, PlacementMode::pmRight))
    {
        DpItem* dpItem = propWnd->GetSelectedItem();
        if (NULL != dpItem)
        {
            _currentTrigger = new MultiTriggerNode();
            _currentTrigger->ref();
            _propCmb->GetItemsSource()->Clear();

            SwitchToProperty(dpItem, NULL);

            _propCmb->AddChild(new OString(dpItem->name));
            _propCmb->SetSelectedIndex(0);
            _currentTrigger->unref();
        }
    }

    propWnd->unref();

    e->SetHandled(true);
}

void TriggerEditorWindow::OnAddPropButton(Element* sender, RoutedEventArg* e)
{
    if (_propCmb->GetItemsSource()->GetCount() > 0 && NULL != _currentTrigger)
    {
        PropSelectorWindow* propWnd = new PropSelectorWindow(NULL, _triggers->GetOwnerType());
        propWnd->ref();

        if (OpenPropSelectorWindow(sender, propWnd, PlacementMode::pmBottom))
        {
            DpItem* dpItem = propWnd->GetSelectedItem();
            if (NULL != dpItem)
            {
                SwitchToProperty(dpItem, NULL);
                _propCmb->AddText(dpItem->name);
                _propCmb->SetSelectedIndex(_propCmb->GetCount() - 1);
            }
        }

        propWnd->unref();
    }

    e->SetHandled(true);
}

void TriggerEditorWindow::OnDeletePropButton(Element* sender, RoutedEventArg* e)
{
    MultiTriggerNode* trigger = RTTICast<MultiTriggerNode>(_currentTrigger);
    if (trigger != NULL && trigger->GetCount() > 1)
    {
        String name = _propCmb->GetText();
        int iSelIndex = _propCmb->GetSelectedIndex();
        trigger->RemoveTrigger(name);
        _propCmb->GetItemsSource()->RemoveByIndex(iSelIndex);
        if (trigger->GetCount() <= 1)
        {
            _delProp->Enable(false);
        }
        _propCmb->SetSelectedIndex(0);
        UpdateLayout();
        
    }

    e->SetHandled(true);
}

void TriggerEditorWindow::OnSelItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e)
{
    e->SetHandled(true);
    MultiTriggerNode* trigger = RTTICast<MultiTriggerNode>(_currentTrigger);
    if (NULL != trigger)
    {
        SetterNode* pSetterNode = trigger->FindTrigger(_propCmb->GetText());
        if (NULL != pSetterNode)
        {
            suic::RTTIOfInfo* ownerRtti = _triggers->GetOwnerType();
            DpItem* dpItem = DpManager::Ins()->FindDpItem(pSetterNode->GetName());
            SwitchToProperty(dpItem, pSetterNode);
        }
    }
}
