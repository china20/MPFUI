/////////////////////////////////////////////////////
// TemplateEditorWindow.cpp

#include "stdafx.h"

#include <Main/DesignPanel.h>
#include <Editor/ThemeEditorWindow.h>
#include <Editor/TemplateEditorWindow.h>
#include <Core/StyleNode.h>
#include <Editor/XamlWalker.h>

#include <Framework/Controls/Rectangle.h>

TemplateEditorWindow::TemplateEditorWindow(DesignElement* resElem, ThemeEditorWindow* owner)
{
    _objTree = new ObjTreeManager();
    _themeWnd = owner;
    _objTree->ref();
    _tempElement = NULL;
    _templateParent = NULL;
}

TemplateEditorWindow::~TemplateEditorWindow()
{
    _objTree->unref();
}

void TemplateEditorWindow::OnIsVisibleChanged(IsVisibleChangedEventArg* e)
{
    DesignPanel* designWnd = _objTree->GetDesignPanel();
    if (NULL != designWnd)
    {
        if (IsVisible())
        {
            designWnd->ResetDesignPanel(false);
        }
        else
        {
            designWnd->ResetDesignPanel(true);
        }
    }
}

void TemplateEditorWindow::SetRootTemplateElement(TemplateRootItem* tempNode)
{
    _tempElement = tempNode;

    DesignPanel* designWnd = _objTree->GetDesignPanel();
    suic::Grid* pLinkGrid = FindElem<suic::Grid>(_U("EditTempGrid"));

    if (NULL != tempNode)
    {
        DesignElement* designRoot = tempNode->GetRootElement();
        suic::FrameworkElement* logicElem = designRoot->GetUIElement();

        ResourceDictionaryNode* pResNode =_themeWnd->GetResourceNode();

        if (NULL != pResNode)
        {
            designRoot->GetUIElement()->SetResources(pResNode->GetResourceDictionary());
        }

        // 检测模版是否已经实例
        tempNode->CheckTemplateInstantiate();
        tempNode->SetObjTreeManager(_objTree);

        if (NULL != pLinkGrid)
        {
            if (tempNode->GetTemplateType()->InheritFrom(ControlTemplate::RTTIType()))
            {
                pLinkGrid->SetVisibility(suic::Visibility::Visible);
            }
            else
            {
                pLinkGrid->SetVisibility(suic::Visibility::Collapsed);
            }
        }

        designWnd->ResetDesignPanel(false);
        designWnd->SwitchRootElement(NULL, NULL);

        tempNode->SetFocusElement(NULL);
        tempNode->SetHotElement(NULL);

        tempNode->SetProject(_themeWnd->GetProject());

        // 加载到编辑器
        _objTree->SetTargetElement(_themeWnd->GetResourceElement(), tempNode);
        _objTree->UpdateElementTree();

        logicElem->SetWidth(260);
        logicElem->SetHeight(260);
        logicElem->SetVerticalAlignment(VertAlign::vaStretch);
        logicElem->SetHorizontalAlignment(HoriAlign::haStretch);

        designWnd->UpdateLayout();

        _tempElement->SealInitTemplate();
    }

    //suic::DesignHelper::RegisterTemplateXamlWalker(NULL, 0);
}

void TemplateEditorWindow::OnEditTempPropClick(DpObject* sender, RoutedEventArg* e)
{
    if (NULL != _tempElement)
    {
        _themeWnd->ShowTriggersEditorWindow(_tempElement);
    }
}

void TemplateEditorWindow::InitTreeDoc()
{
    _objTree->InitElementTree(this);

    // 初始化界面编辑面板
    DesignPanel* designWnd = _objTree->GetDesignPanel();
    ScrollViewer* pViewer = FindElem<ScrollViewer>(_U("DesignViewer"));

    if (NULL != pViewer)
    {
        pViewer->SetContent(designWnd);
    }

    suic::Element* ctrlWnd = _themeWnd->FindName(_U("CtrlToolbar"));
    suic::IAddChild* pAddChild = suic::DynamicCast<suic::IAddChild>(ctrlWnd);

    if (NULL != pAddChild)
    {
        pAddChild->AddChild(_objTree->GetControlWindow()->GetControlWindow(ctrlWnd));
    }

    suic::Element* propWnd = FindName(_U("PropPanel"));
    pAddChild = suic::DynamicCast<suic::IAddChild>(propWnd);

    if (NULL != pAddChild)
    {
        suic::FrameworkElement* fe = _objTree->GetPropertyWindow()->GetPropertyWindow(propWnd);
        if (NULL != fe)
        {
            pAddChild->AddChild(fe);
            propWnd->UpdateMeasure();
            _objTree->GetPropertyWindow()->InitEditRootPanel(_themeWnd, fe);
        }
    }
}

void TemplateEditorWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
    InitTreeDoc();

    suic::TextBlock* pLink = FindElem<suic::TextBlock>(_U("EditTempTrigger"));
    if (NULL != pLink)
    {
        pLink->AddHandler(suic::Hyperlink::ClickEvent, new RoutedEventHandler(this, &TemplateEditorWindow::OnEditTempPropClick));
    }
}

void TemplateEditorWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}
