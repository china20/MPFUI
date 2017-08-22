// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////////
// TextHost20.cpp

#include "stdafx.h"

#include "TextHost20.h"
#include <TOM.h>

#include <Common/SSE.h>

#include <System/Graphics/Bitmap.h>
#include <System/Tools/HwndHelper.h>
#include <System/Tools/CoreHelper.h>

#pragma comment(lib,"riched20.lib")
#pragma comment(lib,"imm32.lib")

#define TIMER_INVALIDATE 30

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

// guid definitions here.  Make sure they don't change!

////8d33f740-cf58-11ce-a89d-00aa006cadc5
EXTERN_C const IID IID_ITextServices = {
    0x8d33f740,
    0xcf58,
    0x11ce,
    {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

// c5bdd8d0-d26e-11ce-a89e-00aa006cadc5
EXTERN_C const GUID IID_ITextHost = 
{ 0x13e670f4, 0x1a5a, 0x11cf, { 0xab, 0xeb, 0x0, 0xaa, 0x0, 0xb6, 0x5e, 0xa1 } };

//
//// {13E670F5-1A5A-11cf-ABEB-00AA00B65EA1}
//EXTERN_C const GUID IID_ITextHost2 = 
//{ 0x13e670f5, 0x1a5a, 0x11cf, { 0xab, 0xeb, 0x0, 0xaa, 0x0, 0xb6, 0x5e, 0xa1 } };
//
//EXTERN_C const GUID IID_IImageOle = 
//{ 0x13e671f4, 0x1a5a, 0x12cf, { 0xab, 0xdb, 0x0, 0xfa, 0x0, 0xe6, 0x5e, 0xa8 } };
//
#define DEFINE_GUIDXXX(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID CDECL name \
    = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

DEFINE_GUIDXXX(IID_ITextDocument,0x8CC497C0,0xA1DF,0x11CE,0x80,0x98,0x00,0xAA,0x00,0x47,0xBE,0x5D);

LONG DXtoHimetricX(LONG dx, LONG xPerInch)
{
    return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

LONG DYtoHimetricY(LONG dy, LONG yPerInch)
{
    return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}

LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch)
{
    return (LONG) MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
}

LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch)
{
    return (LONG) MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
}

void InternalInitCharFormat(DWORD dwColor, CHARFORMAT2W* pcf, HFONT hfont)
{
    memset(pcf, 0, sizeof(CHARFORMAT2W));
    LOGFONT lf;

    if (!hfont)
    {
        hfont = (HFONT)(DWORD_PTR)suic::GetUIFont();
    }

    ::GetObject(hfont, sizeof(LOGFONT), &lf);

    pcf->cbSize = sizeof(CHARFORMAT2W);
    pcf->crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
    LONG yPixPerInch = SystemParameters::LogPixelsy;
    pcf->yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
    pcf->yOffset = 0;
    pcf->dwEffects = 0;
    pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
    if (lf.lfWeight >= FW_BOLD)
    {
        pcf->dwEffects |= CFE_BOLD;
    }

    if (lf.lfItalic)
    {
        pcf->dwEffects |= CFE_ITALIC;
    }

    if (lf.lfUnderline)
    {
        pcf->dwEffects |= CFE_UNDERLINE;
    }

    pcf->bCharSet = lf.lfCharSet;
    pcf->bPitchAndFamily = lf.lfPitchAndFamily;
    _tcscpy(pcf->szFaceName, lf.lfFaceName);
}

void InternalInitParaFormat(PARAFORMAT2* ppf) 
{	
    memset(ppf, 0, sizeof(PARAFORMAT2));
    ppf->cbSize = sizeof(PARAFORMAT2);
    ppf->dwMask = PFM_ALL;
    ppf->wAlignment = PFA_LEFT;
    ppf->cTabCount = 1;
    ppf->rgxTabs[0] = lDefaultTab;
}

TextRenderScope::TextRenderScope()
    : _textSrv(NULL)
    , _textDoc(NULL)
    , _holder(NULL)
    , _lMaxLines(10000)
    , _isScrollPending(false)
{
    ::ZeroMemory(&_refCount, sizeof(TextRenderScope) - offsetof(TextRenderScope, _refCount));

     _refCount = 1;
    _lMaxLength = 16 * 1024 * 1024;
    _laccelpos = -1;

    _textColor = Colors::Black;
    _selTextColor = Color::FromRgb(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    _selBackground = Color::FromRgb(::GetSysColor(COLOR_HIGHLIGHT));

    SetInsetRect(suic::Rect(2, 2));
    _caret.setAutoDelete(false);

    SetName(_U("PART_ScrollHost"));

    SetWordWrap(FALSE);
    SetMultiLine(TRUE);
    SetStyle(ES_NOHIDESEL | WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL/* | WS_EX_TRANSPARENT*/);
}

TextRenderScope::~TextRenderScope(void)
{
    RevokeDragDrop();
    _textSrv->Release();
    _textDoc->Release();
}

//=========================================================================

void TextRenderScope::ModifyStyle(DWORD dwStyle, BOOL bAdd)
{
    if (bAdd)
    {
        _dwStyle |= dwStyle;
    }
    else
    {
        _dwStyle &= ~dwStyle;
    }
}

void TextRenderScope::SetStyle(DWORD dwStyle)
{
    _dwStyle = dwStyle;
}

BOOL TextRenderScope::HasStyle(DWORD dwStyle) const
{
    return ((_dwStyle & dwStyle) == dwStyle);
}

bool TextRenderScope::GetOleImageRect(const REOBJECT& reo, IRichEditOle *pRichEditOle, RECT* lpRect)
{
    HRESULT hr = S_OK;
    OleImage* oleimg = dynamic_cast<OleImage*>(reo.poleobj);
    suic::Rect drRect(oleimg->GetDrawRect());

    ITextDocument *pTextDocument = NULL;
    ITextRange *pTextRange = NULL;
    pRichEditOle->QueryInterface(IID_ITextDocument, (void **)&pTextDocument);

    if (!pTextDocument)
    {
        return false;
    }

    long nLeft = 0;
    long nBottom = 0;
    pTextDocument->Range(reo.cp, reo.cp, &pTextRange);

    if (reo.dwFlags & REO_BELOWBASELINE)
    {
        hr = pTextRange->GetPoint(TA_BOTTOM|TA_LEFT, &nLeft, &nBottom);
    }
    else
    {
        hr = pTextRange->GetPoint(TA_BASELINE|TA_LEFT, &nLeft, &nBottom);
    }

    pTextDocument->Release();
    pTextRange->Release();

    RECT rectWindow;
    ::GetWindowRect(GetHostHwnd(), &rectWindow);

    lpRect->left = nLeft - rectWindow.left;
    lpRect->bottom = nBottom - rectWindow.top;
    lpRect->right = lpRect->left + drRect.Width() + 2;
    lpRect->top = lpRect->bottom - drRect.Height() - 2;

    return true;
}

void TextRenderScope::OnObjectTimer()
{
    IRichEditOle *pRichEditOle = NULL;
    GetTextService()->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle, 0);

    if (pRichEditOle == NULL)
    {
        return;
    }

    int nObjCount = pRichEditOle->GetObjectCount();
    int i = 0;
    int iLineCount = 0;//_richbox->GetLineCount() - 1;
    bool bUpdate = false;

    for (i = 0;i < nObjCount; i++)
    {
        suic::Rect rect;
        REOBJECT reo;
        ZeroMemory(&reo, sizeof(REOBJECT));
        reo.cbStruct = sizeof(REOBJECT);

        HRESULT hr = pRichEditOle->GetObject(i, &reo, REO_GETOBJ_POLEOBJ);
        OleImage* oleimg = dynamic_cast<OleImage*>(reo.poleobj);

        // 获得当前字符所在行号
        int iLineIndex = 0;//_richbox->LineFromCharIndex(reo.cp);
       
        suic::Rect drRect(oleimg->GetDrawRect());

        if (iLineCount == iLineIndex)
        {
            GetOleImageRect(reo, pRichEditOle, &rect);
        }
        else
        {
            // 获取当前行第一个字符索引
            int iCharIndex = 0;//_richbox->CharIndexFromLine(iLineIndex + 1);
            suic::Point curPos;//_richbox->PosFromCharIndex(reo.cp);
            suic::Point nextPos;//_richbox->PosFromCharIndex(iCharIndex);

            rect.left = curPos.x;
            rect.top = nextPos.y - drRect.Height();
            rect.right = rect.left + drRect.Width();
            rect.bottom = nextPos.y;
        }
        
        if (reo.poleobj)
        {
            reo.poleobj->Release();
        }
        if (reo.pstg)
        {
            reo.pstg->Release();
        }
        if (reo.polesite)
        {
            reo.polesite->Release();
        }

        if (rect.Intersect(&_rcClnt))
        {
            if (oleimg->CanAnimate() && oleimg->CheckElapse(TIMER_INVALIDATE))
            {
                oleimg->MoveNextFrame();
                //_richbox->InvalidateRect(&rect, false);
                bUpdate = true;                
            }
        }
    }

    pRichEditOle->Release();
}

void TextRenderScope::AddImage(LPCTSTR filepath)
{
    if (filepath != NULL)
    {
        IRichEditOle* richeditole = GetRichEditOle();
        if (richeditole)
        {
            /*ImageDataObj ido;
            ido.InsertImage(richeditole, filepath, dwUserData);*/
            OleImage::AddImage(richeditole, filepath, 0);
            richeditole->Release();
        }
    }
}

bool TextRenderScope::InsertEmbbedObj(IOleObject* embbed, DWORD dwUserData)
{
    SCODE sc;
    IOleClientSite *pOleClientSite = NULL;
    IRichEditOle* pRichEditOle = GetRichEditOle();

    pRichEditOle->GetClientSite(&pOleClientSite);
    IStorage *pStorage = NULL;
    LPLOCKBYTES lpLockBytes = NULL;
    HRESULT hr = S_OK;

    UINT flag = STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE;
    IOleObject *pOleObject = embbed;

    sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);

    if (sc != S_OK)
    {
        return false;
    }

    /*
    hr = _pstg->CreateStream(szSiteFlagsStm, STGM_READWRITE |
    STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0, 0, &pstm );
    */

    sc = ::StgCreateDocfileOnILockBytes(lpLockBytes, flag, 0, &pStorage);

    if (sc != S_OK)
    {
        lpLockBytes->Release();
        lpLockBytes = NULL;
        return false;
    }

    pOleObject->SetClientSite(pOleClientSite);

    hr = ::OleSetContainedObject(pOleObject, TRUE);

    REOBJECT reobject;
    ZeroMemory(&reobject, sizeof(REOBJECT));
    reobject.cbStruct = sizeof(REOBJECT);

    //reobject.clsid = IID_IImageOle;
    reobject.cp = REO_CP_SELECTION;
    reobject.dvaspect = DVASPECT_CONTENT;
    reobject.poleobj = pOleObject;
    reobject.polesite = pOleClientSite;
    reobject.pstg = pStorage;
    reobject.dwUser = dwUserData;
    reobject.dwFlags = REO_BELOWBASELINE;

    pRichEditOle->InsertObject(&reobject);

    pOleObject->Release();
    pOleClientSite->Release();

    return true;
}

