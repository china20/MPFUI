/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

bool MenuData::CanExecute(Object* target, Object* param)
{
    return true;
}

void MenuData::Execute(Object* target, Object* param)
{
    if (GetName().Equals("系统设置"))
    {
        static SettingWindow* _setWnd;

        if (NULL == _setWnd)
        {
            _setWnd = new SettingWindow();
            _setWnd->ref();
            suic::XamlReader::LoadUri(NULL, _setWnd, "QQ/Layout/SettingWindow.xml");
        }

        if (_setWnd)
        {
            _setWnd->Show();
            _setWnd->CenterWindow();
        }
    }
    else
    {
        InfoBox::Show(GetName(), _U("点击菜单"), InfoBox::IB_OK);
    }
}

MainWindow::MainWindow()
{
    _iAniNum = 0;
    _userMenu = NULL;
    _mainMenu = NULL;

    _timer = new AssignerTimer();
    _timer->ref();
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::Dispose()
{
    //
    // 是否资源
    //
    if (NULL != _userMenu)
    {
        _userMenu->unref();
    }

    if (NULL != _mainMenu)
    {
        _mainMenu->unref();
    }

    _timer->unref();
    _chatSessionMana.RemoveAll();
}

void MainWindow::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    suic::Window::OnMouseLeftButtonDown(e);
}

bool MainWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void MainWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void MainWindow::OnClickHyper(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    Element* pElem = suic::DynamicCast<Element>(e->GetOriginalSource());
    if (pElem != NULL && pElem->GetName().Equals("hyper"))
    {
        suic::InfoBox::Show("Click Hyperlink", "提示");
    }

    e->SetHandled(true);
}

void MainWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    InitBusiness();
    UpdateArrange();

    suic::TextBlock* pTxt = FindElem<suic::TextBlock>("txtBlock");
    if (NULL != pTxt)
    {
        pTxt->AddHandler(suic::Hyperlink::ClickEvent, new suic::RoutedEventHandler(this, &MainWindow::OnClickHyper));
    }

    //
    // 载入登录界面，窗口类使用我们定义的LoginWindow
    //
    _login = suic::XamlReader::LoadUri(NULL, new LoginWindow(), "QQ/Layout/LoginWindow.xml").get();

    if (_login)
    {
        HWND hwnd = HANDLETOHWND(HwndHelper::GetHostHwnd(this));
        //::ShowWindow(hwnd, SW_HIDE);

        //
        // 以模态对话框显示
        //
        _login->ShowDialog();

        if (!_login->IsLogin())
        {
            //
            // 如果登录不成功，关闭窗口，第二个参数传true表示采用异步方式关闭
            //
            //HwndHelper::CloseWindow(this, true);
            _login->AsyncClose();
            return;
        }
        //::ShowWindow(hwnd, SW_SHOW);
        //InvalidateVisual();
    }

    // 
    // 窗口居中显示
    //
    CenterWindow(HwndHelper::GetHostHwnd(_login.get()));
}

