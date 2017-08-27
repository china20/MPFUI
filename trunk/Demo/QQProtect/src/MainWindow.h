// 华勤科技版权所有 2010-2022
// 
// 文件名：MainWindow.h
// 功  能：主界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "ChatWindow.h"
#include "LoginWindow.h"
#include "ThemeWindow.h"
#include "SettingWindow.h"
#include "UserInfoWindow.h"
#include "ChatSessionManager.h"

#include <Framework/Controls/Menu.h>

using namespace suic;

class MenuData : public ICommand, public NotifyPropChanged
{
public:

    BeginMember(MenuData, NotifyPropChanged)
        MemberGetString(Name)
        MemberGetObject(Icon)
    EndMember()

    MenuData()
    {
        _icon = NULL;
    }

    ~MenuData()
    {
    }

    MenuData(String name, Object* icon)
    {
        _name = name;
        _icon = icon;
    }

    bool CanExecute(Object* target, Object* param);
    void Execute(Object* target, Object* param);

    String GetName()
    {
        return _name;
    }

    Object* GetIcon()
    {
        return _icon;
    }

    String ToString()
    {
        return _name;
    }

private:

    String _name;
    Object* _icon;
};

class MainWindow : public OSWindow
{
public:

    MainWindow();
    ~MainWindow();

    void OnLoaded(suic::LoadedEventArg* e);

    void OnButtonClicked(Element* sender, RoutedEventArg* e);
    void OnTreeScrollChanged(Element* sender, ScrollChangedEventArg* e);
    void OnDblTreeClick(Element* sender, MouseButtonEventArg* e);
    void OnTreeKeydown(Element* sender, KeyboardEventArg* e);

    void OnThemeClick(Element* sender, RoutedEventArg* e);
    void OnMainMenuClick(Element* sender, RoutedEventArg* e);
    void OnUserSet(Element* sender, RoutedEventArg* e);
    void OnMenuButtonClick(Element* sender, RoutedEventArg* e);

    void OnCheckUserWndTimer(Object* sender, EventArg* e);

    void OnMouseEnterUserInfo(Element* sender, MouseButtonEventArg* e);
    void OnMouseLeaveUserInfo(Element* sender, MouseButtonEventArg* e);
    
    void OnClickHyper(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    void InitUserInfo();
    void InitUserIds();
    void InitBusiness();

    void Dispose();

    void StartHeadImgAnimate(Element* fe, bool bScale=true);

protected:

    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    int _iAniNum;

    WindowPtr _userWnd;
    WindowPtr _themeWnd;
    AssignerTimer* _timer;

    LoginWindowPtr _login;
    SettingWindow* _setWnd;
    suic::ContextMenu* _userMenu;
    suic::ContextMenu* _mainMenu;

    ChatSessionManager _chatSessionMana;
};

#endif