IRichEditOle* TextRenderScope::GetRichEditOle()
{
    IRichEditOle* richeditole = NULL;
    _textSrv->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&richeditole, 0);
    return richeditole;
}

bool TextRenderScope::SetRichEditOleCallback(IRichEditOleCallback* callback)
{
    LRESULT lr = TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)callback);
    return (lr != 0);
}

void TextRenderScope::ShowVertScrollBar(BOOL val)
{
    ModifyStyle(WS_VSCROLL, val);
}

BOOL TextRenderScope::IsShowVertScrollBar() const
{
    return HasStyle(WS_VSCROLL);
}

void TextRenderScope::ShowHoriScrollBar(BOOL val)
{
    ModifyStyle(WS_HSCROLL, val);
}

BOOL TextRenderScope::IsShowHoriScrollBar() const
{
    return HasStyle(WS_HSCROLL);
}

void TextRenderScope::SetAutoHoriScrollBar(BOOL val)
{
    ModifyStyle(ES_AUTOHSCROLL, val);
}

BOOL TextRenderScope::IsAutoHoriScrollBar() const
{
    return HasStyle(ES_AUTOHSCROLL);
}

void TextRenderScope::SetAutoVertScrollBar(BOOL val)
{
     ModifyStyle(ES_AUTOVSCROLL, val);
}

