// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// TextElement.cpp

#include <Framework/Controls/TextElement.h>

#include <System/Render/Skia/SkTextOp.h>
#include <System/Windows/EventManager.h>
#include <System/Windows/CoreTool.h>
#include <System/Tools/CoreHelper.h>
#include <System/Input/KeyboardNavigation.h>

namespace suic
{

//DpProperty* TextElement::IsReadOnlyProperty;
DpProperty* TextElement::IsSingleLineProperty;
DpProperty* TextElement::TextEffectsProperty;

DpProperty* TextElement::FontFamilyProperty;
DpProperty* TextElement::FontSizeProperty;
DpProperty* TextElement::FontWeightProperty;
DpProperty* TextElement::FontStyleProperty;
DpProperty* TextElement::FontStretchProperty;

DpProperty* TextElement::BackgroundProperty;
DpProperty* TextElement::ForegroundProperty;

ImplementRTTIOfClass(TextElement, FrameworkElement)
ImplementRTTIOfClass(TextDecoration, DpObject)
ImplementRTTIOfClass(TextEffectCollection, DpObject)
ImplementRTTIOfClass(TextDecorationCollection, DpObject)

//Italic Normal Oblique
//Black Bold Light Medium Normal Thin SemiBold ExtraBold ExtraBlack ExtraLight

suic::DpProperty* TextDecoration::PenProperty;
suic::DpProperty* TextDecoration::LocationProperty;
suic::DpProperty* TextDecoration::PenOffsetProperty;

TextDecorationCollection* TextDecorations::none;
TextDecorationCollection* TextDecorations::baseline;
TextDecorationCollection* TextDecorations::overline;
TextDecorationCollection* TextDecorations::strikethrough;
TextDecorationCollection* TextDecorations::underline;

void TextDecoration::StaticInit()
{
    if (PenProperty == NULL)
    {
        PenProperty = DpProperty::Register(_T("Pen"), RTTIType(), Brush::RTTIType(), suic::DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender));
        LocationProperty = DpProperty::Register(_T("Location"), RTTIType(), Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsRender));
        PenOffsetProperty = DpProperty::Register(_T("PenOffset"), RTTIType(), Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsRender));

        TextDecorations::none = TextDecorationCollection::GetEmpty();
        TextDecorations::baseline = new TextDecorationCollection();
        TextDecorations::overline = new TextDecorationCollection();
        TextDecorations::strikethrough = new TextDecorationCollection();
        TextDecorations::underline = new TextDecorationCollection();

        TextDecorations::none->ref();
        TextDecorations::baseline->ref();
        TextDecorations::overline->ref();
        TextDecorations::underline->ref();
        TextDecorations::strikethrough->ref();

        TextDecorations::baseline->AddTextDecoration(new TextDecoration(TextDecorationLocation::Baseline));
        TextDecorations::overline->AddTextDecoration(new TextDecoration(TextDecorationLocation::OverLine));
        TextDecorations::underline->AddTextDecoration(new TextDecoration(TextDecorationLocation::Underline));
        TextDecorations::strikethrough->AddTextDecoration(new TextDecoration(TextDecorationLocation::Strikethrough));
    }
}

TextDecoration::TextDecoration()
{

}

TextDecoration::TextDecoration(TextDecorationLocation textLoc)
{
    SetLocation(textLoc);
}

Brush* TextDecoration::GetPen()
{
    return DynamicCast<Brush>(GetValue(PenProperty));
}

void TextDecoration::SetPen(Brush* val)
{
    SetValue(PenProperty, val);
}

TextDecorationLocation TextDecoration::GetLocation()
{
    return (TextDecorationLocation)GetValue(LocationProperty)->ToInt();
}

void TextDecoration::SetLocation(TextDecorationLocation val)
{
    SetValue(LocationProperty, new Integer(val));
}

Float TextDecoration::GetPenOffset()
{
    return GetValue(PenOffsetProperty)->ToFloat();
}

void TextDecoration::SetPenOffset(Float val)
{
    SetValue(PenOffsetProperty, new OFloat(val));
}

TextDecorationCollection* TextDecorationCollection::s_empty;

TextDecorationCollection* TextDecorationCollection::GetEmpty()
{
    if (s_empty == NULL)
    {
        s_empty = new TextDecorationCollection();
        s_empty->ref();
    }
    return s_empty;
}

