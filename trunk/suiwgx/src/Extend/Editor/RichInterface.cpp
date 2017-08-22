
#include "stdafx.h"
#include <Extend/Editor/RichInterface.h>
#include <System/Windows/SystemParameters.h>

long UI_FPPTS_TO_TWIPS(float x)
{
    return (20 * x + ((x >= 0) ? 0.5 : -0.5));
}

LONG UILXtoDX(LONG x)	
{
    return ((x * suic::SystemParameters::DpiX) + LX_PER_INCH / 2) / LX_PER_INCH;
}

LONG UIDXtoLX(LONG x)	
{
    return ((x * LY_PER_INCH) * 2 + suic::SystemParameters::DpiX) / (2 * suic::SystemParameters::DpiX);
}

namespace suic
{

TextFont* TextDocOp::GetTextFont()
{
    return GetTextHost()->GetTextFont();
}

TextPara* TextDocOp::GetTextPara()
{
    return GetTextHost()->GetTextPara();
}

TextRenderHost* TextDocOp::GetTextHost() const
{
    return _textHost;
}

ITextDocument* TextDocOp::GetTextDoc()
{
    return GetTextHost()->GetTextDocument();
}

void TextDocOp::Clear()
{
    if (!IsReadOnly())
    {
        SelectAll();
        GetTextHost()->TxSendMessage(WM_CLEAR, 0, 0);
    }
}

void TextDocOp::GetStream(suic::ISStream* stm, bool atSel)
{
    DWORD dwFlag = atSel ? SFF_SELECTION : 0;
    EDITSTREAM stmCb;
    stmCb.dwCookie = (DWORD_PTR)stm;
    stmCb.dwError = 0;
    stmCb.pfnCallback = RichDocStreamOutCb;

    dwFlag |= SF_RTF|SF_UNICODE;

    GetTextHost()->TxSendMessage(EM_STREAMOUT, dwFlag, (LPARAM)&stmCb);
}

void TextDocOp::SetStream(const suic::ISStream* stm, bool atSel)
{
    DWORD dwFlag = atSel ? SFF_SELECTION : 0;
    EDITSTREAM stmCb;
    stmCb.dwCookie = (DWORD_PTR)stm;
    stmCb.dwError = 0;
    stmCb.pfnCallback = RichDocStreamInCb;
    dwFlag |= SF_RTF|SF_UNICODE;

    GetTextHost()->TxSendMessage(EM_STREAMIN, dwFlag, (LPARAM)&stmCb);
}

bool TextDocOp::Redo()
{
    LRESULT lResult = TxSendMessage(EM_REDO, 0, 0);
    return lResult != 0; 
}

bool TextDocOp::Undo()
{
    LRESULT lResult = TxSendMessage(EM_UNDO, 0, 0);
    return lResult != 0;
}

void TextDocOp::Copy()
{
    TxSendMessage(WM_COPY, 0, 0); 
}

void TextDocOp::Cut()
{
    TxSendMessage(WM_CUT, 0, 0); 
}

void TextDocOp::Paste()
{
    TxSendMessage(WM_PASTE, 0, 0); 
}

bool TextDocOp::IsWordWrapping()
{
    return _textHost->GetWordWrap();
}

void TextDocOp::SetWordWrapping(bool val)
{
    _textHost->SetWordWrap(val);
}

bool TextDocOp::IsReadOnly()
{
    return _textHost->GetReadOnly();
}

void TextDocOp::SetReadOnly(bool val)
{
    _textHost->SetReadOnly(val);
}

long TextDocOp::GetCharCount() const
{
    return GetTextHost()->GetTextLength();
}

suic::String TextDocOp::GetText() const
{
    long lLen = GetCharCount();
    GETTEXTEX gt;
    suic::String strText;

    gt.flags = GT_DEFAULT;
    gt.cb = (lLen + 1) * sizeof(suic::Char);
    gt.codepage = 1200;

    strText.Resize(lLen);

    gt.lpDefaultChar = NULL;
    gt.lpUsedDefChar = NULL;
    TxSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)strText.c_str());

    return strText;
}

void TextDocOp::AddText(suic::String strText, bool bCanUndo)
{
    ReplaceSel(strText, bCanUndo);
}

int TextDocOp::GetSelLength() const
{
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
    return (int)(cr.cpMax - cr.cpMin);
}

suic::String TextDocOp::GetSelText() const
{
    suic::String strText;
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;

    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
    strText.Resize(cr.cpMax - cr.cpMin + 1);
    TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)strText.c_str());

    return strText;
}

void TextDocOp::SetText(suic::String strText, bool bCanUndo)
{
    SelectAll();
    ReplaceSel(strText, bCanUndo);
}

suic::String TextDocOp::GetLineText(int index) const
{
    return GetTextHost()->GetLineText(index);
}

int TextDocOp::GetLineCount() const
{
    return GetTextHost()->GetLineCount();
}

int TextDocOp::GetCharIndexOfLineStart(int line) const
{
    LRESULT lResult = TxSendMessage(EM_LINEINDEX, line, 0);
    return (int)lResult;
}