BOOL TextRenderScope::IsAutoVertScrollBar() const
{
    return HasStyle(ES_AUTOVSCROLL);
}

void TextRenderScope::SetTextMode(int iMode)
{
    TxSendMessage(EM_SETTEXTMODE, iMode, 0);
}

int TextRenderScope::GetTextMode() const
{
    return 0;
}

void TextRenderScope::OnTxPropertyBitsChange(UINT key, UINT val)
{
    if (_textSrv)
    {
        _textSrv->OnTxPropertyBitsChange(key, val);
    }
}

DWORD TextRenderScope::GetEventMask() const
{
    LRESULT lResult = 0;
    _textSrv->TxSendMessage(EM_GETEVENTMASK, 0, 0, &lResult);
    return (DWORD)lResult;
}

DWORD TextRenderScope::SetEventMask(DWORD dwEventMask)
{
    LRESULT lResult = TxSendMessage(EM_SETEVENTMASK, 0, dwEventMask);
    return (DWORD)lResult;
}

int TextRenderScope::GetLineIndexFromCharIndex(int charIndex) const
{
    return (int)TxSendMessage(EM_EXLINEFROMCHAR, 0, charIndex);
}

int TextRenderScope::GetCharacterIndexFromLineIndex(int lineIndex) const
{
    return (int)TxSendMessage(EM_LINEINDEX, lineIndex, 0);
}

int TextRenderScope::GetCharIndexFromPoint(Point point, bool snapToText) const
{
    POINTL ptl = {point.x, point.y}; 
    return (int)TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl);
}

Point TextRenderScope::GetPointFromCharIndex(int charIndex) const
{ 
    Point pt;
    TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, charIndex); 
    return pt; 
}

int TextRenderScope::GetFirstVisibleLineIndex() const
{
    return (int)TxSendMessage(EM_GETFIRSTVISIBLELINE, 0, 0);
}

int TextRenderScope::GetLastVisibleLineIndex() const
{
    suic::Rect rect = GetClientRect();
    rect.Deflate(GetInsetRect());
    POINTL ptl = {rect.right, rect.bottom}; 

    int iLastVisibleCp = (int)TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl);
    return GetLineIndexFromCharIndex(iLastVisibleCp); 
}

int TextRenderScope::GetLineCount() const
{
    int lineCount = (int)TxSendMessage(EM_GETLINECOUNT, 0, 0);
    if (1 == lineCount && GetLineLength(0) == 0)
    {
        --lineCount;
    }
    return lineCount;
}

int TextRenderScope::GetLineLength(int lineIndex) const
{
    return (int)(TxSendMessage(EM_LINELENGTH, lineIndex, 0));
}

String TextRenderScope::GetLineText(int lineIndex) const
{
    int iLineSize = GetLineLength(lineIndex);
    suic::String strText;
    strText.Resize(iLineSize + 1);
    TxSendMessage(EM_GETLINE, lineIndex, (LPARAM)strText.c_str());

    return strText;
}

void TextRenderScope::SetWordWrap(bool fWordWrap)
{
    _isWordWrap = fWordWrap;
    OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
}

void TextRenderScope::SetMultiLine(bool fMultiLine)
{
    ModifyStyle(ES_MULTILINE, fMultiLine);
    OnTxPropertyBitsChange(TXTBIT_MULTILINE, fMultiLine ? TXTBIT_MULTILINE : 0);
}

bool TextRenderScope::GetReadOnly()
{
    return (_dwStyle & ES_READONLY) != 0;
}

void TextRenderScope::SetReadOnly(bool fReadOnly)
{
    if (fReadOnly)
    {
        _dwStyle |= ES_READONLY;
    }
    else
    {
        _dwStyle &= ~ES_READONLY;
    }

    OnTxPropertyBitsChange(TXTBIT_READONLY, fReadOnly ? TXTBIT_READONLY : 0);
}

bool TextRenderScope::GetAcceptsReturn()
{
    return _isAcceptsReturn != 0;
}

void TextRenderScope::SetAcceptsReturn(bool val)
{
    _isAcceptsReturn = val;
}

bool TextRenderScope::GetAutoWordSelection()
{
    return _isAutoWordSelection != 0;
}

void TextRenderScope::SetAutoWordSelection(bool val)
{
    _isAutoWordSelection = val;
}

bool TextRenderScope::GetAcceptsTab()
{
    return _isAcceptsTab != 0;
}

void TextRenderScope::SetAcceptsTab(bool val)
{
    _isAcceptsTab = val;
}

bool TextRenderScope::GetIsReadOnlyCaretVisible()
{
    return _isReadOnlyCaretVisible != 0;
}

void TextRenderScope::SetIsReadOnlyCaretVisible(bool val)
{
    _isReadOnlyCaretVisible = val;
}

void TextRenderScope::SetFont(HFONT hFont) 
{
    if (hFont == NULL) 
    {
        LOGFONT lf;
        LONG yPixPerInch = SystemParameters::LogPixelsy;

        ::GetObject(hFont, sizeof(LOGFONT), &lf);
        
        _cf.yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;

        if (lf.lfWeight >= FW_BOLD)
        {
            _cf.dwEffects |= CFE_BOLD;
        }

        if (lf.lfItalic)
        {
            _cf.dwEffects |= CFE_ITALIC;
        }

        if (lf.lfUnderline)
        {
            _cf.dwEffects |= CFE_UNDERLINE;
        }

        _cf.bCharSet = lf.lfCharSet;
        _cf.bPitchAndFamily = lf.lfPitchAndFamily;
        _tcscpy(_cf.szFaceName, lf.lfFaceName);

        OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
    }
}

