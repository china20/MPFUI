// 华勤科技版权所有 2010-2022
// 
// 文件名：LoginWindow.h
// 功  能：主界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _LOGINWINDOW_H_
#define _LOGINWINDOW_H_

#include "UserManager.h"

using namespace suic;

enum eAniType
{
    eLoginMainToSet,
    eLoginSetToMain,
};

class LoginEasingFunc : public IEasingFunction
{
public:

    LoginEasingFunc(suic::Element* pMain, eAniType type)
    {
        _bInited = false;
        _main = pMain;
        _type = type;
    }

    Float Ease(Float normalizedTime)
    {
        if (normalizedTime >= 0.5)
        {
            if (!_bInited)
            {
                suic::Element* pSet = _main->FindName("LoginSet");
                suic::Element* pMain = _main->FindName("LoginMain");
                if (NULL != pMain && pSet != NULL)
                {
                    if (_type == eLoginMainToSet)
                    {
                        pSet->SetVisibility(Visibility::Visible);
                        pMain->SetVisibility(Visibility::Collapsed);
                    }
                    else
                    {
                        pSet->SetVisibility(Visibility::Collapsed);
                        pMain->SetVisibility(Visibility::Visible);
                    }
                }
            }
            return normalizedTime - 1.0f;
        }
        else
        {
            return normalizedTime;
        }
    }

private:

    bool _bInited;
    eAniType _type;
    suic::Element* _main;
};

class LoginWindow : public suic::Window
{
public:

    LoginWindow();
    ~LoginWindow();

    bool IsLogin() const;

    UserUseId* GetUserInfo() const;

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void Render(suic::Drawing* drawing);

    void OnTimeInvalidated(Object* sender, EventArg* e);
    void OnSettingBtn(Element* sender, RoutedEventArg* e);
    void OnSettingOkBtn(Element* sender, RoutedEventArg* e);

    void OnLoginButtonClick(Element* sender, RoutedEventArg* e);
    
protected:

    void StartAnimation(eAniType type);
    void StartHeadImgAnimate(Element* fe, bool bScale);

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);
};

typedef shared<LoginWindow> LoginWindowPtr;

inline bool LoginWindow::IsLogin() const
{
    return (GetUserInfo()->IsLogin());
}

inline UserUseId* LoginWindow::GetUserInfo() const
{
    return UserManager::Ins()->GetCurrentUser();
}

#endif

