/////////////////////////////////////////////////////
// BindingEditorWindow.cpp

#include "stdafx.h"

#include <Main/DesignPanel.h>

#include <System/Resources/StaticResource.h>
#include <System/Resources/DynamicResource.h>
#include <Editor/BindingEditorWindow.h>

BindingEditorWindow::BindingEditorWindow(DesignElement* target, DpProperty* dp, ResourceDictionaryNode* resDicNode, ResNode* resNode)
{
    _targetDp = dp;
    _target = target;

    _resNode = resNode;
    _resDicNode = resDicNode;
    
    _prevEditCtrl = NULL;

    _tempParent = NULL;
    _binding = NULL;
    _templateBinding = NULL;
    _staticResource = NULL;
    _dynamicResource = NULL;

    if (NULL != _resNode)
    {
        _resNode->ref();
    }

}

BindingEditorWindow::~BindingEditorWindow()
{
    if (NULL != _resNode)
    {
        _resNode->unref();
    }
}

ResNode* BindingEditorWindow::GetResNode() const
{
    return _resNode;
}

void BindingEditorWindow::SetTemplateParent(TemplateRootItem* tempParent)
{
    _tempParent = tempParent;
}

void BindingEditorWindow::SetResourceElement(DesignElement* target)
{
    _resTarget = target;
}

void BindingEditorWindow::ResetStaticResource(suic::ListBox* pListBox)
{
    if (pListBox->GetCount() > 0)
    {
        pListBox->SetSelectedIndex(0);
    }
    _staticResource->SetChecked(Boolean::True);
}

void BindingEditorWindow::InitBindingValue()
{
    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("propList"));
    suic::Extension* pExt = NULL;

    if (NULL != _resNode)
    {
        pExt = RTTICast<suic::Extension>(_resNode->GetValue());
    }

    if (NULL == pExt)
    {
        ResetStaticResource(pListBox);
    }
    else
    {
        if (pExt->GetRTTIType()->InheritFrom(DRExtension::RTTIType()))
        {
            _dynamicResource->SetChecked(Boolean::True);
            _dynamicText->SetText(((DRExtension*)pExt)->GetResourceKey());
        }
        else if (pExt->GetRTTIType()->InheritFrom(TemplateBindingExtension::RTTIType()))
        {
            _templateBinding->SetChecked(Boolean::True);

            bool bFindRes = false;
            TemplateBindingExtension* tempExt = (TemplateBindingExtension*)pExt;
            suic::ListBox* pTempList = FindElem<suic::ListBox>(_U("templateList"));

            for (int i = 0; i < pTempList->GetCount(); ++i)
            {
                DpItem* pDpItem = RTTICast<DpItem>(pTempList->GetItem(i));
                DpProperty* dp = pDpItem->GetDp(_tempParent->GetTargetType(), true);

                if (NULL != dp && dp == tempExt->GetProp())
                {
                    pTempList->SetSelectedIndex(i);
                    bFindRes = true;
                    break;
                }
            }
        }
        else if (pExt->GetRTTIType()->InheritFrom(Binding::RTTIType()))
        {
            _binding->SetChecked(Boolean::True);
            Binding* pBinding = (Binding*)pExt;
            switch (pBinding->GetBindMode())
            {
            case BindingMode::OneTime:
                FindElem<RadioButton>(_U("OneTime"))->SetChecked(suic::Boolean::True);
                break;

            case BindingMode::OneWay:
                FindElem<RadioButton>(_U("OneWay"))->SetChecked(suic::Boolean::True);
                break;

            case BindingMode::TwoWay:
                FindElem<RadioButton>(_U("TwoWay"))->SetChecked(suic::Boolean::True);
                break;

            case BindingMode::OneWayToSource:
                FindElem<RadioButton>(_U("OneWayToSource"))->SetChecked(suic::Boolean::True);
                break;
            }

            _pathText->SetText(pBinding->GetPath().Path);
        }
        else if (pExt->GetRTTIType()->InheritFrom(SRExtension::RTTIType()))
        {
            _staticResource->SetChecked(Boolean::True);

            bool bFindRes = false;
            String strKey = ((SRExtension*)pExt)->GetResourceKey();

            for (int i = 0; i < pListBox->GetCount(); ++i)
            {
                ResNode* resNode = RTTICast<ResNode>(pListBox->GetItem(i));
                if (NULL != resNode && resNode->GetKey().Equals(strKey))
                {
                    pListBox->SetSelectedIndex(i);
                    bFindRes = true;
                    break;
                }
            }

            if (!bFindRes)
            {
                ResetStaticResource(pListBox);
            }
        }
        else
        {
            ResetStaticResource(pListBox);
        }
    }
}

