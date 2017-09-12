// ======================================================================
//
// Copyright (c) 2008-2022 汪荣, Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// TextBoxBase.cpp

#include "stdafx.h"

#include <Extend/Editor/TextBoxBase.h>

#include <System/Tools/EventHelper.h>
#include <System/Graphics/SolidcolorBrush.h>
#include <System/Windows/SystemParameters.h>
#include <System/Windows/DataHostObj.h>

#include <Extend/Editor/TextHostView.h>
#include <Extend/Editor/TextBox.h>
#include <Extend/Editor/RichTextBox.h>
#include <Extend/Editor/RichInterface.h>


#define ToTextHost(txtBase) ((TextContainer*)txtBase->GetTextContainer())
#define ToRenderHost(pView) ((TextRenderHost*)(ToTextHost(pView)->GetTextRender()))

namespace suic
{

ImplementRTTIOfClass(TextBoxBase, suic::TextEditor)

suic::DpProperty* TextBoxBase::AcceptsReturnProperty;
suic::DpProperty* TextBoxBase::AcceptsTabProperty;
suic::DpProperty* TextBoxBase::AutoWordSelectionProperty;
suic::DpProperty* TextBoxBase::TextDecorationsProperty;

suic::DpProperty* TextBoxBase::IsUndoEnabledProperty;
suic::DpProperty* TextBoxBase::UndoLimitProperty;

suic::RoutedCommand* TextBoxBase::UndoCommand;
suic::RoutedCommand* TextBoxBase::RedoCommand;

suic::RoutedCommand* TextBoxBase::CutCommand;
suic::RoutedCommand* TextBoxBase::CopyCommand;
suic::RoutedCommand* TextBoxBase::PasteCommand;

suic::RoutedCommand* TextBoxBase::SelectAllCommand;

static const suic::String ContentHostName = "PART_ContentHost";

TextBoxBase::TextBoxBase()
    : _textPending(0)
    , _textContainer(NULL)
{
}

TextBoxBase::~TextBoxBase()
{
    if (NULL != _textContainer)
    {
        _textContainer->unref();
    }
}

TextContainer* TextBoxBase::GetTextContainer()
{
    if (NULL == _textContainer)
    {
        _textContainer = CreateTestHost();
        _textContainer->_hostElem = this;
        _textContainer->ref();
        InitTextHost();
    }

    return _textContainer;
}

TextContainer* TextBoxBase::CreateTestHost()
{
    TextContainer* pTc = new TextContainer();
    return pTc;
}

void TextBoxBase::InitTextHost()
{

}

void TextBoxBase::OnAllowDropPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBoxBase* txtBox = suic::RTTICast<TextBoxBase>(d);
    if (NULL != txtBox)
    {
        ToRenderHost(txtBox)->SetEnableDrag(e->GetNewValue()->ToBool());
    }
}

void TextBoxBase::OnTextDecorationsPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBoxBase* txtBox = suic::RTTICast<TextBoxBase>(d);
    if (NULL != txtBox)
    {
        if (e->GetNewValue() == suic::TextDecorations::underline)
        {
            ToRenderHost(txtBox)->SetUnderline(true);
            ToRenderHost(txtBox)->SetStrikeout(false);
        }
        else if (e->GetNewValue() == suic::TextDecorations::strikethrough)
        {
            ToRenderHost(txtBox)->SetStrikeout(true);
            ToRenderHost(txtBox)->SetUnderline(false);
        }
        else
        {
            ToRenderHost(txtBox)->SetUnderline(false);
            ToRenderHost(txtBox)->SetStrikeout(false);
        }
    }
}

