// 华勤科技版权所有 2010-2011
// 
// 文件名：Collection.h
// 功  能：实现相关数据结构。
// 
// 作  者：Sharpui开发组(汪荣)
// 时  间：2010-07-02
// 
// ============================================================

#ifndef __UICOLLECTION_H
#define __UICOLLECTION_H

#include <System/Tools/Comparer.h>

namespace suic
{

template<typename T>
class DeleteCb
{
public:

    DeleteCb(T t) {}
};

template<typename T>
class Collection
{
public:

    Collection()
        : _version(0)
    {
        _comparer = NULL;
    }

    Collection(int initialCapacity) 
        : _items(initialCapacity)
        , _version(0)
    {
        _comparer = NULL;
    }

    ~Collection() 
    {
        Clear();
    }

    int GetCount() const
    {
        return _items.Length();
    }

    T& GetItem(int index) const
    {
        return _items.GetItem(index);
    }

    void SetItem(int index, T val)
    {
        OnSet(index, val);
        _items.SetItem(index, val);
    }

    void SetComparer(IComparer<T>* val)
    {
        _comparer = val;
    }

    int AddItem(T value)
    {
        int index = _items.Length();
        _items.Add(value);
        OnAdd(value);

        return index;
    }

    void Clear()
    {
        OnClear();
        _items.Clear();
    }

    bool Contains(const T& value)
    {
        return (IndexOf(value) != -1);
    }

    int IndexOf(const T& val)
    {
        for (int i = 0; i < _items.Length(); ++i)
        {
            if (_items[i] == val || 
                (_comparer && _comparer->Equals(_items[i], val)))
            {
                return i;
            }
        }

        return -1;
    }

    void InsertItem(int index, T val)
    {
        OnInsert(index, val);
        _items.Insert(index, val);
    }

    void RemoveItem(T value)
    {
        int index = IndexOf(value);
        if (index != -1)
        {
            RemoveItemAt(index);
        }
    }

    void RemoveItemAt(int index)
    {
        OnRemove(index);
        _items.RemoveAt(index);
    }

    T& operator[] (int index)
    {
        return _items[index];
    }

    T* GetItems() const
    {
        return _items.GetItems();
    }

protected:

    virtual void OnClear() {}
    virtual void OnRemove(int index) {}
    virtual void OnAdd(T value) {}
    virtual void OnInsert(int index, T item) {}
    virtual void OnSet(int index, T val) {}

private:

    int _version;
    IComparer<T>* _comparer;
    Array<T> _items;
};

template<typename T>
class AutoColl : public Collection<T*>
{
public:

    AutoColl()
    {
    }

    AutoColl(int initialCapacity)
        : Collection<T*>(initialCapacity)
    {
    }

    void OnClear()
    {
        for (int i = 0; i < GetCount(); ++i)
        {
            T* val = GetItem(i);
            if (val)
            {
                val->unref();
            }
        }
    }

    void OnRemove(int index)
    {
        T* item = GetItem(index);

        if (item)
        {
            item->unref();
        }
    }

    void OnAdd(T* item)
    {
        if (item)
        {
            item->ref();
        }
    }

    void OnInsert(int index, T* item)
    {
        if (item)
        {
            item->ref();
        }
    }

    void OnSet(int index, T* val)
    {
        T* prev = GetItem(index);

        if (val)
        {
            val->ref();
        }

        if (prev)
        {
            prev->unref();
        }
    }
};

template<typename T>
class DelCollection : public Collection<T*>
{
public:

    DelCollection()
    {
    }

    DelCollection(int initialCapacity)
        : Collection<T*>(initialCapacity)
    {
    }

    void OnClear()
    {
        for (int i = 0; i < GetCount(); ++i)
        {
            T* val = GetItem(i);
            if (val)
            {
                delete val;
            }
        }
    }

    void OnRemove(int index)
    {
        T* item = GetItem(index);
        if (item)
        {
            delete item;
        }
    }

    void OnSet(int index, T* val)
    {
        T* prev = GetItem(index);
        if (prev)
        {
            delete prev;
        }
    }
};

template<typename T, typename Base=HoldObj>
class Stack : public Base
{
private:

    // 栈顶索引
    int _size;
    int _version;

    Array<T> _items;

public:

