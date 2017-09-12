// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Control.cpp

#include <Framework/Controls/Control.h>
#include <Framework/Controls/Panel.h>
#include <Framework/Controls/Border.h>
#include <Framework/Controls/TextElement.h>

#include <System/Tools/CoreHelper.h>
#include <System/Tools/StyleHelper.h>
#include <System/Tools/EventHelper.h>
#include <System/Input/MouseDevice.h>

/*--------内部使用头文件--------*/
#include <System/Render/Skia/SkTextOp.h>
#include <System/Interop/InternalOper.h>
#include <System/Windows/VisualStates.h>
#include <System/Input/KeyboardNavigation.h>

namespace suic
{

DpProperty* Control::FontFamilyProperty;
DpProperty* Control::FontSizeProperty;
DpProperty* Control::FontWeightProperty;
DpProperty* Control::FontStyleProperty;
DpProperty* Control::FontStretchProperty;

DpProperty* Control::PaddingProperty;
DpProperty* Control::CornerRadiusProperty;

DpProperty* Control::BackgroundProperty;
DpProperty* Control::ForegroundProperty;
DpProperty* Control::BorderBrushProperty;
DpProperty* Control::BorderThicknessProperty;

DpProperty* Control::TemplateProperty;

DpProperty* Control::TabIndexProperty;
DpProperty* Control::IsTabStopProperty;

DpProperty* Control::HorizontalContentAlignmentProperty;
DpProperty* Control::VerticalContentAlignmentProperty;

RoutedEvent* Control::MouseDoubleClickEvent;
RoutedEvent* Control::PreviewMouseDoubleClickEvent;

ImplementRTTIOfClass(Control, FrameworkElement)

Control::Control()
    : _templateCache(NULL)
    , _itemFlags(0)
{
    //WriteDoFlag(ViewFlags::IsClipToBoundsVF, true);
}

Control::~Control()
{
}

void Control::OnTemplatePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Control* fe(RTTICast<Control>(d));
    FrameworkTemplate* oldTemp = RTTICast<FrameworkTemplate>(e->GetOldValue());
    FrameworkTemplate* newTemp = RTTICast<FrameworkTemplate>(e->GetNewValue());

    StyleHelper::TemplatedBindField->ClearValue(d);
    StyleHelper::UpdateTemplateCache(fe, oldTemp, newTemp, TemplateProperty);
}

void Control::HandleDoubleClick(Element* sender, MouseButtonEventArg* e)
{
    if (e->GetClickCount() == 2)
    {
        Control* control = RTTICast<Control>(sender);
        DpObject* d = MouseDevice::GetMouseDown(e->GetMouseButton());

        if (d != NULL)
        {
            MouseButtonEventArg args(d, e->GetMousePoint());

            if ((e->GetRoutedEvent() == Element::PreMouseLButtonDownEvent) || 
                (e->GetRoutedEvent() == Element::PreMouseRButtonDownEvent))
            {
                args.SetRoutedEvent(PreviewMouseDoubleClickEvent);
                control->OnPreviewMouseDoubleClick(&args);
            }
            else
            {
                args.SetRoutedEvent(MouseDoubleClickEvent);
                control->OnMouseDoubleClick(&args);
            }

            if (args.IsHandled())
            {
                e->SetHandled(true);
            }
        }
    }
}