void MainWindow::InitBusiness()
{
    //
    // 初始化测试用户列表
    //
    InitUserIds();
    InitUserInfo();

    //
    // 像头像控件注册鼠标进入和离开事件，用来检测用户信息提示窗口关闭
    //
    Element* userInfo = FindElem<Element>(_U("HeadImg"));
    if (NULL != userInfo)
    {
        userInfo->AddMouseEnter(new MouseButtonEventHandler(this, &MainWindow::OnMouseEnterUserInfo), true);
        userInfo->AddMouseLeave(new MouseButtonEventHandler(this, &MainWindow::OnMouseLeaveUserInfo), true);
    }

    //
    // 挂接换肤按钮事件，点击后弹出换肤窗口
    //
    Button* themeBtn = FindElem<Button>(_U("ThemeBtn"));
    if (NULL != themeBtn)
    {
        themeBtn->AddClick(new ClickEventHandler(this, &MainWindow::OnThemeClick));
    }

    //
    // 接收窗口所有按钮点击事件，用来处理菜单上的按钮点击
    // 这里也可以直接在菜单对象上注册
    //
    Button* menuBtn = FindElem<Button>(_U("MainMenuBtn"));
    if (NULL != menuBtn)
    {
        menuBtn->AddClick(new ClickEventHandler(this, &MainWindow::OnMainMenuClick));
    }

    //
    // 点击一般设置按钮时弹出下拉菜单
    // 这里演示了菜单的用户数据绑定功能
    //
    ButtonBase* userSet = FindElem<ButtonBase>(_U("UserSet"));
    if (NULL != userSet)
    {
        userSet->AddClick(new ClickEventHandler(this, &MainWindow::OnUserSet));
        _userMenu = new suic::ContextMenu();
        _userMenu->ref();

        _userMenu->SetPlacementTarget(userSet);
        _userMenu->SetPlacement(PlacementMode::pmBottom);

        _userMenu->AddChild(new MenuData("我在线上", FindRes("imonline_icon")));
        _userMenu->AddChild(new MenuData("Q我吧", FindRes("Qme_icon")));
        _userMenu->AddChild(new MenuData("离开", FindRes("away_icon")));
        _userMenu->AddChild(new MenuData("忙碌", FindRes("busy_icon")));
        _userMenu->AddChild(new MenuData("请勿打扰", FindRes("mute_icon")));
        _userMenu->AddChild(new MenuData("隐身", FindRes("invisible_icon")));
        _userMenu->AddChild(new MenuData("离线", FindRes("imoffline_icon")));
        _userMenu->AddChild(new MenuData("添加状态信息", NULL));

        _userMenu->AddChild(new suic::Separator());

        _userMenu->AddChild(new MenuData("关闭声音", NULL));
        _userMenu->AddChild(new MenuData("关闭头像闪动", NULL));

        _userMenu->AddChild(new suic::Separator());

        _userMenu->AddChild(new MenuData("锁定QQ CTRL + ALT + L ", FindRes("lock20_icon")));

        _userMenu->AddChild(new suic::Separator());

        _userMenu->AddChild(new MenuData("系统设置", NULL));
        _userMenu->AddChild(new MenuData("我的资料", NULL));
        _userMenu->AddChild(new MenuData("我的QQ中心", NULL));
    }

    // 
    // 统一注册Button的路由事件，这可以接收树控件下所有的按钮点击事件
    //
    TreeView* tv = DynamicCast<TreeView>(FindName(_U("UserIds")));
    if (NULL != tv)
    {
        tv->AddHandler(Button::ClickEvent, new ClickEventHandler(this, &MainWindow::OnButtonClicked));
    }

    //
    // 下面演示头像的动画（放大缩小、透明度渐变、旋转组合动画）
    //
    Element* fe = FindElem<Element>(_U("HeadImg"));
    StartHeadImgAnimate(fe);
}

void MainWindow::StartHeadImgAnimate(Element* fe, bool bScale)
{
    return;
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
            ScaleTransform* transScale = new ScaleTransform();
            DoubleAnimation* dAniX = new DoubleAnimation(0.5, 1, Duration(2000), FillBehavior::fbHoldEnd);
            DoubleAnimation* dAniY = new DoubleAnimation(0.5, 1, Duration(2000), FillBehavior::fbHoldEnd);

            transGrp->Add(transScale);

            dAniX->SetRepeatBehavior(rb);
            dAniX->SetAutoReverse(true);
            
            dAniY->SetRepeatBehavior(rb);
            dAniY->SetAutoReverse(true);

            transScale->BeginAnimation(ScaleTransform::ScaleXProperty, dAniX);
            transScale->BeginAnimation(ScaleTransform::ScaleYProperty, dAniY);
        }

        transGrp->Add(trans);
        
        fe->SetRenderTransform(transGrp);
        fe->SetRenderTransformOrigin(fPoint(0.5f, 0.5f));
        DoubleAnimation* dAni = new DoubleAnimation(0, 360, Duration(2000), FillBehavior::fbHoldEnd);
        
        // 设置动画行为
        dAni->SetRepeatBehavior(rb);
        // 启动动画
        trans->BeginAnimation(RotateTransform::AngleProperty, dAni);

        DoubleAnimation* dhAni = new DoubleAnimation(0.1f, 1.0f, Duration(2000), FillBehavior::fbHoldEnd);

        dhAni->SetRepeatBehavior(rb);
        dhAni->SetAutoReverse(true);
        dhAni->SetAccelerationRatio(0.8f);
        fe->BeginAnimation(OpacityProperty, dhAni);
    }
}

