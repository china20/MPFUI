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
//EXTERN_C const IID IID_ITextServices = {
//    0x8d33f740,
//    0xcf58,
//    0x11ce,
//    {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
//};
//
//// c5bdd8d0-d26e-11ce-a89e-00aa006cadc5
//EXTERN_C const GUID IID_ITextHost = 
//{ 0x13e670f4, 0x1a5a, 0x11cf, { 0xab, 0xeb, 0x0, 0xaa, 0x0, 0xb6, 0x5e, 0xa1 } };
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

DEFINE_GUIDXXX(IID_ITextDocument,0x8CC497C0,0xA1DF,0x11CE,0x80,0x98,
               0x00,0xAA,0x00,0x47,0xBE,0x5D);

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

void InternalInitCharFormat(HDC hdc, DWORD dwColor, CHARFORMAT2W* pcf, HFONT hfont)
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
    LONG yPixPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
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

TextHostService::TextHostService(TextBoxBase* textBox)
    : _textSrv(textBox)
{
    ::ZeroMemory(&_refCount, sizeof(TextHostService) - offsetof(TextHostService, _refCount));    
    _lMaxText = 16 * 1024 * 1024;
    _laccelpos = -1;
    _animateid = 5000;

    SetWordWrap(TRUE);
    SetMultiLine(TRUE);
    SetStyle(WS_VSCROLL | /*WS_HSCROLL | */ES_AUTOVSCROLL | ES_AUTOHSCROLL/* | WS_EX_TRANSPARENT*/);
}

TextHostService::~TextHostService(void)
{
    RevokeDragDrop();
    _textSrv->OnTxInPlaceDeactivate();
    _textSrv->Release();
}

//=========================================================================

void TextHostService::ModifyStyle(DWORD dwStyle, BOOL bAdd)
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

void TextHostService::SetStyle(DWORD dwStyle)
{
    _dwStyle = dwStyle;
}

BOOL TextHostService::HasStyle(DWORD dwStyle) const
{
    return ((_dwStyle & dwStyle) == dwStyle);
}

bool TextHostService::GetOleImageRect(const REOBJECT& reo, IRichEditOle *pRichEditOle, RECT* lpRect)
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

void TextHostService::OnObjectTimer()
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

        if (rect.Intersect(&_rcClient))
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

void TextHostService::AddImage(LPCTSTR filepath)
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

IRichEditOle* TextHostService::GetRichEditOle()
{
    IRichEditOle* richeditole = NULL;
    _textSrv->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&richeditole, 0);
    return richeditole;
}

bool TextHostService::SetRichEditOleCallback(IRichEditOleCallback* callback)
{
    LRESULT lr = 0;
    TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)callback, &lr);
    return (lr != 0);
}

void TextHostService::ShowVertScrollBar(BOOL val)
{
    ModifyStyle(WS_VSCROLL, val);
}

BOOL TextHostService::IsShowVertScrollBar() const
{
    return HasStyle(WS_VSCROLL);
}

void TextHostService::ShowHoriScrollBar(BOOL val)
{
    ModifyStyle(WS_HSCROLL, val);
}

BOOL TextHostService::IsShowHoriScrollBar() const
{
    return HasStyle(WS_HSCROLL);
}

void TextHostService::SetAutoHoriScrollBar(BOOL val)
{
    ModifyStyle(ES_AUTOHSCROLL, val);
}

BOOL TextHostService::IsAutoHoriScrollBar() const
{
    return HasStyle(ES_AUTOHSCROLL);
}

void TextHostService::SetAutoVertScrollBar(BOOL val)
{
     ModifyStyle(ES_AUTOVSCROLL, val);
}

BOOL TextHostService::IsAutoVertScrollBar() const
{
    return HasStyle(ES_AUTOVSCROLL);
}

void TextHostService::OnTxPropertyBitsChange(UINT key, UINT val)
{
    if (_textSrv)
    {
        _textSrv->OnTxPropertyBitsChange(key, val);
    }
}

DWORD TextHostService::GetEventMask() const
{
    LRESULT lResult = 0;
    _textSrv->TxSendMessage(EM_GETEVENTMASK, 0, 0, &lResult);
    return (DWORD)lResult;
}

DWORD TextHostService::SetEventMask(DWORD dwEventMask)
{
    LRESULT lResult = 0;
    TxSendMessage(EM_SETEVENTMASK, 0, dwEventMask, &lResult);
    return (DWORD)lResult;
}

