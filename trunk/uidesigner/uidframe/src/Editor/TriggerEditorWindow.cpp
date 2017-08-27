/////////////////////////////////////////////////////
// TriggerEditorWindow.cpp

#include "stdafx.h"

#include <Main/DesignPanel.h>
#include <Editor/ThemeEditorWindow.h>
#include <Editor/TriggerEditorWindow.h>
#include <Core/StyleNode.h>

#include <Framework/Controls/Rectangle.h>

TriggerEditorWindow::TriggerEditorWindow(SetterCollectionNode* setterColl, TriggerCollectionNode* trgs, ThemeEditorWindow* owner, eTriggerType trgType)
    : SetterEditorWindow(owner)
{
    _setterColl = setterColl;
    _triggers = trgs;
    _prevSetterEditor = NULL;
    _propTreeView = NULL;
    _trigger = NULL;

    _addProp = NULL;
    _delProp = NULL;
    _propCmb = NULL;

    _tempRootItem = NULL;

    _triggerType = trgType;
    _cond.fromTrigger = true;
}   

TriggerEditorWindow::~TriggerEditorWindow()
{
}

void TriggerEditorWindow::SetSetterColl(SetterCollectionNode* setterColl)
{
    _setterColl = setterColl;
    UpdateSelectedPropShow();
}

void TriggerEditorWindow::SetTriggerCollectionNode(TemplateRootItem* tempRoot, TriggerCollectionNode* trgs)
{
    _tempRootItem = tempRoot;
    _triggers = trgs;

    if (_triggers == NULL && _tempRootItem != NULL)
    {
        _triggers = _tempRootItem->GetTriggerCollection();
        if (NULL == _triggers->GetOwnerType() || 
            _triggers->GetOwnerType() != _tempRootItem->GetTargetType())
        {
            _triggers->SetRTTIOfInfo(_tempRootItem->GetTargetType());
        }
    }
}

TreeView* TriggerEditorWindow::GetTreeView()
{
    if (NULL == _propTreeView)
    {
        _propTreeView = FindElem<suic::TreeView>(_U("SettersTree"));
    }
    return _propTreeView;
}

void TriggerEditorWindow::OnEditStylePropClick(DpObject* sender, RoutedEventArg* e)
{
    if (NULL != _setterColl)
    {
        // 这里是Style中Trigger属性属性
        PropEditorWindow* propWnd = _themeWnd->ShowStylePropEditorWindow();

        propWnd->GetQueryDpCond()->inTemplate = false;
        propWnd->GetQueryDpCond()->fromTrigger = false;
        // Trigger里暂时不允许编辑模版
        propWnd->GetQueryDpCond()->filterTemp = false;
  
        propWnd->UpdateSetterColl(NULL, _setterColl);
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
            ItemCollection* itemColl = _triggers->GetChildren();
            pTree->SetItemsSource(itemColl);

            if (itemColl->GetCount() > 0)
            {
                pTree->SetSelectedItem(itemColl->GetItem(0));
            }
        }
    }
}

void TriggerEditorWindow::OnDbClickTree(Element* sender, MouseButtonEventArg* e)
{
    if (e->GetMouseButton() != suic::MouseButton::mbLeftButton)
    {
        return;
    }

    Element* clickElem = RTTICast<Element>(e->GetOriginalSource());
    if (NULL != clickElem)
    {
        Object* item = clickElem->GetDataContext();
        TriggerNode* trgNode = RTTICast<TriggerNode>(item);
        if (NULL != trgNode)
        {
            SetterCollectionNode* setterColl = trgNode->GetSetterCollection();
            PropEditorWindow* propWnd = _themeWnd->ShowTemplatePropEditorWindow();

            // Trigger来自于Style
            if (NULL != _setterColl)
            {
                propWnd->GetQueryDpCond()->inTemplate = false;
                propWnd->SetPropertyType(ePropertyType::ePropStyleTrigger);
            }
            else
            {
                // 来自于模版中的Trigger
                propWnd->GetQueryDpCond()->inTemplate = true;
                propWnd->SetPropertyType(ePropertyType::ePropTemplateTrigger);
            }

            // Trigger里暂时不允许编辑模版
            propWnd->GetQueryDpCond()->filterTemp = true;
            propWnd->GetQueryDpCond()->trgFilter = trgNode->GetContSetterCollection()->GetItemColl();

            propWnd->UpdateSetterColl(_tempRootItem, setterColl);
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
        pAddTrigger->AddClick(new ClickEventHandler(this, &TriggerEditorWindow::OnAddTriggerButton));
    }

    //-----------------------------------------------------------------------

    suic::Button* pOpRes = FindElem<suic::Button>(_U("DelTriggerBtn"));
    if (NULL != pOpRes)
    {
        pOpRes->AddClick(new ClickEventHandler(this, &TriggerEditorWindow::OnOpTriggerButton));
    }

    pOpRes = FindElem<suic::Button>(_U("UpTriggerBtn"));
    if (NULL != pOpRes)
    {
        pOpRes->AddClick(new ClickEventHandler(this, &TriggerEditorWindow::OnOpTriggerButton));
    }

    pOpRes = FindElem<suic::Button>(_U("DownTriggerBtn"));
    if (NULL != pOpRes)
    {
        pOpRes->AddClick(new ClickEventHandler(this, &TriggerEditorWindow::OnOpTriggerButton));
    }

    //---------------------------------------------------------------------

    _addProp = FindElem<suic::Button>(_U("AddProp"));
    if (NULL != _addProp)
    {
        _addProp->AddClick(new ClickEventHandler(this, &TriggerEditorWindow::OnAddPropButton));
    }

    _delProp = FindElem<suic::Button>(_U("DelProp"));
    if (NULL != _delProp)
    {
        _delProp->AddClick(new ClickEventHandler(this, &TriggerEditorWindow::OnDeletePropButton));
    }

    GetTreeView()->AddPreviewMouseDoubleClick(new MouseButtonEventHandler(this, &TriggerEditorWindow::OnDbClickTree));
    GetTreeView()->AddSelectedItemChanged(new RoutedPropChangedEventHandler(this, &TriggerEditorWindow::OnSelTriggerChanged));

    
    _propCmb = FindElem<ComboBox>(_U("propName"));
    _propCmb->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &TriggerEditorWindow::OnSelTriggerItemChanged));
}

void TriggerEditorWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}

void TriggerEditorWindow::UpdateSelectedPropShow()
{
    if (NULL != _propTreeView)
    {
        Object* selObj = _propTreeView->GetSelectedItem();
        SetterNode* pSetter = NULL;

        if (NULL == selObj && _propTreeView->GetCount() > 0)
        {
            _propTreeView->SetSelectedItem(_propTreeView->GetItem(0));
            selObj = _propTreeView->GetSelectedItem();
        }

        pSetter = RTTICast<SetterNode>(selObj);

        if (pSetter != NULL)
        {
            ShowSetterNode(pSetter, _setterColl, GetPropertyType());
        }
    }
}

void TriggerEditorWindow::OnIsVisibleChanged(suic::IsVisibleChangedEventArg* e)
{
}

//======================================================
//

void TriggerEditorWindow::SelectToSetterNode(SetterNode* pSetterNode)
{
}

SetterNode* TriggerEditorWindow::HandlePropItem(SetterEditor* setterEditor, DpItem* dpItem, SetterNode* pSetter)
{
    SetterNode* pSetterNode = pSetter;
    suic::RTTIOfInfo* ownerRtti = _triggers->GetOwnerType();
    DpProperty* dp = dpItem->GetDp(ownerRtti, true);

    if (NULL == pSetter && NULL != _trigger)
    {
        _trigger->SetRTTIOfInfo(ownerRtti);
    }

    // 编辑MultiTrigger
    if (NULL != _trigger && NULL == pSetterNode)
    {
        suic::ObjectPtr defVal = suic::DpProperty::UnsetValue();
        suic::PropMetadata* meta = dp->GetMetadata(ownerRtti);
        defVal = meta->GetDefaultValue();
        pSetterNode = new SetterNode(dpItem->name, dp, defVal.get());
    }

    return pSetterNode;
}

void TriggerEditorWindow::OnOpTriggerButton(Element* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
    String strName = sender->GetName();
    suic::TreeView* pTreeView = GetTreeView();
    Object* selObj = pTreeView->GetSelectedItem();
    ItemCollection* itemsColl = _triggers->GetChildren();
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

    _themeWnd->SetModified();
    pTreeView->ScrollIntoView(selObj, false);
    pTreeView->UpdateLayout();
}

ePropertyType TriggerEditorWindow::GetPropertyType()
{
    switch (_triggerType)
    {
    case eTriggerType::eTrgStyle:
        return ePropertyType::ePropStyleTrigger;

    case eTriggerType::eTrgTemplate:
        return ePropertyType::ePropTemplateTrigger;

    default:
        return ePropertyType::ePropStyle;
    }
}

void TriggerEditorWindow::OnAddTriggerButton(Element* sender, RoutedEventArg* e)
{
    PropSelectorWindow* propWnd = new PropSelectorWindow();
    propWnd->ref();

    _cond.target = GetOwnerRTTIInfo();
    propWnd->SetQueryDpCond(_cond);

    if (OpenPropSelectorWindow(sender, propWnd, PlacementMode::pmRight))
    {
        DpItem* dpItem = propWnd->GetSelectedItem();
        if (NULL != dpItem)
        {
            _trigger = new TriggerNode();
            _trigger->ref();

            SetterNode* pSetter = NULL;
            suic::TextBox* targetName = FindElem<suic::TextBox>(_U("targetName"));
            targetName->SetText(_triggers->GetOwnerType()->typeName);
            _trigger->SetRTTIOfInfo(_triggers->GetOwnerType());

            _triggers->AddItem(_trigger);

            pSetter = SwitchToProperty(_triggers->GetOwnerType(), dpItem, NULL, _U(""), GetPropertyType());

            _trigger->AddSetterNode(pSetter);

            _propCmb->SetItemsSource(_trigger->GetContSetterCollection()->GetItemColl());
            _propCmb->SetSelectedIndex(0);

            _themeWnd->SetModified();

            if (_trigger->GetCount() <= 1)
            {
                _delProp->Enable(false);
            }

            _trigger->unref();
        }
    }

    propWnd->unref();
    e->SetHandled(true);
}

