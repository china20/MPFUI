
#include <System/Types/Structure.h>
#include <System/Tools/Debug.h>

#include <Framework/Animation/AnimationAsSeries.h>

namespace suic
{

ImplementRTTIOfClass(DoubleAnimation, ValueAnimation)

void DoubleAnimation::StaticInit()
{
}

DoubleAnimation::DoubleAnimation()
    : _currentVal(0)
{
}

DoubleAnimation::~DoubleAnimation()
{

}

DoubleAnimation::DoubleAnimation(Float toVal, Duration duration)
    : _currentVal(0)
{
    SetTo(toVal);
    SetDuration(duration);
}

DoubleAnimation::DoubleAnimation(Float fromVal, Float toVal, Duration duration)
    : _currentVal(0)
{
    SetFrom(fromVal);
    SetTo(toVal);
    SetDuration(duration);
}

DoubleAnimation::DoubleAnimation(Float toVal, Duration duration, FillBehavior fb)
    : _currentVal(0)
{
    SetFrom(toVal);
    SetDuration(duration);
    SetFillBehavior(fb);
}

DoubleAnimation::DoubleAnimation(Float fromVal, Float toVal, Duration duration, FillBehavior fb)
    : _currentVal(0)
{
    SetFrom(fromVal);
    SetTo(toVal);
    SetDuration(duration);
    SetFillBehavior(fb);
}

static Float AddDouble(Float value1, Float value2)
{
    return (value1 + value2);
}

static Float InterpolateDouble(Float from, Float to, Float progress)
{
    return (from + ((to - from) * progress));
}

void DoubleAnimation::SetKeyValue(Object* val, int index)
{
    _keyValues[index] = val->ToFloat();
}

Float DoubleAnimation::GetCurrentValue() const
{
    return _currentVal;
}

Object* DoubleAnimation::GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock)
{
    if (!_isAnimationValid)
    {
        ValidateAnimationFunction();
    }

    Float normalizedTime = clock->GetProgress();
    IEasingFunction* easingFunc = GetEasingFunction();

    if (easingFunc != NULL)
    {
        normalizedTime = easingFunc->Ease(normalizedTime);
    }

    Float num2 = 0.0;
    Float num = 0.0;
    Float num6 = 0.0;
    Float num3 = 0.0;
    bool flag = false;
    bool flag2 = false;

    switch (_animationType)
    {
    case AnimationType::Automatic:
        num2 = defOriVal->ToFloat();
        num = defDestVal->ToFloat();
        flag = true;
        flag2 = true;
        break;

    case AnimationType::From:
        num2 = _keyValues[0];
        num = defDestVal->ToFloat();
        flag2 = true;
        break;

    case AnimationType::To:
        num2 = defOriVal->ToFloat();
        num = _keyValues[0];
        flag = true;
        break;

    case AnimationType::By:
        num = _keyValues[0];
        num3 = defOriVal->ToFloat();
        flag = true;
        break;

    case AnimationType::FromTo:
        num2 = _keyValues[0];
        num = _keyValues[1];
        if (IsAdditive())
        {
            num3 = defOriVal->ToFloat();
            flag = true;
        }
        break;

    case AnimationType::FromBy:
        num2 = _keyValues[0];
        num = AddDouble(_keyValues[0], _keyValues[1]);
        if (IsAdditive())
        {
            num3 = defOriVal->ToFloat();
            flag = true;
        }
        break;
    }

    _currentVal = AddDouble(num3, AddDouble(num6, InterpolateDouble(num2, num, normalizedTime)));

    return new OFloat(_currentVal);
}

Float DoubleAnimation::GetBy()
{
    return GetValue(ByProperty)->ToFloat();
}

void DoubleAnimation::SetBy(Float val)
{
    SetValue(ByProperty, new OFloat(val));
}

Float DoubleAnimation::GetTo()
{
    return GetValue(ToProperty)->ToFloat();
}

void DoubleAnimation::SetTo(Float val)
{
    SetValue(ToProperty, new OFloat(val));
}

Float DoubleAnimation::GetFrom()
{
    return GetValue(FromProperty)->ToFloat();
}

