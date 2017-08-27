// 华勤科技版权所有 2008-2022
// 
// 文件名：Comparer.h
// 功  能：定义数据结构。
// 
// 作  者：MPF开发组(汪荣)
// 时  间：2010-07-02
// 
// ============================================================

#ifndef __UICOMPARER_H
#define __UICOMPARER_H

#include <System/Tools/Array.h>
#include <System/Tools/HashHelper.h>
#include <System/Windows/Exception.h>

namespace suic
{

template<typename T>
class IComparer
{
public:

    virtual ~IComparer() {}

    virtual bool Equals(T x, T y) = 0;
    virtual Uint32 GetHashCode(T obj) = 0;
};

template<typename T>
class ValComparer
{
public:

    ValComparer()
    {
    }

    bool Equals(const T& x, const T& y)
    {
        return x == y;
    }

    Uint32 GetHashCode(const T& key)
    {
        return (Uint32)key;
    }

    String KeyAsString(const T& key)
    {
        return String().Format(_U("%d"), (Uint32)key);
    }
};

class StrComparer
{
public:

    StrComparer()
    {

    }

    bool Equals(const String& x, const String& y)
    {
        return x.Equals(y);
    }

    Uint32 GetHashCode(const String& obj)
    {
        return (Uint32)obj.GetHashCode();
    }

    String KeyAsString(const String& key)
    {
        return key;
    }
};

template<typename T>
class ObjComparer
{
public:

    ObjComparer()
    {
    }

    bool Equals(const T& x, const T& y)
    {
        return (x->Equals(y));
    }

    Uint32 GetHashCode(const T& key)
    {
        return (Uint32)key->GetHashCode();
    }

    String KeyAsString(const T& key)
    {
        return String().Format(_U("%d"), GetHashCode(key));
    }
};

template<typename T>
class PtrComparer
{
public:

    PtrComparer()
    {
    }

    bool Equals(const T& x, const T& y)
    {
        return (x == y);
    }

    Uint32 GetHashCode(const T& key)
    {
        return (Uint32)(DWORD_PTR)key;
    }

    String KeyAsString(const T& key)
    {
        return String().Format(_U("%d"), GetHashCode(key));
    }
};

template<typename TKey, typename TVal>
struct Entry
{
    int hashCode;
    int next;

    TKey key;
    TVal val;
    
    Entry()
    {
        next = 0;
        hashCode = 0;
    }
};

}

#endif
