// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Line.cpp

#include <Framework/Controls/Line.h>

#include <System/Tools/Debug.h>
#include <System/Graphics/SolidcolorBrush.h>

namespace suic
{

ImplementRTTIOfClass(Line, Shape)

DpProperty* Line::X1Property;
DpProperty* Line::Y1Property;
DpProperty* Line::X2Property;
DpProperty* Line::Y2Property;

Line::Line()
{
}

Line::~Line()
{
}

void Line::OnX1PropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Line* lObj = DynamicCast<Line>(d);
    lObj->ResetLineInfo();
}

void Line::OnY1PropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Line* lObj = DynamicCast<Line>(d);
    lObj->ResetLineInfo();
}

void Line::OnX2PropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Line* lObj = DynamicCast<Line>(d);
    lObj->ResetLineInfo();
}

void Line::OnY2PropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Line* lObj = DynamicCast<Line>(d);
    lObj->ResetLineInfo();
}

void Line::StaticInit()
{
    if (X1Property == NULL)
    {
        Shape::StaticInit();

        X1Property = DpProperty::Register(_U("X1"), RTTIType(), OFloat::RTTIType(), 
            DpPropMemory::GetPropMeta(new OFloat(0), PropMetadataOptions::AffectsParentArrange, &Line::OnX1PropChanged));
        Y1Property = DpProperty::Register(_U("Y1"), RTTIType(), OFloat::RTTIType(), 
            DpPropMemory::GetPropMeta(new OFloat(0), PropMetadataOptions::AffectsParentArrange, &Line::OnY1PropChanged));
        X2Property = DpProperty::Register(_U("X2"), RTTIType(), OFloat::RTTIType(), 
            DpPropMemory::GetPropMeta(new OFloat(0), PropMetadataOptions::AffectsParentArrange, &Line::OnX2PropChanged));
        Y2Property = DpProperty::Register(_U("Y2"), RTTIType(), OFloat::RTTIType(), 
            DpPropMemory::GetPropMeta(new OFloat(0), PropMetadataOptions::AffectsParentArrange, &Line::OnY2PropChanged));
    }
}

Float Line::GetX1()
{
    return GetValue(X1Property)->ToFloat();
}

void Line::SetX1(Float val)
{
    SetValue(X1Property, new OFloat(val));
}

Float Line::GetY1()
{
    return GetValue(Y1Property)->ToFloat();
}

void Line::SetY1(Float val)
{
    SetValue(Y1Property, new OFloat(val));
}

Float Line::GetX2()
{
    return GetValue(X2Property)->ToFloat();
}

void Line::SetX2(Float val)
{
    SetValue(X2Property, new OFloat(val));
}

Float Line::GetY2()
{
    return GetValue(Y2Property)->ToFloat();
}

void Line::SetY2(Float val)
{
    SetValue(Y2Property, new OFloat(val));
}

void Line::ResetLineInfo()
{
    if (NULL == _renderedGeometry)
    {
        _renderedGeometry = new LineGeometry();
    }
}

Size Line::OnMeasure(const Size& size)
{
    Size measureSize;
    measureSize.cx = Math::Abs(GetX1() - GetX2());
    measureSize.cy = Math::Abs(GetY1() - GetY2());

    return measureSize;
}

void Line::OnArrange(const suic::Size& arrangeSize)
{
}

void Line::OnRender(Drawing * drawing)
{
    Float iStrokeThickness = GetStrokeThickness();
    Brush* brush = GetStroke();

    if (NULL != brush)
    {
        Pen pen;
        fPoint pt1(GetX1(), GetY1());
        fPoint pt2(GetX2(), GetY2());

        pen.SetThickness(iStrokeThickness);
        pen.SetBrush(brush);

        drawing->DrawLine(DrawCtx::DefDraw, &pen, pt1, pt2);
    }
}

}
