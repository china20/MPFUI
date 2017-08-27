// 华勤科技版权所有 2008-2022
// 
// 文件名：uihwndadapter.h
// 功  能：封装windows的句柄。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIHWNDADAPTER_H_
#define _UIHWNDADAPTER_H_

#include <System/Windows/Object.h>
#include <System/Types/Structure.h>

namespace suic
{

struct SUICORE_API HwndParam
{
    Handle parent;
    Handle hicon;

    Uint32 style;
    Uint32 exstyle;
    Uint32 state;
    Uint32 classStyle;

    Int32 x;
    Int32 y;

    Int32 width;
    Int32 height;

    HwndParam();
};

enum MessageType
{
    UIM_LBDOWN = 0x0201,
    UIM_LBUP = 0x0202,
    UIM_RBDOWN = 0x0204,
    UIM_RBUP = 0x0205,
    UIM_MBDOWN = 0x020B,
    UIM_MBUP = 0x020C,
    UIM_CONTEXTMENU = 0x007B,
    UIM_MOUSEMOVE = 0x0200,
    UIM_KEYDOWN = 0x0100,
    UIM_KEYUP = 0x0101,
    UIM_CHAR = 0x0102,
    UIM_IMCHAR = 0x0286,
    UIM_SETCURSOR = 0x0020,
    UIM_HOTKEY = 0x0312,
    UIM_GOTFOCUS = 0x0007,
    UIM_LOSTFOCUS = 0x0008,
};

struct SUICORE_API MessageParam
{
    Handle hwnd;
    Int32 message;
    Uint32 wp;
    Uint32 lp;
    Point point;
    Uint32 result;
    Uint32 timestamp;

    MessageParam();

};

typedef delegate<bool(Object*, MessageParam*, bool&)> MessageHook;
class HookItem : public Object
{
public:

    Object* owner;
    MessageHook mf;

    HookItem(Object* o, const MessageHook& m);
};

class HookInfo
{
public:

    HookInfo();
    ~HookInfo();

    void Clear();

    int GetCount() const;
    HookItem* GetHookItem(int index);

    void RemoveHookFromObject(Object* obj);
    void RegisterHook(Object* obj, MessageHook& hook);
    void RemoveHook(Object* obj, MessageHook& hook);

    bool DoMessageHook(Object* root, MessageParam* mp, bool& interrupt);

private:

    Array<HookItem*> _hooks;
};

class HwndAdapter : public Object
{
public:

    HwndAdapter(HwndParam hp);
    HwndAdapter();
    ~HwndAdapter();

    bool OnFilterMessage(Object* sender, MessageParam* mp);

    Handle GetHwnd();
    void SetHwnd(Handle hwnd);

    bool BuildHwnd(Object* param);
    bool BuildHwnd(HwndParam hp, Object* param);

    void Close(bool bAsync);
    void Clear();

protected:

    Handle _value;
    HwndParam _hp;
};

}

#endif
