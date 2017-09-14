
#include "stdafx.h"

#include <Extend/Editor/TextContainer.h>
#include <Extend/Editor/TextHostView.h>
#include <Extend/Editor/RichInterface.h>

#include <Extend/Editor/TextBoxBase.h>
#include <Extend/Editor/PasswordBox.h>

#include <System/Windows/DataHostObj.h>
#include <Framework/Controls/TextEditor.h>

namespace suic
{

ImplementRTTIOfClass(TextContainer, suic::FrameworkElement)

#define __ToRenderHost() ((TextRenderHost*)GetTextRender())

TextContainer::TextContainer()
    : _scrollInfo(NULL)
    , _hostElem(NULL)
{
    TextRenderHost* renderHost = new TextRenderHost();
    _textRender = renderHost;
    _caret = new suic::CaretElement();

    SetName(_U("PART_ScrollHost"));
    AddVisualChild(_caret);
    AddLogicalChild(_caret);

    _caret->ref();

    if (!renderHost->InitializeTextService(this))
    {
        renderHost->AddRef();
    }
}

TextContainer::~TextContainer()
{
    if (_scrollInfo)
    {
        delete _scrollInfo;
    }

    if (NULL != _textRender)
    {
        ((TextRenderHost*)(_textRender))->Release();
    }

    _caret->unref();
}

ITextRender* TextContainer::GetTextRender() const
{
    return _textRender;
}

Embbed* TextContainer::FindEmbbed(int cp)
{
    Embbed* pEmbbed = NULL;
    IRichEditOle *pRichEditOle = NULL;
    __ToRenderHost()->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle);

    if (pRichEditOle == NULL)
    {
        return pEmbbed;
    }

    REOBJECT reobject;
    TextRenderHost* renderHost = (TextRenderHost*)_textRender;
    renderHost->GetTextService()->TxFindOleObject(cp, &reobject);

    UIOleObject* pOleObj = dynamic_cast<UIOleObject*>(reobject.poleobj);
    if (pOleObj != NULL)
    {
        pEmbbed = pOleObj->GetEmbbedObj();
        if (pEmbbed != NULL)
        {
            pEmbbed->ref();
        }
    }

    if (reobject.poleobj)
    {
        reobject.poleobj->Release();
    }
    if (reobject.polesite)
    {
        reobject.polesite->Release();
    }
    if (reobject.pstg)
    {
        reobject.pstg->Release();
    }

    pRichEditOle->Release();
    return pEmbbed;
}

Embbed* TextContainer::HitTestEmbbed(suic::Point point)
{
    Embbed* pEmbbed = NULL;
    IRichEditOle *pRichEditOle = NULL;
    __ToRenderHost()->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle);

    if (pRichEditOle == NULL)
    {
        return pEmbbed;
    }

    suic::Rect rcClnt = _textRender->GetViewRect();
    TextRenderHost* renderHost = (TextRenderHost*)_textRender;
    int iFirstVisible = (int)renderHost->GetTextService()->TxFindFirstVisibleOleObject();
    int nCount = pRichEditOle->GetObjectCount();

    for (int i = iFirstVisible; i < nCount; ++i)
    {
        REOBJECT reobject;
        memset(&reobject, 0 , sizeof(REOBJECT));
        reobject.cbStruct = sizeof(REOBJECT);

        SCODE sc = pRichEditOle->GetObject(i, &reobject, REO_GETOBJ_POLEOBJ);

        if (sc != S_OK)
        {
            continue;
        }

        UIOleObject* pOleObj = dynamic_cast<UIOleObject*>(reobject.poleobj);
        if (pOleObj != NULL)
        {
            suic::Rect rect = pOleObj->GetRenderRect();
            if (rect.PointIn(point))
            {
                pEmbbed = pOleObj->GetEmbbedObj();
                if (pEmbbed != NULL)
                {
                    pEmbbed->ref();
                }
                break;
            }

            if (rect.top > rcClnt.bottom)
            {
                break;
            }
        }

        if (reobject.poleobj)
        {
            reobject.poleobj->Release();
        }

        if (NULL != pEmbbed)
        {
            break;
        }
    }

    pRichEditOle->Release();
    return pEmbbed;
}

