// ======================================================================
//
// Copyright (c) 2008-2022 汪荣, Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// PasswordBox.cpp

#include "stdafx.h"
#include "TextHostView.h"

#include <System/Windows/SystemParameters.h>

#include <Extend/Editor/PasswordBox.h>
#include <Extend/Editor/RichTextBox.h>
#include <Extend/Editor/TextContainer.h>
#include <Extend/Editor/TextHostView.h>
#include <Extend/Editor/RichInterface.h>

namespace suic
{

ImplementRTTIOfClass(PasswordBox, suic::Control)

suic::DpProperty* PasswordBox::PasswordCharProperty;
suic::DpProperty* PasswordBox::SelectionBrushProperty;
suic::DpProperty* PasswordBox::SelectionTextBrushProperty;

suic::RoutedEvent* PasswordBox::PasswordChangedEvent;

static const suic::String ContentHostName = "PART_ContentHost";

static TextRenderHost* ToRenderHost(suic::FrameworkElement* pView)
{
    return ((TextRenderHost*)((TextContainer*)(pView))->GetTextRender());
}

PasswordBox::PasswordBox()
    : _textView(NULL)
    , _scrollViewer(NULL)
    , _textPending(0)
{
    TextRenderHost* trScope = NULL;
    TextContainer* txtCtrl = new TextContainer();

    _textView = txtCtrl;
    txtCtrl->_hostElem = this;
    txtCtrl->ref();

    trScope = (TextRenderHost*)txtCtrl->GetTextRender();

    trScope->SetTextMode(TM_PLAINTEXT | TM_SINGLELEVELUNDO | TM_MULTICODEPAGE);
    trScope->SetUsePassword(true);
    trScope->SetMultiLine(false);
}

PasswordBox::~PasswordBox()
{
    TextContainer* txtCtrl = (TextContainer*)_textView;
    txtCtrl->unref();
}

void PasswordBox::StaticInit()
{
    if (NULL == PasswordCharProperty)
    {
        //●
        PasswordCharProperty = suic::DpProperty::Register(_T("PasswordChar"), RTTIType(), suic::OString::RTTIType()
            , suic::DpPropMemory::GetPropMeta(new suic::OString(_U("*")), suic::PropMetadataOptions::AffectsMeasure, &PasswordBox::OnPasswordCharPropChanged));

        SelectionBrushProperty =  suic::DpProperty::Register(_T("SelectionBrush"), RTTIType(), suic::Brush::RTTIType()
            , suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsNone, &PasswordBox::OnSelectionBrushPropChanged));
        SelectionTextBrushProperty =  suic::DpProperty::Register(_T("SelectionTextBrush"), RTTIType(), suic::Brush::RTTIType()
            , suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsNone, &PasswordBox::OnSelectionTextBrushPropChanged));

        PasswordChangedEvent = suic::EventHelper::RegisterRoutedEvent(_T("PasswordChanged"), suic::RoutingStrategy::Bubble, RTTIType(), RTTIType());

        suic::Control::PaddingProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsParentMeasure, &PasswordBox::OnPaddingPropChanged));
        suic::Control::ForegroundProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsNone, &PasswordBox::OnForegroundPropChanged));
        suic::Control::FontFamilyProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsParentMeasure, &PasswordBox::OnFontFamilyPropChanged));
        suic::Control::FontWeightProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsParentMeasure, &PasswordBox::OnFontWeightPropChanged));
        suic::Control::FontSizeProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Integer::GetPosInt(12), suic::PropMetadataOptions::AffectsParentMeasure, &PasswordBox::OnFontSizePropChanged));
    }
}

void PasswordBox::OnPasswordCharPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    PasswordBox* elem = suic::RTTICast<PasswordBox>(d);

    if (NULL != elem && elem->_textView)
    {
        suic::String pwdChar = e->GetNewValue()->ToString();
        TextRenderHost* trScope = ToRenderHost(elem->_textView);
        if (pwdChar.Length() == 1)
        {
            trScope->SetPasswordChar(pwdChar[0]);
        }
    }
}

void PasswordBox::OnSelectionBrushPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    PasswordBox* elem = suic::DynamicCast<PasswordBox>(d);
    
    if (NULL != elem && elem->_textView)
    {
        suic::SolidColorBrush* brh = suic::RTTICast<suic::SolidColorBrush>(e->GetNewValue());
        TextRenderHost* trScope = ToRenderHost(elem->_textView);
        if (brh)
        {
            trScope->SetSelBackground(brh->ToColor());
        }
        else
        {
            trScope->SetSelBackground(suic::Color::FromRgb(::GetSysColor(COLOR_HIGHLIGHT)));
        }
    }
}

