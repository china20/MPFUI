// 华勤科技版权所有 2010-2012
// 
// 文件名：Array.h
// 功  能：数组实现。
// 
// 作  者：MPF开发组
// 时  间：2011-05-01
// 
// ============================================================================

#ifndef _UIARRAY_H_
#define _UIARRAY_H_

#include <System/Tools/Math.h>
#include <System/Tools/Memory.h>
#include <System/Windows/Object.h>

namespace suic
{

template<typename T>
class ValEqual
{
public:

    static int Compare(const T& t1, const T& t2)
    {
        if (t1 == t2)
        {
            return 0;
        }
        else
        {
            return t1 < t2 ? -1 : 1;
        }
    }
};

template<typename T>
class ObjEqual
{
public:

    static int Compare(const T& t1, const T& t2)
    {
        if (t1.Equals(&t2))
        {
            return 0;
        }
        else
        {
            return t1 < t2 ? -1 : 1;
        }
    }
};

class SUICORE_API array_base
{
public:

    array_base() 
        : _length(0)
        , _capacity(0)
    {
    }

    int Length() const
    {
        return _length;
    }

    int GetCount() const
    {
        return _length;
    }

    int Capacity() const
    {
        return _capacity;
    }

protected:

    int _length;
    int _capacity;
};

struct ArraySort
{
    template<typename T, typename C>
    static void QuickSort(T* keys, int left, int right, C* c)
    {
        do
        {
            int a = left;
            int b = right;
            int num3 = a + ((b - a) >> 1);

            SwapWithQuickSort<T, C>(keys, a, num3, c);
            SwapWithQuickSort<T, C>(keys, a, b, c);
            SwapWithQuickSort<T, C>(keys, num3, b, c);

            T y = keys[num3];

            do
            {
                while (c->Compare(keys[a], y) < 0)
                {
                    a++;
                }
                while (c->Compare(y, keys[b]) < 0)
                {
                    b--;
                }
                if (a > b)
                {
                    break;
                }
                if (a < b)
                {
                    T local2 = keys[a];
                    keys[a] = keys[b];
                    keys[b] = local2;
                }
                a++;
                b--;
            }
            while (a <= b);
            if ((b - left) <= (right - a))
            {
                if (left < b)
                {
                    QuickSort(keys, left, b, c);
                }
                left = a;
            }
            else
            {
                if (a < right)
                {
                    QuickSort(keys, a, right, c);
                }
                right = b;
            }
        }
        while (left < right);
    }

    template<typename T, typename C>
    static void SwapWithQuickSort(T* keys, int a, int b, C* c)
    {
        if ((a != b) && (c->Compare(keys[a], keys[b]) > 0))
        {
            T local = keys[a];
            keys[a] = keys[b];
            keys[b] = local;
        }
    }
};

template<typename T, bool construct=false, typename Base=HoldObj>
struct ArrayType : public Base
{
public:

    T* data;
    int length;

    ArrayType() 
        : data(NULL)
        , length(0)
    {
    }

    ~ArrayType()
    {
        Clear();
    }

    T& operator[](int index) const
    {
        return (data)[index];
    }

    bool IsValid() const
    {
        return (data != 0);
    }

    int Length() const
    {
        return length;
    }

    void Reset(int size, bool zero)
    {
        FreeArray();
        data = (T*)suic::AllocMem(sizeof(T) * size);
        length = size;
        if (construct)
        {
            Fill(0, size, T());
        }
        else if (zero)
        {
            suic::MemZero(data, sizeof(T) * length);
        }
    }

    void Resize(int size)
    {
        int oldSize = length;
        if (size > length)
        {
            T* tmp = (T*)AllocMem(sizeof(T) * size);
            if (tmp)
            {
                suic::MemCopy(tmp, data, sizeof(T) * Length());
                suic::FreeMem((void*)data);
                length = size;
                data = tmp;
                if (construct)
                {
                    Fill(oldSize, size, T());
                }
                else
                {
                    suic::MemZero(data + oldSize, sizeof(T) * (length - oldSize));
                }
            }
        }
    }

    void Fill(int iStart, int iEnd, const T& val)
    {
        for (int i = iStart; i < iEnd; ++i)
        {
            Construct(data + i, val);
        }
    }

    void Attach(ArrayType& Other)
    {
        FreeArray();

        data = Other.data;
        length = Other.length;
        Other.data = NULL;
        Other.length = 0;
    }

    void Clear()
    {
        FreeArray();
        length = 0;
        data = NULL;
    }

    template<typename C>
    void Sort(C* c)
    {
        Sort<C>(0, Length(), c);
    }

    template<typename C>
    void Sort(int index, int length, C* c)
    {
        ArraySort::QuickSort<T, C>(data, index, index + length - 1, c);
    }

private:

    void Construct(T* ptr, const T& val)
    {
        void* vptr = ptr;
        new (vptr)T(val);
    }

    void FreeArray()
    {
        if (NULL != data)
        {
            if (construct)
            {
                for (int i = 0; i < length; ++i)
                {
                    T* obj = (data + i);
                    (obj)->~T();
                }
            }
            suic::FreeMem(data);
        }
    }
};

template<typename T, bool construct=false>
class Array : public array_base
{
public:

    Array() : _ptr(0)
    {
    }

    Array(int size)
    {
        _capacity = size;
        _ptr = Init(size);
    }

    ~Array()
    {
        Clear();
    }

    bool IsValid() const
    {
        return (_ptr != 0);
    }

