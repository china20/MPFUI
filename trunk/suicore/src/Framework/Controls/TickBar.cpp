// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// TickBar.cpp

#include <Framework/Controls/TickBar.h>
#include <Framework/Controls/Slider.h>
#include <System/Tools/Debug.h>
#include <Skia/effects/SkDashPathEffect.h>

namespace suic
{

ImplementRTTIOfClass(TickBar, FrameworkElement)

DpProperty* TickBar::FillProperty;
DpProperty* TickBar::IsDirectionReversedProperty;
DpProperty* TickBar::IsSelectionRangeEnabledProperty;
DpProperty* TickBar::MaximumProperty;
DpProperty* TickBar::MinimumProperty;

DpProperty* TickBar::PlacementProperty;
DpProperty* TickBar::ReservedSpaceProperty;
DpProperty* TickBar::SelectionEndProperty;
DpProperty* TickBar::SelectionStartProperty;

DpProperty* TickBar::TicksProperty;
DpProperty* TickBar::TickFrequencyProperty;

TickBar::TickBar()
{

}

TickBar::~TickBar()
{

}

void TickBar::StaticInit()
{
    if (FillProperty == NULL)
    {
        FrameworkElement::StaticInit();

        FillProperty = DpProperty::Register(_U("Fill"), RTTIType(), Brush::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender));
        IsDirectionReversedProperty = DpProperty::Register(_U("IsDirectionReversed"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));
        IsSelectionRangeEnabledProperty = DpProperty::Register(_U("IsSelectionRangeEnabled"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));
        MaximumProperty = DpProperty::Register(_U("Maximum"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(new Integer(0), PropMetadataOptions::AffectsArrange));
        MinimumProperty = DpProperty::Register(_U("Minimum"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(new Integer(0), PropMetadataOptions::AffectsArrange));

        PlacementProperty = DpProperty::Register(_U("Placement"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(new Integer(1), PropMetadataOptions::AffectsRender));
        ReservedSpaceProperty = DpProperty::Register(_U("ReservedSpace"), RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(1), PropMetadataOptions::AffectsArrange));
        SelectionEndProperty = DpProperty::Register(_U("SelectionEnd"), RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(1), PropMetadataOptions::AffectsRender));
        SelectionStartProperty = DpProperty::Register(_U("SelectionStart"), RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(1), PropMetadataOptions::AffectsRender));

        TicksProperty = DpProperty::Register(_U("Ticks"), RTTIType(), IntAutoArray::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsArrange));
        TickFrequencyProperty = DpProperty::Register(_U("TickFrequency"), RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(1), PropMetadataOptions::AffectsArrange));

        TicksProperty->SetConvertValueCb(IntAutoArrayConvert::Convert);
    }
}

bool TickBar::IsDirectionReversed()
{
    return GetValue(IsDirectionReversedProperty)->ToBool();
}

void TickBar::SetIsDirectionReversed(bool val)
{
    SetValue(IsDirectionReversedProperty, BOOLTOBOOLEAN(val));
}

bool TickBar::IsSelectionRangeEnabled()
{
    return GetValue(IsSelectionRangeEnabledProperty)->ToBool();
}

void TickBar::SetIsSelectionRangeEnabled(bool val)
{
    SetValue(IsSelectionRangeEnabledProperty, BOOLTOBOOLEAN(val));
}

int TickBar::GetMinimum()
{
    return GetValue(MinimumProperty)->ToInt();
}

void TickBar::SetMinimum(int val)
{
    SetValue(MinimumProperty, new Integer(val));
}

int TickBar::GetMaximum()
{
    return GetValue(MaximumProperty)->ToInt();
}
void TickBar::SetMaximum(int val)
{
    SetValue(MaximumProperty, new Integer(val));
}

Float TickBar::GetReservedSpace()
{
    return GetValue(ReservedSpaceProperty)->ToFloat();
}

void TickBar::SetReservedSpace(Float val)
{
    SetValue(ReservedSpaceProperty, new OFloat(val));
}

Float TickBar::GetSelectionEnd()
{
    return GetValue(SelectionEndProperty)->ToFloat();
}
void TickBar::SetSelectionEnd(Float val)
{
    SetValue(SelectionEndProperty, new OFloat(val));
}

Float TickBar::GetSelectionStart()
{
    return GetValue(SelectionStartProperty)->ToFloat();
}

void TickBar::SetSelectionStart(Float val)
{
    SetValue(SelectionStartProperty, new OFloat(val));
}

Float TickBar::GetTickFrequency()
{
    return GetValue(TickFrequencyProperty)->ToFloat();
}
void TickBar::SetTickFrequency(Float val)
{
    SetValue(TickFrequencyProperty, new OFloat(val));
}

IntAutoArray* TickBar::GetTicks()
{
    Object* obj = GetValue(TicksProperty);
    if (obj->GetRTTIType() == IntAutoArray::RTTIType())
    {
        return (IntAutoArray*)(obj);
    }
    else
    {
        return NULL;
    }
}

void TickBar::SetTicks(IntAutoArray* val)
{
    SetValue(TicksProperty, val);
}

void TickBar::BindToTemplatedParent(DpProperty* target, DpProperty* source)
{
    if (!HasNonDefaultValue(target))
    {
        Binding* binding = new Binding();
        PropertyPath ppath(source->GetName());
        binding->SetSourceRef(new RelativeSource(RelativeSourceMode::TemplatedParent));
        binding->SetPath(ppath);
        SetBinding(target, binding);
    }
}

void TickBar::OnPreApplyTemplate()
{
    FrameworkElement::OnPreApplyTemplate();
    Slider* templatedParent = DynamicCast<Slider>(GetTemplatedParent());

    if (templatedParent != NULL)
    {
        BindToTemplatedParent(TicksProperty, Slider::TicksProperty);
        BindToTemplatedParent(TickFrequencyProperty, Slider::TickFrequencyProperty);
        BindToTemplatedParent(IsSelectionRangeEnabledProperty, Slider::IsSelectionRangeEnabledProperty);
        BindToTemplatedParent(SelectionStartProperty, Slider::SelectionStartProperty);
        BindToTemplatedParent(SelectionEndProperty, Slider::SelectionEndProperty);
        BindToTemplatedParent(MinimumProperty, RangeBase::MinimumProperty);
        BindToTemplatedParent(MaximumProperty, RangeBase::MaximumProperty);
        BindToTemplatedParent(IsDirectionReversedProperty, Slider::IsDirectionReversedProperty);

        /*if (!HasNonDefaultValue(ReservedSpaceProperty) && (templatedParent->GetTrack() != NULL))
        {
            Binding* binding = new Binding();
            binding->SetSource(templatedParent->GetTrack()->GetThumb());
            if (templatedParent->GetOrientation() == Orientation::Horizontal)
            {
                binding->SetPath(PropertyPath(FrameworkElement::ActualWidthProperty));
            }
            else
            {
                binding->SetPath(PropertyPath(FrameworkElement::ActualHeightProperty));
            }

            SetBinding(ReservedSpaceProperty, binding);
        }*/
    }
}

void TickBar::OnRender(Drawing* drawing)
{
}

}
