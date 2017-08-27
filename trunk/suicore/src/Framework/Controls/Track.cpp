// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Track.cpp

#include <Framework/Controls/Track.h>
#include <Framework/Controls/Slider.h>
#include <Framework/Controls/RangeBase.h>
#include <Framework/Controls/ScrollBar.h>

#include <System/Windows/VisualStates.h>
#include <System/Tools/EventHelper.h>
#include <System/Tools/ResourceParser.h>
#include <System/Types/Numeric.h>

#include <math.h>

namespace suic
{

/////////////////////////////////////////////////////////////////////////
// Track

ImplementRTTIOfClass(Track, FrameworkElement)

DpProperty* Track::ValueProperty;
DpProperty* Track::MaximumProperty;
DpProperty* Track::MinimumProperty;

DpProperty* Track::OrientationProperty;
DpProperty* Track::ViewportSizeProperty;

DpProperty* Track::IsDirectionReversedProperty;

Track::Track()
    : _density(0)
    , _thumb(NULL)
    , _thumbCenterOffset(0)
    , _decreaseButton(NULL)
    , _increaseButton(NULL)
{
    for (int i = 0; i < 3; ++i)
    {
        _visualChildren[i] = NULL;
    }
}

Track::~Track()
{
    if (_thumb)
    {
        _thumb->unref();
    }
    if (_decreaseButton)
    {
        _decreaseButton->unref();
    }
    if (_increaseButton)
    {
        _increaseButton->unref();
    }
}

bool Track::StaticInit()
{
    if (ValueProperty == NULL)
    {
        RangeBase::StaticInit();

        ValueProperty = RangeBase::ValueProperty->AddOwner(RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsArrange));
        MaximumProperty = RangeBase::MaximumProperty->AddOwner(RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(1), PropMetadataOptions::AffectsArrange));
        MinimumProperty = RangeBase::MinimumProperty->AddOwner(RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsArrange));

        ViewportSizeProperty = DpProperty::Register(_T("ViewportSize"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(-1), PropMetadataOptions::AffectsArrange));

        OrientationProperty = DpProperty::Register(_T("Orientation"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(OrientationBox::HorizontalBox, PropMetadataOptions::AffectsArrange));
        OrientationProperty->SetConvertValueCb(OrientationConvert::Convert);

        IsDirectionReversedProperty = DpProperty::Register(_T("IsDirectionReversed"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsArrange));
    }
    return true;
}

static void CoerceLength(int& componentLength, int trackLength)
{
    if (componentLength < 0)
    {
        componentLength = 0;
    }
    else if (componentLength > trackLength)
    {
        componentLength = trackLength;
    }
}

int Track::GetValueFromDistance(int horizontal, int vertical)
{
    int num = IsDirectionReversed() ? ((int) (-1)) : ((int) 1);

    if (GetOrientation() == Orientation::Horizontal)
    {
        return (int)((num * horizontal) * _density + 0.5);
    }
    else
    {
        return (int)(((-1.0 * num) * vertical) * _density + 0.5);
    }
}

int Track::GetValueFromPoint(Point pt)
{
    int num = 0;

    if (GetOrientation() == Orientation::Horizontal)
    {
        num = (int)(GetValue() + GetValueFromDistance(pt.x - _thumbCenterOffset, pt.y - (GetRenderSize().Height() * 0.5)));
    }
    else
    {
        num = (int)(GetValue() + GetValueFromDistance(pt.x - (GetRenderSize().Width() * 0.5), pt.y - _thumbCenterOffset));
    }

    return max(GetMinimum(), min(GetMaximum(), num));
}

void Track::UpdateComponent(Element* oldValue, Element* newValue)
{
    if (oldValue != newValue)
    {
        if (oldValue != NULL)
        {
            RemoveVisualChild(oldValue);
        }

        int index = 0;

        while (index < 3)
        {
            if (_visualChildren[index] == NULL)
            {
                break;
            }
            if (_visualChildren[index] == oldValue)
            {
                while ((index < 2) && (_visualChildren[index + 1] != NULL))
                {
                    _visualChildren[index] = _visualChildren[index + 1];
                    index++;
                }
            }
            else
            {
                index++;
            }
        }

        _visualChildren[index] = newValue;
        AddVisualChild(newValue);

        if (IsInitialized())
        {
            newValue->BeginInit();
            newValue->EndInit();
        }

        InvalidateMeasure();
        InvalidateArrange();
    }
}

