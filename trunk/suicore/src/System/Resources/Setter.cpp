// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
//
// ======================================================================

#include <System/Resources/Setter.h>

namespace suic
{

ImplementRTTIOfClass(Setter, DpObject)

Setter::Setter()
    : _property(NULL)
    , _value(DpProperty::UnsetValue())
{
    _value->ref();
}

Setter::~Setter()
{
    if (_value)
    {
        _value->unref();
    }
}

void Setter::Seal()
{
    DpObject::Seal();
} 

void Setter::SetProperty(DpProperty* dp)
{ 
    CheckValidProperty(dp);
    CheckSealed(); 
    _property = dp;

    SetUnresolvedProperty(dp->GetName());
}

void Setter::SetValue(Object* val) 
{
    if (val == DpProperty::UnsetValue() || val == NULL)
    {
        throw ArgumentException(_U("SetterValueCannotBeUnset!"), __FILELINE__); 
    }

    CheckSealed(); 
    SETREFOBJ(_value, val);
}


void Setter::CheckSealed() 
{
    if (IsSealed())
    {
        throw InvalidOperationException(_U("CannotChangeAfterSealed"), __FILELINE__); 
    }
} 

void Setter::Initialize(DpProperty* property, Object* value, String target) 
{
    /*if (value == DpProperty::UnsetValue())
    {
        throw ArgumentException(_U("SetterValueCannotBeUnset")); 
    }

    if (_value)
    {
        _value->__release();
    }

    CheckValidProperty(property); 

    _property = property;
    _value = value;
    _target = target;

    _value->__addRef();*/
}

void Setter::CheckValidProperty(DpProperty* prop) 
{
    if (prop == NULL) 
    {
        throw ArgumentNullException(_U("property"), __FILELINE__);
    }

    /*if (prop == FrameworkElement::NameProperty)
    {
        throw InvalidOperationException(_U(""));
    }*/ 
}

}