void TextHostService::SetWordWrap(BOOL fWordWrap)
{
    _isWordWrap = fWordWrap;
    OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
}

void TextHostService::SetMultiLine(BOOL fMultiLine)
{
    ModifyStyle(ES_MULTILINE, fMultiLine);
    OnTxPropertyBitsChange(TXTBIT_MULTILINE, fMultiLine ? TXTBIT_MULTILINE : 0);
}

BOOL TextHostService::GetReadOnly()
{
    return (_dwStyle & ES_READONLY) != 0;
}

void TextHostService::SetReadOnly(BOOL fReadOnly)
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

void TextHostService::SetFont(HFONT hFont) 
{
    if (hFont == NULL) 
    {
        HDC dc = GetDC(GetHostHwnd());
        LOGFONT lf;
        LONG yPixPerInch = ::GetDeviceCaps(dc, LOGPIXELSY);

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

        ::ReleaseDC(GetHostHwnd(), dc);

        OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
    }
}

void TextHostService::SetColor(DWORD dwColor)
{
    // 设置文本颜色
    _cf.crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
    OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
}

SIZEL* TextHostService::GetExtent() 
{
    return &_sizelExtent;
}

void TextHostService::SetExtent(SIZEL* psizelExtent) 
{ 
    _sizelExtent = *psizelExtent; 
    OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
}

void TextHostService::LimitText(LONG nChars)
{
    _lMaxText = nChars;
    if (_lMaxText <= 0) 
    {
        _lMaxText = 16 * 1024 * 1024;
    }

    OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
}

BOOL TextHostService::GetAllowBeep()
{
    return _isAllowBeep;
}

void TextHostService::SetAllowBeep(BOOL fAllowBeep)
{
    _isAllowBeep = fAllowBeep;

    OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
}

WORD TextHostService::GetDefaultAlign()
{
    return _pf.wAlignment;
}

void TextHostService::SetDefaultAlign(WORD wNewAlign)
{
    _pf.wAlignment = wNewAlign;
    OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

BOOL TextHostService::GetRichTextFlag()
{
    return _isRichMode;
}

void TextHostService::SetRichTextFlag(BOOL fNew)
{
    if (_isRichMode != fNew)
    {
        _isRichMode = fNew;
        OnTxPropertyBitsChange(TXTBIT_RICHTEXT, fNew ? TXTBIT_RICHTEXT : 0);
    }
}

LONG TextHostService::GetDefaultLeftIndent()
{
    return _pf.dxOffset;
}

void TextHostService::SetDefaultLeftIndent(LONG lNewIndent)
{
    _pf.dxOffset = lNewIndent;
    OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

void TextHostService::SetInsetRect(RECT* prc) 
{
    HDC dc = GetDC(GetHostHwnd());

    LONG xPerInch = ::GetDeviceCaps(dc, LOGPIXELSX); 
    LONG yPerInch =	::GetDeviceCaps(dc, LOGPIXELSY); 
    _rcInset.left = DXtoHimetricX(prc->left, xPerInch);
    _rcInset.top = DYtoHimetricY(prc->top, yPerInch);
    _rcInset.right = DXtoHimetricX(prc->right, xPerInch);
    _rcInset.bottom = DYtoHimetricY(prc->bottom, yPerInch);

    ::ReleaseDC(GetHostHwnd(), dc);

    OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
}

void TextHostService::SetClientRect(RECT *prc) 
{
    _rcClient = *prc;

    HDC dc = GetDC(GetHostHwnd());
    LONG xPerInch = ::GetDeviceCaps(dc, LOGPIXELSX); 
    LONG yPerInch =	::GetDeviceCaps(dc, LOGPIXELSY); 
    _sizelExtent.cx = DXtoHimetricX(_rcClient.right - _rcClient.left, xPerInch);
    _sizelExtent.cy = DYtoHimetricY(_rcClient.bottom - _rcClient.top, yPerInch);

    ::ReleaseDC(GetHostHwnd(), dc);

    OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
}

BOOL TextHostService::SetSaveSelection(BOOL isSaveSelection)
{
    BOOL fResult = _isSaveSelection;
    _isSaveSelection = isSaveSelection;
    OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, isSaveSelection ? TXTBIT_SAVESELECTION : 0);
    return fResult;		
}

void TextHostService::SetTransparent(BOOL isTransparent)
{
    _isTransparent = isTransparent;
    OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
}

LONG TextHostService::SetAccelPos(LONG laccelpos)
{
    LONG laccelposOld = _laccelpos;
    _laccelpos = laccelpos;
    OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);
    return laccelposOld;
}

