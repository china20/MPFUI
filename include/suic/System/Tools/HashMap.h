
#ifndef __UIHASHMAP_H
#define __UIHASHMAP_H

#include <System/Types/Types.h>
#include <System/Windows/Object.h>

namespace suic
{

template <typename KEY, typename VAL, typename BASE=Object>
class HashMap : public BASE
{
public:

    struct item
    {
        KEY first;
        VAL second;

        struct item * pNext;
    };

    struct iterator
    {
        HashMap* _map;
        item* _node;
        int _pos;
        int _size;

        iterator()
            : _node(NULL)
            , _map(NULL)
        {
            ;
        }

        operator bool()
        {
            return (_node != NULL);
        }

        item* operator->()
        {
            return _node;
        }

        bool operator!=(const iterator& Other)
        {
            return (Other._node != _node);
        }

        bool operator==(const iterator& Other)
        {
            return (Other._node == _node);
        }

        void operator++()
        {
            _node = _node->pNext;

            if (_node == NULL)
            {
                for (_pos = _pos + 1; _pos < _size; ++_pos)
                {
                    if ((_node = _map->m_aT[_pos]) != NULL)
                    {
                        break;
                    }
                }
            }
        }
    };

    HashMap()
        : _iBuckets(0)
    {
        Init(63);
    }

    ~HashMap()
    {
        clear();
    }

    void Init(int nSize)
    {
        if (0 == _iBuckets)
        {
            if (nSize < 16) 
            {
                nSize = 16;
            }

            _iBuckets = nSize;
            m_aT = new item*[nSize];
            memset(m_aT, 0, nSize * sizeof(item*));
        }
    }

    iterator begin()
    {
        iterator iter;

        iter._size = _iBuckets;
        iter._map = this;
        iter._node = NULL;

        for (iter._pos = 0; iter._pos < (int)_iBuckets; ++iter._pos)
        {
            if ((iter._node = m_aT[iter._pos]) != NULL)
            {
                break;
            }
        }

        return iter;
    }

    iterator end()
    {
        static const iterator iter;
        return iter;
    }

    void resize(int nSize = 83)
    {
        if (m_aT) 
        {
            int len = _iBuckets;

            while (len--) 
            {
                item* pItem = m_aT[len];

                while (pItem) 
                {
                    item* pKill = pItem;
                    pItem = pItem->pNext;
                    delete pKill;
                }
            }

            delete [] m_aT;
            m_aT = NULL;
        }

        if (nSize < 0) 
        {
            nSize = 0;
        }

        if (nSize > 0) 
        {
            m_aT = new item*[nSize];
            memset(m_aT, 0, nSize * sizeof(item*));
        }

        _iBuckets = nSize;
    }

    iterator find(KEY key)
    {
        iterator iter;

        iter._size = _iBuckets;

        if (_iBuckets > 0) 
        {
            long slot = key.GetHashCode() % _iBuckets;

            for (item* pItem = m_aT[slot]; pItem; pItem = pItem->pNext) 
            {
                if (pItem->first == key) 
                {
                    iter._node = pItem;
                    break;
                }
            }
        }

        return iter;
    }

    bool insert(const KEY& key, VAL& pData)
    {
        if (_iBuckets == 0) 
        {
            Init(83);
        }

        if (find(key) != end()) 
        {
            return false;
        }

        // Add first in bucket
        long slot = key.GetHashCode() % _iBuckets;
        item* pItem = new item;

        pItem->first = key;
        pItem->second = pData;
        pItem->pNext = m_aT[slot];
        m_aT[slot] = pItem;

        return true;
    }

    void set(const KEY& key, VAL& pData)
    {
        if (_iBuckets == 0) 
        {
            return;
        }

        unsigned long slot = key.HashCode() % _iBuckets;

        // Modify existing item
        for (item* pItem = m_aT[slot]; pItem; pItem = pItem->pNext) 
        {
            if (pItem->first == key) 
            {
                VAL& pOldData = pItem->second;
                pItem->second = pData;
                return;
            }
        }

        insert(key, pData);
    }

    bool erase(const KEY& key)
    {
        if (_iBuckets == 0) 
        {
            return false;
        }

        long slot = key.GetHashCode() % _iBuckets;
        item** ppItem = &m_aT[slot];

        while (*ppItem) 
        {
            if ((*ppItem)->first == key) 
            {
                item* pKill = *ppItem;
                *ppItem = (*ppItem)->pNext;
                delete pKill;
                return true;
            }

            ppItem = &((*ppItem)->pNext);
        }

        return false;
    }

    int size() const
    {
        int nCount = 0;
        int len = _iBuckets;

        while (len--) 
        {
            for (const item* pItem = m_aT[len]; pItem; pItem = pItem->pNext) 
            {
                nCount++;
            }
        }

        return nCount;
    }

    KEY get(int iIndex) const
    {
        int pos = 0;
        int len = _iBuckets;

        while (len--) 
        {
            for (item* pItem = m_aT[len]; pItem; pItem = pItem->pNext) 
            {
                if (pos++ == iIndex) 
                {
                    return pItem->first;
                }
            }
        }

        return NULL;
    }

    VAL& operator[] (const KEY key) const
    {
        iterator iter = find(key);
        return iter->second;
    }

    void clear()
    {
        if (m_aT) 
        {
            int len = _iBuckets;

            while (len--) 
            {
                item* pItem = m_aT[len];

                while (pItem) 
                {
                    item* pKill = pItem;
                    pItem = pItem->pNext;
                    delete pKill;
                }
            }

            delete [] m_aT;
            m_aT = NULL;
        }
    }

    unsigned int GetBuckets() const
    {
        return _iBuckets;
    }

protected:

    item** m_aT;
    unsigned int _iBuckets;
};

}

#endif
