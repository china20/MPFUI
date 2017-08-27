// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// Sharpui界面库允许任何个人或团体免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////////
// TextBox.cpp

#include <Framework/Controls/TextEdit.h>
#include <Framework/Controls/TextSimple.h>

namespace suic
{

DpProperty* TextBox::IsSingleLineProperty;
DpProperty* TextBox::PasswordCharProperty;

static const String ContentHostName = "PART_ContentHost";

ImplementRTTIOfClass(TextBox, TextEdit)

TextBox::TextBox()
{
    _textEditor = new TextSimple();
    _textEditor->ref();
}

TextBox::~TextBox()
{
    if (_textEditor)
    {
        _textEditor->unref();
    }
}

void TextBox::OnIsSingleLinePropChanged(DpObject* d, DpPropChangedEventArg& e)
{
    TextBox* tb = DynamicCast<TextBox>(d);
    if (tb != NULL)
    {
        ((TextSimple*)tb->_textEditor)->SetSingleLine(e.GetNewValue()->ToBool());
    }
}

Char TextBox::GetPasswordChar()
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

void TextBox::SetPasswordChar(Char ch)
{
    SetValue(PasswordCharProperty, new OString(ch)); 
}

String TextBox::GetPassword()
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

bool TextBox::StaticInit()
{
    IsSingleLineProperty = TextElement::IsSingleLineProperty->AddOwner(RTTIType(), new PropMetadata(Boolean::False, PropMetadataOptions::AffectsRender, &TextBox::OnIsSingleLinePropChanged));
    //TextProperty = DpProperty::Register(_T("Text"), RTTIType(), OString::RTTIType(), new PropMetadata(OString::EmptyString, PropMetadataOptions::AffectsNone, &TextBox::OnTextPropChanged));
    PasswordCharProperty = TextSimple::PasswordCharProperty->AddOwner(RTTIType(), new PropMetadata(new OString(_U("")), PropMetadataOptions::AffectsRender));

    return true;
}

void TextBox::OnTextCaretChanged(int offset, int diff, bool hori)
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

void TextBox::OnScrollChanged(Element* sender, ScrollChangedEventArg& e)
{
    if (_textEditor != NULL)
    {
        ;
    }

    e.SetHandled(true);
}

void TextBox::BindToTemplatedParent(DpProperty* dp, DpProperty* target)
{
    //if (!_textEditor->HasNonDefaultValue(target))
    {
        Binding* binding = new Binding();
        binding->SetSource(this);
        binding->SetPath(PropertyPath(dp->GetName()));
        _textEditor->SetBinding(target, binding);
    }
}

void TextBox::OnApplyTemplate()
{
    Control::OnApplyTemplate();

    Element* fe = GetTemplateChild(ContentHostName);
    IAddChild* textHost = DynamicCast<IAddChild>(fe);

    _scrollViewer = RTTIEqualCast<ScrollViewer>(fe);

    if (NULL != _scrollViewer)
    {
        ScrollChangedEventHandler* objEvent = new ScrollChangedEventHandler(this, &TextBox::OnScrollChanged);

        _scrollViewer->RemoveScrollChanged(objEvent);
        _scrollViewer->AddScrollChanged(objEvent);
    }
    
    if (textHost)
    {
        textHost->AddChild(_textEditor);

        BindToTemplatedParent(TextBox::PasswordCharProperty, TextSimple::PasswordCharProperty);
        BindToTemplatedParent(TextBox::FontSizeProperty, TextSimple::FontSizeProperty);
        BindToTemplatedParent(TextBox::FontFamilyProperty, TextSimple::FontFamilyProperty);
        BindToTemplatedParent(TextBox::ForegroundProperty, TextSimple::ForegroundProperty);
        BindToTemplatedParent(TextBox::TextProperty, TextSimple::TextProperty);
        //BindToTemplatedParent(TextBox::IsSingleLineProperty, TextElement::IsSingleLineProperty);
        //BindToTemplatedParent(TextBox::IsReadOnlyProperty, TextSimple::IsReadOnlyProperty);

        //BindToTemplatedParent(TextBox::HorizontalContentAlignmentProperty, TextSimple::HorizontalAlignmentProperty);
        //BindToTemplatedParent(TextBox::VerticalContentAlignmentProperty, TextSimple::VerticalAlignmentProperty);

        if (IsInitialized())
        {
            _textEditor->RecursiveInit();
        }
    }

    /*if (NULL != _textEditor)
    {
        _textEditor->TextCaretChangedEvent += TextCaretChangedEventHandler(this, &TextBox::OnTextCaretChanged);
    }*/
}

void TextBox::OnMouseLeftButtonDown(MouseButtonEventArg& e)
{
    Focus();
    e.SetHandled(true);
}

void TextBox::OnGotFocus(RoutedEventArg& e)
{
    e.SetHandled(true);
    if (_textEditor != NULL)
    {
        _textEditor->OnGotFocus(e);
    }
}

void TextBox::OnLostFocus(RoutedEventArg& e)
{
    e.SetHandled(true);
    if (_textEditor != NULL)
    {
        _textEditor->OnLostFocus(e);
    }
}

void TextBox::OnKeyDown(KeyboardEventArg& e)
{
    Control::OnKeyDown(e);

    if (_textEditor != NULL)
    {
        _textEditor->OnKeyDown(e);
    }
}

void TextBox::OnTextInput(KeyboardEventArg& e)
{
    Control::OnTextInput(e);
    if (_textEditor != NULL)
    {
        _textEditor->OnTextInput(e);
    }
}

void TextBox::OnTextChanged(TextChangedEventArg& e)
{
    if (_textEditor != NULL)
    {
        ((TextSimple*)_textEditor)->SetText(e.GetText());
    }
    TextEdit::OnTextChanged(e);
}

}
