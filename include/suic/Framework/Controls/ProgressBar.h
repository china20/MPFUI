// 华勤科技版权所有 2010-2020
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
//
// MPF界面引擎采用业界成熟的体系架构，其属性系统、事件系统以
// 及模版在目前c++界面库是一大特点，真正做到样式和代码分离，底层
// 更采用了SSE2指令进行加速，不论在普通窗口还是半透明窗口下性能都
// 非常优秀，完善、灵活的动画接口，完全可以满足各种商业使用。
// 
// 如果需购买技术支持服务或咨询请联系
// china0851@foxmail.com；QQ:386666951。
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
