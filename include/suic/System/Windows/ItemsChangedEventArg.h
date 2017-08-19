// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ItemsChangedEventArg.h
// ��  �ܣ�������Ŀ�������¼��ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIITEMCHANGEDEVENTARG_H_
#define _UIITEMCHANGEDEVENTARG_H_

#include <System/Windows/ItemEntry.h>

namespace suic
{

struct SUICORE_API NotifyCollectionChangedAction
{
    enum
    {
        Add = 0,
        Insert = 1,
        Remove = 2,
        Replace = 3,
        Move = 4,
        Swap = 5,
        Update = 6,
        Reset = 7,
    };
};

/// <summary>
/// ����仯�¼�
/// </summary>
class SUICORE_API ItemsChangedEventArg : public EventArg
{
public:

    ItemsChangedEventArg(int action, int oldPos, int newPos, int count);

    int GetNewPos() { return _newPos; }
    int GetOldPos() { return _oldPos; }
    int GetCount() { return _count; }
    int GetAction() { return _action; }

protected:

    Int32 _newPos;
    Int32 _oldPos;
    Int32 _count;
    Int32 _action;
};

typedef delegate<void(Object*, ItemsChangedEventArg*)> ItemsChangedEventHandler;

class NotifyCollChangedEventArg : public EventArg
{
public:

    NotifyCollChangedEventArg(int action, ItemEntry* oldValue, int oldIndex, ItemEntry* newValue, int newIndex)
    {
        _action = action;
        _oldValue = oldValue;
        _newValue = newValue;
        _oldStartIndex = oldIndex;
        _newStartIndex = newIndex;
    }

    int GetAction() const
    {
        return _action;
    }

    int GetOldIndex() const
    {
        return _oldStartIndex;
    }

    ItemEntry* GetOldItem() const
    {
        return _oldValue;
    }

    int GetNewIndex() const
    {
        return _newStartIndex;
    }

    ItemEntry* GetNewItem() const
    {
        return _newValue;
    }

private:

    int _action;
    int _oldStartIndex;
    int _newStartIndex;
    ItemEntry* _oldValue;
    ItemEntry* _newValue;
};

/*struct NotifyCollChangedEventHandler
{
    typedef void(*func_type)(Object*, NotifyCollChangedEventArg*);

    union funcCall
    {
        struct func_func
        {
            func_type func;
        } func;
        struct member_func
        {
            func_type func;
            WeakRef target;
        }mfunc;
    };

    bool safe_bool() const 
    {
        return (_funcCall != NULL);
    }

    void operator()(Object* sender, NotifyCollChangedEventArg* e)
    {
        for (int i = 0; i < _funcs.GetCount(); ++i)
        {
            _funcs[i](sender, e);
        }
    }

    void operator+=(const NotifyCollChangedEventHandler& Other)
    {
        _funcCall = Other._funcCall;
    }

    Array<mfunc> _funcs;
};*/

typedef delegate<void(Object*, NotifyCollChangedEventArg*)> NotifyCollChangedEventHandler;

}

#endif
