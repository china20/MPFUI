// 华勤科技版权所有 2010-2011
// 
// 文件名：TextEditDoc.h
// 功  能：编辑框实现逻辑。
// 
// 作  者：Sharpui开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UIEDITDOC_H_
#define _UIEDITDOC_H_

#include <Framework/Controls/Control.h>
#include <System/Graphics/Resource.h>
#include <System/Tools/Array.h>

namespace suic
{

class TextSimple;

class SUICORE_API EditCache
{
public:

    Byte * _buff;
    Uint32 _size;
    Uint32 _maxsize;

    EditCache() 
        :_buff(NULL)
        , _size(0)
        , _maxsize(1024 * 1024) 
    {
    }

    void insert(Byte * p, Uint32 len, Uint32 pos)
    {
    }

    void remove(Uint32 pos, Uint32 len)
    {
    }

    inline Byte at(Uint32 index)
    {
        return *(_buff + index);
    }
};

typedef struct tagLineItem
{
    // 一行中开始偏移位置
    int _nBeg;
    // 一行中结束偏移位置
    int _nEnd;

    Rect _rect;
    short _hf;
    short _clrText;
    short _clrSelText;
    short _clrSelTextBk;
}LineItem;

class SUICORE_API TextBoxLine
{
public:

    TextBoxLine();
    ~TextBoxLine();

    void Draw(TextSimple* owner, Drawing * drawing, FormattedText* paint, fRect * lprc);
    void DrawSel(TextSimple* owner, Drawing * drawing, FormattedText* paint, Color selBk, Color selClr, fRect * lprc, Uint32 beg, Uint32 end);

    void GetText(String & str);
    void GetSelText(String & str, int beg, int len);

    void Add(TextSimple* owner, Char ch);
    void Add(TextSimple* owner, const String & str);
    void Add(TextSimple* owner, const Char * str, int len);

    void AddLine(TextSimple* owner, const TextBoxLine* line);
    void AddLine(TextSimple* owner, const TextBoxLine* line, int beg, int end);

    void Insert(TextSimple* owner, Char ch, int pos);
    void Insert(TextSimple* owner, const String & str, int pos);

    void Remove(TextSimple* owner);
    void Remove(TextSimple* owner, int beg, int end);

    /// <summary>
    ///     选择文本
    /// </summary>
    /// <remarks>
    ///     此方法将从beg到end进行文本选取。
    /// </remarks>
    /// <param name="beg">
    ///     开始位置
    /// </param>
    /// <param name="end">
    ///     结束位置
    /// </param>
    /// <returns>无</returns>
    void SelectWord(int& beg, int & end);
    Uint32 GetCount();

    Uint32 GetItemCount();
    void GetItem(Uint32 index, LineItem & item);

    Rect GetRect() const;
    int PointIn(TextSimple* owner, Point* lppt);

    int Offset(TextSimple* owner, int index);
    int GetLineWid() const;
    int GetLineHei() const;

    void RefleshRect(TextSimple* owner);
    Size MeasureText(TextSimple* owner);

protected:

    Size MeasureText(TextSimple* owner, Char* buff, int len);
    Size MeasureText(TextSimple* owner, FormattedText* paint, Char* buff, int len);
    void DrawLinePart(TextSimple* owner, Drawing* drawing, FormattedText* paint, const Char* buff, int len, const fRect* lprc);

protected:

    Rect _rcLine;
    String _buff;
    Array<LineItem> _items;
};

inline int TextBoxLine::GetLineWid() const
{ 
    return _rcLine.right - _rcLine.left; 
}

inline int TextBoxLine::GetLineHei() const
{ 
    return _rcLine.bottom - _rcLine.top; 
}

class SUICORE_API TextBoxDoc : public Object
{
public:

    TextBoxDoc();
    virtual ~TextBoxDoc();

    void SetOwner(TextSimple* owner);
    void InitDocument(TextSimple* owner);

    void Cut();
    void Copy();
    void Paste();
    void SelectAll();

    void RefleshCaret();

    void DrawSingleLine(TextSimple* owner, Drawing * drawing, FormattedText* paint, Color selBk, Color selClr, fRect * lprc);
    void Draw(TextSimple* owner, Drawing * drawing, FormattedText* paint, Color selBk, Color selClr, fRect * lprc);

    void SetVisualRect(Rect * lprc);
    Rect GetVisualRect() const;

    int GetLineHeight();

    void Add(const String & text);
    void Add(const Char * text, Uint32 len);
    void Add(Char ch);

    void GetSelectText(String & text);
    void GetText(String & text) const;

    bool SetCaret(Point * lppt);
    void TrackCaret(Point * lppt);
    bool SelectWord(Point * lppt);

    Rect GetCaret(bool bUpDown);

    bool OnLineHome(bool bSel);
    bool OnLineEnd(bool bSel);

    bool OnPageNext(bool bSel);
    bool OnPagePrior(bool bSel);

    // 移动光标到可视范围开始
    bool OnVisualHome(bool bSel);
    bool OnVisualEnd(bool bSel);

    bool OnDocHome(bool bSel);
    bool OnDocEnd(bool bSel);

    bool OnLeftCaret(bool bSel);
    bool OnUpCaret(bool bSel);
    bool OnRightCaret(bool bSel);
    bool OnDownCaret(bool bSel);

    void CancelSelect();

    // 如果有选中的返回true，否则false
    bool DeleteSelect();
    void ReplaceSelect(const String & text);

    // 在光标处插入一行
    void InsertLine();

    void DelLeftOne();
    void DelRightOne();

    // 是否有选中
    bool IsSelectMode();
    bool HasContent() const;

    Size Measure() const;

    int GetHorizontalOffset() const;
    void SetHorizontalOffset(int val);

    int GetVerticalOffset() const;
    void SetVerticalOffset(int val);

protected:

    void DeleteLine(int index);
    void DeleteLineRange(int beg, int end);
    void SetCaretOffset(int val);
    void SetSelInfo(bool bSel, int * & nOffset, int * & nLine);
    bool NotifyCaretPos(int iLine);
    void CalcTextRect(Rect * lprc, int nLine);
    bool CalcSelectPos(int & bline, int & bl, int & br, int & eline, int & el, int & er);

protected:

    TextSimple* _owner;
    Array<TextBoxLine*> _lines;

    Rect _rcVisual;

    int _caretLine;
    int _caretOffset;
    int _caretPreOffset;

    int _trackLine;
    int _trackOffset;

    int _horizontalOffset;
    int _verticalOffset;
};

inline Rect suic::TextBoxDoc::GetVisualRect() const
{
    return _rcVisual;
}

}

#endif
