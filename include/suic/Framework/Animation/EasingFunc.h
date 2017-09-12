
#ifndef _UIEASINGFUNC_H_
#define _UIEASINGFUNC_H_

#include <System/Windows/DpObject.h>
#include <System/Tools/Math.h>

namespace suic
{

enum EasingMode
{
    EaseIn,
    EaseOut,
    EaseInOut,
};

class SUICORE_API EasingFuncBase : public IEasingFunction
{
public:
    
    static DpProperty* EasingModeProperty;

    static void StaticInit();

    EasingMode GetEasingMode();
    void SetEasingMode(EasingMode val);

    Float Ease(Float fTime);

protected:

    virtual Float EaseInCore(Float fTime) = 0;
};

class SUICORE_API SineEase : public EasingFuncBase
{
public:

    Float EaseInCore(Float fTime);
};

class SUICORE_API BounceEase : public EasingFuncBase
{
public:

    static DpProperty* BouncesProperty;
    static DpProperty* BouncinessProperty;

    int GetBounces();
    void SetBounces(int val);

    Float GetBounciness();
    void SetBounciness(Float val);

    Float EaseInCore(Float fTime);
};

class SUICORE_API CircleEase : public EasingFuncBase
{
public:

    Float EaseInCore(Float fTime)
    {
        fTime = UFMAX(0.0f, UFMIN(1.0f, fTime));
        return (1.0f - sqrt(1.0f - (fTime * fTime)));
    }
};

class SUICORE_API BackEase : public EasingFuncBase
{
public:

    static DpProperty* AmplitudeProperty;

    Float GetAmplitude();
    void SetAmplitude(Float val);

    Float EaseInCore(Float fTime);
};

class SUICORE_API ElasticEase : public EasingFuncBase
{
public:

    static DpProperty* OscillationsProperty;
    static DpProperty* SpringinessProperty;

    int GetOscillations();
    void SetOscillations(int val);
    Float GetSpringiness();
    void SetSpringiness(Float val);

    Float EaseInCore(Float fTime);
};

class SUICORE_API ExponentialEase : public EasingFuncBase
{
public:

    static DpProperty* ExponentProperty;

    Float GetExponent();
    void SetExponent(Float val);

    Float EaseInCore(Float fTime);
};

class SUICORE_API PowerEase : public EasingFuncBase
{
public:

    static DpProperty* PowerProperty;

    Float GetPower();
    void SetPower(Float val);

    Float EaseInCore(Float fTime)
    {
        Float y = UFMAX(0.0f, GetPower());
        return pow(fTime, y);
    }
};

class SUICORE_API QuinticEase  : public EasingFuncBase
{
public:

    Float EaseInCore(Float fTime)
    {
        return ((((fTime * fTime) * fTime) * fTime) * fTime);
    }
};

class SUICORE_API QuarticEase  : public EasingFuncBase
{
public:

    Float EaseInCore(Float fTime)
    {
        return (((fTime * fTime) * fTime) * fTime);
    }
};

class SUICORE_API CubicEase : public EasingFuncBase
{
public:

    Float EaseInCore(Float fTime)
    {
        return ((fTime * fTime) * fTime);
    }
};

class SUICORE_API QuadraticEase : public EasingFuncBase
{
public:

    Float EaseInCore(Float fTime)
    {
        return (fTime * fTime);
    }
};

}

#endif
