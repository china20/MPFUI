// 华勤科技版权所有 2008-2022
// 
// 文件名：TextHost20.h
// 功  能：实现ITextHost接口
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// ============================================================================

#ifndef _TEXTHOST20_H_
#define _TEXTHOST20_H_

#include <Richedit.h>
#include <RichOle.h>
#include <TOM.h>

#include "RichTextOleCallback.h"

#include <System/Windows/TextInput.h>
#include <Framework/Controls/TextEditor.h>

#include <Extend/Editor/RichTextDef.h>
#include <Extend/Editor/TextContainer.h>

#define Tobool(v) (v ? true : false)
#define ToBOOL(v) (v ? TRUE : FALSE)

namespace suic
{
    class TextContainer;
}

enum eFontChangedFlag
{
    fcfSize = 0x01,
    fcfBkColor = 0x02,
    fcfTextColor = 0x04,
    fcfBold = 0x08,
    fcfItalic = 0x10,
};

class SUIWGX_API TextFontFormat : public suic::TextFont
{
public:

    TextFontFormat();
    ~TextFontFormat();

    void SetTextCb(suic::ITextCallback* textCb)
    {
        _textCb = textCb;
    }

    suic::ITextCallback* GetTextCb()
    {
        return _textCb;
    }

    virtual void Commit();

    virtual bool CanChange()
    {
        return true;
    }

    virtual long GetStyle()
    {
        return 0;
    }

    virtual void SetStyle(long val)
    {
        ;
    }

    virtual long GetAllCaps()
    {
        return 0;
    }

    virtual void SetAllCaps(long val)
    {
        ;
    }

    virtual long GetBackColor()
    {
        return suic::Color::FromRgb(_charf.crBackColor);
    }

    virtual void SetBackColor(long val)
    {
        _flag |= eFontChangedFlag::fcfBkColor;
        _charf.crBackColor = suic::Color::ToRgb(val);
    }

    virtual bool GetBold()
    {
        return false;
    }

    virtual void SetBold(bool bBold)
    {
        _flag |= eFontChangedFlag::fcfBold;
    }

    virtual long GetTextColor()
    {
        return suic::Color::FromRgb(_charf.crTextColor);
    }

    virtual void SetTextColor(long val)
    {
        _flag |= eFontChangedFlag::fcfTextColor;
        _charf.crTextColor = suic::Color::ToRgb(val);
    }

    virtual bool GetHidden()
    {
        return false;
    }

    virtual void SetHidden(bool hidden)
    {
        ;
    }

    virtual bool GetItalic()
    {
        return false;
    }

    virtual void SetItalic(bool val)
    {
        _flag |= eFontChangedFlag::fcfItalic;
        //_charf.bRevAuthor;
    }

    virtual long GetLanguageId()
    {
        return 0;
    }

    virtual void SetLanguageId(long val)
    {
        ;
    }

    virtual suic::String GetName()
    {
        suic::String name(_charf.szFaceName);
        return name;
    }

    virtual void SetName(suic::String name)
    {
        wcscpy(_charf.szFaceName, name.c_str());
    }

    virtual long GetOutline()
    {
        return 0;
    }

    virtual void SetOutline(long val)
    {
        
    }

    virtual bool GetProtected()
    {
        return 0;
    }

    virtual void SetProtected(bool val)
    {
    }

    virtual long GetShadow()
    {
        return 0;
    }
    virtual void SetShadow(long val)
    {
    }

    virtual float GetSize();
    virtual void SetSize(float size);

    virtual float GetSpacing()
    {
        return _charf.sSpacing;
    }
    virtual void SetSpacing(float val)
    {
        _charf.sSpacing = val;
    }

    virtual bool GetUnderline()
    {
        return _charf.bUnderlineType != 0;
    }
    virtual void SetUnderline(bool val)
    {
        _charf.bUnderlineType = val;
    }

    virtual long GetWeight()
    {
        return _charf.wWeight;
    }

    virtual void SetWeight(long val)
    {
        _charf.wWeight = val;
    }

private:

    int _flag;
    CHARFORMAT2W _charf;
    suic::ITextCallback* _textCb;
    ITextServices* _textSrv;

    friend class TextRenderHost;
};

class TextParaFormat : public suic::TextPara
{
public:

    TextParaFormat();
    ~TextParaFormat();

    virtual void Begin();
    virtual void Commit();

    virtual bool IsEqual(suic::TextPara *pPara)
    {
        return false;
    }

    virtual long GetStyle()
    {
        return 0;
    }

    virtual void SetStyle(long value)
    {
    }

    virtual suic::TPAlign GetAlignment()
    {
        return (suic::TPAlign)_paraf.wAlignment;
    }

    virtual void SetAlignment(suic::TPAlign value)
    {
        _paraf.wAlignment = value;
    }

    virtual long GetLineSpace();
    virtual suic::TPLineRule GetLineRule();
    virtual void SetLineSpace(suic::TPLineRule lineRule, long lineSpace);

