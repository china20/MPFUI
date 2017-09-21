
#include <System/Animation/AnimationStorage.h>
#include <System/Animation/TimerManager.h>
#include <System/Windows/MemberVisitor.h>

namespace suic
{
 
UncommonField<AnimationStorageDic> AnimationStorage::AnimatedPropMapField;

ImplementRTTIOfClass(AnimationStorage, DpObject)

AnimationStorageDic::AnimationStorageDic()
{
}

AnimationStorageDic::~AnimationStorageDic()
{

}

AnimationStorage::AnimationStorage()
    : _dpProp(NULL)
    , _baseValue(NULL)
{
}

AnimationStorage::~AnimationStorage()
{
    if (_baseValue)
    {
        _baseValue->unref();
    }
    ClearAnimations();
}

AnimationStorage* AnimationStorage::GetStorage(DpObject* d, DpProperty* dp)
{
    AnimationStorageDic* pAniStorage = AnimatedPropMapField.GetValue(d);
    if (NULL == pAniStorage)
    {
        return NULL;
    }
    else
    {
        AnimationStorage* pStorage = NULL;
        pAniStorage->TryGetValue(dp->GetIndex(), pStorage);
        return pStorage;
    }
}

AnimationStorage* AnimationStorage::CreateStorage(DpObject* d, DpProperty* dp)
{
    AnimationStorageDic* pAniStorage = AnimatedPropMapField.GetValue(d);
    if (NULL == pAniStorage)
    {
        pAniStorage = new AnimationStorageDic();
        AnimatedPropMapField.SetValue(d, pAniStorage);
    }

    AnimationStorage* pStorage = new AnimationStorage();
    pStorage->Initialize(d, dp);
    pAniStorage->Add(dp->GetIndex(), pStorage);
    return pStorage;
}

void AnimationStorage::DetachAnimationClock(AnimationClock* clock)
{
    clock->GetTimeline()->TimerInvalidated -= EventHandler(this, &AnimationStorage::OnTimeInvalidated);

    if (clock->IsControllableRoot())
    {
        clock->GetTimeline()->RemoveRequested -= EventHandler(this, &AnimationStorage::OnRemoveRequested);
    }
}

int AnimationStorage::IndexOf(AnimationClock* clock)
{
    for (int i = 0; i < _animationClocks.Length(); ++i)
    {
        if (clock == _animationClocks[i])
        {
            return i;
        }
    }
    return -1;
}

int AnimationStorage::GetCount() const
{
    return _animationClocks.Length();
}

AnimationClock* AnimationStorage::GetClock(int index)
{
    return _animationClocks[index];
}

void AnimationStorage::ClearAnimations(DpObject* d, DpProperty* dp)
{
    AnimationStorageDic* pAniStorage = AnimatedPropMapField.GetValue(d);
    if (NULL != pAniStorage)
    {
        AnimationStorage* pStorage = NULL;
        pAniStorage->TryGetValue(dp->GetIndex(), pStorage);
        if (NULL != pStorage)
        {
            pStorage->ClearAnimations();
            pAniStorage->Remove(dp->GetIndex());
        }
        if (pAniStorage->GetCount() == 0)
        {
            AnimatedPropMapField.ClearValue(d);
        }
    }
}

void AnimationStorage::ClearAnimations()
{
    for (int i = 0; i < _animationClocks.Length(); ++i)
    {
        AnimationClock* clock = _animationClocks[i];
        if (clock)
        {
            DetachAnimationClock(clock);
            clock->GetTimeline()->TimerInvalidated.Clear();
            clock->unref();
        }
    }

    _animationClocks.Clear();
}

void AnimationStorage::Initialize(DpObject* d, DpProperty* dp)
{
    _dpObject = d;
    _dpProp = dp;
}

void AnimationStorage::ApplyAnimationClock(DpObject* d, DpProperty* dp, AnimationClock* clock)
{
    AnimationStorage* pStorage = GetStorage(d, dp);

    if (NULL == pStorage)
    {
        pStorage = CreateStorage(d, dp);
    }
    else
    {
        pStorage->ClearAnimations();
    }

    if (NULL != clock)
    {
        clock->ref();
        SETREFOBJ(pStorage->_baseValue, d->GetValue(pStorage->_dpProp));
        pStorage->_animationClocks.Add(clock);
    }

    clock->GetTimeline()->TimerInvalidated += EventHandler(pStorage, &AnimationStorage::OnTimeInvalidated);

    if (clock->IsControllableRoot())
    {
        clock->GetTimeline()->RemoveRequested.Clear();
        clock->GetTimeline()->RemoveRequested += EventHandler(pStorage, &AnimationStorage::OnRemoveRequested);
    }
}

void AnimationStorage::OnStateInvalidated(Object* sender, EventArg* e)
{

}

static void GetCurrentPropertyValue(AnimationStorage* storage, DpObject* d, DpProperty* dp, PropMetadata* metadata, Object* baseValue, ObjectPtr& output)
{
    ObjectPtr defaultDestinationValue = baseValue;

    if (defaultDestinationValue == DpProperty::UnsetValue())
    {
        defaultDestinationValue = metadata->GetDefaultValue();
    }

    int num3 = storage->GetCount();
    bool flag = false;
    ObjectPtr obj4 = defaultDestinationValue;
    ObjectPtr defaultOriginValue = defaultDestinationValue;

    for (int j = 0; j < num3; j++)
    {
        AnimationClock* clock = storage->GetClock(j);

        if (clock->GetCurrentState() != Clock::ClockState::Stopped)
        {
            flag = true;
            defaultOriginValue = clock->GetTimerValue(defaultOriginValue.get(), obj4.get());
            if (defaultOriginValue == DpProperty::UnsetValue())
            {
                // throw;
            }
        }
    }

    if (flag)
    {
        defaultDestinationValue = defaultOriginValue;
    }

    output = defaultDestinationValue;
}

void AnimationStorage::OnTimeInvalidated(Object* sender, EventArg* e)
{
    if (!_dpObject.IsAlive())
    {
        return;
    }

    DpObject* target = _dpObject.GetElement<DpObject>();
    if (target != NULL)
    {
        Object* baseValue = NULL;
        EffectiveValue oriEntry;
        DpObject* d = target;
        EffectiveValue oldEntry;

        target->ref();

        MemberVisitor::GetRawEV(d, _dpProp, oldEntry);

        // 没有修改基值
        if (!oldEntry.HasRevise())
        {
            oriEntry = oldEntry;
            baseValue = MemberVisitor::GetValue(&oriEntry);
        }
        else
        {
            oriEntry = EffectiveValue();
            MemberVisitor::SetBaseValueType(&oriEntry, oldEntry.GetBaseValueType());
            baseValue = MemberVisitor::GetReviseValue(&oldEntry)->GetBaseValue();

            MemberVisitor::SetValue(&oriEntry, baseValue);

            if (oldEntry.IsExpression())
            {
                baseValue = MemberVisitor::GetReviseValue(&oldEntry)->GetExprValue();
                MemberVisitor::SetExprValue(&oriEntry, baseValue, MemberVisitor::GetValue(&oriEntry));
            }
        }

        EntryIndex eIndex;
        PropMetadata* metadata = NULL;
        ObjectPtr newVal = NULL;

        metadata = MemberVisitor::FindMetadata(d, eIndex, _dpProp);
        GetCurrentPropertyValue(this, d, _dpProp, metadata, baseValue, newVal);

        if (_dpProp->IsValidValueInternal(d, newVal.get()))
        {
            MemberVisitor::SetMetadata(&oldEntry, metadata);
            oriEntry.SetPropertyAndIndex(_dpProp, _dpProp->GetIndex());
            MemberVisitor::SetAnimatedValue(&oriEntry, newVal.get(), baseValue);
            MemberVisitor::UpdateEV(d, eIndex, _dpProp, oldEntry, oriEntry, false, false, OperationType::otUnknown);
        }

        target->unref();
    }
}

void AnimationStorage::OnRemoveRequested(Object* sender, EventArg* e)
{
    AnimationClock* clock = RTTICast<AnimationClock>(sender);
    int index = IndexOf(clock);

    if (index != -1)
    {
        _animationClocks.RemoveAt(index);
        DetachAnimationClock(clock);
        if (_animationClocks.GetCount() == 0)
        {
            ClearAnimations(_dpObject.GetElement<DpObject>(), _dpProp);
        }
        clock->unref();
    }
}

void AnimationStorage::BeginAnimation(DpObject* d, DpProperty* dp, AnimationTimeline* animation)
{
    //
    // 这里cache动画对象，保证对象能够得到释放
    //
    ObjectPtr cacheAni = animation;
    
    if (animation == NULL)
    {
        // 清除动画
        ClearAnimations(d, dp);
    }
    else
    {
        //
        // 这里返回的clock引用计数已经加1了
        // 做完后需要调用unref释放引用计数
        //
        AnimationClock* clock = animation->CreateClock(true);

        if (NULL != clock)
        {
            ApplyAnimationClock(d, dp, clock);
            clock->unref();
        }
    }
}

}
