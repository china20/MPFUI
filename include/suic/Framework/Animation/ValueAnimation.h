
#ifndef _UIVALUEANIMATION_H_
#define _UIVALUEANIMATION_H_

#include <System/Animation/TimerManager.h>
#include <System/Windows/DpObject.h>
#include <Framework/Animation/EasingFunc.h>

namespace suic
{

class SUICORE_API ValueAnimation : public AnimationTimeline
{
public:

    enum AnimationType : Byte
    {
        Automatic = 0,
        From = 1,
        To = 2,
        By = 3,
        FromTo = 4,
        FromBy = 5,
    };

    static DpProperty* ByProperty;
    static DpProperty* ToProperty;
    static DpProperty* FromProperty;
    static DpProperty* EasingFunctionProperty;

    static bool StaticInit();

    RTTIOfClass(ValueAnimation)

    ValueAnimation();
    ~ValueAnimation();

    IEasingFunction* GetEasingFunction();
    void SetEasingFunction(IEasingFunction* val);

    bool IsAdditive();
    void SetIsAdditive(bool val);

    bool IsCumulative();
    void SetIsCumulative(bool val);

protected:

    virtual void SetKeyValue(Object* val, int index);

    void ValidateAnimationFunction();

protected:

    bool _isAnimationValid;
    AnimationType _animationType;
};

}

#endif
