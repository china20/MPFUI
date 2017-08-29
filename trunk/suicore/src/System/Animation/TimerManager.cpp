// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

#include <System/Windows/CoreTool.h>
#include <System/Animation/TimerManager.h>

namespace suic
{

ImplementRTTIOfClass(IEasingFunction, DpObject)
ImplementRTTIOfClass(Timeline, DpObject)
ImplementRTTIOfClass(TimelineGroup, Timeline)
ImplementRTTIOfClass(Clock, Object)
ImplementRTTIOfClass(ClockGroup, Clock)
ImplementRTTIOfClass(AnimationClock, Clock)
ImplementRTTIOfClass(AnimationTimeline, Timeline)

RepeatBehavior RepeatBehavior::GetForever()
{
    RepeatBehavior behavior;
    behavior.type = Type::Forever;
    return behavior;
}

RepeatBehavior::RepeatBehavior()
    : count(0)
    , duration(0)
    , type(Type::RepeatDuration)
{

}

RepeatBehavior::RepeatBehavior(int count)
    : count(count)
    , duration(0)
    , type(Type::IterationCount)
{

}

RepeatBehavior::RepeatBehavior(Float duration)
    : count(0)
    , duration(duration)
    , type(Type::RepeatDuration)
{

}

bool RepeatBehavior::HasCount() const
{
    return (type == Type::IterationCount);
}

bool RepeatBehavior::HasDuration() const
{
    return (type == Type::RepeatDuration);
}

Float RepeatBehavior::GetCount() const
{
    return count;
}

Float RepeatBehavior::GetDuration() const
{
    return duration;
}

//-------------------------------------------------------
// Duration

Duration::Duration()
    : timeSpan(0)
    , durationType(DurationType::dAutomatic)
{
}

Duration::Duration(Float t)
    : timeSpan(t)
    , durationType(DurationType::dTimeSpan)
{
}

Duration::Duration(Float t, DurationType dt)
    : timeSpan(t)
    , durationType(dt)
{
}

Float Duration::GetTimeSpan() const
{
    return timeSpan;
}

Duration::DurationType Duration::GetDurationType() const
{
    return durationType;
}

bool Duration::HasTimeSpan() const
{
    return (durationType == DurationType::dTimeSpan);
}

Duration Duration::GetAutomatic()
{
    Duration duration;
    duration.durationType = DurationType::dAutomatic;
    return duration;
}

Duration Duration::GetForever()
{
    Duration duration;
    duration.durationType = DurationType::dForever;
    return duration;
}

//-------------------------------------------------------
// Timeline

Timeline::Timeline()
{
    _frameRatio = -1;
    _autoReverse = false;

    _beginTime = 0;
    _accRatio = 0;
    _decRatio = 0;
    _speedRatio = 1.0;

    _duration = Duration::GetAutomatic();
    _fillMode = FillBehavior::fbHoldEnd;
    _repeatMode = RepeatBehavior((int)1);
}

Timeline::~Timeline()
{

}

bool Timeline::GetAutoReverse() const
{
    return _autoReverse;
}

static double MultiplyTimeSpan(double timeSpan, double factor)
{
    return ((factor * timeSpan) + 0.5);
}

static double DivideTimeSpan(double timeSpan, double factor)
{
    return ((timeSpan / factor) + 0.5);
}

void Timeline::SetAutoReverse(bool val)
{
    _autoReverse = val;
}

Float Timeline::GetAccelerationRatio() const
{
    return _accRatio;
}

void Timeline::SetAccelerationRatio(Float val)
{
    _accRatio = val;
    if (_accRatio + GetDecelerationRatio() >= 1)
    {
        _accRatio = 1 - GetDecelerationRatio();
    }
}

Float Timeline::GetDecelerationRatio() const
{
    return _decRatio;
}

void Timeline::SetDecelerationRatio(Float val)
{
    _decRatio = val;
    if (_decRatio + GetAccelerationRatio() >= 1)
    {
        _decRatio = 1 - GetAccelerationRatio();
    }
}

Time_t Timeline::GetBeginTime() const
{
    return _beginTime;
}

void Timeline::SetBeginTime(Time_t val)
{
    _beginTime = val;
}

Duration Timeline::GetDuration() const
{
    return _duration;
}

void Timeline::SetDuration(Duration val)
{
    _duration = val;
}

FillBehavior Timeline::GetFillBehavior() const
{
    return _fillMode;
}

void Timeline::SetFillBehavior(FillBehavior val)
{
    _fillMode = val;
}

RepeatBehavior Timeline::GetRepeatBehavior() const
{
    return _repeatMode;
}

void Timeline::SetRepeatBehavior(RepeatBehavior val)
{
    _repeatMode = val;
}

Float Timeline::GetSpeedRatio() const
{
    return _speedRatio;
}

void Timeline::SetSpeedRatio(Float val)
{
    _speedRatio = val;
}

Clock* Timeline::CreateClock(bool hasCtrlRoot)
{
    return Clock::BuildClockFromTimeline(this, hasCtrlRoot);
}

Clock* Timeline::AllocateClock()
{
    Clock* pClock = new Clock(this);
    pClock->ref();
    return pClock;
}

Float Timeline::GetFrameRate() const
{
    return _frameRatio;
}

void Timeline::SetFrameRate(Float val)
{
    _frameRatio = val;
}

Duration Timeline::GetNaturalDurationCore(Clock* clock)
{
    return Duration::GetAutomatic();
}

Duration Timeline::GetNaturalDuration(Clock* clock)
{
    return GetNaturalDurationCore(clock);
}

//------------------------------------------------------
//

TimelineGroup::TimelineGroup()
{

}

TimelineGroup::TimelineGroup(Time_t beginTime)
{
    SetBeginTime(beginTime);
}

TimelineGroup::TimelineGroup(Time_t beginTime, Duration duration)
{
    SetBeginTime(beginTime);
    SetDuration(duration);
}

TimelineGroup::TimelineGroup(Time_t beginTime, Duration duration, RepeatBehavior repeatBehavior)
{
    SetBeginTime(beginTime);
    SetDuration(duration);
    SetRepeatBehavior(repeatBehavior);
}

void TimelineGroup::AddChild(Object* child)
{
    ObjectPtr cacheObj(child);
    if (RTTICast<Timeline>(child))
    {
        child->ref();
        _timelines.Add(RTTICast<Timeline>(child));
    }
}

void TimelineGroup::AddText(String text)
{

}

void TimelineGroup::RemoveChild(Object* child)
{
    for (int i = 0; i < _timelines.GetCount(); ++i)
    {
        if (_timelines[i] == child)
        {
            _timelines[i]->unref();
            _timelines.RemoveAt(i);
            break;
        }
    } 
}

ClockGroup* TimelineGroup::CreateClock()
{
    return DynamicCast<ClockGroup>(Timeline::CreateClock(true));
}

Clock* TimelineGroup::AllocateClock()
{
    Clock* pClock = new ClockGroup(this);
    pClock->ref();
    return pClock;
}

//------------------------------------------------------
//
Clock::Clock(Timeline* timeline)
{
    _flags = 0;
    _nextTickNeededTime.Reset();
    _timeline = timeline;
    _parent = NULL;

    if (NULL != _timeline)
    {
        _timeline->ref();
        _beginTime = _timeline->GetBeginTime();
        _resolvedDuration = _timeline->GetDuration();
        _speedRatio = _timeline->GetSpeedRatio();
    }
    else
    {
        _speedRatio = 1.0;
    }

    _clockState = ClockState::Stopped;

    if (_beginTime >= 0)
    {
        _nextTickNeededTime = 0;
    }

    if (_resolvedDuration.GetDurationType() == Duration::dAutomatic)
    {
        _resolvedDuration = Duration::GetForever();
    }
    else
    {
        SetFlag(ClockFlags::HasResolvedDuration,true);
    }

    _currentDuration = _resolvedDuration;
    SetFlag(ClockFlags::IsRootFlag, true);
}

Clock::Clock()
{
    _flags = 0;
    _nextTickNeededTime.Reset();
    _parent = NULL;
    _timeline = NULL;
    _speedRatio = 1.0;
    _clockState = ClockState::Stopped;

    if (_beginTime >= 0)
    {
        _nextTickNeededTime = 0;
    }

    if (_resolvedDuration.GetDurationType() == Duration::dAutomatic)
    {
        _resolvedDuration = Duration::GetForever();
    }
    else
    {
        SetFlag(ClockFlags::HasResolvedDuration,true);
    }

    _currentDuration = _resolvedDuration;
    SetFlag(ClockFlags::IsRootFlag, true);
}

Clock::~Clock()
{
    if (_timeline)
    {
        _timeline->unref();
    }
}

FloatNullable Clock::GetProgress() const
{
    return _currentProgress;
}

TimetNullable Clock::GetCurrentTime() const
{
    return _currentTime;
}

Timeline* Clock::GetTimeline() const
{
    return _timeline;
}

bool Clock::IsControllableRoot() const
{
    return GetFlag(ClockFlags::CanControllableRoot);
}

bool Clock::ComputeCurrentSpeed(Float localSpeed)
{
    if (_clockState == ClockState::Stopped)
    {
        _currentGlobalSpeed = 0.0;
    }
    else
    {
        localSpeed *= _speedRatio;
        // 反向动画
        if (GetFlag(ClockFlags::IsInGlobalBackward))
        {
            localSpeed = -localSpeed;
        }

        Float num = localSpeed;
        _currentGlobalSpeed = num * _parent->_currentGlobalSpeed;
    }

    return false;
}

TimetNullable Clock::GetNextTickNeededTime() const
{
    return _nextTickNeededTime;
}

void Clock::SetNextTickNeededTime(TimetNullable val)
{
    _nextTickNeededTime = val;
}

Duration Clock::GetNaturalDuration()
{
    return _timeline->GetNaturalDuration(this);
}

Duration Clock::GetCurrentDuration()
{
    return Duration::GetAutomatic();
}

void Clock::ResolveDuration()
{
    if (!GetFlag(ClockFlags::HasResolvedDuration))
    {
        Duration naturalDuration = GetNaturalDuration();
        if (naturalDuration.GetDurationType() != Duration::dAutomatic)
        {
            _resolvedDuration = naturalDuration;
            _currentDuration = naturalDuration;
            SetFlag(ClockFlags::HasResolvedDuration, true);
        }
    }

    if (CanGrow())
    {
        _currentDuration = GetCurrentDuration();

        if (_currentDuration.GetDurationType() == Duration::dAutomatic)
        {
            _currentDuration = Duration::GetForever();
        }
    }
}

Duration Clock::GetResolvedDuration()
{
    ResolveDuration();
    return _resolvedDuration;
}

bool Clock::GetIsInGlobalBackward() const
{
    return GetFlag(ClockFlags::IsInGlobalBackward);
}

bool Clock::GetIsInEventQueue() const
{
    return GetFlag(ClockFlags::IsInEventQueue);
}

void Clock::SetIsInEventQueue(bool val)
{
    SetFlag(ClockFlags::IsInEventQueue, val);
}

bool Clock::GetNeedsTicksWhenActive() const
{
    return GetFlag(ClockFlags::NeedsTicksWhenActive);
}

void Clock::SetNeedsTicksWhenActive(bool val)
{
    SetFlag(ClockFlags::NeedsTicksWhenActive, val);
}

void Clock::OnStopped()
{

}

void Clock::FireCompleted()
{
    if (GetTimeline()->Completed.safe_bool())
    {
        GetTimeline()->Completed(this, &EventArg::Empty);
    }
}

void Clock::FireRemoveRequested()
{
    if (GetTimeline()->RemoveRequested.safe_bool())
    {
        GetTimeline()->RemoveRequested(this, &EventArg::Empty);
    }
}

void Clock::FireStateInvalidated()
{
    if (GetTimeline()->StateInvalidated.safe_bool())
    {
        GetTimeline()->StateInvalidated(this, &EventArg::Empty);
    }
}

void Clock::FireTimerInvalidated()
{
    if (GetTimeline()->TimerInvalidated.safe_bool())
    {
        GetTimeline()->TimerInvalidated(this, &EventArg::Empty);
    }
}

bool Clock::IsRoot() const
{
    return GetFlag(ClockFlags::IsRootFlag);
}

void Clock::SetFlag(Clock::ClockFlags flagMask, bool bAdd)
{
    if (bAdd)
    {
        _flags |= flagMask;
    }
    else
    {
        _flags &= ~flagMask;
    }
}

void Clock::RaiseCompletedForRoot(bool isInTick)
{
    RaiseCompleted();
}

void Clock::RaiseCompleted()
{
    SetFlag(ClockFlags::CompletedRaised, true);
    if (!GetFlag(ClockFlags::IsInEventQueue))
    {
        _timeManager->AddToEventQueue(this);
        SetFlag(ClockFlags::IsInEventQueue, true);
    }
}

void Clock::RaiseRemoveRequestedForRoot()
{
    RaiseRemoveRequested();
}

void Clock::RaiseRemoveRequested()
{
    SetFlag(ClockFlags::RemoveRequestedRaised, true);
    if (!GetFlag(ClockFlags::RemoveRequestedRaised))
    {
        _timeManager->AddToEventQueue(this);
        SetFlag(ClockFlags::RemoveRequestedRaised, true);
    }
}

void Clock::RaiseCurrentStateInvalidated()
{
    if (_clockState == ClockState::Stopped)
    {
        OnStopped();
    }

    SetFlag(ClockFlags::StateInvalidatedRaised, true);

    if (!GetIsInEventQueue())
    {
        _timeManager->AddToEventQueue(this);
        SetIsInEventQueue(true);
    }
}

void Clock::RaiseCurrentTimeInvalidated()
{
    SetFlag(ClockFlags::TimeInvalidatedRaised, true);

    if (!GetIsInEventQueue())
    {
        _timeManager->AddToEventQueue(this);
        SetIsInEventQueue(true);
    }
}

Time_t Clock::GetCurrentGlobalTime()
{
    if (_timeManager == NULL)
    {
        return 0;
    }

    //
    // 如果不是根ClockGroup则计算子时钟的帧速率
    //
    if (!GetFlag(ClockFlags::IsTimeManager))
    {
        if (GetFlag(ClockFlags::HasDesiredFrameRate))
        {
            return 0;
        }
    }

    //
    // 直接返回当前时钟经过的时间段
    //
    return _timeManager->GetCurrentGlobalTimeDirectly();
}

void Clock::ResetCachedStateToFilling()
{
    _currentGlobalSpeed.Reset();
    _clockState = ClockState::Filling;
    SetFlag(ClockFlags::IsInGlobalBackward, false);
}

void Clock::ResetCachedStateToStopped()
{
    _currentGlobalSpeed.Reset();
    _currentIteration.Reset();
    _currentProgress.Reset();
    _currentTime.Reset();
    _clockState = ClockState::Stopped;
    SetFlag(ClockFlags::IsInGlobalBackward, false);
}

void Clock::NotifyNewEarliestFutureActivity()
{

}

bool Clock::ComputeInpendingValues()
{
    bool flag = false;

    if (GetFlag(ClockFlags::PendingStopFlag))
    {
        SetFlag(ClockFlags::PendingStopFlag, false);
        SetFlag(ClockFlags::IsStoppedFlag, false);

        _beginTime.Reset();
        _iterationBeginTime.Reset();

        if (CanGrow())
        {
            //ResetSlipOnSubtree();
        }
    }

    if (GetFlag(ClockFlags::IsStoppedFlag))
    {
        ResetCachedStateToStopped();
        _nextTickNeededTime.Reset();
        flag = true;
    }
    else
    {
        AdjustBeginTime();
    }

    if (GetFlag(ClockFlags::PendingPauseFlag))
    {
        SetFlag(ClockFlags::PendingPauseFlag, false);
        //RaiseCurrentGlobalSpeedInvalidated();
    }

    if (GetFlag(ClockFlags::PendingResumeFlag))
    {
        SetFlag(ClockFlags::PendingResumeFlag, false);

        if (_clockState != ClockState::Filling)
        {
            //RaiseCurrentGlobalSpeedInvalidated();
        }
    }

    return flag;
}

void Clock::AdjustBeginTime()
{
    if (GetFlag(ClockFlags::RootBeginPending))
    {
        Time_t currentGlobalTime = GetCurrentGlobalTime();
        _beginTime = currentGlobalTime + _timeline->GetBeginTime();

        if (CanGrow())
        {
            _iterationBeginTime = _beginTime;
        }

        SetFlag(ClockFlags::RootBeginPending, false);
    }
}

bool Clock::ComputeCurrentState(TimetNullable expirationTime, TimetNullable& parentTime, Float parentSpeed, bool isInTick)
{
    FillBehavior fillBehavior = _timeline->GetFillBehavior();
    TimetNullable span2 = parentTime;
    TimetNullable nullable2 = _beginTime;

    //
    // 当前时间小于开始时间，动画停止
    //
    if (nullable2.HasValue() ? (span2 < nullable2) : false)
    {
        ResetCachedStateToStopped();
        return true;
    }

    if (expirationTime.HasValue())
    {
        TimetNullable span = parentTime;
        TimetNullable nullable = expirationTime;

        //
        // 返回true表示当前全局时间大于到期时间
        //
        if (nullable.HasValue() ? (span >= nullable) : false)
        {
            RaiseCompletedForRoot(isInTick);
            RaiseRemoveRequestedForRoot();

            if (fillBehavior == FillBehavior::fbHoldEnd)
            {
                ResetCachedStateToFilling();
                parentTime = expirationTime;

                if (parentSpeed != 0.0 && _clockState == ClockState::Active && GetNeedsTicksWhenActive())
                {
                    SetNextTickNeededTime(0);
                }

                return false;
            }

            ResetCachedStateToStopped();
            return true;
        }
    }
    
    _clockState = ClockState::Active;

    if (parentSpeed != 0.0 && GetNeedsTicksWhenActive())
    {
        SetNextTickNeededTime(0);
    }

    return false;
}

void Clock::ComputeLocalStateCore(bool performTickOper, bool seekedAlignedToLastTick)
{
    // 获取父时钟的当前时间和全局时间
    TimetNullable parentTime = _timeManager->GetCurrentGlobalTimeDirectly();
    Float parentSpeed = 1.0;
    bool parentRun = _parent->_clockState != ClockState::Stopped;
    TimetNullable expirationTime;

    if (performTickOper && IsRoot() && ComputeInpendingValues())
    {
        parentRun = false;
    }

    if (ComputeExpirationTime(expirationTime))
    {
        parentRun = false;
    }

    if (performTickOper && GetCurrentState() == ClockState::Active)
    {
        RaiseCurrentTimeInvalidated();
    }

    //
    // 时钟还在运行
    //
    if (parentRun)
    {
        Float num = 0;
        Float span2 = 0;

        if (((!ComputeNextTickNeededTime(expirationTime, parentTime, parentSpeed) && 
            !ComputeCurrentState(expirationTime, parentTime, parentSpeed, performTickOper)) && 
            !ComputeCurrentIteration(parentTime, parentSpeed, expirationTime, span2)) && 
            !ComputeCurrentTime(span2, num))
        {
            ComputeCurrentSpeed(num);
        }
    }
}

void Clock::ComputeLocalState()
{
    ClockState state = _clockState;
    FloatNullable nullable = _currentGlobalSpeed;

    //PauseStateChangedDuringTick = false;
    ComputeLocalStateCore(true, false);

    if (state != _clockState)
    {
        RaiseCurrentStateInvalidated();
        RaiseCurrentTimeInvalidated();
    }
}

bool Clock::ComputeExpirationTime(TimetNullable& expirationTime)
{
    if (!_beginTime.HasValue())
    {
        expirationTime = 0;
        return true;
    }

    TimetNullable effVal = ComputeEffectiveDuration();

    if (effVal.HasValue())
    {
        expirationTime = _beginTime + (Time_t)effVal;
    }
    else
    {
        // 无穷大过期时间
        expirationTime.Reset();
    }

    return false;
}

bool Clock::CanGrow() const
{
    return false;
}

TimetNullable Clock::ComputeEffectiveDuration()
{
    ResolveDuration();

    TimetNullable effVal;
    RepeatBehavior repeatBehavior = _timeline->GetRepeatBehavior();

    if (repeatBehavior.HasCount())
    {
        Float num2 = 0;
        if (repeatBehavior.GetCount() == 0)
        {
            return 0;
        }

        if (_currentDuration.GetDurationType() == Duration::dForever)
        {
            return effVal;
        }

        if (!CanGrow())
        {
            double factor = repeatBehavior.GetCount() / _speedRatio;
            if (_timeline->GetAutoReverse())
            {
                factor *= 2.0;
            }
            return MultiplyTimeSpan(_currentDuration.GetTimeSpan(), factor);
        }

        Float zero = 0;
        double count = repeatBehavior.GetCount();

        if (count <= 1.0)
        {
            num2 = _currentDuration.GetTimeSpan() * count;
        }
        else
        {
            num2 = _currentDuration.GetTimeSpan() + (_currentDuration.GetTimeSpan() * (count - 1.0));
        }

        if (_timeline->GetAutoReverse())
        {
            num2 *= 2.0;
        }

        return (((num2 / _speedRatio) + 0.5) + zero);
    }

    if (repeatBehavior.HasDuration())
    {
        //return _timeline->GetDuration().GetTimeSpan();
        return repeatBehavior.GetDuration();
    }
    else
    {
        return effVal;
    }
}

bool Clock::ComputeNextTickNeededTime(TimetNullable expirationTime, TimetNullable parentTime, Float parentSpeed)
{
    if ((int)parentSpeed == 0)
    {
        if (GetFlag(ClockFlags::IsPausedFlag))
        {
            SetNextTickNeededTime(0);
        }
        else
        {
            _nextTickNeededTime.Reset();
        }
    }
    else
    {
        Float factor = 1.0 / parentSpeed;
        TimetNullable nullable3;
        TimetNullable span2 = (Time_t)MultiplyTimeSpan(_beginTime - parentTime, factor);

        //
        // 如果开始时间大于等于当前时间
        //
        if (span2 >= 0)
        {
            nullable3 = span2;
        }

        // 超时时间有效
        if (expirationTime.HasValue())
        {
            TimetNullable span = (Time_t)MultiplyTimeSpan(expirationTime - parentTime, factor);
            if ((span >= 0) && (!nullable3.HasValue() || (span < nullable3)))
            {
                nullable3 = span;
            }
        }

        if (nullable3.HasValue())
        {
            Time_t currentGlobalTime = GetCurrentGlobalTime();
            SetNextTickNeededTime(currentGlobalTime + nullable3);
        }
        else
        {
            _nextTickNeededTime.Reset();
        }
    }

    return false;
}

bool Clock::ComputeCurrentTime(Float localProgress, Float& localSpeed)
{
    if (_currentDuration.HasTimeSpan())
    {
        Float accelerationRatio = _timeline->GetAccelerationRatio();
        Float decelerationRatio = _timeline->GetDecelerationRatio();
        Float num7 = accelerationRatio + decelerationRatio;
        Float ticks = _currentDuration.GetTimeSpan();
        Float num = ((Float)localProgress) / ticks;

        // 没有加速和减速，匀速前进
        if (num7 == 0)
        {
            localSpeed = 1.0;
            _currentTime = localProgress;
        }
        else
        {
            Float num2 = 2.0f / (2.0f - num7);
            // 当前进度处于加速期间
            if (num < accelerationRatio)
            {
                // 重新计算速度
                localSpeed = (num2 * num) / accelerationRatio;
                num = ((num2 * num) * num) / (2.0f * accelerationRatio);
                if (_clockState == ClockState::Active)
                {
                    SetNextTickNeededTime(0);
                }
            }
            else if (num <= (1.0 - decelerationRatio))
            {
                localSpeed = num2;
                num = num2 * (num - (accelerationRatio / 2.0));
            }
            else
            {
                Float num5 = 1.0 - num;
                localSpeed = (num2 * num5) / decelerationRatio;
                num = 1.0 - (((num2 * num5) * num5) / (2.0f * decelerationRatio));

                if (_clockState == ClockState::Active)
                {
                    SetNextTickNeededTime(0);
                }
            }

            // 重新计算当前时间
            _currentTime = (Time_t)((num * ticks) + 0.5);
        }

        _currentProgress = num;
    }
    else
    {
        _currentTime = (Time_t)localProgress;
        _currentProgress = 0.0;
        localSpeed = 1.0;
    }

    return (_clockState != ClockState::Active);
}

bool Clock::ComputeCurrentIteration(TimetNullable parentTime, Float parentSpeed, TimetNullable expirationTime, Float& localProgress)
{
    // 动画开始启动时间
    TimetNullable span7 = _beginTime;
    RepeatBehavior repeatBehavior = _timeline->GetRepeatBehavior();
    // 当前时间相对于开始时间的偏移
    TimetNullable offsetFromBegin = (Time_t)MultiplyTimeSpan(parentTime - span7, _speedRatio);

    SetFlag(ClockFlags::IsInGlobalBackward, _parent->GetIsInGlobalBackward());

    //
    // 当前Duration指定为期间 
    //
    if (_currentDuration.HasTimeSpan())
    {
        Time_t newIteration = 0;

        // Duration无效
        if ((int)_currentDuration.GetTimeSpan() == 0)
        {
            Float currentProgress = 0;
            localProgress = 0;
            _currentTime = 0;

            if (repeatBehavior.HasCount())
            {
                Float count = repeatBehavior.GetCount();

                if (count <= 1)
                {
                    Float count = repeatBehavior.GetCount();
                    if (count <= 1.0)
                    {
                        currentProgress = count;
                        _currentIteration = 1;
                    }
                    else
                    {
                        Float wholePart = (int)count;
                        if (count == wholePart)
                        {
                            currentProgress = 1.0;
                            _currentIteration = (int)count;
                        }
                        else
                        {
                            currentProgress = count - wholePart;
                            _currentIteration = ((int)(count + 1.0));
                        }
                    }
                }
                else
                {
                    currentProgress = 1.0;
                    _currentIteration = count;
                }
            }
            else
            {
                _currentIteration = 1;
                currentProgress = 1.0;
            }

            if (_timeline->GetAutoReverse())
            {
                if (currentProgress == 1.0)
                {
                    currentProgress = 0.0;
                }
                else if (currentProgress < 0.5)
                {
                    currentProgress *= 2.0;
                }
                else
                {
                    currentProgress = 1.0 - ((currentProgress - 0.5) * 2.0);
                }
            }

            _currentProgress = currentProgress;
            return true;
        }

        //
        // 整个动画时钟已经结束
        //
        if (_clockState == ClockState::Filling && repeatBehavior.HasCount())
        {
            Float factor = repeatBehavior.GetCount();
            
            if (_timeline->GetAutoReverse())
            {
                factor *= 2.0;
            }
            offsetFromBegin = MultiplyTimeSpan(_currentDuration.GetTimeSpan(), factor);
        }

        // 
        // 得到当前所经过的时间
        //
        localProgress = (int)offsetFromBegin % (int)_currentDuration.GetTimeSpan();

        // 
        // 计算得到经过了多少个duration
        //
        newIteration = (int)(offsetFromBegin / _currentDuration.GetTimeSpan());

        if (((localProgress == 0) && (newIteration > 0)) && ((_clockState == ClockState::Filling) || _parent->GetIsInGlobalBackward()))
        {
            localProgress = _currentDuration.GetTimeSpan();
            newIteration--;
        }

        if (_timeline->GetAutoReverse())
        {
            // Duration的奇数倍，反转运行
            if ((newIteration & 1) == 1)
            {
                if (localProgress == 0)
                {
                    SetNextTickNeededTime(0);
                }
                localProgress = _currentDuration.GetTimeSpan() - localProgress;
                SetFlag(ClockFlags::IsInGlobalBackward, !GetFlag(ClockFlags::IsInGlobalBackward));
                parentSpeed = -parentSpeed;
            }
            newIteration /= 2;
        }

        // 得到当前已经运行的次数（运行了多少个Duration）
        _currentIteration = 1 + newIteration;

        if (_clockState == ClockState::Active && parentSpeed != 0.0 && !GetNeedsTicksWhenActive())
        {
            Time_t tNextBoundary = 0;

            if (localProgress == 0)
            {
                tNextBoundary = DivideTimeSpan(_currentDuration.GetTimeSpan(), Math::Abs(parentSpeed));
            }
            else if (parentSpeed > 0.0)
            {
                Time_t tDecelBegin = MultiplyTimeSpan(_currentDuration.GetTimeSpan(), 1.0 - _timeline->GetDecelerationRatio());
                tNextBoundary = DivideTimeSpan(tDecelBegin - localProgress, parentSpeed);
            }
            else
            {
                Time_t tAccelEnd = MultiplyTimeSpan(_currentDuration.GetTimeSpan(), _timeline->GetAccelerationRatio());
                tNextBoundary = DivideTimeSpan(tAccelEnd - localProgress, parentSpeed);
            }

            Time_t proposedNextTickTime = GetCurrentGlobalTime() + tNextBoundary;
            if (!_nextTickNeededTime.HasValue() || (proposedNextTickTime < GetNextTickNeededTime()))
            {
                SetNextTickNeededTime(proposedNextTickTime);
            }
        }
    }
    else
    {
        localProgress = offsetFromBegin;
        _currentIteration = 1;
    }

    return false;
}

void Clock::SetTimeManager(TimeManager* timeManager)
{
    if (_timeManager != timeManager)
    {
        _timeManager = timeManager;
    }
}

void Clock::AddToTimeManager()
{
    TimeManager* timeManager = Assigner::Current()->GetTimeManager();
    if (timeManager != NULL)
    {
        _parent = timeManager->GetTimeManaClock();
        SetTimeManager(_parent->_timeManager);
        _parent->_roots.Add(WeakRef(this));

        //
        // 设置此标志，让动画启动时自动调整其begintime
        // 因为begintime需要动画启动的globaltime时间参与计算
        //
        SetFlag(ClockFlags::RootBeginPending, true);
    }
}

Time_t Clock::GetDesiredFrameTime(Time_t t, int frameOffset)
{
    static Time_t s_TimeSpanTicksPerSecond = 1.0;
    Time_t _desiredFrameRate = 0;

    Float desiredFrameRate = (Float)_desiredFrameRate;
    Time_t num3 = ((t * desiredFrameRate) / s_TimeSpanTicksPerSecond) + frameOffset;
    Time_t num2 = (num3 * s_TimeSpanTicksPerSecond) / desiredFrameRate;
    return num2;
}

void Clock::InternalPause()
{
    if (GetFlag(ClockFlags::PendingResumeFlag))
    {
        SetFlag(ClockFlags::PendingResumeFlag, false);
    }
    else if (!GetFlag(ClockFlags::IsPausedFlag))
    {
        SetFlag(ClockFlags::PendingPauseFlag, true);
    }

    NotifyNewEarliestFutureActivity();
}

void Clock::InternalRemove()
{
    SetFlag(ClockFlags::PendingRemoveFlag, true);
    InternalStop();
}

void Clock::InternalResume()
{
    if (GetFlag(ClockFlags::PendingPauseFlag))
    {
        SetFlag(ClockFlags::PendingPauseFlag, false);
    }
    else if (GetFlag(ClockFlags::IsPausedFlag))
    {
        SetFlag(ClockFlags::PendingResumeFlag, true);
    }
    NotifyNewEarliestFutureActivity();
}

void Clock::InternalStop()
{
    SetFlag(ClockFlags::PendingStopFlag, true);
    SetFlag(ClockFlags::RootBeginPending, false);
    //ResetNodesWithSlip();
    NotifyNewEarliestFutureActivity();
}

void Clock::RaiseAccumulatedEvents()
{
    //try
    {
        if (GetFlag(ClockFlags::TimeInvalidatedRaised))
        {
            FireTimerInvalidated();
        }

        if (GetFlag(ClockFlags::StateInvalidatedRaised))
        {
            FireStateInvalidated();
        }

        if (GetFlag(ClockFlags::CompletedRaised))
        {
            FireCompleted();
        }

        if (GetFlag(ClockFlags::RemoveRequestedRaised))
        {
            FireRemoveRequested();
        }
    }
    /*catch (...)
    {
        ;
    }*/

    SetFlag(ClockFlags::TimeInvalidatedRaised, false);
    SetFlag(ClockFlags::StateInvalidatedRaised, false);
    SetFlag(ClockFlags::CompletedRaised, false);
    SetFlag(ClockFlags::RemoveRequestedRaised, false);
    SetFlag(ClockFlags::IsInEventQueue, false);
}

Clock* Clock::AllocateClock(Timeline* timeline, bool hasCtrlRoot)
{
    Clock* clock = timeline->AllocateClock();
    if (clock != NULL)
    {
        clock->SetFlag(ClockFlags::CanControllableRoot, hasCtrlRoot);
    }
    return clock;
}

Clock* Clock::BuildClockFromTimeline(Timeline* timeline, bool hasCtrlRoot)
{
    Clock* clock = AllocateClock(timeline, hasCtrlRoot);
    //clock.BuildClockSubTreeFromTimeline(clock.Timeline, hasControllableRoot);
    clock->AddToTimeManager();
    return clock;
}

//---------------------------------------------------------
//

StoryBoard::StoryBoard()
{

}

StoryBoard::~StoryBoard()
{

}

FrameworkElement* StoryBoard::GetOwner()
{
    return NULL;
}

void StoryBoard::SetOwner(FrameworkElement* owner)
{

}

AnimationClock::AnimationClock(AnimationTimeline* animation)
    : Clock(animation)
{
}

bool AnimationClock::GetNeedsTicksWhenActive() const
{
    return true;
}

AnimationTimeline* AnimationClock::GetTimeline()
{
    return DynamicCast<AnimationTimeline>(Clock::GetTimeline());
}

Object* AnimationClock::GetTimerValue(Object* defOriVal, Object* defDestVal)
{
    AnimationTimeline* aniTimeline = GetTimeline();
    if (NULL == aniTimeline)
    {
        return DpProperty::UnsetValue();
    }
    else
    {
        return aniTimeline->GetTimerValue(defOriVal, defDestVal, this);
    }
}

AnimationTimeline::AnimationTimeline()
{

}

AnimationTimeline::~AnimationTimeline()
{

}

Object* AnimationTimeline::GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock)
{
    //ReadPreamble();
    return defDestVal;
}

