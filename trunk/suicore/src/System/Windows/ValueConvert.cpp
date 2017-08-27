
#include <System/Windows/ValueConvert.h>
#include <System/Windows/DpProperty.h>
#include <Framework/Controls/Control.h>
#include <Framework/Controls/TextElement.h>

namespace suic
{

static int InternalGetHorizontalAlignment(const String& align)
{
    int val = HoriAlign::haCenter;

    if (align.Equals(_T("Left")) || align.Empty())
    {
        val = HoriAlign::haLeft;
    }
    else if (align.Equals(_T("Right")))
    {
        val = HoriAlign::haRight;
    }
    else if (align.Equals(_T("Stretch")))
    {
        val = HoriAlign::haStretch;
    }

    return val;
}

static int InternalGetVerticalAlignment(const String & align)
{
    int val = VertAlign::vaCenter;

    if (align.Equals(_T("Top")) || align.Empty())
    {
        val = VertAlign::vaTop;
    }
    else if (align.Equals(_T("Bottom")))
    {
        val = VertAlign::vaBottom;
    }
    else if (align.Equals(_T("Stretch")))
    {
        val = VertAlign::vaStretch;
    }

    return val;
}

AlignmentConvert::AlignmentConvert()
{

}

AlignmentConvert* AlignmentConvert::Convert = new AlignmentConvert();

AutoObj AlignmentConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    String val = prop->ToString();

    if (dp == FrameworkElement::HorizontalAlignmentProperty)
    {
        return HoriAlignBox::From(InternalGetHorizontalAlignment(val));
    }
    else if (dp == FrameworkElement::VerticalAlignmentProperty)
    {
        return VertAlignBox::From(InternalGetVerticalAlignment(val));
    }
    else if (dp == Control::HorizontalContentAlignmentProperty)
    {
        return HoriAlignBox::From(InternalGetHorizontalAlignment(val));
    }
    else if (dp == Control::VerticalContentAlignmentProperty)
    {
        return VertAlignBox::From(InternalGetVerticalAlignment(val));
    }
    else
    {
        return DpProperty::UnsetValue();
    }
}

OrientationConvert* OrientationConvert::Convert = new OrientationConvert();

OrientationConvert::OrientationConvert()
{

}

AutoObj OrientationConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    String val = prop->ToString();

    if (val.Equals(_T("Vertical")))
    {
        return OrientationBox::VerticalBox;
    }
    else
    {
        return OrientationBox::HorizontalBox;            
    }
}

//==========================================================
//

VisibilityConvert* VisibilityConvert::Convert = new VisibilityConvert();

VisibilityConvert::VisibilityConvert()
{

}

AutoObj VisibilityConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    Expression* expr = RTTICast<Expression>(prop);
    if (NULL == expr)
    {
        String val = prop->ToString();

        if (val.Equals(_T("Hidden")))
        {
            return VisibilityBox::HiddenBox;
        }
        else if (val.Equals(_T("Visible")))
        {
            return VisibilityBox::VisibleBox;
        }
        else if (val.Equals(_T("Collapsed")))
        {
            return VisibilityBox::CollapsedBox;
        }
        else
        {
            return VisibilityBox::VisibleBox;
        }
    }
    else 
    {
        return DpProperty::UnsetValue();
    }
}

WindowStateConvert* WindowStateConvert::Convert = new WindowStateConvert();

AutoObj WindowStateConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    String val = prop->ToString();

    if (val.Equals(_T("Maximized")))
    {
        return WindowStateBox::MaximizedBox;
    }
    else if (val.Equals(_T("Minimized")))
    {
        return WindowStateBox::MinimizedBox;
    }
    else
    {
        return WindowStateBox::NormalBox;
    }
}

DockConvert* DockConvert::Convert = new DockConvert();

AutoObj DockConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    String val = prop->ToString();

    if (val.Equals(_T("Left")))
    {
        return Integer::GetPosInt(Dock::dockLeft);
    }
    else if (val.Equals(_T("Top")))
    {
        return Integer::GetPosInt(Dock::dockTop);
    }
    else if (val.Equals(_T("Right")))
    {
        return Integer::GetPosInt(Dock::dockRight);
    }
    else if (val.Equals(_T("Bottom")))
    {
        return Integer::GetPosInt(Dock::dockBottom);
    }
    else
    {
        throw ArgumentException(_U("Value of dock in invalid!"), __FILELINE__);
    }
}

DockConvert::DockConvert()
{
}

AutoP0Convert* AutoP0Convert::Convert = new AutoP0Convert();

AutoObj AutoP0Convert::ConvertValue(DpProperty* dp, Object* prop)
{
    String val = prop->ToString();
    if (val.Equals(_U("Auto")))
    {
        return Integer::GetZeroInt();
    }
    else
    {
        return DpProperty::UnsetValue();
    }
}

AutoP0Convert::AutoP0Convert()
{

}

AutoN1Convert* AutoN1Convert::Convert = new AutoN1Convert();