void TriggerEditorWindow::OnAddPropButton(Element* sender, RoutedEventArg* e)
{
    if (_propCmb->GetItemsSource()->GetCount() > 0 && NULL != _trigger)
    {
        PropSelectorWindow* propWnd = new PropSelectorWindow();

        propWnd->ref();
        _cond.target = _triggers->GetOwnerType();
        propWnd->SetQueryDpCond(_cond);

        if (OpenPropSelectorWindow(sender, propWnd, PlacementMode::pmBottom))
        {
            SetterNode* pSetter = NULL;
            DpItem* dpItem = propWnd->GetSelectedItem();

            if (NULL != dpItem)
            {
                pSetter = SwitchToProperty(_triggers->GetOwnerType(), dpItem, NULL, _U(""), GetPropertyType());
                _trigger->AddSetterNode(pSetter);
                _propCmb->SetSelectedIndex(_propCmb->GetCount() - 1);
                _trigger->UpdateDisplayName();

                _themeWnd->SetModified();

                if (_trigger->GetCount() > 1)
                {
                    _delProp->Enable(true);
                }
            }
        }

        propWnd->unref();
    }

    e->SetHandled(true);
}

void TriggerEditorWindow::OnDeletePropButton(Element* sender, RoutedEventArg* e)
{
    TriggerNode* trigger = _trigger;
    if (trigger != NULL && trigger->GetCount() > 1)
    {
        int iSelIndex = _propCmb->GetSelectedIndex();
        SetterNode* selSetter = RTTICast<SetterNode>(_propCmb->GetSelectedItem());

        trigger->RemoveTrigger(selSetter->GetName());
        _trigger->UpdateDisplayName();

        if (trigger->GetCount() <= 1)
        {
            _delProp->Enable(false);
        }

        _propCmb->SetSelectedIndex(0);

        _themeWnd->SetModified();
        UpdateLayout();
    }

    e->SetHandled(true);
}

void TriggerEditorWindow::OnSelTriggerChanged(Element* sender, RoutedPropChangedEventArg* e)
{
    e->SetHandled(true);

    SetterCollectionNode* setterColl = NULL;
    Object* selObj = e->GetNewValue();
    TriggerNode* trgNode = RTTICast<TriggerNode>(selObj);

    if (NULL != trgNode)
    {
        _trigger = trgNode;
        setterColl = _trigger->GetContSetterCollection();

        if (setterColl->GetItemColl()->GetCount() > 1)
        {
            _delProp->Enable(true);
        }
        else
        {
            _delProp->Enable(false);
        }

        _propCmb->SetItemsSource(setterColl->GetItemColl());
        _propCmb->SetSelectedIndex(0);
    }
}

void TriggerEditorWindow::OnSelTriggerItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e)
{
    e->SetHandled(true);
    TriggerNode* trigger = _trigger;
    if (NULL != trigger)
    {
        Object* selItem = _propCmb->GetSelectedItem();
        SetterNode* pSetterNode = RTTICast<SetterNode>(selItem);

        if (NULL != pSetterNode)
        {
            ShowTriggerSetterNode(pSetterNode);
        }
        else
        {
            HandlePrevSetterNode();
        }
    }
}

suic::RTTIOfInfo* TriggerEditorWindow::GetOwnerRTTIInfo()
{
    suic::RTTIOfInfo* ownerRtti = _triggers->GetOwnerType();
    if (NULL == ownerRtti && _tempRootItem != NULL)
    {
        ownerRtti = _tempRootItem->GetTargetType();
    }
    return ownerRtti;
}

void TriggerEditorWindow::ShowTriggerSetterNode(SetterNode* resNode)
{
    if (NULL != resNode)
    {
        suic::RTTIOfInfo* ownerRtti = GetOwnerRTTIInfo();
        suic::TextBox* targetName = FindElem<suic::TextBox>(_U("targetName"));
        DpItem* dpItem = DpManager::Ins()->FindDpItem(resNode->GetName());

        if (NULL != ownerRtti)
        {
            targetName->SetText(ownerRtti->typeName);
        }

        SwitchToProperty(ownerRtti, dpItem, resNode, _U(""), GetPropertyType());
    }
}