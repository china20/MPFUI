
#include <System/Types/Structure.h>
#include <System/Tools/Debug.h>

#include <Framework/Animation/AnimationAsKeyFrame.h>

namespace suic
{

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

    Float num = to - from;
    num *= progress;
    num += FloatUtil::GreaterThanOrClose(num, 0.0) ? 0.5 : -0.5;
    return (from + ((Int32)num));
}

static Float InterpolateDouble(Float from, Float to, Float progress)
{
    return (from + ((to - from) * progress));
}

Float KeyTime::GetPercent()
{
    if (tt != KeyTimeType::Percent)
    {
        throw InvalidOperationException(_U("Not a KeyTimeType::Percent"), __FILELINE__);
    }
    return val;
}

//---------------------------------------------------

ImplementRTTIOfClass(IKeyFrame, Object)
ImplementRTTIOfClass(Int32KeyFrame, IKeyFrame)

Int32KeyFrame::Int32KeyFrame()
        : _value(NULL)
{
}

Int32KeyFrame::Int32KeyFrame(KeyTime kt, Int32 val)
    : _value(new Integer(val))
    , _keyTime(kt)
{
    if (NULL != _value)
    {
        _value->ref();
    }
}

Int32KeyFrame::~Int32KeyFrame()
{
    if (NULL != _value)
    {
        _value->unref();
    }
}

LinearInt32KeyFrame::LinearInt32KeyFrame()
{

}

LinearInt32KeyFrame::LinearInt32KeyFrame(KeyTime kt, Int32 val)
    : Int32KeyFrame(kt, val)
{

}

Int32 LinearInt32KeyFrame::InterpolateValue(Int32 baseValue, Float progress)
{
    if (FloatUtil::AreClose(progress, 0.0f))
    {
        return baseValue;
    }
    else if (FloatUtil::AreClose(progress, 1.0f))
    {
        return GetValue()->ToInt();
    }
    else
    {
        return InterpolateInt32(baseValue, GetValue()->ToInt(), progress);
    }
}


EasingInt32KeyFrame::EasingInt32KeyFrame()
    : _easingFunc(NULL)
{

}

EasingInt32KeyFrame::EasingInt32KeyFrame(KeyTime kt, Int32 val)
    : Int32KeyFrame(kt, val)
    , _easingFunc(NULL)
{

}

void EasingInt32KeyFrame::SetEasingFunction(IEasingFunction* val)
{
    if (_easingFunc)
    {
        _easingFunc->unref();
    }

    _easingFunc = val;

    if (_easingFunc)
    {
        _easingFunc->ref();
    }
}

Int32 EasingInt32KeyFrame::InterpolateValue(Int32 baseValue, Float progress)
{
    if (_easingFunc != NULL)
    {
        progress = _easingFunc->Ease(progress);
    }

    if (FloatUtil::AreClose(progress, 0.0))
    {
        return baseValue;
    }
    else if (FloatUtil::AreClose(progress, 1.0))
    {
        return GetValue()->ToInt();
    }
    else
    {
        return InterpolateInt32(baseValue, GetValue()->ToInt(), progress);
    }
}

//===========================================================

Int32AnimationUsingKeyFrame::Int32AnimationUsingKeyFrame()
    : _keyTimesIsValid(false)
{

}

Int32AnimationUsingKeyFrame::~Int32AnimationUsingKeyFrame()
{
    Clear();
}

void Int32AnimationUsingKeyFrame::AddChild(Object* obj)
{
    if (obj)
    {
        obj->ref();
        Int32KeyFrame* frame = RTTICast<Int32KeyFrame>(obj);
        if (NULL != frame)
        {
            AddKeyFrame(frame);
        }
        obj->unref();
    }
}

void Int32AnimationUsingKeyFrame::AddText(String val)
{

}

void Int32AnimationUsingKeyFrame::RemoveChild(Object* child)
{
    int count = _keyFrames.Length();
    for (int i = 0; i < count; i++)
    {
        if (_keyFrames[i] == child)
        {
            _keyFrames[i]->unref();
            _keyFrames.RemoveAt(i);
            break;
        }
    }
}