    Stack()
        : Object(false)
    {
        _items.Resize(10);
        _size = 0;
        _version = 0;
    }

    Stack(int initialCapacity)
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

    int GetCount() const
    {
        return _size;
    }

    void Push(T val)
    {
        if (_size == _items.Capacity())
        {
            _items.Resize(_items.Capacity() * 2);
        }

        _items[_size++] = val;
        _version++;
        OnAdd(val);
    }

    T& Pop()
    {
        if (_size <= 0)
        {
            throw;
        }

        _version++;
        T obj = _items[--_size];
        _items[_size] = T();

        return obj;
    }

    T& Peek()
    {
        if (_size == 0)
        {
            throw ;
        }
        return _items[_size - 1];
    }

    void Clear()
    {
        OnClear();
        _items.Clear();
        _size = 0;
        _version++;
    }

protected:

    virtual void OnAdd(T val){}
    virtual void OnClear() {}
};

template<typename T>
class SStack : public Stack<T>
{
public:

    SStack() {}

protected:

    void OnAdd(T val)
    {
        if (val)
        {
            val->ref();
        }
    }

    void OnClear()
    {
        while (GetCount() > 0)
        {
            T& val = Pop();
            if (val)
            {
                val->unref();
            }
        }
    }
};

template<typename TKey, typename TVal, typename C>
class DicBase
{
private:

    // 记录桶个数
    ArrayType<int> _buckets;
    ArrayType<Entry<TKey, TVal>, true> _entries;

    int _version;
    int _count;
    int _freeCount;
    int _freeList;

    // 比较算法
    C _comparer;

public:

    virtual ~DicBase() 
    {
        Clear();
    }

    DicBase()
    {
        _version = -1;
        Initialize(16);
    }

    DicBase(int capacity)
    {
        _version = -1;
        Initialize(capacity);
    }

    int GetCount() const
    {
        return (_count - _freeCount);
    }

    bool ContainsKey(const TKey& key)
    {
        return (FindEntry(key) >= 0);
    }

    bool TryGetValue(const TKey& key, TVal& value)
    {
        int index = FindEntry(key);

        if (index >= 0)
        {
            value = _entries[index].val;
            return true;
        }
        else
        {
            value = TVal();
            return false;
        }
    }

    void Add(const TKey& key, TVal value)
    {
        Insert(key, value, true);
    }

    void Clear()
    {
        if (_count > 0)
        {
            for (int i = 0; i < _entries.Length(); ++i)
            {
                if (_entries[i].hashCode > 0)
                {
                    OnRemove(_entries[i].val);
                }
            }

            _buckets.Clear();
            _entries.Clear();
            _freeList = -1;
            _count = 0;
            _freeCount = 0;
            _version++;
        }
    }

    bool Contains(const TKey& key)
    {
        return ContainsKey(key);
    }

    bool Remove(const TKey& key)
    {
        if (_buckets.IsValid())
        {
            int num = _comparer.GetHashCode(key) & 0x7fffffff;
            int index = num % _buckets.Length();
            int num3 = -1;

            for (int i = _buckets[index]; i >= 0; i = _entries[i].next)
            {
                if ((_entries[i].hashCode == num) && _comparer.Equals(_entries[i].key, key))
                {
                    if (num3 < 0)
                    {
                        _buckets[index] = _entries[i].next;
                    }
                    else
                    {
                        _entries[num3].next = _entries[i].next;
                    }

                    _entries[i].hashCode = -1;
                    _entries[i].next = _freeList;
                    _entries[i].key = TKey();

                    OnRemove(_entries[i].val);

                    _entries[i].val = TVal();
                    _freeList = i;
                    _freeCount++;
                    _version++;

                    return true;
                }
                num3 = i;
            }
        }
        return false;
    }

    Entry<TKey, TVal>* FindPair(const TKey& key)
    {
        int index = FindEntry(key);
        if (index >= 0)
        {
            return &(_entries[index]);
        }
        else
        {
            return NULL;
        }
    }

    void Initialize(int capacity)
    {
        if (_version < 0 && capacity > 0)
        {
            _version = 0;
            _count = 0;
            _freeCount = 0;

            int bucketsSize = HashHelper::GetPrime(capacity);

            _buckets.Reset(bucketsSize, false);
            _buckets.Fill(0, bucketsSize, -1);
            _entries.Resize(bucketsSize);

            _freeList = -1;
        }
    }

