
#include <System/Tools/CollectionX.h>
#include <System/Tools/Collection.h>
#include <System/Windows/Exception.h>

namespace suic
{

//====================================================
ImplementRTTIOfClassName(XStack, Object, Stack)
ImplementRTTIOfClassName(XArray, Object, Array)
ImplementRTTIOfClassName(XQueue, Object, Queue)
//ImplementRTTIOfClass(IEnumerable, DpObject)

ImplementRTTIOfClass(XDictionary, Object)

XStack::XStack()
{
    _items.Resize(10);
    _size = 0;
    _version = 0;
}

XStack::XStack(int initialCapacity)
{
    if (initialCapacity < 0)
    {
        throw ;
    }

    if (initialCapacity < 10)
    {
        initialCapacity = 10;
    }

    _items.Resize(initialCapacity);

    _size = 0;
    _version = 0;
}

int XStack::GetCount() const
{
    return _size;
}

void XStack::Push(Object* val)
{
    if (_size == _items.Capacity())
    {
        _items.Resize(_items.Capacity() * 2);
    }

    SETREFOBJ(_items[_size++], val);
    _version++;
}

void XStack::Pop(ObjectPtr& obj)
{
    if (_size <= 0)
    {
        throw;
    }

    _version++;
    obj = _items[--_size];
    SETREFOBJ(_items[_size++], NULL);
}

Object* XStack::Peek()
{
    if (_size == 0)
    {
        throw ;
    }
    return _items[_size - 1];
}

void XStack::Clear()
{
    OnClear();
    _items.Clear();
    _size = 0;
    _version++;
}

void XStack::OnClear()
{
    for (int i = 0; i < _items.Length(); ++i)
    {
        _items[i]->unref();
    }
}

//============================================

XArray::XArray()
{
}

XArray::XArray(int capacity)
{
    if (capacity > 0)
    {
        _items.Resize(capacity);
    }
}

XArray::~XArray()
{
    Clear();
}

void XArray::Resize(int size)
{
    if (size > 0)
    {
        _items.Resize(size);
    }
}

void XArray::Relength(int size)
{
    if (size > 0)
    {
        _items.Relength(size);
    }
}

void XArray::Dispose()
{
    Clear();
}

void XArray::SetItem(int index, Object* value)
{
    if (_items[index] != NULL)
    {
        _items[index]->unref();
    }

    value->ref();
    _items[index] = value;
}

int XArray::Add(Object* value)
{
    _items.Add(value);
    value->ref();
    return _items.Length() - 1;
}

bool XArray::Contains(Object* item)
{
    return (IndexOf(item) >= 0);
}

int XArray::IndexOf(Object* item)
{
    return IndexOf(item, 0, _items.Length());
}

int XArray::IndexOf(Object* item, int startIndex)
{
    return IndexOf(item, startIndex, _items.Length() - startIndex);
}

int XArray::IndexOf(Object* item, int startIndex, int count)
{
    count += startIndex;
    if (count > _items.Length())
    {
        count = _items.Length();
    }

    for (int i = startIndex; i < count; ++i)
    {
        if (_items[i] == item)
        {
            return i;
        }
    }

    return -1;
}

void XArray::Insert(int index, Object* value)
{
    _items.Insert(index, value);
    value->ref();
}

bool XArray::Remove(Object* obj)
{
    int index = IndexOf(obj);
    if (index >= 0)
    {
        obj->unref();
        _items.RemoveAt(index);
        return true;
    }
    else
    {
        return false;
    }
}

void XArray::RemoveAt(int index)
{
    if (GetItem(index))
    {
        GetItem(index)->unref();
    }
   _items.RemoveAt(index); 
}

void XArray::RemoveRange(int index, int count)
{
    for (int i = index; i < index + count; ++i)
    {
        if (_items[i] != NULL)
        {
            _items[i]->unref();
        }
    }

    _items.RemoveRange(index, count);
}

void XArray::Clear()
{
    for (int i = 0; i < GetCount(); ++i)
    {
        if (_items[i] != NULL)
        {
            _items[i]->unref();
        }
    }

    _items.Clear();
}

//============================================

XQueue::XQueue()
{
    Init(0x20, 2);
}

XQueue::XQueue(int capacity)
{
    Init(capacity, 2);
}

XQueue::XQueue(int capacity, Float growFactor)
{
    Init(capacity, growFactor);
}

void XQueue::Init(int capacity, Float growFactor)
{
    if (capacity < 0)
    {
        throw ArgumentException(_U("Capacity of queue must great than zero!"), __FILELINE__);
    }

    _items.Resize(capacity);
    _head = 0;
    _tail = 0;
    _size = 0;
    _growFactor = growFactor;
}

ObjectPtr XQueue::Dequeue()
{
    if (GetCount() == 0)
    {
        throw InvalidOperationException(_U("InvalidOperation_EmptyQueue"), __FILELINE__);
    }

    ObjectPtr item = _items[_head];
    FREEREFOBJ(_items[_head]);
    _items[_head] = NULL;

    _head = (_head + 1) % _items.Length();
    _size--;
    _version++;

    return item;

}

Object* XQueue::Peek()
{
    if (GetCount() == 0)
    {
        throw InvalidOperationException(_U("InvalidOperation_EmptyQueue"), __FILELINE__);
    }
    return _items[_head];
}

void XQueue::Enqueue(Object* obj)
{
    if (_size == _items.Length())
    {
        int capacity = (int)((_items.Length() * _growFactor) / 100L);
        if (capacity < (_items.Length() + 4))
        {
            capacity = _items.Length() + 4;
        }
        _items.Resize(capacity);
    }

    SETREFOBJ(_items[_tail], obj);
    _tail = (_tail + 1) % _items.Length();
    _size++;
    _version++;
}

bool XQueue::Contains(Object* obj)
{
    int index = _head;
    int num2 = _size;

    while (num2-- > 0)
    {
        if (obj == NULL)
        {
            if (_items[index] == NULL)
            {
                return true;
            }
        }
        else if ((_items[index] != NULL) && _items[index]->Equals(obj))
        {
            return true;
        }
        index = (index + 1) % _items.Length();
    }

    return false;

}

void XQueue::Clear()
{
    if (_head < _tail)
    {
        for (int i = _head; i < _tail; ++i)
        {
            FREEREFOBJ(_items[i]);
            _items[i] = NULL;
        }
    }
    else
    {
        int i = 0;
        for (i = 0; i < _head; ++i)
        {
            FREEREFOBJ(_items[i]);
            _items[i] = NULL;
        }

        for (i = _tail; i < _items.Length(); ++i)
        {
            FREEREFOBJ(_items[i]);
            _items[i] = NULL;
        }
    }

    _head = 0;
    _tail = 0;
    _size = 0;
    _version++;
}

Object* XQueue::ToArray()
{
    return NULL;
}

void XQueue::TrimToSize()
{

}

int XQueue::GetCount()
{
    return _size;
}

Object* XQueue::GetElement(int i)
{
    return _items[(_head + i) % _items.Length()];
}

void XQueue::SetCapacity(int capacity)
{

}

}
