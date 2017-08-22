/////////////////////////////////////////////////////
// SettingWindow.cpp

#include "stdafx.h"
#include "SettingWindow.h"

SettingWindow::SettingWindow()
{
    _labels.Add(_U("Login"));
    _labels.Add(_U("Main"));
    _labels.Add(_U("State"));
    _labels.Add(_U("Session"));
    _labels.Add(_U("Info"));
    _labels.Add(_U("Tips"));
    _labels.Add(_U("HotKey"));
    _labels.Add(_U("Voice"));
    _labels.Add(_U("Update"));
    _labels.Add(_U("Files"));
    _labels.Add(_U("Media"));
}

SettingWindow::~SettingWindow()
{
 
}

bool SettingWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void SettingWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void SettingWindow::OnLoaded(suic::LoadedEventArg* e)
{
    OSWindow::OnLoaded(e);
    suic::Element* pLeftPanel = FindName("LeftSwitch");
    if (NULL != pLeftPanel)
    {
        pLeftPanel->AddHandler(suic::Button::ClickEvent, new suic::ClickEventHandler(this, &SettingWindow::OnLeftSwitchClick));
    }

    suic::ScrollViewer* pView = FindElem<suic::ScrollViewer>("ScrollView");
    if (pView)
    {
        pView->AddScrollChanged(new suic::ScrollChangedEventHandler(this, &SettingWindow::OnScrollViewChanged));
    }
}

void SettingWindow::OnScrollViewChanged(suic::Element*, suic::ScrollChangedEventArg* e)
{
    for (int i = 0; i < _labels.GetCount(); ++i)
    {
        suic::String strName;
        strName.Format(_U("%sPanel"), _labels[i].c_str());
        suic::Element* pPanel = FindName(strName);

        if (NULL != pPanel)
        {
            int iOffset = pPanel->GetVisualOffset().y + pPanel->GetActualHeight();
            if (iOffset >= e->GetVertScrollOffset())
            {
                suic::RadioButton* pBtn = FindElem<suic::RadioButton>(_labels[i]);
                if (NULL != pBtn)
                {
                    pBtn->SetChecked(suic::Boolean::True);
                }
                break;
            }
        }
    }
}

void SettingWindow::OnLeftSwitchClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    suic::ScrollViewer* pView = FindElem<suic::ScrollViewer>("ScrollView");
    if (pView)
    {
        suic::String strName;
        strName.Format(_U("%sPanel"), ((suic::Element*)e->GetOriginalSource())->GetName().c_str());
        suic::Element* pPanel = FindName(strName);

        if (NULL != pPanel)
        {
            int iOffset = pPanel->GetVisualOffset().y;

            //strName.Format(_U("%sPanel->%d"), strName.c_str(), iOffset);
            //InfoBox::Show(strName, _U("пео╒"), InfoBox::IB_OK);
            pView->SetVerticalOffset(iOffset);
        }
    }
}
