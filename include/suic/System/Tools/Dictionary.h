
#ifndef __UIDICTIONARY_H
#define __UIDICTIONARY_H

#include <System/Tools/Array.h>
#include <System/Tools/HashHelper.h>

namespace suic
{

template<typename TKey, typename TValue, typename Comparer>
class Dictionary1
{
public:
    
    Dictionary1()
    {
        Initialize(0);
    }

    Dictionary1(int capacity)
    {
        Initialize(capacity);
    }

public:
    
    void Add(TKey key, TValue value)
    {
        Insert(key, value, true);
    }

    bool Remove(TKey key)
    {
        if (_buckets.Length() > 0)
        {
            int num = _comparer.GetHashCode(key) & 0x7fffffff;
            int index = num % _buckets.length;
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
                    _entries[i].next = freeList;

                    OnCleanPair(_entries[i]);

                    _entries[i].key = TKey();
                    _entries[i].value = TValue();
                    freeList = i;
                    freeCount++;
                    version++;
                    return true;
                }
                num3 = i;
            }
        }
        return false;
    }

    bool TryGetValue(TKey key, TValue& value)
    {
        int index = FindEntry(key);
        if (index >= 0)
        {
            value = _entries[index].value;
            return true;
        }
        else
        {
            value = TValue();
            return false;
        }
    }

    void Clear()
    {
        if (count > 0)
        {
            for (int i = 0; i < _buckets.length; i++)
            {
                _buckets[i] = -1;
            }

            for (int i = 0; i < _entries.Length(); ++i)
            {
                if (_entries[i].hashCode > 0)
                {
                    OnCleanPair(_entries[i]);
                }
            }

            _entries.Clear();
            freeList = -1;
            count = 0;
            freeCount = 0;
            version++;
        }
    }

     bool ContainsKey(TKey key)
    {
        return (FindEntry(key) >= 0);
    }

private:

    int FindEntry(TKey key)
    {
        if (_buckets.Length() > 0)
        {
            int num = _comparer.GetHashCode(key) & 0x7fffffff;
            for (int i = _buckets[num % _buckets.length]; i >= 0; i = _entries[i].next)
            {
                if ((_entries[i].hashCode == num) && _comparer.Equals(_entries[i].key, key))
                {
                    return i;
                }
            }
        }
        return -1;
    }

    void Initialize(int capacity)
    {
        int prime = HashHelper::GetPrime(capacity);
        _buckets.Resize(prime);
        for (int i = 0; i < _buckets.length; i++)
        {
            _buckets[i] = -1;
        }
        _entries.Resize(prime);
        freeList = -1;
    }

    void Insert(TKey key, TValue value, bool add)
    {
        int freeList = 0;
        if (_buckets.Length() == 0)
        {
            Initialize(1);
        }
        int num = _comparer.GetHashCode(key) & 0x7fffffff;
        int index = num % _buckets.length;
        int num3 = 0;
        for (int i = _buckets[index]; i >= 0; i = _entries[i].next)
        {
            if ((_entries[i].hashCode == num) && _comparer.Equals(_entries[i].key, key))
            {
                // ÒÑ¾­´æÔÚ
                if (add)
                {
                    throw InvalidOperationException(_U("Key of dictionary is exist!"), __FILELINE__);
                }
                _entries[i].value = value;
                version++;
                OnReplaceItem(_entries[i], value);
                return;
            }
            num3++;
        }

        if (freeCount > 0)
        {
            freeList = freeList;
            freeList = _entries[freeList].next;
            freeCount--;
        }
        else
        {
            if (count == _entries.length)
            {
                Resize();
                index = num % _buckets.length;
            }
            freeList = count;
            count++;
        }

        _entries[freeList].hashCode = num;
        _entries[freeList].next = _buckets[index];
        _entries[freeList].key = key;
        _entries[freeList].value = value;
        _buckets[index] = freeList;
        version++;

        OnAddPair(_entries[freeList]);
    }

private: 
    
    void Resize()
    {
        Resize(HashHelper::ExpandPrime(count), false);
    }

    void Resize(int newSize, bool forceNewHashCodes)
    {
        ArrayType<int> numArray;
        numArray.Resize(newSize);
        for (int i = 0; i < numArray.length; i++)
        {
            numArray[i] = -1;
        }

        _entries.Resize(newSize);

        if (forceNewHashCodes)
        {
            for (int k = 0; k < count; k++)
            {
                if (_entries[k].hashCode != -1)
                {
                    _entries[k].hashCode = _comparer.GetHashCode(_entries[k].key) & 0x7fffffff;
                }
            }
        }
        for (int j = 0; j < count; j++)
        {
            int index = _entries[j].hashCode % newSize;
            _entries[j].next = numArray[index];
            numArray[index] = j;
        }

        _buckets.Attach(numArray);
    }

protected:

    virtual void OnCleanPair(Entry<TKey, TValue>& val) {}
    virtual void OnAddPair(Entry<TKey, TValue>& val) {}
    virtual void OnReplaceItem(Entry<TKey, TValue>& oldVal, TValue newVal) {}

private:

    int count;
    int version;
    int freeList;
    int freeCount;

    Comparer _comparer;
    ArrayType<int> _buckets;
    ArrayType<Entry<TKey, TValue> > _entries;
};

}

#endif
