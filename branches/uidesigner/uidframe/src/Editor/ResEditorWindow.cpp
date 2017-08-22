/////////////////////////////////////////////////////
// TemplateEditorWindow.cpp

#include "stdafx.h"

#include <Main/DesignPanel.h>
#include <Editor/TemplateEditorWindow.h>
#include <Core/StyleNode.h>

#include <Framework/Controls/Rectangle.h>

TemplateWindow::TemplateWindow(XamlTemplateElementNode* resElem)
{
    _objTree = new ObjTreeManager();
    _objTree->ref();
}

TemplateWindow::~TemplateWindow()
{
}

void TemplateWindow::InitTreeDoc()
{
    _objTree->InitElementTree(this);

    // 初始化界面编辑面板
    DesignPanel* designWnd = _objTree->GetDesignPanel();
    ScrollViewer* pViewer = FindElem<ScrollViewer>(_U("DesignViewer"));

    if (NULL != pViewer)
    {
        pViewer->SetContent(designWnd);
    }

    suic::Element* ctrlWnd = FindName(_U("CtrlToolbar"));
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
        }
    }
}

void TemplateWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
    InitTreeDoc();
}

void TemplateWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}
