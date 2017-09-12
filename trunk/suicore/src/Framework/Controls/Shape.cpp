// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Shape.cpp

#include <Framework/Controls/Shape.h>
#include <System/Tools/Debug.h>
#include <Skia/effects/SkDashPathEffect.h>

namespace suic
{

ImplementRTTIOfClass(Shape, FrameworkElement)

DpProperty* Shape::EffectProperty;
DpProperty* Shape::StretchProperty;
DpProperty* Shape::FillProperty;
DpProperty* Shape::StrokeProperty;
DpProperty* Shape::StrokeThicknessProperty;

DpProperty* Shape::StrokeDashCapProperty;
DpProperty* Shape::StrokeDashArrayProperty;
DpProperty* Shape::StrokeDashOffsetProperty;
DpProperty* Shape::StrokeEndLineCapProperty;
DpProperty* Shape::StrokeStartLineCapProperty;
DpProperty* Shape::StrokeLineJoinProperty;
DpProperty* Shape::StrokeMiterLimitProperty;

ImplementRTTIOfClass(FloatAutoArray, Object)

FloatAutoArray::FloatAutoArray()
{
    setAutoDelete(true);
}

String FloatAutoArray::ToString()
{
    String strText;
    int iCount = GetCount() - 1;

    for (int i = 0; i < iCount; ++i)
    {
        strText += String().FromFloat(GetItem(i));
        strText += _U(",");
    }

    if (GetCount() > 0)
    {
        strText += String().FromFloat(iCount);
    }

    return strText;
}

ImplementRTTIOfClass(IntAutoArray, Object)

IntAutoArray::IntAutoArray()
{
    setAutoDelete(true);
}

String IntAutoArray::ToString()
{
    String strText;
    int iCount = GetCount() - 1;

    for (int i = 0; i < iCount; ++i)
    {
        strText += String().FromFloat(GetItem(i));
        strText += _U(",");
    }

    if (GetCount() > 0)
    {
        strText += String().FromFloat(iCount);
    }

    return strText;
}

class PenLineCapConvert : public IConvertValue
{
public:

    static PenLineCapConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* prop)
    {
        String val = prop->ToString();

        if (val.Equals(_U("Square")))
        {
            return Integer::GetPosInt(PenLineCap::plcSquare);
        }
        else if (val.Equals(_U("Round")))
        {
            return Integer::GetPosInt(PenLineCap::plcRound);
        }
        else if (val.Equals(_U("Triangle")))
        {
            return Integer::GetPosInt(PenLineCap::plcTriangle);
        }
        else
        {
            return Integer::GetPosInt(PenLineCap::plcFlat);
        }
    }

    String ToString(DpProperty* dp, Object* prop)
    {
        String strVal;
        int iVal = prop->ToInt();

        switch (iVal)
        {
        case PenLineCap::plcSquare:
            strVal = _U("Square");
            break;

        case PenLineCap::plcRound:
            strVal = _U("Round");
            break;

        case PenLineCap::plcTriangle:
            strVal = _U("Triangle");
            break;

        default:
            strVal = _U("Flat");
        }

        return strVal;
    }
};

PenLineCapConvert* PenLineCapConvert::Convert = new PenLineCapConvert();

class PenLineJoinConvert : public IConvertValue
{
public:

    static PenLineJoinConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* prop)
    {
        String val = prop->ToString();

        if (val.Equals(_U("Bevel")))
        {
            return Integer::GetPosInt(PenLineJoin::pljBevel);
        }
        else if (val.Equals(_U("Round")))
        {
            return Integer::GetPosInt(PenLineJoin::pljRound);
        }
        else
        {
            return Integer::GetPosInt(PenLineJoin::pljMiter);
        }
    }

    String ToString(DpProperty* dp, Object* prop)
    {
        String strVal;
        int iVal = prop->ToInt();

        switch (iVal)
        {
        case PenLineJoin::pljBevel:
            strVal = _U("Bevel");
            break;

        case PenLineJoin::pljRound:
            strVal = _U("Round");
            break;

        default:
            strVal = _U("Miter");
        }

        return strVal;
    }
};

PenLineJoinConvert* PenLineJoinConvert::Convert = new PenLineJoinConvert();

AutoObj FloatAutoArrayConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    String val = prop->ToString();
    StringArray strArr;

    strArr.SplitString(val, _U(","));

    if (strArr.Length() < 2 || (strArr.Length() % 2) != 0)
    {
        return DpProperty::NullValue;
    }
    else
    {
        FloatAutoArray* fArr = new FloatAutoArray();

        for (int i = 0; i < strArr.Length(); ++i)
        {
            String strVal = strArr[i];

            strVal.Trim();
            fArr->Add(strVal.ToFloat());
        }

        return fArr;
    }
}

//String FloatAutoArrayConvert::ToString(DpProperty* dp, Object* val)
//{
//    return String();
//}

FloatAutoArrayConvert* FloatAutoArrayConvert::Convert = new FloatAutoArrayConvert();