Time_t Int32AnimationUsingKeyFrame::GetLargestTimeSpanKeyTime()
{
    bool flag = false;
    Time_t zero = 0;

    if (_keyFrames.Length() > 0)
    {
        int count = _keyFrames.Length();

        for (int i = 0; i < count; i++)
        {
            KeyTime keyTime = _keyFrames[i]->GetKeyTime();
            if (keyTime.tt == KeyTimeType::TimeSpan)
            {
                flag = true;
                if ((int)keyTime.val > zero)
                {
                    zero = (int)keyTime.val;
                }
            }
        }
    }

    if (flag)
    {
        return zero;
    }
    else
    {
        return 1000;
    }
}

void Int32AnimationUsingKeyFrame::ResolveKeyTimes()
{
    _keyTimesIsValid = true;

    Time_t zero = 0;
    Duration duration = GetDuration();
    if (duration.HasTimeSpan())
    {
        zero = (Time_t)duration.GetTimeSpan();
    }
    else
    {
        zero = GetLargestTimeSpanKeyTime();
    }

    int iEndIndex = _keyFrames.Length() - 1;
    for (int i = 0; i < _keyFrames.Length(); ++i)
    {
        KeyTime keyTime = _keyFrames[i]->GetKeyTime();
        switch (keyTime.tt)
        {
            case KeyTimeType::Uniform:
            case KeyTimeType::Paced:
            {
                if (i != iEndIndex)
                {
                    break;
                }
                _keyFrames[i]->GetKeyTime().rt = zero;
                break;
            }
            case KeyTimeType::Percent:
            {
                _keyFrames[i]->GetKeyTime().rt = (Time_t)(keyTime.GetPercent() * zero);
                break;
            }
            case KeyTimeType::TimeSpan:
            {
                _keyFrames[i]->GetKeyTime().rt = (Time_t)keyTime.val;
                break;
            }
            default:
            {
                ;
            }
        }
    }
}

int Int32AnimationUsingKeyFrame::GetCount() const
{
    return _keyFrames.GetCount();
}

Int32KeyFrame* Int32AnimationUsingKeyFrame::GetKeyFrame(int index) const
{
    return _keyFrames.GetItem(index);
}

Object* Int32AnimationUsingKeyFrame::GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock)
{
    if (_keyFrames.Length() == 0)
    {
        return defOriVal;
    }

    if (!_keyTimesIsValid)
    {
        ResolveKeyTimes();
    }

    int index = 0;
    int iSize = _keyFrames.Length();
    int iEndIndex = iSize - 1;
    Int32 aniValue = 0;
    Time_t currTime = clock->GetCurrentTime();

    while (index < iSize && currTime > _keyFrames[index]->GetKeyTime().rt)
    {
        ++index;
    }

    while (index < iEndIndex && currTime == _keyFrames[index + 1]->GetKeyTime().rt)
    {
        ++index;
    }

    if (index == iSize)
    {
        aniValue = _keyFrames[iEndIndex]->GetValue()->ToInt();
    }
    else if (currTime == _keyFrames[index]->GetKeyTime().rt)
    {
        aniValue = _keyFrames[index]->GetValue()->ToInt();
    }
    else
    {
        Object* calcVal = NULL;
        Float keyProgress = 0.0f;
        if (index == 0)
        {
            calcVal = defOriVal;
            keyProgress = (Float)currTime / (Float)(_keyFrames[0]->GetKeyTime().rt);
        }
        else
        {
            calcVal = _keyFrames[index - 1]->GetValue();
            Time_t prevDif = currTime - _keyFrames[index - 1]->GetKeyTime().rt;
            Time_t currDif = _keyFrames[index]->GetKeyTime().rt - currTime;

            keyProgress = (Float)prevDif / (Float)currDif;
            if (FloatUtil::GreaterThan(keyProgress, 1.0))
            {
                keyProgress = 1.0;
            }
        }

        aniValue = _keyFrames[index]->InterpolateValue(calcVal->ToInt(), keyProgress);
    }

    return new Integer(aniValue);
}

void Int32AnimationUsingKeyFrame::Clear()
{
    for (int i = 0; i < _keyFrames.Length(); ++i)
    {
        _keyFrames[i]->unref();
    }
    _keyFrames.Clear();
}

