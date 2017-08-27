
#include <System/Windows/Expression.h>
#include <System/Windows/DpProperty.h>
#include <System/Windows/Binding.h>
#include <System/Windows/MemberVisitor.h>

#include <System/Tools/Debug.h>
#include <System/Types/Structure.h>
#include <Framework/Controls/ItemCollection.h>

namespace suic
{

Object* Expression::NoValue = new Object(false);

ImplementRTTIOfClass(NotifyPropChanged, Object)

typedef String (Object::*PFuncStringCall)();

union mmpf
{
    FnMemberCall pf;

    int (Object::*int_get_pf)();
    void (Object::*int_set_pf)(int);

    Float (Object::*float_get_pf)();
    void (Object::*float_set_pf)(Float);

    bool (Object::*bool_get_pf)();
    void (Object::*bool_set_pf)(bool);

    Uint32 (Object::*uint32_get_pf)();
    void (Object::*uint32_set_pf)(Uint32);

    String (Object::*string_get_pf)();
    void (Object::*string_set_pf)(String);

    Point (Object::*point_get_pf)();
    void (Object::*point_set_pf)(Point);

    Rect (Object::*rect_get_pf)();
    void (Object::*rect_set_pf)(Rect);

    Size (Object::*size_get_pf)();
    void (Object::*size_set_pf)(Size);

    Object* (Object::*obj_get_pf)();
    void (Object::*obj_set_pf)(Object*);

