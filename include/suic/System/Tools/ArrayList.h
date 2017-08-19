// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ArrayList.h
// ��  �ܣ�ItemsControl�������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UIARRAYLIST_H_
#define _UIARRAYLIST_H_

#include <System/Tools/Array.h>

namespace suic
{

class ArrayList
{
public:

    ArrayList()
    {
        _current = -1;
    }

    ~ArrayList()
    {
        Clear();
    }

    bool MoveNext()
    {
        ++_current;
        if (_current >= 0 && _current < _items.GetCount())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    virtual Object* Current()
    {
        return _items[_current];
    }

    virtual void Reset()
    {
        _current = -1;
    }

    // ICollection�ӿ�ʵ��
    virtual int GetCount()
    {
        return _items.GetCount();
    }

    // IList�ӿ�ʵ��
    virtual int Add(Object* val)
    {
        val->ref();
        _items.Add(val);
        return _items.GetCount() - 1;
    }

    virtual void Clear()
    {
        for (int i = 0; i < _items.GetCount(); ++i)
        {
            _items[i]->unref();
        }
        _items.Clear();
    }

    virtual bool Contains(Object* val)
    {
        return IndexOf(val) >= 0;
    }

    virtual int IndexOf(Object* val)
    {
        int index = -1;
        for (int i = 0; i < _items.GetCount(); ++i)
        {
            if (_items[i] == val)
            {
                index = i;
                break;
            }
        }
        return index;
    }

    virtual void Insert(int index, Object* val)
    {
        val->ref();
        _items.Insert(index, val);
    }

    virtual void Remove(Object* val)
    {
        RemoveAt(IndexOf(val));
    }

    virtual void RemoveAt(int index)
    {
        if (index >= 0 && index < _items.GetCount())
        {
            _items[index]->unref();
            _items.RemoveAt(index);
        }
    }

    virtual void RemoveRange(int index, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            _items[i + index]->unref();
        }
        _items.RemoveRange(index, count);
    }

    virtual Object* GetItem(int index)
    {
        return _items.GetItem(index);
    }

    virtual void SetItem(int index, Object* item)
    {
        Object* oldItem = _items.GetItem(index);
        if (NULL != oldItem)
        {
            oldItem->unref();
        }
        item->ref();
        _items.SetItem(index, item);
    }

protected:

    int _current;
    Array<Object*> _items;
};

}

#endif


