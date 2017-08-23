/////////////////////////////////////////////////////
// LoginWindow.cpp

#include "stdafx.h"
#include "LoginWindow.h"

LoginWindow::LoginWindow()
{
}

LoginWindow::~LoginWindow()
{
}

bool LoginWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void LoginWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void LoginWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
}

void LoginWindow::StartAnimation(eAniType type)
{
    Element* elem = FindName(_U("MainGrid"));
    if (elem)
    {
        Rotate3DTransform* pTrans = new Rotate3DTransform();
        DoubleAnimation* pAni3D = new DoubleAnimation(0, 180, Duration(1000), FillBehavior::fbStop);
        RepeatBehavior rbh;
        rbh.type = RepeatBehavior::Type::Forever;

        pTrans->SetAutoScale(true);
        //pAni3D->SetRepeatBehavior(rbh);

        //pAni3D->SetAutoReverse(true);
        pAni3D->SetAccelerationRatio(0.1);
        pAni3D->SetDecelerationRatio(0.6);
        elem->SetRenderTransform(pTrans);
        //elem->SetRenderTransformOrigin(fPoint(0.5, 0.5));

        //
        // 自定义我们动画值计算，使得翻转到90时反向进行
        //
        pAni3D->SetEasingFunction(new LoginEasingFunc(this, type));
        pAni3D->Completed += EventHandler(this, &LoginWindow::OnTimeInvalidated);

        pTrans->BeginAnimation(Rotate3DTransform::AngleYProperty, pAni3D);
    }
}

void LoginWindow::OnTimeInvalidated(Object* sender, EventArg* e)
{
    Clock* pClock = (Clock*)sender;
    Element* elem = FindName(_U("MainGrid"));
    elem->SetRenderTransform(NULL);
    UpdateLayout();
}

void LoginWindow::OnSettingOkBtn(Element* sender, RoutedEventArg* e)
{
    StartAnimation(eLoginSetToMain);
}

void LoginWindow::OnSettingBtn(Element* sender, RoutedEventArg* e)
{
    StartAnimation(eLoginMainToSet);
}

void LoginWindow::OnLoginButtonClick(Element* sender, RoutedEventArg* e)
{
    suic::String userId;
    suic::String userPwd;
    suic::ComboBox* cmb = FindElem<suic::ComboBox>(_U("UserCmb"));
    suic::PasswordBox* pwd = FindElem<suic::PasswordBox>(_U("UserPwd"));

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
        GetUserInfo()->SetIsLogin(true);
        GetUserInfo()->SetUserName(_U("等雨停了"));
        GetUserInfo()->SetUserNumber(userId);
        GetUserInfo()->SetUserPwd(userPwd);
        GetUserInfo()->SetUserMark(_U("一生有你"));
        HwndHelper::CloseWindow(this, true);
    }

    e->SetHandled(true);
}

void LoginWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    // 窗口居中显示
    CenterWindow();

    suic::ComboBox* cmb = FindElem<suic::ComboBox>(_U("UserCmb"));
    if (NULL != cmb)
    {
        cmb->Focus();
    }

    suic::Button* loginBtn = FindElem<suic::Button>(_U("loginBtn"));

    if (NULL != loginBtn)
    {
        // 初始化当前用户信息
        UserUseId* userInfo = UserManager::Ins()->GetCurrentUser();
        loginBtn->SetDataContext(userInfo);
        loginBtn->AddClick(new ClickEventHandler(this, &LoginWindow::OnLoginButtonClick));
    }

    suic::Button* setBtn = FindElem<suic::Button>(_U("SettingBtn"));
    if (setBtn)
    {
        setBtn->AddClick(new ClickEventHandler(this, &LoginWindow::OnSettingBtn));
    }

    suic::Button* setOkBtn = FindElem<suic::Button>(_U("SetOkBtn"));
    if (setOkBtn)
    {
        setOkBtn->AddClick(new ClickEventHandler(this, &LoginWindow::OnSettingOkBtn));
    }

    Element* fe = 0;//FindName(_U("UserHead"));

    if (NULL != fe)
    {
        Rotate3DTransform* pTrans = new Rotate3DTransform();
        DoubleAnimation* pAni3D = new DoubleAnimation(0, 360, Duration(3000), FillBehavior::fbHoldEnd);
        RepeatBehavior rbh;
        rbh.type = RepeatBehavior::Type::Forever;

        //pTrans->SetAutoScale(false);
        pAni3D->SetRepeatBehavior(rbh);
        //pAni3D->SetAccelerationRatio(0.2f);
        //pAni3D->SetDecelerationRatio(0.1f);
        fe->SetRenderTransform(pTrans);
        //fe->SetRenderTransformOrigin(fPoint(0.5, 0.0));
        pTrans->BeginAnimation(Rotate3DTransform::AngleYProperty, pAni3D);
    }

    StartHeadImgAnimate(FindName(_U("UserHead")), true);
}

void LoginWindow::Render(suic::Drawing* drawing)
{
    Window::Render(drawing);
    //suic::Bitmap* pBmp = drawing->GetBitmap();
    //suic::String filename(_U("d:\\login.png"));
    //pBmp->Save(filename, suic::Bitmap::ImageType::kPNG);
}

void LoginWindow::StartHeadImgAnimate(Element* fe, bool bScale)
{
    if (fe)
    {
        if (fe->GetRenderTransform())
        {
            fe->GetRenderTransform()->BeginAnimation(RotateTransform::AngleProperty, NULL);
        }

        TransformGroup* transGrp = new TransformGroup();
        RotateTransform* trans = new RotateTransform();
        RepeatBehavior rb;

        rb.type = RepeatBehavior::Type::Forever;

        if (bScale)
        {
            SkewTransform* transSkew = new SkewTransform();
            DoubleAnimation* dAniX = new DoubleAnimation(0.1f, 0.8f, Duration(2000), FillBehavior::fbHoldEnd);
            DoubleAnimation* dAniY = new DoubleAnimation(0.1f, 0.8f, Duration(2000), FillBehavior::fbHoldEnd);

            transGrp->Add(transSkew);

            dAniX->SetRepeatBehavior(rb);
            dAniX->SetAutoReverse(true);

            dAniY->SetRepeatBehavior(rb);
            dAniY->SetAutoReverse(true);

            transSkew->SetCenterX(2);
            transSkew->SetCenterY(2);

            transSkew->BeginAnimation(SkewTransform::AngleXProperty, dAniX);
            transSkew->BeginAnimation(SkewTransform::AngleYProperty, dAniY);
        }

        transGrp->Add(trans);

        fe->SetRenderTransform(transGrp);
        fe->SetRenderTransformOrigin(fPoint(0.5f, 0.5f));
        DoubleAnimation* dAni = new DoubleAnimation(0, 360, Duration(2000), FillBehavior::fbHoldEnd);

        dAni->SetRepeatBehavior(rb);
        trans->BeginAnimation(RotateTransform::AngleProperty, dAni);

        DoubleAnimation* dhAni = new DoubleAnimation(0.2f, 1.0f, Duration(2000), FillBehavior::fbHoldEnd);

        dhAni->SetRepeatBehavior(rb);
        dhAni->SetAutoReverse(true);
        dhAni->SetAccelerationRatio(0.8f);
        fe->BeginAnimation(OpacityProperty, dhAni);
    }
}