void DoubleAnimation::SetFrom(Float val)
{
    SetValue(FromProperty, new OFloat(val));
}

//=========================================================
//
ImplementRTTIOfClass(Int32Animation, ValueAnimation)

void Int32Animation::StaticInit()
{
}

Int32Animation::Int32Animation()
{
}

Int32Animation::~Int32Animation()
{

}

Int32Animation::Int32Animation(Int32 toVal, Duration duration)
{
    SetTo(toVal);
    SetDuration(duration);
}

Int32Animation::Int32Animation(Int32 fromVal, Int32 toVal, Duration duration)
{
    SetFrom(fromVal);
    SetTo(toVal);
    SetDuration(duration);
}

Int32Animation::Int32Animation(Int32 toVal, Duration duration, FillBehavior fb)
{
    SetFrom(toVal);
    SetDuration(duration);
    SetFillBehavior(fb);
}

Int32Animation::Int32Animation(Int32 fromVal, Int32 toVal, Duration duration, FillBehavior fb)
{
    SetFrom(fromVal);
    SetTo(toVal);
    SetDuration(duration);
    SetFillBehavior(fb);
}

static Int32 AddInt32(Int32 value1, Int32 value2)
{
    return (value1 + value2);
}

static Int32 InterpolateInt32(Int32 from, Int32 to, Float progress)
{
    if (FloatUtil::AreClose(progress, 0.0f))
    {
        return from;
    }

    if (FloatUtil::AreClose(progress, 1.0f))
    {
        return to;
    }

    Int32 num = to - from;
    num *= progress;
    num += (num > 0.0f) ? 0.5f : -0.5f;

    return (from + ((Int32)num));
}

void Int32Animation::SetKeyValue(Object* val, int index)
{
    _keyValues[index] = val->ToInt();
}

Object* Int32Animation::GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock)
{
    if (!_isAnimationValid)
    {
        ValidateAnimationFunction();
    }

    Float normalizedTime = clock->GetProgress();
    IEasingFunction* easingFunc = GetEasingFunction();
    if (easingFunc != NULL)
    {
        normalizedTime = easingFunc->Ease(normalizedTime);
    }

    Int32 num2 = 0.0;
    Int32 num = 0.0;
    Int32 num6 = 0.0;
    Int32 num3 = 0.0;
    bool flag = false;
    bool flag2 = false;

    switch (_animationType)
    {
    case AnimationType::Automatic:
        num2 = defOriVal->ToInt();
        num = defDestVal->ToInt();
        flag = true;
        flag2 = true;
        break;

    case AnimationType::From:
        num2 = _keyValues[0];
        num = defDestVal->ToInt();
        flag2 = true;
        break;

    case AnimationType::To:
        num2 = defOriVal->ToInt();
        num = _keyValues[0];
        flag = true;
        break;

    case AnimationType::By:
        num = _keyValues[0];
        num3 = defOriVal->ToInt();
        flag = true;
        break;

    case AnimationType::FromTo:
        num2 = _keyValues[0];
        num = _keyValues[1];

        if (IsAdditive())
        {
            num3 = defOriVal->ToInt();
            flag = true;
        }
        break;

    case AnimationType::FromBy:
        num2 = _keyValues[0];
        num = AddInt32(_keyValues[0], _keyValues[1]);
        if (IsAdditive())
        {
            num3 = defOriVal->ToInt();
            flag = true;
        }
        break;
    }

    Int32 fVal = AddInt32(num3, AddInt32(num6, InterpolateInt32(num2, num, normalizedTime)));

    return new Integer(fVal);
}

Int32 Int32Animation::GetBy()
{
    return GetValue(ByProperty)->ToInt();
}

void Int32Animation::SetBy(Int32 val)
{
    SetValue(ByProperty, new Integer(val));
}

Int32 Int32Animation::GetTo()
{
    return GetValue(ToProperty)->ToInt();
}

void Int32Animation::SetTo(Int32 val)
{
    SetValue(ToProperty, new Integer(val));
}

Int32 Int32Animation::GetFrom()
{
    return GetValue(FromProperty)->ToInt();
}

