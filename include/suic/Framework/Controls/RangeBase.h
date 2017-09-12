// 华勤科技版权所有 2008-2022
// 
// 文件名：RangeBase.h
// 功  能：工具栏、滚动条基类。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
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

    static void StaticInit();

    RangeBase();
    virtual ~RangeBase();

    RTTIOfAbstractClass(RangeBase)

    void AddValueChanged(FloatPropChangedEventHandler* handler);
    void RemoveValueChanged(FloatPropChangedEventHandler& handler);

// 属性访问定义
public:

    /// <summary>
    ///     变化最大值
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
