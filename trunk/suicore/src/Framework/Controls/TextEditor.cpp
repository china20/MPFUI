// ======================================================================
//
// Copyright (c) 2008-2022 汪荣, Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// TextEditor.cpp

#include <Framework/Controls/TextElement.h>
#include <Framework/Controls/TextEditor.h>

#include <System/Tools/EventHelper.h>
#include <System/Tools/CommandOp.h>
#include <System/Graphics/SolidcolorBrush.h>

namespace suic
{

ImplementRTTIOfClass(TextEditor, Control)

DpProperty* TextEditor::TextProperty;
DpProperty* TextEditor::WatermarkProperty;
DpProperty* TextEditor::CaretBrushProperty;

DpProperty* TextEditor::HorizontalScrollBarVisibilityProperty;
DpProperty* TextEditor::VerticalScrollBarVisibilityProperty;

DpProperty* TextEditor::IsReadOnlyCaretVisibleProperty;
DpProperty* TextEditor::IsReadOnlyProperty;

DpProperty* TextEditor::SelectionBrushProperty;
DpProperty* TextEditor::SelectionTextBrushProperty;

DpProperty* TextEditor::TextWrappingProperty;

//DpProperty* TextBoxBase::SelectionOpacityProperty;

RoutedEvent* TextEditor::TextChangedEvent;
RoutedEvent* TextEditor::SelectionChangedEvent;

TextEditor::TextEditor()
    : _scrollViewer(NULL)
{
}

TextEditor::~TextEditor()
{
}

String TextEditor::GetText()
{
    return GetValue(TextProperty)->ToString();
}

void TextEditor::SetText(const String& text)
{
    VerifyAccess(__FILELINE__);
    SetValue(TextProperty, new OString(text));
}

void TextEditor::StaticInit()
{
    if (NULL == TextProperty)
    {
        ScrollViewer::StaticInit();

        TextChangedEvent = EventHelper::RegisterRoutedEvent(_T("TextChanged"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), TextEditor::RTTIType());
        SelectionChangedEvent = EventHelper::RegisterRoutedEvent(_T("SelectionChanged"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), TextEditor::RTTIType());

        WatermarkProperty = DpProperty::Register(_T("Watermark"), RTTIType(), OString::RTTIType(), DpPropMemory::GetPropMeta(OString::EmptyString, PropMetadataOptions::AffectsRender));
        TextProperty = DpProperty::Register(_T("Text"), RTTIType(), OString::RTTIType(), DpPropMemory::GetPropMeta(OString::EmptyString, PropMetadataOptions::AffectsRender, &TextEditor::OnTextPropChanged));
        CaretBrushProperty =  DpProperty::Register(_T("CaretBrush"), RTTIType(), Brush::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));

        HorizontalScrollBarVisibilityProperty = ScrollViewer::HorizontalScrollBarVisibilityProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(ScrollBarVisibility::sbvCollapsed)));
        VerticalScrollBarVisibilityProperty = ScrollViewer::VerticalScrollBarVisibilityProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(ScrollBarVisibility::sbvCollapsed)));

        IsReadOnlyProperty = DpProperty::Register(_T("IsReadOnly"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender, &TextEditor::OnIsReadOnlyPropChanged));
        IsReadOnlyCaretVisibleProperty =  DpProperty::Register(_T("IsReadOnlyCaretVisible"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsRender, &TextEditor::OnIsReadOnlyCaretVisiblePropChanged));

        SelectionBrushProperty =  DpProperty::Register(_T("SelectionBrush"), RTTIType(), Brush::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &TextEditor::OnSelectionBrushPropChanged));
        SelectionTextBrushProperty =  DpProperty::Register(_T("SelectionTextBrush"), RTTIType(), Brush::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &TextEditor::OnSelectionTextBrushPropChanged));

        TextWrappingProperty = DpProperty::Register(_T("TextWrapping"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(suic::Integer::GetZeroInt(), PropMetadataOptions::AffectsMeasure, &TextEditor::OnTextWrappingPropChanged));
        
        HorizontalScrollBarVisibilityProperty->SetConvertValueCb(SBVisibilityConvert::Convert);
        VerticalScrollBarVisibilityProperty->SetConvertValueCb(SBVisibilityConvert::Convert);
        TextWrappingProperty->SetConvertValueCb(TextWrappingConvert::Convert);

        //EventHelper::RegisterClassHandler(RTTIType(), ExecutedEvent, new ExecutedRoutedEventHandler(OnTextExecutedThunk), false);
    }
}

