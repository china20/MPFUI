// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// Sharpui界面库允许任何个人或团体免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////////
// TextSample.cpp

#include <Framework/Controls/TextSimple.h>
#include <Framework/Controls/WindowHelper.h>
#include <Framework/Controls/Menu.h>
#include <Framework/Controls/TextBox.h>

#include <System/Render/Skia/SkTextOp.h>
#include <System/Windows/EventManager.h>
#include <System/Windows/CoreTool.h>
#include <System/Tools/CoreHelper.h>
#include <System/Input/KeyboardNavigation.h>

namespace suic
{

DpProperty* TextSimple::TextProperty;
DpProperty* TextSimple::PasswordCharProperty;

RoutedEvent* TextSimple::TextChangedEvent;

ImplementRTTIOfClass(TextSimple, TextElement)

//Italic Normal Oblique
//Black Bold Light Medium Normal Thin SemiBold ExtraBold ExtraBlack ExtraLight

TextSimple::TextSimple()
{
    _password = _U('');

    _document.setAutoDelete(false);
    _document.SetOwner(this);

    _caret.setAutoDelete(false);
    _caret.SetVisibility(Visibility::Hidden);
}

TextSimple::~TextSimple()
{
}

void TextSimple::OnTextPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TextSimple* tb = DynamicCast<TextSimple>(d);
    tb->SetText(e->GetNewValue()->ToString());
}

void TextSimple::OnPasswordCharPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TextSimple* tb = DynamicCast<TextSimple>(d);
    String strVal = e->GetNewValue()->ToString();

    if (strVal.Length() == 1)
    {
        tb->_password = strVal[0];
    }
    else
    {
        tb->_password = _U('');
    }
}

