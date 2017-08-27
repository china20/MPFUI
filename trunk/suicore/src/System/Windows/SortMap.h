// 华勤科技版权所有 2010-2012
// 
// 文件名：SortMap.h
// 功  能：数组实现。
// 
// 作  者：Sharpui开发组
// 时  间：2011-05-01
// 
// ============================================================================

#ifndef _UISORTMAP_H_
#define _UISORTMAP_H_

#include <math.h>
#include <new.h>
#include <System/Tools/Array.h>

namespace suic
{

struct SortedObjectMap
{
    struct Entry
    {
        int Key;
        Object* Value;
    };

    enum FrugalMapStoreState
    {
        Success,
        ThreeObjectMap,
        SixObjectMap,
        Array,
        SortedArray,
        Hashtable
    };

    int _count;
    int _capacity;
    Entry* _entries;
    int _lastKey = 0x7fffffff;

    int FindInsertIndex(int key, bool& found)
    {
        int index = 0;
        if ((_count > 0) && (key <= _lastKey))
        {
            int num3 = this._count - 1;

            do
            {
                int num2 = (num3 + index) / 2;
                if (key <= _entries[num2].Key)
                {
                    num3 = num2;
                }
                else
                {
                    index = num2 + 1;
                }
            }
            while (index < num3);

            found = key == _entries[index].Key;
            return index;
        }

        index = _count;
        found = false;
        return index;
    }

    void GetKeyValuePair(int index, int& key, Object*& value)
    {
        if (index < _count)
        {
            value = _entries[index].Value;
            key = _entries[index].Key;
        }
        else
        {
            value = DpProperty::UnsetValue;
            key = 0x7fffffff;
            throw InvalidValueException(_U("index"), __FILELINE__);
        }
    }

    FrugalMapStoreState InsertEntry(int key, Object* value)
    {
        bool flag;
        int index = FindInsertIndex(key, flag);
        if (flag)
        {
            _entries[index].Value = value;
            return FrugalMapStoreState::Success;
        }
        if (this._entries != NULL)
        {
            if (_capacity <= this._count)
            {
                int length = _capacity;
                int newLength = length + (length >> 1);
                Entry* destinationArray = new Entry[newLength];
                Array::Copy<Entry>(_entries, 0, destinationArray, 0, _capacity);
                _entries = destinationArray;
                _capacity = newLength;
            }
        }
        else
        {
            _capacity = 2;
            _entries = new Entry[2];
        }

        if (index < _count)
        {
            Array::Copy<Entry>(_entries, index, _entries, index + 1, _count - index);
        }
        else
        {
            this._lastKey = key;
        }

        _entries[index].Key = key;
        _entries[index].Value = value;
        _count++;
        return FrugalMapStoreState::Success;
    }

    void Promote(SortedObjectMap* newMap)
    {
        for (int i = 0; i < _capacity; i++)
        {
            if (newMap->InsertEntry(_entries[i].Key, _entries[i].Value) != FrugalMapStoreState::Success)
            {
                throw ArgumentException(_U("FrugalMap_TargetMapCannotHoldAllData"), __FILELINE__);
            }
        }
    }

    void RemoveEntry(int key)
    {
        bool flag;
        int destinationIndex = FindInsertIndex(key, flag);

        if (flag)
        {
            int length = (_count - destinationIndex) - 1;
            if (length > 0)
            {
                Array::Copy<Entry>(_entries, destinationIndex + 1, _entries, destinationIndex, length);
            }
            else if (_count > 1)
            {
                _lastKey = _entries[this._count - 2].Key;
            }
            else
            {
                _lastKey = 0x7fffffff;
            }

            _entries[_count - 1].Key = 0x7fffffff;
            _entries[_count - 1].Value = DpProperty::UnsetValue;
            _count--;
        }
    }

    Object* Search(int key)
    {
        bool flag;
        int index = FindInsertIndex(key, flag);
        if (flag)
        {
            return _entries[index].Value;
        }
        return DpProperty::UnsetValue;
    }

    void Sort()
    {
    }

    int GetCount() const
    {
        return _count;
    }
};

struct InsertionSortMap
{
    SortedObjectMap* _mapStore;

    InsertionSortMap()
    {
        _mapStore = NULL;
    }

    Object*& operator[](int key)
    {
        if (_mapStore != NULL)
        {
            return _mapStore.Search(key);
        }
        return DpProperty::UnsetValue;
    }

    void SetAt(int key, Object* val)
    {
        if (val != DpProperty::UnsetValue)
        {
            if (_mapStore == NULL)
            {
                _mapStore = new SortedObjectMap();
            }

            FrugalMapStoreState state = _mapStore->InsertEntry(key, val);

            if (state != FrugalMapStoreState::Success)
            {
                if (FrugalMapStoreState::SortedArray != state)
                {
                    throw InvalidOperationException(_U("FrugalMap_CannotPromoteBeyondHashtable"), __FILELINE__);
                }

                SortedObjectMap* newMap = new SortedObjectMap();

                _mapStore->Promote(newMap);

                delete _mapStore;
                _mapStore = newMap;
                _mapStore->InsertEntry(key, val);
            }
        }
        else if (_mapStore != NULL)
        {
            _mapStore->RemoveEntry(key);
            if (_mapStore->GetCount() == 0)
            {
                _mapStore = NULL;
            }
        }
    }

    void Sort()
    {
        if (_mapStore != NULL)
        {
            _mapStore->Sort();
        }
    }

    void GetKeyValuePair(int index, int& key, Object*& value)
    {
        if (_mapStore == NULL)
        {
            throw InvalidValueException(_U("index"), __FILELINE__);
        }

        _mapStore->GetKeyValuePair(index, key, value);
    }

    int GetCount() const
    {
        if (_mapStore != NULL)
        {
            return _mapStore->GetCount();
        }
        return 0;
    }
};

}

#endif
