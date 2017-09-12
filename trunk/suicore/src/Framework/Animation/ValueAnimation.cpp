
#include <Framework/Animation/ValueAnimation.h>
#include <System/Types/Double.h>
#include <System/Types/Structure.h>

namespace suic
{

DpProperty* ValueAnimation::ByProperty;
DpProperty* ValueAnimation::ToProperty;
DpProperty* ValueAnimation::FromProperty;
DpProperty* ValueAnimation::EasingFunctionProperty;

ImplementRTTIOfClass(ValueAnimation, AnimationTimeline)

void ValueAnimation::StaticInit()
{
    if (NULL == ByProperty)
    {
        ByProperty = DpProperty::Register(_U("By"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL));
        ToProperty = DpProperty::Register(_U("To"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL));
        FromProperty = DpProperty::Register(_U("From"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL));
        EasingFunctionProperty = DpProperty::Register(_U("EasingFunction"), RTTIType(), IEasingFunction::RTTIType(), DpPropMemory::GetPropMeta(NULL));;
    }
}

ValueAnimation::ValueAnimation()
{
    _isAnimationValid = false;
    _animationType = AnimationType::Automatic;
}

ValueAnimation::~ValueAnimation()
{

}

void ValueAnimation::SetKeyValue(Object* val, int index)
{

}

void ValueAnimation::ValidateAnimationFunction()
{
    _animationType = AnimationType::Automatic;
    Object* objBy = NULL;
    Object* objTo = NULL;
    Object* objFrom = NULL;

    objFrom = GetValue(FromProperty);

    if (objFrom != DpProperty::UnsetValue())
    {
        objTo = GetValue(ToProperty);

        if (objTo != DpProperty::UnsetValue())
        {
            _animationType = AnimationType::FromTo;
            SetKeyValue(objFrom, 0);
            SetKeyValue(objTo, 1);
        }
        else if ((objBy=GetValue(ByProperty)) != DpProperty::UnsetValue())
        {
            _animationType = AnimationType::FromBy;
            SetKeyValue(objFrom, 0);
            SetKeyValue(objBy, 1);
        }
        else
        {
            _animationType = AnimationType::From;
            SetKeyValue(objFrom, 0);
        }
    }
    else if ((objTo=GetValue(ToProperty)) != DpProperty::UnsetValue())
    {
        _animationType = AnimationType::To;
        SetKeyValue(objTo, 0);
    }
    else if ((objBy=GetValue(ByProperty)) != DpProperty::UnsetValue())
    {
        _animationType = AnimationType::By;
        SetKeyValue(objBy, 0);
    }

    _isAnimationValid = true;
}

IEasingFunction* ValueAnimation::GetEasingFunction()
{
    return RTTICast<IEasingFunction>(GetValue(EasingFunctionProperty));
}

void ValueAnimation::SetEasingFunction(IEasingFunction* val)
{
    SetValue(EasingFunctionProperty, val);
}

bool ValueAnimation::IsAdditive()
{
    return false;
}

void ValueAnimation::SetIsAdditive(bool val)
{

}

bool ValueAnimation::IsCumulative()
{
    return false;
}

void ValueAnimation::SetIsCumulative(bool val)
{

}

//=====================================================
//

DpProperty* EasingFuncBase::EasingModeProperty;

EasingMode EasingFuncBase::GetEasingMode()
{
    return (EasingMode)GetValue(EasingModeProperty)->ToInt();
}

void EasingFuncBase::SetEasingMode(EasingMode val)
{
    SetValue(EasingModeProperty, Integer::GetPosInt(val));
}

Float EasingFuncBase::Ease(Float fTime)
{
    switch (GetEasingMode())
    {
    case EasingMode::EaseIn:
        return EaseInCore(fTime);

    case EasingMode::EaseOut:
        return (1.0 - EaseInCore(1.0 - fTime));
    }

    if (fTime >= 0.5)
    {
        return (((1.0 - EaseInCore((1.0 - fTime) * 2.0)) * 0.5) + 0.5);
    }
    else
    {
        return (EaseInCore(fTime * 2.0) * 0.5);
    }
}

Float SineEase::EaseInCore(Float fTime)
{
    return (1.0 - sin(1.5707963267948966 * (1.0 - fTime)));
}

DpProperty* BounceEase::BouncesProperty;
DpProperty* BounceEase::BouncinessProperty;

int BounceEase::GetBounces()
{
    return GetValue(BouncesProperty)->ToInt();
}

void BounceEase::SetBounces(int val)
{
    SetValue(BouncesProperty, new Integer(val));
}

Float BounceEase::GetBounciness()
{
    return GetValue(BouncinessProperty)->ToFloat();
}

void BounceEase::SetBounciness(Float val)
{
    SetValue(BouncinessProperty, new OFloat(val));
}

static bool IsOne(double value)
{
    return (Math::Abs((Float)(value - 1.0)) < 2.2204460492503131E-15);
}

double Math::Log(double a, double newBase)
{
    if (Double::IsNaN(a))
    {
        return a;
    }

    if (Double::IsNaN(newBase))
    {
        return newBase;
    }

    if ((newBase != 1.0) && ((a == 1.0) || ((newBase != 0.0) && !Double::IsPositiveInfinity(newBase))))
    {
        return (log(a) / log(newBase));
    }
    else
    {
        return Double::NaN;
    }
}

Float BounceEase::EaseInCore(Float fTime)
{
    Float y = UFMAX(0.0, (Float)GetBounces());
    Float bounciness = GetBounciness();

    if ((bounciness < 1.0) || IsOne(bounciness))
    {
        bounciness = (Float)1.001;
    }

    Float num9 = pow(bounciness, y);
    Float num5 = 1.0 - bounciness;
    Float num4 = ((1.0 - num9) / num5) + (num9 * 0.5);
    Float num15 = fTime * num4;
    Float num3 = floor(Math::Log((-num15 * (1.0 - bounciness)) + 1.0, bounciness));
    Float num13 = num3 + 1.0;
    Float num8 = (1.0 - pow(bounciness, num3)) / (num5 * num4);
    Float num12 = (1.0 - pow(bounciness, num13)) / (num5 * num4);
    Float num7 = (num8 + num12) * 0.5;
    Float num6 = fTime - num7;
    Float num2 = num7 - num8;
    return (((-pow((double)(1.0 / bounciness), (double)(y - num3)) / (num2 * num2)) * (num6 - num2)) * (num6 + num2));
}

DpProperty* BackEase::AmplitudeProperty;

Float BackEase::GetAmplitude()
{
    return GetValue(AmplitudeProperty)->ToFloat();
}

void BackEase::SetAmplitude(Float val)
{
    SetValue(AmplitudeProperty, new OFloat(val));
}

Float BackEase::EaseInCore(Float fTime)
{
    Float num = UFMAX(0.0, GetAmplitude());
    return (pow(fTime, 3) - ((fTime * num) * sin(3.1415926535897931 * fTime)));
}

DpProperty* ElasticEase::OscillationsProperty;
DpProperty* ElasticEase::SpringinessProperty;

int ElasticEase::GetOscillations()
{
    return GetValue(OscillationsProperty)->ToInt();
}

void ElasticEase::SetOscillations(int val)
{
    SetValue(OscillationsProperty, new Integer(val));
}

Float ElasticEase::GetSpringiness()
{
    return GetValue(SpringinessProperty)->ToFloat();
}

void ElasticEase::SetSpringiness(Float val)
{
    SetValue(SpringinessProperty, new OFloat(val));
}

Float ElasticEase::EaseInCore(Float fTime)
{
    Float num2 = 0.0;
    Float num3 = UFMAX(0.0, (double) GetOscillations());
    Float num = UFMAX(0.0, GetSpringiness());

    if (FloatUtil::IsZero(num))
    {
        num2 = fTime;
    }
    else
    {
        num2 = (exp(num * fTime) - 1.0) / (exp(num) - 1.0);
    }
    return (num2 * sin(((6.2831853071795862 * num3) + 1.5707963267948966) * fTime));

}

DpProperty* ExponentialEase::ExponentProperty;

Float ExponentialEase::GetExponent()
{
    return GetValue(ExponentProperty)->ToFloat();
}

void ExponentialEase::SetExponent(Float val)
{
    SetValue(ExponentProperty, new OFloat(val));
}

Float ExponentialEase::EaseInCore(Float fTime)
{
    Float exponent = GetExponent();
    if (FloatUtil::IsZero(exponent))
    {
        return fTime;
    }
    else
    {
        return ((exp(exponent * fTime) - 1.0) / (exp(exponent) - 1.0));
    }
}

DpProperty* PowerEase::PowerProperty;

Float PowerEase::GetPower()
{
    return GetValue(PowerProperty)->ToFloat();
}

void PowerEase::SetPower(Float val)
{
    SetValue(PowerProperty, new OFloat(val));
}

void EasingFuncBase::StaticInit()
{
    if (NULL == EasingFuncBase::EasingModeProperty)
    {
        EasingFuncBase::EasingModeProperty = DpProperty::Register(_T("EasingMode"), EasingFuncBase::RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(EasingMode::EaseOut)));

        BounceEase::BouncesProperty = DpProperty::Register(_T("Bounces"), BounceEase::RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(new Integer(3)));
        BounceEase::BouncinessProperty = DpProperty::Register(_T("Bounciness"), BounceEase::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(2.0)));

        BackEase::AmplitudeProperty = DpProperty::Register(_T("Amplitude"), BackEase::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(1.0)));

        ElasticEase::OscillationsProperty = DpProperty::Register(_T("Oscillations"), ElasticEase::RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(new Integer(3)));
        ElasticEase::SpringinessProperty = DpProperty::Register(_T("Springiness"), ElasticEase::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(3.0)));

        ExponentialEase::ExponentProperty = DpProperty::Register(_T("Exponent"), ExponentialEase::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(2.0)));

        PowerEase::PowerProperty = DpProperty::Register(_T("Power"), PowerEase::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(2.0)));
    }
}

}
