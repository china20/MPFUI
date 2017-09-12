/////////////////////////////////////////////////////////////////////////
// Border.cpp

#include <Framework/Controls/Panel.h>
#include <Framework/Controls/Border.h>
#include <Framework/Controls/TextElement.h>

#include <System/Graphics/Solidcolorbrush.h>

#include <System/Render/Skia/SkTextOp.h>
#include <System/Render/Skia/SkiaDrawing.h>

#include <Skia/effects/SkBlurMaskFilter.h>

namespace suic
{

ImplementRTTIOfClass(Border, Decorator)

DpProperty* Border::CornerRadiusProperty;
DpProperty* Border::BackgroundProperty;
DpProperty* Border::PaddingProperty;
DpProperty* Border::BorderBrushProperty;
DpProperty* Border::BorderThicknessProperty;

Border::Border()
{
    _background = NULL;
    _foreground = NULL;
    _borderBrush = NULL;
    _cachedBitmap = NULL;
}

Border::~Border()
{
}

void Border::OnBackgroundPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Border* border = RTTICast<Border>(d);
    if (border != NULL)
    {
        border->_background = RTTICast<Brush>(e->GetNewValue());
        border->ResetCachedBitmap();
    }
}

void Border::OnForegroundPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Border* border = RTTICast<Border>(d);
    if (border != NULL)
    {
        border->_foreground = RTTICast<Brush>(e->GetNewValue());
        border->ResetCachedBitmap();
    }
}

void Border::OnBorderBrushPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Border* border = RTTICast<Border>(d);
    if (border != NULL)
    {
        border->_borderBrush = DynamicCast<Brush>(e->GetNewValue());
        border->ResetCachedBitmap();
    }
}

void Border::OnBorderThicknessPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Border* border = RTTICast<Border>(d);
    if (border != NULL)
    {
        border->_thickness = ORect::FromObj(e->GetNewValue());
        border->ResetCachedBitmap();
    }
}

void Border::OnPaddingPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Border* border = RTTICast<Border>(d);
    if (border != NULL)
    {
        border->_padding = ORect::FromObj(e->GetNewValue());
        border->ResetCachedBitmap();
    }
}

void Border::StaticInit()
{
    if (BorderThicknessProperty == NULL)
    {
        if (NULL == FrameworkElement::WidthProperty)
        {
            FrameworkElement::StaticInit();
        }

        if (NULL == Panel::BackgroundProperty)
        {
            Panel::StaticInit();
        }

        CornerRadiusProperty = DpProperty::Register(_U("CornerRadius"), RTTIType(), OfRect::RTTIType()
            , DpPropMemory::GetPropMeta(OfRect::GetEmpty(), PropMetadataOptions::AffectsRender));

        BackgroundProperty = Panel::BackgroundProperty->AddOwner(RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender, &Border::OnBackgroundPropChanged));
        PaddingProperty = DpProperty::Register(_U("Padding"), RTTIType(), ORect::RTTIType()
            , DpPropMemory::GetPropMeta(new ORect(), PropMetadataOptions::AffectsRender, &Border::OnPaddingPropChanged));
        BorderBrushProperty = DpProperty::Register(_U("BorderBrush"), RTTIType(), Brush::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender, &Border::OnBorderBrushPropChanged));
        BorderThicknessProperty = DpProperty::Register(_U("BorderThickness"), RTTIType(), ORect::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsParentMeasure, &Border::OnBorderThicknessPropChanged));
    }
}

fRect Border::GetCornerRadius()
{
    return OfRect::FromObj(GetValue(CornerRadiusProperty));
}

void Border::SetCornerRadius(fRect val)
{
    SetValue(CornerRadiusProperty, new OfRect(val));
}

Rect Border::GetBorderThickness()
{
    return ORect::FromObj(GetValue(BorderThicknessProperty));
}

void Border::SetBorderThickness(Rect border)
{
    SetValue(BorderThicknessProperty, new ORect(border));
}

Rect Border::GetPadding()
{
    return ORect::FromObj(GetValue(PaddingProperty));
}

void Border::SetPadding(Rect rc)
{
    SetValue(PaddingProperty, new ORect(&rc));
}

Brush* Border::GetBackground()
{
    return RTTICast<Brush>(GetValue(BackgroundProperty));
}

void Border::SetBackground(Brush* val)
{
    VerifyAccess(__FILELINE__);
    SetValue(BackgroundProperty, val);
}

Brush* Border::GetBorderBrush()
{
    return RTTICast<Brush>(GetValue(BorderBrushProperty));
}