void Track::AddChild(Object* obj)
{

}

int Track::GetVisualChildrenCount()
{
    int count = 0;

    if (NULL != _decreaseButton)
    {
        ++count;
    }

    if (NULL != _increaseButton)
    {
        ++count;
    }

    if (NULL != _thumb)
    {
        ++count;
    }

    return count;
}

Element* Track::GetVisualChild(int index)
{
    return _visualChildren[index];
}

bool Track::OnReadingChild(IXamlNode* node)
{
    String name(node->GetName());

    if (node->HasNext())
    {
        IXamlNode* child = node->Current();

        if (name.Equals(_U("Track.DecreaseRepeatButton")))
        {
            ObjectPtr elem;
            ResourceParserOp::CreateElement(child->GetName(), elem);
            _decreaseButton = RTTICast<RepeatButton>(elem.get());

            if (_decreaseButton)
            {
                _decreaseButton->ref();
            }

            UpdateComponent(NULL, _decreaseButton);
            return true;
        }
        else if (name.Equals(_U("Track.IncreaseRepeatButton")))
        {
            ObjectPtr elem;
            ResourceParserOp::CreateElement(child->GetName(), elem);
            _increaseButton = RTTICast<RepeatButton>(elem.get());

            if (_increaseButton)
            {
                _increaseButton->ref();
            }

            UpdateComponent(NULL, _increaseButton);

            return true;
        }
        else if (name.Equals(_U("Track.Thumb")))
        {
            ObjectPtr elem;
            ResourceParserOp::CreateElement(child->GetName(), elem);
            _thumb = RTTICast<Thumb>(elem.get());

            if (_thumb)
            {
                _thumb->ref();
            }
            UpdateComponent(NULL, _thumb);
            return true;
        }
    }

    return false;
}

void Track::BindToTemplatedParent(DpProperty* target, DpProperty* source)
{
    if (!HasNonDefaultValue(target))
    {
        Binding* binding = new Binding();
        binding->SetSourceRef(new RelativeSource(RelativeSourceMode::TemplatedParent));
        binding->SetPath(PropertyPath(source->GetName()));
        SetBinding(target, binding);
    }
}

void Track::BindChildToTemplatedParent(FrameworkElement* elem, DpProperty* target, DpProperty* source)
{
    if ((elem != NULL) && !elem->HasNonDefaultValue(target))
    {
        Binding* binding = new Binding();
        binding->SetSource(GetTemplatedParent());
        binding->SetPath(PropertyPath(source->GetName()));
        elem->SetBinding(target, binding);
    }
}

void Track::OnPreApplyTemplate()
{
    FrameworkElement::OnPreApplyTemplate();
    RangeBase* templatedParent = RTTICast<RangeBase>(GetTemplatedParent());

    if (templatedParent != NULL)
    {
        BindToTemplatedParent(MinimumProperty, RangeBase::MinimumProperty);
        BindToTemplatedParent(MaximumProperty, RangeBase::MaximumProperty);
        BindToTemplatedParent(ValueProperty, RangeBase::ValueProperty);

        if (templatedParent->GetRTTIType() ==  ScrollBar::RTTIType())
        {
            BindToTemplatedParent(ViewportSizeProperty, ScrollBar::ViewportSizeProperty);
            BindToTemplatedParent(OrientationProperty, ScrollBar::OrientationProperty);
        }
        else if (templatedParent->GetRTTIType() ==  Slider::RTTIType())
        {
            BindToTemplatedParent(OrientationProperty, Slider::OrientationProperty);
            BindToTemplatedParent(IsDirectionReversedProperty, Slider::IsDirectionReversedProperty);
            BindChildToTemplatedParent(GetDecreaseButton(), RepeatButton::DelayProperty, Slider::DelayProperty);
            BindChildToTemplatedParent(GetDecreaseButton(), RepeatButton::IntervalProperty, Slider::IntervalProperty);
            BindChildToTemplatedParent(GetIncreaseButton(), RepeatButton::DelayProperty, Slider::DelayProperty);
            BindChildToTemplatedParent(GetIncreaseButton(), RepeatButton::IntervalProperty, Slider::IntervalProperty);
        }
    }
}

