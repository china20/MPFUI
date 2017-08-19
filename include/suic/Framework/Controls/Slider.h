// ���ڿƼ���Ȩ���� 2010-2020
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
//
// SHARPUI�����������ҵ��������ϵ�ܹ���������ϵͳ���¼�ϵͳ��
// ��ģ����Ŀǰc++�������һ���ص㣬����������ʽ�ʹ�����룬�ײ�
// ��������SSE2ָ����м��٣���������ͨ���ڻ��ǰ�͸�����������ܶ�
// �ǳ����㣬���ơ����Ķ����ӿڣ���ȫ�������������ҵʹ�á�
// 
// ����蹺����֧�ַ������ѯ����ϵ
// china0851@foxmail.com��QQ:386666951��
// ====================================================================

#ifndef _UISLIDER_H_
#define _UISLIDER_H_

#include <Framework/Controls/Track.h>
#include <Framework/Controls/Thumb.h>
#include <Framework/Controls/ToolTip.h>
#include <Framework/Controls/RangeBase.h>

namespace suic
{

class SUICORE_API Slider : public RangeBase
{
public:

    static DpProperty* OrientationProperty;
    static DpProperty* AutoToolTipPlacementProperty;
    static DpProperty* AutoToolTipPrecisionProperty;
    static DpProperty* DelayProperty;
    static DpProperty* IntervalProperty;
    static DpProperty* IsDirectionReversedProperty;
    static DpProperty* IsMoveToPointEnabledProperty;
    static DpProperty* IsSelectionRangeEnabledProperty;
    static DpProperty* IsSnapToTickEnabledProperty;

    static DpProperty* SelectionEndProperty;
    static DpProperty* SelectionStartProperty;

    static DpProperty* TickFrequencyProperty;
    static DpProperty* TickPlacementProperty;
    static DpProperty* TicksProperty;

    static Object* CoerceSelectionEnd(DpObject* d, Object* val);
    static Object* CoerceSelectionStart(DpObject* d, Object* val);

    static void OnSelectionEndChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnSelectionStartChanged(DpObject* d, DpPropChangedEventArg* e);

    static void OnThumbDragCompletedEvent(DpObject* sender, RoutedEventArg* e);
    static void OnThumbDragDeltaEvent(DpObject* sender, RoutedEventArg* e);
    static void OnThumbDragStartedEvent(DpObject* sender, RoutedEventArg* e);

    static void OnMouseLeftButtonDownThunk(Element* sender, MouseButtonEventArg* e);

    static bool StaticInit();

    Slider();
    virtual ~Slider();

    Track* GetTrack() const;

    RTTIOfClass(Slider)

    Orientation GetOrientation();
    void SetOrientation(Orientation val);

    int GetInterval();
    void SetInterval(int val);

    bool IsDirectionReversed();
    void SetIsDirectionReversed(bool val);

    bool IsMoveToPointEnabled();
    void SetIsMoveToPointEnabled(bool val);

    bool IsSnapToTickEnabled();
    void SetIsSnapToTickEnabled(bool val);

    bool IsSelectionRangeEnabled();
    void SetIsSelectionRangeEnabled(bool val);

    Float GetSelectionStart();
    void SetSelectionStart(Float val);

    Float GetSelectionEnd();
    void SetSelectionEnd(Float val);

    Float GetTickFrequency();
    void SetTickFrequency(Float val);

    IntAutoArray* GetTicks();
    void SetTicks(IntAutoArray* val);

    int GetTickPlacement();
    void SetTickPlacement(int val);

public:

    void OnApplyTemplate();

    void OnArrange(const Size& arrangeSize);

    void OnMaximizeValue();
    void OnMaximumChanged(Float oldMaximum, Float newMaximum);
    void OnMinimizeValue();
    void OnMinimumChanged(Float oldMinimum, Float newMinimum);

    void OnValueChanged(Float oldValue, Float newValue);

    void OnThumbDragCompleted(DragCompletedEventArg* e);
    void OnThumbDragDelta(DragDeltaEventArg* e);
    void OnThumbDragStarted(DragStartedEventArg* e);

    void OnPreviewMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnKeyDown(KeyboardEventArg* e);

    void OnMouseEnter(MouseButtonEventArg* e);
    void OnMouseLeave(MouseButtonEventArg* e);

    virtual void OnDecreaseLarge();
    virtual void OnDecreaseSmall();

protected:

    void UpdateValue(int value);
    Float SnapToTick(Float value);
    void MoveToNextTick(Float direction);
    void UpdateSelectionRangeElementPositionAndSize();

protected:
    
    Track* _track;
    bool _hideThumb;
    int _previousValue;
    ToolTip* _autoToolTip;
    FrameworkElement* _selectionRangeElement;
};

}

#endif