bool TextSimple::StaticInit()
{
    TextChangedEvent = EventControl::RegisterRoutedEvent(_T("TextChanged"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
    PasswordCharProperty = DpProperty::Register(_T("PasswordChar"), RTTIType(), OString::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender, &TextSimple::OnPasswordCharPropChanged));
    TextProperty = suic::DpProperty::Register(_T("Text"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(new OString(), PropMetadataOptions::AffectsArrange, &TextSimple::OnTextPropChanged));

    CursorProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(OCursor::Ibeam));

    return true;
}

String TextSimple::GetText()
{
    String text;
    _document.GetText(text);
    return text;
}

void TextSimple::SetText(String text)
{
    _document.SelectAll();
    _document.ReplaceSelect(text);
    _document.SetHorizontalOffset(0);

    UpdateScrollInfo(true);
}

void TextSimple::AppendText(String& text)
{
    _document.Add(text);
    UpdateScrollInfo(true);
}

void TextSimple::GetText(String & text)
{
    _document.GetText(text);
}

String TextSimple::GetRegularExp() const
{
    return String();
}

void TextSimple::SetRegularExp(String val)
{
}

void TextSimple::AddTextChanged(RoutedEventHandler* e)
{

}

void TextSimple::RemoveTextChanged(RoutedEventHandler* e)
{

}

void TextSimple::NotifyHorizontalOffset(int val)
{
    /*if (TextCaretChangedEvent.safe_bool())
    {
        TextCaretChangedEvent(val, _caret.GetWidth(), true);
    }
    else*/
    {
        int iCaret = _caret.GetWidth();

        if (val < _document.GetHorizontalOffset())
        {
            _document.SetHorizontalOffset(val);
        }
        else if (val + iCaret > (_document.GetHorizontalOffset() + GetActualWidth()))
        {
            _document.SetHorizontalOffset(val + iCaret - GetActualWidth());
        }
    }
}

void TextSimple::NotifyVerticalOffset(int val)
{
    if (!GetSingleLine() && TextCaretChangedEvent.safe_bool())
    {
        TextCaretChangedEvent(val, _document.GetLineHeight(), false);
    }
}

Rect TextSimple::DocumentRect(Point pt)
{
    Rect rc(pt.x, pt.y, GetActualWidth(), GetActualHeight());
    return rc;
}

void TextSimple::Cut()
{
    if (GetReadOnly() || !_document.IsSelectMode())
    {
        return;
    }

    Copy();

    _document.DeleteSelect();

    UpdateScrollInfo(true);
    ResetCaretPos();
}

void TextSimple::Copy()
{
    if (!_document.IsSelectMode())
    {
        return;
    }

    _caret.Hide();

    WndHelper helper(this);

    if (!helper.OpenClipboard())
    {
        return;
    }

    String strSel;
    _document.GetSelectText(strSel);

    helper.CopyText(strSel);
    helper.CloseClipboard();
}

void TextSimple::Paste()
{
    if (GetReadOnly())
    {
        return;
    }

    WndHelper helper(this);

    if (!helper.OpenClipboard()) 
    {
        return;
    }

    String text;
    helper.PasteText(text);
    _document.Add(text);

    helper.CloseClipboard();

    UpdateScrollInfo(true);
    ResetCaretPos();
}

void TextSimple::SelectAll()
{
    _caret.Hide();

    _document.SelectAll();
    UpdateScrollInfo(false);
    ResetCaretPos();
}

Size TextSimple::OnMeasure(const Size& size)
{
    Size measureSize = _document.Measure();
    measureSize.cx += _caret.GetWidth();

    return measureSize;
}

void TextSimple::OnArrange(const Size& size)
{
    Rect rect(0, 0, size.Width(), size.Height());

    _document.SetVisualRect(&rect);
    ResetCaretPos();
}

void TextSimple::OnRender(Drawing * drawing)
{     
    fRect rect(0, 0, (Float)GetActualWidth(), (Float)GetActualHeight());
    FormattedText fmtText;
    Brush* foreground = GetForeground();

    InitFormattedText(&fmtText);

    if (GetSingleLine())
    {
        _document.DrawSingleLine(this, drawing, &fmtText, Colors::BlueViolet, Colors::White, &rect);
    }
    else
    {
        _document.Draw(this, drawing, &fmtText, Colors::BlueViolet, Colors::White, &rect);
    }
}

static Uint32 WCharToChar(wchar_t w, Byte * asc)   
{
    DWORD dwNum = WideCharToMultiByte(CP_ACP, 0, &w, 1, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, &w, 1, (LPSTR)asc, dwNum, NULL, NULL);

    return dwNum;
}

void TextSimple::OnTextInput(KeyboardEventArg* e)
{
    e->SetHandled(true);

    if (GetReadOnly() || e->GetKey() == Key::kTab)
    {
        return;
    }

    KeyboardNavigation::Current()->HideFocusVisual();

    int ch = e->GetKey();
    ModifierKey code = e->GetModifierKey();

    union wcharfmt
    {
        Char w;
        Byte asc[2];
    };

    static bool bUnicode = false;
    static wcharfmt wf;

    Uint32 nNum = WCharToChar((wchar_t)ch, (Byte*)&wf.asc[0]);

    Byte nChar = (Byte)ch;

    // 这是一个双字节的编码
    if (ch >= 0x80)
    {
        _document.Add(ch);
        UpdateScrollInfo(true);
    }
    // 处理ascii码，注意这里可能是多字节的一部分
    else if (nChar >= 0x80)
    {
        bUnicode = true;
        wf.asc[0] = nChar;
    }
    else
    {
        if (bUnicode)
        {
            wf.asc[1] = nChar;
        }
        else
        {
            if (VK_BACK == nChar)
            {
                _document.DelLeftOne();
                UpdateScrollInfo(true);
            }
            else if (nChar >= 32 && nChar <= 126)
            {
                bool bCtrl = (code & ModifierKey::mkControl) == ModifierKey::mkControl;
                if (!bCtrl)
                {
                    _document.Add(ch);
                    UpdateScrollInfo(true);
                }
                else if (nChar == 'v' || nChar == 'V')
                {
                    Paste();
                }
            }
        }

        bUnicode = false;
    }

    ResetCaretPos();
    _caret.Show();
}

bool TextSimple::GetReadOnly()
{
    return false;
}

void TextSimple::OnKeyDown(KeyboardEventArg* e)
{
    if (GetReadOnly() || e->GetKey() == Key::kTab)
    {
        return;
    }

    KeyboardNavigation::Current()->HideFocusVisual();
    ModifierKey state = e->GetModifierKey();
    int ch = e->GetKey();

    bool bShift = e->IsShiftDown();
    bool bCtrl = e->IsControlDown();

    bool bValid = true;
    bool bUpDown = false;
    bool bSel = _document.IsSelectMode();

    Rect oldCaret = _document.GetCaret(true);

    if (Key::kLeft == ch)
    {
        bValid = _document.OnLeftCaret(bShift);
    }
    else if (Key::kRight == ch)
    {
        bValid = _document.OnRightCaret(bShift);
    }
    else if (Key::kUp == ch)
    {
        bValid = _document.OnUpCaret(bShift);
        bUpDown = true;
    }
    else if (Key::kDown == ch)
    {
        bValid = _document.OnDownCaret(bShift);
        bUpDown = true;
    }
    else if (Key::kDelete == ch)
    {
        _document.DelRightOne();
    }
    else if (Key::kReturn == ch)
    {
        if (!GetSingleLine())
        {
            _document.InsertLine();
        }
        else
        {
            FrameworkElement::OnKeyDown(e);
            return;
        }
    }
    else if (Key::kEscape == ch)
    {
        _document.CancelSelect();
    }
    else if (Key::kHome == ch)
    {
        if (bCtrl)
        {
            bValid = _document.OnDocHome(bShift);
        }
        else
        {
            bValid = _document.OnLineHome(bShift);
        }
    }
    else if (Key::kEnd == ch)
    {
        if (bCtrl)
        {
            bValid = _document.OnDocEnd(bShift);
        }
        else
        {
            bValid = _document.OnLineEnd(bShift);
        }
    }
    // 下一页
    else if (Key::kNext == ch)
    {
        if (bCtrl)
        {
            bValid = _document.OnVisualHome(bShift);
        }
        else
        {
            bValid = _document.OnPageNext(bShift);
        }
    }
    // 上一页
    else if (Key::kPrior == ch)
    {
        if (bCtrl)
        {
            bValid = _document.OnVisualEnd(bShift);
        }
        else
        {
            bValid = _document.OnPagePrior(bShift);
        }
    }
    else if (bCtrl && (ch == 'v' || ch == 'V'))
    {
        Paste();
        e->SetHandled(true);
        return;
    }
    else if (bCtrl && (ch == 'c' || ch == 'C'))
    {
        Copy();
        e->SetHandled(true);
        return;
    }
    else if (bCtrl && (ch == 'x' || ch == 'X'))
    {
        Cut();
        e->SetHandled(true);
        return;
    }
    else if (bCtrl && (ch == 'a' || ch == 'A'))
    {
        SelectAll();
        e->SetHandled(true);
        return;
    }
    else
    {
        return;
    }

    e->SetHandled(true);

    if (bValid || bSel)
    {
        UpdateScrollInfo(Key::kReturn == ch ? true : false);
    }

    InvalidateVisual();
    ResetCaretPos(bUpDown);
}

void TextSimple::OnKeyUp(KeyboardEventArg* e)
{
    e->SetHandled(true);
}

void TextSimple::OnMouseMove(MouseButtonEventArg* e)
{
    e->SetHandled(true);

    if (IsMouseCaptured())
    {
        _caret.Hide();

        Rect rc(DocumentRect(PointToScreen(Point())));
        Point tmPt(e->GetMousePoint());

        tmPt.x -= rc.left;
        tmPt.y -= rc.top;
        _document.TrackCaret(&tmPt);
        UpdateScrollInfo(false);
        ResetCaretPos();
    }
}

void TextSimple::OnGotFocus(RoutedEventArg* e)
{
    e->SetHandled(true);
    ResetCaretPos();
    _caret.Show();
    FrameworkElement::OnGotFocus(e);
}

void TextSimple::OnLostFocus(RoutedEventArg* e)
{
    _caret.Hide();
    FrameworkElement::OnLostFocus(e);
    SetValue(TextProperty, new OString(GetText()));
}

void TextSimple::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    //e->SetHandled(true);
    _caret.Hide();

    if ((e->GetClickCount() % 2) == 0)
    {
        OnMouseLeftButtonDbclk(e);
    }
    else
    {
        SetCaptureMouse();

        Rect rcClnt(DocumentRect(PointToScreen(Point())));
        int code = e->GetMouseState();
        bool bShift = (code & MK_SHIFT) == MK_SHIFT;
        bool bCtrl = (code & MK_CONTROL) == MK_CONTROL;
        Rect rc(rcClnt);
        Point tmPt(e->GetMousePoint());

        tmPt.x -= rc.left;
        tmPt.y -= rc.top;

        bool bValid = false;

        if (bShift)
        {
            _document.TrackCaret(&tmPt);
            bValid = true;
        }
        else if (bCtrl)
        {
            _document.SelectWord(&tmPt);
            bValid = true;
        }
        else
        {
            bValid = _document.SetCaret(&tmPt);
        }

        if (bValid)
        {
            UpdateScrollInfo(false);
        }
    }
    _caret.Show();
}

