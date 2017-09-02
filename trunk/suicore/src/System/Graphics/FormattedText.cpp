
#include <System/Graphics/FormattedText.h>
#include <System/Render/Skia/SkTextOp.h>
#include <Skia/effects/SkGradientShader.h>
#include <Skia/core/SkTypeface.h>
#include <System/Tools/CoreHelper.h>
#include <System/Graphics/SolidColorBrush.h>

namespace suic
{

Typeface::Typeface()
{
    fontWeight = 0;
    fontStyle = 0;
    textStyle = 0;
    fontFamily = CoreHelper::GetDefaultFontFamily();
}

bool Typeface::GetTextStyle(Uint16 key) const
{
    return (key & textStyle) == key;
}

void Typeface::SetTextStyle(Uint16 key, bool val)
{
    if (val)
    {
        textStyle |= key;
    }
    else
    {
        textStyle &= ~key;
    }
}

DrawCtx::DrawCtx()
    : _alpha(255)
    , _antiAlias(true)
    , _level(DrawCtx::eDrawLevel::flLow)
{
}

DrawCtx::DrawCtx(eDrawLevel l, bool alias)
    : _alpha(255)
    , _antiAlias(true)
    , _level(l)
{
}

DrawCtx::DrawCtx(eDrawLevel l, bool alias, Byte a)
    : _alpha(a)
    , _antiAlias(alias)
    , _level(l)
{
}

DrawCtx::~DrawCtx()
{
}

DrawCtx* DrawCtx::DefDraw = new DrawCtx();

FormattedText::FormattedText()
    : _singleLine(true)
    , _foreground(SolidColorBrush::Black)
    , _trimming(TextTrimming::tNone)
{
    _fontSize = CoreHelper::GetDefaultFontSize();
    _foreground->ref();
}

FormattedText::~FormattedText()
{
    _foreground->unref();
}

String FormattedText::GetFontFamily() const
{
    return _typeface.fontFamily;
}

void FormattedText::SetFontFamily(const String& name)
{
    _typeface.fontFamily = name;
}

Uint16 FormattedText::GetFontStyle() const
{
    return _typeface.fontStyle;
}

void FormattedText::SetFontStyle(Uint16 val)
{
    _typeface.fontStyle = val;
}

Uint16 FormattedText::GetFontWeight() const
{
    return _typeface.fontWeight;
}

void FormattedText::SetFontWeight(Uint16 val)
{
    _typeface.fontWeight = val;
}

Uint16 FormattedText::GetFontSize() const
{
    return _fontSize;
}

void FormattedText::SetFontSize(Uint16 size)
{
    _fontSize = size;
}

bool FormattedText::GetSingleline() const
{
    return _singleLine;
}

void FormattedText::SetSingleline(bool val)
{
    _singleLine = val;
}

bool FormattedText::GetStrikeout() const
{
    return _typeface.GetTextStyle(FormattedTextStyle::ftsStrikeout);
}

void FormattedText::SetStrikeout(bool val)
{
    _typeface.SetTextStyle(FormattedTextStyle::ftsStrikeout, val);
}

bool FormattedText::GetUnderline() const
{
    return _typeface.GetTextStyle(FormattedTextStyle::ftsUnderline);
}

void FormattedText::SetUnderline(bool val)
{
    _typeface.SetTextStyle(FormattedTextStyle::ftsUnderline, val);
}

Color FormattedText::GetColor()
{
    SolidColorBrush* brush = RTTICast<SolidColorBrush>(_foreground);
    if (NULL != brush)
    {
        return brush->GetColor();
    }
    else
    {
        return Colors::Black;
    }
}

void FormattedText::SetColor(Color clr)
{
    _foreground->unref();
    _foreground = new SolidColorBrush(clr);
    _foreground->ref();
}

Brush* FormattedText::GetForeground() const
{
    return _foreground;
}

void FormattedText::SetForeground(Brush* brush)
{
    _foreground->unref();

    if (NULL == brush)
    {
        _foreground = SolidColorBrush::Black;
    }
    else
    {
        _foreground = brush;
    }

    //座位号：1 13603047547 谢老师  B栋 进大门左手边 带身份证和数码回执

    _foreground->ref();
}

TextTrimming FormattedText::GetTextTrimming() const
{
    return _trimming;
}

void FormattedText::SetTextTrimming(TextTrimming val)
{
    _trimming = val;
}

void FormattedText::MeasureString(TmParam& tm, const Char* text, int len)
{
    SkPaint::FontMetrics fm;
    SkPaint paint;
    int iLines = 1;
    SkScalar outlen = 0;
    SkTypeface* skface = NULL;
    Mulstr fName(GetFontFamily().c_str());
    int iSize = (int)CoreHelper::FontPoundToPixel(GetFontSize());

    const int textFlag = SkPaint::kGenA8FromLCD_Flag | SkPaint::Flags::kAntiAlias_Flag
        | /*SkPaint::Flags::kDistanceFieldTextTEMP_Flag | */SkPaint::Flags::kEmbeddedBitmapText_Flag;

    if (GetFontWeight() >= FontWeightStyle::fwBold)
    {
        skface = SkTypeface::CreateFromName(fName.c_str(), SkTypeface::Style::kBold);
    }
    else
    {
        skface = SkTypeface::CreateFromName(fName.c_str(), SkTypeface::Style::kNormal);
    }

    paint.setTypeface(skface);
    paint.setTextSize(iSize);
    paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
    
    paint.setFlags(textFlag);
    //paint.setSubpixelText(true);
    //paint.setAutohinted(true);
    paint.setHinting(SkPaint::Hinting::kNormal_Hinting);

    paint.getFontMetrics(&fm);

    if (tm.constaint.cx > 0)
    {
        if (GetSingleline())
        {
            tm.charCount = SkTextOp::ComputeTextLines(paint, outlen, tm.constaint.cx, text, len);
            tm.size.cx = outlen;
        }
        else
        {
            iLines = SkTextOp::ComputeWrapTextLineCount(paint, outlen, tm.constaint.cx, text, len);

            if (iLines == 1)
            {
                tm.size.cx = paint.measureText(text, len * 2);
            }
            else
            {
                tm.size.cx = outlen;
            }

            tm.charCount = len;
        }
    }
    else
    {
        tm.charCount = len;
        tm.size.cx = paint.measureText(text, len * 2);
    }

    tm.size.cy = CalcLineSpace(fm) * iLines;
}

DashStyle::DashStyle()
{
    offset = 1;
}

Pen::Pen()
    : _dashCap(PenLineCap::plcFlat)
    , _endLineCap(PenLineCap::plcFlat)
    , _startLineCap(PenLineCap::plcFlat)
    , _lineJoin(PenLineJoin::pljMiter)
    , _miterLimit(0)
    , _thickness(1)
    , _antiAlias(true)
    , _brush(SolidColorBrush::Black)
{
    _brush->ref();
}

Pen::~Pen()
{
    _brush->unref();
}

PenLineCap Pen::GetDashCap() const
{
    return _dashCap;
}

void Pen::SetDashCap(PenLineCap val)
{
    _dashCap = val;
}

PenLineCap Pen::GetStartLineCap() const
{
    return _startLineCap;
}

void Pen::SetStartLineCap(PenLineCap val)
{
    _startLineCap = val;
}

PenLineCap Pen::GetEndLineCap() const
{
    return _endLineCap;
}

void Pen::SetEndLineCap(PenLineCap val)
{
    _endLineCap = val;
}

DashStyle* Pen::GetDashStyle()
{
    return &_dashStyle;
}

PenLineJoin Pen::GetLineJoin() const
{
    return _lineJoin;
}

void Pen::SetLineJoin(PenLineJoin val)
{
    _lineJoin = val;
}

Float Pen::GetMiterLimit() const
{
    return _miterLimit;
}

void Pen::SetMiterLimit(Float val)
{
    _miterLimit = val;
}

Float Pen::GetThickness() const
{
    return _thickness;
}

void Pen::SetThickness(Float val)
{
    _thickness = val;
}

Brush* Pen::GetBrush() const
{
    return _brush;
}

void Pen::SetBrush(Brush* val)
{
    _brush->unref();

    if (val != NULL)
    {
        _brush = val;
    }
    else
    {
        _brush = SolidColorBrush::Black;
    }
    _brush->ref();
}

}

