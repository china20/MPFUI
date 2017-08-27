// 华勤科技版权所有 2010-2011
// 
// 文件名：Colors.h
// 功  能：定义核心库的颜色常量。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UICOLORS_H_
#define _UICOLORS_H_

#include <System/Types/Types.h>

namespace suic
{

struct Color
{
    Uint32 _color;

    Color()
    {
        _color = 0xFFFFFFFF;
    }

    Color(Uint32 clr)
        : _color(clr)
    {

    }

    Color(const Color& Other)
        : _color(Other._color)
    {

    }

    operator Uint32()
    {
        return _color;
    }

    bool operator <(const Color& Other)
    {
        return _color < Other._color;
    }

    bool operator >(const Color& Other)
    {
        return _color > Other._color;
    }

    bool operator ==(const Color& Other)
    {
        return _color == Other._color;
    }

    Color operator=(const Color& Other)
    {
        _color = Other._color;
        return *this;
    }
};

struct Colors
{
    static const Color AliceBlue = 0xFFF0F8FF;
    static const Color AntiqueWhite = 0xFFFAEBD7;
    static const Color Aqua = 0xFF00FFFF;
    static const Color Aquamarine = 0xFF7FFFD4;
    static const Color Azure = 0xFFF0FFFF;
    static const Color Beige = 0xFFF5F5DC;
    static const Color Bisque = 0xFFFFE4C4;
    static const Color Black = 0xFF000000;
    static const Color BlanchedAlmond = 0xFFFFEBCD;
    static const Color Blue = 0xFF0000FF;
    static const Color BlueViolet = 0xFF8A2BE2;
    static const Color Brown = 0xFFA52A2A;
    static const Color BurlyWood = 0xFFDEB887;
    static const Color CadetBlue = 0xFF5F9EA0;
    static const Color Chartreuse = 0xFF7FFF00;
    static const Color Chocolate = 0xFFD2691E;
    static const Color Coral = 0xFFFF7F50;
    static const Color CornflowerBlue = 0xFF6495ED;
    static const Color Cornsilk = 0xFFFFF8DC;
    static const Color Crimson = 0xFFDC143C;
    static const Color Cyan = 0xFF00FFFF;
    static const Color DarkBlue = 0xFF00008B;
    static const Color DarkCyan = 0xFF008B8B;
    static const Color DarkGoldenrod = 0xFFB8860B;
    static const Color DarkGray = 0xFFA9A9A9;
    static const Color DarkGreen = 0xFF006400;
    static const Color DarkKhaki = 0xFFBDB76B;
    static const Color DarkMagenta = 0xFF8B008B;
    static const Color DarkOliveGreen = 0xFF556B2F;
    static const Color DarkOrange = 0xFFFF8C00;
    static const Color DarkOrchid = 0xFF9932CC;
    static const Color DarkRed = 0xFF8B0000;
    static const Color DarkSalmon = 0xFFE9967A;
    static const Color DarkSeaGreen = 0xFF8FBC8F;
    static const Color DarkSlateBlue = 0xFF483D8B;
    static const Color DarkSlateGray = 0xFF2F4F4F;
    static const Color DarkTurquoise = 0xFF00CED1;
    static const Color DarkViolet = 0xFF9400D3;
    static const Color DeepPink = 0xFFFF1493;
    static const Color DeepSkyBlue = 0xFF00BFFF;
    static const Color DimGray = 0xFF696969;
    static const Color DodgerBlue = 0xFF1E90FF;
    static const Color Firebrick = 0xFFB22222;
    static const Color FloralWhite = 0xFFFFFAF0;
    static const Color ForestGreen = 0xFF228B22;
    static const Color Fuchsia = 0xFFFF00FF;
    static const Color Gainsboro = 0xFFDCDCDC;
    static const Color GhostWhite = 0xFFF8F8FF;
    static const Color Gold = 0xFFFFD700;
    static const Color Goldenrod = 0xFFDAA520;
    static const Color Gray = 0xFF808080;
    static const Color Green = 0xFF008000;
    static const Color GreenYellow = 0xFFADFF2F;
    static const Color Honeydew = 0xFFF0FFF0;
    static const Color HotPink = 0xFFFF69B4;
    static const Color IndianRed = 0xFFCD5C5C;
    static const Color Indigo = 0xFF4B0082;
    static const Color Ivory = 0xFFFFFFF0;
    static const Color Khaki = 0xFFF0E68C;
    static const Color Lavender = 0xFFE6E6FA;
    static const Color LavenderBlush = 0xFFFFF0F5;
    static const Color LawnGreen = 0xFF7CFC00;
    static const Color LemonChiffon = 0xFFFFFACD;
    static const Color LightBlue = 0xFFADD8E6;
    static const Color LightCoral = 0xFFF08080;
    static const Color LightCyan = 0xFFE0FFFF;
    static const Color LightGoldenrodYellow = 0xFFFAFAD2;
    static const Color LightGreen = 0xFF90EE90;
    static const Color LightGray = 0xFFD3D3D3;
    static const Color LightPink = 0xFFFFB6C1;
    static const Color LightSalmon = 0xFFFFA07A;
    static const Color LightSeaGreen = 0xFF20B2AA;
    static const Color LightSkyBlue = 0xFF87CEFA;
    static const Color LightSlateGray = 0xFF778899;
    static const Color LightSteelBlue = 0xFFB0C4DE;
    static const Color LightYellow = 0xFFFFFFE0;
    static const Color Lime = 0xFF00FF00;
    static const Color LimeGreen = 0xFF32CD32;
    static const Color Linen = 0xFFFAF0E6;
    static const Color Magenta = 0xFFFF00FF;
    static const Color Maroon = 0xFF800000;
    static const Color MediumAquamarine = 0xFF66CDAA;
    static const Color MediumBlue = 0xFF0000CD;
    static const Color MediumOrchid = 0xFFBA55D3;
    static const Color MediumPurple = 0xFF9370DB;
    static const Color MediumSeaGreen = 0xFF3CB371;
    static const Color MediumSlateBlue = 0xFF7B68EE;
    static const Color MediumSpringGreen = 0xFF00FA9A;
    static const Color MediumTurquoise = 0xFF48D1CC;
    static const Color MediumVioletRed = 0xFFC71585;
    static const Color MidnightBlue = 0xFF191970;
    static const Color MintCream = 0xFFF5FFFA;
    static const Color MistyRose = 0xFFFFE4E1;
    static const Color Moccasin = 0xFFFFE4B5;
    static const Color NavajoWhite = 0xFFFFDEAD;
    static const Color Navy = 0xFF000080;
    static const Color OldLace = 0xFFFDF5E6;
    static const Color Olive = 0xFF808000;
    static const Color OliveDrab = 0xFF6B8E23;
    static const Color Orange = 0xFFFFA500;
    static const Color OrangeRed = 0xFFFF4500;
    static const Color Orchid = 0xFFDA70D6;
    static const Color PaleGoldenrod = 0xFFEEE8AA;
    static const Color PaleGreen = 0xFF98FB98;
    static const Color PaleTurquoise = 0xFFAFEEEE;
    static const Color PaleVioletRed = 0xFFDB7093;
    static const Color PapayaWhip = 0xFFFFEFD5;
    static const Color PeachPuff = 0xFFFFDAB9;
    static const Color Peru = 0xFFCD853F;
    static const Color Pink = 0xFFFFC0CB;
    static const Color Plum = 0xFFDDA0DD;
    static const Color PowderBlue = 0xFFB0E0E6;
    static const Color Purple = 0xFF800080;
    static const Color Red = 0xFFFF0000;
    static const Color RosyBrown = 0xFFBC8F8F;
    static const Color RoyalBlue = 0xFF4169E1;
    static const Color SaddleBrown = 0xFF8B4513;
    static const Color Salmon = 0xFFFA8072;
    static const Color SandyBrown = 0xFFF4A460;
    static const Color SeaGreen = 0xFF2E8B57;
    static const Color SeaShell = 0xFFFFF5EE;
    static const Color Sienna = 0xFFA0522D;
    static const Color Silver = 0xFFC0C0C0;
    static const Color SkyBlue = 0xFF87CEEB;
    static const Color SlateBlue = 0xFF6A5ACD;
    static const Color SlateGray = 0xFF708090;
    static const Color Snow = 0xFFFFFAFA;
    static const Color SpringGreen = 0xFF00FF7F;
    static const Color SteelBlue = 0xFF4682B4;
    static const Color Tan = 0xFFD2B48C;
    static const Color Teal = 0xFF008080;
    static const Color Thistle = 0xFFD8BFD8;
    static const Color Tomato = 0xFFFF6347;
    static const Color Turquoise = 0xFF40E0D0;
    static const Color Violet = 0xFFEE82EE;
    static const Color Wheat = 0xFFF5DEB3;
    static const Color White = 0xFFFFFFFF;
    static const Color WhiteSmoke = 0xFFF5F5F5;
    static const Color Yellow = 0xFFFFFF00;
    static const Color YellowGreen = 0xFF9ACD32;
};

}

#endif