void MainWindow::OnThemeClick(Element* sender, RoutedEventArg* e)
{
    if (_themeWnd->IsClosed())
    {
        _themeWnd = XamlReader::LoadUri(NULL, new ThemeWindow(), "QQ/Layout/ThemeWindow.xml").get();

        if (_themeWnd)
        {
            _themeWnd->Show(this);
        }
    }
}

void MainWindow::OnMenuButtonClick(Element* sender, RoutedEventArg* e)
{
    suic::ContextMenu::CloseContextMenu();
    suic::Element* pBtn = DynamicCast<suic::Element>(e->GetOriginalSource());
    if (pBtn->GetName().Equals("SwitchUser"))
    {
        if (_login)
        {
            _login->ShowDialog();
        }
        e->SetHandled(true);
    }
    else if (pBtn->GetName().Equals("EditPwd"))
    {
        if (_login)
        {
            Hide();
            _login->ShowDialog();
            Show();
        }
        e->SetHandled(true);
    }
    else if (pBtn->GetName().Equals("ExitApp"))
    {
        HwndHelper::CloseWindow(this, true);
    }
}

void MainWindow::OnMainMenuClick(Element* sender, RoutedEventArg* e)
{
    if (NULL ==_mainMenu)
    {
        suic::ControlTemplate* menuTemp = DynamicCast<suic::ControlTemplate>(FindRes("MainMenuTemp"));
        if (menuTemp)
        {
            suic::AutoObj root = menuTemp->LoadContent(NULL);
            if (root.get() != NULL)
            {
                _mainMenu = DynamicCast<suic::ContextMenu>(root.get());
                if (NULL != _mainMenu)
                {
                    _mainMenu->ref();
                    _mainMenu->AddHandler(suic::ButtonBase::ClickEvent, new suic::ClickEventHandler(this, &MainWindow::OnMenuButtonClick));

                    _mainMenu->SetPlacementTarget(sender);
                    _mainMenu->SetPlacement(PlacementMode::pmTop);
                }
            }
        }
    }
    
    if (NULL != _mainMenu)
    {
        _mainMenu->TrackContextMenu();
    }
}

void MainWindow::OnUserSet(Element* sender, RoutedEventArg* e)
{
    CheckBox* checkBox = RTTICast<CheckBox>(sender);
    if (NULL != checkBox)
    {
        if (NULL != _userMenu)
        {
            _userMenu->TrackContextMenu();
        }
        checkBox->SetChecked(Boolean::False);
    }
}

void MainWindow::OnButtonClicked(Element* sender, RoutedEventArg* e)
{
    Element* btn = DynamicCast<Element>(e->GetOriginalSource());
    if (btn && btn->GetName().Equals(_U("_tv_qq_group")))
    {
        TreeView* tv = DynamicCast<TreeView>(FindName(_U("UserIds")));
        TreeViewItem* tvItem = DynamicCast<TreeViewItem>(btn->GetUserTag(0));
        if (NULL != tvItem && NULL != tv)
        {
            tvItem->SetIsExpanded(false);
            tv->UpdateLayout();
        }
        e->SetHandled(true);
    }
}

void MainWindow::OnTreeKeydown(Element* sender, KeyboardEventArg* e)
{
    TreeView* tv = DynamicCast<TreeView>(sender);
    if (tv && e->GetKey() == Key::kSpace)
    {
        UserUseId* obj = DynamicCast<UserUseId>(tv->GetSelectedItem());

        if (obj)
        {
            // 建立会话
            _chatSessionMana.OpenChatSession(obj);
        }

        e->SetHandled(true);
    }
}

void MainWindow::OnDblTreeClick(Element* sender, MouseButtonEventArg* e)
{
    //
    // 鼠标双击事件
    //
    Element* elem = DynamicCast<Element>(e->GetOriginalSource());
    UserUseId* obj = DynamicCast<UserUseId>(elem->GetDataContext());

    if (obj)
    {
        // 建立会话
        _chatSessionMana.OpenChatSession(obj);
    }

    e->SetHandled(true);
}