void Int32Animation::SetFrom(Int32 val)
{
    SetValue(FromProperty, new Integer(val));
}

//=========================================================
//
ImplementRTTIOfClass(RectAnimation, ValueAnimation)

void RectAnimation::StaticInit()
{
}

RectAnimation::RectAnimation()
{
}

RectAnimation::~RectAnimation()
{

}

RectAnimation::RectAnimation(Rect toVal, Duration duration)
{
    SetTo(toVal);
    SetDuration(duration);
}

RectAnimation::RectAnimation(Rect fromVal, Rect toVal, Duration duration)
{
    SetFrom(fromVal);
    SetTo(toVal);
    SetDuration(duration);
}

RectAnimation::RectAnimation(Rect toVal, Duration duration, FillBehavior fb)
{
    SetFrom(toVal);
    SetDuration(duration);
    SetFillBehavior(fb);
}

RectAnimation::RectAnimation(Rect fromVal, Rect toVal, Duration duration, FillBehavior fb)
{
    SetFrom(fromVal);
    SetTo(toVal);
    SetDuration(duration);
    SetFillBehavior(fb);
}

static Point AddPoint(Point value1, Point value2)
{
    return Point(value1.x + value2.x, value1.y + value2.y);
}

static Size AddSize(Size value1, Size value2)
{
    return Size(value1.Width() + value2.Width(), value1.Height() + value2.Height());
}

static Rect AddRect(Rect value1, Rect value2)
{
    return Rect(AddPoint(value1.LeftTop(), value2.LeftTop()), AddSize(value1.ToSize(), value2.ToSize()));
}

static Rect InterpolateRect(Rect from, Rect to, Float progress)
{
    Rect rect;
    rect.left = from.left + ((to.left - from.left) * progress);
    rect.top = from.top + ((to.top - from.top) * progress);
    rect.right = rect.left + from.Width() + ((to.Width() - from.Width()) * progress);
    rect.bottom = rect.top + from.Height() + ((to.Height() - from.Height()) * progress);
    return rect;
}

void RectAnimation::SetKeyValue(Object* val, int index)
{
    _keyValues[index] = ORect::FromObj(val);
}

Object* RectAnimation::GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock)
{
    if (!_isAnimationValid)
    {
        ValidateAnimationFunction();
    }

    Float normalizedTime = clock->GetProgress();
    IEasingFunction* easingFunc = GetEasingFunction();
    if (easingFunc != NULL)
    {
        normalizedTime = easingFunc->Ease(normalizedTime);
    }

    Debug::Trace(_U("GetTimerValue: %f\n"), normalizedTime);

    Rect num2;
    Rect num;
    Rect num6;
    Rect num3;
    bool flag = false;
    bool flag2 = false;

    switch (_animationType)
    {
    case AnimationType::Automatic:
        num2 = ORect::FromObj(defOriVal);
        num = ORect::FromObj(defDestVal);
        flag = true;
        flag2 = true;
        break;

    case AnimationType::From:
        num2 = _keyValues[0];
        num = ORect::FromObj(defDestVal);
        flag2 = true;
        break;

    case AnimationType::To:
        num2 = ORect::FromObj(defOriVal);
        num = _keyValues[0];
        flag = true;
        break;

    case AnimationType::By:
        num = _keyValues[0];
        num3 = ORect::FromObj(defOriVal);
        flag = true;
        break;

    case AnimationType::FromTo:
        num2 = _keyValues[0];
        num = _keyValues[1];
        if (IsAdditive())
        {
            num3 = ORect::FromObj(defOriVal);
            flag = true;
        }
        break;

    case AnimationType::FromBy:
        num2 = _keyValues[0];
        num = AddRect(_keyValues[0], _keyValues[1]);
        if (IsAdditive())
        {
            num3 = ORect::FromObj(defOriVal);
            flag = true;
        }
        break;
    }

    Rect fVal = AddRect(num3, AddRect(num6, InterpolateRect(num2, num, normalizedTime)));

    return new ORect(fVal);
}

Rect RectAnimation::GetBy()
{
    return ORect::FromObj(GetValue(ByProperty));
}