void TextRenderScope::SetTextColor(Color clr)
{
    _textColor = clr;
    // 设置文本颜色
    _cf.crTextColor = Color::ToRgb(_textColor);
    OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
}

void TextRenderScope::SetFontFamily(String name)
{
    wcscpy_s(_cf.szFaceName, name.c_str());
    OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
}

void TextRenderScope::SetFontWeight(int weight)
{
    _cf.wWeight = weight;
    OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
}

void TextRenderScope::SetFontSize(int size)
{
    // 字体磅数转换为像素
    size = CoreHelper::FontPoundToPixel(size);
    _cf.yHeight = size * LY_PER_INCH / SystemParameters::LogPixelsy;

    OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
}

SIZEL TextRenderScope::GetExtent() 
{
    SIZEL sizelExtent;

    sizelExtent.cx = DXtoHimetricX(_rcClnt.Width(), SystemParameters::LogPixelsx);
    sizelExtent.cy = DYtoHimetricY(_rcClnt.Height(), SystemParameters::LogPixelsy);

    return sizelExtent;
}

void TextRenderScope::NotifyExtentChange() 
{
    OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
}

void TextRenderScope::NotifyAllChange() 
{
    DWORD dwMask = TXTBIT_RICHTEXT | ES_MULTILINE | ES_READONLY | ES_PASSWORD | ES_NOHIDESEL |
        TXTBIT_AUTOWORDSEL | TXTBIT_WORDWRAP | TXTBIT_SAVESELECTION;
    DWORD dwVal = 0;

    TxGetPropertyBits(dwMask, &dwVal);

    OnTxPropertyBitsChange(dwMask, dwVal);
}

void TextRenderScope::SetMaxLength(int nChars)
{
    _lMaxLength = nChars;
    if (_lMaxLength <= 0) 
    {
        _lMaxLength = 10240;
    }

    OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
}

int TextRenderScope::GetMaxLength() const
{
    return _lMaxLength;
}

void TextRenderScope::SetMaxLines(LONG val)
{
    _lMaxLines = max(1, val);

    if (_lMaxLines > 1)
    {
        SetMultiLine(true);
    }
    else
    {
        SetMultiLine(false);
    }
}

void TextRenderScope::SetMinLines(LONG val)
{
}

void TextRenderScope::SetUndoLimit(LONG val)
{
    val = max(0, val);
    TxSendMessage(EM_SETUNDOLIMIT, val, 0);
}

BOOL TextRenderScope::GetAllowBeep()
{
    return _isAllowBeep;
}

void TextRenderScope::SetAllowBeep(BOOL fAllowBeep)
{
    _isAllowBeep = fAllowBeep;

    OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
}

WORD TextRenderScope::GetDefaultAlign()
{
    return _pf.wAlignment;
}

void TextRenderScope::SetDefaultAlign(WORD wNewAlign)
{
    _pf.wAlignment = wNewAlign;
    OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

BOOL TextRenderScope::GetRichTextFlag()
{
    return _isRichMode;
}

void TextRenderScope::SetRichTextFlag(BOOL fNew)
{
    _isRichMode = fNew;
    OnTxPropertyBitsChange(TXTBIT_RICHTEXT, fNew ? TXTBIT_RICHTEXT : 0);
}

LONG TextRenderScope::GetDefaultLeftIndent()
{
    return _pf.dxOffset;
}

void TextRenderScope::SetDefaultLeftIndent(LONG lNewIndent)
{
    _pf.dxOffset = lNewIndent;
    OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

void TextRenderScope::SetInsetRect(suic::Rect prc) 
{
    _rcInset = prc;
    OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
}

void TextRenderScope::SetClientRect(suic::Rect rect) 
{
    _rcClnt = rect;
    OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
}

BOOL TextRenderScope::SetSaveSelection(BOOL isSaveSelection)
{
    BOOL fResult = _isSaveSelection;
    _isSaveSelection = isSaveSelection;
    OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, isSaveSelection ? TXTBIT_SAVESELECTION : 0);
    return fResult;		
}

void TextRenderScope::SetTransparent(BOOL isTransparent)
{
    _isTransparent = isTransparent;
    OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
}

LONG TextRenderScope::SetAccelPos(LONG laccelpos)
{
    LONG laccelposOld = _laccelpos;
    _laccelpos = laccelpos;
    OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);
    return laccelposOld;
}

WCHAR TextRenderScope::SetPasswordChar(WCHAR chPasswordChar)
{
    WCHAR chOldPasswordChar = _chPasswordChar;
    _chPasswordChar = chPasswordChar;

    OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, (chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);
    return chOldPasswordChar;
}

void TextRenderScope::SetDisabled(BOOL fOn)
{
    _cf.dwMask |= CFM_COLOR | CFM_DISABLED;
    _cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;
    if (!fOn)
    {
        _cf.dwEffects &= ~CFE_DISABLED;
    }
    OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
}

LONG TextRenderScope::SetSelBarWidth(LONG lSelBarWidth)
{
    if (lSelBarWidth)
    {
        _dwStyle |= ES_SELECTIONBAR;
    }
    else
    {
        _dwStyle &= (~ES_SELECTIONBAR);
    }

    OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

    return lSelBarWidth;
}

HRESULT	TextRenderScope::OnTxInPlaceDeactivate()
{
    HRESULT hr = _textSrv->OnTxInPlaceDeactivate();
    if (SUCCEEDED(hr))
    {
        _isInplaceActive = FALSE;
    }
    _caret.Hide();
    return hr;
}

HRESULT	TextRenderScope::OnTxInPlaceActivate(LPCRECT prcClient)
{
    _isInplaceActive = TRUE;
    HRESULT hr = _textSrv->OnTxInPlaceActivate(prcClient);
    if (FAILED(hr))
    {
        _isInplaceActive = FALSE;
    }
    return hr;
}

//=========================================================================
HRESULT _stdcall TextRenderScope::QueryInterface( REFIID riid, void **ppvObject )
{
    HRESULT hr = E_NOINTERFACE;
    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITextHost)
        /*|| IsEqualIID(riid, IID_ITextHost2)*/) 
    {
        AddRef();
        *ppvObject = (ITextHost *) this;
        hr = S_OK;
    }

    return hr;
}

