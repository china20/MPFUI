/////////////////////////////////////////////////////
// ChatWindow.cpp

#include "stdafx.h"
#include "ChatWindow.h"

bool IsXPOs()
{
    OSVERSIONINFO osver;
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osver);

    if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)
    {
        return true;
    }
    else
    {
        return true;//false;
    }
}

OSWindow::OSWindow()
{

}

OSWindow::~OSWindow()
{

}

void OSWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);

    if (IsXPOs())
    {
        suic::Style* pStyle = dynamic_cast<suic::Style*>(FindRes("Window_XP"));
        if (NULL != pStyle)
        {
            SetStyle(pStyle);
        }
    }
}

ChatWindow::ChatWindow()
{
    _sendImgMenu = NULL;
    _timer = new AssignerTimer();
    _timer->ref();
    _bFontSetOpen = false;
    _fontSetInfo = NULL;
    _chatTextBox = NULL;
    _userInfo = NULL;
}

ChatWindow::~ChatWindow()
{
    _timer->unref();

    if (NULL != _fontSetInfo)
    {
        _fontSetInfo->unref();
    }
    if (NULL != _sendImgMenu)
    {
        _sendImgMenu->unref();
    }
}

ChatTextBox* ChatWindow::GetChatTextBox()
{
    if (NULL == _chatTextBox)
    {
        _chatTextBox = FindElem<ChatTextBox>(_U("ChatList"));
    }
    return _chatTextBox;
}

void ChatWindow::SendMsg()
{
    suic::RichTextBox* pText = FindElem<suic::RichTextBox>(_U("InputText"));
    suic::RichTextBox* pList = FindElem<suic::RichTextBox>(_U("ChatList"));

    if (NULL != pList && NULL != pText)
    {
        String strText = pText->GetTextDoc()->GetText();

        if (!strText.Empty())
        {
            ChatItem* pInfo = new ChatItem(_userInfo);
 
            //------------------------------------------------
            /*suic::ImageEmbbed* img = new suic::ImageEmbbed();
            img->Load("d:\\2.gif");
            pText->GetTextDoc()->SelectEnd();
            pText->GetTextDoc()->InsertEmbbedAtSel(img);*/
            //------------------------------------------------

            // 测试，我们随机模拟对话
            int iRand = rand() % 2;
            if (iRand != 0)
            {
                pInfo->SetAlignFlag(eAlignFlag::eAlignLeft);
                pInfo->SetMessageBk(dynamic_cast<suic::Brush*>(FindRes("user_bg_nor")));
            }
            else
            {
                pInfo->SetAlignFlag(eAlignFlag::eAlignRight);
                pInfo->SetMessageBk(dynamic_cast<suic::Brush*>(FindRes("friend_left_bg")));
            }

            pText->GetTextDoc()->GetStream(pInfo->GetChatMsg(), false);
            GetChatTextBox()->AddChatItem(pInfo);
        }

        pText->GetTextDoc()->GetTextPara()->SetIndents(0, 0, 0);
        _fontSetInfo->Reset();
        pText->GetTextDoc()->SetText(_U(""), false);
        pText->Focus();
        KeyboardNavigation::Current()->HideFocusVisual();
    }
}

bool ChatWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    if (pNode->GetName().Equals(_U("ChatList")))
    {
        obj = new ChatTextBox();
        return true;
    }
    else
    {
        return false;
    }
}

void ChatWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void ChatWindow::OnClickButton(Element* sender, RoutedEventArg* e)
{
    ElementPtr elem(e->GetOriginalSource());
    if (elem->GetName().Equals(_U("SendBtn")))
    {
        SendMsg();
        e->SetHandled(true);
    }
    else if (elem->GetName().Equals(_U("CloseBtn")))
    {
        Close();
        e->SetHandled(true);
    }
}

void ChatWindow::OnSendImageClick(Element* sender, RoutedEventArg* e)
{
    // 为空，动态生成菜单信息
    if (NULL == _sendImgMenu)
    {
        _sendImgMenu = new ContextMenu();
        _sendImgMenu->AddChild(new OString(_U("从本地文件发送")));

        _sendImgMenu->ref();

        _sendImgMenu->SetPlacementTarget(sender);
        _sendImgMenu->SetPlacement(PlacementMode::pmBottom);
    }
    
    if (NULL != _sendImgMenu)
    {
        _sendImgMenu->TrackContextMenu();
    }
}

void ChatWindow::OnMouseEnterRightPanel(Element* sender, MouseButtonEventArg* e)
{
    Element* pSplitBtn = FindName(_U("ShowAndHideRightPanel"));
    if (pSplitBtn && !pSplitBtn->IsVisible())
    {
        pSplitBtn->SetVisibility(Visibility::Visible);
        _timer->SetInterval(100);
        _timer->SetTick(EventHandler(this, &ChatWindow::OnCheckSplitButtonTimer));
        _timer->Start();
    }
}

void ChatWindow::OnCheckSplitButtonTimer(Object* sender, EventArg* e)
{
    Element* pRightPanel = FindName(_U("RightPanel"));
    Element* pSplitBtn = FindName(_U("ShowAndHideRightPanel"));

    if ((!PointTransform::PointInElement(pRightPanel, NULL) && 
        !PointTransform::PointInElement(pSplitBtn, NULL)))
    {
        _timer->SetEnabled(false);
        pSplitBtn->SetVisibility(Visibility::Collapsed);
    }
}

