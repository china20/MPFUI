
#ifndef _CHARMESSAGE_H_
#define _CHARMESSAGE_H_

using namespace suic;

class InfoItem : public Object
{
public:

};

// 管理聊天信息
class ChatItem : public InfoItem
{
public:

    ChatItem()
        : _sendState(0)
        , _msgPos(0)
        , _sendState(0)
    {
    }

    suic::Byte GetSendState() const
    {
        return _sendState;
    }

    void SetSendState(suic::Byte val)
    {
        _sendState = val;
    }

    suic::Uint32 GetMessagePos() const
    {
        return _msgPos;
    }

    void SetMessagePos(suic::Uint32 val)
    {
        _msgPos = val;
    }

    suic::String GetUserId() const
    {
        return _userId;
    }

    void SetUserId(suic::String val)
    {
        _userId = val;
    }

    suic::Uint32 GetMessageDate() const
    {
        return _msgDate;
    }

    void SetMessageDate(suic::Uint32 val)
    {
        _msgDate = val;
    }

    suic::ByteStream* GetMessageStream()
    {
        return &_message;
    }

private:

    // 消息发送状态0:成功
    suic::Byte _sendState;
    // 消息在聊天内容开始位置所有
    // 此属性每次加载消息内容会动态改变
    suic::Uint32 _msgPos;
    // 用户ID
    suic::String _userId;
    // 消息发送时间
    suic::Uint32 _msgDate;
    // 消息内容
    suic::ByteStream _message;
};

class ChatMessage
{
public:

    ChatMessage()
    {
    }

    int GetCount() const
    {
        return _msgCache.GetCount();
    }

    InfoItem* GetItem(int index) const
    {
        return DynamicCast<InfoItem>(_msgCache.GetItem(index));
    }

    void AddItem(InfoItem* item)
    {
        _msgCache.Add(item);
    }

private:

    // 消息缓存
    suic::XArray _msgCache;
};

#endif