AutoObj IntAutoArrayConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    String val = prop->ToString();
    StringArray strArr;

    strArr.SplitString(val, _U(","));

    if (strArr.Length() < 2 || (strArr.Length() % 2) != 0)
    {
        return DpProperty::NullValue;
    }
    else
    {
        IntAutoArray* fArr = new IntAutoArray();

        for (int i = 0; i < strArr.Length(); ++i)
        {
            String strVal = strArr[i];

            strVal.Trim();
            fArr->Add(strVal.ToInt());
        }

        return fArr;
    }
}

IntAutoArrayConvert* IntAutoArrayConvert::Convert = new IntAutoArrayConvert();

Shape::Shape()
{
    _renderedGeometry = NULL;
}

Shape::~Shape()
{
    if (NULL != _renderedGeometry)
    {
        _renderedGeometry->unref();
    }
}

void Shape::StaticInit()
{
    if (EffectProperty == NULL)
    {
        FrameworkElement::StaticInit();

        EffectProperty  = DpProperty::Register(_T("Effect"), RTTIType(), Effect::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender));

        StretchProperty = DpProperty::Register(_U("Stretch"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(StretchBox::NoneBox, PropMetadataOptions::AffectsRender));
        FillProperty = DpProperty::Register(_U("Fill"), RTTIType(), Brush::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender));
        StrokeProperty = DpProperty::Register(_U("Stroke"), RTTIType(), Brush::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender));
        StrokeThicknessProperty = DpProperty::Register(_U("StrokeThickness"), RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(1), PropMetadataOptions::AffectsParentArrange));

        StrokeDashCapProperty = DpProperty::Register(_U("StrokeDashCap"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(PenLineCap::plcFlat)));
        StrokeDashArrayProperty = DpProperty::Register(_U("StrokeDashArray"), RTTIType(), FloatAutoArray::RTTIType(), DpPropMemory::GetPropMeta(NULL));
        StrokeDashOffsetProperty = DpProperty::Register(_U("StrokeDashOffset"), RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(OFloat::GetZeroFloat()));
        StrokeEndLineCapProperty = DpProperty::Register(_U("StrokeEndLineCap"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(PenLineCap::plcFlat)));
        StrokeLineJoinProperty = DpProperty::Register(_U("StrokeLineJoin"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(PenLineJoin::pljMiter)));
        StrokeStartLineCapProperty = DpProperty::Register(_U("StrokeStartLineCap"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(PenLineCap::plcFlat)));
        StrokeMiterLimitProperty = DpProperty::Register(_U("StrokeMiterLimit"), RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(10.0)));

        StrokeDashArrayProperty->SetConvertValueCb(FloatAutoArrayConvert::Convert);
        StrokeDashCapProperty->SetConvertValueCb(PenLineCapConvert::Convert);
        StrokeEndLineCapProperty->SetConvertValueCb(PenLineCapConvert::Convert);
        StrokeStartLineCapProperty->SetConvertValueCb(PenLineCapConvert::Convert);
        StrokeLineJoinProperty->SetConvertValueCb(PenLineJoinConvert::Convert);
        StretchProperty->SetConvertValueCb(StretchConvert::Convert);
    }
}

PenLineCap Shape::GetStrokeDashCap()
{
    return (PenLineCap)GetValue(StrokeDashCapProperty)->ToInt();
}

FloatAutoArray* Shape::GetStrokeDashArray()
{
    return DynamicCast<FloatAutoArray>(GetValue(StrokeDashArrayProperty));
}

Float Shape::GetStrokeDashOffset()
{
    return GetValue(StrokeDashOffsetProperty)->ToFloat();
}

PenLineCap Shape::GetStrokeEndLineCap()
{
    return (PenLineCap)GetValue(StrokeEndLineCapProperty)->ToInt();
}

PenLineJoin Shape::GetStrokeLineJoin()
{
    return (PenLineJoin)GetValue(StrokeLineJoinProperty)->ToInt();
}

Float Shape::GetStrokeMiterLimit()
{
    return GetValue(StrokeMiterLimitProperty)->ToFloat();
}

PenLineCap Shape::GetStrokeStartLineCap()
{
    return (PenLineCap)GetValue(StrokeStartLineCapProperty)->ToInt();
}

Size Shape::OnMeasure(const Size& size)
{
    Size containt;

    if (_renderedGeometry != NULL)
    {
        fRect rect;
        rect = _renderedGeometry->GetBounds();
        containt.cx = rect.right;
        containt.cy = rect.bottom;
    }

    return containt;
}

void Shape::OnArrange(const Size& arrangeSize)
{
}

void Shape::InitPen(Pen* pen, Brush* strokeBrush, int iStrokeThickness)
{
    FloatAutoArray* fArr = GetStrokeDashArray();

    pen->SetThickness(iStrokeThickness);
    pen->SetBrush(strokeBrush);

    if (NULL != fArr)
    {
        for (int i = 0; i < fArr->GetCount(); ++i)
        {
            pen->GetDashStyle()->dashes.Add(fArr->GetItem(i));
        }
    }

}

}