void MainWindow::OnTreeScrollChanged(Element* sender, ScrollChangedEventArg* e)
{
    TreeView* tv = DynamicCast<TreeView>(sender);
    if (tv)
    {
        ButtonBase* grpElem = DynamicCast<ButtonBase>(tv->GetTemplateChild(_U("_tv_qq_group")));
        Element* visualElem = tv->GetItemsHost()->GetVisibleChild(0);
        TreeViewItem* tvItem = DynamicCast<TreeViewItem>(visualElem);

        if (tvItem->GetVisualOffset().y <= -16 && tvItem->IsExpanded())
        {
            grpElem->SetVisibility(Visibility::Visible);
            grpElem->SetContent(tvItem->GetDataContext());
            grpElem->SetUserTag(0, tvItem);
        }
        else if (!tvItem->IsExpanded() || tvItem->GetVisualOffset().y >= 0)
        {
            grpElem->SetVisibility(Visibility::Collapsed);
            grpElem->SetContent(NULL);
            grpElem->SetTag(NULL);
        }
    }
}

void MainWindow::OnCheckUserWndTimer(Object* sender, EventArg* e)
{
    Element* userInfo = FindElem<Element>(_U("HeadImg"));

    if (!_userWnd || !_userWnd->IsValid() || 
        (!PointTransform::PointInElement(_userWnd.get(), NULL) && 
        !PointTransform::PointInElement(userInfo, NULL)))
    {
        _timer->SetEnabled(false);
        _userWnd->Close();
        _userWnd = NULL;
    }
}

void MainWindow::OnMouseEnterUserInfo(Element* sender, MouseButtonEventArg* e)
{
    if (_userWnd.get() == NULL)
    {
        _userWnd = XamlReader::LoadUri(NULL, new UserInfoWindow(UserManager::Ins()->GetCurrentUser()), "QQ/Layout/UserInfoWindow.xml").get();
        if (_userWnd)
        {
            Size bounds = Environment::GetScreenBound();
            Point pt = PointTransform::GetRootScreenPos(this);
            Element* userInfo = FindElem<Element>(_U("HeadImg"));

            int left = pt.x + GetActualWidth();
            int top = pt.y + userInfo->PointToScreen(Point()).y;

            if (left + _userWnd->GetWidth() > bounds.cx)
            {
                left = pt.x - _userWnd->GetWidth();
            }

            _userWnd->SetPos(left, top);
            _userWnd->Show(this);

            //
            // 启动定时器侦测鼠标离开时关闭用户信息对话框
            //
            _timer->SetInterval(500);
            _timer->SetTick(EventHandler(this, &MainWindow::OnCheckUserWndTimer));
            _timer->Start();
        }
    }
    e->SetHandled(true);
}

void MainWindow::OnMouseLeaveUserInfo(Element* sender, MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

void MainWindow::InitUserInfo()
{

}

void MainWindow::InitUserIds()
{
    suic::TreeView* tree = FindElem<suic::TreeView>(_U("UserIds"));
    if (tree != NULL)
    {
        //
        // 查找资源中定义的用户分组数据模版 
        //
        suic::DataTemplate* userGrp = DynamicCast<suic::DataTemplate>(tree->FindRes(_U("UserGroupTemp")));

        //
        // 查找资源中定义的用户信息数据模版
        //
        suic::DataTemplate* userIdTemp = DynamicCast<suic::DataTemplate>(tree->FindRes(_U("UserIdTemp")));

        //
        // 设置树项模版选择器（系统根据用户分组或用户信息选择对应的数据模版）
        //
        tree->SetItemTemplateSelector(new UserTemplateSelector(userGrp, userIdTemp));

        //
        // 注册滚动事件
        //
        tree->AddHandler(suic::ScrollViewer::ScrollChangedEvent, new ScrollChangedEventHandler(this, &MainWindow::OnTreeScrollChanged));
        
        //
        // 注册鼠标预览时间（需要处理树子项的双击事件）
        //
        tree->AddMouseDoubleClick(new MouseButtonEventHandler(this, &MainWindow::OnDblTreeClick));

        //
        // 注册键盘事件处理
        //
        tree->AddKeyDown(new KeyboardEventHandler(this, &MainWindow::OnTreeKeydown));

        // 设置好友信息数据源
        tree->SetItemsSource(UserManager::Ins()->GetFriendGroup());
    }
}