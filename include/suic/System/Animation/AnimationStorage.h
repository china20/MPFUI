// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����AnimationStorage.h
// ��  �ܣ��洢������������ĳ�������������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
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

    // �ɿؼ�����洢�������˿ؼ��ϵ����ж���
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

    // �ؼ�����
    WeakRef _dpObject;
    Object* _baseValue;
    // ��������
    DpProperty* _dpProp;

    Array<AnimationClock*> _animationClocks;
};

}

#endif
