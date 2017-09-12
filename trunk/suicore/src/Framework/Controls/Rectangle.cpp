
#include <Framework/Controls/Rectangle.h>
#include <System/Graphics/SolidcolorBrush.h>

namespace suic
{

ImplementRTTIOfClass(Rectangle, Shape)

DpProperty* Rectangle::RadiusXProperty;
DpProperty* Rectangle::RadiusYProperty;

void Rectangle::StaticInit()
{
    if (NULL == RadiusXProperty)
    {
        Shape::StaticInit();
        RadiusXProperty = DpProperty::Register(_T("RadiusX"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsNone));
        RadiusYProperty = DpProperty::Register(_T("RadiusY"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsNone));
    }
}

Rectangle::Rectangle()
{
}

Rectangle::~Rectangle()
{

}

Size Rectangle::OnMeasure(const Size& size)
{
    return Shape::OnMeasure(size);
}

void Rectangle::OnRenderSizeChanged(SizeChangedInfo& e)
{
    Shape::OnRenderSizeChanged(e);
}

void Rectangle::OnRender(Drawing* drawing)
{
    fRect rect(Point(), GetRenderSize());

    int radiusX = GetRadiusX();
    int radiusY = GetRadiusY();

    int iStretch = GetStretch();
    Brush* brush = GetFill();
    Brush* stroke = GetStroke();
    Float iStrokeThickness = GetStrokeThickness();

    if (FloatUtil::GreaterThan(iStrokeThickness, 0) && stroke)
    {
        Pen pen;

        InitPen(&pen, stroke, iStrokeThickness);

        if (radiusX == 0 && radiusY == 0)
        {
            drawing->DrawRect(DrawCtx::DefDraw, brush, &pen, rect);
        }
        else
        {
            drawing->DrawRoundRect(DrawCtx::DefDraw, brush, &pen, &rect, radiusX, radiusY);
        }
    }
    else
    {
        if (radiusX == 0 && radiusY == 0)
        {
            drawing->DrawRect(DrawCtx::DefDraw, brush, NULL, rect);
        }
        else
        {
            drawing->DrawRoundRect(DrawCtx::DefDraw, brush, NULL, &rect, radiusX, radiusY);
        }
    }
}

}
