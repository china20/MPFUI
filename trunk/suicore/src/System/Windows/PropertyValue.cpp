
#include <System/Windows/PropertyValue.h>

namespace suic
{

Object* PropValue::GetValue()
{
    return value;
}

PropValue& PropValue::operator=(const PropValue& Other)
{
    if (&Other != this)
    {
        conditions = Other.conditions;
        valueType = Other.valueType;
        SETREFOBJ(value, Other.value);
        prop = Other.prop;
        childName = Other.childName;
    }

    return *this;
}

PropValue::PropValue()
    : prop(NULL)
    , value(NULL)
{
}

PropValue::PropValue(const PropValue& Other)
{
    value = Other.value;
    ADDREFOBJ(value);

    valueType = Other.valueType;
    conditions = Other.conditions;
    childName = Other.childName;
    prop = Other.prop;
}

PropValue::PropValue(Object* val)
    : prop(NULL)
{
    value = val;
    ADDREFOBJ(value);
}

PropValue::~PropValue()
{
    FREEREFOBJ(value);
}

void PropValue::SetValue(Object* val)
{
    SETREFOBJ(value, val);
}

}