TextDecorationCollection::TextDecorationCollection()
{

}

TextDecorationCollection::~TextDecorationCollection()
{
    Clear();
}

int TextDecorationCollection::GetCount() const
{
    return _textDecorations.GetCount();
}

TextDecoration* TextDecorationCollection::GetTextDecoration(int index) const
{
    return _textDecorations[index];
}

void TextDecorationCollection::AddTextDecoration(TextDecoration* val)
{
    val->ref();
    _textDecorations.Add(val);
}

void TextDecorationCollection::RemoveChild(Object* child)
{
    TextDecoration* pText = RTTICast<TextDecoration>(child);
    Remove(pText);
}

void TextDecorationCollection::Remove(TextDecoration* val)
{
    for (int i = 0; i < _textDecorations.GetCount(); ++i)
    {
        if (_textDecorations[i] == val)
        {
            _textDecorations[i]->unref();
            _textDecorations.RemoveAt(i);
            break;
        }
    }
}

void TextDecorationCollection::Clear()
{
    for (int i = 0; i < _textDecorations.GetCount(); ++i)
    {
        _textDecorations[i]->unref();
    }
    _textDecorations.Clear();
}

void TextDecorationCollection::AddChild(Object* obj)
{
    if (obj)
    {
        TextDecoration* textDecor = RTTICast<TextDecoration>(obj);
        obj->ref();
        if (NULL != textDecor)
        {
            AddTextDecoration(textDecor);
        }
        obj->unref();
    }
}

void TextDecorationCollection::AddText(String val)
{

}

TextEffectCollection* TextEffectCollection::s_empty;

TextEffectCollection* TextEffectCollection::GetEmpty()
{
    if (s_empty == NULL)
    {
        s_empty = new TextEffectCollection();
        s_empty->ref();
    }
    return s_empty;
}

TextEffectCollection::TextEffectCollection()
{

}

TextEffectCollection::~TextEffectCollection()
{
    Clear();
}

void TextEffectCollection::Add(TextEffect* val)
{
    val->ref();
    _effects.Add(val);
}

void TextEffectCollection::RemoveChild(Object* child)
{
    TextEffect* pText = RTTICast<TextEffect>(child);
    Remove(pText);
}

void TextEffectCollection::Remove(TextEffect* val)
{
    for (int i = 0; i < _effects.GetCount(); ++i)
    {
        if (_effects[i] == val)
        {
            _effects.RemoveAt(i);
            break;
        }
    }
}

void TextEffectCollection::Clear()
{
    for (int i = 0; i < _effects.GetCount(); ++i)
    {
        _effects[i]->unref();
    }
    _effects.Clear();
}

void TextEffectCollection::AddChild(Object* obj)
{
    if (obj)
    {
        TextEffect* textEffect = RTTICast<TextEffect>(obj);
        obj->ref();
        if (NULL != textEffect)
        {
            Add(textEffect);
        }
        obj->unref();
    }
}

void TextEffectCollection::AddText(String val)
{

}

int TextEffectCollection::GetCount()
{
    return _effects.GetCount();
}

TextEffect* TextEffectCollection::GetTextEffect(int index)
{
    return _effects[index];
}

//=======================================================
//

TextElement::TextElement()
{
}

TextElement::~TextElement()
{
}