void TextBoxBase::StaticInit()
{
    if (NULL == AcceptsReturnProperty)
    {
        AcceptsReturnProperty =  KeyboardNavigation::AcceptsReturnProperty->AddOwner(RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Boolean::True, suic::PropMetadataOptions::AffectsNone, &TextBoxBase::OnAcceptsReturnPropChanged));
        AcceptsTabProperty =  suic::DpProperty::Register(_T("AcceptsTab"), RTTIType(), suic::Boolean::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Boolean::False, suic::PropMetadataOptions::AffectsNone, &TextBoxBase::OnAcceptsTabPropChanged));
        AutoWordSelectionProperty =  suic::DpProperty::Register(_T("AutoWordSelection"), RTTIType(), suic::Boolean::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Boolean::True, suic::PropMetadataOptions::AffectsNone, &TextBoxBase::OnAutoWordSelectionPropChanged));

        IsUndoEnabledProperty =  suic::DpProperty::Register(_T("IsUndoEnabled"), RTTIType(), suic::Boolean::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Boolean::True, suic::PropMetadataOptions::AffectsNone, &TextBoxBase::OnIsUndoEnabledPropChanged));
        UndoLimitProperty =  suic::DpProperty::Register(_T("UndoLimit"), RTTIType(), suic::Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(new suic::Integer(50), suic::PropMetadataOptions::AffectsNone, &TextBoxBase::OnUndoLimitPropChanged));

        TextDecorationsProperty = suic::DpProperty::Register(_T("TextDecorations"), RTTIType(), suic::TextDecorationCollection::RTTIType()
            , suic::DpPropMemory::GetPropMeta(suic::TextDecorationCollection::GetEmpty(), 
            suic::PropMetadataOptions::AffectsRender, &TextBoxBase::OnTextDecorationsPropChanged));

        TextDecorationsProperty->SetConvertValueCb(suic::TextDecorationsConvert::Convert);

        suic::Control::PaddingProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsParentMeasure, &TextBoxBase::OnPaddingPropChanged));
        suic::Control::ForegroundProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsNone, &TextBoxBase::OnForegroundPropChanged));
        suic::Control::FontFamilyProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsNone, &TextBoxBase::OnFontFamilyPropChanged));
        suic::Control::FontWeightProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsNone, &TextBoxBase::OnFontWeightPropChanged));
        suic::Control::FontSizeProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsNone, &TextBoxBase::OnFontSizePropChanged));
        suic::Control::FontStyleProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsNone, &TextBoxBase::OnFontStylePropChanged));

        AllowDropProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Boolean::False, suic::PropMetadataOptions::AffectsNone, &TextBoxBase::OnAllowDropPropChanged));

        UndoCommand = new RoutedCommand(_T("Undo"), TextEditor::RTTIType());
        RedoCommand = new RoutedCommand(_T("Redo"), TextEditor::RTTIType());

        CutCommand = new RoutedCommand(_T("Cut"), TextEditor::RTTIType());
        CopyCommand = new RoutedCommand(_T("Copy"), TextEditor::RTTIType());
        PasteCommand = new RoutedCommand(_T("Paste"), TextEditor::RTTIType());

        SelectAllCommand = new RoutedCommand(_T("SelectAll"), TextEditor::RTTIType());

        // 注册默认命令

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            UndoCommand, ExecutedRoutedEventHandler(TextBoxBase::OnHandleCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            RedoCommand, ExecutedRoutedEventHandler(TextBoxBase::OnHandleCommand));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            CutCommand, ExecutedRoutedEventHandler(TextBoxBase::OnHandleCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            CopyCommand, ExecutedRoutedEventHandler(TextBoxBase::OnHandleCommand));
        CommandOp::RegisterClassCommandHandler(RTTIType(),
            PasteCommand, ExecutedRoutedEventHandler(TextBoxBase::OnHandleCommand));

        CommandOp::RegisterClassCommandHandler(RTTIType(),
            SelectAllCommand, ExecutedRoutedEventHandler(TextBoxBase::OnHandleCommand));        
    }
}

void TextBoxBase::OnHandleCommand(suic::Object* target, suic::ExecutedRoutedEventArg* e)
{
    TextBoxBase* pTextBox = RTTICast<TextBoxBase>(target);
    ICommand* cmd = e->GetCommand();

    e->SetHandled(true);

    if (cmd == CutCommand)
    {
        pTextBox->Cut();
    }
    else if (cmd == CopyCommand)
    {
        pTextBox->Copy();
    }
    else if (cmd == PasteCommand)
    {
        pTextBox->Paste();
    }
    else if (cmd == UndoCommand)
    {
        pTextBox->Undo();
    }
    else if (cmd == RedoCommand)
    {
        pTextBox->Redo();
    }
    else if (cmd == SelectAllCommand)
    {
        pTextBox->SelectAll();
    }
    else
    {
        e->SetHandled(false);
    }
}

