
#include <Framework/Controls/CaretElement.h>
#include <Framework/Animation/AnimationAsSeries.h>
#include <System/Windows/Assigner.h>
#include <System/Windows/CoreTool.h>

/*--------内部使用头文件--------*/
#include <System/Graphics/Solidcolorbrush.h>

namespace suic
{

DpProperty* CaretElement::BackgroundProperty;

ImplementRTTIOfClass(CaretElement, FrameworkElement)

CaretElement::CaretElement()
{
}

CaretElement::~CaretElement()
{
}

void CaretElement::StaticInit()
{
    if (NULL == BackgroundProperty)
    {
        BackgroundProperty = DpProperty::Register(_U("Background"), RTTIType(), Brush::RTTIType()
            , DpPropMemory::GetPropMeta(SolidColorBrush::Black, PropMetadataOptions::AffectsRender));
    }
}

void CaretElement::Show()
{
    if (GetVisibility() != Visibility::Visible)
    {
        DoubleAnimation* ani = new DoubleAnimation(0.0f, 1.0f, Duration(600), FillBehavior::fbStop);
        RepeatBehavior rb;

        rb.type = RepeatBehavior::Forever;
        ani->SetAutoReverse(true);
        ani->SetRepeatBehavior(rb);
        BeginAnimation(OpacityProperty, ani);
        SetVisibility(Visibility::Visible);
    }
}

suic::Brush* CaretElement::GetBackground()
{
    return suic::RTTICast<suic::Brush>(GetValue(BackgroundProperty));
}

void CaretElement::SetBackground(suic::Brush* val)
{
    SetValue(BackgroundProperty, val);
}

bool CaretElement::IsBlinking()
{
    return false;
}

void CaretElement::Hide()
{
    if (GetVisibility() != Visibility::Collapsed)
    {
        BeginAnimation(OpacityProperty, NULL);
        SetVisibility(Visibility::Collapsed);
        InvalidateVisual();
    }
}

void CaretElement::SetPos(Point point)
{
    _offset = point;
    _canvasOffset = GetUIParent()->GetCanvasOffset();
    _canvasOffset.x += _offset.x;
    _canvasOffset.y += _offset.y;
    _finalArrange = Rect(point, _measureSize);
}

void CaretElement::SetSize(Size size)
{
    _renderSize = size;
    _measureSize = size;
}

void CaretElement::OnInitialized(EventArg* e)
{
    FrameworkElement::OnInitialized(e);

    bool del = setAutoDelete(false);

    if (GetWidth() <= 0)
    {
        SetWidth(2);
    }

    setAutoDelete(del);
}

void CaretElement::OnRender(Drawing* drawing)
{
    if (IsVisible())
    {
        Brush* brush = GetBackground();
        if (NULL != brush)
        {
            fRect rect(Point(), GetRenderSize());
            drawing->DrawRect(DrawCtx::DefDraw, brush, NULL, rect);
        }
    }
}

}