void Int32AnimationUsingKeyFrame::AddKeyFrame(Int32KeyFrame* frame)
{
    frame->ref();

    if (_keyFrames.Length() == 0)
    {
        _keyFrames.Add(frame);
    }
    else
    {
        for (int i = 0; i < _keyFrames.Length(); ++i)
        {
            if (frame->GetKeyTime().val < _keyFrames[i]->GetKeyTime().val)
            {
                _keyFrames.Insert(i, frame);
                return;
            }
        }

        _keyFrames.Add(frame);
    }
}

//-----------------------------------------------------

ImplementRTTIOfClass(DoubleKeyFrame, IKeyFrame)

DoubleKeyFrame::DoubleKeyFrame()
    : _value(NULL)
{
}

DoubleKeyFrame::DoubleKeyFrame(KeyTime kt, Float val)
    : _value(new OFloat(val))
    , _keyTime(kt)
{
    if (NULL != _value)
    {
        _value->ref();
    }
}

DoubleKeyFrame::~DoubleKeyFrame()
{
    if (NULL != _value)
    {
        _value->unref();
    }
}

LinearDoubleKeyFrame::LinearDoubleKeyFrame()
{

}

LinearDoubleKeyFrame::LinearDoubleKeyFrame(KeyTime kt, Float val)
    : DoubleKeyFrame(kt, val)
{

}

Float LinearDoubleKeyFrame::InterpolateValue(Float baseValue, Float progress)
{
    if (FloatUtil::AreClose(progress, 0.0))
    {
        return baseValue;
    }
    else if (FloatUtil::AreClose(progress, 1.0))
    {
        return GetValue()->ToFloat();
    }
    else
    {
        return InterpolateDouble(baseValue, GetValue()->ToFloat(), progress);
    }
}


EasingDoubleKeyFrame::EasingDoubleKeyFrame()
    : _easingFunc(NULL)
{

}

EasingDoubleKeyFrame::EasingDoubleKeyFrame(KeyTime kt, Float val)
    : DoubleKeyFrame(kt, val)
    , _easingFunc(NULL)
{

}

void EasingDoubleKeyFrame::SetEasingFunction(IEasingFunction* val)
{
    if (_easingFunc)
    {
        _easingFunc->unref();
    }

    _easingFunc = val;

    if (_easingFunc)
    {
        _easingFunc->ref();
    }
}

Float EasingDoubleKeyFrame::InterpolateValue(Float baseValue, Float progress)
{
    if (_easingFunc != NULL)
    {
        progress = _easingFunc->Ease(progress);
    }

    if (FloatUtil::AreClose(progress, 0.0))
    {
        return baseValue;
    }
    else if (FloatUtil::AreClose(progress, 1.0))
    {
        return GetValue()->ToFloat();
    }
    else
    {
        return InterpolateDouble(baseValue, GetValue()->ToFloat(), progress);
    }
}

//===========================================================

DoubleAnimationUsingKeyFrame::DoubleAnimationUsingKeyFrame()
    : _keyTimesIsValid(false)
{

}

DoubleAnimationUsingKeyFrame::~DoubleAnimationUsingKeyFrame()
{
    Clear();
}

void DoubleAnimationUsingKeyFrame::AddChild(Object* obj)
{
    if (obj)
    {
        obj->ref();
        DoubleKeyFrame* frame = DynamicCast<DoubleKeyFrame>(obj);
        if (NULL != frame)
        {
            AddKeyFrame(frame);
        }
        obj->unref();
    }
}

void DoubleAnimationUsingKeyFrame::AddText(String val)
{

}

Time_t DoubleAnimationUsingKeyFrame::GetLargestTimeSpanKeyTime()
{
    bool flag = false;
    Time_t zero = 0;

    if (_keyFrames.Length() > 0)
    {
        int count = _keyFrames.Length();

        for (int i = 0; i < count; i++)
        {
            KeyTime keyTime = _keyFrames[i]->GetKeyTime();
            if (keyTime.tt == KeyTimeType::TimeSpan)
            {
                flag = true;
                if ((int)keyTime.val > zero)
                {
                    zero = (int)keyTime.val;
                }
            }
        }
    }

    if (flag)
    {
        return zero;
    }
    else
    {
        return 1000;
    }
}

