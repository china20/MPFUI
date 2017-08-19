// 华勤科技版权所有 2010-2011
// 
// 文件名：FormattedText.h
// 功  能：定义文本绘制参数。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIFORMATTEDTEXT_H_
#define _UIFORMATTEDTEXT_H_

#include <System/Tools/Array.h>
#include <System/Graphics/Brush.h>

namespace suic
{

struct SUICORE_API Typeface
{
    String fontFamily;
    Uint16 textStyle;
    Uint16 fontStyle;
    Uint16 fontWeight;

    Typeface();

    bool GetTextStyle(Uint16 key) const;
    void SetTextStyle(Uint16 key, bool val);
};

enum FontWeightStyle
{
    fwExtraBlack = 950,
    fwBlack      = 900,
    fwExtraBold  = 800,
    fwBold       = 700,
    fwSemiBold   = 600,
    fwMedium     = 500,
    fwNormal     = 400,
    fwLight      = 300,
    fwExtraLight = 200,
    fwThin       = 100,
};

enum FontStretchStyle
{
    fsUltraCondensed = 1,
    fsExtraCondensed,
    fsCondensed,
    fsSemiCondensed,
    fsNormal,
    fsSemiExpanded,
    fsExpanded,
    fsExtraExpanded,
    fsUltraExpanded,
};

enum FontStyleStyle
{
    fssNormal = 1,
    fssOblique = 2,
    fssItalic = 4,
    fssUnderline = 8,
};

enum FormattedTextStyle
{
    ftsNone = 0,
    ftsUnderline = 1,
    ftsBaseline = 2,
    ftsOverline = 4,
    ftsStrikeout = 8,
};

class SUICORE_API FormattedText
{
public:

    FormattedText();
    ~FormattedText();

    String GetFontFamily() const;
    void SetFontFamily(const String& name);

    Uint16 GetFontStyle() const;
    void SetFontStyle(Uint16 val);

    Uint16 GetFontWeight() const;
    void SetFontWeight(Uint16 val);

    Uint16 GetFontSize() const;
    void SetFontSize(Uint16 size);

    bool GetSingleline() const;
    void SetSingleline(bool val);

    bool GetStrikeout() const;
    void SetStrikeout(bool val);

    bool GetUnderline() const;
    void SetUnderline(bool val);

    Typeface* GetTypeface();

    Color GetColor();
    void SetColor(Color clr);

    Brush* GetForeground() const;
    void SetForeground(Brush* brush);

    TextTrimming GetTextTrimming() const;
    void SetTextTrimming(TextTrimming val);

    void MeasureString(TmParam& tm, const Char* text, int len);

private:

    String _text;

    Uint16 _fontSize;
    bool _singleLine;
    Typeface _typeface;
    Brush* _foreground;
    FormattedTextStyle _textStyle;

    TextTrimming _trimming;
};

inline Typeface* FormattedText::GetTypeface()
{
    return &_typeface;
}

struct SUICORE_API DashStyle
{
    Float offset;
    FloatAutoArray dashes;

    DashStyle();
};

class SUICORE_API Pen
{
public:

    Pen();
    ~Pen();

    PenLineCap GetDashCap() const;
    void SetDashCap(PenLineCap val);

    PenLineCap GetStartLineCap() const;
    void SetStartLineCap(PenLineCap val);

    PenLineCap GetEndLineCap() const;
    void SetEndLineCap(PenLineCap val);

    DashStyle* GetDashStyle();

    PenLineJoin GetLineJoin() const;
    void SetLineJoin(PenLineJoin val);

    Float GetMiterLimit() const;
    void SetMiterLimit(Float val);

    Float GetThickness() const;
    void SetThickness(Float val);

    Brush* GetBrush() const;
    void SetBrush(Brush* val);

    bool GetAntiAlias() const;
    void SetAntiAlias(bool val);

private:

    PenLineCap _dashCap;
    PenLineCap _endLineCap;
    PenLineCap _startLineCap;
    DashStyle _dashStyle;
    PenLineJoin _lineJoin;

    Float _miterLimit;
    Float _thickness;

    bool _antiAlias;

    Brush* _brush;
};

inline bool Pen::GetAntiAlias() const
{
    return _antiAlias;
}

inline void Pen::SetAntiAlias(bool val)
{
    _antiAlias = val;
}

}

#endif