suic::String TextContainer::GetText()
{
    return __ToRenderHost()->GetText();
}

void TextContainer::SetText(suic::String text)
{
    __ToRenderHost()->SetText(text);
}

bool TextContainer::GetReadOnly()
{
    return __ToRenderHost()->GetReadOnly();
}

void TextContainer::SetReadOnly(bool val)
{
    __ToRenderHost()->SetReadOnly(val);
}

bool TextContainer::GetEnabled()
{
    return IsEnabled();
}

bool TextContainer::GotFocus()
{
    DoMessage(WM_SETFOCUS, 0, 0, NULL);
    //ShowCaret(true);
    return true;
}

void TextContainer::UpdateView()
{
    __ToRenderHost()->RefleshView();
}

void TextContainer::SetLineLeftOffset(int index, int xLeft)
{
    __ToRenderHost()->SetLineLeftOffset(index, xLeft);
}

void TextContainer::SetTextCallback(suic::ITextCallback* textCb)
{
    __ToRenderHost()->SetTextCallback(textCb);
}

void TextContainer::RefleshVisual()
{
    InvalidateVisual();
}

void TextContainer::SetAcceptsTab(bool val)
{
    __ToRenderHost()->SetAcceptsTab(val);
}

void TextContainer::ShowCaret(bool bShow)
{
    if (IsInitialized())
    {
        GetCaretElement()->UpdateArrange();
    }

    if (bShow && _hostElem != NULL && _hostElem->IsKeyboardFocused())
    {
        GetCaretElement()->Show();
    }
    else
    {
        GetCaretElement()->Hide();
    }
}

void TextContainer::InitCaret(int w, int h)
{
    suic::CaretElement* caret = GetCaretElement();
    caret->SetWidth(w);
    caret->SetHeight(h);
    caret->SetSize(suic::Size(w, h));
}

void TextContainer::ResetCaret(suic::Point point)
{
    GetCaretElement()->SetPos(point);
    GetCaretElement()->InvalidateVisual();
}

int TextContainer::GetVisualChildrenCount()
{
    TextRenderHost* txtHost = __ToRenderHost();
    if (!txtHost->GetReadOnly() || txtHost->GetIsReadOnlyCaretVisible())
    {
        return (_caret->IsVisible() ? 1 : 0);
    }
    else
    {
        return 0;
    }
}

suic::Element* TextContainer::GetVisualChild(int index)
{
    return _caret;
}

void TextContainer::UpdateScrollRange(bool bHori, int minPos, int maxPos, bool bRedraw)
{
    suic::ScrollViewer* scrollViewer = GetScrollViewer();

    if (__ToRenderHost()->GetMaxLines() > 1 && scrollViewer != NULL)
    {
        int iSize = maxPos - minPos;
        suic::ScrollData* scrData = _scrollInfo->GetScrollData();

        if (bHori)
        {
            iSize += _textRender->GetPadding().SumLR();
            if (scrData->extent.cx != iSize)
            {
                scrData->extent.cx = iSize;
            }
        }
        else
        {
            iSize += _textRender->GetPadding().SumTB();
            if (scrData->extent.cy != iSize)
            {
                scrData->extent.cy = iSize;
            }
        }

        InvalidateScrollInfo(true);
        scrollViewer->InvalidateArrange();

        if (bRedraw)
        {
            scrollViewer->InvalidateVisual();
        }
    }
}

