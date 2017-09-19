
// ============================================================================
// RadialGradientBrush

#include <System/Graphics/RadialGradientBrush.h>
#include <System/Tools/Array.h>
#include <System/Types/Interface.h>
#include <System/Render/Skia/SkiaDrawing.h>
#include <Skia/effects/SkGradientShader.h>

namespace suic
{

/*
<RadialGradientBrush MappingMode="" RadiusX="0.5" RadiusY="0.5" GradientOrigin="0.5,0.5" >
      <GradientStop Color="Yellow" Offset="0.0" />
      <GradientStop Color="Red" Offset="0.25" />
      <GradientStop Color="Blue" Offset="0.75" />
      <GradientStop Color="LimeGreen" Offset="1.0" />
</RadialGradientBrush>

*/
ImplementRTTIOfClass(RadialGradientBrush, GradientBrush)

RadialGradientBrush::RadialGradientBrush()
    : _radiusX(0.5f)
    , _radiusY(0.5f)
{
    _origin.x = 0.5f;
    _origin.y = 0.5f;
    _index = Brush::eBrushIndex::biRadialBrush;
}

RadialGradientBrush::~RadialGradientBrush()
{

}

void RadialGradientBrush::SetOrigin(Float x, Float y)
{
    _origin.x = x;
    _origin.y = y;
}

void RadialGradientBrush::SetRadiusXY(Float x, Float y)
{
    _radiusX = x;
    _radiusY = y;
}

RadialGradientBrush* RadialGradientBrush::Clone() const
{
    return const_cast<RadialGradientBrush*>(this);
}

bool RadialGradientBrush::Parse(const String& xml)
{
    return false;
}

bool RadialGradientBrush::Parse(IXamlNode* pNode)
{
    int iCount = 0;
    IXamlAttris* pAttri = pNode->GetAttris();

    pAttri->Reset();

    while (pAttri->HasNext())
    {
        String name(pAttri->GetName());
        String val(pAttri->GetValue());

        if (name.Equals(_T("GradientOrigin")))
        {
            _origin = val.TofPoint();            
        }
        else if (name.Equals(_T("RadiusX")))
        {
            _radiusX = val.ToFloat();
        }
        else if (name.Equals(_T("RadiusY")))
        {
            _radiusY = val.ToFloat();
        }
    }

    _key = pNode->FindAttri(_T("Key"));
    if (_key.Empty())
    {
        _key = pNode->FindAttri(_T("x:Key"));
    }
    
    //
    // Ω‚ŒˆGradientStop
    //
    return GradientBrush::Parse(pNode);
}

}