void Control::StaticInit()
{
    if (NULL == PaddingProperty)
    {
        suic::TextElement::StaticInit();
        Panel::StaticInit();
        Border::StaticInit();

        FocusableProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsArrange));

        // 初始化属性
        FontFamilyProperty = suic::TextElement::FontFamilyProperty->AddOwner(RTTIType(), NULL);
        FontSizeProperty = suic::TextElement::FontSizeProperty->AddOwner(RTTIType(), NULL);
        FontStretchProperty = suic::TextElement::FontStretchProperty->AddOwner(RTTIType(), NULL);
        FontStyleProperty = suic::TextElement::FontStyleProperty->AddOwner(RTTIType(), NULL);
        FontWeightProperty = suic::TextElement::FontWeightProperty->AddOwner(RTTIType(), NULL);
        ForegroundProperty = suic::TextElement::ForegroundProperty->AddOwner(RTTIType(), NULL);

        BackgroundProperty = Panel::BackgroundProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender));
        
        CornerRadiusProperty = Border::CornerRadiusProperty->AddOwner(RTTIType(), NULL);
        BorderBrushProperty = Border::BorderBrushProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender));
        BorderThicknessProperty = Border::BorderThicknessProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(new ORect(), PropMetadataOptions::AffectsParentMeasure));

        PaddingProperty = DpProperty::Register(_T("Padding"), RTTIType(), ORect::RTTIType()
            , DpPropMemory::GetPropMeta(new ORect(), PropMetadataOptions::AffectsParentMeasure));

        HorizontalContentAlignmentProperty = DpProperty::Register(_T("HorizontalContentAlignment")
            , RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(HoriAlignBox::From(HoriAlign::haLeft), PropMetadataOptions::AffectsArrange));
        VerticalContentAlignmentProperty = DpProperty::Register(_T("VerticalContentAlignment")
            , RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(VertAlignBox::From(VertAlign::vaCenter), PropMetadataOptions::AffectsArrange));

        HorizontalContentAlignmentProperty->SetConvertValueCb(AlignmentConvert::Convert);
        VerticalContentAlignmentProperty->SetConvertValueCb(AlignmentConvert::Convert);

        TemplateProperty = DpProperty::Register(_T("Template"), RTTIType(), ControlTemplate::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsParentMeasure, PropChangedCallback(Control::OnTemplatePropChanged)));

        TabIndexProperty = KeyboardNavigation::TabIndexProperty->AddOwner(RTTIType(), NULL);
        IsTabStopProperty = KeyboardNavigation::IsTabStopProperty->AddOwner(RTTIType(), NULL);

        MouseDoubleClickEvent = EventHelper::RegisterRoutedEvent(_U("MouseDoubleClick"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());
        PreviewMouseDoubleClickEvent = EventHelper::RegisterRoutedEvent(_U("PreviewMouseDoubleClick"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());

        //EventHelper::RegisterClassHandler(RTTIType(), Element::PreMouseLButtonDownEvent, new MouseButtonEventHandler(&Control::HandleDoubleClick), true);
        //EventHelper::RegisterClassHandler(RTTIType(), Element::MouseLButtonDownEvent, new MouseButtonEventHandler(&Control::HandleDoubleClick), true);
        //EventHelper::RegisterClassHandler(RTTIType(), Element::PreMouseRButtonDownEvent, new MouseButtonEventHandler(&Control::HandleDoubleClick), true);
        //EventHelper::RegisterClassHandler(RTTIType(), Element::MouseRButtonDownEvent, new MouseButtonEventHandler(&Control::HandleDoubleClick), true);
    }
}

void Control::OnPreviewMouseDown(MouseButtonEventArg* e)
{
    if (e->GetClickCount() == 2)
    {
        DpObject* d = MouseDevice::GetMouseDown(e->GetMouseButton());
        if (d != NULL)
        {
            MouseButtonEventArg args(d, e->GetMousePoint());
            if (e->GetMouseButton() == suic::MouseButton::mbLeftButton)
            {
                args.SetRoutedEvent(PreviewMouseDoubleClickEvent);
                OnPreviewMouseDoubleClick(&args);
                if (args.IsHandled())
                {
                    e->SetHandled(true);
                }
            }
        }
    }
}

void Control::OnMouseDown(MouseButtonEventArg* e)
{
    if (e->GetClickCount() == 2)
    {
        DpObject* d = MouseDevice::GetMouseDown(e->GetMouseButton());
        if (d != NULL)
        {
            MouseButtonEventArg args(d, e->GetMousePoint());
            if (e->GetMouseButton() == suic::MouseButton::mbLeftButton)
            {
                args.SetRoutedEvent(MouseDoubleClickEvent);
                OnMouseDoubleClick(&args);
                if (args.IsHandled())
                {
                    e->SetHandled(true);
                }
            }
        }
    }
}

FrameworkElement* Control::GetCommandTarget()
{
    FrameworkElement* templatedParent = RTTICast<FrameworkElement>(GetTemplatedParent());
    if (templatedParent == NULL)
    {
        templatedParent = this;
    }
    return templatedParent;
}

fRect Control::GetCornerRadius()
{
    return OfRect::FromObj(GetValue(CornerRadiusProperty));
}

void Control::SetCornerRadius(fRect val)
{
    SetValue(CornerRadiusProperty, new OfRect(val));
}

bool Control::IsTabStop()
{
    return ReadFlag(CoreFlags::IsTabStop);
}

void Control::SetTabStop(bool val)
{
    WriteFlag(CoreFlags::IsTabStop, val);
}

int Control::GetTabIndex()
{
    return GetValue(TabIndexProperty)->ToInt();
}

void Control::SetTabIndex(int index)
{
    SetValue(TabIndexProperty, new Integer(index));
}

Rect Control::GetBorderThickness()
{
    return ORect::FromObj(GetValue(BorderThicknessProperty));
}

