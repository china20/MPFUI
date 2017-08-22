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
#include <TOM.h>

#include "RichTextOleCallback.h"

#include <System/Windows/ScrollInfo.h>
#include <Extend/RichText/TextBoxBase.h>
#include <Framework/Controls/CaretElement.h>

#define Tobool(v) (v ? true : false)
#define ToBOOL(v) (v ? TRUE : FALSE)

class TextRenderScope : public ITextHost, public suic::FrameworkElement
{
public:

    TextRenderScope();
    ~TextRenderScope(void);

    BOOL InitializeTextService(TextBoxBase* holder);

    ITextServices* GetTextService() const
    {
        return _textSrv;
    }

    ITextDocument* GetTextDocument() const
    {
        return _textDoc;
    }

    ScrollViewer* GetScrollViewer() const
    {
        return _scrollInfo.GetScrollOwner();
    }

    int Select(long lStart, long lEnd)
    {
        CHARRANGE cr;
        cr.cpMin = lStart;
        cr.cpMax = lEnd;
        LRESULT lResult = TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr);

        return (int)lResult;
    }

    void ReplaceSel(const suic::String& strText, bool bCanUndo)
    {
        TxSendMessage(EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)strText.c_str()); 
    }

    void SetText(const suic::String& strText)
    {
        Select(0, -1);
        ReplaceSel(strText, false);
    }

    int GetVisualChildrenCount()
    {
        return (_caret.IsVisible() ? 1 : 0);
    }

    Element* GetVisualChild(int index)
    {
        return &_caret;
    }

    //-----------------------------------------------------------

    void InitializeRichMode();
    void SetScrollPending(bool val);

    IRichEditOle* GetRichEditOle();
    bool SetRichEditOleCallback(IRichEditOleCallback* callback);

    void TxRender(HDC hdc, suic::Rect rcClip);

    LRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam) const;
    LRESULT InternalSendMessage(UINT msg) const;

    //-----------------------------------------------------------
    // IOleObject操作
    
    int GetOleObjCount();

    bool GetOleObjOnCp(REOBJECT* preobj);
    bool GetOleObjOnSel(REOBJECT* preobj);
    bool GetOleObjOnIndex(LONG index, REOBJECT* preobj);

