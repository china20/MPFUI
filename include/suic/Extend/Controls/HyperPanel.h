// 华勤科技版权所有 2010-2011
// 
// 文件名：HyperPanel.h
// 功  能：换行布局控件。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UIHYPERPANEL_H_
#define _UIHYPERPANEL_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/Panel.h>

namespace suic
{

class SUIWGX_API HyperPanel : public suic::Panel
{
public:

    static suic::DpProperty* CenterProperty;
    static suic::DpProperty* DistributionProperty;
    static suic::DpProperty* OrientationProperty;
    static suic::DpProperty* ApplyOpacityProperty;
    static suic::DpProperty* MinElementScaleProperty;
    static suic::DpProperty* VerticalContentAlignmentProperty;
    static suic::DpProperty* HorizontalContentAlignmentProperty;

    static void OnOrientationPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnCenterPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnDistributionPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static void OnApplyOpacityPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnMinElementScalePropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnHorizontalContentAlignmentPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e); 
    static void OnVerticalContentAlignmentPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static bool StaticInit();

    HyperPanel();
    ~HyperPanel();

    RTTIOfClass(HyperPanel)

    suic::Orientation GetOrientation();
    void SetOrientation(suic::Orientation val);

    bool GetApplyOpacity();
    void SetApplyOpacity(bool val);

    suic::Float GetCenter();
    void SetCenter(suic::Float val);

    suic::Float GetDistribution();
    void SetDistribution(suic::Float val);

    suic::Float GetMinElementScale();
    void SetMinElementScale(suic::Float val);

    int GetVerticalContentAlignment();
    void SetVerticalContentAlignment(int val);

    int GetHorizontalContentAlignment();
    void SetHorizontalContentAlignment(int val);

protected:

    suic::Size OnMeasure(const suic::Size& size);
    void OnArrange(const suic::Size& arrangeSize);
    void OnMouseMove(suic::MouseButtonEventArg* e);

    void OnApplyOpacityChanged(bool oldValue);
    void OnCenterChanged(suic::Float oldValue);
    void OnDistributionChanged(suic::Float oldValue);
    void OnMinElementScaleChanged(suic::Float oldValue);

    suic::Size GetScaledSize(suic::Element* elem);
    void UpdateTransforms(suic::Size availableSize);

private:

    suic::Float _offsetX;
    suic::Float _offsetY;
    suic::Int32 _totalWidth;
    suic::Int32 _totalHeight;
    suic::Float _scaledChildSize;
};

}

#endif
