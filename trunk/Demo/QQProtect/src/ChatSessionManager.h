// 华勤科技版权所有 2010-2022
// 
// 文件名：ChatSessionManager.h
// 功  能：聊天会话管理。
// 
// 作  者：MPF开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _CHATSESSIONMANAGER_H_
#define _CHATSESSIONMANAGER_H_

#include "UserManager.h"
#include "ChatSession.h"
#include "ChatWindow.h"

class ChatSessionItem
{
public:

    ChatWindow* chatWindow;
    ChatSession* chatSession;

    ChatSessionItem();
    ~ChatSessionItem();
};

class ChatSessionManager : public Object
{
public:

    ChatSessionManager();
    ~ChatSessionManager();

    bool OpenChatSession(UserUseId* pUserInfo);

    void RemoveAll();
    void RemoveChatWindow(ChatWindow* pWindow);

    void OnClosingChatWindow(Object* sender, EventArg* e);

private:

    bool _operPending;
    suic::Array<ChatSessionItem*> _chatSessions;
};

#endif
