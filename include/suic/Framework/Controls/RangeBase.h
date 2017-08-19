// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����RangeBase.h
// ��  �ܣ������������������ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
// 
// ============================================================================


#ifndef _UIRANGEBASE_H_
#define _UIRANGEBASE_H_

#include <Framework/Controls/RoutedPropertyChangedEventArg.h>

namespace suic
{

class SUICORE_API RangeBase : public Control
{
public:

    static DpProperty* LargeChangeProperty;
    static DpProperty* MaximumProperty;
    static DpProperty* MinimumProperty;
    static DpProperty* SmallChangeProperty;
    static DpProperty* ValueProperty;

    static RoutedEvent* ValueChangedEvent;

    static void OnValuePropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMinimumPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMaximumPropChanged(DpObject* d, DpPropChangedEventArg* e);

    static bool StaticInit();

    RangeBase();
    virtual ~RangeBase();

    RTTIOfAbstractClass(RangeBase)

    void AddValueChanged(FloatPropChangedEventHandler* handler);
    void RemoveValueChanged(FloatPropChangedEventHandler& handler);

// ���Է��ʶ���
public:

    /// <summary>
    ///     �仯���ֵ
    /// </summary>
    Float GetSmallChange();
    void SetSmallChange(Float val);

    Float GetLargeChange();
    void SetLargeChange(Float val);

    Float GetMinimum();
    void SetMinimum(Float val);

    Float GetMaximum();
    void SetMaximum(Float val);

    Float GetValue();
    void SetValue(Float val);

protected:

    virtual void OnMaximumChanged(Float oldMaximum, Float newMaximum);
    virtual void OnMinimumChanged(Float oldMinimum, Float newMinimum);
    virtual void OnValueChanged(Float oldValue, Float newValue);
};

}

#endif