Duration AnimationTimeline::GetNaturalDurationCore(Clock* clock)
{
    return Duration(1000);
}

AnimationClock* AnimationTimeline::CreateClock(bool hasCtrlRoot)
{
    return DynamicCast<AnimationClock>(Timeline::CreateClock(hasCtrlRoot));
}

Clock* AnimationTimeline::AllocateClock()
{
    Clock* pClock = new AnimationClock(this);
    pClock->ref();
    return pClock;
}

ClockGroup::ClockGroup()
    : Clock(NULL)
{

}

ClockGroup::ClockGroup(Timeline* timeline)
    : Clock(timeline)
{

}

ClockGroup::~ClockGroup()
{

}

void ClockGroup::UpdateClockState()
{
    int iCount = _roots.Length();

    for (int i = 0; i < iCount; ++i)
    {
        Clock* curr = DynamicCast<Clock>(_roots[i].GetTarget());

        if (NULL != curr)
        {
            Time_t currGlobalTime = GetCurrentGlobalTime();
            TimetNullable nextTickNeededTime = curr->GetNextTickNeededTime();

            //
            // 当前时钟到，nextTickNeededTime如果小于0表示无效时间
            //
            if (nextTickNeededTime.HasValue() && currGlobalTime >= nextTickNeededTime)
            {
                curr->ComputeLocalState();
            }
        }
        else
        {
            _roots.RemoveAt(i);
            --i;
            --iCount;
        }
    }
}