void ChatWindow::OnClickSplitButton(Element* sender, RoutedEventArg* e)
{
    suic::StylePtr style;
    suic::DoubleAnimation* widAni = NULL;
    FrameworkElement* pRightPanel = FindElem<FrameworkElement>(_U("RightPanel"));
    FrameworkElement* pSplitBtn = FindElem<FrameworkElement>(_U("SplitBtn"));

    if (pRightPanel->GetWidth() > 20)
    {
        style = FindRes(_U("SplitLeftStyle"));
        widAni = new suic::DoubleAnimation(180, 2, Duration(200), FillBehavior::fbHoldEnd);
    }
    else
    {
        style = FindRes(_U("SplitRightStyle"));
        widAni = new suic::DoubleAnimation(2, 180, Duration(200), FillBehavior::fbHoldEnd);
    }

    pSplitBtn->SetStyle(style.get());
    pRightPanel->BeginAnimation(FrameworkElement::WidthProperty, widAni);

    e->SetHandled(true);
}

void ChatWindow::OnClickFontSet(Element* sender, RoutedEventArg* e)
{
    FrameworkElement* pElem = FindElem<FrameworkElement>("FontSet");
    if (pElem != NULL)
    {
        pElem->SetMargin(Rect(0, -32, 0, 32));
        pElem->SetVisibility(Visibility::Visible);

        suic::RectAnimation* rectAni = NULL;
        
        if (_bFontSetOpen)
        {
            _bFontSetOpen = false;
            rectAni = new suic::RectAnimation(Rect(), Rect(0, -32, 0, 32), Duration(300), FillBehavior::fbHoldEnd);
        }
        else
        {
            _bFontSetOpen = true;
            rectAni = new suic::RectAnimation(Rect(0, -32, 0, 32), Rect(), Duration(300), FillBehavior::fbHoldEnd);
        }
        pElem->BeginAnimation(FrameworkElement::MarginProperty, rectAni);
    }
}

void ChatWindow::OnClickSwitchChatMode(Element* sender, RoutedEventArg* e)
{
    ChatTextBox* pElem = FindElem<ChatTextBox>("ChatList");
    suic::CheckBox* checkBox = suic::DynamicCast<suic::CheckBox>(sender);

    if (pElem != NULL && checkBox != NULL)
    {
        ChatTextContainer* textContainer = pElem->GetChatTextContainer();
        if (!checkBox->IsChecked())
        {
            textContainer->UpdateDisplayMode(eDisplayMode::eDisplayBubble);
        }
        else
        {
            textContainer->UpdateDisplayMode(eDisplayMode::eDisplayNormal);
        }
    }
}

void ChatWindow::OnPreviewKeyDown(suic::KeyboardEventArg* e)
{
    if (e->GetKey() == Key::kReturn)
    {
        e->SetHandled(true);
        if (!e->IsAltDown())
        {
            SendMsg();
        }
        else
        {
            String strText;
            suic::RichTextBox* pText = FindElem<suic::RichTextBox>(_U("InputText"));
            
            if (NULL != pText)
            {
                pText->GetTextDoc()->SelectEnd();
                pText->GetTextDoc()->AddText(_U("\n"), false);
            }
        }
    }
}

void ChatWindow::OnClosing(CancelEventArg* e)
{
    Window::OnClosing(e);
}

void ChatWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    // 窗口居中显示
    CenterWindow();

    //
    // 设置数据源，用户信息绑定到聊天窗口显示
    //
    ContentControl* userInfo = FindElem<ContentControl>(_U("UserInfoContent"));
    if (NULL != userInfo)
    {
        userInfo->SetContent(_userInfo);
    }

    Button* sendImg = FindElem<Button>(_U("SendImgBtn"));
    if (sendImg)
    {
        sendImg->AddClick(new ClickEventHandler(this, &ChatWindow::OnSendImageClick));
    }

    //
    // 窗口分割按钮事件注册
    //
    Button* splitBtn = FindElem<Button>(_U("SplitBtn"));
    if (splitBtn)
    {
        splitBtn->AddClick(new ClickEventHandler(this, &ChatWindow::OnClickSplitButton));
    }
    
    //
    // 通过路由事件的路由功能，由父窗口统一处理所有未处理的按钮点击事件
    //
    AddHandler(Button::ClickEvent, new ClickEventHandler(this, &ChatWindow::OnClickButton));

    CheckBox* fontSet = FindElem<CheckBox>("FontSetBtn");
    if (NULL != fontSet)
    {
        fontSet->AddClick(new ClickEventHandler(this, &ChatWindow::OnClickFontSet));
    }

    CheckBox* switchBox = FindElem<CheckBox>("SwitchChatMode");
    if (NULL != switchBox)
    {
        switchBox->AddClick(new ClickEventHandler(this, &ChatWindow::OnClickSwitchChatMode));
    }
    
    // 绑定我们的字体信息到字体设置界面
    // 这样，当界面操作字体信息变化时我们定义的
    // 字体信息会同步更新
    Element* fontSetPanel = FindName(_U("FontSetPanel"));
    if (NULL != fontSetPanel)
    {
        suic::RichTextBox* pInputText = FindElem<suic::RichTextBox>(_U("InputText"));
        _fontSetInfo = new FontSettingInfo(pInputText);
        _fontSetInfo->ref();
        fontSetPanel->SetDataContext(_fontSetInfo);
        //fontSetPanel->InvalidatePropDeps(suic::Element::DataContextProperty, true);
    }

    //
    // 鼠标进入右边面板时显示分割条按钮
    //
    Element* pRightPanel = FindName(_U("RightPanel"));
    if (NULL != pRightPanel)
    {
        pRightPanel->AddMouseEnter(new MouseButtonEventHandler(this, &ChatWindow::OnMouseEnterRightPanel), true);
    }
}
