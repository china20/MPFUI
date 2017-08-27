// =============================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// =============================================================

#include <System/Windows/CoreTool.h>
#include <Framework/Animation/Timeline.h>
#include <Framework/Animation/Storyboard.h>

namespace suic
{

Timeline::Timeline()
    : _autoReverse(false)
    , _beginTime(0)
    , _fillMode(0)
    , _repeatMode(RepeatDuration)
    , _speedRatio(1)
    , _duration(150)
    , _accRatio(0)
    , _decRatio(0)
{

}

Timeline::~Timeline()
{
}

TimerBase::TimerBase()
    : _interval(0)
    , _frameRatio(30)
{
}

bool TimerBase::IsRun()
{
    return false;
}

void TimerBase::Start(Element* target)
{
    /*suic::StoryBoard* stry(suic::GetStoryBoard(target));
    if (stry)
    {
        stry->Start(this);
    }*/
}

void TimerBase::OnTimeout(int id, Object*)
{
}

bool TimerBase::CheckInterval(Float val)
{
    _interval -= val;

    // 定时器时间到
    if ((int)_interval <= 5)
    {
        // 重新设置其帧速率，等待下一次定时时间到
        _interval = GetFrameRate();
        return true;
    }
    else
    {
        return false;
    }
}

bool Timeline::GetAutoReverse()
{
    return _autoReverse;
}

void Timeline::SetAutoReverse(bool val)
{
    _autoReverse = val;
}

Float Timeline::GetAccelerationRatio()
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

Float Timeline::GetDecelerationRatio()
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

Float Timeline::GetBeginTime()
{
    return _beginTime;
}

void Timeline::SetBeginTime(Float val)
{
    _beginTime = val;
}

Float Timeline::GetDuration()
{
    return _duration;
}

void Timeline::SetDuration(Float val)
{
    _duration = val;
}

int Timeline::GetPlayMode()
{
    return _fillMode;
}

void Timeline::SetPlayMode(int val)
{
    _fillMode = val;
}

int Timeline::GetRepeatMode()
{
    return _repeatMode;
}

void Timeline::SetRepeatMode(int val)
{
    _repeatMode = val;
}
    
Float Timeline::GetSpeedRatio()
{
    return _speedRatio;
}

void Timeline::SetSpeedRatio(Float val)
{
    _speedRatio = val;
}

Float TimerBase::GetFrameRate()
{
    return _frameRatio;
}

void TimerBase::SetFrameRate(Float val)
{
    _frameRatio = val;
}

}