void TextBoxBase::OnAcceptsReturnPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBoxBase* pText(suic::DynamicCast<TextBoxBase>(d));
    if (NULL != pText)
    {
        ToRenderHost(pText)->SetAcceptsReturn(e->GetNewValue()->ToBool());
    }
}

void TextBoxBase::OnAutoWordSelectionPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBoxBase* pText(suic::DynamicCast<TextBoxBase>(d));
    if (NULL != pText)
    {
        ToRenderHost(pText)->SetAutoWordSelection(e->GetNewValue()->ToBool());
    }
}

void TextBoxBase::OnAcceptsTabPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBoxBase* pText(suic::DynamicCast<TextBoxBase>(d));
    if (NULL != pText)
    {
        ToTextHost(pText)->SetAcceptsTab(e->GetNewValue()->ToBool());
    }
}

void TextBoxBase::OnIsUndoEnabledPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBoxBase* pText(suic::DynamicCast<TextBoxBase>(d));
    if (NULL != pText)
    {
        if (e->GetNewValue()->ToBool())
        {
            ToRenderHost(pText)->SetUndoLimit(pText->GetUndoLimit());
        }
        else
        {
            ToRenderHost(pText)->SetUndoLimit(0);
        }
    }
}

void TextBoxBase::OnUndoLimitPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBoxBase* pText(suic::DynamicCast<TextBoxBase>(d));
    if (NULL != pText)
    {
        ToRenderHost(pText)->SetUndoLimit(e->GetNewValue()->ToInt());
    }
}

void TextBoxBase::OnForegroundPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
        TextBoxBase* elem = suic::DynamicCast<TextBoxBase>(d);
        if (NULL != elem && 0 == elem->_textPending)
        {
            suic::SolidColorBrush* brh = suic::RTTICast<suic::SolidColorBrush>(e->GetNewValue());
            TextRenderHost* trScope = ToRenderHost(elem);
            if (brh)
            {
                trScope->SetTextColor(brh->ToColor());
            }
        }
}

void TextBoxBase::OnFontFamilyPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBoxBase* elem = suic::DynamicCast<TextBoxBase>(d);
    if (NULL != elem && 0 == elem->_textPending)
    {
        suic::OString* obj = suic::RTTICast<suic::OString>(e->GetNewValue());
        TextRenderHost* trScope = ToRenderHost(elem);
        if (obj)
        {
            trScope->SetFontFamily(obj->ToString());
        }
    }
}

void TextBoxBase::OnFontWeightPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBoxBase* elem = suic::DynamicCast<TextBoxBase>(d);
    if (NULL != elem && 0 == elem->_textPending)
    {
        suic::Integer* obj = suic::RTTICast<suic::Integer>(e->GetNewValue());
        TextRenderHost* trScope = ToRenderHost(elem);
        if (obj)
        {
            trScope->SetFontWeight(obj->ToInt());
        }
    }
}

void TextBoxBase::OnFontSizePropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBoxBase* elem = suic::DynamicCast<TextBoxBase>(d);
    if (NULL != elem && 0 == elem->_textPending)
    {
        suic::Integer* obj = suic::RTTICast<suic::Integer>(e->GetNewValue());
        TextRenderHost* trScope = ToRenderHost(elem);
        if (obj)
        {
            trScope->SetFontSize(obj->ToInt());
        }
    }
}

void TextBoxBase::OnFontStylePropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBoxBase* elem = suic::DynamicCast<TextBoxBase>(d);
    if (NULL != elem && 0 == elem->_textPending)
    {
        int iStyle = e->GetNewValue()->ToInt();
        TextRenderHost* trScope = ToRenderHost(elem);
        if (trScope)
        {
            if (iStyle == suic::FontStyleStyle::fssItalic)
            {
                trScope->SetFontItalic(true);
            }
            else
            {
                trScope->SetFontItalic(false);
            }
        }
    }
}

