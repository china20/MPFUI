// 华勤科技版权所有 2008-2022
// 
// 文件名：ItemsChangedEventArg.h
// 功  能：定义核心库的子项事件类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
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
/// 子项变化事件
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

typedef delegate<void(Object*, NotifyCollChangedEventArg*)> NotifyCollChangedEventHandler;

}

#endif
