
#include <System/Windows/Effect.h>
#include <System/Windows/SystemEffect.h>
#include <System/Types/Interface.h>

#include <Skia/core/SkPaint.h>
#include <Skia/effects/SkBlurImageFilter.h>
#include <Skia/effects/SkDropShadowImageFilter.h>

namespace suic 
{

ImplementRTTIOfClass(Effect, Object)
ImplementRTTIOfClass(TextEffect, Effect)
ImplementRTTIOfClass(BlurEffect, Effect)
ImplementRTTIOfClass(LightingEffect, Effect)
ImplementRTTIOfClass(DropShadowEffect, Effect)
ImplementRTTIOfClass(EmbossEffect, Effect)

BlurEffect::BlurEffect()
    : _sigmaX(0)
    , _sigmaY(0)
{

}

//void BlurEffect::Apply(Drawing* drawing)
//{
//    //SkPaint& paint = DrawInfoToSkia(drawInfo);
//    //SkImageFilter* pFilter = SkBlurImageFilter::Create(_sigmaX, _sigmaY);
//    //paint.setImageFilter(pFilter)->unref();
//}

void BlurEffect::Parse(IXamlNode* node)
{
    String x = node->FindAttri(_U("SigmaX"));
    String y = node->FindAttri(_U("SigmaY"));

    _sigmaX = x.ToFloat();
    _sigmaY = y.ToFloat();
}

String BlurEffect::ToString()
{
    String strXml;
    strXml.Format(_U("SigmaX=\"%g\" SigmaY=\"%g\""), _sigmaX, _sigmaY);
    return strXml;
}

//-------------------------------------------------
//
DropShadowEffect::DropShadowEffect()
    : _sigmaX(0)
    , _sigmaY(0)
    , _dx(0)
    , _dy(0)
{

}

//void DropShadowEffect::Apply(Drawing* drawing)
//{
//    /*SkPaint& paint = DrawInfoToSkia(drawInfo);
//    SkImageFilter* pFilter = SkDropShadowImageFilter::Create(_dx, _dy, _sigmaX, _sigmaY, _color);
//    paint.setImageFilter(pFilter)->unref();*/
//}

void DropShadowEffect::Parse(IXamlNode* node)
{
    String x = node->FindAttri(_U("SigmaX"));
    String y = node->FindAttri(_U("SigmaY"));
    String dx = node->FindAttri(_U("DX"));
    String dy = node->FindAttri(_U("DY"));
    String clr = node->FindAttri(_U("Color"));

    _sigmaX = x.ToFloat();
    _sigmaY = y.ToFloat();
    _dx = dx.ToFloat();
    _dy = dy.ToFloat();
    _color = Color::Parse(clr);
}

String DropShadowEffect::ToString()
{
    String strXml;
    strXml.Format(_U("SigmaX=\"%g\" SigmaY=\"%g\" DX=\"%g\"  DY=\"%g\"  Color=\"%s\""), 
        _sigmaX, _sigmaY, _dx, _dy, Color::ToString(_color).c_str());
    return strXml;
}

//-------------------------------------------------
//
LightingEffect::LightingEffect()
    : _x(0)
    , _y(0)
    , _z(0)
    , _kd(0)
    , _scale(0)
{
}

void LightingEffect::Parse(IXamlNode* node)
{

}

String LightingEffect::ToString()
{
    String strXml;
    return strXml;
}

//-------------------------------------------------
//
EmbossEffect::EmbossEffect()
    : _x(0)
    , _y(0)
    , _z(0)
{
}

void EmbossEffect::Parse(IXamlNode* node)
{

}

String EmbossEffect::ToString()
{
    String strXml;
    return strXml;
}

}

