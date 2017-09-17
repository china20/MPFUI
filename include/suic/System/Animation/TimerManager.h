// 华勤科技版权所有 2008-2022
// 
// 文件名：TimerManager.h
// 功  能：时钟管理器
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UITIMERMANAGER_H_
#define _UITIMERMANAGER_H_

#include <System/Windows/Object.h>
#include <System/Windows/EventHandler.h>
#include <System/Animation/Transform.h>

namespace suic
{

enum FillBehavior
{
    fbHoldEnd,
    fbStop,
};

class SUICORE_API IEasingFunction : public DpObject
{
public:

    RTTIOfAbstractClass(IEasingFunction)

    virtual ~IEasingFunction() {}
    virtual Float Ease(Float normalizedTime) = 0;
};

struct SUICORE_API Duration
{
    enum DurationType
    {
        dAutomatic,
        dTimeSpan,
        dForever,
    };

    Float timeSpan;
    DurationType durationType;

    Duration();
    Duration(Float t);
    Duration(Float t, DurationType dt);

    Float GetTimeSpan() const;
    DurationType GetDurationType() const;
    bool HasTimeSpan() const;

    static Duration GetForever();
    static Duration GetAutomatic();
};

struct SUICORE_API RepeatBehavior
{
    enum Type 
    {
        // 重复次数
        IterationCount,
        // 不重复
        RepeatDuration,
        // 永远执行
        Forever
    };

    Float count;
    Float duration;
    RepeatBehavior::Type type;

    static RepeatBehavior GetForever();

    RepeatBehavior();
    RepeatBehavior(int count);
    RepeatBehavior(Float duration);
    
    bool HasCount() const;
    bool HasDuration() const;
    Float GetCount() const;
    Float GetDuration() const;
};

class Clock;

class SUICORE_API Timeline : public DpObject
{
public:

    // 时钟完成回调事件
    EventHandler Completed;
    // 时钟移除时回调事件
    EventHandler RemoveRequested;
    // 时钟进度改变时通知事件
    EventHandler TimerInvalidated;
    EventHandler StateInvalidated;

    RTTIOfClass(Timeline)

    Timeline();
    ~Timeline();

    Float GetFrameRate() const;
    void SetFrameRate(Float val);

    bool CheckInterval(Float val);

    /// <summary>
    ///  设置正反序运行属性。
    /// </summary>
    /// <remarks>
    ///  该属性如果为True，指定动画将正序运行后，反序运行一次。
    /// </remarks>
    /// <returns>true: 正序运行后，反序运行一次</returns>
    bool GetAutoReverse() const;
    void SetAutoReverse(bool val);

    /// <summary>
    ///  设置和获取动画加速值
    /// </summary>
    /// <remarks>
    ///  取值范围为0-1，并且和减速值之和不能大于1.
    /// </remarks>
    /// <returns>加速值</returns>
    Float GetAccelerationRatio() const;
    void SetAccelerationRatio(Float val);

    /// <summary>
    ///  设置和获取动画减速值
    /// </summary>
    /// <remarks>
    ///  取值范围为0-1，并且和加速值之和不能大于1.
    /// </remarks>
    /// <returns>减速值</returns>
    Float GetDecelerationRatio() const;
    void SetDecelerationRatio(Float val);

    /// <summary>
    ///  动画运行开始时间。
    /// </summary>
    /// <remarks>
    ///  相对于Duration的动画开始时间。
    /// </remarks>
    /// <returns>动画开始时间</returns>
    Time_t GetBeginTime() const;
    void SetBeginTime(Time_t val);

    /// <summary>
    ///   动画执行时间
    /// </summary>
    /// <remarks>
    ///   动画插值根据此值进行计算，单位为秒。
    /// </remarks>
    /// <returns>动画执行时间</returns>
    Duration GetDuration() const;
    void SetDuration(Duration val);

    /// <summary>
    ///     设置和获取播放动画的方式。
    /// </summary>
    /// <returns>播放动画的方式</returns>
    FillBehavior GetFillBehavior() const;
    void SetFillBehavior(FillBehavior val);

    /// <summary>
    ///     设置和获取动画的重复类型。
    /// </summary>
    /// <returns>动画的重复类型</returns>
    RepeatBehavior GetRepeatBehavior() const;
    void SetRepeatBehavior(RepeatBehavior val);

    /// <summary>
    ///  设置和获取动画的播放速率。
    /// </summary>
    /// <remarks>
    ///  取值0-1减速；大于1加速。
    /// </remarks>
    /// <returns>动画的播放速率</returns>
    Float GetSpeedRatio() const;
    void SetSpeedRatio(Float val);

