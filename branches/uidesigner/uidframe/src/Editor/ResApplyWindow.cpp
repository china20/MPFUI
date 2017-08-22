/////////////////////////////////////////////////////
// ResApplyWindow.cpp

#include "stdafx.h"

#include <Main/DesignPanel.h>
#include <Editor/ResApplyWindow.h>
#include <Core/StyleNode.h>

ResApplyWindow::ResApplyWindow(FileRootItem* rootItem)
{
    _rootItem = rootItem;
    _listBox = NULL;
    _listBoxApply = NULL;
    _resDicNode = NULL;
}

ResApplyWindow::~ResApplyWindow()
{
}

void ResApplyWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
    suic::Array<ResourceDicRootItem*> resRootItems;
    _rootItem->GetProject()->FindAllResourceDicRootItems(resRootItems);

    _listBox = FindElem<ListBox>(_U("listBox"));
    _listBoxApply = FindElem<ListBox>(_U("applyListBox"));

    // 初始化已经应用的资源
    InitAppliedResource();

    if (NULL != _listBox)
    {
        suic::ItemCollection* itemColl = _listBox->GetItemsSource();
        for (int i = 0; i < resRootItems.GetCount(); ++i)
        {
            if (!ExistResourceDicRootItem(resRootItems.GetItem(i)))
            {
                itemColl->AddItem(new ResApplyItem(resRootItems.GetItem(i)));
            }
        }
    }
}

bool ResApplyWindow::ExistResourceDicRootItem(ResourceDicRootItem* dicRootItem)
{
    suic::ItemCollection* itemColl = _listBoxApply->GetItemsSource();
    for (int i = 0; i < itemColl->GetCount(); ++i)
    {
        ResApplyItem* resItem = dynamic_cast<ResApplyItem*>(itemColl->GetItem(i));
        if (resItem->GetResourceDicRootItem() == dicRootItem)
        {
            return true;
        }
    }
    return false;
}

void ResApplyWindow::InitAppliedResource()
{
    ElementRootItem* elemRootItem = suic::RTTICast<ElementRootItem>(_rootItem);
    if (NULL != elemRootItem)
    {
        _resDicNode = elemRootItem->GetRootElement()->GetResourceDictionary();
    }
    else
    {
        ApplicationRootItem* appRootItem = suic::RTTICast<ApplicationRootItem>(_rootItem);
        if (NULL != appRootItem)
        {
            _resDicNode = appRootItem->GetApplicationNode()->GetResourceDictionary();
        }
    }

    if (NULL != _resDicNode)
    {
        MergedDictionariesNode* mergedDic = _resDicNode->GetMergedDictionariesNode();
        if (NULL != mergedDic)
        {
            for (int i = 0; i < mergedDic->GetCount(); ++i)
            {
                ResourceDictionaryNode* resDic = mergedDic->GetResourceDictionaryNode(i);
                if (NULL != resDic->GetSourceResource())
                {
                    _listBoxApply->AddChild(new ResApplyItem(resDic->GetSourceResource()));
                }
            }
        }
    }
}

void ResApplyWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}

void ResApplyWindow::OnApplyButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::Object* selObj = _listBox->GetSelectedItem();
    if (selObj != NULL && selObj != suic::DpProperty::UnsetValue())
    {
        _listBoxApply->AddChild(selObj);
        _listBox->GetItemsSource()->RemoveItem(selObj);
    }
}

void ResApplyWindow::OnRemoveButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::Object* selObj = _listBoxApply->GetSelectedItem();
    if (selObj != NULL && selObj != suic::DpProperty::UnsetValue())
    {
        _listBox->AddChild(selObj);
        _listBoxApply->GetItemsSource()->RemoveItem(selObj);
    }
}

void ResApplyWindow::OnUpMoveButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e)
{
}

void ResApplyWindow::OnDownMoveButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e)
{
}

void ResApplyWindow::OnOkButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    MergedDictionariesNode* mergedDic = _resDicNode->GetMergedDictionariesNode();
    if (NULL != mergedDic)
    {
        mergedDic->Clear();
    }
    suic::ItemCollection* itemColl = _listBoxApply->GetItemsSource();
    for (int i = 0; i < itemColl->GetCount(); ++i)
    {
        ResApplyItem* resItem = dynamic_cast<ResApplyItem*>(itemColl->GetItem(i));
        if (resItem->GetResourceDicRootItem() != NULL)
        {
            /*if (!resItem->GetResourceDicRootItem()->IsLoaded())
            {
                resItem->GetResourceDicRootItem()->Load(false);
            }*/

            ResourceDictionaryNode* pResNode = new ResourceDictionaryNode();
            pResNode->SetSourceResource(resItem->GetResourceDicRootItem());
            _resDicNode->AddMergedDictionary(pResNode);
        }
    }
    _rootItem->SetModified(true);

    // 从新读取其资源
    suic::String strXml = _resDicNode->GetResXml(_U(""));
    ResourceDicRootItem resRootItem;

    resRootItem.setAutoDelete(false);
    resRootItem.SetProject(_rootItem->GetProject());
    
    XamlLoader xamlLoader;
    suic::Mulstr strData(strXml.c_str());

    if (xamlLoader.LoadResourceDicRootXamlFromMemory(&resRootItem, strData.c_str(), strData.Length()))
    {
        ElementRootItem* elemRootItem = suic::RTTICast<ElementRootItem>(_rootItem);
        if (NULL != elemRootItem)
        {
            elemRootItem->GetRootElement()->SetResourceDictionary(resRootItem.GetResourceDicNode());
        }
        else
        {
            ApplicationRootItem* appRootItem = suic::RTTICast<ApplicationRootItem>(_rootItem);
            if (NULL != appRootItem)
            {
                appRootItem->GetApplicationNode()->SetResourceDictionary(resRootItem.GetResourceDicNode());
            }
        }
    }

    AsyncClose();
}

void ResApplyWindow::OnCancelButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    AsyncClose();
}
