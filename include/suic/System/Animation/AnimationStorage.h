// 华勤科技版权所有 2008-2022
// 
// 文件名：AnimationStorage.h
// 功  能：存储动画，仅仅和某个依赖属性相关
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIANIMATIONSTORAGE_H_
#define _UIANIMATIONSTORAGE_H_

#include <System/Windows/FrameworkElement.h>
#include <System/Animation/TimerManager.h>

namespace suic
{

class AnimationStorage;
class AnimationStorageDic : public ValueAutoDic<int, AnimationStorage>
                          , public Object
{
public:

    AnimationStorageDic();
    ~AnimationStorageDic();
};

class AnimationStorage : public DpObject
{
public:

    RTTIOfClass(AnimationStorage)

    AnimationStorage();
    ~AnimationStorage();

    // 由控件对象存储关联到此控件上的所有动画
    static UncommonField<AnimationStorageDic> AnimatedPropMapField;

    static AnimationStorage* GetStorage(DpObject* d, DpProperty* dp);
    static AnimationStorage* CreateStorage(DpObject* d, DpProperty* dp);
    static void BeginAnimation(DpObject* d, DpProperty* dp, AnimationTimeline* animation);

public:

    int GetCount() const;
    int IndexOf(AnimationClock* clock);
    AnimationClock* GetClock(int index);

    void OnStateInvalidated(Object* sender, EventArg* e);
    void OnTimeInvalidated(Object* sender, EventArg* e);
    void OnRemoveRequested(Object* sender, EventArg* e);

protected:

    void ClearAnimations();
    void DetachAnimationClock(AnimationClock* clock);
    void Initialize(DpObject* d, DpProperty* dp);

    static void ClearAnimations(DpObject* d, DpProperty* dp);
    static void ApplyAnimationClock(DpObject* d, DpProperty* dp, AnimationClock* animationClock);

private:

    // 控件对象
    WeakRef _dpObject;
    Object* _baseValue;
    // 依赖属性
    DpProperty* _dpProp;

    Array<AnimationClock*> _animationClocks;
};

}

#endif
