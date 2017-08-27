//////////////////////////////////////////////////////////////////////////////
// DrawInfo.cpp

#include <System/Graphics/DrawInfo.h>
#include <System/Graphics/Drawing.h>
#include <System/Graphics/SkiaPaint.h>

#include <Skia/core/SkPath.h>
#include <Skia/core/SkPaint.h>
#include <Skia/core/SkRRect.h>

//void *__CRTDECL operator new(size_t size)
//{
//    return malloc(size);
//}
//
//void operator delete(void* ptr)
//{
//    free(ptr);
//}

namespace suic
{

TmParam::TmParam()
    : charCount(0)
    , tmFlag(0)
{

}

DrawInfo::DrawInfo()
    : _brush(NULL)
    , _lineTextAlign(Align::Near_Align)
{
    _meta = new SkDrawMeta();
}

DrawInfo::~DrawInfo()
{
    SkDrawMeta* skMeta = dynamic_cast<SkDrawMeta*>(_meta);
    if (skMeta)
    {
        delete skMeta;
    }
}

void DrawInfo::SetWrapText(bool val)
{
    if (val)
    {
        _flag |= WrapText_Flag;
    }
    else
    {
        _flag &= ~WrapText_Flag;
    }
}

void DrawInfo::SetSingleLine(bool val)
{
    if (val)
    {
        _flag |= SingleLine_Flag;
    }
    else
    {
        _flag &= ~SingleLine_Flag;
    }
}

DrawInfo::Style DrawInfo::GetStyle() const
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    return DrawInfo::Style(meta->GetPaint().getStyle());
}

void DrawInfo::SetStyle(Style val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setStyle(SkPaint::Style(val));
}

void DrawInfo::SetFont(String name, FontStyle style)
{
    Mulstr fName(name.c_str());
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    SkTypeface* skface = SkTypeface::CreateFromName(fName.c_str(), SkTypeface::Style(style));

    meta->GetPaint().setTypeface(skface);
}

Color DrawInfo::GetColor() const
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    return meta->GetPaint().getColor();
}

void DrawInfo::SetColor(Color clr)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setColor(clr);
}

void DrawInfo::SetAlpha(Byte a)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setAlpha(a);
}

void DrawInfo::SetStrokeWidth(Float width)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setStrokeWidth(width);
}

void DrawInfo::SetStrokeCap(Cap cap)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setStrokeCap(SkPaint::Cap(cap));
}

void DrawInfo::SetStrokeJoin(Join val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setStrokeJoin(SkPaint::Join(val));
}

DrawInfo::Align DrawInfo::GetTextAlign() const
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    return DrawInfo::Align(meta->GetPaint().getTextAlign());
}

void DrawInfo::SetTextAlign(Align align)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setTextAlign(SkPaint::Align(align));
}

DrawInfo::Align DrawInfo::GetLineTextAlign() const
{
    return _lineTextAlign;
}

void DrawInfo::SetLineTextAlign(Align align)
{
    _lineTextAlign = align;
}

void DrawInfo::SetTextSize(Float textSize)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setTextSize(textSize);
}

void DrawInfo::SetTextScaleX(Float scaleX)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setTextScaleX(scaleX);
}

void DrawInfo::SetTextSkewX(Float skewX)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setTextSkewX(skewX);
}

void DrawInfo::SetFlags(Uint32 val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setFlags(val);
}

void DrawInfo::SetAntiAlias(bool val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setAntiAlias(val);
}

void DrawInfo::SetDither(bool val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setDither(val);
}

void DrawInfo::SetLCDRenderText(bool val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setLCDRenderText(val);
}

void DrawInfo::SetEmbeddedBitmapText(bool val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setEmbeddedBitmapText(val);
}

void DrawInfo::SetAutohinted(bool val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setAutohinted(val);
}

void DrawInfo::SetUnderlineText(bool val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setUnderlineText(val);
}

void DrawInfo::SetStrikeThruText(bool val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setStrikeThruText(val);
}

void DrawInfo::SetFakeBoldText(bool val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setFakeBoldText(val);
}

void DrawInfo::SetFilterBitmap(bool val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setFilterBitmap(val);
}

DrawInfo::FilterLevel DrawInfo::GetFilterLevel() const
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    return DrawInfo::FilterLevel(meta->GetPaint().getFilterLevel());
}

void DrawInfo::SetFilterLevel(FilterLevel val)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;
    meta->GetPaint().setFilterLevel(SkPaint::FilterLevel(val));
}

Size DrawInfo::MeasureString(const Char* text, int count)
{
    TmParam tm;

    MeasureString(tm, text, count);

    return tm.size;
}

void DrawInfo::MeasureString(TmParam& tm, const Char* text, int count)
{
    SkDrawMeta* meta = (SkDrawMeta*)_meta;

    tm.charCount = 0;

    if (tm.constaint.cx > 0)
    {
        if (GetSingleLine())
        {
            tm.size = meta->MeasureText(tm.constaint.cx, text, count, tm.charCount);
        }
        else
        {
            tm.charCount = count;
            tm.size = meta->MeasureText(tm.constaint.cx, text, count);
        }
    }
    else
    {
        tm.size = meta->MeasureText(text, count);
    }
}

}
