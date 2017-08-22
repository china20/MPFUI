// 华勤科技版权所有 2010-2011
// 
// 文件名：ItemCollectionEx.h
// 功  能：ItemsControl分组对象
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIITEMCOLLECTIONEX_H_
#define _UIITEMCOLLECTIONEX_H_

#include <System/Windows/Binding.h>

#include <Framework/Items/CollectionView.h>
#include <Framework/Items/ArrayList.h>

namespace suic
{

class ItemCollectionEx : public IList
                       , public CollectionView
{
public:

    ItemCollectionEx(Element* host);
    ~ItemCollectionEx();

    // IEnumerable接口实现
    virtual IEnumerator* GetEnumerator()
    {
        return _items.GetEnumerator();
    }

    // ICollection接口实现
    virtual int GetCount()
    {
        return _items.GetCount();
    }

    // IList接口实现
    virtual int Add(Object* value)
    {
        return _items.Add(value);
    }

    virtual void Clear()
    {
        _items.Clear();
    }

    virtual bool Contains(Object* value)
    {
        return _items.Contains(value);
    }

    virtual int IndexOf(Object* value)
    {
        return _items.IndexOf(value);
    }

    virtual void Insert(int index, Object* value)
    {
        _items.Insert(index, value);
    }

    virtual void Remove(Object* value)
    {
        _items.Remove(value);
    }

    virtual void RemoveAt(int index)
    {
        _items.RemoveAt(index);
    }

    virtual void RemoveRange(int index, int count)
    {
        _items.RemoveRange(index, count);
    }

    virtual Object* GetItem(int index)
    {
        return _items.GetItem(index);
    }

    virtual void SetItem(int index, Object* item)
    {
        _items.SetItem(index, item);
    }

    virtual IList* GetView()
    {
        return &_items;
    }

    void BeginInit() {}
    void EndInit() {}

protected:

    ArrayList _items;
    WeakRef _modalParent;
};

}

#endif


