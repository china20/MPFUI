
#ifndef _UIDOUBLEANIMATION_H_
#define _UIDOUBLEANIMATION_H_

#include <Framework/Animation/ValueAnimation.h>

namespace suic
{

class SUICORE_API DoubleAnimation : public ValueAnimation
{
public:

    static bool StaticInit();

    RTTIOfAbstractClass(DoubleAnimation)

    DoubleAnimation();
    ~DoubleAnimation();

    DoubleAnimation(Float toVal, Duration duration);
    DoubleAnimation(Float fromVal, Float toVal, Duration duration);
    DoubleAnimation(Float toVal, Duration duration, FillBehavior fb);
    DoubleAnimation(Float fromVal, Float toVal, Duration duration, FillBehavior fb);

    Object* GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock);

    Float GetBy();
    void SetBy(Float val);

    Float GetTo();
    void SetTo(Float val);

    Float GetFrom();
    void SetFrom(Float val);

    Float GetCurrentValue() const;

protected:

    void SetKeyValue(Object* val, int index);

private:

    Float _keyValues[2];
    Float _currentVal;
};

class SUICORE_API Int32Animation : public ValueAnimation
{
public:

    static bool StaticInit();

    RTTIOfAbstractClass(Int32Animation)

    Int32Animation();
    ~Int32Animation();

    Int32Animation(Int32 toVal, Duration duration);
    Int32Animation(Int32 fromVal, Int32 toVal, Duration duration);
    Int32Animation(Int32 toVal, Duration duration, FillBehavior fb);
    Int32Animation(Int32 fromVal, Int32 toVal, Duration duration, FillBehavior fb);

    Object* GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock);

    Int32 GetBy();
    void SetBy(Int32 val);

    Int32 GetTo();
    void SetTo(Int32 val);

    Int32 GetFrom();
    void SetFrom(Int32 val);

protected:

    void SetKeyValue(Object* val, int index);

private:

    Int32 _keyValues[2];
};

class SUICORE_API RectAnimation : public ValueAnimation
{
public:

    static bool StaticInit();

    RTTIOfAbstractClass(RectAnimation)

    RectAnimation();
    ~RectAnimation();

    RectAnimation(Rect toVal, Duration duration);
    RectAnimation(Rect fromVal, Rect toVal, Duration duration);
    RectAnimation(Rect toVal, Duration duration, FillBehavior fb);
    RectAnimation(Rect fromVal, Rect toVal, Duration duration, FillBehavior fb);

    Object* GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock);

    Rect GetBy();
    void SetBy(Rect val);

    Rect GetTo();
    void SetTo(Rect val);

    Rect GetFrom();
    void SetFrom(Rect val);

protected:

    void SetKeyValue(Object* val, int index);

private:

    Rect _keyValues[2];
};

class SUICORE_API ColorAnimation : public ValueAnimation
{
public:

    static bool StaticInit();

    RTTIOfAbstractClass(ColorAnimation)

    ColorAnimation();
    ~ColorAnimation();

    ColorAnimation(Color toVal, Duration duration);
    ColorAnimation(Color fromVal, Color toVal, Duration duration);
    ColorAnimation(Color toVal, Duration duration, FillBehavior fb);
    ColorAnimation(Color fromVal, Color toVal, Duration duration, FillBehavior fb);

    Object* GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock);

    Color GetBy();
    void SetBy(Color val);

    Color GetTo();
    void SetTo(Color val);

    Color GetFrom();
    void SetFrom(Color val);

protected:

    void SetKeyValue(Object* val, int index);

private:

    Color _keyValues[2];
};

class SUICORE_API PointAnimation : public ValueAnimation
{
public:

    static bool StaticInit();

    RTTIOfAbstractClass(PointAnimation)

    PointAnimation();
    ~PointAnimation();

    PointAnimation(Point toVal, Duration duration);
    PointAnimation(Point fromVal, Point toVal, Duration duration);
    PointAnimation(Point toVal, Duration duration, FillBehavior fb);
    PointAnimation(Point fromVal, Point toVal, Duration duration, FillBehavior fb);

    Object* GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock);

    Point GetBy();
    void SetBy(Point val);

    Point GetTo();
    void SetTo(Point val);

    Point GetFrom();
    void SetFrom(Point val);

protected:

    void SetKeyValue(Object* val, int index);

private:

    Point _keyValues[2];
};

}

#endif