void TextContainer::UpdateScrollPos(bool bHori, int pos, bool bRedraw)
{
    suic::ScrollViewer* scrollViewer = GetScrollViewer();

    if (__ToRenderHost()->GetMaxLines() > 1 && scrollViewer != NULL)
    {
        suic::ScrollData* scrData = _scrollInfo->GetScrollData();

        if (bHori)
        {
            if (__ToRenderHost()->GetWordWrap())
            {
                scrollViewer->SetHorizontalScrollBarVisibility(suic::ScrollBarVisibility::sbvCollapsed);
            }
            else
            {
                scrData->offset.x = pos;
                scrData->computedOffset.x = pos;
                scrollViewer->SetHorizontalOffset(pos);
            }
        }
        else
        {
            scrData->offset.y = pos;
            scrData->computedOffset.y = pos;
            scrollViewer->SetVerticalOffset(pos);
        }

        scrollViewer->InvalidateArrange();

        if (bRedraw)
        {
            scrollViewer->InvalidateVisual();
        }
    }
}

//-------------------------------------------------------------
//

void TextContainer::OnArrange(const suic::Size& arrangeSize)
{
}

void TextContainer::OnRenderSizeChanged(suic::SizeChangedInfo& e)
{
    suic::FrameworkElement::OnRenderSizeChanged(e);
}

void TextContainer::UpdateViewRect()
{
    suic::Point pt = GetCanvasOffset();
    suic::Rect rcClient(pt, GetRenderSize());
    __ToRenderHost()->SetClntRect(rcClient);
}

void TextContainer::OnRender(suic::Drawing* drawing)
{
    UpdateViewRect();
    _textRender->Render(drawing);
}

bool TextContainer::DoMessage(suic::Uint32 message, suic::Uint32 wp, suic::Uint32 lp, suic::Uint64* lr)
{
    if (NULL != _textRender)
    {
        return __ToRenderHost()->DoMessage(message, wp, lp, lr);
    }
    else
    {
        return 0;
    }
}

void TextContainer::InvalidateScrollInfo(bool bRaised)
{
    if (GetScrollInfo() != NULL)
    {
        suic::ScrollData* scrData = _scrollInfo->GetScrollData();

        if (_scrollInfo->GetScrollOwner())
        {
            __ToRenderHost()->SetScrollPending(true);
            GetScrollViewer()->InvalidateScrollInfo(bRaised);
            __ToRenderHost()->SetScrollPending(false);
        }
    }
}

suic::Size TextContainer::OnMeasure(const suic::Size& constraint)
{
    MeasureChild(_caret, constraint);

    suic::IScrollInfo* scrInfo = GetScrollInfo();

    if (scrInfo)
    {
        suic::ScrollData* scrData = _scrollInfo->GetScrollData();
        scrData->viewport = constraint;
        scrData->SetComputedOffset(scrData->offset);
    }

    if (__ToRenderHost()->GetMaxLines() <= 1)
    {
        suic::TmParam tm;
        suic::FormattedText fmtText;
        fmtText.SetFontFamily(__ToRenderHost()->GetCF().szFaceName);
        fmtText.SetFontSize(__ToRenderHost()->GetFontSize());
        fmtText.MeasureString(tm, L"A", 1);
        return suic::Size(tm.size.cy + 2, tm.size.cy + 2);
    }
    else
    {
        return suic::Size();
    }
}

void TextContainer::OnSetCursor(suic::CursorEventArg* e)
{
    suic::Point pt = PointToScreen(suic::Point());
    suic::Rect rect(0, 0, GetActualWidth(), GetActualHeight());
    rect.Offset(pt);
    pt = e->GetMousePoint();
    e->SetCursor(suic::OCursor::Ibeam);
    e->SetHandled(true);
}

void TextContainer::OnTextInput(suic::KeyboardEventArg* e)
{
    suic::Uint64 lr = 0;

    if (e->GetKey() != suic::Key::kTab || 
        (__ToRenderHost()->GetAcceptsTab() && !e->IsControlDown() && !e->IsShiftDown()))
    {
        e->SetHandled(true);
        DoMessage(WM_CHAR, suic::Assigner::Lastmp()->wp, suic::Assigner::Lastmp()->lp, &lr);
    }
}

