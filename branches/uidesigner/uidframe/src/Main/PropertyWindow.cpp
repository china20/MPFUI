/////////////////////////////////////////////////////
// PropertyWindow.cpp

#include "stdafx.h"

#include <Core/ValueEditor.h>
#include <Editor/EditRootPanel.h>

#include "PropertyWindow.h"
#include "DesignPanel.h"

#include <Framework/Controls/Path.h>
#include <Framework/Controls/Thumb.h>
#include <Framework/Controls/Track.h>
#include <Framework/Controls/DatePicker.h>
#include <Framework/Controls/Canvas.h>
#include <Framework/Controls/DockPanel.h>
#include <Framework/Controls/ScrollBar.h>

ImplementRTTIOfClass(NameEditor, suic::TextBox)

void NameEditor::SetTargetElement(DesignElement* delem, EditRootPanel* editPanel)
{
    _elem = delem;
    _editPanel = editPanel;
    SetText(delem->GetName());
}

void NameEditor::UpdateName()
{
    if (NULL != _elem)
    {
        String strVal = GetText();
        SetterNode* pSetter = _elem->GetSetterCollection()->FindSetter(_U("Name"));
        if (NULL == pSetter)
        {
            pSetter = new SetterNode(_U("Name"), Element::NameProperty, new OString(strVal));
            _elem->GetSetterCollection()->AddSetter(pSetter);
        }
        else
        {
            pSetter->SetValue(new OString(strVal));
        }

        _elem->SetDisplayName(strVal);
        _editPanel->SetModified(true);
    }
}

PropertyWindow::PropertyWindow()
{
}

PropertyWindow::~PropertyWindow()
{
    
}

bool PropertyWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void PropertyWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void PropertyWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}

PropertyWindowOp::PropertyWindowOp()
{
    _propWindow = NULL;
    _editPanel = NULL;
    _designPanel = NULL;
}

PropertyWindowOp::~PropertyWindowOp()
{
    if (NULL != _propWindow)
    {
        _propWindow->unref();
    }
}

void PropertyWindowOp::InitEditRootPanel(Element* rootPanel, Element* parent)
{
    _editPanel = rootPanel->FindElem<EditRootPanel>(_U("editRootPanel"));
    if (NULL != _editPanel)
    {
        _editPanel->InitPropsInfo(parent);
    }
}

suic::FrameworkElement* PropertyWindowOp::GetPropertyWindow(suic::Element* parent)
{
    if (NULL == _propWindow)
    {
        const suic::String strPath(_U("/mpfuid;/resource/uidesign/layout/PropertyWindow.xaml"));
        _propWindow = new PropertyWindow();
        _propWindow->ref();
        suic::XamlReader::LoadUri(parent, _propWindow, strPath.c_str());
    }
    return _propWindow;
}

void PropertyWindowOp::SetCurrentElement(DesignElement* resElem, DesignElement* delem)
{
    if (NULL != _editPanel)
    {
        _editPanel->SwitchCurrentElement(resElem, delem);
    }
}

void PropertyWindowOp::SetDesignPanel(DesignPanel* design)
{
    _designPanel = design;
    _editPanel->SetDesignPanel(design);
}

void PropertyWindowOp::UpdateModified()
{
    _editPanel->SetModified(true);
}

void PropertyWindowOp::InitEditPanelInfo(ElementRootItem* rootElem, DesignPanel* design)
{
    if (NULL != _editPanel)
    {
        _designPanel = design;
        _editPanel->InitEditPanelInfo(rootElem, design);
    }
}

void PropertyWindowOp::UpdateSetter(SetterNode* pSetter, bool needNotifyChanged)
{
    if (NULL != _editPanel)
    {
        _editPanel->UpdateSetter(pSetter, needNotifyChanged);
    }
}

void PropertyWindowOp::UpdateResource(ResNode* setter)
{

}

void PropertyWindowOp::SetSetterNotify(SetterNotifyEventHandler h)
{

}
