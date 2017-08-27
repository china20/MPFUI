
#include "stdafx.h"
#include "EditRootPanel.h"

#include <System/Resources/DynamicResource.h>

#include <Core/ValueEditor.h>
#include <Main/Project.h>
#include <Main/DesignPanel.h>
#include <Main/MainWindow.h>
#include <Editor/GridDefWindow.h>

ImplementRTTIOfClass(EditRootPanel, suic::Grid)

EditRootPanel::EditRootPanel()
{
    _editElem = NULL;
    _designPanel = NULL;
    _rootItem = NULL;
    _designRoot = NULL;
    _project = NULL;
    //_tempParent = NULL;
    _currentResNode = NULL;

    _isSettingElement = false;
    _timer = new suic::AssignerTimer();
    _timer->ref();
}

EditRootPanel::~EditRootPanel()
{
    int i = 0;

    for (i = 0; i < _props.GetCount(); ++i)
    {
        PropInfo* propInfo = _props[i];
        delete propInfo;
    }

    _props.Clear();
    _timer->unref();
}

void EditRootPanel::OnInitialized(EventArg* e)
{
    suic::Grid::OnInitialized(e);

    _timer->SetTick(suic::EventHandler(this, &EditRootPanel::OnTimer));
    _timer->SetInterval(100);
    _timer->Start();
}

void EditRootPanel::OnTimer(suic::Object* sender, suic::EventArg* e)
{
    if (NULL != _editElem)
    {
        EnableDesignWithElementLock(_editElem);
    }
}

void EditRootPanel::InitPropsInfo(Element* parent)
{
    int i = 0;
    Collection<DpItem*> dpItems;

    DpManager::Ins()->GetDpItems(dpItems);

    for (i = 0; i < dpItems.GetCount(); ++i)
    {
        RTTIOfInfo* target = NULL;
        SetterEditor* setterEditor = NULL;
        DpItem* dpItem = dpItems.GetItem(i);
        suic::String strName = dpItem->name;
        setterEditor = dynamic_cast<SetterEditor*>(parent->FindName(strName));
 
        if (NULL == setterEditor && dpItem->dps.GetCount() > 0)
        {
            for (int i = 0; i < dpItem->dps.GetCount(); ++i)
            {
                if (dpItem->dps[i]->IsAttached())
                {
                    strName.Format(_U("%s.%s"), dpItem->dps[i]->GetOwnerType()->typeName, dpItem->name.c_str());
                    setterEditor = dynamic_cast<SetterEditor*>(parent->FindName(strName));
                    target = dpItem->dps[i]->GetOwnerType();
                    break;
                }
            }
        }

        if (NULL != setterEditor)
        {
            PropInfo* propInfo = new PropInfo();
            propInfo->dpItem = dpItem;
            propInfo->name = strName;
            propInfo->target = target;
            propInfo->valEditor = setterEditor;
            _props.Add(propInfo);
        }
    }

    ButtonSetterEditor* btnEditor = parent->FindElem<ButtonSetterEditor>(_U("RCDefinitions"));
    if (NULL != btnEditor)
    {
        PropInfo* propInfo = new PropInfo();
        propInfo->dpItem = NULL;
        propInfo->valEditor = btnEditor;
        propInfo->target = suic::Grid::RTTIType();
        _props.Add(propInfo);

        btnEditor->AddClick(new suic::ClickEventHandler(this, &EditRootPanel::OnEditGridDefinitions));
    }
}

void EditRootPanel::OnEditGridDefinitions(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    ButtonSetterEditor* btnEditor = RTTICast<ButtonSetterEditor>(sender->GetUIParent());
    if (NULL != btnEditor)
    {
        suic::Window* pWnd = new GridDefWindow(btnEditor);
        pWnd->ref();
        pWnd->ShowDialog(_U("/mpfuid;/resource/uidesign/layout/GridDefWindow.xaml"));
        pWnd->unref();
    }
}

void EditRootPanel::SetDesignPanel(DesignPanel* design)
{
    _designPanel = design;
}

void EditRootPanel::UpdateDesignPanel(DesignElement* target)
{
    if (NULL != _designPanel && NULL != _designPanel->GetRootElement() && 
        target == _designPanel->GetRootElement()->GetRootElement())
    {
        _designPanel->UpdateDElement();
    }
}

void EditRootPanel::SetRootItem(RootItem* rootItem)
{
    _rootItem = rootItem;
}

void EditRootPanel::SetDesignRootItem(RootItem* rootItem)
{
    _designRoot = rootItem;
}

//void EditRootPanel::SetTemplateParent(RTTIOfInfo* typeInfo)
//{
//    _tempParent = typeInfo;
//}