int TextDocOp::GetLineLength(int line) const
{
    return GetTextHost()->GetLineLength(line);
}

int TextDocOp::Select(long lStart, long lCount)
{
    CHARRANGE cr;
    cr.cpMin = lStart;

    if (lCount < 0)
    {
        cr.cpMax = GetCharCount();
    }
    else if (lCount >= 0 && lStart >= 0)
    {
        cr.cpMax = lStart + lCount;
    }
    else
    {
        return -1;
    }

    LRESULT lResult = TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr);
    return (int)lResult;
}

int TextDocOp::SelectAll()
{
    int iRet = Select(0, GetCharCount());
    return iRet;
}

void TextDocOp::SelectStart()
{
    Select(0, 0);
}

void TextDocOp::SelectEnd()
{
    int iSize = GetCharCount();
    Select(iSize, iSize + 1);
}

void TextDocOp::ReplaceSel(const suic::String& strText, bool bCanUndo)
{
    TxSendMessage(EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)strText.c_str());
}

int TextDocOp::LineFromCharIndex(int ch) const
{ 
    LRESULT lResult = TxSendMessage(EM_EXLINEFROMCHAR, 0, ch);
    return (long)lResult;
}

int TextDocOp::CharIndexFromLine(int line) const
{
    LRESULT lResult = TxSendMessage(EM_LINEINDEX, line, 0);
    return (int)lResult;
}

suic::Point TextDocOp::PosFromCharIndex(int ch) const
{ 
    suic::Point pt;
    TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, ch); 
    return pt; 
}

int TextDocOp::CharIndexFromPos(suic::Point pt) const
{
    /*suic::Rect rect = GetTextHost()->GetClntRect();
    rect.Deflate(GetTextHost()->GetInsetRect());*/
    POINTL ptl = {pt.x, pt.y}; 
    LRESULT lResult = TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl);
    return (int)lResult; 
}

void TextDocOp::RecalcView(bool bForce)
{
    GetTextHost()->GetTextService()->TxRecalcView(bForce ? TRUE : FALSE);
}

void TextDocOp::GetLineDetail(int index, LineDetail* info, int* iYPos)
{
    GetTextHost()->GetLineDetail(index, info, iYPos);
}

suic::Point TextDocOp::GetPoint(suic::Uint32 cp, bool bLeftTop)
{
    return GetTextHost()->GetPoint(cp, bLeftTop);
}

int TextDocOp::GetFirstVisibleCharIndex() const
{
    suic::Rect rect = GetTextHost()->GetViewRect();
    rect.Deflate(GetTextHost()->GetPadding());
    POINTL ptl = {rect.left, rect.top}; 

    return (int)TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl);
}

int TextDocOp::GetLastVisibleCharIndex() const
{ 
    suic::Rect rect = GetTextHost()->GetViewRect();
    rect.Deflate(GetTextHost()->GetPadding());
    //rect.Offset(-rect.left, -rect.top);
    POINTL ptl = {rect.right, rect.bottom}; 

    return (int)TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl);
}

int TextDocOp::GetFirstVisibleLine() const
{ 
    LRESULT lResult = TxSendMessage(EM_GETFIRSTVISIBLELINE, 0, 0);
    return (int)lResult; 
}

bool TextDocOp::SetZoom(int nNum, int nDen)
{
    if (nNum < 0 || nNum > 64) 
    {
        return false;
    }

    if (nDen < 0 || nDen > 64) 
    {
        return false;
    }

    LRESULT lResult = TxSendMessage(EM_SETZOOM, nNum, nDen);
    return (BOOL)lResult == TRUE;
}

bool TextDocOp::SetZoomOff()
{
    LRESULT lResult = TxSendMessage(EM_SETZOOM, 0, 0);
    return (BOOL)lResult == TRUE;
}

LRESULT TextDocOp::TxSendMessage(UINT message, WPARAM wp, LPARAM lp) const
{
    LRESULT lr = 0;
    GetTextHost()->GetTextService()->TxSendMessage(message, wp, lp, &lr);
    return lr;
}

void TextDocOp::OnTxPropertyBitsChange(UINT key, UINT val) const
{
    if (GetTextHost())
    {
        GetTextHost()->GetTextService()->OnTxPropertyBitsChange(key, val);
    }
}

extern "C" DWORD CALLBACK RichDocStreamInCb(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
    suic::ISStream* bstm = (suic::ISStream*)dwCookie;

    if (pcb != NULL)
    {
        *pcb = bstm->Read(pbBuff, cb);
    }
    else
    {
        bstm->Read(pbBuff, cb);
    }

    return 0;
}

extern "C" DWORD CALLBACK RichDocStreamOutCb(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
    suic::ISStream* bstm = (suic::ISStream*)dwCookie;

    if (pcb != NULL)
    {
        *pcb = bstm->Write(pbBuff, cb);
    }
    else
    {
        bstm->Write(pbBuff, cb);
    }

    return 0;
}

}