void Border::SetBorderBrush(Brush* val)
{
    SetValue(BorderBrushProperty, val);
}

bool Border::HitTestCore(HitTestCtx* hitResult, bool onlySelf)
{
    return Decorator::HitTestCore(hitResult, onlySelf);
}

Size Border::OnMeasure(const Size& constraint)
{
    Element* child = GetChild();
    Rect rcPadding = SystemParameters::TransformToDevice(_padding);
    Rect rcThickness = _thickness;
    // Rect rcThickness = SystemParameters::TransformToDevice(_thickness);

    Size sizeRet(rcThickness.SumLR() + rcPadding.SumLR(), rcThickness.SumTB() + rcPadding.SumTB());

    if (child != NULL)
    {
        Size childSize;
        
        childSize.cx = max(0, (constraint.Width() - sizeRet.Width()));
        childSize.cy = max(0, (constraint.Height() - sizeRet.Height()));
        
        MeasureChild(child, childSize);

        childSize = child->GetDesiredSize();
        childSize.cx += sizeRet.Width();
        childSize.cy += sizeRet.Height();

        return childSize;
    }
    else
    {
        return sizeRet;
    }
}

void Border::OnArrange(const Size& arrangeSize)
{
    if (_child)
    {
        Rect rcPadding = SystemParameters::TransformToDevice(_padding);
        Rect rcThickness = _thickness;
        // Rect rcThickness = SystemParameters::TransformToDevice(_thickness);

        int marginWidth = rcPadding.SumLR() + rcThickness.SumLR();
        int marginHeight = rcPadding.SumTB() + rcThickness.SumTB();
        int marginLeft = rcPadding.left + rcThickness.left;
        int marginTop = rcPadding.top + rcThickness.top;

        Rect arrangeRect(marginLeft, marginTop, 
            arrangeSize.cx - marginWidth, arrangeSize.cy - marginHeight);

        _child->Arrange(arrangeRect);
    }
}

static void InitRRect(fRRect& rrect, const fRect& radius)
{
    rrect.radii[0].x = radius.left;
    rrect.radii[0].y = radius.left;
    rrect.radii[1].x = radius.top;
    rrect.radii[1].y = radius.top;
    rrect.radii[2].x = radius.right;
    rrect.radii[2].y = radius.right;
    rrect.radii[3].x = radius.bottom;
    rrect.radii[3].y = radius.bottom;
}

void Border::OnRenderSizeChanged(SizeChangedInfo& e)
{
    Decorator::OnRenderSizeChanged(e);
    ResetCachedBitmap();
}

static void CalcInnerRRect(const fRRect& rrect, Rect radius, fRRect& outrect)
{
    if (rrect.radii[0].x > radius.left)
    {
        outrect.radii[0].x = rrect.radii[0].x - radius.left;
        outrect.radii[0].y = rrect.radii[0].x - radius.left;
    }
    else
    {
        outrect.radii[0].x = 0;
        outrect.radii[0].y = 0;
    }

    if (rrect.radii[1].x > radius.right)
    {
        outrect.radii[1].x = rrect.radii[1].x - radius.right;
        outrect.radii[1].y = rrect.radii[1].x - radius.right;
    }
    else
    {
        outrect.radii[1].x = 0;
        outrect.radii[1].y = 0;
    }

    if (rrect.radii[2].x > radius.bottom)
    {
        outrect.radii[2].x = rrect.radii[2].x - radius.bottom;
        outrect.radii[2].y = rrect.radii[2].x - radius.bottom;
    }
    else
    {
        outrect.radii[2].x = 0;
        outrect.radii[2].y = 0;
    }

    if (rrect.radii[3].x > radius.top)
    {
        outrect.radii[3].x = rrect.radii[3].x - radius.top;
        outrect.radii[3].y = rrect.radii[3].x - radius.top;
    }
    else
    {
        outrect.radii[3].x = 0;
        outrect.radii[3].y = 0;
    }
}

void Border::RenderChildren(Drawing* drawing)
{
    if (EnterRenderChildren(drawing))
    {
        Decorator::RenderChildren(drawing);
    }

    ExitRenderChildren(drawing);
}