//RTTIOfInfo* EditRootPanel::GetTemplateParentType()
//{
//    return _tempParent;
//}

void EditRootPanel::InitEditPanelInfo(RootItem* rootElem, DesignPanel* design)
{
    _designPanel = design;
    _designRoot = rootElem;

    if (NULL != rootElem)
    {
        _project = rootElem->GetProject();
        _resName = _project->GetProjectName();
        _resDir = _project->GetProjectDir();

        TemplateRootItem* tempRoot = RTTICast<TemplateRootItem>(rootElem);

        if (tempRoot == NULL)
        {
            _rootItem = rootElem;
        }

        suic::ThemeOp::AddDir(_resName, _resDir);
    }
    else
    {
        _project = NULL;
        _resName = _U("");
        _resDir = _U("");
        _rootItem = NULL;
    }
}

void EditRootPanel::EnableDesignWithElementLock(DesignElement* delem)
{
    if (delem != NULL)
    {
        suic::Element* propWnd = FindName(_U("propWindow"));
        if (NULL != propWnd)
        {
            if (delem->GetLockElement())
            {
                propWnd->Enable(false);
            }
            else
            {
                propWnd->Enable(true);
            }
        }
    }
}

void EditRootPanel::SwitchCurrentElement(DesignElement* resElem, DesignElement* delem)
{
    if (_editElem == delem || NULL == _designPanel || _isSettingElement)
    {
        return;
    }

    int i = 0;
    bool elemIsPanel = false;
    RTTIOfInfo* rttiInfo = NULL;
    RTTIOfInfo* panelRttiInfo = NULL;
    FrameworkElement* dfe = NULL;

    _isSettingElement = true;

    if (delem != NULL)
    {
        dfe = delem->GetUIElement();
        EnableDesignWithElementLock(delem);
    }

    if (delem != NULL && delem->GetParentUIElement() != NULL)
    {
        panelRttiInfo = delem->GetParentUIElement()->GetRTTIType();
        if (!panelRttiInfo->InheritFrom(Panel::RTTIType()) && 
            panelRttiInfo != suic::PlayBox::RTTIType())
        {
            panelRttiInfo = NULL;;
        }
    }

    if (NULL != delem && dfe != NULL)
    {
        rttiInfo = dfe->GetRTTIType();
        elemIsPanel = rttiInfo->InheritFrom(Panel::RTTIType());
    }

    BrushEditorPanel* brushPanel = FindElem<BrushEditorPanel>(_U("BrushEditor"));
    if (NULL != brushPanel)
    {
        brushPanel->ResetBrushEditorPanel();
    }

    for (i = 0; i < _props.GetCount(); ++i)
    {
        PropInfo* propInfo = _props[i];
        DpItem* dpItem = propInfo->dpItem;

        if (NULL != propInfo->valEditor)
        {
            if (NULL == dpItem)
            {
                if (rttiInfo == propInfo->target)
                {
                    propInfo->valEditor->Reset(resElem, delem, true);
                }
                else
                {
                    propInfo->valEditor->Reset(resElem, delem, false);
                }
            }
            else 
            {
                bool bOwner = false;
                DpProperty* dp = dpItem->GetDpEx(rttiInfo, true, bOwner);

                // 如果父控件是一个Panel
                // 检查当前属性是否为附加属性
                if (dp != NULL && dp->IsAttached())
                {
                    if (panelRttiInfo != NULL && panelRttiInfo->InheritFrom(dp->GetOwnerType()))
                    {
                        ;
                    }
                    else
                    {
                        if (dp->GetOwnerType()->InheritFrom(Panel::RTTIType()) || 
                            dp->GetOwnerType() == suic::PlayBox::RTTIType())
                        {
                            dp = NULL;
                        }
                    }
                }

                if (dp != NULL && (propInfo->target == NULL || propInfo->target == dp->GetOwnerType()))
                {
                    bool bHandled = false;
                    if (GetTemplateRoot() != NULL)
                    {
                        if (dp->GetPropType()->InheritFrom(suic::FrameworkTemplate::RTTIType()))
                        {
                            bHandled = true;
                            propInfo->valEditor->Reset(resElem, delem, false);
                        }
                    }

                    propInfo->valEditor->SetTemplateMode(GetTemplateRoot() != NULL);

                    if (!bHandled)
                    {
                        propInfo->valEditor->Reset(resElem, delem, true);
                        if (NULL != rttiInfo)
                        {
                            PropMetadata* meta = dp->GetMetadata(rttiInfo);
                            propInfo->valEditor->SetDefaultValue(meta->GetDefaultValue());
                        }
                    }
                }
                else
                {
                    propInfo->valEditor->Reset(resElem, delem, false);
                }  
            }
        }
    }

    if (delem != NULL)
    {
        NameEditor* textBox = FindElem<NameEditor>(_U("ElementName"));

        if (NULL != textBox)
        {
            textBox->SetTargetElement(delem, this);
        }

        suic::TextBlock* clsName = FindElem<suic::TextBlock>(_U("ElementType"));

        if (NULL != clsName)
        {
            clsName->SetText(delem->GetTypeName());
        }

        suic::Rectangle* clsIcon = FindElem<suic::Rectangle>(_U("ElementIcon"));

        if (NULL != clsIcon)
        {
            suic::String strIcon;
            suic::ImageBrush* imgIcon = NULL;
            
            strIcon.Format(_U("%s_on_24x24"), delem->GetTypeName().c_str());
            imgIcon = RTTICast<suic::ImageBrush>(Application::Current()->FindRes(strIcon));
            if (NULL == imgIcon)
            {
                imgIcon = RTTICast<suic::ImageBrush>(Application::Current()->FindRes(_U("Element_on_24x24")));
            }
            clsIcon->SetFill(imgIcon);
        }

        for (int i = 0; i < delem->GetSetterCollection()->GetCount(); ++i)
        {
            SetterNode* pSetter = delem->GetSetterCollection()->GetSetter(i);
            if (NULL != pSetter)
            {
                UpdateSetter(pSetter, false);
            }
        }
    }

    _editElem = delem;
    _isSettingElement = false;
}

