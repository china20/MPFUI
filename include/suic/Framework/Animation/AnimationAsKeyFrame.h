
#ifndef _UIANIMATIONASKEYFRAME_H_
#define _UIANIMATIONASKEYFRAME_H_

#include <System/Animation/TimerManager.h>
#include <System/Windows/DpObject.h>
#include <Framework/Animation/EasingFunc.h>

namespace suic
{

enum KeyTimeType : Byte
{
    Paced = 3,
    Percent = 1,
    TimeSpan = 2,
    Uniform = 0
};

struct SUICORE_API KeyTime
{
    Float val;
    Time_t rt;
    KeyTimeType tt;

    KeyTime()
        : val(0)
        , rt(0)
        , tt(KeyTimeType::TimeSpan)
    {
    }

    KeyTime(Float tv)
        : val(tv)
        , rt(tv)
        , tt(KeyTimeType::TimeSpan)
    {
    }

    KeyTime(Float tv, KeyTimeType ttv)
        : val(tv)
        , tt(ttv)
        , rt(tv)
    {
    }

    Float GetPercent();
};

class SUICORE_API IKeyFrame : public Object
{
public:

    RTTIOfAbstractClass(IKeyFrame)

    virtual ~IKeyFrame() {}

    virtual KeyTime& GetKeyTime() = 0;
    virtual Object* GetValue() = 0;
};

class SUICORE_API Int32KeyFrame : public IKeyFrame
{
public:

    RTTIOfAbstractClass(Int32KeyFrame)

    Int32KeyFrame();
    Int32KeyFrame(KeyTime kt, Int32 val);
    ~Int32KeyFrame();

    KeyTime& GetKeyTime()
    {
        return _keyTime;
    }

    Object* GetValue()
    {
        return _value;
    }

    virtual Int32 InterpolateValue(Int32 baseValue, Float progress) = 0;

private:

    KeyTime _keyTime;
    Integer* _value;
};

class SUICORE_API LinearInt32KeyFrame : public Int32KeyFrame
{
public:

    LinearInt32KeyFrame();
    LinearInt32KeyFrame(KeyTime kt, Int32 val);

    Int32 InterpolateValue(Int32 baseValue, Float progress);
};

class SUICORE_API EasingInt32KeyFrame : public Int32KeyFrame
{
public:

    EasingInt32KeyFrame();
    EasingInt32KeyFrame(KeyTime kt, Int32 val);

    void SetEasingFunction(IEasingFunction* val);
    Int32 InterpolateValue(Int32 baseValue, Float progress);

private:

    IEasingFunction* _easingFunc;
};

typedef Array<Int32KeyFrame*> Int32KeyFrameArray;

class SUICORE_API Int32AnimationUsingKeyFrame : public AnimationTimeline, public IAddChild
{
public:

    Int32AnimationUsingKeyFrame();
    ~Int32AnimationUsingKeyFrame();

    void AddChild(Object* obj);
    void AddText(String val);
    void RemoveChild(Object* child);

    int GetCount() const;
    Int32KeyFrame* GetKeyFrame(int index) const;

    Object* GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock);
    void Clear();

private:

    void ResolveKeyTimes();
    Time_t GetLargestTimeSpanKeyTime();
    void AddKeyFrame(Int32KeyFrame* frame);

private:

    bool _keyTimesIsValid;
    Int32KeyFrameArray _keyFrames;
};

//-------------------------------------------------------

class SUICORE_API DoubleKeyFrame : public IKeyFrame
{
public:

    RTTIOfAbstractClass(DoubleKeyFrame)

    DoubleKeyFrame();
    DoubleKeyFrame(KeyTime kt, Float val);
    ~DoubleKeyFrame();

    KeyTime& GetKeyTime()
    {
        return _keyTime;
    }

    Object* GetValue()
    {
        return _value;
    }

    virtual Float InterpolateValue(Float baseValue, Float progress) = 0;

private:

    KeyTime _keyTime;
    OFloat* _value;
};

class SUICORE_API LinearDoubleKeyFrame : public DoubleKeyFrame
{
public:

    LinearDoubleKeyFrame();
    LinearDoubleKeyFrame(KeyTime kt, Float val);

    Float InterpolateValue(Float baseValue, Float progress);
};

class SUICORE_API EasingDoubleKeyFrame : public DoubleKeyFrame
{
public:

    EasingDoubleKeyFrame();
    EasingDoubleKeyFrame(KeyTime kt, Float val);

    void SetEasingFunction(IEasingFunction* val);
    Float InterpolateValue(Float baseValue, Float progress);

private:

    IEasingFunction* _easingFunc;
};

typedef Array<DoubleKeyFrame*> DoubleKeyFrameArray;

class SUICORE_API DoubleAnimationUsingKeyFrame : public AnimationTimeline, public IAddChild
{
public:

    DoubleAnimationUsingKeyFrame();
    ~DoubleAnimationUsingKeyFrame();

    void AddChild(Object* obj);
    void AddText(String val);

    int GetCount() const;
    DoubleKeyFrame* GetKeyFrame(int index) const;

    Object* GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock);
    void Clear();

private:

    void ResolveKeyTimes();
    Time_t GetLargestTimeSpanKeyTime();
    void AddKeyFrame(DoubleKeyFrame* frame);

private:

    bool _keyTimesIsValid;
    DoubleKeyFrameArray _keyFrames;
};

}

#endif
