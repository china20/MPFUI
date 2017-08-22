
#include "stdafx.h"
#include "UserInfo.h"

ImplementRTTIOfClass(UserInfo, NotifyPropChanged)

UserInfo::UserInfo(String usrName, String usrPwd)
{
    _usrName = usrName;
    _isLogin = false;
}

bool UserInfo::CanExecute(Object* target, Object* param)
{
    return true;
}

void UserInfo::Execute(Object* target, Object* param)
{
    suic::Element* elem = DynamicCast<suic::Element>(target);
    if (NULL != elem)
    {
        suic::String userId;
        suic::String userPwd;
        suic::Element* root = VisualTreeOp::GetVisualRoot(elem);
        suic::ComboBox* cmb = root->FindElem<suic::ComboBox>(_U("UserCmb"));
        sui::PasswordBox* pwd = root->FindElem<sui::PasswordBox>(_U("UserPwd"));

        if (cmb)
        {
            userId = cmb->GetText();
        }

        if (pwd)
        {
            userPwd = pwd->GetPassword();
        }

        if (userId.Empty() || userPwd.Empty())
        {
            InfoBox::Show(_U("用户号码或密码为空，请重新输入！"), _U("信息"), InfoBox::IB_OK);
        }
        else
        {
            SetIsLogin(true);
            HwndHelper::CloseWindow(elem, true);
        }
    }
}

String UserInfo::GetUserName()
{
    return _usrName;
}

void UserInfo::SetUserName(String usrName)
{
    _usrName = usrName;
}

String UserInfo::GetUserPwd()
{
    return _usrPwd;
}

void UserInfo::SetUserPwd(String usrPwd)
{
    _usrPwd = usrPwd;
}