    void GetKeys(Collection<TKey>& keys)
    {
        for (int i = 0; i < _entries.Length(); ++i)
        {
            if (_entries[i].hashCode > 0)
            {
                keys.AddItem(_entries[i].key);
            }
        }
    }

    void GetValues(Collection<TVal>& vals)
    {
        for (int i = 0; i < _entries.Length(); ++i)
        {
            if (_entries[i].hashCode > 0)
            {
                vals.AddItem(_entries[i].val);
            }
        }
    }

private:

    void Resize()
    {
        int prime = HashHelper::GetPrime(_count * 2);

        _buckets.Reset(prime, false);
        _buckets.Fill(0, prime, -1);
        _entries.Resize(prime);

        for (int j = 0; j < _count; j++)
        {
            int hashcode = _entries[j].hashCode;
            int index = hashcode % prime;
            _entries[j].next = _buckets[index];
            _buckets[index] = j;
        }
    }

    int FindEntry(const TKey& key)
    {
        if (_buckets.IsValid())
        {
            int num = _comparer.GetHashCode(key) & 0x7fffffff;

            for (int i = _buckets[num % _buckets.Length()]; i >= 0; i = _entries[i].next)
            {
                if ((_entries[i].hashCode == num) && _comparer.Equals(_entries[i].key, key))
                {
                    return i;
                }
            }
        }
        return -1;
    }

    void Insert(const TKey& key, TVal val, bool add)
    {
        int freeList;

        // 初始化默认桶
        if (!_buckets.IsValid())
        {
            Initialize(1);
        }

        // 取得其hash散列值
        int num = _comparer.GetHashCode(key) & 0x7fffffff;
        int index = num % _buckets.Length();

        for (int i = _buckets[index]; i >= 0; i = _entries[i].next)
        {
            TKey& tmp = _entries[i].key;

            if ((_entries[i].hashCode == num) && _comparer.Equals(tmp, key))
            {
                // 元素重复
                if (add)
                {
                    ThrowExcept(tmp, __FILELINE__);
                }

                OnReplace(_entries[i].val, val);

                _entries[i].val = val;

                _version++;
                return;
            }
        }

        if (_freeCount > 0)
        {
            freeList = _freeList;
            _freeList = _entries[freeList].next;
            _freeCount--;
        }
        else
        {
            if (_count == _entries.Length())
            {
                Resize();
                index = num % _buckets.Length();
            }

            freeList = _count;
            _count++;
        }

        _entries[freeList].hashCode = num;
        _entries[freeList].next = _buckets[index];
        _entries[freeList].key = key;
        _entries[freeList].val = val;

        OnAdd(val);

        _buckets[index] = freeList;
        _version++;
    }

protected:

    virtual void ThrowExcept(const TKey& key, char* f, int l)
    {
        String strErr;
        strErr.Format(_U("Key(%s) of dictionary is exist!"), _comparer.KeyAsString(key).c_str());
        throw InvalidOperationException(strErr, f, l);
    }

    virtual void OnRemove(TVal oldVal) {}
    virtual void OnAdd(TVal newVal) {}
    virtual void OnReplace(TVal oldVal, TVal newVal) {}

public:

    struct Enumerator
    {
    private:

        int version;
        int index;
        int getEnumeratorRetType;

        DicBase* dictionary;
        Entry<TKey, TVal>* current;

    public:

        Enumerator()
        {
            current = NULL;
            dictionary = NULL;
            version = 0;
            index = 0;
            getEnumeratorRetType = 0;
        }

        Enumerator(DicBase* dic, int enumeratorRetType)
        {
            Init(dic, enumeratorRetType);
        }

        bool HasNext()
        {
            if (version != dictionary->_version)
            {
                throw;
            }

            while (index < dictionary->_count)
            {
                if (dictionary->_entries[index].hashCode >= 0)
                {
                    current = &(dictionary->_entries[index]);
                    index++;
                    return true;
                }

                index++;
            }

            index = dictionary->_count + 1;
            current = NULL;

            return false;
        }

