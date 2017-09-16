// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Binding.cpp

#include <System/Types/Const.h>
#include <System/Types/Micro.h>
#include <System/Windows/CoreTool.h>
#include <System/Windows/Binding.h>
#include <System/Windows/BuildFactory.h>
#include <System/Windows/BindingExpression.h>
#include <System/Resources/BindParser.h>
#include <System/Tools/VisualTreeOp.h>

namespace suic
{

ImplementRTTIOfClass(BindingBase, Extension)
ImplementRTTIOfClass(Binding, BindingBase)

PropertyPath::PropertyPath()
{

}

PropertyPath::PropertyPath(String val)
    : Path(val)
{
    
}

Binding::Binding()
    : _bindMode(OneWay)
    , _prop(NULL)
    , _refSource(NULL)
    , _converter(NULL)
    , _sourceType(BindingSource::Unknown)
    , _triggerType(UpdateSourceTrigger::ustDefault)
{

}

Binding::Binding(String path)
    : _bindMode(OneWay)
    , _prop(NULL)
    , _refSource(NULL)
    , _converter(NULL)
    , _sourceType(BindingSource::Unknown)
    , _triggerType(UpdateSourceTrigger::ustDefault)
{
    if (!path.Empty())
    {
        _ppath.Path = path;
    }
}

Binding::~Binding()
{
    if (_refSource)
    {
        delete _refSource;
    }
    if (_converter)
    {
        _converter->unref();
    }
}

void Binding::SetConverterParam(const String& val)
{
    _convertParam = val;
}

String Binding::GetConverterParam() const
{
    return _convertParam;
}

void Binding::SetConverterName(const String& val)
{
    _convertName = val;
    SetConverter(BindConverter::Ins()->Lookup(val));
}

String Binding::GetConverterName() const
{
    return _convertName;
}

void Binding::SetConverter(IBindConverter* val)
{
    SETREFOBJ(_converter, val);
}

BindingExpression* Binding::CreateBindingExpression()
{
    BindingExpression* expr = new BindingExpression();
    expr->SetBinding(this);
    return expr;
}

void Binding::ProvideValue(DpObject* d, DpProperty* dp, ObjectPtr& obj)
{
    obj = CreateBindingExpression();
}

void Binding::ParseKeyValue(const String& keyval)
{
    StringArray vecTmp;
    vecTmp.SplitString(keyval, _U("="));

    if (vecTmp.Length() == 2)
    {
        String key = vecTmp[0];
        String val = vecTmp[1];

        key.Trim();
        val.Trim();

        if (key.Equals(_U("Mode")))
        {
            if (val.Equals(_U("OneWay")))
            {
                SetBindMode(BindingMode::OneWay);
            }
            else if (val.Equals(_U("TwoWay")))
            {
                SetBindMode(BindingMode::TwoWay);
            }
            else if (val.Equals(_U("OneWayToSource")))
            {
                SetBindMode(BindingMode::OneWayToSource);
            }
            else
            {
                SetBindMode(BindingMode::OneTime);
            }
        }
        else if (key.Equals(_U("UpdateSourceTrigger")))
        {
            if (val.Equals(_U("PropertyChanged")))
            {
                SetUpdateSourceTrigger(UpdateSourceTrigger::ustPropertyChanged);
            }
            else if (val.Equals(_U("LostFocus")))
            {
                SetUpdateSourceTrigger(UpdateSourceTrigger::ustLostFocus);
            }
            else if (val.Equals(_U("Explicit")))
            {
                SetUpdateSourceTrigger(UpdateSourceTrigger::ustExplicit);
            }
            else
            {
                SetUpdateSourceTrigger(UpdateSourceTrigger::ustDefault);
            }
        }
        else if (key.Equals(_U("ElementName")))
        {
            _target = val;
            _sourceType = BindingSource::ElementName;
        }
        else if (key.Equals(_U("RelativeSource")))
        {
            StringArray vecVal;
            String strVal = val;

            strVal.Trim();
            strVal = strVal.Substring(1, strVal.Length() - 2);
            vecVal.SplitString(strVal, _U(" "));

            _sourceType = BindingSource::RelativeTemplatedParent;
        }
    }
}

PropertyPath Binding::GetPath()
{
    return _ppath;
}

void Binding::SetPath(const PropertyPath& path)
{
    _ppath = path;
}

DpObject* Binding::GetSource()
{
    return RTTICast<DpObject>(_source.GetTarget());
}

void Binding::SetSource(DpObject* d)
{
    if (d != NULL)
    {
        _source = d;
    }
    else
    {
        _source = WeakRef();
    }
}

SourceRef* Binding::GetSourceRef()
{
    return _refSource;
}

void Binding::SetSourceRef(SourceRef* d)
{
    if (NULL != _refSource)
    {
        delete _refSource;
    }
    _refSource = d;
}

//================================================================
//

Object* ObjectSource::GetSource(DpObject* target, DpProperty* targetDp, bool bFirst)
{
    return NULL;
}

Object* ElementNameSource::GetSource(DpObject* target, DpProperty* targetDp, bool bFirst)
{
    Object* source = NULL;
    FrameworkElement* fe = RTTICast<FrameworkElement>(target);

    if (NULL != fe)
    {
        if (bFirst)
        {
            source = fe->GetTemplateChild(Name);

            if (NULL == source)
            {
                Element* fee = fe->GetUIParent();
                while (fee != NULL)
                {
                    if (fee->GetName().Equals(Name))
                    {
                        source = fee;
                        break;
                    }

                    if (fee->GetUIParent() == NULL)
                    {
                        fee = CASTTOFE(fee)->GetParent();
                    }
                    else
                    {
                        fee = fee->GetUIParent();
                    }
                }
            }

            if (source == NULL)
            {
                fe = CASTTOFE(fe->GetTemplatedParent());
                while (NULL != fe)
                {
                    source = fe->GetTemplateChild(Name);

                    if (NULL != source)
                    {
                        break;
                    }

                    fe = CASTTOFE(fe->GetTemplatedParent());
                }
            }
        }
        else
        {
            fe = CASTTOFE(VisualTreeOp::GetVisualRoot(target));
            if (NULL != fe)
            {
                source = fe->FindName(Name);
            }
        }
    }

    return source;
}

Object* RelativeSource::GetSource(DpObject* target, DpProperty* targetDp, bool bFirst)
{
    switch (Mode)
    {
    case RelativeSourceMode::Self:
        return target;

    case RelativeSourceMode::TemplatedParent:
        {
            FrameworkElement* fe = RTTICast<FrameworkElement>(target);
            if (NULL != fe)
            {
                if (AncestorLevel > 1)
                {
                    fe = RTTICast<FrameworkElement>(fe->GetTemplatedParent());
                }

                if (NULL != fe)
                {
                    return fe->GetTemplatedParent();
                }
                else
                {
                    return NULL;
                }
            }
            else
            {
                return NULL;
            }
        }

    case RelativeSourceMode::FindAncestor:
        {
            Object* source = NULL;
            int level = 0;
            Element* fe = VisualTreeOp::GetParent(target);

            while (fe != NULL)
            {
                if (fe->GetRTTIType()->InheritFrom(AncestorType))
                {
                    level++;
                }

                if (level == AncestorLevel)
                {
                    source = fe;
                    break;
                }

                if (fe->GetUIParent() == NULL)
                {
                    fe = CASTTOFE(fe)->GetParent();
                }
                else
                {
                    fe = fe->GetUIParent();
                }
            }

            return source;
        }
    }
    return NULL;
}

}
