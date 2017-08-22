
#include "stdafx.h"
#include "PropertyManager.h"

suic::Array<PropItem*> PropManager::_props;

PropItem::PropItem(const suic::String& n, suic::ObjectPtr val, suic::DpProperty* t, suic::RTTIOfInfo* o)
    : name(n)
    , defVal(val)
    , prop(t)
    , owner(o)
    , elemBase(NULL)
{
}

PropManager::PropManager()
{
    InitProps();
}

PropManager* PropManager::Ins()
{
    static PropManager _ins;
    return &_ins;
}

void PropManager::InitProps()
{
    _props.Add(new PropItem(_U("Name"), new suic::OString(), suic::Element::NameProperty, suic::Element::RTTIType()));
    _props.Add(new PropItem(_U("Opacity"), new suic::OFloat(1.0f), suic::Element::OpacityProperty, suic::Element::RTTIType()));
    _props.Add(new PropItem(_U("Visibility"), new suic::Integer(), suic::Element::VisibilityProperty, suic::Element::RTTIType()));
    _props.Add(new PropItem(_U("ToolTip"), new suic::OString(), suic::FrameworkElement::ToolTipProperty, suic::FrameworkElement::RTTIType()));
    _props.Add(new PropItem(_U("IsEnabled"), suic::Boolean::True, suic::Element::IsEnabledProperty, suic::Element::RTTIType()));
    _props.Add(new PropItem(_U("Focusable"), suic::Boolean::True, suic::Element::FocusableProperty, suic::Element::RTTIType()));

    _props.Add(new PropItem(_U("Cursor"), NULL, suic::FrameworkElement::CursorProperty, suic::FrameworkElement::RTTIType()));
    
    _props.Add(new PropItem(_U("Foreground"), NULL, suic::Control::ForegroundProperty, suic::Control::RTTIType()));
    _props.Add(new PropItem(_U("Background"), NULL, suic::Control::BackgroundProperty, suic::Control::RTTIType()));
    _props.Add(new PropItem(_U("BorderThickness"), new suic::ORect(), suic::Control::BorderThicknessProperty, suic::Control::RTTIType()));
    
    _props.Add(new PropItem(_U("Template"), NULL, suic::Control::TemplateProperty, suic::Control::RTTIType()));
    
    _props.Add(new PropItem(_U("Width"), new suic::Integer(), suic::FrameworkElement::WidthProperty, suic::FrameworkElement::RTTIType()));
    _props.Add(new PropItem(_U("Height"), new suic::Integer(), suic::FrameworkElement::HeightProperty, suic::FrameworkElement::RTTIType()));
    _props.Add(new PropItem(_U("MinWidth"), new suic::Integer(), suic::FrameworkElement::MinWidthProperty, suic::FrameworkElement::RTTIType()));
    _props.Add(new PropItem(_U("MinHeight"), new suic::Integer(), suic::FrameworkElement::MinHeightProperty, suic::FrameworkElement::RTTIType()));
    _props.Add(new PropItem(_U("MaxWidth"), new suic::Integer(), suic::FrameworkElement::MaxWidthProperty, suic::FrameworkElement::RTTIType()));
    _props.Add(new PropItem(_U("MaxHeight"), new suic::Integer(), suic::FrameworkElement::MaxHeightProperty, suic::FrameworkElement::RTTIType()));
    
    _props.Add(new PropItem(_U("HorizontalAlignment"), new suic::Integer(), suic::FrameworkElement::HorizontalAlignmentProperty, suic::FrameworkElement::RTTIType()));
    _props.Add(new PropItem(_U("VerticalAlignment"), new suic::Integer(), suic::FrameworkElement::VerticalAlignmentProperty, suic::FrameworkElement::RTTIType()));
    _props.Add(new PropItem(_U("HorizontalContentAlignment"), new suic::Integer(), suic::Control::HorizontalContentAlignmentProperty, suic::Control::RTTIType()));
    _props.Add(new PropItem(_U("VerticalContentAlignment"), new suic::Integer(), suic::Control::VerticalContentAlignmentProperty, suic::Control::RTTIType()));
    
    _props.Add(new PropItem(_U("Padding"), new suic::ORect(), suic::Control::PaddingProperty, suic::Control::RTTIType()));
    _props.Add(new PropItem(_U("Margin"), new suic::ORect(), suic::FrameworkElement::MarginProperty, suic::FrameworkElement::RTTIType()));
    
    _props.Add(new PropItem(_U("Fill"), NULL, suic::Shape::FillProperty, suic::Shape::RTTIType()));
    _props.Add(new PropItem(_U("StrokeBrush"), NULL, suic::Shape::StrokeProperty, suic::Shape::RTTIType()));
    _props.Add(new PropItem(_U("StrokeThickness"), new suic::Integer(1), suic::Shape::StrokeThicknessProperty, suic::Shape::RTTIType()));
    
    // ToggleButton
    _props.Add(new PropItem(_U("IsChecked"), suic::Boolean::False, suic::ToggleButton::IsCheckedProperty, suic::ToggleButton::RTTIType()));
}

PropItem* PropManager::FindProp(const suic::String& name)
{
    PropItem* val = NULL;
    for (int i = 0; i < PropManager::Ins()->GetCount(); ++i)
    {
        PropItem* propItem = PropManager::Ins()->GetPropItem(i);
        if (propItem->name.Equals(name))
        {
            val = propItem;
            break;
        }
    }
    return val;
}

int PropManager::GetCount() const
{
    return _props.GetCount();
}

PropItem* PropManager::GetPropItem(int index) const
{
    return _props[index];
}