AutoObj AutoN1Convert::ConvertValue(DpProperty* dp, Object* prop)
{
    String val = prop->ToString();
    if (val.Equals(_U("Auto")))
    {
        return new Integer(-1);
    }
    else 
    {
        return new Integer(val.ToInt());
    }
}

AutoN1Convert::AutoN1Convert()
{

}

FontWeightConvert* FontWeightConvert::Convert = new FontWeightConvert();

AutoObj FontWeightConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    static ValueDic<int, int> WeightDic;
    if (WeightDic.GetCount() == 0)
    {
        WeightDic.Add(String(_U("Black")).GetHashCode(), FontWeightStyle::fwBlack);
        WeightDic.Add(String(_U("Bold")).GetHashCode(), FontWeightStyle::fwBold);
        WeightDic.Add(String(_U("Light")).GetHashCode(), FontWeightStyle::fwLight);
        WeightDic.Add(String(_U("Medium")).GetHashCode(), FontWeightStyle::fwMedium);
        WeightDic.Add(String(_U("Normal")).GetHashCode(), FontWeightStyle::fwNormal);
        WeightDic.Add(String(_U("Thin")).GetHashCode(), FontWeightStyle::fwThin);
        WeightDic.Add(String(_U("SemiBold")).GetHashCode(), FontWeightStyle::fwSemiBold);
        WeightDic.Add(String(_U("ExtraBold")).GetHashCode(), FontWeightStyle::fwExtraBold);
        WeightDic.Add(String(_U("ExtraBlack")).GetHashCode(), FontWeightStyle::fwExtraBlack);
        WeightDic.Add(String(_U("ExtraLight")).GetHashCode(), FontWeightStyle::fwExtraLight);
    }

    // Black Bold Light Medium Normal Thin SemiBold ExtraBold ExtraBlack ExtraLight
    String val = prop->ToString();
    int iWeight = 0;

    if (WeightDic.TryGetValue(val.GetHashCode(), iWeight))
    {
        return new Integer(iWeight);
    }
    else
    {
        return new Integer(400);
    }
}

FontStyleConvert* FontStyleConvert::Convert = new FontStyleConvert();

AutoObj FontStyleConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    // Italic Normal Oblique

    String val = prop->ToString();

    if (val.Equals(_U("Italic")))
    {
        return Integer::GetPosInt(FontStyleStyle::fssItalic);
    }
    else if (val.Equals(_U("Underline")))
    {
        return Integer::GetPosInt(FontStyleStyle::fssUnderline);
    }
    else if (val.Equals(_U("Oblique")))
    {
        return Integer::GetPosInt(FontStyleStyle::fssOblique);
    }
    else
    {
        return Integer::GetPosInt(FontStyleStyle::fssNormal);
    }
}

TextTrimmingConvert* TextTrimmingConvert::Convert = new TextTrimmingConvert();

AutoObj TextTrimmingConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    String val = prop->ToString();

    if (val.Equals(_U("CharacterEllipsis")))
    {
        return Integer::GetPosInt(TextTrimming::tCharacterEllipsis);
    }
    else if (val.Equals(_U("WordEllipsis")))
    {
        return Integer::GetPosInt(TextTrimming::tWordEllipsis);
    }
    else
    {
        return Integer::GetPosInt(TextTrimming::tNone);
    }
}

TextAlignmentConvert* TextAlignmentConvert::Convert = new TextAlignmentConvert();

AutoObj TextAlignmentConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    String val = prop->ToString();

    if (val.Equals(_U("Left")))
    {
        return Integer::GetPosInt(TextAlignment::tLeft);
    }
    else if (val.Equals(_U("Center")))
    {
        return Integer::GetPosInt(TextAlignment::tCenter);
    }
    else if (val.Equals(_U("Right")))
    {
        return Integer::GetPosInt(TextAlignment::tRight);
    }
    else
    {
        return Integer::GetPosInt(TextAlignment::tJustify);
    }
}

TextDecorationsConvert* TextDecorationsConvert::Convert = new TextDecorationsConvert();

AutoObj TextDecorationsConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    String val = prop->ToString();
    if (val.Equals(_U("Underline")))
    {
        return TextDecorations::underline;
    }
    else if (val.Equals(_U("Overline")))
    {
        return TextDecorations::overline;
    }
    else if (val.Equals(_U("Baseline")))
    {
        return TextDecorations::baseline;
    }
    else if (val.Equals(_U("StrikeThrough")))
    {
        return TextDecorations::strikethrough;
    }
    else
    {
        return suic::DpProperty::UnsetValue();
    }
}

TextWrappingConvert* TextWrappingConvert::Convert = new TextWrappingConvert();

AutoObj TextWrappingConvert::ConvertValue(DpProperty* dp, Object* prop)
{
    String val = prop->ToString();

    if (val.Equals(_U("Wrap")))
    {
        return Integer::GetPosInt(TextWrapping::tWrap);
    }
    else if (val.Equals(_U("WrapWithOverflow")))
    {
        return Integer::GetPosInt(TextWrapping::tWrapWithOverflow);
    }
    else
    {
        return Integer::GetPosInt(TextWrapping::tNoWrap);
    }
}



}
