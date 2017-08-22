// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////////
// TextBase.cpp

#include "stdafx.h"

#include <Extend/RichText/TextBase.h>

#include "TextHost20.h"
#include "TextContainer.h"
#include "TextDocument.h"

namespace suic
{

ImplementRTTIOfClass(TextBoxBase, suic::Control)

TextBoxBase::TextBoxBase()
{
    _textContainer = new TextContainer(this);
}

TextBoxBase::~TextBoxBase()
{
}

suic::RoutedEvent* TextBoxBase::TextChangedEvent 
    = suic::EventHelper::RegisterRoutedEvent(_T("TextChanged")
    , suic::RoutingStrategy::Bubble, TextBoxBase::RTTIType(), TextBoxBase::RTTIType());

//------------------------------------------------------------

void TextBoxBase::LineDown()
{

}

void TextBoxBase::LineLeft()
{

}

void TextBoxBase::LineRight()
{

}

void TextBoxBase::LineUp()
{

}

void TextBoxBase::PageDown()
{

}

void TextBoxBase::PageLeft()
{

}
void TextBoxBase::PageRight()
{

}

void TextBoxBase::PageUp()
{

}

void TextBoxBase::ScrollToEnd()
{

}

void TextBoxBase::ScrollToHome()
{

}

void TextBoxBase::ScrollToHorizontalOffset(int offset)
{

}

void TextBoxBase::ScrollToVerticalOffset(int offset)
{

}

void TextBoxBase::SelectAll()
{

}

bool TextBoxBase::Undo()
{
    return 0;
}

bool TextBoxBase::Redo()
{
    return 0;
}

void TextBoxBase::Cut()
{

}

void TextBoxBase::Copy()
{

}

void TextBoxBase::Paste()
{

}

bool TextBoxBase::GetAcceptsReturn()
{
    return 0;
}

void TextBoxBase::SetAcceptsReturn(bool val)
{

}

bool TextBoxBase::GetAcceptsTab()
{
    return 0;
}

void TextBoxBase::SetAcceptsTab(bool val)
{

}

bool TextBoxBase::GetAutoWordSelection()
{
    return false;
}

void TextBoxBase::SetAutoWordSelection(bool val)
{

}

bool TextBoxBase::CanRedo()
{
    return 0;
}

bool TextBoxBase::CanUndo()
{
    return 0;
}

Brush* TextBoxBase::GetCaretBrush()
{
    return 0;
}

void TextBoxBase::SetCaretBrush(Brush* val)
{

}

Brush* TextBoxBase::GetSelectionBrush()
{
    return NULL;
}

void TextBoxBase::SetSelectionBrush(Brush* val)
{

}

int TextBoxBase::GetExtentHeight()
{
    return 0;
}

int TextBoxBase::GetExtentWidth()
{
    return 0;
}

int TextBoxBase::GetViewportHeight()
{
    return 0;
}

int TextBoxBase::GetViewportWidth()
{
    return 0;
}

int TextBoxBase::GetHorizontalOffset()
{
    return 0;
}

int TextBoxBase::GetVerticalOffset()
{
    return 0;
}

//---------------------------------------------------------

void TextBoxBase::ScrollToLine(int lineIndex)
{

}

void TextBoxBase::Select(int start, int length)
{

}

ScrollBarVisibility TextBoxBase::GetHorizontalScrollBarVisibility()
{
    return ScrollBarVisibility::AutoScroll;
}

void TextBoxBase::SetHorizontalScrollBarVisibility(ScrollBarVisibility val)
{

}

ScrollBarVisibility TextBoxBase::GetVerticalScrollBarVisibility()
{
    return ScrollBarVisibility::AutoScroll;
}

void TextBoxBase::SetVerticalScrollBarVisibility(ScrollBarVisibility val)
{

}

bool TextBoxBase::IsReadOnly()
{
    return 0;
}

void TextBoxBase::SetIsReadOnly(bool val)
{

}

bool TextBoxBase::IsUndoEnabled()
{
    return 0;
}

void TextBoxBase::SetIsUndoEnabled(bool val)
{

}

bool TextBoxBase::IsReadOnlyCaretVisible()
{
    return 0;
}

void TextBoxBase::SetIsReadOnlyCaretVisible(bool val)
{

}

Float TextBoxBase::GetSelectionOpacity()
{
    return 0;
}

void TextBoxBase::SetSelectionOpacity(Float val)
{

}

int TextBoxBase::GetUndoLimit()
{
    return 0;
}

void TextBoxBase::SetUndoLimit(int val)
{

}

//------------------------------------------------

int TextBoxBase::GetCaretIndex()
{
    return 0;
}

void TextBoxBase::SetCaretIndex(int val)
{

}

int TextBoxBase::GetLineIndexFromCharacterIndex(int charIndex)
{
    return 0;
}

int TextBoxBase::GetCharacterIndexFromLineIndex(int lineIndex)
{
    return 0;
}

int TextBoxBase::GetCharacterIndexFromPoint(Point point, bool snapToText)
{
    return 0;
}

int TextBoxBase::GetFirstVisibleLineIndex()
{
    return 0;
}

int TextBoxBase::GetLastVisibleLineIndex()
{
    return 0;
}

int TextBoxBase::GetLineCount()
{
    return 0;
}

int TextBoxBase::GetLineLength(int lineIndex)
{
    return 0;
}

String TextBoxBase::GetLineText(int lineIndex)
{
    return String();
}

int TextBoxBase::GetMaxLength()
{
    return 0;
}

void TextBoxBase::SetMaxLength(int val)
{

}

int TextBoxBase::GetMaxLines()
{
    return 0;
}

void TextBoxBase::SetMaxLines(int val)
{

}

int TextBoxBase::GetMinLines()
{
    return 0;
}

void TextBoxBase::SetMinLines(int val)
{

}

Float TextBoxBase::GetLineHeight()
{
    return 0;
}

//------------------------------------------------------------

void TextBoxBase::OnInitialized(EventArg& e)
{
    Control::OnInitialized(e);
}

void TextBoxBase::OnLoaded(suic::LoadedEventArg& e)
{
    /*_richTxt = ui::CreateRichText();
    _richTxt->__addRef();

    _richTxt->AddText(_U("Richtext test\n"));
    _richTxt->AddText(_U("Richtext 中文测试\n\n"));

    TextHost20* th20 = (TextHost20*)_texthost;
    BOOL bInited = th20->Init(this, _T(""));

    if (!bInited)
    {
        th20->Release();
        _texthost = NULL;
    }
    else
    {
        th20->SetInsetRect(&GetPadding());
        th20->SetClientRect(&GetClientRect());
        th20->SetRichTextFlag(TRUE);
        th20->SetMultiLine(TRUE);
        th20->SetTransparent(TRUE);
    }*/
}

void TextBoxBase::OnUnloaded(suic::LoadedEventArg& e)
{
}

void TextBoxBase::OnRenderSizeChanged(SizeChangedInfo& e)
{
    Control::OnRenderSizeChanged(e);
    /*GetTextHost()->SetClientRect(&GetClientRect());
    SIZEL szExtent = { -1, -1 };
    LONG lWidth = e.NewSize().cx;
    LONG lHeight = e.NewSize().cy;

    if (_richTxt)
    {
        _richTxt->SetViewport(suic::Rect(0, 0, GetActualWidth(), GetActualHeight()));
    }*/
}

void TextBoxBase::OnRender(suic::Drawing * drawing)
{
}

void TextBoxBase::OnSetCursor(suic::CursorEventArg& e)
{
    ;
    Control::OnSetCursor(e);
}
void TextBoxBase::OnTextInput(suic::KeyboardEventArg& e)
{
    e.SetHandled(true);
}

void TextBoxBase::OnKeyDown(suic::KeyboardEventArg& e)
{
    e.SetHandled(true);
}

void TextBoxBase::OnKeyUp(suic::KeyboardEventArg& e)
{
    e.SetHandled(true);
}

void TextBoxBase::OnGotFocus(suic::RoutedEventArg& e)
{
    e.SetHandled(true);
}

void TextBoxBase::OnLostFocus(suic::RoutedEventArg& e)
{
    e.SetHandled(true);
}

void TextBoxBase::OnMouseEnter(suic::MouseButtonEventArg& e)
{
    Control::OnMouseEnter(e);
    OnMouseMove(e);
}

void TextBoxBase::OnMouseMove(suic::MouseButtonEventArg& e)
{
    e.SetHandled(true);
}

void TextBoxBase::OnMouseLeave(suic::MouseButtonEventArg& e)
{
    Control::OnMouseLeave(e);
    OnMouseMove(e);
}

void TextBoxBase::OnMouseLeftButtonDown(suic::MouseButtonEventArg& e)
{
    Control::OnMouseLeftButtonDown(e);
    e.SetHandled(true);
}

void TextBoxBase::OnMouseLeftButtonDbclk(suic::MouseButtonEventArg& e)
{
}

void TextBoxBase::OnMouseLeftButtonUp(suic::MouseButtonEventArg& e)
{
    Control::OnMouseLeftButtonUp(e);
    e.SetHandled(true);
}

void TextBoxBase::OnMouseRightButtonDown(suic::MouseButtonEventArg& e)
{
    e.SetHandled(true);
}

void TextBoxBase::OnMouseRightButtonUp(suic::MouseButtonEventArg& e)
{
    e.SetHandled(true);
}

}