WCHAR TextHostService::SetPasswordChar(WCHAR chPasswordChar)
{
    WCHAR chOldPasswordChar = _chPasswordChar;
    _chPasswordChar = chPasswordChar;

    OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, (chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);
    return chOldPasswordChar;
}

void TextHostService::SetDisabled(BOOL fOn)
{
    _cf.dwMask |= CFM_COLOR | CFM_DISABLED;
    _cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;
    if (!fOn)
    {
        _cf.dwEffects &= ~CFE_DISABLED;
    }
    OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
}

LONG TextHostService::SetSelBarWidth(LONG lSelBarWidth)
{
    LONG lOldSelBarWidth = _lSelBarWidth;
    _lSelBarWidth = lSelBarWidth;

    if (_lSelBarWidth)
    {
        _dwStyle |= ES_SELECTIONBAR;
    }
    else
    {
        _dwStyle &= (~ES_SELECTIONBAR);
    }

    OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

    return lOldSelBarWidth;
}

HRESULT	TextHostService::OnTxInPlaceDeactivate()
{
    HRESULT hr = _textSrv->OnTxInPlaceDeactivate();
    if (SUCCEEDED(hr))
    {
        _isInplaceActive = FALSE;
    }
    return hr;
}

HRESULT	TextHostService::OnTxInPlaceActivate(LPCRECT prcClient)
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
HRESULT _stdcall TextHostService::QueryInterface( REFIID riid, void **ppvObject )
{
    HRESULT hr = E_NOINTERFACE;
    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown) 
        || IsEqualIID(riid, IID_ITextHost)
        /*|| IsEqualIID(riid, IID_ITextHost2)*/) 
    {
        AddRef();
        *ppvObject = (ITextHost *) this;
        hr = S_OK;
    }

    return hr;
}

ULONG _stdcall TextHostService::AddRef( void )
{
    return ++_refCount;
}

ULONG _stdcall TextHostService::Release( void )
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
HRESULT TextHostService::TxGetViewInset(LPRECT prc)
{
    *prc = _rcInset;
    return NOERROR;
}

HRESULT TextHostService::TxGetCharFormat(const CHARFORMATW **ppCF)
{
    *ppCF = &_cf;
    return NOERROR;
}

HRESULT TextHostService::TxGetParaFormat( const PARAFORMAT **ppPF )
{
    *ppPF = &_pf;
    return NOERROR;
}

HRESULT TextHostService::TxGetClientRect(LPRECT prc)
{
    *prc = _rcClient;
    return NOERROR;
}

HRESULT TextHostService::TxDeactivate( LONG lNewState )
{
    return S_OK;
}

HRESULT TextHostService::TxActivate( LONG * plOldState )
{
    return S_OK;
}

BOOL TextHostService::TxClientToScreen(LPPOINT lppt)
{
    return ::ClientToScreen(GetHostHwnd(), lppt);
}

HWND TextHostService::GetHostHwnd()
{
    return 0;//HandleToHwnd(suic::GetHostHwnd(//_richbox));
}

BOOL TextHostService::TxScreenToClient( LPPOINT lppt )
{
    return ::ScreenToClient(GetHostHwnd(), lppt);
}

void TextHostService::TxSetCursor(HCURSOR hcur, BOOL fText)
{
    ::SetCursor(hcur);
}

void TextHostService::TxSetFocus()
{
    //_richbox->SetFocus();
}

void TextHostService::TxSetCapture(BOOL fCapture)
{
    if (fCapture) 
    {
        //_richbox->SetCaptureMouse();
    }
    else
    {
        //_richbox->ReleaseCaptureMouse();
    }
}

void TextHostService::TxScrollWindowEx( INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll )
{
}

void TextHostService::TxKillTimer(UINT idTimer)
{
    ::KillTimer(GetHostHwnd(), idTimer);
    _timerid = 0;
}

