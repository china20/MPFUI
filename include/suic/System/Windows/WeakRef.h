// 华勤科技版权所有 2010-2011
// 
// 文件名：WeakRef
// 功  能：实现弱引用对象。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIWEAKREF_H_
#define _UIWEAKREF_H_

#include <System/Types/Micro.h>
#include <System/Types/Const.h>

namespace suic
{

class Object;

class WeakHolder
{
public:

    WeakHolder(Object* t)
    {
        refCount = 0;
        target = t;
    }

    void ref()
    {
        refCount++;
    }

    void unref();

private:

    int refCount;
    Object* target;
    
    friend class Object;
    friend class WeakRef;
};

class SUICORE_API WeakRef
{
public:

    WeakRef();    
    WeakRef(const WeakRef& Other);
    explicit WeakRef(Object* const target);

    ~WeakRef();

    WeakRef& operator=(Object* const target);
    WeakRef& operator=(const WeakRef& Other);

    bool operator!=(const WeakRef& Other);
    bool operator==(const WeakRef& Other);
    bool Equal(const WeakRef& Other);

    bool IsAlive() const;
    Object* GetTarget() const;

    template<typename T>
    T* GetElement() const
    {
        return dynamic_cast<T*>(GetTarget());
    }

    operator Object*() const;

    static bool TestTag(Object* o, Byte f);
    static void SetTag(Object* o, Byte f, bool add);

private:

    void ref();
    void unref();

    void CheckHolder(Object* target);

private:

    WeakHolder* _holder;
    friend class Object;
};

inline WeakRef::operator Object*() const
{
    return GetTarget();
}

inline Object* WeakRef::GetTarget() const
{
    if (NULL == _holder)
    {
        return NULL;
    }
    else
    {
        return _holder->target;
    }
}

class WeakRefKey
{
public:

    int _hashCode;
    WeakRef _weakRef;

    int GetHashCode();
    bool Equals(Object* Other);
};

}

#endif