bool Border::EnterRenderChildren(Drawing* drawing)
{
    fRect radius = GetCornerRadius();
    // fRect radius = SystemParameters::TransformToDevice(GetCornerRadius());

    if (!radius.IsZero())
    {
        fRRect rrect;
        fRRect outrect;
        fRect rect(Point(), GetRenderSize());
        Rect rcThickness = _thickness;
        // Rect rcThickness = SystemParameters::TransformToDevice(_thickness);

        rect.Deflate(rcThickness.left, rcThickness.top, rcThickness.right, rcThickness.bottom);
        InitRRect(rrect, radius);
        CalcInnerRRect(rrect, rcThickness, outrect);

        outrect.rect = rect;
        drawing->Save();
        drawing->ClipRound(&outrect, ClipOp::OpIntersect, true);
    }

    return GetVisibleChildrenCount() > 0;
}

void Border::ExitRenderChildren(Drawing* drawing)
{
    fRect radius = GetCornerRadius();
    if (!radius.IsZero())
    {
        drawing->Restore();
    }
}

void Border::ResetCachedBitmap()
{
    if (_cachedBitmap != NULL)
    {
        _cachedBitmap->Clear();
        InvalidateVisual();
    }
}

void Border::UpdateCachedBitmap(Drawing* drawing)
{
    // ‰÷»æ«¯”Ú
    fRect rect(Point(), GetRenderSize());
    fRect radius = GetCornerRadius();
    // fRect radius = SystemParameters::TransformToDevice(GetCornerRadius());
    Rect rcThickness = _thickness;
    // Rect rcThickness = SystemParameters::TransformToDevice(_thickness);

    //  «∑Ò”–±ﬂøÚ
    bool bHasBorder = !rcThickness.IsZero();
    bool bHasCorner = !radius.IsZero();

    fRRect rrect;

    Float fLeft = rcThickness.left;
    Float fTop = rcThickness.top;
    Float fRight = rcThickness.right;
    Float fBottom = rcThickness.bottom;

    InitRRect(rrect, radius);

    // ±ﬂøÚª≠À¢
    Brush* borderBrush = _borderBrush;
    // ±≥æ∞ª≠À¢
    Brush* backBrush = _background;

    if (backBrush != NULL)
    {
        SolidColorBrush* clrBrush = RTTIEqualCast<SolidColorBrush>(backBrush);
        if (NULL != clrBrush)
        {
            if (bHasCorner)
            {
                rrect.rect = rect;
                drawing->DrawRRect(DrawCtx::DefDraw, clrBrush, NULL, &rrect);
            }
            else
            {
                drawing->DrawRect(DrawCtx::DefDraw, backBrush, NULL, rect);
            }
        }
        else
        {
            if (bHasCorner)
            {
                // ∞¥÷∏∂®µƒ‘≤Ω«≤√ºÙ
                rrect.rect = rect;
                drawing->Save();
                drawing->ClipRound(&rrect, ClipOp::OpIntersect, true);
                drawing->DrawRect(DrawCtx::DefDraw, backBrush, NULL, rect);
                drawing->Restore();
            }
            else
            {
                drawing->DrawRect(DrawCtx::DefDraw, backBrush, NULL, rect);
            }
        }
    }

    // ªÊ÷∆±ﬂøÚ
    if (bHasBorder && borderBrush)
    {
        // ”–‘≤Ω«£¨≤¢«“±ﬂøÚ“ª÷¬
        if (bHasCorner)
        {
            // ÃÓ≥‰«¯”Úµƒ≤√ºÙ«¯”Ú
            fRect fFillClip = rect;
            fFillClip.Deflate(fLeft, fTop, fRight, fBottom);

            fRRect outrect;
            CalcInnerRRect(rrect, rcThickness, outrect);

            // œ»≤√ºÙ£¨»ª∫Û÷±Ω”ÃÓ≥‰
            rrect.rect = rect;
            outrect.rect = fFillClip;

            drawing->Save();
            drawing->ClipRound(&rrect, ClipOp::OpIntersect, true);
            drawing->ClipRound(&outrect, ClipOp::OpDifference, true);
            drawing->DrawRect(DrawCtx::DefDraw, borderBrush, NULL, rect);
            drawing->Restore();
        }
        else
        {
            // ÃÓ≥‰«¯”Úµƒ≤√ºÙ«¯”Ú
            fRect fFillClip = rect;
            fFillClip.Deflate(fLeft, fTop, fRight, fBottom);

            drawing->Save();
            drawing->ClipRect(&rect, ClipOp::OpIntersect);
            drawing->ClipRect(&fFillClip, ClipOp::OpDifference);
            drawing->DrawRect(DrawCtx::DefDraw, borderBrush, NULL, rect);
            drawing->Restore();
        }
    }
}

void Border::OnRender(Drawing * drawing)
{
    UpdateCachedBitmap(drawing);
}

}
