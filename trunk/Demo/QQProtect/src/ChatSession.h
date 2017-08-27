// 华勤科技版权所有 2010-2022
// 
// 文件名：ChatSession.h
// 功  能：聊天会话管理。
// 
// 作  者：MPF开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _CHATSESSION_H_
#define _CHATSESSION_H_

#include "UserManager.h"
#include <Framework/Controls/Menu.h>
#include <Extend/Editor/TextContainer.h>

enum eChatType
{
    eSingleChat,
    eGroupChat,
    eSessionChat,
};

enum eAlignFlag
{
    eAlignLeft,
    eAlignCenter,
    eAlignRight,
};

enum eDisplayMode
{
    eDisplayNormal,
    eDisplayBubble,
};

class ChatTextBox;

class ChatItem
{
public:

    ChatItem(UserUseId* userInfo);
    ~ChatItem();

    void SetStartCp(int cp);
    void SetMessageCp(int cp);

    void SetEndCp(int cp);
    int GetStartCp();

    int GetMessageCp();
    int GetEndCp();

    int GetMessageWid() const;
    void SetMessageWid(int v);

    eAlignFlag GetAlignFlag() const;
    void SetAlignFlag(eAlignFlag val);

    suic::ByteStream* GetChatMsg()
    {
        return &_chatMsg;
    }

    suic::DateTime& GetChatDate()
    {
        return _chatDate;
    }

    UserUseId* GetUserInfo() const
    {
        return _userInfo;
    }

    //--------------------------------------

    void SetMessageBk(suic::Brush* brush);

    void DrawHeader(suic::Drawing* drawing, suic::Rect rect);
    void DrawMessageBk(suic::Drawing* drawing, suic::Rect rect);

private:

    // 聊天类型（0：个人；1：群；2：讨论组）
    eChatType iChatType;
    // 消息头开始的字符索引
    int _iStartCp;
    // 消息体开始的字符索引
    int _iMessageCp;
    // 消息结束的字符索引
    int _iEndCp;
    int _messageWid;
    // 消息对齐属性
    eAlignFlag _alignFlag;

    UserUseId* _userInfo;

    suic::DateTime _chatDate;
    suic::ByteStream  _chatMsg;

    suic::Brush* _messageBk;
};

class ChatSession : public suic::Object
{
public:

    ChatSession(suic::String sessionId);
    ~ChatSession();

    void Clear();
    void AddChatItem(ChatItem* pInfo);
    int GetCount() const;
    ChatItem* GetChatItem(int index) const;
    int FindFirstVisibleChatItem(int iCp);

    suic::String GetSessionId() const;

    eDisplayMode GetDisplayMode() const;
    bool SetDisplayMode(eDisplayMode displayMode);

private:

    // 显示方式
    eDisplayMode _displayMode;
    // 会话标识
    suic::String _sessionId;
    // 聊天内容
    suic::Array<ChatItem*> _chatItems;
};

class ChatTextContainer : public suic::TextContainer
{
public:

    ChatTextContainer();
    ~ChatTextContainer();

    void AddChatItem(ChatItem* pInfo);
    void AddChatItemToView(ChatItem* pInfo);

    void SetChatSession(ChatSession* chatSession);
    void UpdateDisplayMode(eDisplayMode mode);

protected:

    void AdjustChatItem(ChatItem* pInfo);
    void OnRender(suic::Drawing* drawing);

private:

    suic::ITextDoc* _textDoc;
    // 当前会话内容
    ChatSession* _chatSession;

    friend class ChatTextBox;
};

class ChatTextBox : public suic::RichTextBox
{
public:

    RTTIOfClass(ChatTextBox)

    ChatTextBox();
    ~ChatTextBox();

    void AddChatItem(ChatItem* pInfo);
    ChatTextContainer* GetChatTextContainer();

protected:

    void OnInitialized(suic::EventArg* e);
    
    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);

    suic::TextContainer* CreateTestHost();

private:

    suic::WindowPtr _imageView;
};

#endif