TimeManager::TimeManager()
{
    _globalTime = 0;
    _pauseTime = 0;
    _startTime = 0;
    _lastTickTime = 0;
    _lastTickTime = -1;
    _timeManaClock = new ClockGroup();
    _timeManaClock->ref();

    _timeState = TimeState::Stopped;
    _lastTimeState = TimeState::Stopped;
    _nextTickTimeQueried = false;
    _isInTick = false;
    _lockTickTime = false;

    _timeManaClock->_timeManager = this;
}

TimeManager::~TimeManager()
{
    _timeManaClock->unref();
}

Time_t TimeManager::GetCurrentTime() const
{
    if (_timeState == TimeState::Stopped)
    {
        return -1;
    }
    else
    {
        return _globalTime;
    }
}

Time_t TimeManager::GetCurrentGlobalTimeDirectly() const
{
    return _globalTime;
}

Time_t TimeManager::GetCurrentGlobalTime() const
{
    switch (_timeState)
    {
    case TimeState::Stopped:
        return 0;

    case TimeState::Paused:
        return (_pauseTime - _startTime);

    case TimeState::Running:
        if (!_isInTick && !_lockTickTime)
        {
            return (Environment::GetSystemTick() - _startTime);
        }
        else
        {
            return _globalTime;
        }
    }

    return 0;
}

