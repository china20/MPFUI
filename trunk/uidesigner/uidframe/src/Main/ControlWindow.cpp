/////////////////////////////////////////////////////
// ControlWindow.cpp

#include "stdafx.h"
#include "ControlWindow.h"

ControlWindow::ControlWindow()
{

}

ControlWindow::~ControlWindow()
{
}

bool ControlWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void ControlWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void ControlWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
}

void ControlWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}


ControlWindowOp::ControlWindowOp()
{
    _ctrlWindow = NULL;
    _moreElement = NULL;
    _ctrlAssetsWnd = NULL;
}

ControlWindowOp::~ControlWindowOp()
{
    if (NULL != _ctrlWindow)
    {
        _ctrlWindow->unref();
    }

    if (NULL != _ctrlAssetsWnd)
    {
        _ctrlAssetsWnd->unref();
    }
}

suic::FrameworkElement* ControlWindowOp::GetControlWindow(suic::Element* parent)
{
    if (NULL == _ctrlWindow)
    {
        const suic::String strPath(_U("/mpfuid;/resource/uidesign/layout/ControlWindow.xaml"));

        _ctrlWindow = new ControlWindow();
        _ctrlWindow->ref();

        suic::XamlReader::LoadUri(parent, _ctrlWindow, strPath.c_str());
        _ctrlWindow->AddHandler(suic::RadioButton::CheckedEvent, new suic::RoutedEventHandler(this, &ControlWindowOp::OnCheckedChanged));

        suic::Button* moreExpanderBtn = _ctrlWindow->FindElem<suic::Button>(_U("MoreExpander"));
        if (NULL != moreExpanderBtn)
        {
            moreExpanderBtn->AddClick(new suic::ClickEventHandler(this, &ControlWindowOp::OnMoreExpanderClick));
        }
    }

    return _ctrlWindow;
}

void ControlWindowOp::OnMoreExpanderClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    if (NULL == _ctrlAssetsWnd)
    {
        _ctrlAssetsWnd = new ControlAssetsWindow(this);
        _ctrlAssetsWnd->ref();
        suic::Builder::BuildUri(_ctrlAssetsWnd, "/mpfuid;/resource/uidesign/layout/editor/ControlAssetsWindow.xaml");
    }

    if (_ctrlAssetsWnd->IsClosed())
    {
        Popup popup;
        popup.setAutoDelete(false);
        popup.SetPlacementTarget(sender);
        popup.SetPlacement(suic::PlacementMode::pmBottom);
        popup.SetChild(_ctrlAssetsWnd);
        popup.TrackingPopup();
    }
}

suic::FrameworkElementPtr ControlWindowOp::GetControlSample(suic::Object* parent, String strType)
{
    suic::String strPath;
    strPath.Format(_U("/mpfuid;/resource/uidesign/Controls/%s.control"), strType.c_str());
    return suic::Builder::BuildUri(NULL, strPath.c_str()).get();
}

void ControlWindowOp::RestoreSelectMode()
{
    suic::RadioButton* pSelect = _ctrlWindow->FindElem<suic::RadioButton>(_U("SelectArrow"));
    if (NULL != pSelect)
    {
        pSelect->SetChecked(Boolean::True);
    }
}

suic::RadioButton* ControlWindowOp::GetMoreElement()
{
    if (NULL == _moreElement)
    {
        _moreElement = _ctrlWindow->FindElem<suic::RadioButton>(_U("MoreElement"));
    }
    return _moreElement;
}

void ControlWindowOp::UpdateMoreElement(const String& name, bool setChecked)
{
    suic::RadioButton* pMoreElem = GetMoreElement();
    if (NULL != pMoreElem)
    {
        suic::String strIconOn;
        suic::String strIconOff;
        suic::ImageBrush* iconOff = NULL;
        suic::ImageBrush* iconOn = NULL;

        strIconOn.Format(_U("%s_on_24x24"), name.c_str());
        strIconOff.Format(_U("%s_off_24x24"), name.c_str());

        iconOn = RTTICast<suic::ImageBrush>(_ctrlWindow->FindRes(strIconOn));
        iconOff = RTTICast<suic::ImageBrush>(_ctrlWindow->FindRes(strIconOff));

        if (NULL == iconOff || NULL == iconOn)
        {
            iconOn = RTTICast<suic::ImageBrush>(_ctrlWindow->FindRes(_U("Element_on_24x24")));
            iconOff = RTTICast<suic::ImageBrush>(_ctrlWindow->FindRes(_U("Element_off_24x24")));
        }

        pMoreElem->SetTag(iconOff);
        pMoreElem->SetUserTag(1, iconOn);
        pMoreElem->SetUserTag(3, new OString(name));
        pMoreElem->SetToolTip(pMoreElem->GetUserTag(3));

        if (setChecked)
        {
            if (pMoreElem->IsChecked() && _selectEvent.safe_bool())
            {
                _selectEvent(DesignMode::NoneMode, name);
            }

            pMoreElem->SetChecked(Boolean::True);
        }

        _ctrlWindow->UpdateLayout();
    }
}

void ControlWindowOp::SetSelectNotify(SelectNotifyEventHandler selEvent)
{
    _selectEvent = selEvent;
}

void ControlWindowOp::OnCheckedChanged(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    DesignMode mode = DesignMode::AddMode;
    RadioButton* pButton = suic::DynamicCast<RadioButton>(e->GetOriginalSource());
    suic::String strName(pButton->GetName());

    if (strName.Equals(_U("MoveArrow")))
    {
        mode = DesignMode::MoveMode;
    }
    else if (strName.Equals(_U("SelectArrow")))
    {
        mode = DesignMode::SelectMode;
    }
    else if (strName.Equals(_U("MoreElement")))
    {
        Object* pTag = pButton->GetUserTag(3);
        if (NULL != pTag && DpProperty::UnsetValue() != pTag)
        {
            strName = pTag->ToString();
        }
    }
    else
    {
        UpdateMoreElement(strName, false);
    }

    if (_selectEvent.safe_bool())
    {
        _selectEvent(mode, strName);
    }
}
