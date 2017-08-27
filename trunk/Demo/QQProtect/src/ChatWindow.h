// 华勤科技版权所有 2010-2022
// 
// 文件名：ChatWindow.h
// 功  能：聊天界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _CHATWINDOW_H_
#define _CHATWINDOW_H_

#include "UserManager.h"
#include "ChatSession.h"
#include <Framework/Controls/Menu.h>
#include <Extend/Editor/TextContainer.h>

using namespace suic;

class FontSettingInfo : public NotifyPropChanged
{
public:

    BeginMember(FontSettingInfo, NotifyPropChanged)
        MemberString(FontFamily)
        MemberString(FontSize)
        MemberObject(FontBold)
        MemberObject(FontItalic)
        MemberObject(FontUnderline)
    EndMember()

    FontSettingInfo(suic::RichTextBox* inputBox)
    {
        _fontFamily = _U("宋体");
        _fontSize = _U("10");
        _bold = Boolean::False;
        _italic = Boolean::False;
        _underline = suic::TextDecorations::none;
        _input = inputBox;
    }

    String GetFontFamily()
    {
        return _fontFamily;
    }

    void SetFontFamily(String val)
    {
        _fontFamily = val;
    }

    String GetFontSize()
    {
        return _fontSize;
    }

    void SetFontSize(String val)
    {
        _fontSize = val;
        if (NULL != _input)
        {
            _input->SetFontSize(_fontSize.ToInt());
            _input->Focus();
        }
    }

    Object* GetFontBold()
    {
        return _bold;
    }

    void SetFontBold(Object* val)
    {
        _bold = DynamicCast<Boolean>(val);
        if (NULL != _input)
        {
            if (_bold != Boolean::False)
            {
                _input->SetFontWeight(FontWeightStyle::fwExtraBold);
            }
            else
            {
                _input->SetFontWeight(FontWeightStyle::fwNormal);
            }
            _input->Focus();
        }
    }

    Object* GetFontItalic()
    {
        return _italic;
    }

    void SetFontItalic(Object* val)
    {
        _italic = DynamicCast<Boolean>(val);
        if (NULL != _input)
        {
            if (_italic != Boolean::False)
            {
                _input->SetFontStyle(FontStyleStyle::fssItalic);
            }
            else
            {
                _input->SetFontStyle(FontStyleStyle::fssNormal);
            }
            _input->Focus();
        }
    }

    Object* GetFontUnderline()
    {
        if (_underline == suic::TextDecorations::underline)
        {
            return suic::Boolean::True;
        }
        else
        {
            return suic::Boolean::False;
        }
    }

    void SetFontUnderline(Object* val)
    {
        if (val == suic::Boolean::True)
        {
            _underline = suic::TextDecorations::underline;
        }
        else
        {
            _underline = suic::TextDecorations::none;
        }

        if (NULL != _input)
        {
            if (_underline == suic::TextDecorations::underline)
            {
                _input->SetTextDecorations(TextDecorations::underline);
            }
            else
            {
                _input->SetTextDecorations(TextDecorations::none);
            }
            _input->Focus();
        }
    }

    void Reset()
    {
        if (NULL != _input)
        {
            _input->SetFontFamily(_fontFamily);
            _input->SetFontSize(_fontSize.ToInt());
            _input->SetFontWeight(_bold->ToBool() ? FontWeightStyle::fwExtraBold : FontWeightStyle::fwNormal);

            if (_italic != Boolean::False)
            {
                _input->SetFontStyle(FontStyleStyle::fssItalic);
            }
            else
            {
                _input->SetFontStyle(FontStyleStyle::fssNormal);
            }
        }
    }

private:

    String _fontFamily;
    String _fontSize;
    Boolean* _bold;
    Boolean* _italic;
    TextDecorationCollection* _underline;

    suic::RichTextBox* _input;
};

class BubbleChatItem : public Object
{
public:

    virtual Size GetHeaderSize() = 0;

    virtual void DrawHeader(suic::Drawing* drawing, suic::Rect rect) = 0;
    virtual void DrawMessageBk(suic::Drawing* drawing, suic::Rect rect) = 0;

    virtual void DrawTitle(suic::ITextDoc* txtDoc) = 0;
    virtual void DrawMessage(suic::ITextDoc* txtDoc) = 0;

private:

    // 聊天项最大宽度
    int _iWidth;
};

bool IsXPOs();

class OSWindow : public suic::Window
{
public:

    OSWindow();
    ~OSWindow();

    void OnInitialized(EventArg* e);
};

class ChatWindow : public OSWindow
{
public:

    ChatWindow();
    ~ChatWindow();

    ChatTextBox* GetChatTextBox();

    void SetUserInfo(UserUseId* userInfo)
    {
        _userInfo = userInfo;
    }

    void OnLoaded(suic::LoadedEventArg* e);

    void OnClickSplitButton(Element* sender, RoutedEventArg* e);
    void OnClickButton(Element* sender, RoutedEventArg* e);
    void OnClickFontSet(Element* sender, RoutedEventArg* e);
    void OnClickSwitchChatMode(Element* sender, RoutedEventArg* e);
    void OnSendImageClick(Element* sender, RoutedEventArg* e);

    void OnMouseEnterRightPanel(Element* sender, MouseButtonEventArg* e);
    void OnCheckSplitButtonTimer(Object* sender, EventArg* e);

protected:

    void SendMsg();

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

    // 通过预览事件最先截获键盘事件
    void OnPreviewKeyDown(suic::KeyboardEventArg* e);

    void OnClosing(CancelEventArg* e);

private:

    bool _bFontSetOpen;
    WindowPtr _userWnd;
    
    AssignerTimer* _timer;
    ContextMenu* _sendImgMenu;
    FontSettingInfo* _fontSetInfo;

    ChatTextBox* _chatTextBox;

    UserUseId* _userInfo;
};

#endif