void TextElement::StaticInit()
{
    if (NULL == ForegroundProperty)
    {
        KeyboardNavigation::StaticInit();
        FrameworkElement::StaticInit();

        Uint32 flag = PropMetadataOptions::AffectsParentMeasure | PropMetadataOptions::AffectsRender | PropMetadataOptions::AffectsInherits;
        Uint32 flag1 = PropMetadataOptions::AffectsMeasure | PropMetadataOptions::AffectsRender | PropMetadataOptions::AffectsInherits;

        IsSingleLineProperty = DpProperty::Register(_T("IsSingleLine"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsNone));
        TextEffectsProperty = DpProperty::Register(_T("TextEffects"), RTTIType(), TextEffectCollection::RTTIType(), DpPropMemory::GetPropMeta(TextEffectCollection::GetEmpty(), PropMetadataOptions::AffectsRender));

        BackgroundProperty = DpProperty::Register(_T("Background"), RTTIType(), Brush::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender));//, &TextElement::OnForegroundPropChanged));
        ForegroundProperty = DpProperty::RegisterAttached(_T("Foreground"), RTTIType(), Brush::RTTIType(), DpPropMemory::GetPropMeta(NULL, flag));//, &TextElement::OnForegroundPropChanged));
        FontFamilyProperty = DpProperty::RegisterAttached(_T("FontFamily"), RTTIType(), OString::RTTIType(), suic::DpPropMemory::GetPropMeta(new OString(CoreHelper::GetDefaultFontFamily()), flag));//, &TextElement::OnFontFamilyPropChanged));
        FontSizeProperty = DpProperty::RegisterAttached(_T("FontSize"), RTTIType(), Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(new Integer(CoreHelper::GetDefaultFontSize()), flag));//, &TextElement::OnFontSizePropChanged));
        FontWeightProperty = DpProperty::RegisterAttached(_T("FontWeight"), RTTIType(), Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(new Integer(FontWeightStyle::fwNormal), flag));//, &TextElement::OnFontWeightPropChanged));
        FontStyleProperty = DpProperty::RegisterAttached(_T("FontStyle"), RTTIType(), Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(Integer::GetPosInt(FontStyleStyle::fssNormal), flag));
        FontStretchProperty = DpProperty::RegisterAttached(_T("FontStretch"), RTTIType(), Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(Integer::GetZeroInt(), flag1));

        MinHeightProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(16), 0));
        HorizontalAlignmentProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(HoriAlignBox::LeftBox, PropMetadataOptions::AffectsParentArrange));
        VerticalAlignmentProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(VertAlignBox::TopBox, PropMetadataOptions::AffectsParentArrange));

        FontWeightProperty->SetConvertValueCb(FontWeightConvert::Convert);
        FontStyleProperty->SetConvertValueCb(FontStyleConvert::Convert);

        KeyboardNavigation::TabIndexProperty->AddOwner(RTTIType(), NULL);
    }
}

String TextElement::GetFontFamily()
{
    return GetValue(FontFamilyProperty)->ToString();
}

void TextElement::SetFontFamily(String val)
{
    SetValue(FontFamilyProperty, new OString(val));
}

int TextElement::GetFontSize()
{
    return GetValue(FontSizeProperty)->ToInt();
}

Float TextElement::GetRealFontSize()
{
    if (SystemParameters::isNeedTransform())
    {
        return SystemParameters::TransformXToDevice(GetFontSize() - SystemParameters::DpiToX * SystemParameters::DpiToX);
    }
    else
    {
        return GetFontSize();
    }
}

void TextElement::SetFontSize(int val)
{
    SetValue(FontSizeProperty, new Integer(val));
}

int TextElement::GetFontWeight()
{
    return GetValue(FontWeightProperty)->ToInt();
}

void TextElement::SetFontWeight(int val)
{
    SetValue(FontWeightProperty, new Integer(val));
}

int TextElement::GetFontStyle()
{
    return GetValue(FontStyleProperty)->ToInt();
}

void TextElement::SetFontStyle(int val)
{
    SetValue(FontStyleProperty, new Integer(val));
}

int TextElement::GetFontStretch()
{
    return GetValue(FontStretchProperty)->ToInt();
}

void TextElement::SetFontStretch(int val)
{
    SetValue(FontStretchProperty, new Integer(val));
}

Brush* TextElement::GetForeground()
{
    return RTTICast<Brush>(GetValue(ForegroundProperty));
}

void TextElement::SetForeground(Brush* val)
{
    SetValue(ForegroundProperty, val);
}

Brush* TextElement::GetBackground()
{
    return RTTICast<Brush>(GetValue(BackgroundProperty));
}

void TextElement::SetBackground(Brush* val)
{
    SetValue(BackgroundProperty, val);
}

bool TextElement::GetSingleLine()
{
    return GetValue(IsSingleLineProperty)->ToBool();
}

void TextElement::SetSingleLine(bool val)
{
    SetValue(IsSingleLineProperty, BOOLTOBOOLEAN(val));
}

TextEffectCollection* TextElement::GetTextEffects()
{
    return dynamic_cast<TextEffectCollection*>(GetValue(TextEffectsProperty));
}

void TextElement::SetTextEffects(TextEffectCollection* val)
{
    SetValue(TextEffectsProperty, val);
}

}
