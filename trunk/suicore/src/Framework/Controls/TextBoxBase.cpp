// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////////
// TextBoxBase.cpp

#include <Framework/Controls/TextElement.h>
#include <Framework/Controls/TextBoxBase.h>

#include <System/Tools/EventHelper.h>
#include <System/Graphics/SolidcolorBrush.h>

namespace suic
{

ImplementRTTIOfClass(TextEdit, Control)

DpProperty* TextEdit::TextProperty;

DpProperty* TextEdit::CaretBrushProperty;

DpProperty* TextEdit::HorizontalScrollBarVisibilityProperty;
DpProperty* TextEdit::VerticalScrollBarVisibilityProperty;

DpProperty* TextEdit::IsReadOnlyCaretVisibleProperty;
DpProperty* TextEdit::IsReadOnlyProperty;

DpProperty* TextEdit::SelectionBrushProperty;
DpProperty* TextEdit::SelectionTextBrushProperty;

DpProperty* TextEdit::TextWrappingProperty;

//DpProperty* TextBoxBase::SelectionOpacityProperty;

RoutedEvent* TextEdit::TextChangedEvent;
RoutedEvent* TextEdit::SelectionChangedEvent;

TextEdit::TextEdit()
    : _scrollViewer(NULL)
{
}

TextEdit::~TextEdit()
{
}

String TextEdit::GetText()
{
    return GetValue(TextProperty)->ToString();
}

void TextEdit::SetText(const String& text)
{
    SetValue(TextProperty, new OString(text));
}

bool TextEdit::StaticInit()
{
    TextChangedEvent = EventHelper::RegisterRoutedEvent(_T("TextChanged"), RoutingStrategy::Bubble, TextEdit::RTTIType(), TextEdit::RTTIType());
    SelectionChangedEvent = EventHelper::RegisterRoutedEvent(_T("SelectionChanged"), RoutingStrategy::Bubble, TextEdit::RTTIType(), TextEdit::RTTIType());

    TextProperty = DpProperty::Register(_T("Text"), RTTIType(), OString::RTTIType(), new PropMetadata(OString::EmptyString, PropMetadataOptions::AffectsNone, &TextEdit::OnTextPropChanged));

    CaretBrushProperty =  DpProperty::Register(_T("CaretBrush"), RTTIType(), Brush::RTTIType(), new PropMetadata(new SolidColorBrush(0xFF000000), PropMetadataOptions::AffectsNone));

    HorizontalScrollBarVisibilityProperty = ScrollViewer::HorizontalScrollBarVisibilityProperty->AddOwner(RTTIType(), new PropMetadata(Integer::GetPosInt(ScrollBarVisibility::AutoScroll)));
    VerticalScrollBarVisibilityProperty = ScrollViewer::VerticalScrollBarVisibilityProperty->AddOwner(RTTIType(), new PropMetadata(Integer::GetPosInt(ScrollBarVisibility::AutoScroll)));

    IsReadOnlyCaretVisibleProperty =  DpProperty::Register(_T("IsReadOnlyCaretVisible"), RTTIType(), Boolean::RTTIType(), new PropMetadata(Boolean::True, PropMetadataOptions::AffectsRender, &TextEdit::OnIsReadOnlyCaretVisiblePropChanged));

    SelectionBrushProperty =  DpProperty::Register(_T("SelectionBrush"), RTTIType(), Brush::RTTIType(), new PropMetadata(NULL, PropMetadataOptions::AffectsNone, &TextEdit::OnSelectionBrushPropChanged));
    SelectionTextBrushProperty =  DpProperty::Register(_T("SelectionTextBrush"), RTTIType(), Brush::RTTIType(), new PropMetadata(NULL, PropMetadataOptions::AffectsNone, &TextEdit::OnSelectionTextBrushPropChanged));

    TextWrappingProperty = DpProperty::Register(_T("TextWrapping"), RTTIType(), Boolean::RTTIType(), new PropMetadata(Boolean::False, PropMetadataOptions::AffectsMeasure, &TextEdit::OnTextWrappingPropChanged));
    IsReadOnlyProperty = DpProperty::Register(_T("IsReadOnly"), RTTIType(), Boolean::RTTIType(), new PropMetadata(Boolean::False, PropMetadataOptions::AffectsRender, &TextEdit::OnIsReadOnlyPropChanged));
    
    return true;
}

void TextEdit::OnTextPropChanged(DpObject* d, DpPropChangedEventArg& e)
{
    TextEdit* textBox = DynamicCast<TextEdit>(d);
    if (textBox)
    {
        TextChangedEventArg tce(textBox->GetText());
        tce.SetRoutedEvent(TextChangedEvent);
        textBox->OnTextChanged(tce);
    }
}

void TextEdit::OnIsReadOnlyPropChanged(DpObject* d, DpPropChangedEventArg& e)
{
    TextEdit* textBox = DynamicCast<TextEdit>(d);
    if (textBox)
    {
        textBox->OnIsReadOnlyChanged(e);
    }
}

void TextEdit::OnIsReadOnlyCaretVisiblePropChanged(DpObject* d, DpPropChangedEventArg& e)
{
    TextEdit* textBox = DynamicCast<TextEdit>(d);
    if (textBox)
    {
        textBox->OnIsReadOnlyCaretVisibleChanged(e);
    }
}

void TextEdit::OnSelectionBrushPropChanged(DpObject* d, DpPropChangedEventArg& e)
{
    TextEdit* textBox = DynamicCast<TextEdit>(d);
    if (textBox)
    {
        textBox->OnSelectionBrushChanged(e);
    }
}

void TextEdit::OnSelectionTextBrushPropChanged(DpObject* d, DpPropChangedEventArg& e)
{
    TextEdit* textBox = DynamicCast<TextEdit>(d);
    if (textBox)
    {
        textBox->OnSelectionTextBrushChanged(e);
    }
}

void TextEdit::OnTextWrappingPropChanged(DpObject* d, DpPropChangedEventArg& e)
{
    TextEdit* textBox = DynamicCast<TextEdit>(d);
    if (textBox)
    {
        textBox->OnTextWrappingChanged(e);
    }
}

//------------------------------------------------------------

void TextEdit::OnIsReadOnlyChanged(DpPropChangedEventArg& e)
{

}

void TextEdit::OnIsReadOnlyCaretVisibleChanged(DpPropChangedEventArg& e)
{

}

void TextEdit::OnSelectionBrushChanged(DpPropChangedEventArg& e)
{

}

void TextEdit::OnSelectionTextBrushChanged(DpPropChangedEventArg& e)
{

}

void TextEdit::OnTextWrappingChanged(DpPropChangedEventArg& e)
{

}

//------------------------------------------------------------

void TextEdit::LineDown()
{
    OnScrollToPos(eScrollType::eScrollLineDown, 0);
}

void TextEdit::LineLeft()
{
    OnScrollToPos(eScrollType::eScrollLineLeft, 0);
}

void TextEdit::LineRight()
{
    OnScrollToPos(eScrollType::eScrollLineRight, 0);
}

void TextEdit::LineUp()
{
    OnScrollToPos(eScrollType::eScrollLineUp, 0);
}

void TextEdit::PageDown()
{
    OnScrollToPos(eScrollType::eScrollPageDown, 0);
}

void TextEdit::PageLeft()
{
    OnScrollToPos(eScrollType::eScrollPageLeft, 0);
}

void TextEdit::PageRight()
{
    OnScrollToPos(eScrollType::eScrollPageRight, 0);
}

void TextEdit::PageUp()
{
    OnScrollToPos(eScrollType::eScrollPageUp, 0);
}

void TextEdit::ScrollToEnd()
{
    OnScrollToPos(eScrollType::eScrollToEnd, AxisDirection::xyAxis);
}

void TextEdit::ScrollToHome()
{
    OnScrollToPos(eScrollType::eScrollToHome, AxisDirection::xyAxis);
}

void TextEdit::ScrollToHorizontalOffset(int offset)
{
    OnScrollToPos(eScrollType::eScrollToHori, offset);
}

void TextEdit::ScrollToVerticalOffset(int offset)
{
    OnScrollToPos(eScrollType::eScrollToVert, offset);
}

void TextEdit::Cut()
{
    OnCut();
}

void TextEdit::Copy()
{
    OnCopy();
}

void TextEdit::Paste()
{
    OnPaste();
}

Brush* TextEdit::GetCaretBrush()
{
    return DynamicCast<Brush>(GetValue(CaretBrushProperty));
}

void TextEdit::SetCaretBrush(Brush* val)
{
    SetValue(CaretBrushProperty, val);
}

Brush* TextEdit::GetSelectionBrush()
{
    return DynamicCast<Brush>(GetValue(SelectionBrushProperty));
}

void TextEdit::SetSelectionBrush(Brush* val)
{
    SetValue(SelectionBrushProperty, val);
}

int TextEdit::GetExtentHeight()
{
    return _scrollViewer->GetExtentHeight();
}

int TextEdit::GetExtentWidth()
{
    return _scrollViewer->GetExtentWidth();
}

int TextEdit::GetViewportHeight()
{
    return _scrollViewer->GetViewportHeight();
}

int TextEdit::GetViewportWidth()
{
    return _scrollViewer->GetViewportWidth();
}

int TextEdit::GetHorizontalOffset()
{
    return _scrollViewer->GetHorizontalOffset();
}

int TextEdit::GetVerticalOffset()
{
    return _scrollViewer->GetVerticalOffset();
}

//---------------------------------------------------------

void TextEdit::ScrollToLine(int lineIndex)
{
    OnScrollToPos(eScrollType::eScrollToLine, lineIndex);
}

ScrollBarVisibility TextEdit::GetHorizontalScrollBarVisibility()
{
    return ScrollBarVisibility::AutoScroll;
}

void TextEdit::SetHorizontalScrollBarVisibility(ScrollBarVisibility val)
{
    _scrollViewer->SetHorizontalScrollBarVisibility(val);
}

ScrollBarVisibility TextEdit::GetVerticalScrollBarVisibility()
{
    return ScrollBarVisibility::AutoScroll;
}

void TextEdit::SetVerticalScrollBarVisibility(ScrollBarVisibility val)
{
    _scrollViewer->SetVerticalScrollBarVisibility(val);
}

bool TextEdit::IsReadOnly()
{
    return GetValue(IsReadOnlyProperty)->ToBool();
}

void TextEdit::SetIsReadOnly(bool val)
{
    SetValue(IsReadOnlyProperty, BoolToBoolean(val));
}

bool TextEdit::IsReadOnlyCaretVisible()
{
    return GetValue(IsReadOnlyCaretVisibleProperty)->ToBool();
}

void TextEdit::SetIsReadOnlyCaretVisible(bool val)
{
    SetValue(IsReadOnlyCaretVisibleProperty, BoolToBoolean(val));
}

Float TextEdit::GetSelectionOpacity()
{
    return 0;
}

void TextEdit::SetSelectionOpacity(Float val)
{

}

bool TextEdit::GetTextWrapping()
{
    return GetValue(TextWrappingProperty)->ToBool();
}

void TextEdit::SetTextWrapping(bool val)
{
    SetValue(TextWrappingProperty, BoolToBoolean(val));
}

void TextEdit::OnScrollChanged(Element* sender, ScrollChangedEventArg& e)
{
}

void TextEdit::OnCut()
{

}

void TextEdit::OnPaste()
{

}

void TextEdit::OnCopy()
{

}

void TextEdit::OnScrollToPos(eScrollType iType, int iFlag)
{
    if (_scrollViewer == NULL)
    {
        return;
    }

    switch (iType)
    {
        case eScrollType::eScrollLineLeft:
            _scrollViewer->LineLeft();
            break;

        case eScrollType::eScrollLineRight:
            _scrollViewer->LineRight();
            break;

        case eScrollType::eScrollLineUp:
            _scrollViewer->LineUp();
            break;

        case eScrollType::eScrollLineDown:
            _scrollViewer->LineDown();
            break;

        case eScrollType::eScrollPageLeft:
            _scrollViewer->PageLeft();
            break;

        case eScrollType::eScrollPageRight:
            _scrollViewer->PageRight();
            break;

        case eScrollType::eScrollPageUp:
            _scrollViewer->PageUp();
            break;

        case eScrollType::eScrollPageDown:
            _scrollViewer->PageDown();
            break;

        case eScrollType::eScrollToHome:
            _scrollViewer->ScrollToHome((AxisDirection)iFlag);
            break;

        case eScrollType::eScrollToEnd:
            _scrollViewer->ScrollToEnd((AxisDirection)iFlag);
            break;

        case eScrollType::eScrollToHori:
            _scrollViewer->ScrollToHorizontalOffset(iFlag);
            break;

        case eScrollType::eScrollToVert:
            _scrollViewer->ScrollToVerticalOffset(iFlag);
            break;

        default:
            ;
    }
}

void TextEdit::OnTextChanged()
{
    TextChangedEventArg e;
    e.OnOverrideOriginalSource(this);
    e.SetRoutedEvent(TextChangedEvent);
    OnTextChanged(e);
}

void TextEdit::OnTextSelectionChanged()
{
    RoutedEventArg e(this, SelectionChangedEvent);
    OnSelectionChanged(e);
}

void TextEdit::OnSelectionChanged(RoutedEventArg& e)
{
    RaiseEvent(&e);
}

void TextEdit::OnTextChanged(TextChangedEventArg& e)
{
    RaiseEvent(&e);
}

}