ULONG _stdcall TextRenderScope::AddRef( void )
{
    return ++_refCount;
}

ULONG _stdcall TextRenderScope::Release( void )
{
    ULONG lRefs = --_refCount;

    if (lRefs == 0)
    {
        delete this;
    }

    return lRefs;
}


//////////////////////////////////////////////////////////////////////////
// ITextHost
HRESULT TextRenderScope::TxGetViewInset(LPRECT prc)
{
    prc->left = DXtoHimetricX(_rcInset.left, SystemParameters::LogPixelsx);
    prc->top = DYtoHimetricY(_rcInset.top, SystemParameters::LogPixelsy);
    prc->right = DXtoHimetricX(_rcInset.right, SystemParameters::LogPixelsx);
    prc->bottom = DYtoHimetricY(_rcInset.bottom, SystemParameters::LogPixelsy);

    return NOERROR;
}

HRESULT TextRenderScope::TxGetCharFormat(const CHARFORMATW **ppCF)
{
    *ppCF = &_cf;
    return NOERROR;
}

HRESULT TextRenderScope::TxGetParaFormat( const PARAFORMAT **ppPF )
{
    *ppPF = &_pf;
    return NOERROR;
}

HRESULT TextRenderScope::TxGetClientRect(LPRECT prc)
{
    *prc = _rcClnt;
    return NOERROR;
}

HRESULT TextRenderScope::TxDeactivate( LONG lNewState )
{
    return S_OK;
}

HRESULT TextRenderScope::TxActivate( LONG * plOldState )
{
    return S_OK;
}

BOOL TextRenderScope::TxClientToScreen(LPPOINT lppt)
{
    return ::ClientToScreen(GetHostHwnd(), lppt);
}

HWND TextRenderScope::GetHostHwnd()
{
    return HandleToHwnd(HwndHelper::GetHostHwnd(this));
}

BOOL TextRenderScope::TxScreenToClient( LPPOINT lppt )
{
    return ::ScreenToClient(GetHostHwnd(), lppt);
}

void TextRenderScope::TxSetCursor(HCURSOR hcur, BOOL fText)
{
    ::SetCursor(hcur);
}

void TextRenderScope::TxSetFocus()
{
    //Focus();
}

void TextRenderScope::TxSetCapture(BOOL fCapture)
{
    if (fCapture) 
    {
        SetCaptureMouse();
    }
    else
    {
        ReleaseCaptureMouse();
    }
}

void TextRenderScope::TxScrollWindowEx( INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll )
{
}

void TextRenderScope::TxKillTimer(UINT idTimer)
{
    ::KillTimer(GetHostHwnd(), idTimer);
}

bool TextRenderScope::TxOnSetTimer(UINT idTimer)
{
    return true;
}

BOOL TextRenderScope::TxSetTimer(UINT idTimer, UINT uTimeout)
{
    //int id = //_richbox->SetTimer(uTimeout, suic::TimerFunc(//_richbox, &ui::RichText20::OnTimeout));
    ////int id = (int)::SetTimer(GetHostHwnd(), idTimer, uTimeout, NULL);
    //if (idTimer != _animateid)
    //{
    //    _timerid = id;
    //}
    //else
    //{
    //    _animateid = id;
    //}
    return TRUE;
}

BOOL TextRenderScope::TxSetCaretPos(INT x, INT y)
{
    Point pt = PointToScreen(Point());

    pt.x = x - pt.x;
    pt.y = y - pt.y;

    _caret.SetPos(pt);
    _caret.InvalidateVisual();

    return TRUE;
}

BOOL TextRenderScope::TxShowCaret(BOOL fShow)
{
    //::ShowCaret(GetHostHwnd());
    
    _caret.UpdateArrange();

    if (fShow)
    {
        _caret.Show();
    }
    else
    {
        _caret.Hide();
    }

    return TRUE;
}

BOOL TextRenderScope::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
    _caret.SetWidth(xWidth);
    _caret.SetHeight(yHeight);
    _caret.SetSize(Size(xWidth, yHeight));
    //_richbox->CreateCaret(xWidth, yHeight);
    return TRUE;//CreateCaret(GetHostHwnd(), hbmp, xWidth, yHeight);
}

HDC TextRenderScope::TxGetDC()
{
    return GetDC(GetHostHwnd());
}

INT TextRenderScope::TxReleaseDC( HDC hdc )
{
    return ReleaseDC(GetHostHwnd(), hdc);
}

BOOL TextRenderScope::TxShowScrollBar(INT fnBar, BOOL fShow)
{
    if (GetScrollViewer() != NULL)
    {
        if (fnBar == SB_HORZ)
        {
            GetScrollViewer()->SetHorizontalScrollBarVisibility(fShow ? ScrollBarVisibility::AutoScroll : ScrollBarVisibility::Hidden);
        }
        else
        {
            GetScrollViewer()->SetVerticalScrollBarVisibility(fShow ? ScrollBarVisibility::AutoScroll : ScrollBarVisibility::Hidden);
        }

        GetScrollViewer()->UpdateLayout();
    }

    return TRUE;
}

BOOL TextRenderScope::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags)
{
    return TRUE;
}

BOOL TextRenderScope::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
    if (GetScrollViewer() != NULL)
    {
        ScrollData* scrData = _scrollInfo.GetScrollData();

        if (fnBar == SB_HORZ)
        {
            scrData->extent.cx = nMaxPos - nMinPos + _rcInset.SumLR();
        }
        else
        {
            scrData->extent.cy = nMaxPos - nMinPos + _rcInset.SumTB();
        }

        scrData->viewport = _rcClnt.ToSize();

        if (!_isScrollPending)
        {
            InvalidateScrollInfo(NULL);

            if (fRedraw)
            {
                GetScrollViewer()->InvalidateVisual();
            }
        }
    }

    return TRUE;
}

