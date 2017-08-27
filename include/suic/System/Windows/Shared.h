// 华勤科技版权所有 2008-2022
// 
// 文件名：UIShared
// 功  能：定义核心库的智能指针对象。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UISHARED_H_
#define _UISHARED_H_

#include <Config/suicoreconfig.h>

namespace suic
{

template <typename T>
class autoshared
{
public:

    autoshared() : _obj(0)
    {
    }

    autoshared(T* ptr) : _obj(ptr)
    {
        if (_obj)
        {
            _obj->ref();
        }
    }

    autoshared(autoshared& myObj)
    {
        _obj = myObj.detach();
    }

    ~autoshared ()
    {
        if (NULL != _obj)
        {
            _obj->unref();
        }
    }

    autoshared& operator=(T* other)
    {
        if (NULL != other)
        {
            other->ref();
        }
        if (NULL != _obj)
        {
            _obj->unref();
        }
        _obj = other;
        return (*this);
    }

    autoshared& operator=(autoshared& myObj)
    {
        if (this != &myObj)
        {
            T* oldObj = _obj;
            _obj = myObj.detach();
            if (NULL != oldObj)
            {
                oldObj->unref();
            }
        }
        return (*this);
    }

    inline T& operator *()
    {
        return (*_obj);
    }

    inline T* operator->() const
    {
        return _obj;
    }

    bool operator==(const autoshared& other) const
    {
        return (other._obj == _obj);
    }

    bool operator!=(const autoshared& other) const
    {
        return (other._obj != _obj);
    }

    bool operator!=(T * other) const
    {
        return (other != _obj);
    }

    T* get() const
    {
        return _obj;
    }

    T* detach()
    {
        T* ret = _obj;
        _obj = NULL;
        return ret;
    }

private:

    T* _obj;
};

template <typename T>
class shared
{
public:

    shared() : _obj(0)
    {
    }

    shared(T* ptr) : _obj(ptr)
    {
        if (_obj)
        {
            _obj->ref();
        }
    }

    shared(const shared& myObj)
    {
        if (this != &myObj)
        {
            __setFirstPtr (myObj._obj);
        }
    }

    shared(autoshared<T>& rp)
    {
        _obj = rp.detach();
    }

    template<typename P>
    shared(const shared<P> & rp)
    {
        __setFirstPtr (dynamic_cast<T*>(rp._obj));
    }

    template<typename P>
    shared(P* rp)
    {
        __setFirstPtr(dynamic_cast<T*>(rp));
    }

    ~shared ()
    {
        if (_obj)
        {
            _obj->unref();
        }
    }

    shared& operator=(T* other)
    {
        __setSharePtr(other);
        return (*this);
    }

    shared& operator=(const shared& myObj)
    {
        if (this != &myObj)
        {
            __setSharePtr (myObj._obj);
        }
        return (*this);
    }

    shared& operator=(autoshared<T>& rp)
    {
        if (_obj)
        {
            _obj->unref();
        }
        _obj = rp.detach();
        return (*this);
    }

    template<typename P>
    shared& operator=(const shared<P>& rp)
    {
        __setSharePtr(dynamic_cast<T*>(rp._obj));
        return (*this);
    }
    template<typename P>
    shared& operator=(P* rp)
    {
        __setSharePtr(dynamic_cast<T*>(rp));
        return (*this);
    }

    inline T& operator *()
    {
        return (*_obj);
    }

    inline T* operator->() const
    {
        return _obj;
    }

    inline T* get() const
    {
        return _obj;
    }

    operator bool() const
    {
        return _obj ? true : false;
    }

    bool operator<(const shared& other) const
    {
        return (other._obj < _obj);
    }

    bool operator>(const shared& other) const
    {
        return (other._obj > _obj);
    }

    bool operator==(const shared& other) const
    {
        return (other._obj == _obj);
    }

    bool operator!=(const shared& other) const
    {
        return (other._obj != _obj);
    }

    bool operator!=(T * other) const
    {
        return (other != _obj);
    }

    void swap(shared& other)
    {
    }

    void attach (T* ptr)
    {
        __setSharePtr (ptr);
    }

    template<class P>
    static shared cast(const shared<P>& rp)
    {
        return shared(dynamic_cast<T*>(rp._obj));
    }

    template<class P>
    static shared cast(P* rp)
    {
        return shared(dynamic_cast<T*>(rp));
    }

    void __setSharePtr(T* ptr)
    {
        if (_obj != ptr)
        {
            T * op = _obj;
            _obj = ptr;
            if (_obj)
            {
                _obj->ref();
            }
            if (op)
            {
                op->unref();
            }
        }
    }

    void __setFirstPtr (T* ptr)
    {
        if (0 != (_obj = ptr))
        {
            _obj->ref();
        }
    }

    T*	_obj;
};

template<typename L, typename R>
inline bool operator==(const shared<L>& lp, const shared<R>& rp)
{
    const L * l = lp.get();
    const R * r = rp.get();
    if(l && r)
    {
        return l == r;
    }
    return !l && !r;
}

template<typename L, typename R>
inline bool operator!=(const shared<L>& lp, const shared<R>& rp)
{
    return !operator==(lp, rp);
}

/////////////////////////////////////////////////////////////////////////
template<typename L, typename R>
inline bool operator==(const shared<L>& lp, const R* rp)
{
    L * l = lp.get();
    if(l && rp)
    {
        return l == rp;
    }
    return !l && !rp;
}

template<typename L, typename R>
inline bool operator!=(const shared<L>& lp, const R* rp)
{
    return !operator==(lp, rp);
}

/////////////////////////////////////////////////////////////////////////
template<typename L, typename R>
inline bool operator==(const L* lp, const shared<R>& rp)
{
    R * r = rp.get();
    if(lp && r)
    {
        return lp == r;
    }
    return !lp && !r;
}

template<typename L, typename R>
inline bool operator!=(const L* lp, const shared<R>& rp)
{
    return !operator==(lp, rp);
}

};

#endif
