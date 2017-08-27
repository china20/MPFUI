// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// SuiPaint.cpp

#include <System/Graphics/SuiPaint.h>
#include <System/Tools/Map.h>
#include <Skia/core/SkPaint.h>
#include <Skia/core/SkTypeface.h>
#include <System/Render/Skia/SkTextOp.h>

namespace suic
{

SuiPaint::SuiPaint()
    : _color(0xFF000000)
    , _width(1)
    , _textSize(12)
    , _textScaleX(1)
    , _textSkewX(1)
    , _data(NULL)
{
    //fTextSize     = SkPaintDefaults_TextSize;
    //fTextScaleX   = SK_Scalar1;
    //fTextSkewX    = 0;
    //fColor        = SK_ColorBLACK;
    //fWidth        = 0;
    //fMiterLimit   = SkPaintDefaults_MiterLimit;

    //// Zero all bitfields, then set some non-zero defaults.
    //fBitfields    = 0;
    //fFlags        = SkPaintDefaults_Flags;
    //fCapType      = kDefault_Cap;
    //fJoinType     = kDefault_Join;
    //fTextAlign    = kLeft_Align;
    //fStyle        = kFill_Style;
    //fTextEncoding = kUTF8_TextEncoding;
    //fHinting      = SkPaintDefaults_Hinting;

    //fDirtyBits    = 0;
}

SuiPaint::~SuiPaint()
{
    SkPaint* paint = (SkPaint*)_data;
    delete paint;
}

void SuiPaint::SetStyle(Style style)
{
    _style = style;
}

void SuiPaint::SetColor(Color clr)
{
    _color = clr;
}

void SuiPaint::SetAlpha(Byte a)
{
    ;
}

void SuiPaint::SetStrokeWidth(Float width)
{
    _width = width;
}

void SuiPaint::SetStrokeCap(Cap cap)
{
    _capType = cap;
}

void SuiPaint::SetStrokeJoin(Join join)
{
    _joinType = join;
}

void SuiPaint::SetTextAlign(Align align)
{
    _textAlign = align;
}

void SuiPaint::SetTextSize(Float textSize)
{
    _textSize = textSize;
}

void SuiPaint::SetTextScaleX(Float scaleX)
{
    _textScaleX = scaleX;
}

void SuiPaint::SetTextSkewX(Float skewX)
{
    _textSkewX = skewX;
}

void SuiPaint::SetFlags(Uint32 val)
{
    _flags = val;
}

void SuiPaint::SetAntiAlias(bool val)
{
    if (val)
    {
        _flags |= SuiPaint::Flags::AntiAlias_Flag;
    }
    else
    {
        _flags &= ~SuiPaint::Flags::AntiAlias_Flag;
    }
}

void SuiPaint::SetDither(bool val)
{
    if (val)
    {
        _flags |= SuiPaint::Flags::Dither_Flag;
    }
    else
    {
        _flags &= ~SuiPaint::Flags::Dither_Flag;
    }
}

void SuiPaint::SetLCDRenderText(bool val)
{
    if (val)
    {
        _flags |= SuiPaint::Flags::LCDRenderText_Flag;
    }
    else
    {
        _flags &= ~SuiPaint::Flags::LCDRenderText_Flag;
    }
}

void SuiPaint::SetEmbeddedBitmapText(bool val)
{
    if (val)
    {
        _flags |= SuiPaint::Flags::EmbeddedBitmapText_Flag;
    }
    else
    {
        _flags &= ~SuiPaint::Flags::EmbeddedBitmapText_Flag;
    }
}

void SuiPaint::SetAutohinted(bool val)
{
    if (val)
    {
        _flags |= SuiPaint::Flags::AutoHinting_Flag;
    }
    else
    {
        _flags &= ~SuiPaint::Flags::AutoHinting_Flag;
    }
}

void SuiPaint::SetUinderlineText(bool val)
{
    if (val)
    {
        _flags |= SuiPaint::Flags::UnderlineText_Flag;
    }
    else
    {
        _flags &= ~SuiPaint::Flags::UnderlineText_Flag;
    }
}

void SuiPaint::SetStrikeThruText(bool val)
{
    if (val)
    {
        _flags |= SuiPaint::Flags::StrikeThruText_Flag;
    }
    else
    {
        _flags &= ~SuiPaint::Flags::StrikeThruText_Flag;
    }
}

void SuiPaint::SetFakeBoldText(bool val)
{
    if (val)
    {
        _flags |= SuiPaint::Flags::FakeBoldText_Flag;
    }
    else
    {
        _flags &= ~SuiPaint::Flags::FakeBoldText_Flag;
    }
}

void SuiPaint::SetFilterBitmap(bool val)
{
    if (val)
    {
        _flags |= SuiPaint::Flags::FilterBitmap_Flag;
    }
    else
    {
        _flags &= ~SuiPaint::Flags::FilterBitmap_Flag;
    }
}

Size SuiPaint::MeasureText(const Char* text, int size) const
{
    Size sizeRet;
    SkPaint paint;
    SkTypeface* skface = (SkTypeface*)GetTypeface()->GetData();

    paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
    paint.setTypeface(skface);
    paint.setTextSize(GetTextSize());
    paint.setAutohinted(true);

    SkPaint::FontMetrics fm;

    sizeRet.cy = (int)(paint.getFontMetrics(&fm) + 0.4);
    sizeRet.cx = (int)(paint.measureText(text, size * 2) + 0.4);

    return sizeRet;
}

}