BOOL TextRenderScope::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
{
    if (GetScrollViewer() != NULL && !_isScrollPending)
    {
        if (fnBar == SB_HORZ)
        {
            GetScrollViewer()->SetHorizontalOffset(nPos);
        }
        else
        {
            GetScrollViewer()->SetVerticalOffset(nPos);
        }

        if (fRedraw)
        {
            GetScrollViewer()->UpdateLayout();
        }
    }

    return TRUE;
}

void TextRenderScope::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
    if (prc && !fMode)
    {
        if (prc->right > prc->left)
        {
            InvalidateVisual();
        }
    }
    else
    {
        InvalidateVisual();
    }
}

void TextRenderScope::TxViewChange(BOOL fUpdate)
{
    if (fUpdate)
    {
        InvalidateVisual();
    }
}

COLORREF TextRenderScope::TxGetSysColor(int nIndex)
{
    switch (nIndex)
    {
        // 高亮文本颜色
        case COLOR_HIGHLIGHTTEXT:
            return _selTextColor.ToRgb();

            // 高亮文本背景颜色
        case COLOR_HIGHLIGHT:
            return _selBackground.ToRgb();

            // 背景颜色
        case COLOR_WINDOW:
            break;
            // 正常文本颜色
        case COLOR_WINDOWTEXT:
            break;
            // 失去焦点文本颜色
        case COLOR_GRAYTEXT:
            break;
    }

    return ::GetSysColor(nIndex);
}

HRESULT TextRenderScope::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
    *pstyle = _isTransparent ? TXTBACK_TRANSPARENT: TXTBACK_OPAQUE;
    return S_OK;
}

HRESULT TextRenderScope::TxGetMaxLength(DWORD *plength)
{
    *plength = _lMaxLength;
    return S_OK;
}

HRESULT TextRenderScope::TxGetScrollBars(DWORD *pdwScrollBar)
{
    *pdwScrollBar = _dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

    return S_OK;
}

HRESULT TextRenderScope::TxGetPasswordChar(TCHAR *pch)
{
    *pch = _chPasswordChar;
    return S_OK;
}

HRESULT TextRenderScope::TxGetAcceleratorPos(LONG *pcp)
{
    *pcp = _laccelpos;
    return S_OK;
}

HRESULT TextRenderScope::TxGetExtent(LPSIZEL lpExtent)
{
    *lpExtent = GetExtent();
    return S_OK;
}

HRESULT TextRenderScope::OnTxCharFormatChange(const CHARFORMATW * pcf)
{
    return S_FALSE;
}

HRESULT TextRenderScope::OnTxParaFormatChange(const PARAFORMAT * ppf)
{
    return S_FALSE;
}

HRESULT TextRenderScope::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
{
    DWORD dwProperties = 0;

    if (_isRichMode)
    {
        dwProperties = TXTBIT_RICHTEXT;
    }

    if (_dwStyle & ES_MULTILINE)
    {
        dwProperties |= TXTBIT_MULTILINE;
    }

    if (_dwStyle & ES_READONLY)
    {
        dwProperties |= TXTBIT_READONLY;
    }

    if (_dwStyle & ES_PASSWORD)
    {
        dwProperties |= TXTBIT_USEPASSWORD;
    }

    if (!(_dwStyle & ES_NOHIDESEL))
    {
        dwProperties |= TXTBIT_HIDESELECTION;
    }

    if (_isEnableAutoWordSel)
    {
        dwProperties |= TXTBIT_AUTOWORDSEL;
    }

    if (_isWordWrap)
    {
        dwProperties |= TXTBIT_WORDWRAP;
    }

    if (_isAllowBeep)
    {
        dwProperties |= TXTBIT_ALLOWBEEP;
    }

    if (_isSaveSelection)
    {
        dwProperties |= TXTBIT_SAVESELECTION;
    }

    *pdwBits = dwProperties & dwMask; 

    return NOERROR;
}

HRESULT TextRenderScope::TxNotify(DWORD iNotify, void *pv)
{
    //return E_NOTIMPL;
    switch (iNotify)
    {
        // 达到最大长度
    case EN_MAXTEXT:
        break;
        // 选择变化，需要事件掩码ENM_SELCHANGE
    case EN_SELCHANGE:
        {
            SELCHANGE* s = (SELCHANGE*)pv;
            RoutedEventArg e(this, TextBoxBase::SelectionChangedEvent);

            if (_holder != NULL)
            {
                _holder->OnSelectionChanged(e);
            }
        }
        break;
        // 内存溢出
    case EN_ERRSPACE:
        break;
        // 输入变化，需要事件ENM_CHANGE掩码
    case EN_CHANGE:
        {
            CHANGENOTIFY* c = (CHANGENOTIFY*)pv;
            TextChangedArg e(0, 0, 0);

            if (_holder != NULL)
            {
                e.OnOverrideOriginalSource(this);
                e.SetRoutedEvent(TextBoxBase::TextChangedEvent);
                _holder->OnTextChanged(e);
            }
        }
        break;
        // 文件拖动，需要事件ENM_DROPFILES掩码
    case EN_DROPFILES:
        {
            ENDROPFILES* e = (ENDROPFILES*)pv;
        }
        break;
        // 超链接，dwEffects需要CFE_LINK
    case EN_LINK:
        {
            ENLINK* enlink = (ENLINK*)pv;
            int i = 0;
        }
        break;
        // 需要事件掩码ENM_REQUESTRESIZE
    case EN_REQUESTRESIZE:
        {
            /*REQRESIZE* p = (REQRESIZE*)pv;
            _scrollInfo.GetScrollData()->extent.cx = p->rc.right - p->rc.left;
            _scrollInfo.GetScrollData()->extent.cy = p->rc.bottom - p->rc.top;

            InvalidateScrollInfo(NULL);
            GetScrollViewer()->UpdateLayout();*/

            break;
    }
        // ole对象操作失败
    case EN_OLEOPFAILED:
        break;
        // 准备绘制
    case EN_UPDATE:
        break;
        // 失去焦点
    case EN_KILLFOCUS:
        break;
        // 数据保护, CFE_PROTECTED
    case EN_PROTECTED:
        {
            ENPROTECTED* enp = (ENPROTECTED*)pv;
        }
        return S_FALSE;
        break;
        // 消息过滤
    case EN_MSGFILTER:
        {
            MSGFILTER* mf = (MSGFILTER*)pv;
        }
        break;
        // 存储剪贴板
    case EN_SAVECLIPBOARD:
        {
            ENSAVECLIPBOARD* ens = (ENSAVECLIPBOARD*)pv;
        }
        break;
        // 拖动完成，需要事件掩码ENM_DRAGDROPDONE
    case EN_DRAGDROPDONE:
        break;
    }

    return S_OK;
}

