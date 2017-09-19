
// ============================================================================
// SolidColorBrush

#include <System/Graphics/SolidColorBrush.h>
#include <System/Types/Interface.h>

namespace suic
{

SolidColorBrush* SolidColorBrush::Red;
SolidColorBrush* SolidColorBrush::Green;
SolidColorBrush* SolidColorBrush::Blue;
SolidColorBrush* SolidColorBrush::Black;
SolidColorBrush* SolidColorBrush::White;

ImplementRTTIOfClass(SolidColorBrush, Brush)

SolidColorBrush::SolidColorBrush(Color color)
    : _color(color)
{
    _index = Brush::eBrushIndex::biSolidBrush;
}

SolidColorBrush::SolidColorBrush()
    : _color(ARGB(255,255,255,255))
{
    _index = Brush::eBrushIndex::biSolidBrush;
}

SolidColorBrush::~SolidColorBrush()
{

}

void SolidColorBrush::StaticInit()
{
    if (NULL == Red)
    {
        Red = new SolidColorBrush(Colors::Red);
        Green = new SolidColorBrush(Colors::Green);
        Blue = new SolidColorBrush(Colors::Blue);
        Black = new SolidColorBrush(Colors::Black);
        White = new SolidColorBrush(Colors::White);

        SolidColorBrush::Red->ref();
        SolidColorBrush::Green->ref();
        SolidColorBrush::Blue->ref();
        SolidColorBrush::Black->ref();
        SolidColorBrush::White->ref();
    }
}

SolidColorBrush* SolidColorBrush::Clone() const
{
    SolidColorBrush* pBrush(new SolidColorBrush());
    pBrush->_color = _color;
    return pBrush;
}

bool SolidColorBrush::Parse(const String& xml)
{
    return false;
}

bool SolidColorBrush::Parse(IXamlNode* pNode)
{
    _color = OColor::FromString(pNode->FindAttri(_T("Color")));
    _key = pNode->FindAttri(_T("x:Key"));
    return !_key.Empty();
}

//void SolidColorBrush::Draw(Drawing* drawing, const fRect* lprc)
//{
//    drawing->FillRect(_color, lprc);
//}

Color SolidColorBrush::GetColor() const
{
    return _color;
}

void SolidColorBrush::SetColor(Color color)
{
    _color = color;
}

String SolidColorBrush::ToString()
{
    String strXml;

    strXml = _T("<SolidColorBrush ");

    strXml += _T("Key=\"");
    strXml += _key;
    strXml += _T("\" ");

    OColor vp(_color);

    vp.setAutoDelete(false);

    strXml += _T("\" Color=\"");
    strXml += vp.ToString();
    strXml += _T("\" ");

    strXml += _T("/>");

    return strXml;
}

};