void RectAnimation::SetBy(Rect val)
{
    SetValue(ByProperty, new ORect(val));
}

Rect RectAnimation::GetTo()
{
    return ORect::FromObj(GetValue(ToProperty));
}

void RectAnimation::SetTo(Rect val)
{
    SetValue(ToProperty, new ORect(val));
}

Rect RectAnimation::GetFrom()
{
    return ORect::FromObj(GetValue(FromProperty));
}

void RectAnimation::SetFrom(Rect val)
{
    SetValue(FromProperty, new ORect(val));
}

//=========================================================
//
ImplementRTTIOfClass(ColorAnimation, ValueAnimation)

void ColorAnimation::StaticInit()
{
}

ColorAnimation::ColorAnimation()
{
}

ColorAnimation::~ColorAnimation()
{

}

ColorAnimation::ColorAnimation(Color toVal, Duration duration)
{
    SetTo(toVal);
    SetDuration(duration);
}

ColorAnimation::ColorAnimation(Color fromVal, Color toVal, Duration duration)
{
    SetFrom(fromVal);
    SetTo(toVal);
    SetDuration(duration);
}

ColorAnimation::ColorAnimation(Color toVal, Duration duration, FillBehavior fb)
{
    SetFrom(toVal);
    SetDuration(duration);
    SetFillBehavior(fb);
}

ColorAnimation::ColorAnimation(Color fromVal, Color toVal, Duration duration, FillBehavior fb)
{
    SetFrom(fromVal);
    SetTo(toVal);
    SetDuration(duration);
    SetFillBehavior(fb);
}

static Color AddColor(Color value1, Color value2)
{
    return (value1 + value2);
}

static Color InterpolateColor(Color from, Color to, Float progress)
{
    return (from + ((Color)((to - from) * ((float)progress))));
}

void ColorAnimation::SetKeyValue(Object* val, int index)
{
    _keyValues[index] = OColor::FromObj(val);
}

Object* ColorAnimation::GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock)
{
    if (!_isAnimationValid)
    {
        ValidateAnimationFunction();
    }

    Float normalizedTime = clock->GetProgress();
    IEasingFunction* easingFunc = GetEasingFunction();
    if (easingFunc != NULL)
    {
        normalizedTime = easingFunc->Ease(normalizedTime);
    }

    Color num2;
    Color num;
    Color num6;
    Color num3;
    bool flag = false;
    bool flag2 = false;

    switch (_animationType)
    {
    case AnimationType::Automatic:
        num2 = OColor::FromObj(defOriVal);
        num = OColor::FromObj(defDestVal);
        flag = true;
        flag2 = true;
        break;

    case AnimationType::From:
        num2 = _keyValues[0];
        num = OColor::FromObj(defDestVal);
        flag2 = true;
        break;

    case AnimationType::To:
        num2 = OColor::FromObj(defOriVal);
        num = _keyValues[0];
        flag = true;
        break;

    case AnimationType::By:
        num = _keyValues[0];
        num3 = OColor::FromObj(defOriVal);
        flag = true;
        break;

    case AnimationType::FromTo:
        num2 = _keyValues[0];
        num = _keyValues[1];
        if (IsAdditive())
        {
            num3 = OColor::FromObj(defOriVal);
            flag = true;
        }
        break;

    case AnimationType::FromBy:
        num2 = _keyValues[0];
        num = AddColor(_keyValues[0], _keyValues[1]);
        if (IsAdditive())
        {
            num3 = OColor::FromObj(defOriVal);
            flag = true;
        }
        break;
    }

    Color fVal = AddColor(num3, AddColor(num6, InterpolateColor(num2, num, normalizedTime)));

    return new OColor(fVal);
}

Color ColorAnimation::GetBy()
{
    return OColor::FromObj(GetValue(ByProperty));
}

void ColorAnimation::SetBy(Color val)
{
    SetValue(ByProperty, new OColor(val));
}

Color ColorAnimation::GetTo()
{
    return OColor::FromObj(GetValue(ToProperty));
}

void ColorAnimation::SetTo(Color val)
{
    SetValue(ToProperty, new OColor(val));
}

