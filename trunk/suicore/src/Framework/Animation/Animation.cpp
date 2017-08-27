// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

#include <System/Windows/CoreTool.h>
#include <Framework/Controls/Window.h>
#include <Framework/Animation/Animation.h>
#include <Framework/Animation/Storyboard.h>

namespace suic
{

Transform::Transform()
    : radio(0)
    , alpha(1)
    , widRadio(1)
    , heiRadio(1)
    , frame(NULL)
    , Object(false)
{
}

Transform::~Transform()
{
    if (frame)
    {
        frame->__unref();
    }
}

void Transform::SetBitmap(Bitmap* bmp)
{
    SetRefObject(frame, bmp);
}

bool Transform::TransformPoint(Point inPoint, Point& result)
{
    return false;
}

bool Transform::TransformRect(Rect inRect, Rect& result)
{
    return false;
}

//-----------------------------------------------------
//

AnimateNotifyArg::AnimateNotifyArg(int iOper)
    : _oper(iOper)
{
}

bool AnimateNotifyArg::IsStart()
{
    return (_oper == 1);
}

bool AnimateNotifyArg::IsFinish()
{
    return (_oper != 1 && _oper != 2);
}

bool AnimateNotifyArg::IsPause()
{
    return (_oper == 2);
}

//-----------------------------------------------------
//

Animate::Animate()
    : _state(0)
    , _restore(false)
    , _target(NULL)
    , _cacheLayer(true)
    , _interpolatedTime(0)
    , _offsetTime(0)
    , _stayAnimate(false)
{
}

Animate::Animate(FrameworkElement* target)
    : _state(0)
    , _restore(false)
    , _target(target)
    , _cacheLayer(true)
    , _interpolatedTime(0)
    , _offsetTime(0)
    , _stayAnimate(false)
{
}

Animate::~Animate()
{
}

Float Animate::GetInterpolation()
{
    if (_interpolatedTime < 0)
    {
        _interpolatedTime = 0;
    }
    else if (_interpolatedTime > 1.0)
    {
        _interpolatedTime = 1.0;
    }

    return _interpolatedTime;
}

Float Animate::ComputeAnimateSpeed()
{
    Float dRatio = _offsetTime / (GetDuration() * 1000);
    Float localSpeed = ComputeInterpolation(dRatio);
    return localSpeed;
}

Float Animate::ComputeInterpolation(Float t)
{
    Float resolveDuration = GetDuration() * 1000;
    Float localSpeed = 1.0;
    Float userAcceleration = GetAccelerationRatio(); 
    Float userDeceleration = GetDecelerationRatio();
    Float transitionTime = userAcceleration + userDeceleration;

    if (t >= 1)
    {
        _interpolatedTime = 1;
        //_currentTime = resolveDuration;
        return 1.0;
    }

    // 无加速和减速
    if (transitionTime == 0)
    { 
        localSpeed = 1;
    }
    else
    {
        Float maxRate = 2 / (2 - transitionTime); 

        // 处于加速阶段
        if (t < userAcceleration) 
        {
            localSpeed = maxRate * t / userAcceleration; 
            t = maxRate * t * t / (2 * userAcceleration);
        }
        // 处于匀速阶段
        else if (t <= (1 - userDeceleration))
        { 
            localSpeed = maxRate;
            t = maxRate * (t - userAcceleration / 2); 
        }
        // 处于减速阶段
        else
        {
            Float tc = 1 - t;
            localSpeed = maxRate * tc / userDeceleration; 
            t = 1 - maxRate * tc * tc / (2 * userDeceleration); 
        }

        // 计算已过时间值
        //_currentTime = t * resolveDuration + 0.5;
    } 

    _interpolatedTime = t;

    return localSpeed;
}

void Animate::SetRestoreLayered(bool val)
{
    _restore = val;
}

bool Animate::IsFinish()
{
    return (!IsRun() && !IsPause());
}

bool Animate::IsRun()
{
    return _state == 1;
}

bool Animate::IsPause()
{
    return _state == 2;
}

void Animate::Start()
{
    suic::StoryBoard* stry(suic::GetStoryBoard(_target));
    if (stry)
    {
        stry->Start(this);
    }
    _state = 1;
}

void Animate::Stop()
{
    suic::StoryBoard* stry(suic::GetStoryBoard(_target));
    if (stry)
    {
        stry->Stop(this);
    }
    _state = 0;
}

void Animate::Pause()
{
    suic::StoryBoard* stry(suic::GetStoryBoard(_target));
    if (stry)
    {
        stry->Pause(this);
    }
}

FrameworkElement* Animate::GetTarget()
{
    return _target;
}

void Animate::SetTarget(FrameworkElement* target)
{
    _target = target;
}

Float Animate::GetInterval()
{
    //
    // 动画时间滑动到下一帧
    //
    _offsetTime += GetFrameRate() * GetSpeedRatio() + 0.5;

    // 
    // 计算动画速度已经插值
    //
    ComputeAnimateSpeed();

    return GetFrameRate();
}

bool Animate::CanAnimate()
{
    return true;
}

bool Animate::GetTransform(Transform* trans)
{
    bool isrun = IsRun();
    DoTransform(GetInterpolation(), trans);
    return isrun;
}

void Animate::DoTransform(Float interTime, Transform* trans)
{
}

void Animate::OnAnimate(FrameworkElement* owner, Rect* rcvalid)
{
    // 计算其何时停止
    if (GetInterpolation() >= 1)
    {
        ++_repeatcount;

        switch (GetRepeatMode())
        {
        case Timeline::RepeatDuration:
            Stop();
            break;

        case Timeline::IterationCount:
            Stop();
            break;

        case Timeline::Forever:
            _repeatcount = 0;
            _offsetTime = 0.0;
            break;

        default:
            ;
        }
    }

    // 如果有动画算法，则设置算法到目标元素上
    if (_target)
    {
        *rcvalid = suic::GetRenderBound(_target);
    }
}

void Animate::OnStart(FrameworkElement* owner)
{
    Window* targetWnd(dynamic_cast<Window*>(_target));
    _offsetTime = -GetFrameRate();
    _state = 1;

    if (_restore && targetWnd)
    {
        if (!targetWnd->AllowsTransparency())
        {
            targetWnd->SetAllowsTransparency(true);
        }
        else
        {
            _restore = false;
        }
    }

    NotifyAnimate(_state);
}

void Animate::OnFinish(FrameworkElement* owner)
{
    _state = 0;
    Window* targetWnd(dynamic_cast<Window*>(_target));

    if (_restore && targetWnd)
    {
        targetWnd->SetAllowsTransparency(false);
    }
    NotifyAnimate(_state);
}

void Animate::OnPause(FrameworkElement* owner)
{
    _state = 2;
    NotifyAnimate(_state);
}

void Animate::OnResume(FrameworkElement* owner)
{
    if (_state == 2)
    {
        _state = 1;
        NotifyAnimate(_state);
    }
}

void Animate::NotifyAnimate(int state)
{
    if (AnimateNotify.safe_bool())
    {
        AnimateNotifyArg e(state);
        AnimateNotify(this, e);
    }
}

void Animate::AddAnimateNotify(AnimateNotifyHandler handler)
{
    AnimateNotify += handler;
}

void Animate::RemoveAnimateNotify()
{
    AnimateNotify.clear();
}

//========================================================
// AnimateGroup

AnimateGroup::AnimateGroup()
{

}

AnimateGroup::~AnimateGroup()
{

}

bool AnimateGroup::IsRun()
{
    UIMutex::Locker sync(_locker);
    for (int i = 0; i < _anis.size(); ++i)
    {
        if (_anis[i]->IsRun())
        {
            return true;
        }
    }
    return false;
}

bool AnimateGroup::IsPause()
{
    UIMutex::Locker sync(_locker);
    for (int i = 0; i < _anis.size(); ++i)
    {
        if (_anis[i]->IsPause())
        {
            return true;
        }
    }
    return false;
}

void AnimateGroup::Start()
{
    if (!GetTarget())
    {
        return;
    }

    UIMutex::Locker sync(_locker);
    for (int i = 0; i < _anis.size(); ++i)
    {
        _anis[i]->_state = 1;
    }
    _state = 1;

    GetTarget()->GetStoryBoard()->Start(this);
}

void AnimateGroup::Stop()
{
    if (!GetTarget())
    {
        return;
    }

    UIMutex::Locker sync(_locker);
    for (int i = 0; i < _anis.size(); ++i)
    {
        _anis[i]->_state = 1;
    }
    _state = 0;
    GetTarget()->GetStoryBoard()->Stop(this);
    GetTarget()->WriteViewFlag(ViewFlags::IsInAnimating, false);
    GetTarget()->SetValue(FrameworkElement::AnimageTagProperty, NULL);
}

void AnimateGroup::Pause()
{
    UIMutex::Locker sync(_locker);
    for (int i = 0; i < _anis.size(); ++i)
    {
        _anis[i]->_state = 2;
    }
    _state = 2;
}

bool AnimateGroup::GetTransform(Transform* trans)
{
    UIMutex::Locker sync(_locker);
    int iEndCount = 0;

    for (int i = 0; i < _anis.size(); ++i)
    {
        _anis[i]->GetTransform(trans);
        if (!_anis[i]->IsRun())
        {
            ++iEndCount;
        }
    }

    if (iEndCount != _anis.size())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void AnimateGroup::DoTransform(Float interTime, Transform* trans)
{
}

void AnimateGroup::OnAnimate(FrameworkElement* owner, Rect* rcvalid)
{
    UIMutex::Locker sync(_locker);
    for (int i = 0; i < _anis.size(); ++i)
    {
        Animate* ani = _anis[i];

        // 
        // 动画有效并且运行中
        //
        if (ani && ani->IsRun() && ani->CanAnimate()
            && ani->CheckInterval((Float)30))
        {
            Rect rect;
            ani->_interval = ani->GetInterval();
            ani->OnAnimate(owner, &rect);

            rcvalid->UnionOnly(&rect);
        }
    }
}

}