    virtual long GetListAlignment()
    {
        return 0;
    }

    virtual void SetListAlignment(long value)
    {
    }

    virtual long GetListLevelIndex()
    {
        return 0;
    }

    virtual void SetListLevelIndex(long value)
    {
    }

    virtual long GetListStart()
    {
        return _paraf.wNumberingStart;
    }

    virtual void SetListStart(long value)
    {
        _paraf.wNumberingStart = value;
    }

    virtual long GetListTab();
    virtual void SetListTab(long value);

    virtual long GetListType()
    {
        return _paraf.wNumberingStyle;
    }

    virtual void SetListType(long value)
    {
        _paraf.wNumberingStyle = value;
    }

    virtual long GetNoLineNumber()
    {
        return _paraf.wNumbering;
    }

    virtual void SetNoLineNumber(long value)
    {
        _paraf.wNumbering = value;
    }

    virtual long GetRightIndent();
    virtual void SetRightIndent(long value);
    virtual long GetFirstLineIndent();
    virtual long GetLeftIndent();

    virtual void SetIndents(long startIndent, long leftIndent, long rightIndent);

    virtual long GetSpaceAfter();
    virtual void SetSpaceAfter(long value);

    virtual long GetSpaceBefore();
    virtual void SetSpaceBefore(long value);

private:

    void AddParaMask(DWORD dwMask);
    void InitParaFormat(DWORD dwMask);

private:

    PARAFORMAT2 _paraf;
    ITextServices* _textSrv;

    friend class TextRenderHost;
};

class TextRenderHost : public ITextHostEx, public suic::ITextRender
{
public:

    TextRenderHost();
    ~TextRenderHost(void);

    BOOL InitializeTextService(suic::TextContainer* holder);

    ITextServicesEx* GetTextService() const
    {
        return _textSrv;
    }

    ITextDocument* GetTextDocument() const
    {
        return _textDoc;
    }

    suic::Element* GetHolder() const
    {
        return (suic::Element*)_holder;
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

    suic::String GetText();
    void SetText(const suic::String& strText);

    CHARFORMAT2W& GetCF()
    {
        return _cf._charf;
    }

    int GetFontSize()
    {
        return _cf.GetSize();
    }

    suic::TextFont* GetTextFont()
    {
        return &_cf;
    }

    PARAFORMAT2& GetPF()
    {
        return _pf._paraf;
    }

    suic::TextPara* GetTextPara()
    {
        return &_pf;
    }

    void EnableImmStatus(bool bEnable);

    //-----------------------------------------------------------

    void InitializeRichMode();

    bool IsScrollPending() const { return _isScrollPending; }
    void SetScrollPending(bool val);

    IRichEditOle* GetRichEditOle();
    bool SetRichEditOleCallback(IRichEditOleCallback* callback);

    void TxRender(HDC hdc, suic::Rect rcClip, suic::Point offset);

    LRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam) const;
    HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& lr) const;

    LRESULT InternalSendMessage(UINT msg) const;

    //-----------------------------------------------------------
    // IOleObject操作
    
    int GetOleObjCount();

    bool GetOleObjOnCp(REOBJECT* preobj);
    bool GetOleObjOnSel(REOBJECT* preobj);
    bool GetOleObjOnIndex(LONG index, REOBJECT* preobj);

public:

    void SetTextCallback(suic::ITextCallback* textCb);

    DWORD GetEventMask() const;
    DWORD SetEventMask(DWORD dwEventMask);

    int GetLineIndexFromCharIndex(int charIndex) const;
    int GetCharacterIndexFromLineIndex(int lineIndex) const;
    int GetCharIndexFromPoint(suic::Point point, bool snapToText) const;
    suic::Point GetPointFromCharIndex(int charIndex) const;
    int GetFirstVisibleLineIndex() const;
    int GetLastVisibleLineIndex() const;
    int GetLineCount() const;
    int GetLineLength(int lineIndex) const;
    
    long GetTextLength() const;

    suic::String GetLineText(int lineIndex) const;

    void SetLineLeftOffset(int index, int xLeft);
    void GetLineDetail(int index, suic::LineDetail* info, int* iYPos);
    
    suic::Point GetPoint(suic::Uint32 cp, bool bLeftTop);

    bool GetWordWrap(void) const { return _isWordWrap; }
    void SetWordWrap(bool fWordWrap);

    bool IsRichMode() const;

    bool GetMultiLine(void) { return _isMultiLine; }
    void SetMultiLine(bool fMultiLine);

    bool GetReadOnly() const;
    void SetReadOnly(bool fReadOnly);

    bool CanEnableDrag() const;
    void SetEnableDrag(bool fCanDrag);

    bool GetAcceptsReturn();
    void SetAcceptsReturn(bool val);

    bool GetAutoWordSelection();
    void SetAutoWordSelection(bool val);

    bool GetAcceptsTab();
    void SetAcceptsTab(bool val);

    bool GetIsReadOnlyCaretVisible();
    void SetIsReadOnlyCaretVisible(bool val);

    void SetFont(HFONT hFont);

    suic::Color GetTextColor();
    suic::String GetFontFamily();
    int GetFontWeight();

    void SetTextColor(suic::Color clr);
    void SetFontFamily(suic::String name);
    void SetFontWeight(int weight);
    void SetFontSize(int size);
    void SetFontItalic(bool val);
    void SetUnderline(bool val);
    void SetStrikeout(bool val);

    void SetSelTextColor(suic::Color clr);
    void SetSelBackground(suic::Color clr);

    SIZEL GetExtent();

    void NotifyExtentChange();
    void NotifyAllChange() ;

    void SetMaxLength(int nChars);
    int GetMaxLength() const;

    void SetMaxLines(LONG val);
    LONG GetMaxLines() const;

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

    void SetTransparent(BOOL fTransparent);
    BOOL IsTransparent() const { return _isTransparent; }

    void SetUsePassword(bool fUsePassword);

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

    bool IsHoriScrollBarVisible() const 
    {
        return _isHoriScrollBarVisible;
    }
   
    bool IsVertScrollBarVisible() const 
    {
        return _isVertScrollBarVisible;
    }

    void SetTextMode(int iMode);
    int GetTextMode() const;

    bool GetFocused() const
    {
        return _isFocused;
    }

    //----------------------------------------------

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

    void UpdateHostUI();

    //----------------------------------------------

    bool InsertEmbbedObj(IOleObject* embbed, DWORD dwUserData);

