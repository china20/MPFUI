// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// Sharpui界面库允许任何个人或团体免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////////
// UIEdit.cpp

#include <Framework/Controls/TextBox.h>
#include <Framework/Controls/TextSimple.h>

#include <System/Windows/CommandSource.h>
#include <System/Tools/EventHelper.h>
#include <System/Tools/CommandHelper.h>

namespace suic
{

DpProperty* UIEdit::IsSingleLineProperty;
DpProperty* UIEdit::PasswordCharProperty;

static const String ContentHostName = "PART_ContentHost";

ImplementRTTIOfClass(UIEdit, TextEditor)

UIEdit::UIEdit()
{
    _textEditor = new TextSimple();
    _textEditor->ref();
}

UIEdit::~UIEdit()
{
    if (_textEditor)
    {
        _textEditor->unref();
    }
}

void UIEdit::OnIsSingleLinePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    UIEdit* tb = DynamicCast<UIEdit>(d);
    if (tb != NULL)
    {
        ((TextSimple*)tb->_textEditor)->SetSingleLine(e->GetNewValue()->ToBool());
    }
}

Char UIEdit::GetPasswordChar()
{
    String strChar = GetValue(PasswordCharProperty)->ToString();
    if (strChar.Length() == 1)
    {
        return strChar[0];
    }
    else
    {
        return Char();
    }
}

void UIEdit::SetPasswordChar(Char ch)
{
    SetValue(PasswordCharProperty, new OString(ch)); 
}

String UIEdit::GetPassword()
{
    if (_textEditor != NULL)
    {
        return ((TextSimple*)_textEditor)->GetText();
    }
    else
    {
        return String();
    }
}

bool UIEdit::StaticInit()
{
    IsSingleLineProperty = TextElement::IsSingleLineProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender, &UIEdit::OnIsSingleLinePropChanged));
    PasswordCharProperty = TextSimple::PasswordCharProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(new OString(_U("")), PropMetadataOptions::AffectsRender));
    return true;
}

void UIEdit::Cut()
{
    ((TextSimple*)_textEditor)->Cut();
}

void UIEdit::Copy()
{
    ((TextSimple*)_textEditor)->Copy();
}

void UIEdit::Paste()
{
    ((TextSimple*)_textEditor)->Paste();
}

void UIEdit::OnTextCaretChanged(int offset, int diff, bool hori)
{
    if (NULL != _scrollViewer)
    {
        if (hori)
        {
            int iViewWidth = _scrollViewer->GetViewportWidth();
            int iHoriOffset = _scrollViewer->GetHorizontalOffset();

            if (offset < iHoriOffset)
            {
                offset = max(0, offset);
                _scrollViewer->SetHorizontalOffset(offset);
            }
            else if (offset + diff > (iHoriOffset + iViewWidth))
            {
                offset += diff;
                _scrollViewer->UpdateMeasure();
                iViewWidth = _scrollViewer->GetViewportWidth();
                offset = max(0, offset - iViewWidth);
                _scrollViewer->SetHorizontalOffset(offset);
            }
        }
        else
        {
            _scrollViewer->UpdateMeasure();

            int iViewHeight = _scrollViewer->GetViewportHeight();
            int iVertOffset = _scrollViewer->GetVerticalOffset();

            if (offset < iVertOffset)
            {
                offset = max(0, offset);
                _scrollViewer->SetVerticalOffset(offset);
            }
            else if (offset + diff > (iVertOffset + iViewHeight))
            {
                offset += diff;
                offset = max(0, offset - iViewHeight);
                _scrollViewer->SetVerticalOffset(offset);
            }
        }
    }
}

void UIEdit::OnScrollChanged(Element* sender, ScrollChangedEventArg* e)
{
    if (_textEditor != NULL)
    {
        ;
    }

    e->SetHandled(true);
}

void UIEdit::BindToTemplatedParent(DpProperty* dp, DpProperty* target)
{
    //if (!_textEditor->HasNonDefaultValue(target))
    {
        Binding* binding = new Binding();
        binding->SetSource(this);
        binding->SetPath(PropertyPath(dp->GetName()));
        _textEditor->SetBinding(target, binding);
    }
}

void UIEdit::OnApplyTemplate()
{
    Control::OnApplyTemplate();

    Element* fe = GetTemplateChild(ContentHostName);
    IAddChild* textHost = DynamicCast<IAddChild>(fe);

    _scrollViewer = RTTIEqualCast<ScrollViewer>(fe);

    if (NULL != _scrollViewer)
    {
        ScrollChangedEventHandler* objEvent = new ScrollChangedEventHandler(this, &UIEdit::OnScrollChanged);

        _scrollViewer->RemoveScrollChanged(objEvent);
        _scrollViewer->AddScrollChanged(objEvent);
    }
    
    if (textHost)
    {
        textHost->AddChild(_textEditor);

        BindToTemplatedParent(UIEdit::PasswordCharProperty, TextSimple::PasswordCharProperty);
        BindToTemplatedParent(UIEdit::FontSizeProperty, TextSimple::FontSizeProperty);
        BindToTemplatedParent(UIEdit::FontFamilyProperty, TextSimple::FontFamilyProperty);
        BindToTemplatedParent(UIEdit::ForegroundProperty, TextSimple::ForegroundProperty);
        BindToTemplatedParent(UIEdit::TextProperty, TextSimple::TextProperty);

        if (IsInitialized())
        {
            _textEditor->RecursiveInit();
        }
    }
}

void UIEdit::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    Focus();
    e->SetHandled(true);
}

void UIEdit::OnGotFocus(RoutedEventArg* e)
{
    e->SetHandled(true);
    if (_textEditor != NULL)
    {
        _textEditor->OnGotFocus(e);
    }
}

void UIEdit::OnLostFocus(RoutedEventArg* e)
{
    e->SetHandled(true);
    if (_textEditor != NULL)
    {
        _textEditor->OnLostFocus(e);
    }
}

void UIEdit::OnKeyDown(KeyboardEventArg* e)
{
    Control::OnKeyDown(e);

    if (_textEditor != NULL)
    {
        _textEditor->OnKeyDown(e);
    }
}

void UIEdit::OnTextInput(KeyboardEventArg* e)
{
    Control::OnTextInput(e);
    if (_textEditor != NULL)
    {
        _textEditor->OnTextInput(e);
    }
}

void UIEdit::OnTextChanged(TextChangedEventArg* e)
{
    if (_textEditor != NULL)
    {
        ((TextSimple*)_textEditor)->SetText(e->GetText());
    }
    TextEditor::OnTextChanged(e);
}

}