bool TextHostService::TxOnSetTimer(UINT idTimer)
{
    if (_timerid == idTimer)
    {
        return true;
    }
    else if (_animateid == idTimer)
    {
        OnObjectTimer();
        return true;
    }
    else
    {
        return false;
    }
}

BOOL TextHostService::TxSetTimer(UINT idTimer, UINT uTimeout)
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

BOOL TextHostService::TxSetCaretPos(INT x, INT y)
{
    //_richbox->SetCaretPos(x, y);
    return TRUE;//::SetCaretPos(x, y);
}

BOOL TextHostService::TxShowCaret(BOOL fShow)
{
    //_richbox->ShowCaret(Tobool(fShow));
    return TRUE;// ::ShowCaret(GetHostHwnd());
}

BOOL TextHostService::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
    //_richbox->CreateCaret(xWidth, yHeight);
    return TRUE;// CreateCaret(GetHostHwnd(), hbmp, xWidth, yHeight);
}

HDC TextHostService::TxGetDC()
{
    return GetDC(GetHostHwnd());
}

INT TextHostService::TxReleaseDC( HDC hdc )
{
    return ReleaseDC(GetHostHwnd(), hdc);
}

BOOL TextHostService::TxShowScrollBar(INT fnBar, BOOL fShow)
{
    ////_richbox->OnShowScrollBar(fnBar == SB_HORZ, Tobool(fShow));

    /*if (fnBar == SB_HORZ)
    {
        //_richbox->GetHoriScrollBar()->SetVisible(Tobool(fShow));
    }
    else
    {
        //_richbox->GetVertScrollBar()->SetVisible(Tobool(fShow));
    }*/
    return TRUE;
}

BOOL TextHostService::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags)
{
    return TRUE;
}

BOOL TextHostService::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
    //_richbox->OnSetScrollRange(fnBar == SB_HORZ, nMaxPos - nMinPos, Tobool(fRedraw));

    /*ScrollBar* pBar = NULL;
    if (fnBar == SB_HORZ)
    {
        pBar = //_richbox->GetHoriScrollBar();
    }
    else
    {
        pBar = //_richbox->GetVertScrollBar();
    }

    pBar->UpdateScrollLogic(nMaxPos - nMinPos, Tobool(fRedraw));*/

    return TRUE;
}

BOOL TextHostService::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
{
    //_richbox->OnSetScrollPos(fnBar == SB_HORZ, (int)nPos, Tobool(fRedraw));
    
    /*if (fnBar == SB_HORZ)
    {
        //_richbox->GetHoriScrollBar()->SetScrollPos(nPos);
        if (fRedraw) //_richbox->GetHoriScrollBar()->UpdateLayout();
    }
    else
    {
        //_richbox->GetVertScrollBar()->SetScrollPos(nPos);
        if (fRedraw) //_richbox->GetVertScrollBar()->UpdateLayout();
    }*/
    return TRUE;
}

void TextHostService::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
    if (prc && !fMode)
    {
        if (prc->right > prc->left)
        {
            //_richbox->InvalidateVisual();
        }
    }
    else
    {
        //_richbox->InvalidateVisual();
    }
}

void TextHostService::TxViewChange(BOOL fUpdate)
{
    if (fUpdate)
    {
        //_richbox->InvalidateVisual();
    }
}