void TextEditor::OnTextPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TextEditor* textBox = RTTICast<TextEditor>(d);
    if (textBox)
    {
        TextChangedEventArg tce(textBox->GetText());
        tce.SetRoutedEvent(TextChangedEvent);
        textBox->OnTextChanged(&tce);
    }
}

void TextEditor::OnCaretBrushPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TextEditor* textBox = RTTICast<TextEditor>(d);
    if (textBox)
    {
        textBox->OnCaretBrushChanged(RTTICast<Brush>(e->GetNewValue()));
    }
}

void TextEditor::OnIsReadOnlyPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TextEditor* textBox = RTTICast<TextEditor>(d);
    if (textBox)
    {
        textBox->OnIsReadOnlyChanged(e);
    }
}

void TextEditor::OnIsReadOnlyCaretVisiblePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TextEditor* textBox = RTTICast<TextEditor>(d);
    if (textBox)
    {
        textBox->OnIsReadOnlyCaretVisibleChanged(e);
    }
}

void TextEditor::OnSelectionBrushPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TextEditor* textBox = RTTICast<TextEditor>(d);
    if (textBox)
    {
        textBox->OnSelectionBrushChanged(e);
    }
}

void TextEditor::OnSelectionTextBrushPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TextEditor* textBox = RTTICast<TextEditor>(d);
    if (textBox)
    {
        textBox->OnSelectionTextBrushChanged(e);
    }
}

void TextEditor::OnTextWrappingPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TextEditor* textBox = RTTICast<TextEditor>(d);
    if (textBox)
    {
        textBox->OnTextWrappingChanged(e);
    }
}

//------------------------------------------------------------

void TextEditor::OnIsReadOnlyChanged(DpPropChangedEventArg* e)
{

}

void TextEditor::OnIsReadOnlyCaretVisibleChanged(DpPropChangedEventArg* e)
{

}

void TextEditor::OnSelectionBrushChanged(DpPropChangedEventArg* e)
{

}

void TextEditor::OnSelectionTextBrushChanged(DpPropChangedEventArg* e)
{

}

void TextEditor::OnTextWrappingChanged(DpPropChangedEventArg* e)
{

}

//------------------------------------------------------------

void TextEditor::LineDown()
{
    OnScrollToPos(eScrollType::eScrollLineDown, 0);
}

void TextEditor::LineLeft()
{
    OnScrollToPos(eScrollType::eScrollLineLeft, 0);
}

void TextEditor::LineRight()
{
    OnScrollToPos(eScrollType::eScrollLineRight, 0);
}

void TextEditor::LineUp()
{
    OnScrollToPos(eScrollType::eScrollLineUp, 0);
}

void TextEditor::PageDown()
{
    OnScrollToPos(eScrollType::eScrollPageDown, 0);
}

void TextEditor::PageLeft()
{
    OnScrollToPos(eScrollType::eScrollPageLeft, 0);
}

void TextEditor::PageRight()
{
    OnScrollToPos(eScrollType::eScrollPageRight, 0);
}

void TextEditor::PageUp()
{
    OnScrollToPos(eScrollType::eScrollPageUp, 0);
}

void TextEditor::ScrollToEnd()
{
    OnScrollToPos(eScrollType::eScrollToEnd, AxisDirection::xyAxis);
}

void TextEditor::ScrollToHome()
{
    OnScrollToPos(eScrollType::eScrollToHome, AxisDirection::xyAxis);
}

void TextEditor::ScrollToHorizontalOffset(int offset)
{
    OnScrollToPos(eScrollType::eScrollToHori, offset);
}

void TextEditor::ScrollToVerticalOffset(int offset)
{
    OnScrollToPos(eScrollType::eScrollToVert, offset);
}

void TextEditor::Cut()
{
}

void TextEditor::Copy()
{
}

void TextEditor::Paste()
{
}

void TextEditor::SelectAll()
{
    ;
}

