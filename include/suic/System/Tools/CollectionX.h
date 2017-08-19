// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����CollectionX.h
// ��  �ܣ�ʵ��������ݽṹ��
// 
// ��  �ߣ�MPF������(����)
// ʱ  �䣺2010-07-02
// 
// ============================================================

#ifndef __UICOLLECTIONREF_H
#define __UICOLLECTIONREF_H

#include <System/Tools/Array.h>
#include <System/Tools/Collection.h>
#include <System/Tools/HashHelper.h>
#include <System/Windows/DpObject.h>

namespace suic
{

class SUICORE_API XStack : public Object
{
private:

    // ջ������
    int _size;
    int _version;

    Array<Object*> _items;

public:

    RTTIOfClass(XStack)

    XStack();
    XStack(int initialCapacity);

    int GetCount() const;
    void Push(Object* val);

    void Pop(ObjectPtr& obj);
    Object* Peek();

    void Clear();

protected:

    void OnClear();
};

class SUICORE_API XArray : public Object
{
public:

    RTTIOfClass(XArray)

    XArray();
    XArray(int capacity);
    ~XArray();

    void Resize(int size);
    void Relength(int size);

    int GetCount() const;
    int Add(Object* value);

    bool Contains(Object* item);

    Object* GetItem(int index) const;
    void SetItem(int index, Object* value);

    int IndexOf(Object* value);
    int IndexOf(Object* value, int startIndex);
    int IndexOf(Object* value, int startIndex, int count);
    
    void Insert(int index, Object* value);

    bool Remove(Object* obj);
    void RemoveAt(int index);
    void RemoveRange(int index, int count);

    void Clear();
    void Dispose();

    template<typename C>
    void Sort(C* comparer)
    {
        _items.Sort<C>(comparer);
    }

    template<typename C>
    void Sort(int index, int length, C* comparer)
    {
        _items.Sort<C>(index, length, comparer);
    }

private:

    Array<Object*> _items;
};

typedef shared<XArray> XArrayPtr;

inline int XArray::GetCount() const
{
    return _items.Length();
}

inline Object* XArray::GetItem(int index) const
{
    return _items.GetItem(index);
}

class SUICORE_API XQueue : public Object
{
public:

    RTTIOfClass(XQueue)

    XQueue();
    XQueue(int capacity);
    XQueue(int capacity, Float growFactor);

public:

    ObjectPtr Dequeue();
    Object* Peek();
    void Enqueue(Object* obj);

    bool Contains(Object* obj);

    void Clear();

    Object* ToArray();
    void TrimToSize();

    int GetCount();

protected:

    Object* GetElement(int i);

private:

    void SetCapacity(int capacity);
    void Init(int capacity, Float growFactor);

private:

    int _head;
    int _tail;
    int _size;
    int _version;
    Float _growFactor;
    Array<Object*> _items;
};

class SUICORE_API XDictionary : public StringAutoDic<Object>
    , public Object
{
public:

    RTTIOfClass(XDictionary)

    XDictionary() {}
};

}

#endif
