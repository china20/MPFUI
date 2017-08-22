// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////////
// TextContainer.cpp

#include "stdafx.h"
#include "TextContainer.h"

namespace suic
{

ImplementRTTIOfClass(TextContainer, suic::ITextContainer)

TextContainer::TextContainer()
    : _richTxt(NULL)
{
    _richTxt = new TextHostService();
    _richTxt->AddRef();
}

TextContainer::~TextContainer()
{
    if (_richTxt)
    {
        _richTxt->Release();
    }
}

CharFormat::CharFormat()
    : size(0)
    , color(-1)
    , weight(0)
    , italic(0)
    , underline(0)
{
    _tcscpy_s(facename, 32, _T(""));
}

void SetCharFormat2(CHARFORMAT2& cf2, const CharFormat& cf)
{
    if (cf.color != -1)
    {
        cf2.crTextColor = ARGBTORGB(cf.color);
    }

    if (cf.weight > 700)
    {
        cf2.dwEffects |= CFE_BOLD;
    }
    else if (cf.weight > 0)
    {
        cf2.dwEffects &= ~CFE_BOLD;
    }

    if (cf.italic != 0)
    {
        cf2.dwEffects |= CFE_ITALIC;
    }
    else if (cf.weight > 0)
    {
        cf2.dwEffects &= ~CFE_ITALIC;
    }

    if (cf.underline != 0)
    {
        cf2.dwEffects |= CFE_UNDERLINE;
    }
    else if (cf.weight > 0)
    {
        cf2.dwEffects &= ~CFE_UNDERLINE;
    }

    if (cf.size > 0)
    {
        cf2.yHeight = -cf.size * 1440 / 96;
    }

    if (_tcslen(cf.facename) > 0)
    {
        _tcscpy(cf2.szFaceName, cf.facename);
    }
}

void GetCharFormat(const CHARFORMAT2& cf2, CharFormat& cf)
{
    cf.color = RGBTOARGB(cf2.crTextColor);
    if ((cf2.dwEffects & CFE_BOLD) == CFE_BOLD)
    {
        cf.weight = 800;
    }
    else
    {
        cf.weight = 1;
    }

    if ((cf2.dwEffects & CFE_ITALIC) == CFE_ITALIC)
    {
        cf.italic = 1;
    }
    else
    {
        cf.italic = 0;
    }

    if ((cf2.dwEffects & CFE_UNDERLINE) == CFE_UNDERLINE)
    {
        cf.underline = 1;
    }
    else
    {
        cf.underline = 0;
    }

    cf.size = -cf2.yHeight * 96 / 1440;
    _tcscpy(cf.facename, cf2.szFaceName);
}

int TextContainer::Select(long lStart, long lEnd)
{
    CHARRANGE cr;
    cr.cpMin = lStart;
    cr.cpMax = lEnd;
    LRESULT lResult = 0;
    TXSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult);

    return (int)lResult;
}

void TextContainer::ReplaceSel(const suic::String& strText, bool bCanUndo)
{
    TXSendMessage(EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)strText.c_str()); 
}

long TextContainer::GetTextSize(DWORD dwFlags) const
{
    GETTEXTLENGTHEX textLenEx;
    textLenEx.flags = dwFlags;
    textLenEx.codepage = 1200;

    return (long)TXSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0);
}

suic::String TextContainer::GetText() const
{
    long lLen = GetTextSize(GTL_DEFAULT);
    GETTEXTEX gt;
    suic::String strText;

    gt.flags = GT_DEFAULT;
    gt.cb = sizeof(TCHAR) * (lLen + 1) ;
    gt.codepage = 1200;

    strText.Resize(lLen + 1);

    gt.lpDefaultChar = NULL;
    gt.lpUsedDefChar = NULL;
    TXSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)strText.c_str());

    return strText;
}

void TextContainer::SetText(const suic::String& strText)
{
    if (_richTxt)
    {
        Select(0, -1);
        ReplaceSel(strText, false);
    }
}

suic::String TextContainer::GetTextRange(long nStartChar, long nEndChar) const
{
    suic::String strText;
    TEXTRANGEW tr = {0};
    tr.chrg.cpMin = nStartChar;
    tr.chrg.cpMax = nEndChar;

    strText.Resize(nEndChar - nStartChar + 1);
    tr.lpstrText = strText.c_str();
    TXSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr);

    return strText;
}

