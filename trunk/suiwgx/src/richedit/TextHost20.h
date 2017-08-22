// 华勤科技版权所有 2010-2011
// 
// 文件名：TextHost20.h
// 功  能：实现ITextHost接口
// 
// 作  者：Sharpui开发组
// 时  间：2010-08-31
// 
// ============================================================================

#ifndef _TEXTHOST20_H_
#define _TEXTHOST20_H_

#include <Richedit.h>
#include <RichOle.h>
#include <textserv.h>

#include "RichEditOleCallback.h"

#define Tobool(v) (v ? true : false)
#define ToBOOL(v) (v ? TRUE : FALSE)

class TextHostService : public ITextHost
{
public:

    TextHostService(void);
    ~TextHostService(void);

    BOOL Init(LPCWSTR pszText);

    ITextServices * GetTextService(){return _textSrv;}
    IRichEditOle* GetRichEditOle();

    bool SetRichEditOleCallback(IRichEditOleCallback* callback);

    void TxRender(HDC hdc, suic::Rect rcClip);
    HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const;

public:

    DWORD GetEventMask() const;
    DWORD SetEventMask(DWORD dwEventMask);

    BOOL GetWordWrap(void) const { return _isWordWrap; }
    void SetWordWrap(BOOL fWordWrap);

    BOOL GetMultiLine(void) { return ((_dwStyle & ES_MULTILINE) == ES_MULTILINE); }
    void SetMultiLine(BOOL fMultiLine);

    BOOL GetReadOnly();
    void SetReadOnly(BOOL fReadOnly);

    void SetFont(HFONT hFont);
    void SetColor(DWORD dwColor);

    SIZEL* GetExtent();
    void SetExtent(SIZEL *psizelExtent);

    void LimitText(LONG nChars);

    BOOL GetAllowBeep();
    void SetAllowBeep(BOOL fAllowBeep);

    WORD GetDefaultAlign();
    void SetDefaultAlign(WORD wNewAlign);

    BOOL GetRichTextFlag();
    void SetRichTextFlag(BOOL fNew);

    LONG GetDefaultLeftIndent();
    void SetDefaultLeftIndent(LONG lNewIndent);

    BOOL SetSaveSelection(BOOL fSaveSelection);
    BOOL IsInpaceActive(void) { return _isInplaceActive; }

    void SetInsetRect(RECT* prc) ;
    void SetClientRect(RECT* prc) ;
    void SetTransparent(BOOL fTransparent);
    BOOL IsTransparent() const { return _isTransparent; }

    LONG SetAccelPos(LONG laccelpos);
    WCHAR SetPasswordChar(WCHAR chPasswordChar);
    void SetDisabled(BOOL fOn);
    LONG SetSelBarWidth(LONG lSelBarWidth);

    void ShowVertScrollBar(BOOL val);
    BOOL IsShowVertScrollBar() const;
    void ShowHoriScrollBar(BOOL val);
    BOOL IsShowHoriScrollBar() const;

    void SetAutoHoriScrollBar(BOOL val);
    BOOL IsAutoHoriScrollBar() const;
    void SetAutoVertScrollBar(BOOL val);
    BOOL IsAutoVertScrollBar() const;

    void ModifyStyle(DWORD dwStyle, BOOL bAdd);
    void SetStyle(DWORD dwStyle);
    BOOL HasStyle(DWORD dwStyle) const;

    //-------------------------------------图像操作
    void AddImage(LPCTSTR filepath);

public:

    void OnObjectTimer();
    bool TxOnSetTimer(UINT idTimer);

    // -----------------------------
    //	IUnknown interface
    // -----------------------------
    virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG _stdcall AddRef(void);
    virtual ULONG _stdcall Release(void);

protected:

    HRESULT OnTxInPlaceDeactivate();
    HRESULT OnTxInPlaceActivate(LPCRECT prcClient);

    // -----------------------------
    //	ITextHost interface
    // -----------------------------

    //@cmember Get the DC for the host
    virtual HDC 		TxGetDC();
    //@cmember Release the DC gotten from the host
    virtual INT			TxReleaseDC(HDC hdc);
    //@cmember Show the scroll bar
    virtual BOOL 		TxShowScrollBar(INT fnBar, BOOL fShow);
    //@cmember Enable the scroll bar
    virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
    //@cmember Set the scroll range
    virtual BOOL TxSetScrollRange(INT fnBar,LONG nMinPos, INT nMaxPos, BOOL fRedraw);

    //@cmember Set the scroll position
    virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
    //@cmember InvalidateRect
    virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
    //@cmember Send a WM_PAINT to the window
    virtual void TxViewChange(BOOL fUpdate);
    //@cmember Create the caret
    virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
    //@cmember Show the caret
    virtual BOOL TxShowCaret(BOOL fShow);
    //@cmember Set the caret position
    virtual BOOL TxSetCaretPos(INT x, INT y);
    //@cmember Create a timer with the specified timeout
    virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
    //@cmember Destroy a timer
    virtual void TxKillTimer(UINT idTimer);

