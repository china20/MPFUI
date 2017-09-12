// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// RangeBase.cpp

#include <Framework/Controls/RangeBase.h>
#include <System/Tools/EventHelper.h>

namespace suic
{

ImplementRTTIOfClass(RangeBase, Control)

DpProperty* RangeBase::ValueProperty;
RoutedEvent* RangeBase::ValueChangedEvent;

DpProperty* RangeBase::LargeChangeProperty;
DpProperty* RangeBase::MaximumProperty;
DpProperty* RangeBase::MinimumProperty;
DpProperty* RangeBase::SmallChangeProperty;

RangeBase::RangeBase()
{
}

RangeBase::~RangeBase()
{
}

void RangeBase::OnValuePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    RangeBase* rb(RTTICast<RangeBase>(d));
    Float oldValue = e->GetOldValue()->ToFloat();
    Float newValue = e->GetNewValue()->ToFloat();

    rb->OnValueChanged(oldValue, newValue);
}

void RangeBase::OnMinimumPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    RangeBase* rb(RTTICast<RangeBase>(d));
    Float oldValue = e->GetOldValue()->ToFloat();
    Float newValue = e->GetNewValue()->ToFloat();

    rb->OnMinimumChanged(oldValue, newValue);
}

void RangeBase::OnMaximumPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    RangeBase* rb(RTTICast<RangeBase>(d));
    Float oldValue = e->GetOldValue()->ToFloat();
    Float newValue = e->GetNewValue()->ToFloat();

    rb->OnMaximumChanged(oldValue, newValue);
}

void RangeBase::StaticInit()
{
    if (ValueProperty == NULL)
    {
        Control::StaticInit();
        ValueChangedEvent = EventHelper::RegisterRoutedEvent(_U("ValueChanged"), RoutingStrategy::Bubble
            , FloatPropChangedEventHandler::RTTIType(), RangeBase::RTTIType());

        ValueProperty = DpProperty::Register(_T("Value"), RTTIType(), OFloat::RTTIType()
            , DpPropMemory::GetPropMeta(OFloat::GetZeroFloat(), PropMetadataOptions::AffectsMeasure, &RangeBase::OnValuePropChanged));

        LargeChangeProperty = DpProperty::Register(_T("LargeChange"), RTTIType(), OFloat::RTTIType()
            , DpPropMemory::GetPropMeta(new OFloat(1.0f), PropMetadataOptions::AffectsArrange));
        MaximumProperty = DpProperty::Register(_T("Maximum"), RTTIType(), OFloat::RTTIType()
            , DpPropMemory::GetPropMeta(new OFloat(1.0f), PropMetadataOptions::AffectsArrange, &RangeBase::OnMaximumPropChanged));
        MinimumProperty = DpProperty::Register(_T("Minimum"), RTTIType(), OFloat::RTTIType()
            , DpPropMemory::GetPropMeta(OFloat::GetZeroFloat(), PropMetadataOptions::AffectsArrange, &RangeBase::OnMinimumPropChanged));
        SmallChangeProperty = DpProperty::Register(_T("SmallChange"), RTTIType(), OFloat::RTTIType()
            , DpPropMemory::GetPropMeta(new OFloat(0.1f), PropMetadataOptions::AffectsArrange));
    }
}

void RangeBase::AddValueChanged(FloatPropChangedEventHandler* handler)
{
    AddHandler(ValueChangedEvent, handler, false);
}

void RangeBase::RemoveValueChanged(FloatPropChangedEventHandler& handler)
{
    RemoveHandler(ValueChangedEvent, &handler);
}

Float RangeBase::GetLargeChange()
{
    return Control::GetValue(LargeChangeProperty)->ToFloat();
}

void RangeBase::SetLargeChange(Float val)
{
    Control::SetValue(LargeChangeProperty, new OFloat(val));
}

Float RangeBase::GetSmallChange()
{
    return Control::GetValue(SmallChangeProperty)->ToFloat();
}

void RangeBase::SetSmallChange(Float val)
{
    Control::SetValue(SmallChangeProperty, new OFloat(val));
}

Float RangeBase::GetMaximum()
{
    return Control::GetValue(MaximumProperty)->ToFloat();
}

void RangeBase::SetMaximum(Float val)
{
    Control::SetValue(MaximumProperty, new OFloat(val));
}

Float RangeBase::GetMinimum()
{
    return Control::GetValue(MinimumProperty)->ToFloat();
}

void RangeBase::SetMinimum(Float val)
{
    if (val != GetMinimum())
    {
        Control::SetValue(MinimumProperty, new OFloat(val));
    }
}

Float RangeBase::GetValue()
{
    return Control::GetValue(ValueProperty)->ToFloat();
}

void RangeBase::SetValue(Float val)
{
    if (val < GetMinimum())
    {
        val = GetMinimum();
    }
    else if (val >= GetMaximum())
    {
        val = GetMaximum();
    }

    Control::SetValue(ValueProperty, new OFloat(val));
}

void RangeBase::OnMaximumChanged(Float oldMaximum, Float newMaximum)
{
}

void RangeBase::OnMinimumChanged(Float oldMinimum, Float newMinimum)
{
}

void RangeBase::OnValueChanged(Float oldValue, Float newValue)
{
    UpdateLayout();
    FloatPropChangedEventArg e(oldValue, newValue, ValueChangedEvent);
    RaiseEvent(&e);
}

}
