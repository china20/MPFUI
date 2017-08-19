// 华勤科技版权所有 2010-2011
// 
// 文件名：uiPen.h
// 功  能：定义画笔基类。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIPEN_H_
#define _UIPEN_H_

#include <Skia/core/SkPaint.h>
#include <System/Graphics/Resource.h>

namespace suic
{

class SUICORE_API SuiPaint
{
public:

    enum Style 
    {
        Fill_Style,
        Stroke_Style,
        StrokeAndFill_Style,
        StyleCount,
    };

    enum Hinting 
    {
        No_Hinting      = 0,
        Slight_Hinting  = 1,
        Normal_Hinting  = 2, 
        Full_Hinting    = 3,
    };

    enum FilterLevel 
    {
        fNone_FilterLevel,
        fLow_FilterLevel,
        fMedium_FilterLevel,
        fHigh_FilterLevel
    };

    enum Flags 
    {
        AntiAlias_Flag       = 0x01,
        FilterBitmap_Flag    = 0x02,
        Dither_Flag          = 0x04,
        UnderlineText_Flag   = 0x08,
        StrikeThruText_Flag  = 0x10,
        FakeBoldText_Flag    = 0x20,
        LinearText_Flag      = 0x40,
        SubpixelText_Flag    = 0x80,
        DevKernText_Flag     = 0x100,
        LCDRenderText_Flag   = 0x200,
        EmbeddedBitmapText_Flag = 0x400,
        AutoHinting_Flag     = 0x800, 

        kAllFlags = 0xFFF
    };

    enum Align 
    {
        Left_Align,
        Center_Align,
        Right_Align,
        AlignCount,
    };

    enum Cap 
    {
        Butt_Cap,
        Round_Cap,
        Square_Cap,
        CapCount,
        Default_Cap = Butt_Cap
    };

    enum Join 
    {
        Miter_Join,
        Round_Join,
        Bevel_Join,
        JoinCount,
        Default_Join = Miter_Join,
    };

    SuiPaint();
    ~SuiPaint();

    Style GetStyle() const;
    void SetStyle(Style style);

    SuiTypeface* GetTypeface();
    const SuiTypeface* GetTypeface() const;

    Color GetColor() const;
    void SetColor(Color color);

    Byte GetAlpha() const;
    void SetAlpha(Byte a);

    Float GetStrokeWidth() const;
    void SetStrokeWidth(Float width);

    Cap GetStrokeCap() const;
    void SetStrokeCap(Cap cap);

    Join GetStrokeJoin() const;
    void SetStrokeJoin(Join join);

    Align GetTextAlign() const;
    void SetTextAlign(Align align);

    Float GetTextSize() const;
    void SetTextSize(Float textSize);

    Float GetTextScaleX() const;
    void SetTextScaleX(Float scaleX);

    Float GetTextSkewX() const;
    void SetTextSkewX(Float skewX);

    Uint32 GetFlags() const;
    void SetFlags(Uint32 val);

    bool IsAntiAlias() const;
    void SetAntiAlias(bool val);

    bool IsDither() const;
    void SetDither(bool val);

    bool IsLCDRenderText() const;
    void SetLCDRenderText(bool val);

    bool IsEmbeddedBitmapText() const;
    void SetEmbeddedBitmapText(bool val);

    bool IsAutohinted() const;
    void SetAutohinted(bool useAutohinter);

    bool IsUnderlineText() const;
    void SetUinderlineText(bool underlineText);

    bool IsStrikeThruText() const;
    void SetStrikeThruText(bool strikeThruText);

    bool IsFakeBoldText() const;
    void SetFakeBoldText(bool fakeBoldText);

    bool IsFilterBitmap() const;
    void SetFilterBitmap(bool doFilter);

    FilterLevel GetFilterLevel() const;
    void SetFilterLevel(FilterLevel);

    Size MeasureText(const Char* text, int size) const;

protected:

    Float _textSize;
    Float _textScaleX;
    Float _textSkewX;
    Color _color;
    Float _width;
    Float _miterLimit;

    union 
    {
        struct 
        {
            Uint32 _flags : 16;
            Uint32 _textAlign : 2;
            Uint32 _capType : 2;
            Uint32 _capType : 2;
            Uint32 _style : 2;
            Uint32 _textEncoding : 2;
            Uint32 _hinting : 2;
            Uint32 _filterLevel : 2;
        };

        Uint32 _bitfields;
    };

    SuiTypeface  _typeface;
};

inline SuiPaint::Style SuiPaint::GetStyle() const
{
    return (SuiPaint::Style)_style;
}

inline void SuiPaint::SetFilterBitmap(bool doFilter) 
{
    SetFilterLevel(doFilter ? fLow_FilterLevel : fNone_FilterLevel);
}

inline SuiPaint::FilterLevel SuiPaint::GetFilterLevel() const
{
    return (SuiPaint::FilterLevel)_filterLevel;
}

inline void SuiPaint::SetFilterLevel(SuiPaint::FilterLevel val)
{
    _filterLevel = val;
}

inline SuiTypeface* SuiPaint::GetTypeface()
{
    return &_typeface;
}

inline const SuiTypeface* SuiPaint::GetTypeface() const
{
    return &_typeface;
}

inline Color SuiPaint::GetColor() const
{
    return _color;
}

inline Byte SuiPaint::GetAlpha() const
{
    return ((_color & 0xFF000000) >> 24);
}

inline Float SuiPaint::GetStrokeWidth() const
{
    return _width;
}

inline SuiPaint::Cap SuiPaint::GetStrokeCap() const
{
    return (SuiPaint::Cap)_capType;
}

inline SuiPaint::Join SuiPaint::GetStrokeJoin() const
{
    return (SuiPaint::Join)_joinType;
}

inline SuiPaint::Align SuiPaint::GetTextAlign() const
{
    return (SuiPaint::Align)_textAlign;
}

inline Float SuiPaint::GetTextSize() const
{
    return _textSize;
}

inline Float SuiPaint::GetTextScaleX() const
{
    return _textScaleX;
}

inline Float SuiPaint::GetTextSkewX() const
{
    return _textSkewX;
}

inline Uint32 SuiPaint::GetFlags() const
{
    return _flags;
}

inline bool SuiPaint::IsAntiAlias() const
{
    return (GetFlags() & SuiPaint::Flags::AntiAlias_Flag) != 0;
}

inline bool SuiPaint::IsDither() const
{
    return (GetFlags() & SuiPaint::Flags::Dither_Flag) != 0;
}

inline bool SuiPaint::IsLCDRenderText() const
{
    return (GetFlags() & SuiPaint::Flags::AntiAlias_Flag) != 0;
}

inline bool SuiPaint::IsEmbeddedBitmapText() const
{
    return (GetFlags() & SuiPaint::Flags::EmbeddedBitmapText_Flag) != 0;
}

inline bool SuiPaint::IsAutohinted() const
{
    return (GetFlags() & SuiPaint::Flags::AutoHinting_Flag) != 0;
}

inline bool SuiPaint::IsUnderlineText() const
{
    return (GetFlags() & SuiPaint::Flags::UnderlineText_Flag) != 0;
}

inline bool SuiPaint::IsStrikeThruText() const
{
    return (GetFlags() & SuiPaint::Flags::StrikeThruText_Flag) != 0;
}

inline bool SuiPaint::IsFakeBoldText() const
{
    return (GetFlags() & SuiPaint::Flags::FakeBoldText_Flag) != 0;
}

inline bool SuiPaint::IsFilterBitmap() const
{
    return (GetFlags() & SuiPaint::Flags::FilterBitmap_Flag) != 0;
}

}

#endif