void TextBoxBase::OnPaddingPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBoxBase* elem = suic::DynamicCast<TextBoxBase>(d);
    if (NULL != elem)
    {
        suic::ORect* obj = suic::RTTICast<suic::ORect>(e->GetNewValue());
        TextRenderHost* trScope = ToRenderHost(elem);
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

//void TextBoxBase::OnRenderSizeChanged(suic::SizeChangedInfo& e)
//{
//    suic::TextEditor::OnRenderSizeChanged(e);
//    TextContainer* textElem = (TextContainer*)(_textHost);
//    if (NULL != textElem)
//    {
//        suic::Point pt = textElem->GetScreenOffset();
//        suic::Rect rcClient(pt, textElem->GetRenderSize());
//        textElem->GetTextRender()->SetClntRect(rcClient);
//    }
//}

void TextBoxBase::OnApplyTemplate()
{
    suic::TextEditor::OnApplyTemplate();

    suic::Element* fe = GetTemplateChild(ContentHostName);
    suic::IAddChild* textHost = suic::DynamicCast<suic::IAddChild>(fe);
    TextContainer* textElem = (TextContainer*)(GetTextContainer());

    _scrollViewer = suic::RTTICast<suic::ScrollViewer>(fe);

    if (NULL != _scrollViewer)
    {
        suic::IAddChild* scrollCont = suic::DynamicCast<suic::IAddChild>(_scrollViewer->GetContent());
        suic::ScrollChangedEventHandler* objEvent = new suic::ScrollChangedEventHandler(this, &TextBoxBase::OnScrollChanged);

        _scrollViewer->RemoveScrollChanged(objEvent);
        _scrollViewer->AddScrollChanged(objEvent);

        if (scrollCont != NULL)
        {
            scrollCont->AddChild(textElem);
        }
        else
        {
            _scrollViewer->SetContent(textElem);
        }
    }
    else if (NULL != textHost)
    {
        textHost->AddChild(textElem);
    }
    
    textElem->SetTextCallback(suic::DynamicCast<ITextCallback>(this));

    if (IsFocused())
    {
        textElem->GotFocus();
    }

    suic::Brush* caretBrush = GetCaretBrush();
    if (NULL != caretBrush)
    {
        GetTextContainer()->GetCaretElement()->SetBackground(caretBrush);
    }
}

void TextBoxBase::OnLoaded(suic::LoadedEventArg* e)
{
    suic::TextEditor::OnLoaded(e);
    if (IsFocused())
    {
        GetTextContainer()->GotFocus();
    }
}

void TextBoxBase::OnGotKeyboardFocus(suic::KeyboardFocusEventArg* e)
{
    suic::TextEditor::OnGotKeyboardFocus(e);
    e->SetHandled(true);

    if (GetTextContainer() != NULL)
    {
        GetTextContainer()->GotFocus();
    }
}

void TextBoxBase::OnLostKeyboardFocus(suic::KeyboardFocusEventArg* e)
{
    suic::TextEditor::OnLostKeyboardFocus(e);
    e->SetHandled(true);

    if (GetTextContainer() != NULL)
    {
        GetTextContainer()->OnLostKeyboardFocus(e);
    }
    UpdateDepProp(TextBoxBase::TextProperty);
}

void TextBoxBase::OnMouseEnter(suic::MouseButtonEventArg* e)
{
    suic::TextEditor::OnMouseEnter(e);
}


void TextBoxBase::OnMouseLeave(suic::MouseButtonEventArg* e)
{

}

void TextBoxBase::OnMouseMove(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

void TextBoxBase::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    TextEditor::OnMouseLeftButtonDown(e);
    e->SetHandled(true);
}

void TextBoxBase::OnMouseLeftButtonDbclk(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

void TextBoxBase::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

void TextBoxBase::OnTextInput(suic::KeyboardEventArg* e)
{
    if (GetTextContainer() != NULL)
    {
        GetTextContainer()->OnTextInput(e);
    }
}

void TextBoxBase::OnImeEvent(suic::MessageEventArg* e)
{
    if (GetTextContainer() != NULL)
    {
        GetTextContainer()->OnImeEvent(e);
    }
}

void TextBoxBase::OnKeyDown(suic::KeyboardEventArg* e)
{
    if (GetTextContainer() != NULL)
    {
        GetTextContainer()->OnKeyDown(e);
    }
}

void TextBoxBase::OnKeyUp(suic::KeyboardEventArg* e)
{
    if (GetTextContainer() != NULL)
    {
        GetTextContainer()->OnKeyUp(e);
    }
}

//bool TextBoxBase::OnEvent(suic::Object* sender, suic::MessageParam* mp)
//{
//    if (_textHost != NULL)
//    {
//        if (mp->wp == VK_TAB)
//        {
//            return true;
//        }
//        return _textHost->OnEvent(sender, mp);
//    }
//    else
//    {
//        return false;
//    }
//}

bool TextBoxBase::Undo()
{
    return ToRenderHost(this)->TxSendMessage(EM_UNDO, 0, 0) != 0;
}

bool TextBoxBase::Redo()
{
    return ToRenderHost(this)->TxSendMessage(EM_REDO, 0, 0) != 0;
}

void TextBoxBase::Cut()
{
    ToRenderHost(this)->TxSendMessage(WM_CUT, 0, 0);
    //NotifyTextChanged();
}

void TextBoxBase::Copy()
{
    ToRenderHost(this)->TxSendMessage(WM_COPY, 0, 0);
}

void TextBoxBase::Paste()
{
    ToRenderHost(this)->TxSendMessage(WM_PASTE, 0, 0);
    //NotifyTextChanged();
}

void TextBoxBase::SelectAll()
{
    Select(0, -1);
}

bool TextBoxBase::GetAcceptsReturn()
{
    return GetValue(AcceptsReturnProperty)->ToBool();
}

void TextBoxBase::SetAcceptsReturn(bool val)
{
    SetValue(AcceptsReturnProperty, BOOLTOBOOLEAN(val));
}

bool TextBoxBase::GetAcceptsTab()
{
    return GetValue(AcceptsTabProperty)->ToBool();
}

void TextBoxBase::SetAcceptsTab(bool val)
{
    SetValue(AcceptsTabProperty, BOOLTOBOOLEAN(val));
}

bool TextBoxBase::GetAutoWordSelection()
{
    return GetValue(AutoWordSelectionProperty)->ToBool();
}

void TextBoxBase::SetAutoWordSelection(bool val)
{
    SetValue(AutoWordSelectionProperty, BOOLTOBOOLEAN(val));
}

suic::TextDecorationCollection* TextBoxBase::GetTextDecorations()
{
    return dynamic_cast<suic::TextDecorationCollection*>(GetValue(TextDecorationsProperty));
}

void TextBoxBase::SetTextDecorations(suic::TextDecorationCollection* val)
{
    SetValue(TextDecorationsProperty, val);
}


bool TextBoxBase::CanRedo()
{
    return ToRenderHost(this)->TxSendMessage(EM_CANREDO, 0, 0) != 0;
}

bool TextBoxBase::CanUndo()
{
    return ToRenderHost(this)->TxSendMessage(EM_CANUNDO, 0, 0) != 0;
}

//---------------------------------------------------------

void TextBoxBase::Select(int start, int length)
{
    CHARRANGE cr;
    LRESULT lResult = S_OK;

    cr.cpMin = start;
    cr.cpMax = start + length;
    lResult = ToRenderHost(this)->TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr);
}

bool TextBoxBase::IsUndoEnabled()
{
    return GetValue(IsUndoEnabledProperty)->ToBool();
}

void TextBoxBase::SetIsUndoEnabled(bool val)
{
    SetValue(IsUndoEnabledProperty, BOOLTOBOOLEAN(val));
}

int TextBoxBase::GetUndoLimit()
{
    return GetValue(UndoLimitProperty)->ToInt();
}

void TextBoxBase::SetUndoLimit(int val)
{
    SetValue(UndoLimitProperty, new suic::Integer(val));
}

void TextBoxBase::SetCaretPos(int x, int y)
{
    ToRenderHost(this)->TxSetCaretPos(x, y);
}

void TextBoxBase::ShowCaret(bool fShow)
{
    ToRenderHost(this)->TxShowCaret(fShow);
}

//------------------------------------------------------------

void TextBoxBase::OnDragEnter(suic::DragEventArg* e)
{
    e->SetHandled(true);

    suic::IDataStore* dataStore = e->GetData();

    if (NULL != dataStore)
    {
        DataHostObj* dataObj = suic::DynamicCast<DataHostObj>(dataStore->GetData(_U("DataObj")));
        if (NULL != dataObj)
        {
            IDropTarget *pdt = NULL;
            ITextServicesEx* txtService = ToRenderHost(this)->GetTextService();

            if (txtService->TxGetDropTarget(&pdt) == NOERROR)
            {
                POINTL point;
                suic::Point pt;
                DWORD dwEffect = e->GetEffects();

                ::GetCursorPos(&pt);
                point.x = pt.x;
                point.y = pt.y;
                pdt->DragEnter(dataObj->GetDataObject(), e->GetKeyStates(), point, &dwEffect);
                e->SetEffects((suic::DragDropEffects)dwEffect);
                pdt->Release();
            }
        }
    }
}

void TextBoxBase::OnDragOver(suic::DragEventArg* e)
{
    e->SetHandled(true);

    IDropTarget *pdt = NULL;
    ITextServicesEx* txtService = ToRenderHost(this)->GetTextService();

    if (txtService->TxGetDropTarget(&pdt) == NOERROR)
    {
        POINTL point;
        suic::Point pt;
        DWORD dwEffect = e->GetEffects();

        ::GetCursorPos(&pt);
        point.x = pt.x;
        point.y = pt.y;
        pdt->DragOver(e->GetKeyStates(), point, &dwEffect);
        e->SetEffects((suic::DragDropEffects)dwEffect);

        pdt->Release();
    }
}

void TextBoxBase::OnDragLeave(suic::DragEventArg* e)
{
    e->SetHandled(true);

    IDropTarget *pdt = NULL;
    ITextServicesEx* txtService = ToRenderHost(this)->GetTextService();

    if (txtService->TxGetDropTarget(&pdt) == NOERROR)
    {
        pdt->DragLeave();
        pdt->Release();
    }
}

void TextBoxBase::OnDrop(suic::DragEventArg* e)
{
    suic::IDataStore* dataStore = e->GetData();

    if (NULL != dataStore)
    {
        DataHostObj* dataObj = suic::DynamicCast<DataHostObj>(dataStore->GetData(_U("DataObj")));
        if (NULL != dataObj)
        {
            IDropTarget *pdt = NULL;
            ITextServicesEx* txtService = ToRenderHost(this)->GetTextService();

            if (txtService->TxGetDropTarget(&pdt) == NOERROR)
            {
                POINTL point;
                suic::Point pt;
                DWORD dwEffect = e->GetEffects();

                ::GetCursorPos(&pt);
                point.x = pt.x;
                point.y = pt.y;
                pdt->Drop(dataObj->GetDataObject(), e->GetKeyStates(), point, &dwEffect);
                e->SetEffects((suic::DragDropEffects)dwEffect);

                pdt->Release();
            }
        }
    }

    e->SetHandled(true);
}

void TextBoxBase::OnTextChanged(suic::TextChangedEventArg* e)
{
    ToRenderHost(this)->SetText(e->GetText());
    TextEditor::OnTextChanged();
}

void TextBoxBase::OnCaretBrushChanged(suic::Brush* brush)
{
    TextEditor::OnCaretBrushChanged(brush);
    if (NULL != GetTextContainer())
    {
        GetTextContainer()->GetCaretElement()->SetBackground(brush);
    }
}

void TextBoxBase::OnScrollToPos(suic::eScrollType iType, int iFlag)
{
    switch (iType)
    {
    case suic::eScrollType::eScrollLineLeft:
        ToRenderHost(this)->TxSendMessage(WM_HSCROLL, SB_LINELEFT, 0L);
        break;

    case suic::eScrollType::eScrollLineRight:
        ToRenderHost(this)->TxSendMessage(WM_HSCROLL, SB_LINERIGHT, 0L);
        break;

    case suic::eScrollType::eScrollLineUp:
        ToRenderHost(this)->TxSendMessage(WM_VSCROLL, SB_LINEUP, 0L);
        break;

    case suic::eScrollType::eScrollLineDown:
        ToRenderHost(this)->TxSendMessage(WM_VSCROLL, SB_LINEDOWN, 0L);
        break;

    case suic::eScrollType::eScrollPageLeft:
        ToRenderHost(this)->TxSendMessage(WM_HSCROLL, SB_PAGELEFT, 0L);
        break;

    case suic::eScrollType::eScrollPageRight:
        ToRenderHost(this)->TxSendMessage(WM_HSCROLL, SB_PAGERIGHT, 0L);
        break;

    case suic::eScrollType::eScrollPageUp:
        ToRenderHost(this)->TxSendMessage(WM_VSCROLL, SB_PAGEUP, 0L);
        break;

    case suic::eScrollType::eScrollPageDown:
        ToRenderHost(this)->TxSendMessage(WM_VSCROLL, SB_PAGEDOWN, 0L);
        break;

    case suic::eScrollType::eScrollToHome:
        ToRenderHost(this)->TxSendMessage(WM_VSCROLL, SB_TOP, 0L);
        break;

    case suic::eScrollType::eScrollToEnd:
        ToRenderHost(this)->TxSendMessage(WM_VSCROLL, SB_BOTTOM, 0L);
        break;

    case suic::eScrollType::eScrollToHori:
        {
            WPARAM wp = MAKEWPARAM(SB_THUMBPOSITION, iFlag);
            ToRenderHost(this)->TxSendMessage(WM_HSCROLL, wp, 0L);
        }        
        break;

    case suic::eScrollType::eScrollToVert:
        {
            WPARAM wp = MAKEWPARAM(SB_THUMBPOSITION, iFlag);
            ToRenderHost(this)->TxSendMessage(WM_VSCROLL, wp, 0L);
        }   
        break;

    default:
        ToRenderHost(this)->TxSendMessage(EM_LINESCROLL, 0, iFlag);
    }
}

void TextBoxBase::OnScrollChanged(suic::Element* sender, suic::ScrollChangedEventArg* e)
{
    TextRenderHost* textHost = ToRenderHost(this);

    if (textHost && textHost->GetMaxLines() > 1 && _scrollViewer != NULL && !textHost->IsScrollPending())
    {
        textHost->SetScrollPending(true);
        textHost->ScrollToVPos(e->GetVertScrollOffset());
        textHost->ScrollToHPos(e->GetHoriScrollOffset());
        textHost->SetScrollPending(false);
    }
}

void TextBoxBase::OnIsReadOnlyChanged(suic::DpPropChangedEventArg* e)
{
    TextRenderHost* trScope = ToRenderHost(this);
    trScope->SetReadOnly(e->GetNewValue()->ToBool());
}

void TextBoxBase::OnIsReadOnlyCaretVisibleChanged(suic::DpPropChangedEventArg* e)
{
    TextRenderHost* trScope = ToRenderHost(this);
    trScope->SetIsReadOnlyCaretVisible(e->GetNewValue()->ToBool());
}

void TextBoxBase::OnSelectionBrushChanged(suic::DpPropChangedEventArg* e)
{
    suic::SolidColorBrush* brh = suic::RTTICast<suic::SolidColorBrush>(e->GetNewValue());
    TextRenderHost* trScope = ToRenderHost(this);

    if (brh)
    {
        trScope->SetSelBackground(brh->ToColor());
    }
    else
    {
        trScope->SetSelBackground(suic::Color::FromRgb(::GetSysColor(COLOR_HIGHLIGHT)));
    }
}

void TextBoxBase::OnSelectionTextBrushChanged(suic::DpPropChangedEventArg* e)
{
    suic::SolidColorBrush* brh = suic::RTTICast<suic::SolidColorBrush>(e->GetNewValue());
    TextRenderHost* trScope = ToRenderHost(this);

    if (brh)
    {
        trScope->SetSelTextColor(brh->ToColor());
    }
    else
    {
        trScope->SetSelTextColor(suic::Color::FromRgb(::GetSysColor(COLOR_HIGHLIGHTTEXT)));
    }
}

void TextBoxBase::OnTextWrappingChanged(suic::DpPropChangedEventArg* e)
{
    int iWrap = e->GetNewValue()->ToInt();
    TextRenderHost* textHost = ToRenderHost(this);

    if (iWrap == suic::TextWrapping::tNoWrap)
    {
        textHost->SetWordWrap(false);
    }
    else
    {
        textHost->SetWordWrap(true);
    }
}

void TextBoxBase::NotifyTextChanged()
{
    TextRenderHost* trScope = ToRenderHost(this);
    SetText(trScope->GetText());
}

void TextBoxBase::NotifyTextFontChanged(int flag)
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

void TextBoxBase::NotifyTextSelectionChanged()
{
    suic::RoutedEventArg e(this, SelectionChangedEvent);
    OnSelectionChanged(&e);
}

}
