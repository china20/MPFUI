// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Colors.cpp

#include <System/Types/Colors.h>
#include <System/Types/Micro.h>
#include <System/Types/String.h>
#include <System/Interop/ColorManager.h>

namespace suic
{

Color Colors::AliceBlue = 0xFFF0F8FF;
Color Colors::AntiqueWhite = 0xFFFAEBD7;
Color Colors::Aqua = 0xFF00FFFF;
Color Colors::Aquamarine = 0xFF7FFFD4;
Color Colors::Azure = 0xFFF0FFFF;
Color Colors::Beige = 0xFFF5F5DC;
Color Colors::Bisque = 0xFFFFE4C4;
Color Colors::Black = 0xFF000000;
Color Colors::BlanchedAlmond = 0xFFFFEBCD;
Color Colors::Blue = 0xFF0000FF;
Color Colors::BlueViolet = 0xFF8A2BE2;
Color Colors::Brown = 0xFFA52A2A;
Color Colors::BurlyWood = 0xFFDEB887;
Color Colors::CadetBlue = 0xFF5F9EA0;
Color Colors::Chartreuse = 0xFF7FFF00;
Color Colors::Chocolate = 0xFFD2691E;
Color Colors::Coral = 0xFFFF7F50;
Color Colors::CornflowerBlue = 0xFF6495ED;
Color Colors::Cornsilk = 0xFFFFF8DC;
Color Colors::Crimson = 0xFFDC143C;
Color Colors::Cyan = 0xFF00FFFF;
Color Colors::DarkBlue = 0xFF00008B;
Color Colors::DarkCyan = 0xFF008B8B;
Color Colors::DarkGoldenrod = 0xFFB8860B;
Color Colors::DarkGray = 0xFFA9A9A9;
Color Colors::DarkGreen = 0xFF006400;
Color Colors::DarkKhaki = 0xFFBDB76B;
Color Colors::DarkMagenta = 0xFF8B008B;
Color Colors::DarkOliveGreen = 0xFF556B2F;
Color Colors::DarkOrange = 0xFFFF8C00;
Color Colors::DarkOrchid = 0xFF9932CC;
Color Colors::DarkRed = 0xFF8B0000;
Color Colors::DarkSalmon = 0xFFE9967A;
Color Colors::DarkSeaGreen = 0xFF8FBC8F;
Color Colors::DarkSlateBlue = 0xFF483D8B;
Color Colors::DarkSlateGray = 0xFF2F4F4F;
Color Colors::DarkTurquoise = 0xFF00CED1;
Color Colors::DarkViolet = 0xFF9400D3;
Color Colors::DeepPink = 0xFFFF1493;
Color Colors::DeepSkyBlue = 0xFF00BFFF;
Color Colors::DimGray = 0xFF696969;
Color Colors::DodgerBlue = 0xFF1E90FF;
Color Colors::Firebrick = 0xFFB22222;
Color Colors::FloralWhite = 0xFFFFFAF0;
Color Colors::ForestGreen = 0xFF228B22;
Color Colors::Fuchsia = 0xFFFF00FF;
Color Colors::Gainsboro = 0xFFDCDCDC;
Color Colors::GhostWhite = 0xFFF8F8FF;
Color Colors::Gold = 0xFFFFD700;
Color Colors::Goldenrod = 0xFFDAA520;
Color Colors::Gray = 0xFF808080;
Color Colors::Green = 0xFF008000;
Color Colors::GreenYellow = 0xFFADFF2F;
Color Colors::Honeydew = 0xFFF0FFF0;
Color Colors::HotPink = 0xFFFF69B4;
Color Colors::IndianRed = 0xFFCD5C5C;
Color Colors::Indigo = 0xFF4B0082;
Color Colors::Ivory = 0xFFFFFFF0;
Color Colors::Khaki = 0xFFF0E68C;
Color Colors::Lavender = 0xFFE6E6FA;
Color Colors::LavenderBlush = 0xFFFFF0F5;
Color Colors::LawnGreen = 0xFF7CFC00;
Color Colors::LemonChiffon = 0xFFFFFACD;
Color Colors::LightBlue = 0xFFADD8E6;
Color Colors::LightCoral = 0xFFF08080;
Color Colors::LightCyan = 0xFFE0FFFF;
Color Colors::LightGoldenrodYellow = 0xFFFAFAD2;
Color Colors::LightGreen = 0xFF90EE90;
Color Colors::LightGray = 0xFFD3D3D3;
Color Colors::LightPink = 0xFFFFB6C1;
Color Colors::LightSalmon = 0xFFFFA07A;
Color Colors::LightSeaGreen = 0xFF20B2AA;
Color Colors::LightSkyBlue = 0xFF87CEFA;
Color Colors::LightSlateGray = 0xFF778899;
Color Colors::LightSteelBlue = 0xFFB0C4DE;
Color Colors::LightYellow = 0xFFFFFFE0;
Color Colors::Lime = 0xFF00FF00;
Color Colors::LimeGreen = 0xFF32CD32;
Color Colors::Linen = 0xFFFAF0E6;
Color Colors::Magenta = 0xFFFF00FF;
Color Colors::Maroon = 0xFF800000;
Color Colors::MediumAquamarine = 0xFF66CDAA;
Color Colors::MediumBlue = 0xFF0000CD;
Color Colors::MediumOrchid = 0xFFBA55D3;
Color Colors::MediumPurple = 0xFF9370DB;
Color Colors::MediumSeaGreen = 0xFF3CB371;
Color Colors::MediumSlateBlue = 0xFF7B68EE;
Color Colors::MediumSpringGreen = 0xFF00FA9A;
Color Colors::MediumTurquoise = 0xFF48D1CC;
Color Colors::MediumVioletRed = 0xFFC71585;
Color Colors::MidnightBlue = 0xFF191970;
Color Colors::MintCream = 0xFFF5FFFA;
Color Colors::MistyRose = 0xFFFFE4E1;
Color Colors::Moccasin = 0xFFFFE4B5;
Color Colors::NavajoWhite = 0xFFFFDEAD;
Color Colors::Navy = 0xFF000080;
Color Colors::OldLace = 0xFFFDF5E6;
Color Colors::Olive = 0xFF808000;
Color Colors::OliveDrab = 0xFF6B8E23;
Color Colors::Orange = 0xFFFFA500;
Color Colors::OrangeRed = 0xFFFF4500;
Color Colors::Orchid = 0xFFDA70D6;
Color Colors::PaleGoldenrod = 0xFFEEE8AA;
Color Colors::PaleGreen = 0xFF98FB98;
Color Colors::PaleTurquoise = 0xFFAFEEEE;
Color Colors::PaleVioletRed = 0xFFDB7093;
Color Colors::PapayaWhip = 0xFFFFEFD5;
Color Colors::PeachPuff = 0xFFFFDAB9;
Color Colors::Peru = 0xFFCD853F;
Color Colors::Pink = 0xFFFFC0CB;
Color Colors::Plum = 0xFFDDA0DD;
Color Colors::PowderBlue = 0xFFB0E0E6;
Color Colors::Purple = 0xFF800080;
Color Colors::Red = 0xFFFF0000;
Color Colors::RosyBrown = 0xFFBC8F8F;
Color Colors::RoyalBlue = 0xFF4169E1;
Color Colors::SaddleBrown = 0xFF8B4513;
Color Colors::Salmon = 0xFFFA8072;
Color Colors::SandyBrown = 0xFFF4A460;
Color Colors::SeaGreen = 0xFF2E8B57;
Color Colors::SeaShell = 0xFFFFF5EE;
Color Colors::Sienna = 0xFFA0522D;
Color Colors::Silver = 0xFFC0C0C0;
Color Colors::SkyBlue = 0xFF87CEEB;
Color Colors::SlateBlue = 0xFF6A5ACD;
Color Colors::SlateGray = 0xFF708090;
Color Colors::Snow = 0xFFFFFAFA;
Color Colors::SpringGreen = 0xFF00FF7F;
Color Colors::SteelBlue = 0xFF4682B4;
Color Colors::Tan = 0xFFD2B48C;
Color Colors::Teal = 0xFF008080;
Color Colors::Thistle = 0xFFD8BFD8;
Color Colors::Tomato = 0xFFFF6347;
Color Colors::Turquoise = 0xFF40E0D0;
Color Colors::Violet = 0xFFEE82EE;
Color Colors::Wheat = 0xFFF5DEB3;
Color Colors::White = 0xFFFFFFFF;
Color Colors::WhiteSmoke = 0xFFF5F5F5;
Color Colors::Yellow = 0xFFFFFF00;
Color Colors::YellowGreen = 0xFF9ACD32;

Uint32 Color::ToRgb()
{
    Byte r = (Byte)(_color >> 16);
    Byte g = (Byte)(_color >> 8);
    Byte b = (Byte)(_color & 0xFF);

    Uint32 val = Uint32((Byte)r);

    val |= (Uint32)(((Uint32)(g)) << 8);
    val |= (Uint32)(((Uint32)(b)) << 16);

    return val;
}

Uint32 Color::ToArgb()
{
    return _color;
}

Uint32 Color::ToRgb(Color clr)
{
    //#define ALPHACOLOR(d,o,alpha) ((BYTE)(((Uint32)(d * (255.0 - alpha) + o * alpha)) >> 8))
    return clr.ToRgb();
}

String Color::ToHex(Color clr)
{
    String strHex;
    if (Color::A(clr) == 255)
    {
        strHex.Format(_T("#%02X%02X%02X"), Color::R(clr), Color::G(clr), Color::B(clr));
    }
    else
    {
        strHex.Format(_T("#%02X%02X%02X%02X"), Color::A(clr), Color::R(clr), Color::G(clr), Color::B(clr));
    }
    return strHex;
}

String Color::ToString(Color clr)
{
    String strClr;
    strClr = ColorManager::Ins()->ColorToString(clr);
    return strClr;
}

Color Color::Parse(const String& strClr)
{
    Uint32 clr = 0xFF000000;
    if (strClr.IndexOf(_U("#")) == -1)
    {
        ColorManager::Ins()->GetColor(strClr, clr);
    }
    else if (strClr.Length() > 1)
    {
        if (strClr.Length() == 2)
        {
            if (strClr.Equals(_T("#0")))
            {
                return ARGB(255,0,0,0);
            }
            else if (strClr.CompareI(_T("#f")) == 0)
            {
                return ARGB(255,255,255,255);
            }
        }

        Uint32 uClr = _tcstoul(strClr.c_str() + 1, 0, 16);
        if (strClr.Length() < 8)
        {
            uClr |= 0xFF000000;
        }
        clr = uClr;
    }
    return clr;
}

Byte Color::A(Color clr)
{
    return (Byte((clr._color) >> 24));
}

Byte Color::R(Color clr)
{
    return (Byte((clr._color) >> 16));
}

Byte Color::G(Color clr)
{
    return (Byte((clr._color) >> 8));
}

Byte Color::B(Color clr)
{
    return (Byte((clr._color) & 0xFF));
}

Uint32 Color::ToArgb(Color clr)
{
    return clr._color;
}

Color Color::FromRgb(Uint32 val)
{
    Byte b = (Byte)(val >> 16);
    Byte g = (Byte)(val >> 8);
    Byte r = (Byte)(val & 0xFF);

    return Color::FromRgb(r, g, b);
}

Color Color::FromRgb(Byte r, Byte g, Byte b)
{
    Uint32 val = Uint32((Byte)b);

    val |= (Uint32)(((Uint32)(g)) << 8);
    val |= (Uint32)(((Uint32)(r)) << 16);

    return Color(val | 0xFF000000);
}

Color Color::FromArgb(Byte a, Byte r, Byte g, Byte b)
{
    Uint32 val = Uint32((Byte)b);

    val |= (Uint32)(((Uint32)(g)) << 8);
    val |= (Uint32)(((Uint32)(r)) << 16);
    val |= (Uint32)(((Uint32)(a)) << 24);

    return Color(val);
}

}