HIMC TextRenderScope::TxImmGetContext()
{
    return ImmGetContext(GetHostHwnd());
}

void TextRenderScope::TxImmReleaseContext(HIMC himc)
{
    ImmReleaseContext(GetHostHwnd(), himc);
}

HRESULT TextRenderScope::TxGetSelectionBarWidth(LONG *plSelBarWidth)
{
    *plSelBarWidth = 0;
    return S_OK;
}

BOOL TextRenderScope::TxIsDoubleClickPending()
{
    return FALSE;
}

HRESULT TextRenderScope::TxGetWindow(HWND *phwnd)
{
    *phwnd = GetHostHwnd();
    return S_OK;
}

HRESULT TextRenderScope::TxSetForegroundWindow()
{
    return S_OK;
}

HPALETTE TextRenderScope::TxGetPalette()
{
    return S_OK;
}

void TextRenderScope::RegisterDragDrop(void)
{
    IDropTarget *pdt;

    if (!_isRegisteredForDrop && _textSrv->TxGetDropTarget(&pdt) == NOERROR)
    {
        HRESULT hr = ::RegisterDragDrop(GetHostHwnd(), pdt);

        if (hr == NOERROR)
        {	
            _isRegisteredForDrop = TRUE;
        }

        pdt->Release();
    }
}


void TextRenderScope::RevokeDragDrop(void)
{
    if (_isRegisteredForDrop)
    {
        ::RevokeDragDrop(GetHostHwnd());
        _isRegisteredForDrop = FALSE;
    }
}

BOOL TextRenderScope::InitializeTextService(TextBoxBase* holder)
{
    IUnknown *pUnk = NULL;
    HRESULT hr = S_OK;
    DWORD dwColor = ARGB(255,0,0,0);

    _holder = holder;
    _isInplaceActive = TRUE;

    InternalInitCharFormat(dwColor, &_cf, NULL);
    InternalInitParaFormat(&_pf);

    if (FAILED(CreateTextServices(NULL, this, &pUnk))) 
    {
        return FALSE;
    }

    hr = pUnk->QueryInterface(IID_ITextServices,(void **)&_textSrv);

    if (FAILED(hr)) 
    {
        pUnk->Release();
        return FALSE;
    }

    hr = pUnk->QueryInterface(IID_ITextDocument,(void **)&_textDoc);
    pUnk->Release();

    if (FAILED(hr)) 
    {
        return FALSE;
    }

    //
    // 设置需要接收的事件类型
    //
    SetEventMask(ENM_OBJECTPOSITIONS | ENM_PROTECTED | ENM_DROPFILES | ENM_CHANGE | ENM_LINK | ENM_SELCHANGE);

    LRESULT lResult = 0;

    // 设置ClipBoard
    RegisterClipboardFormat(CF_RETEXTOBJ);
    _textSrv->TxSendMessage(EM_SETLANGOPTIONS, 0, 0, &lResult);

    //
    // 激活
    //
    OnTxInPlaceActivate(NULL);
    
    SetMultiLine(TRUE);
    SetTransparent(TRUE);
    SetClientRect(_rcClnt);
    AddVisualChild(&_caret);

    //TM_PLAINTEXT			= 1,
    //    TM_RICHTEXT				= 2,	// Default behavior 
    //    TM_SINGLELEVELUNDO		= 4,
    //    TM_MULTILEVELUNDO		= 8,	// Default behavior 
    //    TM_SINGLECODEPAGE		= 16,
    //    TM_MULTICODEPAGE		= 32	// Default behavior 

    return TRUE;
}

void TextRenderScope::InitializeRichMode()
{
    SetRichTextFlag(TRUE);

    RichEditOleCallback* callback = new RichEditOleCallback();
    SetRichEditOleCallback(callback);
    callback->SetTextService(GetTextService());
}

LRESULT TextRenderScope::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam) const
{
    LRESULT lr = 0;
    _textSrv->TxSendMessage(msg, wparam, lparam, &lr);
    return lr;
}

LRESULT TextRenderScope::InternalSendMessage(UINT msg) const
{
    MessageParam& mp = Assigner::Current()->lastmp;
#ifdef _DEBUG
    if (msg != Assigner::Current()->lastmp.msg)
    {
        throw "message don't equal";
        abort();
    }
#endif
    return TxSendMessage(mp.msg, mp.wp, mp.lp);
}

void TextRenderScope::TxRender(HDC hdc, suic::Rect rcClip)
{
    GetTextService()->TxDraw(
            DVASPECT_CONTENT,
            0,
            NULL,
            NULL,
            hdc,
            NULL,
            (RECTL*)&_rcClnt,
            NULL,
            &rcClip,
            NULL,
            NULL,
            IsInpaceActive() ? TXTVIEW_ACTIVE : TXTVIEW_INACTIVE);

    ::SelectClipRgn(hdc, NULL);
}

//-------------------------------------------------------------
//

void TextRenderScope::OnRenderSizeChanged(SizeChangedInfo& e)
{
    FrameworkElement::OnRenderSizeChanged(e);

    Point pt = PointToScreen(Point());
    Rect rcClient(Point(), e.NewSize());

    //
    // 这里的区域是相对于宿主窗口客户全的偏移，因此需要校正其坐标
    //
    rcClient.Offset(pt);
    SetClientRect(rcClient);
}