Time_t TimeManager::GetLastTickDelta() const
{
    return (_globalTime - _lastTickTime);
}

Time_t TimeManager::GetLastTickTime() const
{
    return _lastTickTime;
}

TimeManager::TimeState TimeManager::GetState() const
{
    return _timeState;
}

ClockGroup* TimeManager::GetTimeManaClock() const
{
    return _timeManaClock;
}

void TimeManager::AddToEventQueue(Clock* sender)
{
    _eventQueue.Add(WeakRef(sender));
}

void TimeManager::Start()
{
    if (_timeState == TimeState::Stopped)
    {
        _lastTickTime = 0;
        _startTime = Environment::GetSystemTick();
        _globalTime = 0;
        _timeState = TimeState::Running;
        GetTimeManaClock()->_clockState = Clock::ClockState::Active;
        //_timeManaClock->RootActivate();
    }
}

void TimeManager::Stop()
{
    if (_timeState >= TimeState::Paused)
    {
        //_timeManaClock->RootDisable();
        _timeState = TimeState::Stopped;
    }

}

void TimeManager::Tick()
{
    //try
    {
        //
        // 如果已经在tick中，仅仅运行更新时钟状态，不允许触发事件防止递归
        //
        if (_isInTick)
        {
            _timeManaClock->UpdateClockState();
            return;
        }

        _nextTickTimeQueried = false;
        _isDirty = false;

        //
        // 这里更新当前经过的全局时间（相对于_startTime的时间间隔）
        //
        if (_timeState == TimeState::Running)
        {
            _globalTime = GetCurrentGlobalTime();
            _isInTick = true;
        }

        Time_t span = _startTime + _globalTime;
        
        if ((_lastTimeState == TimeState::Stopped) && (_timeState == TimeState::Stopped))
        {
            //_currentTickInterval = TimeIntervalCollection.CreateNullPoint();
        }
        else
        {
            //_currentTickInterval = TimeIntervalCollection.CreateOpenClosedInterval(_lastTickTime, _globalTime);
            if ((_lastTimeState == TimeState::Stopped) || (_timeState == TimeState::Stopped))
            {
                //_currentTickInterval.AddNullPoint();
            }
        }

        _timeManaClock->UpdateClockState();
        _lastTimeState = _timeState;
        RaiseEnqueuedEvents();
    }
    /*catch (...)
    {
    }*/

    _isInTick = false;
    _lastTickTime = _globalTime;

    CleanupClocks();
}