void TextSimple::OnMouseLeftButtonDbclk(MouseButtonEventArg* e)
{
    Point pt(PointToScreen(Point()));
    Rect rc(pt.x, pt.y, GetRenderSize().cx, GetRenderSize().cy);
    Point tmPt(e->GetMousePoint());

    tmPt.x -= rc.left;
    tmPt.y -= rc.top;

    bool bValid = _document.SelectWord(&tmPt);

    if (bValid)
    {
        UpdateScrollInfo(false);
    }

    ResetCaretPos();
}

void TextSimple::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    Point pt(PointToScreen(Point()));
    Rect rc(pt.x, pt.y, GetRenderSize().cx, GetRenderSize().cy);

    e->SetHandled(true);
    ReleaseCaptureMouse();
    ResetCaretPos();
    _caret.Show();
}

void TextSimple::OnMouseRightButtonDown(MouseButtonEventArg* e)
{
    e->SetHandled(true);
    ContextMenu* menu = GetContextMenu();
}

int TextSimple::GetVisualChildrenCount()
{
    if (_caret.IsVisible())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

Element* TextSimple::GetVisualChild(int index)
{
    return &_caret;
}

void TextSimple::OnInitialized(EventArg* e)
{
    FrameworkElement::OnInitialized(e);
    _caret.RecursiveInit();
    AddVisualChild(&_caret);

    _document.InitDocument(this);
}

void TextSimple::ResetCaretPos(bool bUpDown)
{
    Point pt = PointToScreen(Point());
    Rect rcCaret = _document.GetCaret(bUpDown);

    rcCaret.right = rcCaret.left + _caret.GetWidth();
    _caret.SetDesiredSize(rcCaret.ToSize());
    _caret.Arrange(rcCaret);

    UpdateLayout();
}

void TextSimple::InitFormattedText(FormattedText* fmtText)
{
    fmtText->SetFontFamily(GetFontFamily());
    fmtText->SetSingleline(GetSingleLine());
    fmtText->SetFontSize(GetFontSize());
    fmtText->SetFontWeight(GetFontWeight());
}

bool TextSimple::GetSingleLine()
{
    if (_password != _U(''))
    {
        return true;
    }
    else
    {
        return TextElement::GetSingleLine();
    }
}

void TextSimple::UpdateScrollInfo(bool bUpdate)
{
    //RefleshVisual();
}

void TextSimple::RaiseTextChanged(String oldText)
{
    RoutedEventArg e(this, TextChangedEvent);
    RaiseEvent(&e);
    //RefleshVisual();
}

}