Color ColorAnimation::GetFrom()
{
    return OColor::FromObj(GetValue(FromProperty));
}

void ColorAnimation::SetFrom(Color val)
{
    SetValue(FromProperty, new OColor(val));
}

//=========================================================
//
ImplementRTTIOfClass(PointAnimation, ValueAnimation)

void PointAnimation::StaticInit()
{
}

PointAnimation::PointAnimation()
{
}

PointAnimation::~PointAnimation()
{

}

PointAnimation::PointAnimation(Point toVal, Duration duration)
{
    SetTo(toVal);
    SetDuration(duration);
}

PointAnimation::PointAnimation(Point fromVal, Point toVal, Duration duration)
{
    SetFrom(fromVal);
    SetTo(toVal);
    SetDuration(duration);
}

PointAnimation::PointAnimation(Point toVal, Duration duration, FillBehavior fb)
{
    SetFrom(toVal);
    SetDuration(duration);
    SetFillBehavior(fb);
}

PointAnimation::PointAnimation(Point fromVal, Point toVal, Duration duration, FillBehavior fb)
{
    SetFrom(fromVal);
    SetTo(toVal);
    SetDuration(duration);
    SetFillBehavior(fb);
}

static Point InterpolatePoint(Point from, Point to, Float progress)
{
    Point pt1(to);
    pt1 -= from;
    Point pt2(pt1.x * progress, pt1.y * progress);
    return Point(from.x + pt2.x, from.y + pt2.y);
}

void PointAnimation::SetKeyValue(Object* val, int index)
{
    _keyValues[index] = OPoint::FromObj(val);
}

Object* PointAnimation::GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock)
{
    if (!_isAnimationValid)
    {
        ValidateAnimationFunction();
    }

    Float normalizedTime = clock->GetProgress();
    IEasingFunction* easingFunc = GetEasingFunction();
    if (easingFunc != NULL)
    {
        normalizedTime = easingFunc->Ease(normalizedTime);
    }

    Point num2;
    Point num;
    Point num6;
    Point num3;
    bool flag = false;
    bool flag2 = false;

    switch (_animationType)
    {
    case AnimationType::Automatic:
        num2 = OPoint::FromObj(defOriVal);
        num = OPoint::FromObj(defDestVal);
        flag = true;
        flag2 = true;
        break;

    case AnimationType::From:
        num2 = _keyValues[0];
        num = OPoint::FromObj(defDestVal);
        flag2 = true;
        break;

    case AnimationType::To:
        num2 = OPoint::FromObj(defOriVal);
        num = _keyValues[0];
        flag = true;
        break;

    case AnimationType::By:
        num = _keyValues[0];
        num3 = OPoint::FromObj(defOriVal);
        flag = true;
        break;

    case AnimationType::FromTo:
        num2 = _keyValues[0];
        num = _keyValues[1];
        if (IsAdditive())
        {
            num3 = OPoint::FromObj(defOriVal);
            flag = true;
        }
        break;

    case AnimationType::FromBy:
        num2 = _keyValues[0];
        num = AddPoint(_keyValues[0], _keyValues[1]);
        if (IsAdditive())
        {
            num3 = OPoint::FromObj(defOriVal);
            flag = true;
        }
        break;
    }

    Point fVal = AddPoint(num3, AddPoint(num6, InterpolatePoint(num2, num, normalizedTime)));

    return new OPoint(fVal);
}

Point PointAnimation::GetBy()
{
    return OPoint::FromObj(GetValue(ByProperty));
}

void PointAnimation::SetBy(Point val)
{
    SetValue(ByProperty, new OPoint(val));
}

Point PointAnimation::GetTo()
{
    return OPoint::FromObj(GetValue(ToProperty));
}

void PointAnimation::SetTo(Point val)
{
    SetValue(ToProperty, new OPoint(val));
}

Point PointAnimation::GetFrom()
{
    return OPoint::FromObj(GetValue(FromProperty));
}

void PointAnimation::SetFrom(Point val)
{
    SetValue(FromProperty, new OPoint(val));
}

}
