// 华勤科技版权所有 2008-2022
// 
// 文件名：ItemSource.h
// 功  能：项集合容器。
// 
// 作  者：MPF开发组(汪荣)
// 时  间：2010-07-02
// 
// ============================================================

#ifndef __UIITEMSOURCE_H
#define __UIITEMSOURCE_H

#include <System/Tools/Array.h>
#include <System/Windows/DpObject.h>
#include <Framework/Controls/ItemCollection.h>

namespace suic
{

/// <Summary>
///  实现默认的ItemCollection
/// </Summary>
class SUICORE_API ObservableCollection : public ItemCollection
{
public:

    ObservableCollection();
    ObservableCollection(int capacity);
    ~ObservableCollection();

    bool IsEmpty();

    int GetCount() const;
    bool Contains(Object* item);
    Object* GetItem(int index) const;
    ItemEntry* GetItemEntry(int index) const;

    int IndexOf(Object* value);
    int IndexOf(Object* value, int startIndex);
    int IndexOf(Object* value, int startIndex, int count);

    int AddItem(Object* value);
    void SetItem(int index, Object* value);
    void MoveItem(int oriPos, int dstPos);
    void SwapItem(int index1, int index2);
    void InsertItem(int index, Object* value);

    bool RemoveItem(Object* obj);
    bool RemoveItemAt(int index);
    void RemoveRange(int index, int count);

    void Clear();

    template<typename C>
    void Sort(C* comparer)
    {
        _items.Sort<C>(comparer);
    }

    template<typename C>
    void Sort(int index, int length, C* comparer)
    {
        _items.Sort<C>(index, length, comparer);
    }

private:

    Array<ItemEntry*> _items;
};

inline bool ObservableCollection::IsEmpty()
{
    return (GetCount() == 0);
}

inline int ObservableCollection::GetCount() const
{
    return _items.Length();
}

inline Object* ObservableCollection::GetItem(int index) const
{
    return _items.GetItem(index)->GetItem();
}

}

#endif