int TextContainer::GetSelLength() const
{
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;
    TXSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
    return (int)(cr.cpMax - cr.cpMin);
}

suic::String TextContainer::GetSelText() const
{
    suic::String strText;
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;

    TXSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
    strText.Resize(cr.cpMax - cr.cpMin + 1);
    TXSendMessage(EM_GETSELTEXT, 0, (LPARAM)strText.c_str());

    return strText;
}

int TextContainer::InsertText(long index, const suic::String& strText)
{
    int nRet = Select(index, index);
    ReplaceSel(strText, true);
    return nRet;
}

int TextContainer::InsertText(long index, const suic::String& strText, const CharFormat& cf)
{
    int nRet = Select(index, index);
    ReplaceSel(strText, true);
    return nRet;
}

int TextContainer::AddText(const suic::String& strText)
{
    int nRet = Select(GetTextSize(0), GetTextSize(0));
    ReplaceSel(strText, true);
    return nRet;
}

int TextContainer::AddText(const suic::String& strText, const CharFormat& cf)
{
    CHARFORMAT2 cf2 = GetTextHost()->_cf;

    SetSelTextFormat(cf);
    int iRet = AddText(strText);

    GetTextHost()->_cf = cf2;
    TXSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2);

    return iRet;
}

void TextContainer::ApplyTextFormat(long lStart, long lEnd, const CharFormat& cf)
{
    Select(lStart, lEnd);
    SetSelTextFormat(cf);
    SelectNone();
}

void TextContainer::SetTextColor(suic::Color dwTextColor)
{
    if (GetTextHost()) 
    {
        GetTextHost()->SetColor(dwTextColor);
    }
}

void TextContainer::GetTextFormat(CharFormat& cf) const
{
    CHARFORMAT2 cf2;
    cf2.cbSize = sizeof(CHARFORMAT2);
    TXSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf2);
    GetCharFormat(cf2, cf);
}

void TextContainer::SetTextFormat(const CharFormat& cf)
{
    CHARFORMAT2& cf2 = GetTextHost()->_cf;
    SetCharFormat2(cf2, cf);
    TXSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf2);
}

void TextContainer::GetSelTextFormat(CharFormat& cf) const
{
    CHARFORMAT2 cf2;
    cf2.cbSize = sizeof(CHARFORMAT2);
    TXSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf2);
    GetCharFormat(cf2, cf);
}

void TextContainer::SetSelTextFormat(const CharFormat& cf)
{
    CHARFORMAT2 cf2;
    cf2.cbSize = sizeof(CHARFORMAT2);
    TXSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf2);
    SetCharFormat2(cf2, cf);
    TXSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2);
}

void TextContainer::SetLeftIndent(int indent)
{
    PARAFORMAT2& pf2 = GetTextHost()->_pf;
    pf2.dwMask = PFM_STARTINDENT;
    pf2.dxStartIndent = indent;
    TXSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf2);
}

void TextContainer::AddImage(const suic::String& filepath)
{
    if (GetTextHost())
    {
        GetTextHost()->AddImage(filepath.c_str());
    }
}