void TextRenderScope::OnRender(Drawing* drawing)
{
    suic::Rect rcClip = drawing->GetClipBound();
    Bitmap* dBmp = drawing->GetBitmap();
    suic::Byte alpha = Color::A(_textColor);

    rcClip.Intersect(&_rcClnt);

    if (alpha == 255)
    {
        suic::AlphaOp aOp(dBmp, _rcClnt);

        aOp.Backup(drawing, rcClip);
        TxRender(aOp.GetDrawDc(), rcClip);
        aOp.Restore(drawing);
    }
    else
    {
        suic::SelfAlphaOp aOp2(dBmp, _rcClnt);

        aOp2.Backup(drawing, rcClip);
        TxRender(aOp2.GetDrawDc(), rcClip);
        aOp2.Restore(drawing, alpha);
    }
}

void TextRenderScope::InvalidateScrollInfo(const Size* constraint)
{
    ScrollData* scrData = _scrollInfo.GetScrollData();

    if (_scrollInfo.GetScrollOwner())
    {
        if (constraint != NULL)
        {
            scrData->viewport = *constraint;
            //scrData->viewport.cx -= _rcInset.SumLR();
            //scrData->viewport.cy -= _rcInset.SumLR();
        }

        suic::Point offset = scrData->offset;
        offset.x = max(0, min(offset.x, scrData->extent.cx - scrData->viewport.cx));
        offset.y = max(0, min(offset.y, scrData->extent.cy - scrData->viewport.cy));
        scrData->_computedOffset = offset;
        _scrollInfo.GetScrollOwner()->InvalidateScrollInfo(false);
    }
}

Size TextRenderScope::OnMeasure(const Size& constraint)
{
    InvalidateScrollInfo(&constraint);
    _caret.Measure(constraint);
    return _scrollInfo.GetScrollData()->extent;
}

Size TextRenderScope::OnArrange(const Size& arrangeSize)
{
    return arrangeSize;
}

void TextRenderScope::OnSetCursor(CursorEventArg& e)
{
    //FrameworkElement::OnSetCursor(e);

    suic::Point pt = PointToScreen(suic::Point());
    suic::Rect rect(0, 0, GetActualWidth(), GetActualHeight());
    rect.Offset(pt);
    pt = e.GetMousePoint();

    e.SetCursor(OCursor::Ibeam);
    //GetTextService()->OnTxSetCursor(DVASPECT_CONTENT, -1, NULL, NULL, NULL, NULL, &rect, pt.x, pt.y);
    //TxSendMessage(WM_SETCURSOR, Assigner::Current()->lastmp.wp, Assigner::Current()->lastmp.lp);
    //e.SetIgnore(true);
    e.SetHandled(true);
}

void TextRenderScope::OnTextInput(KeyboardEventArg& e)
{
    e.SetHandled(true);
    InternalSendMessage(WM_CHAR);
}

void TextRenderScope::OnKeyDown(KeyboardEventArg& e)
{
    e.SetHandled(true);
    InternalSendMessage(WM_KEYDOWN);
}

void TextRenderScope::OnKeyUp(KeyboardEventArg& e)
{
    e.SetHandled(true);
    InternalSendMessage(WM_KEYUP);
}

void TextRenderScope::OnGotFocus(RoutedEventArg& e)
{
    TxShowCaret(TRUE);
    _textSrv->OnTxUIActivate();
}

void TextRenderScope::OnLostFocus(RoutedEventArg& e)
{
    TxSendMessage(WM_KILLFOCUS, Assigner::Current()->lastmp.wp, Assigner::Current()->lastmp.lp);
    TxShowCaret(FALSE);
    _textSrv->OnTxUIDeactivate();
}

void TextRenderScope::OnMouseEnter(MouseButtonEventArg& e)
{
    FrameworkElement::OnMouseEnter(e);
    OnMouseMove(e);
}

void TextRenderScope::OnMouseMove(MouseButtonEventArg& e)
{
    e.SetHandled(true);
    InternalSendMessage(WM_MOUSEMOVE);
}

void TextRenderScope::OnMouseLeave(MouseButtonEventArg& e)
{
    FrameworkElement::OnMouseLeave(e);
    InternalSendMessage(Assigner::Current()->lastmp.msg);
}

void TextRenderScope::OnMouseLeftButtonDown(MouseButtonEventArg& e)
{
    FrameworkElement::OnMouseLeftButtonDown(e);

    TxSendMessage(WM_SETFOCUS, Assigner::Current()->lastmp.wp, Assigner::Current()->lastmp.lp);
    InternalSendMessage(Assigner::Current()->lastmp.msg);
}

void TextRenderScope::OnMouseLeftButtonDbclk(MouseButtonEventArg& e)
{
    ;
}

void TextRenderScope::OnMouseLeftButtonUp(MouseButtonEventArg& e)
{
    FrameworkElement::OnMouseLeftButtonUp(e);
    //e.SetHandled(true);
    InternalSendMessage(WM_LBUTTONUP);
}

//--------------------------------------------------------

int TextRenderScope::GetOleObjCount()
{
    IRichEditOle* pRichEditOle = GetRichEditOle();
    if (NULL == pRichEditOle)
    {
        return 0;
    }
    else
    {
        return pRichEditOle->GetObjectCount();
    }
}

bool TextRenderScope::GetOleObjOnIndex(LONG cp, REOBJECT* preobj)
{
    //WCH_EMBEDDING 
    IRichEditOle* pRichEditOle = GetRichEditOle();
    if (NULL == pRichEditOle)
    {
        return false;
    }
    else
    {
        return (NOERROR == pRichEditOle->GetObject(cp, preobj, REO_GETOBJ_POLEOBJ | REO_GETOBJ_PSTG | REO_GETOBJ_POLESITE));
    }
}

bool TextRenderScope::GetOleObjOnCp(REOBJECT* preobj)
{
    return GetOleObjOnIndex(REO_IOB_USE_CP, preobj);
}

bool TextRenderScope::GetOleObjOnSel(REOBJECT* preobj)
{
    return GetOleObjOnIndex(REO_IOB_SELECTION, preobj);
}

//int (CALLBACK* EDITWORDBREAKPROC)(LPWSTR lpch, int ichCurrent, int cch, int code);
//case EM_SETWORDBREAKPROC:
//_pfnWB = (EDITWORDBREAKPROC) lparam;
//break;