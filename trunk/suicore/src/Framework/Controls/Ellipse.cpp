
#include <Framework/Controls/Ellipse.h>
#include <System/Graphics/SolidcolorBrush.h>

namespace suic
{

ImplementRTTIOfClass(Ellipse, Shape)

void Ellipse::StaticInit()
{
}

Ellipse::Ellipse()
{

}

Ellipse::~Ellipse()
{

}

Size Ellipse::OnMeasure(const Size& size)
{
    return Shape::OnMeasure(size);
}

void Ellipse::OnRender(Drawing * drawing)
{
    fRect rect(Point(), GetRenderSize());
    Float iStrokeThickness = GetStrokeThickness();
    Brush* strokeBrush = GetStroke();
    Brush* fillBrush = GetFill();

    if (NULL != strokeBrush && iStrokeThickness > 0)
    {
        Pen pen;

        InitPen(&pen, strokeBrush, iStrokeThickness);
        drawing->DrawEllipse(DrawCtx::DefDraw, fillBrush, &pen, &rect);
    }
    else
    {
        drawing->DrawEllipse(DrawCtx::DefDraw, fillBrush, NULL, &rect);
    }
}

}
