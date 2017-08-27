
#include <Framework/Controls/ItemSource.h>
#include <Framework/Controls/ItemCollection.h>

namespace suic
{

//============================================

ImplementRTTIOfClass(ItemCollection, Object)

ItemEntry::ItemEntry()
    : _item(NULL)
{
    _size.cx = -1;
}

ItemEntry::ItemEntry(Object* val)
    : _item(val)
{
    if (NULL != _item)
    {
        _item->ref();
    }
    _size.cx = -1;
}

ItemEntry::~ItemEntry()
{
    if (NULL != _item)
    {
        _item->unref();
    }
}

ObservableCollection::ObservableCollection()
{

}

ObservableCollection::ObservableCollection(int capacity)
{
    _items.Resize(capacity);
}

ObservableCollection::~ObservableCollection()
{
    Clear();
}

int ObservableCollection::AddItem(Object* value)
{
    ItemEntry* itemb = new ItemEntry(value);
    int index = _items.Length();
    itemb->ref();
    _items.Add(itemb);

    if (CollectionChanged.safe_bool())
    {
        NotifyCollChangedEventArg e(NotifyCollectionChangedAction::Add, NULL, 0, itemb, index);
        CollectionChanged(this, &e);
    }

    return index;
}

void ObservableCollection::SetItem(int index, Object* value)
{
    ItemEntry* item = _items[index];
    ItemEntry* itemb = new ItemEntry(value);
    itemb->ref();

    _items.SetItem(index, itemb);

    if (CollectionChanged.safe_bool())
    {
        NotifyCollChangedEventArg e(NotifyCollectionChangedAction::Replace, item, index, itemb, index);
        CollectionChanged(this, &e);
    }

    item->unref();
}

void ObservableCollection::MoveItem(int oriPos, int dstPos)
{
    ItemEntry* item = _items[oriPos];

    _items.RemoveAt(oriPos);
    _items.Insert(dstPos, item);

    if (CollectionChanged.safe_bool())
    {
        NotifyCollChangedEventArg e(NotifyCollectionChangedAction::Move, item, oriPos, item, dstPos);
        CollectionChanged(this, &e);
    }
}

void ObservableCollection::SwapItem(int index1, int index2)
{
    ItemEntry* item1 = _items[index1];
    ItemEntry* item2 = _items[index2];

    _items.SetItem(index1, item2);
    _items.SetItem(index2, item1);

    if (CollectionChanged.safe_bool())
    {
        NotifyCollChangedEventArg e(NotifyCollectionChangedAction::Swap, item1, index1, item2, index2);
        CollectionChanged(this, &e);
    }
}

bool ObservableCollection::Contains(Object* item)
{
    return (IndexOf(item) >= 0);
}

ItemEntry* ObservableCollection::GetItemEntry(int index) const
{
    if (index < 0 || index >= _items.GetCount())
    {
        return NULL;
    }
    else
    {
        return _items.GetItem(index);
    }
}

int ObservableCollection::IndexOf(Object* item)
{
    return IndexOf(item, 0, _items.Length());
}

int ObservableCollection::IndexOf(Object* item, int startIndex)
{
    return IndexOf(item, startIndex, _items.Length() - startIndex);
}

int ObservableCollection::IndexOf(Object* item, int startIndex, int count)
{
    count += startIndex;
    if (count > _items.Length())
    {
        count = _items.Length();
    }

    for (int i = startIndex; i < count; ++i)
    {
        if (_items[i]->GetItem() == item)
        {
            return i;
        }
    }

    return -1;
}

void ObservableCollection::InsertItem(int index, Object* value)
{
    ItemEntry* itemb = new ItemEntry(value);
    _items.Insert(index, itemb)->ref();

    if (CollectionChanged.safe_bool())
    {
        NotifyCollChangedEventArg e(NotifyCollectionChangedAction::Insert, NULL, 0, itemb, index);
        CollectionChanged(this, &e);
    }
}

bool ObservableCollection::RemoveItem(Object* obj)
{
    int index = IndexOf(obj);
    if (index >= 0)
    {
        return RemoveItemAt(index);
    }
    else
    {
        return false;
    }
}

bool ObservableCollection::RemoveItemAt(int index)
{
    if (index >= 0 && index < GetCount())
    {
        ItemEntry* item = _items[index];
        _items.RemoveAt(index);

        if (CollectionChanged.safe_bool())
        {
            NotifyCollChangedEventArg e(NotifyCollectionChangedAction::Remove, item, index, NULL, 0);
            CollectionChanged(this, &e);
        }

        item->unref();

        return true;
    }
    else
    {
        return false; 
    }
}

void ObservableCollection::RemoveRange(int index, int count)
{
    count += index;
    if (count >= GetCount())
    {
        count = GetCount() - 1;
    }

    for (int i = index; i <= count; ++i)
    {
        if (_items[i] != NULL)
        {
            _items[i]->unref();
        }
    }

    _items.RemoveRange(index, count);
}

void ObservableCollection::Clear()
{
    for (int i = 0; i < GetCount(); ++i)
    {
        if (_items[i] != NULL)
        {
            _items[i]->unref();
        }
    }

    _items.Clear();

    if (CollectionChanged.safe_bool())
    {
        NotifyCollChangedEventArg e(NotifyCollectionChangedAction::Reset, NULL, 0, NULL, 0);
        CollectionChanged(this, &e);
    }
}

////////////////////////////////////////////////////////
//

void ItemCollection::BeginInit()
{
}

void ItemCollection::EndInit()
{
}

void ItemCollection::SetLogicalParent(DpObject* parent)
{
    if (NULL == parent)
    {
        _logicalParent = WeakRef();
    }
    else
    {
        _logicalParent = parent;
    }
}

}
