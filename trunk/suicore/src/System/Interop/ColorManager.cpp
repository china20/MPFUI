
#include <System/Interop/InternalOper.h>
#include <System/Interop/colormanager.h>

namespace suic
{

static ColorManager* g_colormanager = NULL;

ColorManager::ColorManager()
{
    //--------------------------------------------------------
    _mapColor.Initialize(150);
    _mapColor.Add(_U("AliceBlue"), 0xFFF0F8FF);
    _mapColor.Add(_U("AntiqueWhite"), 0xFFFAEBD7);
    _mapColor.Add(_U("Aqua"), 0xFF00FFFF);
    _mapColor.Add(_U("Aquamarine"), 0xFF7FFFD4);
    _mapColor.Add(_U("Azure"), 0xFFF0FFFF);
    _mapColor.Add(_U("Beige"), 0xFFF5F5DC);
    _mapColor.Add(_U("Bisque"), 0xFFFFE4C4);
    _mapColor.Add(_U("Black"), 0xFF000000);
    _mapColor.Add(_U("BlanchedAlmond"), 0xFFFFEBCD);
    _mapColor.Add(_U("Blue"), 0xFF0000FF);
    _mapColor.Add(_U("BlueViolet"), 0xFF8A2BE2);
    _mapColor.Add(_U("Brown"), 0xFFA52A2A);
    _mapColor.Add(_U("BurlyWood"), 0xFFDEB887);
    _mapColor.Add(_U("CadetBlue"), 0xFF5F9EA0);
    _mapColor.Add(_U("Chartreuse"), 0xFF7FFF00);
    _mapColor.Add(_U("Chocolate"), 0xFFD2691E);
    _mapColor.Add(_U("Coral"), 0xFFFF7F50);
    _mapColor.Add(_U("CornflowerBlue"), 0xFF6495ED);
    _mapColor.Add(_U("Cornsilk"), 0xFFFFF8DC);
    _mapColor.Add(_U("Crimson"), 0xFFDC143C);
    _mapColor.Add(_U("Cyan"), 0xFF00FFFF);
    _mapColor.Add(_U("DarkBlue"), 0xFF00008B);
    _mapColor.Add(_U("DarkCyan"), 0xFF008B8B);
    _mapColor.Add(_U("DarkGoldenrod"), 0xFFB8860B);
    _mapColor.Add(_U("DarkGray"), 0xFFA9A9A9);
    _mapColor.Add(_U("DarkGreen"), 0xFF006400);
    _mapColor.Add(_U("DarkKhaki"), 0xFFBDB76B);
    _mapColor.Add(_U("DarkMagenta"), 0xFF8B008B);
    _mapColor.Add(_U("DarkOliveGreen"), 0xFF556B2F);
    _mapColor.Add(_U("DarkOrange"), 0xFFFF8C00);
    _mapColor.Add(_U("DarkOrchid"), 0xFF9932CC);
    _mapColor.Add(_U("DarkRed"), 0xFF8B0000);
    _mapColor.Add(_U("DarkSalmon"), 0xFFE9967A);
    _mapColor.Add(_U("DarkSeaGreen"), 0xFF8FBC8F);
    _mapColor.Add(_U("DarkSlateBlue"), 0xFF483D8B);
    _mapColor.Add(_U("DarkSlateGray"), 0xFF2F4F4F);
    _mapColor.Add(_U("DarkTurquoise"), 0xFF00CED1);
    _mapColor.Add(_U("DarkViolet"), 0xFF9400D3);
    _mapColor.Add(_U("DeepPink"), 0xFFFF1493);
    _mapColor.Add(_U("DeepSkyBlue"), 0xFF00BFFF);
    _mapColor.Add(_U("DimGray"), 0xFF696969);
    _mapColor.Add(_U("DodgerBlue"), 0xFF1E90FF);
    _mapColor.Add(_U("Firebrick"), 0xFFB22222);
    _mapColor.Add(_U("FloralWhite"), 0xFFFFFAF0);
    _mapColor.Add(_U("ForestGreen"), 0xFF228B22);
    _mapColor.Add(_U("Fuchsia"), 0xFFFF00FF);
    _mapColor.Add(_U("Gainsboro"), 0xFFDCDCDC);
    _mapColor.Add(_U("GhostWhite"), 0xFFF8F8FF);
    _mapColor.Add(_U("Gold"), 0xFFFFD700);
    _mapColor.Add(_U("Goldenrod"), 0xFFDAA520);
    _mapColor.Add(_U("Gray"), 0xFF808080);
    _mapColor.Add(_U("Green"), 0xFF008000);
    _mapColor.Add(_U("GreenYellow"), 0xFFADFF2F);
    _mapColor.Add(_U("Honeydew"), 0xFFF0FFF0);
    _mapColor.Add(_U("HotPink"), 0xFFFF69B4);
    _mapColor.Add(_U("IndianRed"), 0xFFCD5C5C);
    _mapColor.Add(_U("Indigo"), 0xFF4B0082);
    _mapColor.Add(_U("Ivory"), 0xFFFFFFF0);
    _mapColor.Add(_U("Khaki"), 0xFFF0E68C);
    _mapColor.Add(_U("Lavender"), 0xFFE6E6FA);
    _mapColor.Add(_U("LavenderBlush"), 0xFFFFF0F5);
    _mapColor.Add(_U("LawnGreen"), 0xFF7CFC00);
    _mapColor.Add(_U("LemonChiffon"), 0xFFFFFACD);
    _mapColor.Add(_U("LightBlue"), 0xFFADD8E6);
    _mapColor.Add(_U("LightCoral"), 0xFFF08080);
    _mapColor.Add(_U("LightCyan"), 0xFFE0FFFF);
    _mapColor.Add(_U("LightGoldenrodYellow"), 0xFFFAFAD2);
    _mapColor.Add(_U("LightGreen"), 0xFF90EE90);
    _mapColor.Add(_U("LightGray"), 0xFFD3D3D3);
    _mapColor.Add(_U("LightPink"), 0xFFFFB6C1);
    _mapColor.Add(_U("LightSalmon"), 0xFFFFA07A);
    _mapColor.Add(_U("LightSeaGreen"), 0xFF20B2AA);
    _mapColor.Add(_U("LightSkyBlue"), 0xFF87CEFA);
    _mapColor.Add(_U("LightSlateGray"), 0xFF778899);
    _mapColor.Add(_U("LightSteelBlue"), 0xFFB0C4DE);
    _mapColor.Add(_U("LightYellow"), 0xFFFFFFE0);
    _mapColor.Add(_U("Lime"), 0xFF00FF00);
    _mapColor.Add(_U("LimeGreen"), 0xFF32CD32);
    _mapColor.Add(_U("Linen"), 0xFFFAF0E6);
    _mapColor.Add(_U("Magenta"), 0xFFFF00FF);
    _mapColor.Add(_U("Maroon"), 0xFF800000);
    _mapColor.Add(_U("MediumAquamarine"), 0xFF66CDAA);
    _mapColor.Add(_U("MediumBlue"), 0xFF0000CD);
    _mapColor.Add(_U("MediumOrchid"), 0xFFBA55D3);
    _mapColor.Add(_U("MediumPurple"), 0xFF9370DB);
    _mapColor.Add(_U("MediumSeaGreen"), 0xFF3CB371);
    _mapColor.Add(_U("MediumSlateBlue"), 0xFF7B68EE);
    _mapColor.Add(_U("MediumSpringGreen"), 0xFF00FA9A);
    _mapColor.Add(_U("MediumTurquoise"), 0xFF48D1CC);
    _mapColor.Add(_U("MediumVioletRed"), 0xFFC71585);
    _mapColor.Add(_U("MidnightBlue"), 0xFF191970);
    _mapColor.Add(_U("MintCream"), 0xFFF5FFFA);
    _mapColor.Add(_U("MistyRose"), 0xFFFFE4E1);
    _mapColor.Add(_U("Moccasin"), 0xFFFFE4B5);
    _mapColor.Add(_U("NavajoWhite"), 0xFFFFDEAD);
    _mapColor.Add(_U("Navy"), 0xFF000080);
    _mapColor.Add(_U("OldLace"), 0xFFFDF5E6);
    _mapColor.Add(_U("Olive"), 0xFF808000);
    _mapColor.Add(_U("OliveDrab"), 0xFF6B8E23);
    _mapColor.Add(_U("Orange"), 0xFFFFA500);
    _mapColor.Add(_U("OrangeRed"), 0xFFFF4500);
    _mapColor.Add(_U("Orchid"), 0xFFDA70D6);
    _mapColor.Add(_U("PaleGoldenrod"), 0xFFEEE8AA);
    _mapColor.Add(_U("PaleGreen"), 0xFF98FB98);
    _mapColor.Add(_U("PaleTurquoise"), 0xFFAFEEEE);
    _mapColor.Add(_U("PaleVioletRed"), 0xFFDB7093);
    _mapColor.Add(_U("PapayaWhip"), 0xFFFFEFD5);
    _mapColor.Add(_U("PeachPuff"), 0xFFFFDAB9);
    _mapColor.Add(_U("Peru"), 0xFFCD853F);
    _mapColor.Add(_U("Pink"), 0xFFFFC0CB);
    _mapColor.Add(_U("Plum"), 0xFFDDA0DD);
    _mapColor.Add(_U("PowderBlue"), 0xFFB0E0E6);
    _mapColor.Add(_U("Purple"), 0xFF800080);
    _mapColor.Add(_U("Red"), 0xFFFF0000);
    _mapColor.Add(_U("RosyBrown"), 0xFFBC8F8F);
    _mapColor.Add(_U("RoyalBlue"), 0xFF4169E1);
    _mapColor.Add(_U("SaddleBrown"), 0xFF8B4513);
    _mapColor.Add(_U("Salmon"), 0xFFFA8072);
    _mapColor.Add(_U("SandyBrown"), 0xFFF4A460);
    _mapColor.Add(_U("SeaGreen"), 0xFF2E8B57);
    _mapColor.Add(_U("SeaShell"), 0xFFFFF5EE);
    _mapColor.Add(_U("Sienna"), 0xFFA0522D);
    _mapColor.Add(_U("Silver"), 0xFFC0C0C0);
    _mapColor.Add(_U("SkyBlue"), 0xFF87CEEB);
    _mapColor.Add(_U("SlateBlue"), 0xFF6A5ACD);
    _mapColor.Add(_U("SlateGray"), 0xFF708090);
    _mapColor.Add(_U("Snow"), 0xFFFFFAFA);
    _mapColor.Add(_U("SpringGreen"), 0xFF00FF7F);
    _mapColor.Add(_U("SteelBlue"), 0xFF4682B4);
    _mapColor.Add(_U("Tan"), 0xFFD2B48C);
    _mapColor.Add(_U("Teal"), 0xFF008080);
    _mapColor.Add(_U("Thistle"), 0xFFD8BFD8);
    _mapColor.Add(_U("Tomato"), 0xFFFF6347);
    _mapColor.Add(_U("Turquoise"), 0xFF40E0D0);
    _mapColor.Add(_U("Violet"), 0xFFEE82EE);
    _mapColor.Add(_U("Wheat"), 0xFFF5DEB3);
    _mapColor.Add(_U("White"), 0xFFFFFFFF);
    _mapColor.Add(_U("WhiteSmoke"), 0xFFF5F5F5);
    _mapColor.Add(_U("Yellow"), 0xFFFFFF00);
    _mapColor.Add(_U("YellowGreen"), 0xFF9ACD32);
}

ColorManager::~ColorManager()
{
}

ColorManager* ColorManager::Ins()
{
    if (!g_colormanager)
    {
        g_colormanager = new ColorManager();
    }
    return g_colormanager;
}

bool ColorManager::GetColor(const String& strClr, Uint32& clr)
{
    return _mapColor.TryGetValue(strClr, clr);
}

String ColorManager::ColorToString(Color clr)
{
    StringDic<Uint32>::Enumerator enumer(&_mapColor, 0);
    while (enumer.HasNext())
    {
        if (enumer.Current()->val == clr)
        {
            return enumer.Current()->key;
        }
    }
    return Color::ToHex(clr);
}

}