    Clock* CreateClock(bool hasCtrlRoot);

protected:

    Duration GetNaturalDuration(Clock* clock);

    virtual Clock* AllocateClock();
    virtual Duration GetNaturalDurationCore(Clock* clock);

    friend class Clock;

protected:

    Float _frameRatio;
    bool _autoReverse;

    Time_t _beginTime;
    Float _accRatio;
    Float _decRatio;
    Float _speedRatio;

    Duration _duration;
    FillBehavior _fillMode;
    RepeatBehavior _repeatMode;
};

class TimeManager;
class ClockGroup;

class TimelineGroup : public Timeline, public IAddChild
{
public:

    RTTIOfClass(TimelineGroup)

    TimelineGroup();
    TimelineGroup(Time_t beginTime);
    TimelineGroup(Time_t beginTime, Duration duration);
    TimelineGroup(Time_t beginTime, Duration duration, RepeatBehavior repeatBehavior);

    void AddChild(Object* child);
    void AddText(String text);
    void RemoveChild(Object* child);

    ClockGroup* CreateClock();

protected:

    Clock* AllocateClock();

protected:

    Array<Timeline*, false> _timelines;
};

class SUICORE_API Clock : public Object
{
public:

    enum ClockState
    {
        Active,
        Filling,
        Stopped,
    };

    enum ClockFlags : Uint32
    {
        IsTimeManager = 0x1,
        IsInEventQueue = 0x2,
        IsInGlobalBackward = 0x4,
        IsPausedFlag = 0x8,

        IsStoppedFlag = 0x10,
        IsRootFlag = 0x20,

        HasResolvedDuration = 0x100,
        NeedsTicksWhenActive = 0x200,
        CanControllableRoot = 0x400,

        PendingPauseFlag = 0x1000,
        PendingRemoveFlag = 0x2000,
        PendingResumeFlag = 0x4000,
        PendingStopFlag = 0x8000,
        
        CompletedRaised = 0x10000,
        RemoveRequestedRaised = 0x20000,
        StateInvalidatedRaised = 0x40000,
        TimeInvalidatedRaised = 0x80000,
        HasDesiredFrameRate = 0x100000,
        RootBeginPending = 0x200000,
    };

    RTTIOfClass(Clock)

    Clock();
    Clock(Timeline* timeline);
    ~Clock();

    FloatNullable GetProgress() const;
    TimetNullable GetCurrentTime() const;
    Timeline* GetTimeline() const;

    ClockState GetCurrentState() const;
    bool IsControllableRoot() const;

protected:

    void ComputeLocalState();
    Time_t GetCurrentGlobalTime();

    void RaiseCompletedForRoot(bool isInTick);
    void RaiseRemoveRequestedForRoot();
    void RaiseCompleted();
    void RaiseRemoveRequested();
    void RaiseCurrentStateInvalidated();
    void RaiseCurrentTimeInvalidated();

    bool IsRoot() const;

    bool GetIsInEventQueue() const;
    void SetIsInEventQueue(bool val);

    TimetNullable GetNextTickNeededTime() const;
    void SetNextTickNeededTime(TimetNullable val);

    bool GetIsInGlobalBackward() const;
    void RaiseAccumulatedEvents();

    void AddToTimeManager();
    void SetTimeManager(TimeManager* timeManager);

    Time_t GetDesiredFrameTime(Time_t t, int frameOffset);

    void InternalPause();
    void InternalRemove();
    void InternalResume();
    void InternalStop();

    static Clock* AllocateClock(Timeline* timeline, bool hasCtrlRoot);
    static Clock* BuildClockFromTimeline(Timeline* timeline, bool hasCtrlRoot);

    virtual void OnStopped();
    virtual Duration GetCurrentDuration();
    virtual bool GetNeedsTicksWhenActive() const;
    virtual void SetNeedsTicksWhenActive(bool val);

    friend class TimeManager;
    friend class ClockGroup;
    friend class Timeline;

private:

    void FireCompleted();
    void FireRemoveRequested();
    void FireStateInvalidated();
    void FireTimerInvalidated();

    bool GetFlag(ClockFlags flagMask) const;
    void SetFlag(ClockFlags flagMask, bool bAdd);

    bool CanGrow() const;

    void ResetCachedStateToFilling();
    void ResetCachedStateToStopped();

    void ResolveDuration();
    Duration GetNaturalDuration();
    Duration GetResolvedDuration();
    void NotifyNewEarliestFutureActivity();