public:

    // 实现IRenderHost接口
    virtual void Render(suic::Drawing* drawing);

    virtual suic::Rect GetViewRect() const;
    virtual void SetClntRect(suic::Rect val);

    virtual suic::Rect GetPadding() const;
    virtual void SetPadding(suic::Rect val);

    virtual void RefleshView();

    virtual bool DoMessage(suic::Uint32 message, suic::Uint32 wp, suic::Uint32 lp, suic::Uint64* lr);

public:

    HWND GetHostHwnd();
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

    //Tbl:新增接口
    virtual HRESULT     TxObjectWrapping(BOOL& val);
    HRESULT             TxInsertOleObject(LPDATAOBJECT pdo, STGMEDIUM* pStdMedium, FORMATETC* fetc, REOBJECT* reobj);

    void DoVertScrollBarShow(BOOL fShow);

protected:

    void OnTxPropertyBitsChange(UINT key, UINT val);
    void RegisterDragDrop(void);
    void RevokeDragDrop(void);
    bool GetOleImageRect(const REOBJECT& reo, IRichEditOle *pRichEditOle, RECT* lpRect);

protected:

    suic::TextContainer* _holder;
    ITextServicesEx* _textSrv;
    ITextDocument* _textDoc;

    // 字符格式
    TextFontFormat _cf;
    // 段格式
    TextParaFormat _pf;

    // COM对象引用计数
    ULONG _refCount;

    HWND _hwnd;
    HIMC _himc;

    suic::Color _textColor;
    suic::Color _selTextColor;
    suic::Color _selBackground;

    suic::Uint32 _isFocused : 1;
    suic::Uint32 _isScrollPending : 1;
    suic::Uint32 _isRegisteredForDrop : 1;
    suic::Uint32 _isTransparent : 1;
    suic::Uint32 _isWordWrap : 1;
    suic::Uint32 _isMultiLine : 1;
    suic::Uint32 _isReadOnly : 1;
    suic::Uint32 _isAllowBeep : 1;
    suic::Uint32 _isSaveSelection : 1;
    suic::Uint32 _isHideSelection : 1;
    suic::Uint32 _isInplaceActive : 1;
    suic::Uint32 _isRichMode : 1;
    suic::Uint32 _isUsePassword : 1;
    suic::Uint32 _isEnableAutoWordSel : 1;
    suic::Uint32 _isAcceptsTab : 1;
    suic::Uint32 _isAcceptsReturn : 1;
    suic::Uint32 _isAutoWordSelection : 1;
    suic::Uint32 _isReadOnlyCaretVisible : 1;
    suic::Uint32 _isHoriScrollBarVisible : 1;
    suic::Uint32 _isVertScrollBarVisible : 1;
    suic::Uint32 _isRenderPending : 1;
    suic::Uint32 _isRecacViewPending : 1;
    suic::Uint32 _isUpdateTextPending : 1;
    suic::Uint32 _isCanDrag : 1;
    
    LONG _lMaxLength;
    LONG _lMaxLines;
    LONG _laccelpos;

    suic::Rect _rcClnt;
    suic::Rect _rcInset;
    
    // 密码掩码字符
    suic::Char _chPasswordChar;

    friend class suic::TextBoxBase;
    friend class suic::TextContainer;
};

inline void TextRenderHost::SetScrollPending(bool val)
{
    _isScrollPending = val;
}

#endif
