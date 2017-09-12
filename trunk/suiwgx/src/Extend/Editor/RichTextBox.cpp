// ======================================================================
//
// Copyright (c) 2008-2022 汪荣, Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// RichTextBox.cpp

#include "stdafx.h"
#include "TextHostView.h"

#include <System/Windows/SystemParameters.h>
#include <Extend/Editor/TextBox.h>
#include <Extend/Editor/RichTextBox.h>
#include <Extend/Editor/RichInterface.h>

namespace suic
{

ImplementRTTIOfClass(RichTextBox, suic::TextBoxBase)
ImplementRTTIOfClass(TextBox, suic::TextBoxBase)

ImplementRTTIOfClass(TextRangeOp, suic::TextRange)
ImplementRTTIOfClass(TextSelectionOp, suic::TextSel)

suic::DpProperty* TextBox::CharacterCasingProperty;
suic::DpProperty* TextBox::MaxLengthProperty;
suic::DpProperty* TextBox::MaxLinesProperty;
suic::DpProperty* TextBox::MinLinesProperty;
suic::DpProperty* TextBox::TextAlignmentProperty;
suic::DpProperty* TextBox::TextDecorationsProperty;

static TextRenderHost* ToRenderHost(suic::FrameworkElement* pView)
{
    return (TextRenderHost*)(((TextContainer*)pView)->GetTextRender());
}

RichTextBox::RichTextBox()
{
    _textDoc = NULL;
}

RichTextBox::~RichTextBox()
{
    if (NULL != _textDoc)
    {
        _textDoc->unref();
    }
}

void RichTextBox::InitTextHost()
{
    TextContainer* hostCtrl = (TextContainer*)GetTextContainer();
    TextRenderHost* trScope = (TextRenderHost*)(hostCtrl->GetTextRender());

    _textDoc = new TextDocOp(trScope);
    _textDoc->ref();
    trScope->InitializeRichMode();
    trScope->SetMaxLines(100000);
}

void RichTextBox::StaticInit()
{
    TextBoxBase::StaticInit();
}

ITextDoc* RichTextBox::GetTextDoc()
{
    return _textDoc;
}

TextSel* RichTextBox::GetSelection()
{
    return _textDoc->GetTextSel();
}

bool RichTextBox::InsertEmbbed(Embbed* embbed)
{
    return InsertEmbbed(-1, embbed);
}

bool RichTextBox::InsertEmbbed(suic::Int32 cp, Embbed* embbed)
{
    IRichEditOle* richeditole = NULL;
    TextRenderHost* trScope = ToRenderHost(GetTextContainer());

    embbed->SetHolder(GetTextContainer());
    trScope->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&richeditole);
    return UIOleObject::InsertOleObj(richeditole, cp, embbed);
}

bool RichTextBox::CharIndexIsObject(int charIndex)
{
    REOBJECT preobj;
    TextRenderHost* trScope = ToRenderHost(GetTextContainer());

    preobj.cp = charIndex;
    return trScope->GetOleObjOnCp(&preobj);
}

void RichTextBox::NotifyTextChanged()
{
    TextEditor::OnTextChanged();
}

void RichTextBox::OnTextWrappingChanged(suic::DpPropChangedEventArg* e)
{
    TextRenderHost* trScope = ToRenderHost(GetTextContainer());
    bool bWrap = e->GetNewValue()->ToInt() != suic::TextWrapping::tNoWrap;
    trScope->SetWordWrap(bWrap);
}

//----------------------------------------------------------

TextBox::TextBox()
{
}

TextBox::~TextBox()
{
}

void TextBox::InitTextHost()
{
    TextRenderHost* trScope = ToRenderHost(GetTextContainer());
    trScope->SetTextMode(TM_PLAINTEXT | TM_RICHTEXT | TM_MULTILEVELUNDO | TM_MULTICODEPAGE);
}