void Track::OnArrange(const Size& arrangeSize)
{
    // 上边页大小
    int num = 0;
    // 滑块大小
    int num2 = 0;
    // 下边页大小
    int num3 = 0;

    int iOrien = GetOrientation();
    bool isVertical = iOrien == Orientation::Vertical;
    int iViewSize = GetViewportSize();
    int d = max(0, iViewSize);
    DpObject* fe = GetTemplatedParent();

    if (iViewSize < 0)
    {
        ComputeSliderLengths(arrangeSize, isVertical, num, num2, num3);
    }
    else if (!ComputeScrollBarLengths(arrangeSize, d, isVertical, num, num2, num3))
    {
        return;
    }

    int dx = 0;
    int dy = 0;
    Size size = arrangeSize;
    bool isDirectionReversed = IsDirectionReversed();

    if (isVertical)
    {
        CoerceLength(num, arrangeSize.Height());
        CoerceLength(num3, arrangeSize.Height());
        CoerceLength(num2, arrangeSize.Height());
        dy = isDirectionReversed ? (num + num2) : 0;
        size.cy = num3;

        if (_increaseButton != NULL)
        {
            _increaseButton->Arrange(Rect(dx, dy, size.Width(), size.Height()));
        }

        dy = isDirectionReversed ? 0.0 : (num3 + num2);
        size.cy = num;

        if (_decreaseButton != NULL)
        {
            _decreaseButton->Arrange(Rect(dx, dy, size.Width(), size.Height()));
        }

        dy = isDirectionReversed ? num : num3;
        size.cy = num2;

        if (_thumb != NULL)
        {
            _thumb->Arrange(Rect(dx, dy, size.Width(), size.Height()));
        }
        _thumbCenterOffset = dy + (num2 * 0.5);
        return ;
    }

    CoerceLength(num, arrangeSize.Width());
    CoerceLength(num3, arrangeSize.Width());
    CoerceLength(num2, arrangeSize.Width());
    dx = isDirectionReversed ? (num3 + num2) : 0;
    size.cx = num;

    if (_decreaseButton != NULL)
    {
        _decreaseButton->Arrange(Rect(dx, dy, size.Width(), size.Height()));
    }

    dx = isDirectionReversed ? 0 : (num + num2);
    size.cx = num3;

    if (_increaseButton != NULL)
    {
        _increaseButton->Arrange(Rect(dx, dy, size.Width(), size.Height()));
    }

    dx = isDirectionReversed ? num3 : num;
    size.cx = num2;

    if (_thumb != NULL)
    {
        _thumb->Arrange(Rect(dx, dy, size.Width(), size.Height()));
    }

    _thumbCenterOffset = dx + (num2 * 0.5);
}

Size Track::OnMeasure(const Size& constraintSize)
{
    Size desiredSize;

    if (GetThumb() != NULL)
    {
        GetThumb()->Measure(constraintSize);
        desiredSize = GetThumb()->GetDesiredSize();
    }

    if (GetViewportSize() > 0)
    {
        if (GetOrientation() == Orientation::Vertical)
        {
            desiredSize.cy = 0;
        }
        else
        {
            desiredSize.cx = 0;
        }
    }

    return desiredSize;
}