void Control::SetBorderThickness(Rect border)
{
    SetValue(BorderThicknessProperty, new ORect(border));
}

String Control::GetFontFamily()
{
    return GetValue(FontFamilyProperty)->ToString();
}

void Control::SetFontFamily(String val)
{
    SetValue(FontFamilyProperty, new OString(val));
}

int Control::GetFontSize()
{
    return GetValue(FontSizeProperty)->ToInt();
}

void Control::SetFontSize(int val)
{
    SetValue(FontSizeProperty, new Integer(val));
}

int Control::GetFontWeight()
{
    return GetValue(FontWeightProperty)->ToInt();
}

void Control::SetFontWeight(int val)
{
    SetValue(FontWeightProperty, new Integer(val));
}

int Control::GetFontStyle()
{
    return GetValue(FontStyleProperty)->ToInt();
}

void Control::SetFontStyle(int val)
{
    SetValue(FontStyleProperty, new Integer(val));
}

int Control::GetFontStretch()
{
    return GetValue(FontStretchProperty)->ToInt();
}

void Control::SetFontStretch(int val)
{
    SetValue(FontStretchProperty, new Integer(val));
}

Brush* Control::GetBackground()
{
    return RTTICast<Brush>(GetValue(BackgroundProperty));
}

void Control::SetBackground(Brush* val)
{
    VerifyAccess(__FILELINE__);
    SetValue(BackgroundProperty, val);
}

Brush* Control::GetForeground()
{
    return RTTICast<Brush>(GetValue(ForegroundProperty));
}

void Control::SetForeground(Brush* val)
{
    VerifyAccess(__FILELINE__);
    SetValue(ForegroundProperty, val);
}

Brush* Control::GetBorderBrush()
{
    return RTTICast<Brush>(GetValue(BorderBrushProperty));
}

void Control::SetBorderBrush(Brush* val)
{
    SetValue(BorderBrushProperty, val);
}

int Control::GetHorizontalContentAlignment()
{
    return GetValue(HorizontalContentAlignmentProperty)->ToInt();
}

int Control::GetVerticalContentAlignment()
{
    return GetValue(VerticalContentAlignmentProperty)->ToInt();
}

void Control::SetHorizontalContentAlignment(int val)
{
    SetValue(HorizontalContentAlignmentProperty, HoriAlignBox::From(val));
}

void Control::SetVerticalContentAlignment(int val)
{
    SetValue(VerticalContentAlignmentProperty, VertAlignBox::From(val));
}

void Control::AddPreviewMouseDoubleClick(MouseButtonEventHandler* h)
{
    AddHandler(PreviewMouseDoubleClickEvent, h, false);
}

void Control::RemovePreviewMouseDoubleClick(MouseButtonEventHandler* h)
{
    RemoveHandler(PreviewMouseDoubleClickEvent, h);
}

void Control::AddMouseDoubleClick(MouseButtonEventHandler* h)
{
    AddHandler(MouseDoubleClickEvent, h, false);
}

void Control::RemoveMouseDoubleClick(MouseButtonEventHandler* h)
{
    RemoveHandler(MouseDoubleClickEvent, h);
}

Rect Control::GetPadding()
{
    return ORect::FromObj(GetValue(PaddingProperty));
}

void Control::SetPadding(Rect rc)
{
    SetValue(PaddingProperty, new ORect(&rc));
}

Size Control::OnMeasure(const Size& constraint)
{
    if (GetVisualChildrenCount() > 0)
    {
        Element* child = GetVisualChild(0);

        if (child != NULL)
        {
            MeasureChild(child, constraint);
            return child->GetDesiredSize();
        }
    }
    
    return Size();
}

void Control::OnArrange(const Size& arrangeSize)
{
    if (GetVisualChildrenCount() > 0)
    {
        Element* child = GetVisualChild(0);

        if (child != NULL)
        {
            Rect rect(0, 0, arrangeSize.cx, arrangeSize.cy);
            child->Arrange(rect);
        }
    }
}

void Control::OnMouseDoubleClick(MouseButtonEventArg* e)
{
    RaiseEvent(e);
}

void Control::OnPreviewMouseDoubleClick(MouseButtonEventArg* e)
{
    RaiseEvent(e);
}

void Control::OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate)
{
    FrameworkElement::OnTemplateChangedInternal(oldTemplate, newTemplate);
    Control::OnTemplateChanged((ControlTemplate*)oldTemplate, (ControlTemplate*)newTemplate);
}

void Control::OnTemplateChanged(ControlTemplate* oldTemp, ControlTemplate* newTemp)
{
}

}