void TextBox::StaticInit()
{
    if (NULL == MaxLengthProperty)
    {
        TextBoxBase::StaticInit();

        CharacterCasingProperty = suic::DpProperty::Register(_T("CharacterCasing"), RTTIType(), suic::OString::RTTIType()
            , suic::DpPropMemory::GetPropMeta(suic::OString::EmptyString, suic::PropMetadataOptions::AffectsMeasure));
        MaxLengthProperty = suic::DpProperty::Register(_T("MaxLength"), RTTIType(), suic::Integer::RTTIType()
            , suic::DpPropMemory::GetPropMeta(new suic::Integer(1024 * 64), suic::PropMetadataOptions::AffectsMeasure, &TextBox::OnMaxLinesPropChanged));
        MaxLinesProperty = suic::DpProperty::Register(_T("MaxLines"), RTTIType(), suic::Integer::RTTIType()
            , suic::DpPropMemory::GetPropMeta(new suic::Integer(-1), suic::PropMetadataOptions::AffectsMeasure, &TextBox::OnMaxLinesPropChanged));
        MinLinesProperty = suic::DpProperty::Register(_T("MinLines"), RTTIType(), suic::Integer::RTTIType()
            , suic::DpPropMemory::GetPropMeta(suic::Integer::GetZeroInt(), suic::PropMetadataOptions::AffectsMeasure, &TextBox::OnMinLinesPropChanged));
        TextAlignmentProperty = suic::DpProperty::Register(_T("TextAlignment"), RTTIType(), suic::Integer::RTTIType()
            , suic::DpPropMemory::GetPropMeta(suic::Integer::GetZeroInt(), suic::PropMetadataOptions::AffectsMeasure, &TextBox::OnTextAlignmentPropChanged));
        TextDecorationsProperty = suic::DpProperty::Register(_T("TextDecorations"), RTTIType(), suic::OString::RTTIType()
            , suic::DpPropMemory::GetPropMeta(suic::OString::EmptyString, suic::PropMetadataOptions::AffectsMeasure));
    }
}

void TextBox::OnMaxLengthPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBox* elem = suic::DynamicCast<TextBox>(d);
    if (NULL != elem)
    {
        TextRenderHost* trScope = ToRenderHost(elem->GetTextContainer());
        suic::Integer* obj = suic::RTTICast<suic::Integer>(e->GetNewValue());

        if (NULL != obj)
        {
            trScope->SetMaxLength(obj->ToInt());
        }
    }
}

void TextBox::OnMaxLinesPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBox* elem = suic::DynamicCast<TextBox>(d);
    if (NULL != elem)
    {
        TextRenderHost* trScope = ToRenderHost(elem->GetTextContainer());
        suic::Integer* obj = suic::RTTICast<suic::Integer>(e->GetNewValue());

        if (NULL != obj)
        {
            int iLines = obj->ToInt();
            if (iLines <= 0)
            {
                trScope->SetMaxLines(100000);
            }
            else
            {
                trScope->SetMaxLines(iLines);
            }
        }
    }
}

void TextBox::OnMinLinesPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{

}

void TextBox::OnTextAlignmentPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
}

void TextBox::OnTextWrappingChanged(suic::DpPropChangedEventArg* e)
{
    TextRenderHost* trScope = ToRenderHost(GetTextContainer());
    bool bWrap = e->GetNewValue()->ToInt() != suic::TextWrapping::tNoWrap;
    trScope->SetWordWrap(bWrap);
}

//---------------------------------------------------

int TextBox::GetMaxLines()
{
    return GetValue(MaxLinesProperty)->ToInt();
}

void TextBox::SetMaxLines(int val)
{
    SetValue(MaxLinesProperty, new suic::Integer(val));
}

int TextBox::GetMinLines()
{
    return GetValue(MinLinesProperty)->ToInt();
}

void TextBox::SetMinLines(int val)
{
    SetValue(MinLinesProperty, new suic::Integer(val));
}

suic::TextAlignment TextBox::GetTextAlignment()
{
    return (suic::TextAlignment)GetValue(TextAlignmentProperty)->ToInt();
}

void TextBox::SetTextAlignment(suic::TextAlignment val)
{
    SetValue(TextAlignmentProperty, new suic::Integer(val));
}

}