    void AdjustBeginTime();
    TimetNullable ComputeEffectiveDuration();
    bool ComputeCurrentSpeed(Float localSpeed);
    bool ComputeInpendingValues();
    bool ComputeExpirationTime(TimetNullable& expirationTime);
    bool ComputeCurrentTime(Float localProgress, Float& localSpeed);
    void ComputeLocalStateCore(bool performTickOper, bool seekedAlignedToLastTick);
    bool ComputeNextTickNeededTime(TimetNullable expirationTime, TimetNullable parentTime, Float parentSpeed);
    bool ComputeCurrentState(TimetNullable expirationTime, TimetNullable& parentTime, Float parentSpeed, bool isInTick);
    bool ComputeCurrentIteration(TimetNullable parentTime, Float parentSpeed, TimetNullable expirationTime, Float& localProgress);

private:

    int _flags;
    ClockGroup* _parent;
    Timeline* _timeline;
    TimeManager* _timeManager;

    // 重复次数
    IntNullable _currentIteration;
    // 当前时钟进度（取值为0-1）
    FloatNullable _currentProgress;
    FloatNullable _currentGlobalSpeed;
    Float _speedRatio;

    Duration _resolvedDuration;
    Duration _currentDuration;
    
    // 开始时间
    TimetNullable _beginTime;
    // 当前时钟经过时间
    TimetNullable _currentTime;
    // 下一次tick时间
    TimetNullable _nextTickNeededTime;
    TimetNullable _iterationBeginTime;

    ClockState _clockState;
};

inline Clock::ClockState Clock::GetCurrentState() const
{
    return _clockState;
}

inline bool Clock::GetFlag(Clock::ClockFlags flag) const
{
    return ((_flags & flag) == flag);
}

class SUICORE_API ClockGroup : public Clock
{
public:

    RTTIOfClass(ClockGroup)

    ClockGroup();
    ClockGroup(Timeline* timeline);
    ~ClockGroup();

    void UpdateClockState();

private:

    Array<WeakRef, true> _roots;
    Array<Clock*, false> _clocks;

    friend class Clock;
};

class AnimationTimeline;

class SUICORE_API AnimationClock : public Clock
{
public:

    RTTIOfAbstractClass(AnimationClock)

    AnimationClock(AnimationTimeline* animation);

    bool GetNeedsTicksWhenActive() const;
 
    /// <summary>
    ///  获取时钟管理的动画对象
    /// </summary>
    /// <remarks>
    ///  动画类型必须为AnimationTimeline
    /// </remarks>
    /// <returns>动画对象</returns>
    AnimationTimeline* GetTimeline();

    /// <summary>
    ///  获取当前时钟进度的动画值
    /// </summary>
    /// <remarks>
    ///  defOriVal为上一个时钟时的动画值，defDestVal为动画的目标值
    /// </remarks>
    /// <returns>动画值</returns>
    virtual Object* GetTimerValue(Object* defOriVal, Object* defDestVal);

};

class SUICORE_API AnimationTimeline : public Timeline
{
public:

    RTTIOfClass(AnimationTimeline)

    AnimationTimeline();
    ~AnimationTimeline();

    AnimationClock* CreateClock(bool hasCtrlRoot);
    virtual Object* GetTimerValue(Object* defOriVal, Object* defDestVal, AnimationClock* clock);

protected:

    Clock* AllocateClock();
    Duration GetNaturalDurationCore(Clock* clock);
};

class SUICORE_API TimeManager
{
public:

    enum TimeState
    {
        Stopped,
        Paused,
        Running
    };

    TimeManager();
    ~TimeManager();

    void Start();
    void Stop();
    void Tick();
    void Pause();
    void RaiseEnqueuedEvents();
    void Restart();
    void Resume();

    Time_t GetNextTickNeeded();
    Time_t GetCurrentTime() const;
    Time_t GetCurrentGlobalTime() const;
    Time_t GetCurrentGlobalTimeDirectly() const;
    int GetMaxDesiredFrameRate();

    void AddToEventQueue(Clock* clock);

protected:

    void LockTickTime();
    void UnlockTickTime();

    void CleanupClocks();

    Time_t GetLastTickDelta() const;
    Time_t GetLastTickTime() const;
    TimeState GetState() const;
    ClockGroup* GetTimeManaClock() const;

    friend class Clock;

private:

    Time_t _startTime;
    Time_t _pauseTime;
    Time_t _globalTime;
    Time_t _lastTickTime;

    TimeState _timeState;
    TimeState _lastTimeState;
    
    bool _isInTick;
    bool _isDirty;
    bool _lockTickTime;
    bool _nextTickTimeQueried;

    ClockGroup* _timeManaClock;
    Array<WeakRef, true> _eventQueue;
};

}

#endif
