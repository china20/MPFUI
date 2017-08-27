// =============================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// =============================================================

#include <Framework/Animation/PropertyAnimation.h>

namespace suic
{

PropertyAnimate::PropertyAnimate(DpProperty* property)
    : _property(property)
    , _relayout(false)
{
}

PropertyAnimate::~PropertyAnimate()
{

}

void PropertyAnimate::SetProp(DpProperty* prop)
{
    _property = const_cast<DpProperty*>(prop);
}

DpProperty* PropertyAnimate::GetProp()
{
    return _property;
}

}
