/////////////////////////////////////////////////////
// UserInfoWindow.cpp

#include "stdafx.h"
#include "UserInfoWindow.h"

UserInfoWindow::UserInfoWindow()
{
    _moveTop = 20;
    _userInfo = NULL;
    _timer = new AssignerTimer();
    _timer->ref();
}

UserInfoWindow::UserInfoWindow(UserUseId* userInfo)
{
    _moveTop = 30;
    _userInfo = userInfo;
    _timer = new AssignerTimer();
}

UserInfoWindow::~UserInfoWindow()
{
    _timer->unref();
}

void UserInfoWindow::OnTimer(Object* sender, EventArg* e)
{
    _moveTop -= 6;

    SetPos(_pos.x, min(_pos.y, _pos.y - _moveTop));
    SetOpacity(GetOpacity() + 0.04);

    //UpdateLayout();

    if (GetOpacity() >= 1)
    {
        SetOpacity(1.0);
        _timer->SetEnabled(false);
    }
}

bool UserInfoWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void UserInfoWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void UserInfoWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
}

void UserInfoWindow::StartHeadImgAnimate()
{
    Element* fe = this;
    if (fe)
    {
        if (fe->GetRenderTransform())
        {
            fe->GetRenderTransform()->BeginAnimation(RotateTransform::AngleProperty, NULL);
        }

        TransformGroup* transGrp = new TransformGroup();
        ScaleTransform* transScale = new ScaleTransform();

        transGrp->Add(transScale);
        fe->SetRenderTransform(transGrp);
        fe->SetRenderTransformOrigin(fPoint(0.5f, 0.5f));
        
        DoubleAnimation* dAniX = new DoubleAnimation(0.0f, 1.0f, Duration(250), FillBehavior::fbHoldEnd);
        DoubleAnimation* dAniY = new DoubleAnimation(0.0f, 1.0f, Duration(250), FillBehavior::fbHoldEnd);

        //dAniX->SetEasingFunction(new BackEase());
        //dAniY->SetEasingFunction(new BackEase());

        transScale->BeginAnimation(ScaleTransform::ScaleXProperty, dAniX);
        transScale->BeginAnimation(ScaleTransform::ScaleYProperty, dAniY);

        DoubleAnimation* dhAni = new DoubleAnimation(0.6f, 1.0f, Duration(200), FillBehavior::fbHoldEnd);

        dhAni->SetAccelerationRatio(0.5);

        fe->BeginAnimation(OpacityProperty, dhAni);
    }
}

void UserInfoWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    SetOpacity(0);
    _pos = GetLocation();
    // ´°¿Ú¾ÓÖÐÏÔÊ¾
    SetDataContext(_userInfo);

    /*SetPos(_pos.x, _pos.y - _moveTop);

    _timer->__setAutoDelete(false);
    _timer->SetInterval(10);
    _timer->SetTick(EventHandler(this, &UserInfoWindow::OnTimer));
    _timer->Start();*/

    StartHeadImgAnimate();

    //BeginAnimation(OpacityProperty, NULL);
    //Duration duration(300);
    //DoubleAnimation* pAni = new DoubleAnimation(0.2, 1, duration, FillBehavior::fbHoldEnd);

    //pAni->SetAutoReverse(true);
    //pAni->SetAccelerationRatio(0.3);
    //pAni->SetDecelerationRatio(0.3);
    //BeginAnimation(OpacityProperty, pAni);
}