Brush* TextEditor::GetCaretBrush()
{
    return DynamicCast<Brush>(GetValue(CaretBrushProperty));
}

void TextEditor::SetCaretBrush(Brush* val)
{
    SetValue(CaretBrushProperty, val);
}

Brush* TextEditor::GetSelectionBrush()
{
    return DynamicCast<Brush>(GetValue(SelectionBrushProperty));
}

void TextEditor::SetSelectionBrush(Brush* val)
{
    SetValue(SelectionBrushProperty, val);
}

int TextEditor::GetExtentHeight()
{
    return _scrollViewer->GetExtentHeight();
}

int TextEditor::GetExtentWidth()
{
    return _scrollViewer->GetExtentWidth();
}

int TextEditor::GetViewportHeight()
{
    return _scrollViewer->GetViewportHeight();
}

int TextEditor::GetViewportWidth()
{
    return _scrollViewer->GetViewportWidth();
}

int TextEditor::GetHorizontalOffset()
{
    return _scrollViewer->GetHorizontalOffset();
}

int TextEditor::GetVerticalOffset()
{
    return _scrollViewer->GetVerticalOffset();
}

//---------------------------------------------------------

void TextEditor::ScrollToLine(int lineIndex)
{
    OnScrollToPos(eScrollType::eScrollToLine, lineIndex);
}

ScrollBarVisibility TextEditor::GetHorizontalScrollBarVisibility()
{
    return ScrollBarVisibility::sbvAutoScroll;
}

void TextEditor::SetHorizontalScrollBarVisibility(ScrollBarVisibility val)
{
    _scrollViewer->SetHorizontalScrollBarVisibility(val);
}

ScrollBarVisibility TextEditor::GetVerticalScrollBarVisibility()
{
    return ScrollBarVisibility::sbvAutoScroll;
}

void TextEditor::SetVerticalScrollBarVisibility(ScrollBarVisibility val)
{
    _scrollViewer->SetVerticalScrollBarVisibility(val);
}

bool TextEditor::IsReadOnly()
{
    return GetValue(IsReadOnlyProperty)->ToBool();
}

void TextEditor::SetIsReadOnly(bool val)
{
    SetValue(IsReadOnlyProperty, BOOLTOBOOLEAN(val));
}

bool TextEditor::IsReadOnlyCaretVisible()
{
    return GetValue(IsReadOnlyCaretVisibleProperty)->ToBool();
}

void TextEditor::SetIsReadOnlyCaretVisible(bool val)
{
    SetValue(IsReadOnlyCaretVisibleProperty, BOOLTOBOOLEAN(val));
}

Float TextEditor::GetSelectionOpacity()
{
    return 0;
}

void TextEditor::SetSelectionOpacity(Float val)
{

}

TextWrapping TextEditor::GetTextWrapping()
{
    return (TextWrapping)(GetValue(TextWrappingProperty)->ToInt());
}

void TextEditor::SetTextWrapping(TextWrapping val)
{
    SetValue(TextWrappingProperty, suic::Integer::GetPosInt(val));
}

void TextEditor::AddTextChanged(RoutedEventHandler* h)
{
    AddHandler(TextChangedEvent, h);
}

void TextEditor::RemoveTextChanged(RoutedEventHandler* h)
{
    RemoveHandler(TextChangedEvent, h);
}

void TextEditor::OnScrollChanged(Element* sender, ScrollChangedEventArg* e)
{
}

void TextEditor::OnScrollToPos(eScrollType iType, int iFlag)
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

void TextEditor::OnTextChanged()
{
    TextChangedEventArg e;

    e.OnOverrideOriginalSource(this);
    e.SetRoutedEvent(TextChangedEvent);

    RaiseEvent(&e);
}

void TextEditor::OnCaretBrushChanged(Brush* brush)
{

}

void TextEditor::OnTextSelectionChanged()
{
    RoutedEventArg e(this, SelectionChangedEvent);
    
    RaiseEvent(&e);
}

void TextEditor::OnSelectionChanged(RoutedEventArg* e)
{
    RaiseEvent(e);
}

void TextEditor::OnTextChanged(TextChangedEventArg* e)
{
    RaiseEvent(e);
}

}