void PasswordBox::OnSelectionTextBrushPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    PasswordBox* elem = suic::DynamicCast<PasswordBox>(d);
    if (NULL != elem && elem->_textView)
    {
        suic::SolidColorBrush* brh = suic::RTTICast<suic::SolidColorBrush>(e->GetNewValue());
        TextRenderHost* trScope = ToRenderHost(elem->_textView);
        if (brh)
        {
            trScope->SetSelTextColor(brh->ToColor());
        }
        else
        {
            trScope->SetSelTextColor(suic::Color::FromRgb(::GetSysColor(COLOR_HIGHLIGHTTEXT)));
        }
    }
}

void PasswordBox::OnForegroundPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    PasswordBox* elem = suic::DynamicCast<PasswordBox>(d);
    if (NULL != elem && 0 == elem->_textPending)
    {
        suic::SolidColorBrush* brh = suic::RTTICast<suic::SolidColorBrush>(e->GetNewValue());
        if (brh)
        {
            ToRenderHost(elem->_textView)->SetTextColor(brh->ToColor());
        }
    }
}

void PasswordBox::OnFontFamilyPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    PasswordBox* elem = suic::DynamicCast<PasswordBox>(d);
    if (NULL != elem && 0 == elem->_textPending)
    {
        suic::OString* obj = suic::RTTICast<suic::OString>(e->GetNewValue());
        TextRenderHost* trScope = ToRenderHost(elem->_textView);
        if (obj)
        {
            trScope->SetFontFamily(obj->ToString());
        }
    }
}

void PasswordBox::OnFontWeightPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
     PasswordBox* elem = suic::DynamicCast<PasswordBox>(d);
     if (NULL != elem && 0 == elem->_textPending)
     {
         suic::Integer* obj = suic::RTTICast<suic::Integer>(e->GetNewValue());
         TextRenderHost* trScope = ToRenderHost(elem->_textView);
         if (obj)
         {
             trScope->SetFontWeight(obj->ToInt());
         }
     }
}

void PasswordBox::OnFontSizePropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    PasswordBox* elem = suic::DynamicCast<PasswordBox>(d);
    if (NULL != elem && 0 == elem->_textPending)
    {
        suic::Integer* obj = suic::RTTICast<suic::Integer>(e->GetNewValue());
        TextRenderHost* trScope = ToRenderHost(elem->_textView);
        if (obj)
        {
            trScope->SetFontSize(obj->ToInt());
        }
    }
}

void PasswordBox::OnPaddingPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    PasswordBox* elem = suic::RTTICast<PasswordBox>(d);
    if (NULL != elem)
    {
        suic::ORect* obj = suic::RTTICast<suic::ORect>(e->GetNewValue());
        TextRenderHost* trScope = ToRenderHost(elem->_textView);
        if (obj)
        {
            trScope->SetPadding(obj->ToRect());
        }
        else
        {
            trScope->SetPadding(suic::Rect(2, 2));
        }
    }
}

//---------------------------------------------------

suic::String PasswordBox::GetPassword()
{
    suic::String text;
    BSTR bstr = NULL;
    TextRenderHost* trScope = NULL;
    
    if (NULL != _textView)
    {
        trScope = ToRenderHost(_textView);
        trScope->GetTextService()->TxGetText(&bstr);

        if (NULL != bstr)
        {
            text = (LPWSTR)bstr;
            ::SysFreeString(bstr);
        }
    }

    return text;
}

void PasswordBox::SetPassword(suic::String val)
{
    if (_textView)
    {
        TextRenderHost* trScope = ToRenderHost(_textView);
        trScope->SetText(val);
    }
}

suic::Char PasswordBox::GetPasswordChar()
{
    suic::String strChar = GetValue(PasswordCharProperty)->ToString();
    if (strChar.Length() == 1)
    {
        return strChar[0];
    }
    else
    {
        return suic::Char();
    }
}

void PasswordBox::SetPasswordChar(suic::Char ch)
{
    SetValue(PasswordCharProperty, new suic::OString(ch)); 
}

void PasswordBox::AddPasswordChanged(suic::RoutedEventHandler* h)
{
    AddHandler(PasswordChangedEvent, h);
}

