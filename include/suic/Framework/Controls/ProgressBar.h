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

#ifndef _UIPROGRESSBAR_H_
#define _UIPROGRESSBAR_H_

#include <Framework/Controls/RangeBase.h>

namespace suic
{

/// <summary>
/// ProgressBar
/// </summary>
class SUICORE_API ProgressBar : public RangeBase
{
public:

    static DpProperty* IsIndeterminateProperty;
    static DpProperty* OrientationProperty;

    static bool StaticInit();

    ProgressBar();
    virtual ~ProgressBar();

    String ToString() const;
    bool IsIndeterminate();

    Orientation GetOrientation();
    void SetOrientation(Orientation val);

    RTTIOfClass(ProgressBar)

public:

    void OnApplyTemplate();

    void OnTrackSizeChanged(DpObject* sender, RoutedEventArg* e);

    void OnValueChanged(Float oldValue, Float newValue);
    void OnMaximumChanged(Float oldMaximum, Float newMaximum);
    void OnMinimumChanged(Float oldMinimum, Float newMinimum);

protected:

    void UpdateAnimation();
    void SetProgressBarIndicatorLength();
    void SetProgressBarGlowElementBrush();

protected:

    FrameworkElement* _glow;
    FrameworkElement* _track;
    FrameworkElement* _indicator;
};

}

#endif
