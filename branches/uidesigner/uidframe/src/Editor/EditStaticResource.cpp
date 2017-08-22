/////////////////////////////////////////////////////
// EditStaticResource.cpp

#include "stdafx.h"

#include <Main/DesignPanel.h>
#include <Editor/EditStaticResource.h>

EditStaticResourceWindow::
EditStaticResourceWindow(ResourceDictionaryNode* resDicNode, DesignElement* target, DpProperty* dp, ResNode* resNode)
{
    _resDicNode = resDicNode;
    _resNode = resNode;
    _target = target;
    _dp = dp;
    _prevEditCtrl = NULL;
}

EditStaticResourceWindow::~EditStaticResourceWindow()
{
}

String EditStaticResourceWindow::GetStaticResource()
{
    return _strResourceKey;
}

void EditStaticResourceWindow::OnButtonClick(Element* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
    if (sender->GetName().Equals(_U("OkBtn")))
    {
        suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("propList"));
        if (NULL != pListBox)
        {
            ResNode* pResNode = RTTICast<ResNode>(pListBox->GetSelectedItem());
            if (NULL != pResNode)
            {
                _strResourceKey = pResNode->GetKey();
            }
        }
    }
    else
    {
        _strResourceKey = _U("");
    }

    AsyncClose();
}

void EditStaticResourceWindow::OnItemSelectionChanged(Element* sender, SelectionChangedEventArg* e)
{
    if (e->GetAddedItems()->GetCount() > 0)
    {
        ResNode* resNode = RTTICast<ResNode>(e->GetAddedItems()->GetItem(0));
        e->SetHandled(true);

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
}

void EditStaticResourceWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);

    suic::Button* pBtn = NULL;
    ItemCollection* itemColl = NULL;

    suic::ListBox* pListBox = FindElem<suic::ListBox>(_U("propList"));

    itemColl = pListBox->GetItemsSource();

    if (NULL != _target && _dp != NULL && NULL != pListBox)
    {
        DesignElement* currElem = _target;
        while (currElem != NULL)
        {
            currElem->GetResourceDictionary()->FillResByResType(itemColl, _dp->GetPropType(), _resNode);
            currElem = currElem->GetParentXamlElement();
        }

        pListBox->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &EditStaticResourceWindow::OnItemSelectionChanged));
    }

    if (NULL != _resDicNode && NULL != itemColl && NULL != _dp)
    {
        _resDicNode->FillResByResType(itemColl, _dp->GetPropType(), NULL);
    }

    if (itemColl->GetCount() > 0 && NULL != pListBox)
    {
        pListBox->SetSelectedIndex(0);
    }

    pBtn = FindElem<suic::Button>(_U("CancelBtn"));
    if (NULL != pBtn)
    {
        pBtn->AddClick(new ClickEventHandler(this, &EditStaticResourceWindow::OnButtonClick));
    }

    pBtn = FindElem<suic::Button>(_U("OkBtn"));
    if (NULL != pBtn)
    {
        pBtn->AddClick(new ClickEventHandler(this, &EditStaticResourceWindow::OnButtonClick));
    }
}

void EditStaticResourceWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}

