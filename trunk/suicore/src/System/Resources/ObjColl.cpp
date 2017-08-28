// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
//
// ======================================================================

#include <System/Resources/ObjColl.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

ObjColl::ObjColl()
{
    _items.Resize(4);
}

ObjColl::~ObjColl()
{
    Clear();
}

void ObjColl::Attach(Object* item)
{
    if (NULL != item)
    {
        item->ref();
    }
}

void ObjColl::Detach(Object* item)
{
    if (NULL != item)
    {
        item->unref();
    }
}

void ObjColl::AddItem(Object* item)
{
    Attach(item);
    _items.Add(item);
}

void ObjColl::InsertItem(int index, Object* item)
{ 
    Attach(item);
    _items.Insert(index, item);
}

void ObjColl::RemoveItem(int index) 
{
    Object* item = GetItem(index);
    _items.RemoveAt(index);
    Detach(item);
}

void ObjColl::SetItem(int index, Object* item) 
{
    Object* oldVal = _items[index];

    Attach(item);
    Detach(oldVal);

    _items.SetItem(index, item);
} 

void ObjColl::Clear()
{
    for (int i = 0; i < _items.GetCount(); ++i)
    {
        Detach(_items[i]);
    }
    _items.Clear();
}

bool ObjColl::Contains(Object* value)
{
    return (IndexOf(value) != -1);
}

int ObjColl::IndexOf(Object* val)
{
    for (int i = 0; i < _items.Length(); ++i)
    {
        if (_items[i] == val)
        {
            return i;
        }
    }

    return -1;
}

int ObjColl::GetCount() const
{
    return _items.GetCount();
}

Object* ObjColl::GetItem(int index) const
{
    return _items[index];
}

}
