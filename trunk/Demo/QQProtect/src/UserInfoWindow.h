// 华勤科技版权所有 2010-2022
// 
// 文件名：UserInfoWindow.h
// 功  能：用户信息界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _USERINFOWINDOW_H_
#define _USERINFOWINDOW_H_

#include "UserManager.h"

using namespace suic;

class UserInfoWindow : public suic::Window
{
public:

    UserInfoWindow();
    UserInfoWindow(UserUseId* userInfo);
    ~UserInfoWindow();

    void OnTimer(Object* sender, EventArg* e);
    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

protected:

    void StartHeadImgAnimate();
    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    int _moveTop;
    Point _pos;
    UserUseId* _userInfo;
    AssignerTimer* _timer;
};

#endif