    void Resize(int size)
    {
        int oldSize = _capacity;
        if (size > _capacity)
        {
            T* tmp(Init(size));
            if (tmp)
            {
                suic::MemCopy(tmp, _ptr, sizeof(T) * Length());
                suic::FreeMem(_ptr);
                _capacity = size;
                _ptr = tmp;
                if (construct)
                {
                    FillFrom(oldSize, size, T());
                }
                else
                {
                    suic::MemZero(_ptr + oldSize, sizeof(T) * (size - oldSize));
                }
            }
        }
    }

    void Relength(int size)
    {
        Resize(size);
        _length = size;
    }

    void Fill(int count, const T& val)
    {
        if (count > Length())
        {
            FillFrom(Length(), count, val);
        }
    } 

    void FillFrom(int pos, int iEnd, const T& val)
    {
        for (int i = pos; i < iEnd; ++i)
        {
            Construct(_ptr + i, val);
        }
    }

    T& operator[](int index) const
    {
        return (_ptr)[index];
    }

    T& GetItem(int index) const
    {
        return (_ptr)[index];
    }

    const T& SetItem(int index, const T& val)
    {
        // 先虚构
        Unconstruct(_ptr + index);
        Construct(_ptr + index, val);
        return val;
    }

    void RemoveLength(int len)
    {
        _length -= len;
    }

    void RemoveAt(int index)
    {
        if (construct)
        {
            Unconstruct(_ptr + index);
        }

        if (index < Length() - 1)
        {
            suic::MemMove(_ptr + index, _ptr + (index + 1), (Length() - index - 1) * sizeof(T));
        }
        _length--;
    }

    void RemoveRange(int index, int count)
    {
        count = UIMIN(count, (_length - index));
        if (construct)
        {
            for (int i = index; i < index + count; ++i)
            {
                Unconstruct(_ptr + i);
            }
        }
        if (index < Length() - 1)
        {
            suic::MemMove(_ptr + index, _ptr + (index + count), (Length() - index - count) * sizeof(T));
        }
        _length -= count;
    }

    const T& Insert(int index, const T& val)
    {
        T* newPtr = 0;

        if (Length() == Capacity())
        {
            _capacity = (int)((Length() + 1) * 1.6);
            // 扩大容量
            newPtr = Init(_capacity);

            suic::MemCopy(newPtr, _ptr, sizeof(T) * index);
            suic::MemCopy(newPtr + index + 1, _ptr + index, sizeof(T) * (Length() - index));

            suic::FreeMem(_ptr);
            _ptr = newPtr;
        }
        else
        {
            suic::MemMove(_ptr + index + 1, _ptr + index, sizeof(T) * (Length() - index));
        }

        Construct(_ptr + index, val);

        _length++;

        return val;
    }

    const T& Add(const T& val)
    {
        if (Length() == Capacity())
        {
            // 扩大容量
            Resize((int)((Length() + 1) * 1.6));
        }
        Construct(_ptr + Length(), val);
        _length++;
        return val;
    }

    T* GetItems() const
    {
        return _ptr;
    }

    template<typename C>
    void Sort(C* c)
    {
        Sort<C>(0, Length(), c);
    }

    template<typename C>
    void Sort(int index, int length, C* c)
    {
        ArraySort::QuickSort<T, C>(_ptr, index, index + length - 1, c);
    }

    template<typename T, typename C>
    static void Sort(T* keys, int index, int length, C* c)
    {
        ArraySort::QuickSort<T, C>(keys, index, index + length - 1, c);
    }

    static void Copy(Array* sourceArray, int sourceIndex, Array* destArray, int destIndex, int length)
    {
        if (destArray->Capacity() < (destIndex + length))
        {
            destArray->Resize(destIndex + length);
        }

        if (!construct)
        {
            MemCopy(destArray->_ptr + destIndex, sourceArray->_ptr + sourceIndex, sizeof(T) * length);
        }
        else
        {
            for (int i = 0; i < length; ++i)
            {
                destArray->SetItem(destIndex + i, sourceArray->GetItem(sourceIndex + i));
            }
        }
    }

    void CopyTo(Array* array, int index)
    {
        Copy(this, 0, array, index, Length());
    }

    void Clear()
    {
        if (_ptr)
        {
            if (construct)
            {
                for (int i = 0; i < Length(); ++i)
                {
                    Unconstruct(_ptr + i);
                }
            }

            suic::FreeMem(_ptr);

            _ptr = NULL;
            _length = 0;
            _capacity = 0;
        }
    }

    void Reset()
    {
        if (_ptr)
        {
            if (construct)
            {
                for (int i = 0; i < Length(); ++i)
                {
                    Unconstruct(_ptr + i);
                }
            }
            _length = 0;
        }
    }

private:

    T* Init(int size)
    {
        void* ptr = suic::AllocMem(sizeof(T) * size);
        return (T*)ptr;
    }

    void Unconstruct(T* ptr)
    {
        (ptr)->~T();
    }

    void Construct(T* ptr, const T& val)
    {
        void* vptr = ptr;
        new (vptr)T(val);
    }

private:

    T* _ptr;
};

typedef Array<Byte> ByteArray;

class SUICORE_API StringArray : public Array<String>
{
public:

    int SplitString(const String& str, const String& delims, Uint32 maxSplits = 0);
};

class FloatAutoArray : public Array<Float>
                     , public Object
{
public:

    FloatAutoArray();

    RTTIOfClass(FloatAutoArray)
        
    String ToString();
};

class IntAutoArray : public Array<int>
                   , public Object
{
public:

    IntAutoArray();

    RTTIOfClass(IntAutoArray)

    String ToString();
};

}

#endif
