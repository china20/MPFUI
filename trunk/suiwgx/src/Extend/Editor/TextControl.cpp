
#include "stdafx.h"

#include <Extend/RichText/TextControl.h>
#include <Extend/RichText/TextHostView.h>
#include <Extend/RichText/RichInterface.h>
#include <Framework/Controls/TextEditor.h>

namespace sui
{

ImplementRTTIOfClass(TextControl, suic::FrameworkElement)

TextControl::TextControl()
    : _scrollInfo(NULL)
    , _owner(NULL)
{
    _renderHost = new TextRenderHost();
    _caret = new suic::CaretElement();

    SetName(_U("PART_ScrollHost"));
    AddVisualChild(_caret);

    _caret->ref();

    if (!_renderHost->InitializeTextService(this))
    {
        _renderHost->AddRef();
    }
}

TextControl::~TextControl()
{
    if (_scrollInfo)
    {
        delete _scrollInfo;
    }

    if (NULL != _renderHost)
    {
        ((TextRenderHost*)(_renderHost))->Release();
    }

    _caret->unref();
}

ITextCallback* TextControl::GetTextCallback() const
{
    return _owner;
}

TextRenderHost* TextControl::GetTextRender() const
{
    return _renderHost;
}

String TextControl::GetText()
{
    return GetTextRender()->GetText();
}

void TextControl::SetText(String text)
{
    GetTextRender()->SetText(text);
}

bool TextControl::GetReadOnly()
{
    return GetTextRender()->GetReadOnly();
}

void TextControl::SetReadOnly(bool val)
{
    GetTextRender()->SetReadOnly(val);
}

bool TextControl::GetEnabled()
{
    return IsEnabled();
}

bool TextControl::GotFocus()
{
    return Focus();
}

void TextControl::RefleshVisual()
{
    InvalidateVisual();
}

void TextControl::SetAcceptsTab(bool val)
{
    GetTextRender()->SetAcceptsTab(val);
}

void TextControl::ShowCaret(bool bShow)
{
    if (IsInitialized())
    {
        GetCaret()->UpdateArrange();
    }

    if (bShow && ((TextRenderHost*)_renderHost)->GetFocused())
    {
        GetCaret()->Show();
    }
    else
    {
        GetCaret()->Hide();
    }
}

void TextControl::InitCaret(int w, int h)
{
    GetCaret()->SetWidth(w);
    GetCaret()->SetHeight(h);
    GetCaret()->SetSize(Size(w, h));
}

void TextControl::ResetCaret(Point point)
{
    GetCaret()->SetPos(point);
    GetCaret()->InvalidateVisual();
}

int TextControl::GetVisualChildrenCount()
{
    return (_caret->IsVisible() ? 1 : 0);
}

Element* TextControl::GetVisualChild(int index)
{
    return _caret;
}

void TextControl::UpdateScrollRange(bool bHori, int minPos, int maxPos, bool bRedraw)
{
    ScrollViewer* scrollViewer = GetScrollViewer();

    if (_renderHost->GetMaxLines() > 1 && scrollViewer != NULL && GetScrollInfo())
    {
        ScrollData* scrData = _scrollInfo->GetScrollData();

        if (bHori)
        {
            scrData->extent.cx = maxPos - minPos + _renderHost->GetInsetRect().SumLR();
        }
        else
        {
            scrData->extent.cy = maxPos - minPos + _renderHost->GetInsetRect().SumTB();
        }

        InvalidateScrollInfo();

        if (bRedraw)
        {
            scrollViewer->InvalidateVisual();
        }
    }
}

void TextControl::UpdateScrollPos(bool bHori, int pos, bool bRedraw)
{
    ScrollViewer* scrollViewer = GetScrollViewer();

    if (_renderHost->GetMaxLines() > 1 && GetScrollInfo() && 
        scrollViewer != NULL && !IsOperationPending())
    {
        ScrollData* scrData = _scrollInfo->GetScrollData();

        if (bHori)
        {
            scrData->_computedOffset.x = pos;
            scrData->offset.x = pos;
            scrollViewer->SetHorizontalOffset(pos);
        }
        else
        {
            scrData->_computedOffset.y = pos;
            scrData->offset.y = pos;
            scrollViewer->SetVerticalOffset(pos);
        }

        if (bRedraw)
        {
            scrollViewer->UpdateLayout();
        }
    }
}

//-------------------------------------------------------------
//

void TextControl::OnRenderSizeChanged(SizeChangedInfo& e)
{
    FrameworkElement::OnRenderSizeChanged(e);

    Point pt = PointToScreen(Point());
    Rect rcClient(Point(), e.NewSize());

    //
    // 这里的区域是相对于宿主窗口客户全的偏移，因此需要校正其坐标
    //
    rcClient.Offset(pt);
    _renderHost->SetClntRect(rcClient);
}

void TextControl::OnRender(Drawing* drawing)
{
    Point pt = PointToScreen(Point());
    Rect rcClient(pt, GetRenderSize());

    _renderHost->SetClntRect(rcClient);
    _renderHost->Render(drawing);
}

bool TextControl::CallMessage(Uint32 message, Uint32 wp, Uint32 lp, Uint64* lr)
{
    if (NULL != _renderHost)
    {
        return _renderHost->CallMessage(message, wp, lp, lr);
    }
    else
    {
        return 0;
    }
}

void TextControl::InvalidateScrollInfo()
{
    if (GetScrollInfo() != NULL)
    {
        ScrollData* scrData = _scrollInfo->GetScrollData();

        if (_scrollInfo->GetScrollOwner())
        {
            GetScrollViewer()->InvalidateScrollInfo(false);
        }
    }
}

Size TextControl::OnMeasure(const Size& constraint)
{
    _caret->Measure(constraint);

    if (GetScrollInfo())
    {
        ScrollData* scrData = _scrollInfo->GetScrollData();
        scrData->viewport = constraint;
        scrData->_computedOffset = scrData->offset;
    }

    if (_renderHost->GetMaxLines() <= 1)
    {
        suic::TmParam tm;
        suic::FormattedText fmtText;
        fmtText.SetFontFamily(_renderHost->GetCF().szFaceName);
        fmtText.SetFontSize(_renderHost->GetFontSize());
        fmtText.MeasureString(tm, L"A", 1);
        return suic::Size(constraint.cx, tm.size.cy + 2);
    }
    else
    {
        return constraint;
    }
}

Size TextControl::OnArrange(const Size& arrangeSize)
{
    return arrangeSize;
}

void TextControl::OnSetCursor(CursorEventArg* e)
{
    suic::Point pt = PointToScreen(suic::Point());
    suic::Rect rect(0, 0, GetActualWidth(), GetActualHeight());
    rect.Offset(pt);
    pt = e->GetMousePoint();
    e->SetCursor(OCursor::Ibeam);
    e->SetHandled(true);
}

void TextControl::OnTextInput(suic::KeyboardEventArg* e)
{
    Uint64 lr = 0;

    if (e->GetKey() != Key::kTab || (GetTextRender()->GetAcceptsTab() && !e->IsControlDown() && !e->IsShiftDown()))
    {
        e->SetHandled(true);
        CallMessage(WM_CHAR, suic::Assigner::Lastmp()->wp, Assigner::Lastmp()->lp, &lr);
    }
}

void TextControl::OnKeyDown(suic::KeyboardEventArg* e)
{
    Uint64 lr = 0;
    if (e->GetKey() != Key::kTab || (GetTextRender()->GetAcceptsTab() && !e->IsControlDown() && !e->IsShiftDown()))
    {
        e->SetHandled(true);
        CallMessage(WM_KEYDOWN, suic::Assigner::Lastmp()->wp, Assigner::Lastmp()->lp, &lr);
    }
}

void TextControl::OnKeyUp(suic::KeyboardEventArg* e)
{
    Uint64 lr = 0;
    e->SetHandled(true);
    CallMessage(WM_KEYUP, suic::Assigner::Lastmp()->wp, Assigner::Lastmp()->lp, &lr);
}

bool TextControl::OnEvent(Object* sender, MessageParam* mp)
{
    Uint64 lr = 0;
    bool handled = CallMessage(mp->msg, mp->wp, mp->lp, &lr);

    if (handled)
    {
        mp->result = (Uint32)lr;
    }

    if (mp->msg == WM_IME_COMPOSITION)
    {
        return false;
    }

    return handled;
}

void TextControl::OnLoaded(suic::LoadedEventArg* e)
{

}

void TextControl::OnUnloaded(suic::LoadedEventArg* e)
{
    _renderHost->_hwnd = NULL;
}

void TextControl::OnGotFocus(RoutedEventArg* e)
{
    //
    // 鼠标压下时已经设置过
    //
    if (MouseDevice::GetLeftButton() != MouseButtonState::eMousePress)
    {
        CallMessage(WM_SETFOCUS, Assigner::Lastmp()->wp, Assigner::Lastmp()->lp, NULL);
    }
}

void TextControl::OnLostFocus(RoutedEventArg* e)
{
    CallMessage(WM_KILLFOCUS, Assigner::Lastmp()->wp, Assigner::Lastmp()->lp, NULL);
    ShowCaret(false);
}

void TextControl::OnMouseEnter(MouseButtonEventArg* e)
{
    FrameworkElement::OnMouseEnter(e);
    OnMouseMove(e);
}

void TextControl::OnMouseMove(MouseButtonEventArg* e)
{
    e->SetHandled(true);
    CallMessage(WM_MOUSEMOVE, Assigner::Lastmp()->wp, Assigner::Lastmp()->lp, NULL);
}

void TextControl::OnMouseLeave(MouseButtonEventArg* e)
{
    FrameworkElement::OnMouseLeave(e);
    CallMessage(Assigner::Lastmp()->msg, Assigner::Lastmp()->wp, Assigner::Lastmp()->lp, NULL);
}

void TextControl::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    FrameworkElement::OnMouseLeftButtonDown(e);

    if ((e->GetClickCount() % 2) == 0)
    {
        OnMouseLeftButtonDbclk(e);
        ShowCaret(false);
    }
    else
    {
        CallMessage(WM_SETFOCUS, Assigner::Lastmp()->wp, Assigner::Lastmp()->lp, NULL);
        CallMessage(WM_LBUTTONDOWN, Assigner::Lastmp()->wp, Assigner::Lastmp()->lp, NULL);
    }
}

void TextControl::OnMouseLeftButtonDbclk(MouseButtonEventArg* e)
{
    CallMessage(WM_SETFOCUS, Assigner::Lastmp()->wp, Assigner::Lastmp()->lp, NULL);
    CallMessage(WM_LBUTTONDBLCLK, Assigner::Lastmp()->wp, Assigner::Lastmp()->lp, NULL);
}

void TextControl::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    FrameworkElement::OnMouseLeftButtonUp(e);
    CallMessage(WM_LBUTTONUP, Assigner::Lastmp()->wp, Assigner::Lastmp()->lp, NULL);
}

}
