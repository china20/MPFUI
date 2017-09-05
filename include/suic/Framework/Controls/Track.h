// 华勤科技版权所有 2008-2022
// 
// 文件名：Track.h
// 功  能：页范围逻辑。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UITRACK_H_
#define _UITRACK_H_

#include <Framework/Controls/Thumb.h>
#include <Framework/Controls/RepeatButton.h>

namespace suic
{

/// <summary>
/// 实现Track控件
/// </summary>
class SUICORE_API Track : public FrameworkElement
{
public:

    static DpProperty* ValueProperty;
    static DpProperty* MaximumProperty;
    static DpProperty* MinimumProperty;

    static DpProperty* OrientationProperty;
    static DpProperty* ViewportSizeProperty;

    static DpProperty* IsDirectionReversedProperty;

    static bool StaticInit();

    Track();
    virtual ~Track();

    RTTIOfClass(Track)

    int GetMaximum();
    void SetMaximum(int val);

    int GetMinimum();
    void SetMinimum(int val);

    int GetValue();
    void SetValue(int val);

    int GetThumbCenterOffset();
    void SetThumbCenterOffset(int val);

    int GetViewportSize();
    void SetViewportSize(int val);

    int GetOrientation();
    void SetOrientation(int val);

    bool IsDirectionReversed();
    void SetIsDirectionReversed(bool val);
    
    Thumb* GetThumb();
    void SetThumb(Thumb* val);

    RepeatButton* GetDecreaseButton() const;
    void SetDecreaseButton(RepeatButton* val);

    RepeatButton* GetIncreaseButton() const;
    void SetIncreaseButton(RepeatButton* val);

public:

    virtual int GetValueFromPoint(Point pt);
    virtual int GetValueFromDistance(int horizontal, int vertical);

protected:

    void AddChild(Object* obj);
    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

    void OnPreApplyTemplate();

    bool OnReadingChild(IXamlNode* node);

    void OnArrange(const Size& arrangeSize);
    Size OnMeasure(const Size& constraintSize);

private:

    void UpdateComponent(Element* oldValue, Element* newValue);
    void BindToTemplatedParent(DpProperty* target, DpProperty* source);
    void BindChildToTemplatedParent(FrameworkElement* element, DpProperty* target, DpProperty* source);

    void ComputeSliderLengths(Size arrangeSize, bool isVertical, int& decreaseButtonLength, int& thumbLength, int& increaseButtonLength);
    bool ComputeScrollBarLengths(Size arrangeSize, bool isVertical, int viewportSize, int& decreaseButtonLength, int& thumbLength, int& increaseButtonLength);

protected:

    Thumb* _thumb;

    RepeatButton* _decreaseButton;
    RepeatButton* _increaseButton;

    Float _density;
    int _thumbCenterOffset;

    Element* _visualChildren[3];
};

inline int Track::GetMaximum()
{
    return DpObject::GetValue(MaximumProperty)->ToInt();
}

inline void Track::SetMaximum(int val)
{
    if ((int)val != (int)GetMaximum())
    {
        DpObject::SetValue(MaximumProperty, new OFloat(val));
    }
}

inline int Track::GetMinimum()
{
    return DpObject::GetValue(MinimumProperty)->ToInt();
}

inline void Track::SetMinimum(int val)
{
    DpObject::SetValue(MinimumProperty, new Integer(val));
}

inline int Track::GetValue()
{
    return DpObject::GetValue(ValueProperty)->ToInt();
}

inline void Track::SetValue(int val)
{
    if ((int)val != (int)GetValue())
    {
        DpObject::SetValue(ValueProperty, new Integer(val));
    }
}

inline int Track::GetThumbCenterOffset()
{
    return _thumbCenterOffset;
}

inline void Track::SetThumbCenterOffset(int val)
{
    _thumbCenterOffset = val;
}

inline int Track::GetViewportSize()
{
    return DpObject::GetValue(ViewportSizeProperty)->ToInt();
}

inline void Track::SetViewportSize(int val)
{
    DpObject::SetValue(ViewportSizeProperty, new Integer(val));
}

inline int Track::GetOrientation()
{
    return DpObject::GetValue(OrientationProperty)->ToInt();
}

inline void Track::SetOrientation(int val)
{
    if (val == Orientation::Horizontal)
    {
        DpObject::SetValue(OrientationProperty, OrientationBox::HorizontalBox);
    }
    else
    {
        DpObject::SetValue(OrientationProperty, OrientationBox::VerticalBox);
    }
}

inline bool Track::IsDirectionReversed()
{
    return DpObject::GetValue(IsDirectionReversedProperty)->ToInt() != 0;
}

inline void Track::SetIsDirectionReversed(bool val)
{
    DpObject::SetValue(IsDirectionReversedProperty, BOOLTOBOOLEAN(val));
}

inline Thumb* Track::GetThumb()
{
    return _thumb;
}

}

#endif