void PasswordBox::RemovePasswordChanged(suic::RoutedEventHandler* h)
{
    RemoveHandler(PasswordChangedEvent, h);
}

//-----------------------------------------------------------

void PasswordBox::OnApplyTemplate()
{
    suic::Control::OnApplyTemplate();

    TextContainer* txtCtrl = (TextContainer*)(_textView);
    TextRenderHost* trScope = (TextRenderHost*)txtCtrl->GetTextRender();
    suic::IAddChild* hostElem = suic::DynamicCast<suic::IAddChild>(GetTemplateChild(ContentHostName));

    if (hostElem)
    {
        trScope->SetPasswordChar(GetPasswordChar());
        hostElem->AddChild(txtCtrl);
    }

    txtCtrl->SetTextCallback(suic::DynamicCast<ITextCallback>(this));
}

void PasswordBox::NotifyTextChanged()
{
    suic::TextChangedEventArg e;

    e.OnOverrideOriginalSource(this);
    e.SetRoutedEvent(PasswordChangedEvent);
    OnPasswordChanged(&e);
}

void PasswordBox::NotifyTextFontChanged(int flag)
{
    TextRenderHost* trScope = ToRenderHost(this);
    CHARFORMAT2W& cf = trScope->GetCF();

    _textPending = 1;

    if (flag & eFontChangedFlag::fcfSize)
    {
        SetFontSize(trScope->GetFontSize());
    }
    if (flag & eFontChangedFlag::fcfTextColor)
    {
        SetForeground(new suic::SolidColorBrush(trScope->GetTextColor()));
    }
    if (flag & eFontChangedFlag::fcfBold)
    {
        SetFontWeight(suic::FontWeightStyle::fwExtraBold);
    }
    /*if (flag & eFontChangedFlag::fcfItalic)
    {
        SetForeground(new suic::SolidColorBrush(trScope->GetTextColor()));
    }*/

    _textPending = 0;
}

void PasswordBox::NotifyTextSelectionChanged()
{

}

void PasswordBox::OnMouseEnter(suic::MouseButtonEventArg* e)
{
    suic::Control::OnMouseEnter(e);
}


void PasswordBox::OnMouseLeave(suic::MouseButtonEventArg* e)
{

}

void PasswordBox::OnMouseMove(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

void PasswordBox::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    Control::OnMouseLeftButtonDown(e);
    e->SetHandled(true);
}

void PasswordBox::OnMouseLeftButtonDbclk(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

void PasswordBox::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

void PasswordBox::OnTextInput(suic::KeyboardEventArg* e)
{
    if (_textView != NULL)
    {
        _textView->OnTextInput(e);
    }
}

void PasswordBox::OnKeyDown(suic::KeyboardEventArg* e)
{
    if (_textView != NULL)
    {
        _textView->OnKeyDown(e);
    }
}

void PasswordBox::OnKeyUp(suic::KeyboardEventArg* e)
{
    if (_textView != NULL)
    {
        _textView->OnKeyUp(e);
    }
}

//bool PasswordBox::OnEvent(suic::Object* sender, suic::MessageParam* mp)
//{
//    if (_textView != NULL)
//    {
//        return _textView->OnEvent(sender, mp);
//    }
//    else
//    {
//        return false;
//    }
//}

void PasswordBox::OnGotKeyboardFocus(suic::KeyboardFocusEventArg* e)
{
    suic::Control::OnGotKeyboardFocus(e);
    e->SetHandled(true);
    if (_textView != NULL)
    {
        _textView->GotFocus();
        ToRenderHost(_textView)->EnableImmStatus(false);
    }
}

void PasswordBox::OnLostKeyboardFocus(suic::KeyboardFocusEventArg* e)
{
    suic::Control::OnLostKeyboardFocus(e);
    e->SetHandled(true);
    if (_textView != NULL)
    {
        _textView->OnLostFocus(e);
        ToRenderHost(_textView)->EnableImmStatus(true);
    }
}

//void PasswordBox::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
//{
//    suic::Control::OnMouseLeftButtonDown(e);
//    e->SetHandled(true);
//    Focus();
//}
//
//void PasswordBox::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
//{
//    suic::Control::OnMouseLeftButtonUp(e);
//    e->SetHandled(true);
//}

void PasswordBox::OnPasswordChanged(suic::TextChangedEventArg* e)
{
    RaiseEvent(e);
}

}
