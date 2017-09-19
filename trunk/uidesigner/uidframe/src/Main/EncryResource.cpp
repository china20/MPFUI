/////////////////////////////////////////////////////
// EncryResource.cpp

#include "stdafx.h"
#include "EncryResource.h"

EncryResource::EncryResource(Project* pPrj)
{
    _project = pPrj;
}

EncryResource::~EncryResource()
{
}

void EncryResource::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);

    Button* pBtn(FindElem<Button>(_T("OpenEncryFile")));

    if (pBtn)
    {
        // 注册按钮的点击事件
        pBtn->AddClick(new suic::ClickEventHandler(this, &EncryResource::OnOpenFile));
    }

    pBtn = FindElem<Button>(_T("EncryBtn"));

    if (pBtn)
    {
        // 注册按钮的点击事件
        pBtn->AddClick(new suic::ClickEventHandler(this, &EncryResource::OnEncry));
    }

    pBtn = FindElem<Button>(_T("Cancel"));

    if (pBtn)
    {
        // 注册按钮的点击事件
        pBtn->AddClick(new suic::ClickEventHandler(this, &EncryResource::OnCancel));
    }

    AddHandler(PreKeyDownEvent, new suic::KeyboardEventHandler(this, &EncryResource::OnEnter), false);
}

void EncryResource::OnEnter(suic::Element* sender, suic::KeyboardEventArg* e)
{
    if (e->GetKey() == Key::kReturn)
    {
        e->SetHandled(true);

        //suic::RoutedEventArg e(this);
        //OnEncry(this, e);
    }
}

void EncryResource::OnLoaded(suic::LoadedEventArg* e)
{
    __super::OnLoaded(e);
}

void EncryResource::OnOpenFile(suic::Element* pItem, suic::RoutedEventArg* e)
{
    FileBrowser fb;

    if (fb.Save(this))
    {
        suic::TextBox* pathPtr(FindElem<suic::TextBox>(_T("EncryFile")));

        pathPtr->SetText(fb.GetFilePath());
        pathPtr->InvalidateForce();
    }
}

void EncryResource::OnEncry(suic::Element* pItem, suic::RoutedEventArg* e)
{
    suic::CheckBox* onlyEncry(FindElem<suic::CheckBox>(_T("onlyEncry")));
    suic::TextBox* pathPtr(FindElem<suic::TextBox>(_T("EncryFile")));
    suic::PasswordBox* pwdPtr(FindElem<suic::PasswordBox>(_T("Pwd")));

    suic::String strFile = pathPtr->GetText();
    suic::String strPwd = pwdPtr->GetPassword();

    if (strFile.Empty())
    {
        suic::InfoBox::Show(_T("请输入导出文件!"), _T("提示"));
        return;
    }

    // 仅仅加密
    if (onlyEncry->IsChecked())
    {
        if (strPwd.Empty())
        {
            suic::InfoBox::Show(_T("请输入资源加密密码!"), _T("提示"));
            return;
        }

        if (UIEncrypt::Encrypt(strPwd, strFile) == 0)
        {
            suic::InfoBox::Show(_U("加密资源成功!"), _T("提示"));  
            AsyncClose();
        }
        return;
    }

    if (!_project->Zip(strFile, strPwd))
    {
        suic::InfoBox::Show(_T("导出失败!"), _T("提示"));
        return ;
    }

    AsyncClose();
    suic::InfoBox::Show(__T("导出成功!"), _T("提示"));  

    /*if (0 != UIEncrypt::Encrypt(strPwd, strFile))
    {
        suic::InfoBox::Show(_T("提示"), _T("导出失败!"));
    }
    else
    {
        Close();
        suic::InfoBox::Show(_T("提示"), _T("导出成功!"));  
    }*/
}

void EncryResource::OnCancel(suic::Element* pItem, suic::RoutedEventArg* e)
{
    Close();
}