void Track::ComputeSliderLengths(Size arrangeSize, bool isVertical, int& decreaseButtonLength, int& thumbLength, int& increaseButtonLength)
{
    int height = 0;
    int minimum = (int)GetMinimum();
    int num2 = max(0, (int) (GetMaximum() - minimum));
    int num3 = min(num2, GetValue() - minimum);

    if (isVertical)
    {
        height = arrangeSize.Height();
        thumbLength = (GetThumb() == NULL) ? 0 : GetThumb()->GetDesiredSize().Height();
    }
    else
    {
        height = arrangeSize.Width();
        thumbLength = (GetThumb() == NULL) ? 0 : GetThumb()->GetDesiredSize().Width();
    }

    CoerceLength(thumbLength, height);
    int trackLength = height - thumbLength;
    decreaseButtonLength = (int)((Float)(trackLength * num3) / (Float)num2);
    CoerceLength(decreaseButtonLength, trackLength);
    increaseButtonLength = trackLength - decreaseButtonLength;
    CoerceLength(increaseButtonLength, trackLength);

    _density = (Float)num2 / (Float)trackLength;
}

bool Track::ComputeScrollBarLengths(Size arrangeSize, int viewportSize, bool isVertical, int& decreaseButtonLength, int& thumbLength, int& increaseButtonLength)
{
    int height = 0;
    int num6 = 0;
    int minimum = GetMinimum();
    // 滚动的逻辑大小
    // GetMaximum()表示可以滚动的逻辑高度（总的逻辑高度减掉可视高度）
    int num2 = max(0, (int)(GetMaximum() - minimum));
    // 当前跟踪的逻辑位置
    int num3 = min(num2, (int)(GetValue() - minimum));
    // 计算总的逻辑高度（加上viewportSize可视高度）
    int num4 = max(0, num2) + viewportSize;

    if (num4 == 0)
    {
        return false;
    }

    if (isVertical)
    {
        height = arrangeSize.Height();
        num6 = 16;

        if (num6 < _thumb->GetDesiredSize().Height())
        {
            num6 = _thumb->GetDesiredSize().Height();
        }
    }
    else
    {
        height = arrangeSize.Width();
        num6 = 16;

        if (num6 < _thumb->GetDesiredSize().Width())
        {
            num6 = _thumb->GetDesiredSize().Width();
        }
    }

    // 计算滑块的大小
    // 这里height相当于page（逻辑高度一页的大小）
    // height是Track的尺寸，viewportSize是滚动区尺寸
    thumbLength = (int)((Float) (height * viewportSize) / (Float) num4);
    // 校正大小，不能大于布局大小
    CoerceLength(thumbLength, height);
    // 滑块最小值不能小于默认滑块大小的一半
    thumbLength = max(num6, thumbLength);

    // 滑块无效，隐藏
    bool flag = num2 <= 0;
    bool flag2 = thumbLength > height;

    if (flag || flag2)
    {
        _thumbCenterOffset = 0;
        _density = 0;
        decreaseButtonLength = 0;
        increaseButtonLength = 0;
        return false;
    }

    // 上下页总大小（可视大小减掉滑块大小）
    int trackLength = height - (int)thumbLength;
    // 最上边滑块大小
    decreaseButtonLength = (int)((Float) (trackLength * num3) / (Float) num2);
    CoerceLength(decreaseButtonLength, trackLength);
    // 最下边滑块大小
    increaseButtonLength = trackLength - decreaseButtonLength;
    CoerceLength(increaseButtonLength, trackLength);

    _density = (Float)num2 / (Float)trackLength;

    return true;
}

void Track::SetThumb(Thumb* val)
{
    UpdateComponent(_thumb, val);
    SETREFOBJ(_thumb, val);
}

void Track::SetDecreaseButton(RepeatButton* val)
{
    UpdateComponent(_decreaseButton, val);
    SETREFOBJ(_decreaseButton, val);
}

void Track::SetIncreaseButton(RepeatButton* val)
{
    UpdateComponent(_increaseButton, val);
    SETREFOBJ(_increaseButton, val);
}

RepeatButton* Track::GetDecreaseButton() const
{
    return _decreaseButton;
}

RepeatButton* Track::GetIncreaseButton() const
{
    return _increaseButton;
}

}
