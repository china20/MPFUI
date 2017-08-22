// 华勤科技版权所有 2010-2011
// 
// 文件名：Memory.h
// 功  能：实现内存相关操作。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef __UIMEMORY_H
#define __UIMEMORY_H

#pragma warning(disable : 4244)

#include <new.h>
#include <Config/suicoreconfig.h>

namespace suic
{

typedef unsigned int _size_type;

SUICORE_API void* AllocMem(int);
SUICORE_API void FreeMem(void*);

SUICORE_API void MemZero(void* ptr, int size);
SUICORE_API void MemMove(void* dst, void* src, int size);
SUICORE_API void MemCopy(void* dst, void* src, int size);

template<class T>
void swap(T& _Left, T& _Right)
{
    T _Val = _Left;
    _Left = _Right;
    _Right = _Val;
}

template<class T> inline
T* __allocate(_size_type count, T*)
{
    if (count <= 0)
    {
        count = 0;
    }
    else if (((_size_type)(-1) / count) < sizeof (T))
    {
    }

    return ((T*)AllocMem(count * sizeof (T)));
}

template<class T1, class T2> inline
void __construct(T1* ptr, const T2& val)
{	
    void* vptr = ptr;
    new (vptr)T1(val);
}

template<class T1, class T2> inline
void __uninit_copy(T1 first, T1 last, T2 dest)
{
    size_t count = (size_t)(last - first);

    T2 result = dest + count;

    if (count > 0)
    {
        MemMove(&*dest, &*first, count * sizeof(*first));
    }
}

}

#endif