void DoubleAnimationUsingKeyFrame::ResolveKeyTimes()
{
    _keyTimesIsValid = true;

    Time_t zero = 0;
    Duration duration = GetDuration();
    if (duration.HasTimeSpan())
    {
        zero = (Time_t)duration.GetTimeSpan();
    }
    else
    {
        zero = GetLargestTimeSpanKeyTime();
    }

    int iEndIndex = _keyFrames.Length() - 1;
    for (int i = 0; i < _keyFrames.Length(); ++i)
    {
        KeyTime keyTime = _keyFrames[i]->GetKeyTime();
        switch (keyTime.tt)
        {
        case KeyTimeType::Uniform:
        case KeyTimeType::Paced:
            {
                if (i != iEndIndex)
                {
                    break;
                }
                _keyFrames[i]->GetKeyTime().rt = zero;
                break;
            }
        case KeyTimeType::Percent:
            {
                _keyFrames[i]->GetKeyTime().rt = (Time_t)(keyTime.GetPercent() * zero);
                break;
            }
        case KeyTimeType::TimeSpan:
            {
                _keyFrames[i]->GetKeyTime().rt = (Time_t)keyTime.val;
                break;
            }
        default:
            {
                ;
            }
        }
    }
}

int DoubleAnimationUsingKeyFrame::GetCount() const
{
    return _keyFrames.GetCount();
}

DoubleKeyFrame* DoubleAnimationUsingKeyFrame::GetKeyFrame(int index) const
{
    return _keyFrames.GetItem(index);
}

Object* DoubleAnimationUsingKeyFrame::GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock)
{
    if (_keyFrames.Length() == 0)
    {
        return defOriVal;
    }

    if (!_keyTimesIsValid)
    {
        ResolveKeyTimes();
    }

    int index = 0;
    int iSize = _keyFrames.Length();
    int iEndIndex = iSize - 1;
    Int32 aniValue = 0;
    Time_t currTime = clock->GetCurrentTime();

    while (index < iSize && currTime > _keyFrames[index]->GetKeyTime().rt)
    {
        ++index;
    }

    while (index < iEndIndex && currTime == _keyFrames[index + 1]->GetKeyTime().rt)
    {
        ++index;
    }

    if (index == iSize)
    {
        aniValue = _keyFrames[iEndIndex]->GetValue()->ToFloat();
    }
    else if (currTime == _keyFrames[index]->GetKeyTime().rt)
    {
        aniValue = _keyFrames[index]->GetValue()->ToFloat();
    }
    else
    {
        Object* calcVal = NULL;
        Float keyProgress = 0.0f;
        if (index == 0)
        {
            calcVal = defOriVal;
            keyProgress = (Float)currTime / (Float)(_keyFrames[0]->GetKeyTime().rt);
        }
        else
        {
            calcVal = _keyFrames[index - 1]->GetValue();
            Time_t prevDif = currTime - _keyFrames[index - 1]->GetKeyTime().rt;
            Time_t currDif = _keyFrames[index]->GetKeyTime().rt - currTime;

            keyProgress = (Float)prevDif / (Float)currDif;
            if (FloatUtil::GreaterThan(keyProgress, 1.0))
            {
                keyProgress = 1.0;
            }
        }

        aniValue = _keyFrames[index]->InterpolateValue(calcVal->ToFloat(), keyProgress);
    }

    return new OFloat(aniValue);
}

void DoubleAnimationUsingKeyFrame::Clear()
{
    for (int i = 0; i < _keyFrames.Length(); ++i)
    {
        _keyFrames[i]->unref();
    }
    _keyFrames.Clear();
}

void DoubleAnimationUsingKeyFrame::AddKeyFrame(DoubleKeyFrame* frame)
{
    frame->ref();

    if (_keyFrames.Length() == 0)
    {
        _keyFrames.Add(frame);
    }
    else
    {
        for (int i = 0; i < _keyFrames.Length(); ++i)
        {
            if (frame->GetKeyTime().val < _keyFrames[i]->GetKeyTime().val)
            {
                _keyFrames.Insert(i, frame);
                return;
            }
        }

        _keyFrames.Add(frame);
    }
}

}