bool TextContainer::PointInObject(suic::Point pt)
{
    int cp = CharIndexFromPos(pt);
    REOBJECT reobj = {0};
    reobj.cp = cp;
    //reobj.dwFlags = REO_GETOBJ_POLEOBJ;//REO_GETOBJ_PSTG|REO_GETOBJ_POLESITE
    reobj.cbStruct = sizeof(REOBJECT);
    // REO_IOB_SELECTION使用选择第一个cp；REO_IOB_USE_CP使用用户指定的cp
    HRESULT hr = GetTextHost()->GetRichEditOle()->GetObject(REO_IOB_USE_CP, &reobj, REO_GETOBJ_POLEOBJ);
    OleImage* oleImg(dynamic_cast<OleImage*>(reobj.poleobj));

    if (reobj.poleobj != NULL)
    {
        reobj.poleobj->Release();
        if (oleImg != NULL)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool TextContainer::Redo()
{ 
    LRESULT lResult = TXSendMessage(EM_REDO, 0, 0);
    return lResult != 0; 
}

bool TextContainer::Undo()
{ 
    LRESULT lResult = TXSendMessageRet(EM_UNDO, 0, 0);
    return lResult != 0;
}

void TextContainer::Clear()
{ 
    TXSendMessage(WM_CLEAR, 0, 0); 
}

void TextContainer::ClearUndoCache()
{ 
    TXSendMessage(EM_EMPTYUNDOBUFFER, 0, 0); 
}

int TextContainer::SetUndoLimit(int limit)
{ 
    LRESULT lResult = TXSendMessage(EM_SETUNDOLIMIT, (WPARAM)limit, 0);
    return (int)lResult; 
}

void TextContainer::Copy()
{
    TXSendMessage(WM_COPY, 0, 0); 
}

void TextContainer::Cut()
{ 
    TXSendMessage(WM_CUT, 0, 0); 
}

void TextContainer::Paste()
{ 
    TXSendMessage(WM_PASTE, 0, 0); 
}

int TextContainer::GetLineCount() const
{ 
    LRESULT lResult = TXSendMessageRet(EM_GETLINECOUNT, 0, 0);
    return (int)lResult;
}

int TextContainer::GetLineLength(int line) const
{
    LRESULT lResult = TXSendMessageRet(EM_LINELENGTH, line, 0);
    return (int)lResult;
}

int TextContainer::LineFromCharIndex(int ch) const
{ 
    LRESULT lResult = TXSendMessageRet(EM_EXLINEFROMCHAR, 0, ch);
    return (long)lResult;
}

int TextContainer::CharIndexFromLine(int line) const
{
    LRESULT lResult = TXSendMessageRet(EM_LINEINDEX, line, 0);
    return (int)lResult;
}

suic::String TextContainer::GetLineText(int index, int iMaxSize) const
{
    if (iMaxSize <= 0)
    {
        iMaxSize = GetLineLength(index);
    }

    suic::String strText;
    if (iMaxSize > 0)
    {
        strText.Resize(iMaxSize + 1);
        TXSendMessage(EM_GETLINE, index, (LPARAM)strText.c_str());
    }

    return strText;
}

suic::Point TextContainer::PosFromCharIndex(int ch) const
{ 
    suic::Point pt;
    TXSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, ch); 
    return pt; 
}

int TextContainer::CharIndexFromPos(suic::Point pt) const
{ 
    POINTL ptl = {pt.x, pt.y}; 
    LRESULT lResult = TXSendMessageRet(EM_CHARFROMPOS, 0, (LPARAM)&ptl);
    return (int)lResult; 
}

bool TextContainer::IsReadOnly() const
{
    return Tobool(GetTextHost()->GetReadOnly());
}

suic::Rect TextContainer::GetClientRect() const
{
    suic::Point pt = PointToScreen(suic::Point());
    suic::Rect rcClient(0, 0, GetActualWidth(), GetActualHeight());
    rcClient.Offset(pt);
    return rcClient;
}

bool TextContainer::IsRichMode() const
{
    return Tobool(GetTextHost()->_isRichMode);
}

void TextContainer::SetReadOnly(bool val)
{
    GetTextHost()->SetReadOnly(val ? TRUE : FALSE);
}

void TextContainer::SetRichMode(bool val)
{
    GetTextHost()->SetRichTextFlag(val ? TRUE : FALSE);
}

bool TextContainer::IsWordWrap() const
{
    return Tobool(GetTextHost()->GetWordWrap());
}

void TextContainer::SetWordWrap(bool bWordWrap)
{
    GetTextHost()->SetWordWrap(ToBOOL(bWordWrap));
}

bool TextContainer::IsMultiLine() const
{
    return Tobool(GetTextHost()->HasStyle(ES_MULTILINE));
}

void TextContainer::SetMultiLine(bool bMulti)
{
    GetTextHost()->SetMultiLine(ToBOOL(bMulti));
}

bool TextContainer::IsPassword() const
{
    return Tobool(GetTextHost()->HasStyle(ES_PASSWORD));
}

void TextContainer::SetPassword(bool bPassword)
{
    GetTextHost()->ModifyStyle(ES_PASSWORD, ToBOOL(bPassword));
    GetTextHost()->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, TXTBIT_USEPASSWORD);
}