void TextContainer::OnImeEvent(suic::MessageEventArg* e)
{
    suic::Uint64 lr = 0;
    suic::MessageParam* mp = e->GetMessageParam();

    DoMessage(mp->message, mp->wp, mp->lp, &lr);

    if (S_OK == lr)
    {
        e->SetHandled(true);
    }
}

void TextContainer::OnKeyDown(suic::KeyboardEventArg* e)
{
    suic::Uint64 lr = 0;
    if (e->GetKey() != suic::Key::kTab || (__ToRenderHost()->GetAcceptsTab() && !e->IsControlDown() && !e->IsShiftDown()))
    {
        e->SetHandled(true);
        DoMessage(WM_KEYDOWN, suic::Assigner::Lastmp()->wp, suic::Assigner::Lastmp()->lp, &lr);
    }
}

void TextContainer::OnKeyUp(suic::KeyboardEventArg* e)
{
    suic::Uint64 lr = 0;
    e->SetHandled(true);
    DoMessage(WM_KEYUP, suic::Assigner::Lastmp()->wp, suic::Assigner::Lastmp()->lp, &lr);
}

void TextContainer::OnLoaded(suic::LoadedEventArg* e)
{

}

void TextContainer::OnUnloaded(suic::LoadedEventArg* e)
{
    __ToRenderHost()->_hwnd = NULL;
}

void TextContainer::OnLostFocus(suic::RoutedEventArg* e)
{
    DoMessage(WM_KILLFOCUS, 0, 0, NULL);
    ShowCaret(false);
}

void TextContainer::OnLostKeyboardFocus(suic::KeyboardFocusEventArg* e)
{
    DoMessage(WM_KILLFOCUS, 0, 0, NULL);
    ShowCaret(false);
}

void TextContainer::OnMouseEnter(suic::MouseButtonEventArg* e)
{
    FrameworkElement::OnMouseEnter(e);
    OnMouseMove(e);
}

void TextContainer::OnMouseMove(suic::MouseButtonEventArg* e)
{
    DoMessage(WM_MOUSEMOVE, suic::Assigner::Lastmp()->wp, suic::Assigner::Lastmp()->lp, NULL);
}

void TextContainer::OnMouseLeave(suic::MouseButtonEventArg* e)
{
    FrameworkElement::OnMouseLeave(e);
    DoMessage(suic::Assigner::Lastmp()->message, suic::Assigner::Lastmp()->wp, suic::Assigner::Lastmp()->lp, NULL);
}

void TextContainer::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    FrameworkElement::OnMouseLeftButtonDown(e);

    bool needGotFocus = true;

    if (NULL != _hostElem)
    {
        if (!_hostElem->IsFocused())
        {
            needGotFocus = false;
        }
        _hostElem->Focus();
    }

    if (needGotFocus)
    {
        GotFocus();
    }

    if ((e->GetClickCount() % 2) == 0)
    {
        OnMouseLeftButtonDbclk(e);
        ShowCaret(false);
    }
    else
    {
        DoMessage(WM_LBUTTONDOWN, suic::Assigner::Lastmp()->wp, suic::Assigner::Lastmp()->lp, NULL);
    }
}

void TextContainer::OnMouseLeftButtonDbclk(suic::MouseButtonEventArg* e)
{
    GotFocus();
    DoMessage(WM_LBUTTONDBLCLK, suic::Assigner::Lastmp()->wp, suic::Assigner::Lastmp()->lp, NULL);
}

void TextContainer::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
{
    FrameworkElement::OnMouseLeftButtonUp(e);
    DoMessage(WM_LBUTTONUP, suic::Assigner::Lastmp()->wp, suic::Assigner::Lastmp()->lp, NULL);
}

}