public:

    DWORD GetEventMask() const;
    DWORD SetEventMask(DWORD dwEventMask);

    int GetLineIndexFromCharIndex(int charIndex) const;
    int GetCharacterIndexFromLineIndex(int lineIndex) const;
    int GetCharIndexFromPoint(Point point, bool snapToText) const;
    Point GetPointFromCharIndex(int charIndex) const;
    int GetFirstVisibleLineIndex() const;
    int GetLastVisibleLineIndex() const;
    int GetLineCount() const;
    int GetLineLength(int lineIndex) const;

    String GetLineText(int lineIndex) const;

    bool GetWordWrap(void) const { return _isWordWrap; }
    void SetWordWrap(bool fWordWrap);

    bool GetMultiLine(void) { return ((_dwStyle & ES_MULTILINE) == ES_MULTILINE); }
    void SetMultiLine(bool fMultiLine);

    bool GetReadOnly();
    void SetReadOnly(bool fReadOnly);

    bool GetAcceptsReturn();
    void SetAcceptsReturn(bool val);

    bool GetAutoWordSelection();
    void SetAutoWordSelection(bool val);

    bool GetAcceptsTab();
    void SetAcceptsTab(bool val);

    bool GetIsReadOnlyCaretVisible();
    void SetIsReadOnlyCaretVisible(bool val);

    void SetFont(HFONT hFont);

    void SetTextColor(Color clr);
    void SetFontFamily(String name);
    void SetFontWeight(int weight);
    void SetFontSize(int size);

    void SetSelTextColor(Color clr);
    void SetSelBackground(Color clr);

    SIZEL GetExtent();
    void NotifyExtentChange();

    void NotifyAllChange() ;

    void SetMaxLength(int nChars);
    int GetMaxLength() const;

    void SetMaxLines(LONG val);
    void SetMinLines(LONG val);
    void SetUndoLimit(LONG val);

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

    void SetInsetRect(suic::Rect prc);
    void SetClientRect(suic::Rect rect);

    suic::Rect GetInsetRect() const { return _rcInset; }
    suic::Rect GetClientRect() const { return _rcClnt; }

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

    void SetTextMode(int iMode);
    int GetTextMode() const;

    void ModifyStyle(DWORD dwStyle, BOOL bAdd);
    void SetStyle(DWORD dwStyle);
    BOOL HasStyle(DWORD dwStyle) const;

    //----------------------------------------------

    IScrollInfo* GetScrollInfo()
    {
        return &_scrollInfo;
    }

    void ScrollToVPos(int iPos)
    {
        WPARAM wp = MAKEWPARAM(SB_THUMBPOSITION, iPos);
        TxSendMessage(WM_VSCROLL, wp, 0L);
    }

    void ScrollToHPos(int iPos)
    {
        WPARAM wp = MAKEWPARAM(SB_THUMBPOSITION, iPos);
        TxSendMessage(WM_HSCROLL, wp, 0L);
    }

    suic::Rect GetClientRect()
    {
        return _rcClnt;
    }

    //----------------------------------------------
    void AddImage(LPCTSTR filepath);
    bool InsertEmbbedObj(IOleObject* embbed, DWORD dwUserData);

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

    void InvalidateScrollInfo(const Size* constraint);

    Size OnMeasure(const Size& constraint);
    Size OnArrange(const Size& arrangeSize);

    void OnRender(Drawing * drawing);
    void OnRenderSizeChanged(SizeChangedInfo& e);

    void OnSetCursor(CursorEventArg& e);
    void OnTextInput(KeyboardEventArg& e);
    void OnKeyDown(KeyboardEventArg& e);
    void OnKeyUp(KeyboardEventArg& e);

    void OnGotFocus(RoutedEventArg& e);
    void OnLostFocus(RoutedEventArg& e);

    void OnMouseEnter(MouseButtonEventArg& e);
    void OnMouseMove(MouseButtonEventArg& e);
    void OnMouseLeave(MouseButtonEventArg& e);
    void OnMouseLeftButtonDown(MouseButtonEventArg& e);
    void OnMouseLeftButtonDbclk(MouseButtonEventArg& e);
    void OnMouseLeftButtonUp(MouseButtonEventArg& e);

protected:

    TextBoxBase* _holder;
    ITextServices* _textSrv;
    ITextDocument* _textDoc;

    CaretElement _caret;
    ScrollInfo _scrollInfo;

    // COM对象引用计数
    ULONG _refCount;

    suic::Color _textColor;
    suic::Color _selTextColor;
    suic::Color _selBackground;

    suic::Uint32 _isScrollPending : 1;
    suic::Uint32 _isRegisteredForDrop : 1;
    suic::Uint32 _isTransparent : 1;
    suic::Uint32 _isWordWrap : 1;
    suic::Uint32 _isAllowBeep : 1;
    suic::Uint32 _isSaveSelection : 1;
    suic::Uint32 _isInplaceActive : 1;
    suic::Uint32 _isRichMode : 1;
    suic::Uint32 _isEnableAutoWordSel : 1;
    suic::Uint32 _isAcceptsTab : 1;
    suic::Uint32 _isAcceptsReturn : 1;
    suic::Uint32 _isAutoWordSelection : 1;
    suic::Uint32 _isReadOnlyCaretVisible : 1;
    
    LONG _lMaxLength;
    LONG _lMaxLines;
    LONG _laccelpos;

    suic::Rect _rcClnt;
    suic::Rect _rcInset;

    // 控件风格
    DWORD _dwStyle;
    // 字符格式
    CHARFORMAT2W _cf;
    // 段格式
    PARAFORMAT2 _pf;
    
    // 密码掩码字符
    Char _chPasswordChar;

    friend class TextBoxBase;
};

inline void TextRenderScope::SetSelTextColor(Color clr)
{
    _selTextColor = clr;
    InvalidateVisual();
}

inline void TextRenderScope::SetSelBackground(Color clr)
{
    _selBackground = clr;
    InvalidateVisual();
}

inline void TextRenderScope::SetScrollPending(bool val)
{
    _isScrollPending = val;
}

#endif
