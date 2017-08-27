/////////////////////////////////////////////////////////////////////////
// uiobject.cpp

#include <System/Windows/WeakRef.h>
#include <System/Windows/WeakRefObj.h>

namespace suic
{

void WeakHolder::unref()
{
    refCount--;
    if (0 == refCount)
    {
        if (NULL != target)
        {
            target->_holder = NULL;
        }
        target = NULL;
        delete this;
    }
}

WeakRef::WeakRef()
    : _holder(NULL)
{
}

WeakRef::WeakRef(Object* const target)
{
    if (target != NULL)
    {
        CheckHolder(target);
        _holder = target->_holder;
        _holder->ref();
    }
    else
    {
        _holder = NULL;
    }
}

WeakRef::~WeakRef()
{
    unref();
}

bool WeakRef::TestTag(Object* o, Byte f)
{
    return ((o->_flag & f) == f);
}

void WeakRef::SetTag(Object* o, Byte f, bool add)
{
    if (f > 4)
    {
        if (add)
        {
            o->_flag |= f;
        }
        else
        {
            o->_flag &= ~f;
        }
    }
}

void WeakRef::ref()
{
    if (_holder != NULL)
    {
        _holder->ref();
    }
}

void WeakRef::unref()
{
    if (NULL != _holder)
    {
        _holder->unref();
        _holder = NULL;
    }
}

void WeakRef::CheckHolder(Object* target)
{
    if (target != NULL && target->_holder == NULL)
    {
        target->_holder = new WeakHolder(target);
    }
}

WeakRef::WeakRef(const WeakRef& Other)
{
    _holder = Other._holder;
    ref();
}

WeakRef& WeakRef::operator=(Object* const target)
{
    if (NULL == target)
    {
        unref();
    }
    else
    {
        CheckHolder(target);

        if (_holder != target->_holder)
        {
            unref();
            _holder = target->_holder;
            ref();
        }
    }

    return *this;
}

WeakRef& WeakRef::operator=(const WeakRef& Other)
{
    if (_holder != Other._holder)
    {
        unref();
        _holder = Other._holder;
        ref();
    }

    return *this;
}

bool WeakRef::operator!=(const WeakRef& Other)
{
    return !Equal(Other);
}

bool WeakRef::operator==(const WeakRef& Other)
{
    return Equal(Other);
}

bool WeakRef::Equal(const WeakRef& Other)
{
    return _holder == Other._holder;
}

bool WeakRef::IsAlive() const
{
    return (GetTarget() != NULL);
}

//------------------------------------------------------------
//

ImplementRTTIOfClass(WeakRefObj, Object)

WeakRefObj::WeakRefObj()
{

}

WeakRefObj::WeakRefObj(Object* target)
{
    _target = WeakRef(target);
}

WeakRefObj::WeakRefObj(const WeakRef& Other)
{
    _target = WeakRef(Other);
}

WeakRefObj::~WeakRefObj()
{
}

WeakRefObj WeakRefObj::operator=(const WeakRef& Other)
{
    _target = Other;
    return *this;
}

bool WeakRefObj::operator!=(const WeakRef& Other)
{
    return _target != Other;
}

bool WeakRefObj::operator==(const WeakRef& Other)
{
    return _target == Other;
}

bool WeakRefObj::Equals(Object* Other)
{
    WeakRefObj* weakObj = RTTICast<WeakRefObj>(Other);
    return weakObj && Equal(weakObj->_target);
}

bool WeakRefObj::Equal(const WeakRef& Other)
{
    return _target.GetTarget() == Other.GetTarget();
}

bool WeakRefObj::IsAlive()
{
    return _target.IsAlive();
}

Object* WeakRefObj::GetTarget()
{
    return _target.GetTarget();
}

WeakRefObj::operator Object*()
{
    return _target.GetTarget();
}

}
