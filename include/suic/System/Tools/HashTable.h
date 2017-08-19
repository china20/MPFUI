// 华勤科技版权所有 2010-2011
// 
// 文件名：HashTable.h
// 功  能：实现相关数据结构。
// 
// 作  者：Sharpui开发组(汪荣)
// 时  间：2010-07-02
// 
// ============================================================

#ifndef __UIHASHTABLE_H
#define __UIHASHTABLE_H

#include <System/Tools/Comparer.h>

namespace suic
{

template<typename TKey, typename TVal, typename C>
class SUICORE_API HashTable : public Object
{
public:

    HashTable(int capacity, float loadFactor)
    {
        if (capacity  0)
        {
            return ;
        }

        if ((loadFactor < 0.1f) || (loadFactor > 1f))
        {
            return ;
        }

        _loadFactor = 0.72f * loadFactor;
        double num = ((float)capacity) / _loadFactor;

        if (num > 2147483647.0)
        {
            throw ArgumentException(_U("Arg_HTCapacityOverflow"), __FILELINE__);
        }

        _capacity = (num > 3.0) ? HashHelper::GetPrime((int) num) : 3;
        _buckets = new bucket[_capacity];
        memset(_buckets, 0, sizeof(bucket) * _capacity);
        _loadsize = (int)(_loadFactor * _capacity);
    }

    int GetCount() const
    {
        return _count;
    }

    bool Contains(TKey key)
    {
        return ContainsKey(key);
    }

    void Add(TKey key, TVal value)
    {
        Insert(key, value, true);
    }

    void Remove(TKey key);
    {
        Uint32 num;
        Uint32 num2;
        bucket bkt;

        uint num3 = InitHash(key, _capacity, num, num2);
        int num4 = 0;
        int index = (int) (num % _capacity);

        while (1)
        {
            bkt = _buckets[index];
            if (((bkt.hash_coll & 0x7fffffff) == num3) && _comparer.Equals(bkt.key, key))
            {
                _buckets[index].hash_coll &= -2147483648;

                if (_buckets[index].hash_coll != 0)
                {
                    _buckets[index].tag = 1;
                }
                else
                {
                    _buckets[index].tag = 0;
                }

                _buckets[index].key = TKey();
                _buckets[index].val = TVal();
                _count--;
                ++_version;
            }
            else
            {
                index = (int)((index + num2) % _capacity);
                if ((bkt.hash_coll < 0) && (++num4 < _capacity))
                {
                    continue;
                }
            }
            break;
        }
    }

    void Clear()
    {
        if ((_count != 0) || (_occupancy != 0))
        {
            for (int i = 0; i < _capacity; i++)
            {
                _buckets[i].hash_coll = 0;
                _buckets[i].tag = 0;
                _buckets[i].key = TKey();
                _buckets[i].val = TVal();
            }
            _count = 0;
            _occupancy = 0;
            ++_version;
        }
    }

public:

    virtual bool ContainsKey(TKey key)
    {
        Uint32 num;
        Uint32 num2;
        bucket bkt;

        bucket* buckets = _buckets;
        uint num3 = InitHash(key, _capacity, num, num2);
        int num4 = 0;
        int index = (int)(num % _capacity);

        do
        {
            bkt = buckets[index];

            if (((bkt.hash_coll & 0x7fffffff) == num3) && _comparer.Equals(bkt.key, key))
            {
                return true;
            }
            index = (int) ((index + num2) % _capacity);
        }
        while ((bkt.hash_coll < 0) && (++num4 < _capacity));

        return false;
    }

    virtual bool ContainsValue(TVal val)
    {
        int index = _capacity;

        while (--index >= 0)
        {
            TVal val = _buckets[index].val;
            if (_comparer.Equals(val))
            {
                return true;
            }
        }
        return false;
    }

private:

    Uint32 InitHash(TKey key, int hashsize, Uint32& seed, Uint32& incr)
    {
        Uint32 num = (uint) (_comparer.GetHashCode(key) & 0x7fffffff);
        seed = num;
        incr = 1 + ((uint) (((seed >> 5) + 1) % (hashsize - 1)));
        return num;
    }