void EditRootPanel::UpdateSetter(SetterNode* pSetter, bool needNotifyChanged)
{
    if (NULL == pSetter->GetProperty())
    {
        return;
    }

    suic::String strName = pSetter->GetName();
    suic::RTTIOfInfo* ownerType = pSetter->GetProperty()->GetOwnerType();
    suic::Element* pElem = FindName(strName);

    if (NULL == pElem && pSetter->GetProperty()->IsAttached())
    {
        strName.Format(_U("%s.%s"), ownerType->typeName, strName.c_str());
        pElem = FindName(strName);
    }

    SetterEditor* pElemBase = DynamicCast<SetterEditor>(pElem);
    if (NULL != pElemBase)
    {
        pElemBase->SetSetterNode(pSetter, pSetter->GetFromTrigger());
    }

    // 通知文件内容发生改变
    if (needNotifyChanged && _rootItem != NULL)
    {
        _rootItem->SetModified(true);
    }
}

void EditRootPanel::UpdateResource(ResNode* setter)
{
    SetModified(true);
}

void EditRootPanel::UpdateSetterToUI(SetterNode* pSetter)
{
    if (NULL != pSetter && !_isSettingElement)
    {
        if (NULL != _editElem && _editElem->GetUIElement())
        {
            if (NULL != _designPanel && NULL != _designPanel->GetRootElement())
            {
                if (_designPanel->GetRootElement()->SetElementSetter(_editElem, pSetter, true))
                {
                    // 来自于资源里的模版编辑
                    if (NULL != _rootItem)
                    {
                        _rootItem->SetModified(true);
                    }
                }
            }
        }
        else if (_rootItem != NULL)
        {
            _rootItem->SetModified(true);
        }
    }
}

void EditRootPanel::SetModified(bool bModified)
{
    if (_rootItem != NULL && !_isSettingElement)
    {
        _rootItem->SetModified(bModified);
    }
}

SetterNode* EditRootPanel::CreateSetterNode(const suic::String& prop)
{
    if (NULL == _editElem)
    {
        return NULL;
    }

    SetterNode* pSetterNode = _editElem->GetSetterCollection()->FindSetter(prop);
    DpProperty* dp = NULL;

    if (NULL == pSetterNode)
    {
        StringArray vec;
        String name = prop;
        vec.SplitString(prop, _U("."));

        if (vec.GetCount() == 2)
        {
            name = vec[1];
            RTTIOfInfo* rttiInfo = BuildFactory::Ins()->Lookup(vec[0]);
            dp = suic::DpProperty::Lookup(name, rttiInfo);
        }
        else
        {
            dp = DpManager::Ins()->FindDp(name, _editElem->GetUIElement()->GetRTTIType(), true);
        }

        if (NULL != dp)
        {
            pSetterNode = new SetterNode(name, dp, dp->GetDefaultMetadata()->GetDefaultValue());
            _editElem->GetSetterCollection()->AddSetter(pSetterNode);
        }
    }
    return pSetterNode;
}

suic::Object* EditRootPanel::FindElemRes(const suic::String& resKey)
{
    if (NULL == _editElem)
    {
        return NULL;
    }
    else
    {
        suic::FrameworkElement* fe = CASTTOFE(_editElem->GetUIElement());
        return fe->FindRes(resKey);
    }
}
