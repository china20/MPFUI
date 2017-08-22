// 华勤科技版权所有 2010-2011
// 
// 文件名：DrawInfo.h
// 功  能：定义内部绘制信息。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIDRAWINFO_H_
#define _UIDRAWINFO_H_

#include <System/Graphics/FormattedText.h>

namespace suic
{

class SUICORE_API DrawMeta 
{
public:

    virtual ~DrawMeta() {} 
};

class SUICORE_API DrawInfo
{
public:

    enum WrapFlag 
    {
        SingleLine_Flag = 0x01,
        WrapText_Flag = 0x02,
        WrapFlagCount,
    };

    enum Style 
    {
        Fill_Style,
        Stroke_Style,
        StrokeAndFill_Style,
        StyleCount,
    };

    enum FontStyle 
    {
        fNormal = 0,
        fBold   = 0x01,
        fItalic = 0x02,
        fBoldItalic = 0x03,
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
        Near_Align,
        Center_Align,
        Far_Align,
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

    DrawInfo();
    ~DrawInfo();

    DrawMeta* GetDrawMeta() const;

    bool GetWrapText() const;
    void SetWrapText(bool val);

    bool GetSingleLine() const;
    void SetSingleLine(bool val);

    Style GetStyle() const;
    void SetStyle(Style style);

    void SetFont(String name, FontStyle style);
    
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

    Align GetLineTextAlign() const;
    void SetLineTextAlign(Align align);

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
    void SetUnderlineText(bool underlineText);

    void SetStrikeThruText(bool val);
    void SetFakeBoldText(bool val);
    void SetFilterBitmap(bool val);

    FilterLevel GetFilterLevel() const;
    void SetFilterLevel(FilterLevel val);

    Brush* GetBrush() const;
    void SetBrush(Brush* val);

    Size MeasureString(const Char* text, int count);
    void MeasureString(TmParam& tm, const Char* text, int count);

private:

    DrawMeta* _meta;

    Byte _flag;
    Brush* _brush;

    Align _lineTextAlign;
};

inline DrawMeta* DrawInfo::GetDrawMeta() const
{
    return _meta;
}

inline bool DrawInfo::GetWrapText() const
{
    return (WrapFlag::WrapText_Flag == (_flag & WrapFlag::WrapText_Flag));
}

inline bool DrawInfo::GetSingleLine() const
{
    return (WrapFlag::SingleLine_Flag == (_flag & WrapFlag::SingleLine_Flag));
}

inline Brush* DrawInfo::GetBrush() const
{
    return _brush;
}

inline void DrawInfo::SetBrush(Brush* val)
{
    _brush = val;
}

}

#endif