    bool (Object::*attri_get_pf)(const String&, ObjectPtr&);
    bool (Object::*attri_set_pf)(const String&, Object*);
};

NotifyPropChanged::NotifyPropChanged()
{

}

NotifyPropChanged::~NotifyPropChanged()
{

}

void NotifyPropChanged::AddListener(Object* target)
{
    for (int i = 0; i < _listeners.GetCount(); ++i)
    {
        const WeakRef& wObj = _listeners[i];
        if (wObj.GetTarget() == target)
        {
            return;
        }
    }
    _listeners.Add(WeakRef(target));
}

void NotifyPropChanged::RemoveListener(Object* target)
{
    for (int i = 0; i < _listeners.GetCount(); ++i)
    {
        const WeakRef& wObj = _listeners[i];
        if (wObj.GetTarget() == target)
        {
            _listeners.RemoveAt(i);
            break;
        }
    }
}

void NotifyPropChanged::ClearListeners()
{
    _listeners.Clear();
}

static const MemberEntry* __FindMemberFunc(const MemberInfo* pFuncEntry, const String& name)
{
    const MemberEntry* fcall = NULL;
    while (pFuncEntry != NULL)
    {
        int iEntry = 0;
        for (;;)
        {
            const MemberEntry* pEntry = (pFuncEntry->fcall + iEntry);

            if (pEntry->type == 0)
            {
                break;
            }

            if (pEntry->type == suic::UIDataType::AttriType || 
                pEntry->name.Equals(name))
            {
                fcall = pEntry;
                break;
            }

            iEntry++;
        }

        if (NULL != fcall)
        {
            break;
        }
        else
        {
            pFuncEntry = pFuncEntry->fnGetMemberInfo();
        }
    }
    return fcall;
}

void NotifyPropChanged::NotifyChanged(const String& name)
{
    if (_listeners.GetCount() > 0)
    {
        PropertyChangedEventArg e(name);
        for (int i = 0; i < _listeners.GetCount(); ++i)
        {
            const WeakRef& target = _listeners[i];
            if (target.IsAlive())
            {
                FrameworkElement* fe = (FrameworkElement*)target.GetTarget();
                fe->OnDataPropertyChanged(this, &e);
            }
            else
            {
                _listeners.RemoveAt(i);
                --i;
            }
        }
    }
}

bool Object::SetMember(const String& name, Object* val)
{
    bool bExist = true;
    ObjectPtr cachedObj(val);
    const MemberEntry* fcall = __FindMemberFunc(GetMemberInfo(), name);

    if (NULL != fcall && NULL != fcall->setfunc)
    {
        mmpf _mmf;
        _mmf.pf = fcall->setfunc;

        switch (fcall->type)
        {
        case UIDataType::IntType:
            (this->*_mmf.int_set_pf)(val->ToInt());
            break;

        case UIDataType::FloatType:
            (this->*_mmf.float_set_pf)(val->ToFloat());
            break;

        case UIDataType::BoolType:
            (this->*_mmf.bool_set_pf)(val->ToBool());
            break;

        case UIDataType::Uint32Type:
            (this->*_mmf.uint32_set_pf)((Uint32)val->ToInt());
            break;

        case UIDataType::StringType:
            (this->*_mmf.string_set_pf)(val->ToString());
            break;

        case UIDataType::PointType:
            (this->*_mmf.point_set_pf)(OPoint::FromObj(val));
            break;

        case UIDataType::RectType:
            (this->*_mmf.rect_set_pf)(ORect::FromObj(val));
            break;

        case UIDataType::SizeType:
            (this->*_mmf.size_set_pf)(OSize::FromObj(val));
            break;

        case UIDataType::ObjectType:
            (this->*_mmf.obj_set_pf)(val);
            break;

        case UIDataType::AttriType:
            bExist = (this->*_mmf.attri_set_pf)(name, val);
            break;

        default:
            bExist = false;
        }
    }
    else
    {
        bExist = false;
    }

    return bExist;
}

bool Object::GetMember(const String& name, ObjectPtr& retVal)
{
    bool bExist = true;
    const MemberEntry* fcall = __FindMemberFunc(GetMemberInfo(), name);

    if (NULL != fcall && fcall->getfunc != NULL)
    {
        mmpf _mmf;

        _mmf.pf = fcall->getfunc;

        switch (fcall->type)
        {
        case UIDataType::IntType:
            retVal = new Integer((this->*_mmf.int_get_pf)());
            break;

        case UIDataType::FloatType:
            retVal = new OFloat((this->*_mmf.float_get_pf)());
            break;

        case UIDataType::BoolType:
            retVal = BOOLTOBOOLEAN((this->*_mmf.bool_get_pf)());
            break;

        case UIDataType::Uint32Type:
            retVal = new Integer((this->*_mmf.uint32_get_pf)());
            break;

        case UIDataType::StringType:
            retVal = new OString((this->*_mmf.string_get_pf)());
            break;

        case UIDataType::PointType:
            retVal = new OPoint((this->*_mmf.point_get_pf)());
            break;

        case UIDataType::RectType:
            retVal = new ORect((this->*_mmf.rect_get_pf)());
            break;

        case UIDataType::SizeType:
            retVal = new OSize((this->*_mmf.size_get_pf)());
            break;

        case UIDataType::ObjectType:
            retVal = (this->*_mmf.obj_get_pf)();
            break;

        case UIDataType::AttriType:
            bExist = (this->*_mmf.attri_get_pf)(name, retVal);
            break;

        default:
            bExist = false;
        }
    }
    else
    {
        bExist = false;
    }

    return bExist;
}

//--------------------------------------------------------

Expression::Expression()
{
    _enabled = true;
}

Expression::~Expression()
{

}

DpObject* Expression::GetSource()
{
    return dynamic_cast<DpObject*>(_source.GetTarget());
}

void Expression::SetSource(DpObject* d)
{
    _source = WeakRef(d);
}

bool Expression::GetValue(DpObject* d, DpProperty* dp, ObjectPtr& val)
{ 
    val = DpProperty::UnsetValue();
    return false;
}

void Expression::OnAttach(DpObject* d, DpProperty* dp)
{

}

void Expression::OnDetach(DpObject* d, DpProperty* dp)
{

}

bool Expression::SetValue(DpObject* d, DpProperty* dp, Object* value) 
{
    return false;
}

bool Expression::OnUpdateProp(DpObject* d, DpProperty* dp)
{
    return true;
}

}
