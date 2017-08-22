
#include "stdafx.h"
#include "ChatSessionManager.h"

ChatSessionItem::ChatSessionItem()
{
    chatSession = NULL;
    chatWindow = new ChatWindow();
    chatWindow->ref();
}

ChatSessionItem::~ChatSessionItem()
{
    chatWindow->unref();

    if (chatSession != NULL)
    {
        chatSession->unref();
    }
}

ChatSessionManager::ChatSessionManager()
    : Object(false)
    , _operPending(false)
{
}

ChatSessionManager::~ChatSessionManager()
{
    RemoveAll();
}

bool ChatSessionManager::OpenChatSession(UserUseId* pUserInfo)
{
    ChatSessionItem* chatItem = new ChatSessionItem();

    _chatSessions.Add(chatItem);
    XamlReader::LoadUri(NULL, chatItem->chatWindow, "QQ/Layout/ChatWindow.xml");

    chatItem->chatWindow->Closing += EventHandler(this, &ChatSessionManager::OnClosingChatWindow);
    chatItem->chatWindow->SetUserInfo(pUserInfo);
    chatItem->chatWindow->Show();

    //
    // 初始化ChatSession到显示控件
    //
    ChatTextBox* pChatListBox = chatItem->chatWindow->GetChatTextBox();

    if (NULL != pChatListBox)
    {
        chatItem->chatSession = new ChatSession(pUserInfo->GetUserNumber());
        chatItem->chatSession->ref();

        pChatListBox->GetChatTextContainer()->SetChatSession(chatItem->chatSession);
    }

    return true;
}

void ChatSessionManager::OnClosingChatWindow(Object* sender, EventArg* e)
{
    ChatWindow* chatWnd = dynamic_cast<ChatWindow*>(sender);
    RemoveChatWindow(chatWnd);
}

void ChatSessionManager::RemoveAll()
{
    _operPending = true;
    for (int i = 0; i < _chatSessions.GetCount(); ++i)
    {
        ChatSessionItem* chatItem = _chatSessions[i];

        chatItem->chatWindow->Close();
        delete chatItem;
    }
    _chatSessions.Clear();
    _operPending = false;
}

void ChatSessionManager::RemoveChatWindow(ChatWindow* pWindow)
{
    for (int i = 0; i < _chatSessions.GetCount(); ++i)
    {
        ChatSessionItem* chatItem = _chatSessions[i];

        if (chatItem->chatWindow == pWindow)
        {
            if (!_operPending)
            {
                delete chatItem;
                _chatSessions.RemoveAt(i);
            }
            break;
        }
    }
}