bool BindingEditorWindow::UpdateBindingValue()
{
    bool bOk = true;
    String errMsg;
    ResNodePtr newRes;

    if (_staticResource->IsChecked())
    {
        suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("propList"));
        ResNode* pResNode = RTTICast<ResNode>(pListBox->GetSelectedItem());
        if (NULL != pResNode)
        {
            ResNode::CreateResNode(new SRExtension(pResNode->GetKey()), newRes);
        }
        else
        {
            bOk = false;
            errMsg = _U("请选择需要静态资源（StaticResource）！");
        }
    }
    else if (_dynamicResource->IsChecked())
    {
        String strKey = _dynamicText->GetText();

        strKey.Trim();
        if (!strKey.Empty())
        {
            ResNode::CreateResNode(new DRExtension(strKey), newRes);
        }
        else
        {
            bOk = false;
            errMsg = _U("动态资源名称不合法，请重新输入！");
        }
    }
    else if (_binding->IsChecked())
    {
        String strPath = _pathText->GetText();
        suic::RadioButton* pMode = NULL;

        if (!strPath.Empty())
        {
            strPath.Trim();
            suic::Binding* pBind = new suic::Binding();
            suic::PropertyPath propPath(strPath);
            pBind->SetPath(propPath);

            pMode = FindElem<RadioButton>(_U("OneTime"));
            if (pMode->IsChecked())
            {
                pBind->SetBindMode(BindingMode::OneTime);
            }
            else 
            {
                pMode = FindElem<RadioButton>(_U("OneWay"));
                if (pMode->IsChecked())
                {
                    pBind->SetBindMode(BindingMode::OneWay);
                }
                else
                {
                    pMode = FindElem<RadioButton>(_U("TwoWay"));
                    if (pMode->IsChecked())
                    {
                        pBind->SetBindMode(BindingMode::TwoWay);
                    }
                    else
                    {
                        pMode = FindElem<RadioButton>(_U("OneWayToSource"));
                        if (pMode->IsChecked())
                        {
                            pBind->SetBindMode(BindingMode::OneWayToSource);
                        }
                    }
                }
            }

            ResNode::CreateResNode(pBind, newRes);
        }
        else
        {
            bOk = false;
            errMsg = _U("Binding路径不合法，请重新输入！");
        }
    }
    else if (_templateBinding->IsChecked())
    {
        DpProperty* dp = NULL;
        suic::ListBox* pTempList = FindElem<suic::ListBox>(_U("templateList"));
        DpItem* pDpItem = DynamicCast<DpItem>(pTempList->GetSelectedItem());

        if (NULL != pDpItem)
        {
            dp = pDpItem->GetDp(_tempParent->GetTargetType(), true);
            
        }

        if (NULL != dp)
        {
            ResNode::CreateResNode(new TemplateBindingExtension(dp), newRes);
        }
        else
        {
            bOk = false;
            errMsg = _U("请选择TemplateBinding属性！");
        }
    }

    if (newRes.get() != NULL)
    {
        SETREFOBJ(_resNode, newRes.get());
    }

    if (!bOk)
    {
        InfoBox::Show(errMsg, _U("错误"));
    }

    return bOk;
}

void BindingEditorWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);

    _binding = FindElem<suic::RadioButton>(_U("Binding"));
    _templateBinding = FindElem<suic::RadioButton>(_U("TemplateBinding"));
    _staticResource = FindElem<suic::RadioButton>(_U("StaticResource"));
    _dynamicResource = FindElem<suic::RadioButton>(_U("DynamicResource"));

    _pathText = FindElem<suic::TextBox>(_U("BindingPath"));
    _dynamicText = FindElem<suic::TextBox>(_U("DynamicResource_TextBox"));

    if (NULL != _templateBinding)
    {
        if (_tempParent == NULL || !_tempParent->GetTemplateType()->InheritFrom(ControlTemplate::RTTIType()))
        {
            _templateBinding->SetVisibility(Visibility::Collapsed);
        }
        else
        {
            _templateBinding->SetVisibility(Visibility::Visible);
        }
    }

    suic::TextBox* oriProp = FindElem<suic::TextBox>(_U("OriProperty"));
    if (NULL != oriProp && NULL != _targetDp)
    {
        oriProp->SetText(_targetDp->GetName());
    }

    suic::Button* pBtn = FindElem<suic::Button>(_U("CancelBtn"));
    if (NULL != pBtn)
    {
        pBtn->AddClick(new ClickEventHandler(this, &BindingEditorWindow::OnButtonClick));
    }

    pBtn = FindElem<suic::Button>(_U("OkBtn"));
    if (NULL != pBtn)
    {
        pBtn->AddClick(new ClickEventHandler(this, &BindingEditorWindow::OnButtonClick));
    }

    InitTemplateBinding();
    InitStaticResource();
    InitBindingValue();
}

void BindingEditorWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("propList"));
    if (NULL != pListBox)
    {
        pListBox->ScrollIntoView(pListBox->GetSelectedItem());
    }

    pListBox = FindElem<suic::ListBox>(_U("templateList"));
    if (NULL != pListBox)
    {
        pListBox->ScrollIntoView(pListBox->GetSelectedItem());
    }
}

void BindingEditorWindow::InitTemplateBinding()
{
    ItemCollection* itemColl = NULL;
    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("templateList"));

    if (_tempParent != NULL && NULL != pListBox)
    {
        itemColl = pListBox->GetItemsSource();
        DpManager::Ins()->FillPropByDp(itemColl, _tempParent->GetTargetType(), _targetDp);
    }
}

void BindingEditorWindow::FilterResources(ItemCollection* itemColl)
{
    if (NULL != _target)
    {
        int i = 0;
        suic::ObservableCollection tmpColl;
        suic::RTTIOfInfo* rttiInfo = _target->GetUIElement()->GetRTTIType();

        if (_targetDp == suic::FrameworkElement::StyleProperty)
        {
            for (i = 0; i < itemColl->GetCount(); ++i)
            {
                StyleNode* pStyle = RTTICast<StyleNode>(itemColl->GetItem(i));
                if (NULL != pStyle && !pStyle->GetKey().Empty() && 
                    rttiInfo->InheritFrom(pStyle->GetTargetType()))
                {
                    tmpColl.AddItem(pStyle);
                }
            }
        }
        else if (_targetDp == suic::Control::TemplateProperty)
        {
            for (i = 0; i < itemColl->GetCount(); ++i)
            {
                TemplateRootItem* pTemplate = RTTICast<TemplateRootItem>(itemColl->GetItem(i));
                if (NULL != pTemplate && !pTemplate->GetKey().Empty() && 
                    rttiInfo->InheritFrom(pTemplate->GetTargetType()))
                {
                    tmpColl.AddItem(pTemplate);
                }
            }
        }
        else
        {
            return;
        }

        itemColl->Clear();

        for (int j = 0; j < tmpColl.GetCount(); ++j)
        {
            itemColl->AddItem(tmpColl.GetItem(j));
        }
    }
}

void BindingEditorWindow::InitStaticResource()
{
    ItemCollection* itemColl = NULL;
    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("propList"));

    if (_targetDp != NULL && NULL != pListBox)
    {
        itemColl = pListBox->GetItemsSource();

        if (_target != NULL)
        {
            DesignElement* currElem = _target;
            if (NULL != _tempParent)
            {
                currElem = _resTarget;
            }

            while (currElem != NULL)
            {
                currElem->GetResourceDictionary()->FillResByResType(itemColl, _targetDp->GetPropType(), _resNode);
                currElem = currElem->GetParentXamlElement();
            }
        }

        if (NULL != _resDicNode)
        {
            _resDicNode->FillResByResType(itemColl, _targetDp->GetPropType(), _resNode);
        }

        FilterResources(itemColl);
        pListBox->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &BindingEditorWindow::OnItemSelectionChanged));
    }
}

void BindingEditorWindow::OnButtonClick(Element* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
    if (sender->GetName().Equals(_U("OkBtn")))
    {
        if (!UpdateBindingValue())
        {
            return;
        }
    }
    
    AsyncClose();
}

void BindingEditorWindow::OnItemSelectionChanged(Element* sender, SelectionChangedEventArg* e)
{
    if (e->GetAddedItems()->GetCount() > 0)
    {
        ResNode* resNode = RTTICast<ResNode>(e->GetAddedItems()->GetItem(0));
        e->SetHandled(true);

        SwitchToResNode(resNode);
    }
}

void BindingEditorWindow::SwitchToResNode(ResNode* resNode)
{
    if (NULL != resNode)
    {
        if (NULL != _prevEditCtrl)
        {
            _prevEditCtrl->SetVisibility(Visibility::Collapsed);
            _prevEditCtrl = NULL;
        }

        if (NULL != resNode)
        {
            String strName;
            Object* val = resNode->GetValue();

            if (RTTICast<FrameworkTemplate>(val) != NULL)
            {
                strName = _U("Template_Res");
            }
            else if (RTTICast<Element>(val) != NULL)
            {
                strName = _U("Layout_Res");
            }
            else if (RTTICast<StyleNode>(resNode) != NULL)
            {
                val = resNode;
                strName = _U("StyleNode_Res");
            }

            if (NULL != val)
            {
                if (strName.Empty())
                {
                    strName.Format(_U("%s_Res"), val->GetRTTIType()->typeName);
                }
            }

            EditorControl* editCtrl = FindElem<EditorControl>(strName);

            if (NULL != editCtrl && NULL != val)
            {
                editCtrl->SetVisibility(Visibility::Visible);
                editCtrl->UpdateResValue(resNode);

                _prevEditCtrl = editCtrl;
            }
        }
    }
}