void TextContainer::SetPasswordChar(suic::Char ch)
{
    GetTextHost()->SetPasswordChar(ch);
}

bool TextContainer::IsModified() const
{ 
    LRESULT lResult = TXSendMessageRet(EM_GETMODIFY, 0, 0);
    return (BOOL)lResult == TRUE;
}

void TextContainer::SetModified(bool bModified) const
{ 
    TXSendMessage(EM_SETMODIFY, bModified, 0);
}

bool TextContainer::IsAutoURLDetect() const
{
    LRESULT lResult = TXSendMessage(EM_GETAUTOURLDETECT, 0, 0);
    return (BOOL)lResult == TRUE;
}

bool TextContainer::SetAutoURLDetect(bool bAutoDetect)
{
    LRESULT lResult = TXSendMessage(EM_AUTOURLDETECT, bAutoDetect, 0);
    return (BOOL)lResult == FALSE;
}

bool TextContainer::SetZoom(int nNum, int nDen)
{
    if (nNum < 0 || nNum > 64) 
    {
        return false;
    }

    if (nDen < 0 || nDen > 64) 
    {
        return false;
    }

    LRESULT lResult = TXSendMessage(EM_SETZOOM, nNum, nDen);
    return (BOOL)lResult == TRUE;
}

bool TextContainer::SetZoomOff()
{
    LRESULT lResult = TXSendMessage(EM_SETZOOM, 0, 0);
    return (BOOL)lResult == TRUE;
}

int TextContainer::SelectAll()
{
    int iRet = Select(0, -1);
    return iRet;
}

int TextContainer::SelectNone()
{
    return Select(-1, 0);
}

void TextContainer::HideSelection(bool bHide, bool bChangeStyle)
{
    TXSendMessage(EM_HIDESELECTION, bHide, bChangeStyle);
}

//--------------------------------- 滚动操作
void TextContainer::ScrollToCaret()
{
    TXSendMessage(EM_SCROLLCARET, 0, 0);
}

void TextContainer::ScrollLineUp()
{
    TXSendMessage(WM_VSCROLL, SB_LINEUP, 0L);
}

void TextContainer::ScrollLineDown()
{
    TXSendMessage(WM_VSCROLL, SB_LINEDOWN, 0L);
}

void TextContainer::ScrollLineLeft()
{
    TXSendMessage(WM_HSCROLL, SB_LINELEFT, 0L);
}

void TextContainer::ScrollLineRight()
{
    TXSendMessage(WM_HSCROLL, SB_LINERIGHT, 0L);
}

void TextContainer::ScrollPageUp()
{
    TXSendMessage(WM_VSCROLL, SB_PAGEUP, 0L);
}

void TextContainer::ScrollPageDown()
{
    TXSendMessage(WM_VSCROLL, SB_PAGEDOWN, 0L);
}

void TextContainer::ScrollPageLeft()
{
    TXSendMessage(WM_HSCROLL, SB_PAGELEFT, 0L);
}

void TextContainer::ScrollPageRight()
{
    TXSendMessage(WM_HSCROLL, SB_PAGERIGHT, 0L);
}

void TextContainer::ScrollToHomeUp()
{
    TXSendMessage(WM_VSCROLL, SB_TOP, 0L);
}

void TextContainer::ScrollToEndDown()
{
    TXSendMessage(WM_VSCROLL, SB_BOTTOM, 0L);
    //InvalidateVisual();
}

void TextContainer::ScrollToHomeLeft()
{
    TXSendMessage(WM_HSCROLL, SB_LEFT, 0L);
}

void TextContainer::ScrollToEndRight()
{
    TXSendMessage(WM_HSCROLL, SB_RIGHT, 0L);
}

void TextContainer::ScrollToVPos(int iPos)
{
    WPARAM wp = MAKEWPARAM(SB_THUMBPOSITION, iPos);
    TXSendMessage(WM_VSCROLL, wp, 0L);
}

void TextContainer::ScrollToHPos(int iPos)
{
    WPARAM wp = MAKEWPARAM(SB_THUMBPOSITION, iPos);
    TXSendMessage(WM_HSCROLL, wp, 0L);
}

void TextContainer::ClearAndFill(TextContainer* Other)
{
    SelectAll();
    Cut();
}

}