    //@cmember Scroll the content of the specified window's client area
    virtual void TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip,
        HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);

    //@cmember Get mouse capture
    virtual void TxSetCapture(BOOL fCapture);
    //@cmember Set the focus to the text window
    virtual void TxSetFocus();
    //@cmember Establish a new cursor shape
    virtual void  TxSetCursor(HCURSOR hcur, BOOL fText);
    //@cmember Converts screen coordinates of a specified point to the client coordinates
    virtual BOOL TxScreenToClient (LPPOINT lppt);
    //@cmember Converts the client coordinates of a specified point to screen coordinates
    virtual BOOL TxClientToScreen (LPPOINT lppt);
    //@cmember Request host to activate text services
    virtual HRESULT TxActivate( LONG * plOldState );
    //@cmember Request host to deactivate text services
    virtual HRESULT TxDeactivate( LONG lNewState );
    //@cmember Retrieves the coordinates of a window's client area
    virtual HRESULT TxGetClientRect(LPRECT prc);
    //@cmember Get the view rectangle relative to the inset
    virtual HRESULT TxGetViewInset(LPRECT prc);
    //@cmember Get the default character format for the text
    virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF );
    //@cmember Get the default paragraph format for the text
    virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
    //@cmember Get the background color for the window
    virtual COLORREF TxGetSysColor(int nIndex);
    //@cmember Get the background (either opaque or transparent)
    virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
    //@cmember Get the maximum length for the text
    virtual HRESULT TxGetMaxLength(DWORD *plength);
    //@cmember Get the bits representing requested scroll bars for the window
    virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
    //@cmember Get the character to display for password input
    virtual HRESULT TxGetPasswordChar(TCHAR *pch);
    //@cmember Get the accelerator character
    virtual HRESULT	TxGetAcceleratorPos(LONG *pcp);
    //@cmember Get the native size
    virtual HRESULT	TxGetExtent(LPSIZEL lpExtent);
    //@cmember Notify host that default character format has changed
    virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);
    //@cmember Notify host that default paragraph format has changed
    virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);
    //@cmember Bulk access to bit properties
    virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
    //@cmember Notify host of events
    virtual HRESULT TxNotify(DWORD iNotify, void *pv);

    // Far East Methods for getting the Input Context
    //#ifdef WIN95_IME
    virtual HIMC TxImmGetContext();
    virtual void TxImmReleaseContext( HIMC himc );
    //#endif
    virtual HRESULT TxGetSelectionBarWidth (LONG *plSelBarWidth);

    // ITextHost2
    virtual BOOL		TxIsDoubleClickPending(); 
    //@cmember Get the overall window for this control	 
    virtual HRESULT		TxGetWindow(HWND *phwnd);
    //@cmember Set control window to foreground
    virtual HRESULT		TxSetForegroundWindow();
    //@cmember Set control window to foreground
    virtual HPALETTE	TxGetPalette();

    HWND GetHostHwnd();

protected:

    void OnTxPropertyBitsChange(UINT key, UINT val);

    void RegisterDragDrop(void);
    void RevokeDragDrop(void);

    bool GetOleImageRect(const REOBJECT& reo, IRichEditOle *pRichEditOle, RECT* lpRect);

protected:

    ITextServices* _textSrv;
    //RichText20* _richbox;

    ULONG _refCount;
    BOOL _isRegisteredForDrop;
    // 是否透明
    BOOL _isTransparent;
    // 是否自动换行
    BOOL _isWordWrap;
    BOOL _isAllowBeep;
    BOOL _isSaveSelection;
    BOOL _isInplaceActive;
    BOOL _isRichMode;
    BOOL _isEnableAutoWordSel;

    LONG _lSelBarWidth;
    // 允许的最大字符个数
    LONG _lMaxText;
    SIZEL _sizelExtent;
    suic::Rect _rcClient;
    suic::Rect _rcInset;

    int _timerid;
    // 动画定时器id
    int _animateid;

    // 控件风格
    DWORD _dwStyle;
    // 字符格式
    CHARFORMAT2W _cf;
    // 段格式
    PARAFORMAT2 _pf;
    LONG _laccelpos;
    // 密码掩码字符
    suic::Char _chPasswordChar;
};

inline TextHostService* GetTextHost20(void* texthost)
{
    TextHostService* th20 = (TextHostService*)(texthost);
    return th20;
}

inline ITextServices* GetTextSrv(void* texthost)
{
    if (!texthost)
    {
        return NULL;
    }
    else
    {
        TextHostService* th20 = (TextHostService*)(texthost);
        return th20->GetTextService();
    }
}

#endif