COLORREF TextHostService::TxGetSysColor(int nIndex)
{
    switch (nIndex)
    {
        // 高亮文本颜色
        case COLOR_HIGHLIGHTTEXT:
            break;
            // 高亮文本背景颜色
        case COLOR_HIGHLIGHT:
            break;
            // 背景颜色
        case COLOR_WINDOW:
            return RGB(10,150,160);
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

HRESULT TextHostService::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
    *pstyle = _isTransparent ? TXTBACK_TRANSPARENT: TXTBACK_OPAQUE;
    return S_OK;
}

HRESULT TextHostService::TxGetMaxLength(DWORD *plength)
{
    *plength = _lMaxText;
    return S_OK;
}

HRESULT TextHostService::TxGetScrollBars(DWORD *pdwScrollBar)
{
    *pdwScrollBar = _dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL 
        | ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

    return S_OK;
}

HRESULT TextHostService::TxGetPasswordChar(TCHAR *pch)
{
    *pch = _chPasswordChar;
    return S_OK;
}

HRESULT TextHostService::TxGetAcceleratorPos(LONG *pcp)
{
    *pcp = _laccelpos;
    return S_OK;
}

HRESULT TextHostService::TxGetExtent(LPSIZEL lpExtent)
{
    *lpExtent = _sizelExtent;
    return S_OK;
}

HRESULT TextHostService::OnTxCharFormatChange(const CHARFORMATW * pcf)
{
    return S_FALSE;
}

HRESULT TextHostService::OnTxParaFormatChange(const PARAFORMAT * ppf)
{
    return S_FALSE;
}

HRESULT TextHostService::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
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

HRESULT TextHostService::TxNotify(DWORD iNotify, void *pv)
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
        }
        break;
        // 内存溢出
    case EN_ERRSPACE:
        break;
        // 输入变化，需要事件ENM_CHANGE掩码
    case EN_CHANGE:
        {
            CHANGENOTIFY* c = (CHANGENOTIFY*)pv;
            //_richbox->OnTextChanged();
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
            REQRESIZE* p = (REQRESIZE*)pv;
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

HIMC TextHostService::TxImmGetContext()
{
    return ImmGetContext(GetHostHwnd());
}

void TextHostService::TxImmReleaseContext(HIMC himc)
{
    ImmReleaseContext(GetHostHwnd(), himc);
}

HRESULT TextHostService::TxGetSelectionBarWidth(LONG *plSelBarWidth)
{
    *plSelBarWidth = 0;
    return S_OK;
}

BOOL TextHostService::TxIsDoubleClickPending()
{
    return FALSE;
}

HRESULT TextHostService::TxGetWindow(HWND *phwnd)
{
    *phwnd = GetHostHwnd();
    return S_OK;
}

HRESULT TextHostService::TxSetForegroundWindow()
{
    return S_OK;
}

HPALETTE TextHostService::TxGetPalette()
{
    return S_OK;
}

void TextHostService::RegisterDragDrop(void)
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


void TextHostService::RevokeDragDrop(void)
{
    if (_isRegisteredForDrop)
    {
        ::RevokeDragDrop(GetHostHwnd());
        _isRegisteredForDrop = FALSE;
    }
}

BOOL TextHostService::Init(LPCWSTR pszText)
{
    IUnknown *pUnk;
    HRESULT hr;
    DWORD dwColor = ARGB(255,0,0,0);
    HDC hdc = GetDC(GetHostHwnd());

    //_richbox = pObj;
    _isInplaceActive = TRUE;

    InternalInitCharFormat(hdc, dwColor, &_cf, NULL);
    InternalInitParaFormat(&_pf);

    ::ReleaseDC(GetHostHwnd(), hdc);

    if (FAILED(CreateTextServices(NULL, this, &pUnk))) 
    {
        return FALSE;
    }

    hr = pUnk->QueryInterface(IID_ITextServices,(void **)&_textSrv);

    pUnk->Release();

    if(FAILED(hr)) 
    {
        return FALSE;
    }

    if (!1/*_richbox->IsReadOnly()*/)
    {
        RegisterDragDrop();
    }

    RichEditOleCallback* callback = new RichEditOleCallback();
    SetRichEditOleCallback(callback);

    callback->SetTextService(GetTextService());

    SetTransparent(TRUE);
    SetEventMask(ENM_CHANGE | ENM_LINK/* | ENM_REQUESTRESIZE | ENM_SELCHANGE*/);

    LRESULT lResult = 0;

    // 设置ClipBoard
    RegisterClipboardFormat(CF_RETEXTOBJ);

    _textSrv->TxSendMessage(EM_SETLANGOPTIONS, 0, 0, &lResult);
    OnTxInPlaceActivate(NULL);
    //_textSrv->TxSetText(pszText);

    TxSetTimer(_animateid, TIMER_INVALIDATE);

    return TRUE;
}

HRESULT TextHostService::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const
{
    return _textSrv->TxSendMessage(msg, wparam, lparam, plresult);
}

void TextHostService::TxRender(HDC hdc, suic::Rect rcClip)
{
    rcClip.Intersect(&_rcClient);

    GetTextService()->TxDraw(
        DVASPECT_CONTENT,
        0,
        NULL,
        NULL,
        hdc,
        NULL,
        (RECTL*)&_rcClient,
        NULL,
        &rcClip,
        NULL,
        NULL,
        IsInpaceActive() ? TXTVIEW_ACTIVE : TXTVIEW_INACTIVE);
}