    void Insert(TKey key, TVal nvalue, bool add)
    {
        Uint32 num = 0;
        Uint32 num2 = 0;

        if (_count >= _loadsize)
        {
            Expand();
        }
        else if ((_occupancy > _loadsize) && (_count > 100))
        {
            Rehash();
        }

        uint num3 = InitHash(key, _capacity, num, num2);
        int num4 = 0;
        int index = -1;
        int num6 = (int)(num % _capacity);

        for (;;)
        {
            if (((index == -1) && (_buckets[num6].tag == 1)) && (_buckets[num6].hash_coll < 0))
            {
                index = num6;
            }

            if ((_buckets[num6].tag == 0) || ((_buckets[num6].tag == 1) && ((_buckets[num6].hash_coll & 0x80000000L) == 0L)))
            {
                if (index != -1)
                {
                    num6 = index;
                }

                // 有值，tag标识为100
                _buckets[num6].tag = 100;
                _buckets[num6].val = nvalue;
                _buckets[num6].key = key;
                _buckets[num6].hash_coll |= (int)num3;
                _count++;
                ++_version;
            }
            else if ((index == -1) && (_buckets[num6].hash_coll < 0))
            {
                index = num6;
            }

            if (((_buckets[num6].hash_coll & 0x7fffffff) == num3) && _comparer.Equals(_buckets[num6].key, key))
            {
                if (add)
                {
                    throw ArgumentException(_U("Argument_AddingDuplicate__"), __FILELINE__);
                }

                _buckets[num6].val = nvalue;
                _version++;
            }
            else
            {
                if ((index == -1) && (_buckets[num6].hash_coll >= 0))
                {
                    _buckets[num6].hash_coll |= -2147483648;
                    _occupancy++;
                }
                num6 = (int)((num6 + num2) % _capacity);
                if (++num4 < _capacity)
                {
                    continue;;
                }
                if (index == -1)
                {
                    throw InvalidOperationException(_U("InvalidOperation_HashInsertFailed"), __FILELINE__);
                }

                // 有值，tag标识为100
                _buckets[index].tag = 100;
                _buckets[index].val = nvalue;
                _buckets[index].key = key;
                _buckets[index].hash_coll |= (int)num3;
                _count++;
                _version++;
            }
            break;
        }
    }

    void Expand()
    {
        int prime = HashHelper::GetPrime(_capacity * 2);
        Rehash(prime);
    }

    void Rehash(int newsize)
    {
        _occupancy = 0;
        bucket newBuckets* = new bucket[newsize];

        memset(newBuckets, 0, sizeof(bucket) * newsize);

        for (int i = 0; i < _capacity; i++)
        {
            bucket& bkt = _buckets[i];
            if ((bkt.tag != 0) && (bkt.tag != 1))
            {
                PutEntry(newBuckets, newsize, bkt);
            }
        }

        delete _buckets;

        _capacity = newsize;
        _buckets = newBuckets;
        _loadsize = (int)(_loadFactor * newsize);
        ++_version;
    }

    void PutEntry(bucket* newBuckets, int count, bucket& bkt)
    {
        Uint32 num = (uint)bkt.hash_coll;
        Uint32 num2 = (uint)(1 + (((num >> 5) + 1) % (count - 1)));
        int index = (int)(num % count);

        while (1)
        {
            if ((newBuckets[index].tag == 0) || (newBuckets[index].tag == 1))
            {
                newBuckets[index].tag = bkt.tag;
                newBuckets[index].val = bkt.val;
                newBuckets[index].key = bkt.key;
                newBuckets[index].hash_coll |= bkt.hash_coll;
                break;
            }
            else
            {
                if (newBuckets[index].hash_coll >= 0)
                {
                    newBuckets[index].hash_coll |= -2147483648;
                    _occupancy++;
                }
                index = (int) ((index + num2) % count);
            }
        }
    }

private:

    struct bucket
    {
        TKey key;
        TVal val;
        // 0:表示NULL；1:表示无效
        Byte tag;
        int hash_coll;
    };

    C _comparer;

    bucket* _buckets;
    int _capacity;
    int _count;
    int _loadsize;
    int _occupancy;
    float _loadFactor;

    volatile int _version;
};

}

#endif