        Entry<TKey, TVal>* Current()
        {
            return current;
        }

        void Reset()
        {
            if (version != dictionary->_version)
            {
                throw;
            }

            index = 0;
            current = NULL;
        }

    protected:

        void Init(DicBase* dic, int enumeratorRetType)
        {
            current = NULL;
            dictionary = dic;
            version = dic->_version;
            index = 0;
            getEnumeratorRetType = enumeratorRetType;
        }
    };
};

template<typename TKey, typename TVal, typename C>
class Dictionary : public DicBase<TKey, TVal, C>
{
public:

    Dictionary() {}
    Dictionary(int capacity)
        : DicBase(capacity) {}
};

template<typename TKey, typename TVal, typename C>
class AutoDic : public DicBase<TKey, TVal*, C>
{
public:

    AutoDic() {}
    AutoDic(int capacity)
        : DicBase(capacity) {}

protected:

    void OnRemove(TVal* oldVal)
    {
        if (NULL != oldVal)
        {
            oldVal->unref();
        }
    }

    void OnAdd(TVal* newVal)
    {
        if (NULL != newVal)
        {
            newVal->ref();
        }
    }

    void OnReplace(TVal* oldVal, TVal* newVal)
    {
        if (NULL != newVal)
        {
            newVal->ref();
        }
        if (NULL != oldVal)
        {
            oldVal->unref();
        }
    }
};

template<typename TKey, typename TVal, typename C>
class DeleteDic : public DicBase<TKey, TVal*, C>
{
public:

    DeleteDic() {}
    DeleteDic(int capacity)
        : DicBase(capacity) {}

protected:

    void OnRemove(TVal* oldVal)
    {
        if (NULL != oldVal)
        {
            delete oldVal;
        }
    }

    void OnReplace(TVal* oldVal, TVal* newVal)
    {
        if (NULL != oldVal)
        {
            delete oldVal;
        }
    }
};

template<typename TKey, typename TVal>
class ValueDic : public Dictionary<TKey, TVal, ValComparer<TKey> >
{
public:

    ValueDic() {}
    ValueDic(int capacity)
        : Dictionary(capacity) {}
};

template<typename TVal>
class StringDic : public Dictionary<String, TVal, StrComparer >
{
public:

    StringDic() {}
    StringDic(int capacity)
        : Dictionary(capacity) {}
};

template<typename TKey, typename TVal>
class ObjectDic : public Dictionary<TKey, TVal, ObjComparer<TKey> >
{
public:

    ObjectDic() {}
    ObjectDic(int capacity)
        : Dictionary(capacity) {}
};

template<typename TKey, typename TVal>
class PointerDic : public Dictionary<TKey, TVal, PtrComparer<TKey> >
{
public:

    PointerDic() {}
    PointerDic(int capacity)
        : Dictionary(capacity) {}
};

//--------------------------------------------------------------------

template<typename TKey, typename TVal>
class ValueAutoDic : public AutoDic<TKey, TVal, ValComparer<TKey> >
{
public:

    ValueAutoDic() {}
    ValueAutoDic(int capacity)
        : AutoDic(capacity) {}
};

class IntAutoDicObj : public ValueAutoDic<int, Object>
                    , public Object
{
public:

    RTTIOfClass(IntAutoDicObj)

    IntAutoDicObj();
    ~IntAutoDicObj();
};

template<typename TVal>
class StringAutoDic : public AutoDic<suic::String, TVal, StrComparer >
{
public:

    StringAutoDic() {}
    StringAutoDic(int capacity)
        : AutoDic(capacity) {}
};

class XDictionary : public StringAutoDic<Object>
                  , public Object
{
public:

    RTTIOfClass(XDictionary)

    XDictionary() {}
};

template<typename TKey, typename TVal>
class ObjectAutoDic : public AutoDic<TKey, TVal, ObjComparer<TKey> >
{
public:

    ObjectAutoDic() {}
    ObjectAutoDic(int capacity)
        : AutoDic(capacity) {}
};

template<typename TKey, typename TVal>
class PointerAutoDic : public AutoDic<TKey, TVal, PtrComparer<TKey> >
{
public:

    PointerAutoDic() {}
    PointerAutoDic(int capacity)
        : AutoDic(capacity) {}
};

}

#endif