void TimeManager::Pause()
{
    if (_timeState == TimeState::Running)
    {
        _pauseTime = Environment::GetSystemTick();
        _timeState = TimeState::Paused;
    }
}

void TimeManager::RaiseEnqueuedEvents()
{
    while (_eventQueue.Length() > 0)
    {
        int index = _eventQueue.Length() - 1;
        Clock* target = _eventQueue[index].GetElement<Clock>();

        if (target != NULL)
        {
            target->ref();
            target->RaiseAccumulatedEvents();
            target->unref();
        }

        _eventQueue.RemoveAt(index);
    }

    _eventQueue.Clear();
}

void TimeManager::Restart()
{
    TimeState state = _timeState;
    Stop();
    Start();
    _timeState = state;

    if (_timeState == TimeState::Paused)
    {
        _pauseTime = _startTime;
    }
}

void TimeManager::Resume()
{
    if (_timeState == TimeState::Paused)
    {
        _startTime = (_startTime + Environment::GetSystemTick()) - _pauseTime;
        _timeState = TimeState::Running;

        if (GetNextTickNeeded() >= 0)
        {
            //NotifyNewEarliestFutureActivity();
        }
    }
}

Time_t TimeManager::GetNextTickNeeded()
{
    _nextTickTimeQueried = true;

    if (_timeState != TimeState::Running)
    {
        return -1;
    }

    Time_t nextTickNeededTime = _timeManaClock->GetNextTickNeededTime();
    if (nextTickNeededTime == 0)
    {
        return -1;
    }

    Time_t span2 = Environment::GetSystemTick() - _startTime;
    Time_t span = nextTickNeededTime - span2;

    if (span <= 0)
    {
        return 0;
    }

    return span;
}

int TimeManager::GetMaxDesiredFrameRate()
{
    //_timeManaClock->GetMaxDesiredFrameRate();
    return 16;
}

void TimeManager::LockTickTime()
{
    _lockTickTime = true;
}

void TimeManager::UnlockTickTime()
{
    _lockTickTime = false;
}

//void TimeManager::NotifyNewEarliestFutureActivity()
//{
//    if (_nextTickTimeQueried)
//    {
//        _nextTickTimeQueried = false;
//        //_userNeedTickSooner(this, EventArgs::Empty);
//    }
//}

void TimeManager::CleanupClocks()
{
}

}
