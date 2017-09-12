/////////////////////////////////////////////////////////////////////////
// Expander.cpp

#include <Framework/Controls/Expander.h>
#include <System/Windows/EventManager.h>

namespace suic
{

/////////////////////////////////////////////////////////////////////////
// Expander

ImplementRTTIOfClass(Expander, HeaderedContentControl)

RoutedEvent* Expander::ExpandedEvent;
RoutedEvent* Expander::CollapsedEvent;
DpProperty* Expander::ExpandDirectionProperty;
DpProperty* Expander::IsExpandedProperty;

class ExpandDirectionConvert : public IConvertValue
{
public:

    ExpandDirectionConvert()
    {

    }

    AutoObj ConvertValue(DpProperty* dp, Object* prop)
    {
        String val = prop->ToString();

        if (val.Equals(_T("Left")))
        {
            return Integer::GetPosInt(ExpandDirection::edLeft);
        }
        else if (val.Equals(_T("Right")))
        {
            return Integer::GetPosInt(ExpandDirection::edRight);
        }
        else if (val.Equals(_T("Up")))
        {
            return Integer::GetPosInt(ExpandDirection::edUp);
        }
        else
        {
            return Integer::GetPosInt(ExpandDirection::edDown);
        }
    }
};

Expander::Expander()
{
}

Expander::~Expander()
{
}

void Expander::OnIsExpandedChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Expander* element = DynamicCast<Expander>(d);
    bool newValue = (bool) e->GetNewValue()->ToBool();

    if (newValue)
    {
        element->OnExpanded();
    }
    else
    {
        element->OnCollapsed();
    }
    element->UpdateVisualState(true);
}

void Expander::StaticInit()
{
    if (NULL == IsExpandedProperty)
    {
        HeaderedContentControl::StaticInit();

        ExpandedEvent = EventControl::RegisterRoutedEvent(_T("Expanded"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
        CollapsedEvent = EventControl::RegisterRoutedEvent(_T("Collapsed"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
        ExpandDirectionProperty = DpProperty::Register(_T("ExpandDirection"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetPosInt(ExpandDirection::edDown), PropMetadataOptions::AffectsParentMeasure));
        IsExpandedProperty = DpProperty::Register(_T("IsExpanded"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsParentMeasure, &OnIsExpandedChanged));

        ExpandDirectionProperty->SetConvertValueCb(new ExpandDirectionConvert());
        Control::IsTabStopProperty->OverrideMetadata(RTTIType(), new PropMetadata(Boolean::False));
    }
}

ExpandDirection Expander::GetExpandDirection()
{
    return (ExpandDirection)GetValue(ExpandDirectionProperty)->ToInt();
}

void Expander::SetExpandDirection(ExpandDirection val)
{
    SetValue(ExpandDirectionProperty, Integer::GetPosInt(val));
}

bool Expander::IsExpanded()
{
    return GetValue(IsExpandedProperty)->ToBool();
}

void Expander::AddExpanded(RoutedEventHandler* handler)
{
    AddHandler(ExpandedEvent, handler);
}

void Expander::AddCollapsed(RoutedEventHandler* handler)
{
    AddHandler(CollapsedEvent, handler);
}

void Expander::Expand()
{
    SetValue(IsExpandedProperty, Boolean::True);
}

void Expander::Collapse()
{
    SetValue(IsExpandedProperty, Boolean::False);
}

void Expander::OnCollapsed()
{
    RoutedEventArg e;

    e.SetRoutedEvent(CollapsedEvent);
    RaiseEvent(&e);
}

void Expander::OnExpanded()
{
    RoutedEventArg e;

    e.SetRoutedEvent(ExpandedEvent);
    RaiseEvent(&e);
}

}
