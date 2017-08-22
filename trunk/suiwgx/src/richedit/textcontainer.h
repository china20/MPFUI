// 华勤科技版权所有 2010-2011
// 
// 文件名：TextContainer.h
// 功  能：居于TextContainer实现标准的富文本控件
// 
// 作  者：Sharpui开发组
// 时  间：2010-08-31
// 
// ============================================================================

#ifndef _UITEXTCONTAINER_H_
#define _UITEXTCONTAINER_H_

#include "TextHost20.h"
#include <Extend/RichText/RicheditDoc.h>

namespace suic
{

struct SUICORE_API CharFormat
{
    int size;
    int weight;
    suic::Byte italic;
    suic::Byte underline;
    suic::Color color;
    suic::Char facename[32];

    CharFormat();
};

class TextDoc : public ITextDoc
{
public:

    TextDoc(ITextDocument* pDoc);
    virtual TextDoc();

    ITextDocument2* GetTextDoc() const
    {
        return _textDoc;
    }

private:

    ITextDocument* _textDoc;
};

/// <summary>
///  TextContainer，实现标准的RichEdit。
/// </summary>
class SUICORE_API TextContainer : public ITextContainer
{
public:

    RTTIOfClass(TextContainer)

    TextContainer();
    virtual ~TextContainer();

    ITextDoc* GetTextDoc()
    {
        return NULL;
    }

    TextHostService* GetTextHost()
    {
        return _richTxt;
    }

    //--------------------------------- 文本操作

    long GetTextSize(DWORD dwFlags) const;
    suic::String GetText() const;
    void SetText(const suic::String& strText);

    int GetSelLength() const;
    suic::String GetSelText() const;
    void ReplaceSel(const suic::String& strText, bool bCanUndo);

    suic::String GetTextRange(long nStartChar, long nEndChar) const;

    int AddText(const suic::String& strText);
    int AddText(const suic::String& strText, const CharFormat& cf);
    int InsertText(long index, const suic::String& strText);
    int InsertText(long index, const suic::String& strText, const CharFormat& cf);
    void ApplyTextFormat(long lStart, long lEnd, const CharFormat& cf);

    bool Redo();
    bool Undo();
    void Copy();
    void Cut();
    void Paste();

    //--------------------------------- 行操作
    int GetLineCount() const;
    int GetLineLength(int line) const;
    suic::String GetLineText(int index, int iMaxSize) const;

    int LineFromCharIndex(int ch) const;
    int CharIndexFromLine(int line) const;

    suic::Point PosFromCharIndex(int ch) const;
    int CharIndexFromPos(suic::Point pt) const;

    void Clear();
    void ClearUndoCache();

    int SetUndoLimit(int limit);

    bool IsReadOnly() const;
    bool IsRichMode() const;

    void SetReadOnly(bool val);
    void SetRichMode(bool val);

    bool IsWordWrap() const;
    void SetWordWrap(bool bWordWrap);

    bool IsMultiLine() const;
    void SetMultiLine(bool bMulti);

    bool IsPassword() const;
    void SetPassword(bool bPassword);
    void SetPasswordChar(suic::Char ch);

    bool IsModified() const;
    void SetModified(bool bModified) const;

    bool IsAutoURLDetect() const;
    bool SetAutoURLDetect(bool bAutoDetect);

    bool SetZoom(int nNum, int nDen);
    bool SetZoomOff();

    int Select(long lStart, long lEnd);
    int SelectAll();
    int SelectNone();
    void HideSelection(bool bHide, bool bChangeStyle);

    //--------------------------------- 文本格式设置
    void SetTextColor(suic::Color dwTextColor);

    void GetTextFormat(CharFormat& cf) const;
    void SetTextFormat(const CharFormat& cf);

    void GetSelTextFormat(CharFormat& cf) const;
    void SetSelTextFormat(const CharFormat& cf);

    //--------------------------------- 段落操作

    /// <summary>
    ///  设置做缩进距离
    /// </summary>
    /// <param name="indent">缩进距离</param>
    /// <returns>无</returns>
    void SetLeftIndent(int indent);

    //-------------------------------------图像操作
    void AddImage(const suic::String& filepath);
    bool PointInObject(suic::Point pt);

    void SetCaretPos(int x, int y);
    void ShowCaret(bool fShow);
    void CreateCaret(int xWidth, int yHeight);

    suic::Rect GetClientRect() const;

protected:

    void InternalSendMessage();

    LRESULT TXSendMessage(UINT msg, WPARAM wp, LPARAM lp)
    {
        LRESULT lResult = 0;
        _richTxt->TxSendMessage(msg, wp, lp, &lResult);
        return lResult;
    }

protected:

    TextHostService* _richTxt;
    suic::CaretElement _caret;
};

}

#endif
