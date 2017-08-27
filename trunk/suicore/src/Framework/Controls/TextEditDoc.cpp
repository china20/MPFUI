// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// Sharpui界面库允许任何个人或团体免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////////
// TextEditDoc.cpp

#include <Framework/Controls/TextEditDoc.h>
#include <Framework/Controls/TextSimple.h>

#include <assert.h>

namespace suic
{

/////////////////////////////////////////////////////////////////
// GlyphFactory

static HFONT DefFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);

static Uint32 DefTextColor = ARGB(255,0,0,0);
static Uint32 DefInactivateSelTextBkColor = ARGB(255,191,205,219);
static Uint32 DefSelTextBkColor = ARGB(255,51,153,255);
static Uint32 DefSelTextColor = ARGB(255,20,20,255);
static Uint32 DefInactivateSelTextColor = ARGB(255,67,78,84);

/////////////////////////////////////////////////////////////////
// TextBoxLine
TextBoxLine::TextBoxLine()
{
    //_rcLine.bottom = GlyphFactory::GetAveCharHei();
}

TextBoxLine::~TextBoxLine()
{
}

void TextBoxLine::Draw(TextSimple* owner, Drawing * drawing, FormattedText* paint, fRect * lprc)
{
    if (_buff.Length() > 0)
    {
        DrawLinePart(owner, drawing, paint, _buff.c_str(), _buff.Length(), lprc);
    }
}

void TextBoxLine::DrawLinePart(TextSimple* owner, Drawing* drawing, FormattedText* paint, const Char* buff, int len, const fRect* lprc)
{
    Char ch = owner->GetPasswordChar();

    if (ch == _U(''))
    {
        drawing->DrawString(paint, buff, len, lprc);
    }
    else
    {
        String strText(ch, len);

        drawing->DrawString(paint, strText.c_str(), len, lprc);
    }
}

Size TextBoxLine::MeasureText(TextSimple* owner, FormattedText* paint, Char* buff, int len)
{
    TmParam tm;
    Char ch = owner->GetPasswordChar();

    if (ch == _U(''))
    {
        paint->MeasureString(tm, buff, len);
    }
    else
    {
        paint->MeasureString(tm, &ch, 1);
        tm.size.cx *= len;
    }

    tm.size.cx += 1;
    return tm.size.ToSize();
}

Size TextBoxLine::MeasureText(TextSimple* owner, Char* buff, int len)
{
    FormattedText paint;
    owner->InitFormattedText(&paint);
    return MeasureText(owner, &paint, buff, len);
}

Size TextBoxLine::MeasureText(TextSimple* owner)
{
    return MeasureText(owner, _buff.c_str(), _buff.Length());
}

void TextBoxLine::DrawSel(TextSimple* owner, Drawing* drawing, FormattedText* p, Color selBk, Color selClr, fRect* lprc, Uint32 beg, Uint32 end)
{
    if (_buff.Length() > 0)
    {
        Size sz;
        Color oldClr = p->GetColor();

        if (end == -1)
        {
            drawing->FillRect(selBk, lprc);
            p->SetColor(selClr);
            DrawLinePart(owner, drawing, p, _buff.c_str(), _buff.Length(), lprc);
            p->SetColor(oldClr);

            return;
        }

        if (beg > 0)
        {
            sz = MeasureText(owner, p, _buff.c_str(), beg);

            int nRight = lprc->right;
            lprc->right = lprc->left + sz.cx;

            DrawLinePart(owner, drawing, p, _buff.c_str(), beg, lprc);

            lprc->left = lprc->right;
            lprc->right = nRight;
        }

        sz = MeasureText(owner, p, _buff.c_str() + beg, end - beg);

        int nRight = lprc->right;
        lprc->right = lprc->left + sz.cx;

        p->SetColor(selClr);

        drawing->FillRect(selBk, lprc);
        DrawLinePart(owner, drawing, p, _buff.c_str() + beg, (int)(end - beg), lprc);
        lprc->left = lprc->right;
        lprc->right = nRight;

        if (end < (Uint32)_buff.Length())
        {
            p->SetColor(oldClr);
            sz = MeasureText(owner, p, _buff.c_str() + end, (int)_buff.Length() - end);
            DrawLinePart(owner, drawing, p, _buff.c_str() + end, (int)(_buff.Length() - end), lprc);  
        }
        else
        {
            p->SetColor(oldClr);
        }
    }
}

void TextBoxLine::GetText(String & str)
{
    str += _buff;
}

void TextBoxLine::GetSelText(String & str, int beg, int len)
{
    str += _buff.Substring(beg, len);
}

void TextBoxLine::Add(TextSimple* owner, Char ch)
{
    _buff += String(ch, 1);
    RefleshRect(owner);
}

void TextBoxLine::Add(TextSimple* owner, const String & str)
{
    _buff += str;
    RefleshRect(owner);
}

void TextBoxLine::Add(TextSimple* owner, const Char * str, int len)
{
    String tmp(str, len);

    _buff += tmp;
    RefleshRect(owner);
}

void TextBoxLine::AddLine(TextSimple* owner, const TextBoxLine* line)
{
    _buff += line->_buff;
    RefleshRect(owner);
}

void TextBoxLine::AddLine(TextSimple* owner, const TextBoxLine* line, int beg, int end)
{
    if (end == -1)
    {
        end = (int)line->_buff.Length();
    }

    String tmp(line->_buff.c_str() + beg, end - beg);

    _buff += tmp;
    RefleshRect(owner);
}

void TextBoxLine::Insert(TextSimple* owner, Char ch, int pos)
{
    String tmp(_buff.c_str(), pos);
    String rTmp(_buff.c_str() + pos, _buff.Length() - pos);

    tmp += String(ch, 1);
    tmp += rTmp;

    _buff = tmp;

    RefleshRect(owner);
}

void TextBoxLine::Insert(TextSimple* owner, const String & str, int pos)
{
    String tmp(_buff.c_str(), pos);
    String rTmp(_buff.c_str() + pos, _buff.Length() - pos);

    tmp += str;
    tmp += rTmp;

    _buff = tmp;

    RefleshRect(owner);
}

void TextBoxLine::Remove(TextSimple* owner)
{
    _buff = String();
    RefleshRect(owner);
}

void TextBoxLine::Remove(TextSimple* owner, int beg, int end)
{
    if (beg < 0) beg = 0;
    if (end < 0) end = (int)_buff.Length();
    if (beg >= end) return;

    String tmp(_buff.Substring(end, _buff.Length() - end));

    _buff = _buff.Substring(0, beg);
    _buff += tmp;

    RefleshRect(owner);
}

static int GetCharType(int ch)
{
    if (ch == '\r' || ch == '\n')
    {
        return 1;
    }
    // 空格
    else if (ch < 0x20 || ch == ' ')
    {
        return 2;
    }
    else if (ch >= 0x80 || isalnum(ch) || ch == '_')
    {
        return 3;
    }
    else
    {
        return 4;
    }
}

void TextBoxLine::SelectWord(int & beg, int & end)
{
    int nPos = beg;
    if (nPos == (int)_buff.Length())
    {
        nPos = (int)_buff.Length() - 1;
    }

    int ch = (int)_buff[nPos];
    int npt = GetCharType(ch);

    beg = nPos;
    end = nPos + 1;

    int i = 0;
    for (i = nPos; i < (int)_buff.Length(); ++i)
    {
        ch = (int)_buff[i];
        if (GetCharType(ch) != npt)
        {
            end = i;
            break;
        }
    }
    end = i;

    for (i = nPos; i >= 0; --i)
    {
        ch = (int)_buff[i];

        if (GetCharType(ch) != npt)
        {
            beg = i + 1;
            break;
        }
    }
    if (-1 == i)
    {
        beg = 0;
    }
}

Uint32 TextBoxLine::GetCount()
{
    return static_cast<Uint32>(_buff.Length());
}

Uint32 TextBoxLine::GetItemCount()
{
    return static_cast<Uint32>(_items.Length());
}

void TextBoxLine::GetItem(Uint32 index, LineItem & item)
{
    item = _items[index];
}

Rect TextBoxLine::GetRect() const
{
    return _rcLine;
}

int TextBoxLine::PointIn(TextSimple* owner, Point * lppt)
{
    if (_buff.Length() == 0) 
    {
        return 0;
    }

    int i = 0;
    int iPos = 0;
    int iOffset = 0;
    FormattedText paint;

    iPos = _buff.Length();
    owner->InitFormattedText(&paint);

    for (i = 0; i < _buff.Length(); ++i)
    {
        Size size = MeasureText(owner, &paint, _buff.c_str() + i, 1);

        iOffset += size.cx;

        if (iOffset > lppt->x)
        {
            iPos = i;

            if ((iOffset - size.cx / 2) < lppt->x)
            {
                iPos += 1;
            }

            break;
        }
    }

    return iPos;
}

int TextBoxLine::Offset(TextSimple* owner, int index)
{
    if (_buff.Length() == 0 || index <= 0) 
    {
        return 0;
    }

    Size size = MeasureText(owner, _buff.c_str(), index);

    return size.cx;
}

void TextBoxLine::RefleshRect(TextSimple* owner)
{
    Size size;
    Char ch = owner->GetPasswordChar();

    if (_buff.Length() > 0)
    {
        size = MeasureText(owner, _buff.c_str(), (int)_buff.Length());
    }
    else
    {
        size = MeasureText(owner, _U("W"), 1);
        size.cx = 0;
    }

    _rcLine.top = 0;
    _rcLine.left = 0;
    _rcLine.right = size.cx;
    _rcLine.bottom = size.cy;
}

/////////////////////////////////////////////////////////////////
// EditDoc
//
TextBoxDoc::TextBoxDoc()
    : _caretLine(-1)
    , _caretOffset(0)
    , _caretPreOffset(0)
    , _trackLine(-1)
    , _trackOffset(-1)
    , _owner(NULL)
    , _horizontalOffset(0)
    , _verticalOffset(0)
{
    InsertLine();
}

TextBoxDoc::~TextBoxDoc()
{
    DeleteLineRange(0, _lines.Length() - 1);
}

void TextBoxDoc::SetOwner(TextSimple* owner)
{
    _owner = owner;
    _lines[0]->RefleshRect(_owner);
}

void TextBoxDoc::InitDocument(TextSimple* owner)
{
    _owner = owner;
    _lines[0]->RefleshRect(_owner);
    RefleshCaret();
}

void TextBoxDoc::SetVisualRect(Rect * lprc)
{
    _rcVisual.right = lprc->Width();
    _rcVisual.bottom = lprc->Height();
}

void TextBoxDoc::Cut()
{
}

void TextBoxDoc::Copy()
{
}

void TextBoxDoc::Paste()
{
}

void TextBoxDoc::DeleteLine(int index)
{
    TextBoxLine* pLine = _lines[index];
    delete pLine;
    _lines.RemoveAt(index);
}

void TextBoxDoc::DeleteLineRange(int beg, int end)
{
    if (end >= _lines.Length())
    {
        end = _lines.Length() - 1;
    }

    if (end >= beg)
    {
        for (int i = beg; i <= end; ++i)
        {
            delete _lines[i];
        }
        _lines.RemoveRange(beg, end - beg + 1);
    }
}

void TextBoxDoc::SetCaretOffset(int val)
{
    _caretOffset = val;
    _caretPreOffset = val;
}

void TextBoxDoc::SelectAll()
{
    _caretLine = 0;
    SetCaretOffset(0);
    _trackLine = (int)_lines.Length() - 1;
    _trackOffset = _lines[_trackLine]->GetCount();

    NotifyCaretPos(0);
}

int TextBoxDoc::GetLineHeight()
{
    Rect rcdr = _lines[0]->GetRect();
    return rcdr.Height();
}

void TextBoxDoc::DrawSingleLine(TextSimple* owner, Drawing * drawing, FormattedText* paint, Color selBk, Color selClr, fRect * lprc)
{
    fRect rc;
    int i = 0;
    rc.top = 0;
    int by = _verticalOffset;
    int ey = _rcVisual.bottom;

    int bline=-1, bl=0, br=0, eline=-1, el=0, er=0;
    bool bSel = CalcSelectPos(bline, bl, br, eline, el, er);

    int iLineHei = _lines[i]->GetLineHei();

    //rc.top += (_rcVisual.Height() - iLineHei) / 2;
    rc.bottom = rc.top + iLineHei;

    if (_lines.Length() > 0)
    {
        if (rc.bottom >= by)
        {
            rc.right = _lines[i]->GetLineWid();
            rc.Offset(lprc->left - _horizontalOffset, lprc->top - _verticalOffset);

            if (!bSel)
            {
                _lines[i]->Draw(owner, drawing, paint, &rc);
            }
            else if (bline == i)
            {
                _lines[i]->DrawSel(owner, drawing, paint, selBk, selClr, &rc, bl, br);
            }
            else if (eline == i)
            {
                _lines[i]->DrawSel(owner, drawing, paint, selBk, selClr, &rc, el, er);
            }
            else if (i > bline && i < eline)
            {
                _lines[i]->DrawSel(owner, drawing, paint, selBk, selClr, &rc, 0, -1);
            }
            else
            {
                _lines[i]->Draw(owner, drawing, paint, &rc);
            }
        }
    }
}

void TextBoxDoc::Draw(TextSimple* owner, Drawing * drawing, FormattedText* paint, Color selBk, Color selClr, fRect * lprc)
{
    fRect rc;
    int iLineHei = _lines[0]->GetLineHei();
    int by = _verticalOffset;
    int ey = _rcVisual.bottom + _verticalOffset;

    int bline=-1, bl=0, br=0, eline=-1, el=0, er=0;
    bool bSel = CalcSelectPos(bline, bl, br, eline, el, er);

    for (int i = 0; i < (int)_lines.Length(); ++i)
    {
        if (rc.top > ey) 
        {
            break;
        }

        rc.bottom = rc.top + iLineHei;

        if (rc.bottom >= by)
        {
            rc.right = _lines[i]->GetLineWid();

            fRect rcLine = rc;

            rcLine.Offset(lprc->left - _horizontalOffset, lprc->top - _verticalOffset);

            if (rcLine.right > _rcVisual.left)
            {
                if (!bSel)
                {
                    _lines[i]->Draw(owner, drawing, paint, &rcLine);
                }
                else if (bline == i)
                {
                    _lines[i]->DrawSel(owner, drawing, paint, selBk, selClr, &rcLine, bl, br);
                }
                else if (eline == i)
                {
                    _lines[i]->DrawSel(owner, drawing, paint, selBk, selClr, &rcLine, el, er);
                }
                else if (i > bline && i < eline)
                {
                    _lines[i]->DrawSel(owner, drawing, paint, selBk, selClr, &rcLine, 0, -1);
                }
                else
                {
                    _lines[i]->Draw(owner, drawing, paint, &rcLine);
                }
            }
        }

        rc.top = rc.bottom;
    }
}

void TextBoxDoc::Add(const String & text)
{
    Add(text.c_str(), (Uint32)text.Length());
}

void TextBoxDoc::Add(const Char * text, Uint32 len)
{
    String oldText;

    GetText(oldText);
    DeleteSelect();

    typedef struct tagLineSep
    {
        const Char * beg;
        int len;
    }LineSep;

    Array<LineSep> seps;

    const Char * beg = text;
    Char * cur = const_cast<Char *>(beg);
    Char * end = cur + len;

    int nAllLine = 0;

    while (cur != end)
    {
        if (*cur == L'\n' || *cur == L'\r')
        {
            LineSep sp;
            sp.beg = beg;
            sp.len = (int)(cur - beg);
            seps.Add(sp);
            beg = cur + 1;
            if (*beg == L'\n' || 
                *beg == L'\r')
            {
                ++beg;
                ++cur;
            }
            ++nAllLine;
        }
        else if (*cur == L'\t')
        {
            int iii = 0;
            *cur = L' ';
        }

        ++cur;
    }
    if ((*end) != L'\n')
    {
        LineSep sp;
        sp.beg = beg;
        sp.len = (int)(cur - beg);
        ++nAllLine;
        seps.Add(sp);
    }

    if (seps.Length() > 0)
    {
        String str(seps[0].beg, seps[0].len);

        _lines[_caretLine]->Insert(_owner, str, _caretOffset);
        _caretOffset += seps[0].len;
        SetCaretOffset(_caretOffset);
    }

    if (seps.Length() > 1 && !_owner->GetSingleLine())
    {
        int nInsertPos = _caretLine;
        for (int i = 1; i < (int)seps.Length(); ++i)
        {
            ++_caretLine;
            _lines.Insert(_caretLine, new TextBoxLine());
            _lines[_caretLine]->Add(_owner, seps[i].beg, seps[i].len);
        }

        // 记下光标所在位置行的偏移
        SetCaretOffset(_lines[_caretLine]->GetCount());
    }

    NotifyCaretPos(_caretLine + 1);

    _owner->RaiseTextChanged(oldText);
}

void TextBoxDoc::Add(Char ch)
{
    String oldText;

    GetText(oldText);
    DeleteSelect();

    _lines[_caretLine]->Insert(_owner, ch, _caretOffset);
    SetCaretOffset(_caretOffset + 1);

    NotifyCaretPos(_caretLine + 1);
    _owner->RaiseTextChanged(oldText);
}

void TextBoxDoc::GetSelectText(String & text)
{
    int bline=-1, bl=0, br=0, eline=-1, el=0, er=0;
    bool bSel = CalcSelectPos(bline, bl, br, eline, el, er);

    if (bSel)
    {
        _lines[bline]->GetSelText(text, bl, br - bl);
        for (int i = bline + 1; i < eline; ++i)
        {
            text += L"\n";
            _lines[i]->GetText(text);
        }
        if (eline > bline)
        {
            text += L"\n";
            _lines[eline]->GetSelText(text, el, er - el);
        }
    }
}

void TextBoxDoc::GetText(String& text) const
{
    int iLen = _lines.Length() - 1;

    for (int i = 0; i <= iLen; ++i)
    {
        String strline;

        if (i > 0)
        {
            text += _T("\n");
        }

        _lines[i]->GetText(strline);
        text += strline;
    }
}

bool TextBoxDoc::SetCaret(Point * lppt)
{
    Rect rc;
    Point pt(lppt);

    bool bRet = false;
    bool bSel = IsSelectMode();

    _trackLine = -1;
    _trackOffset = -1;

    pt.x += _horizontalOffset;
    pt.y += _verticalOffset;

    for (int i = 0; i < (int)_lines.Length(); ++i)
    {
        rc.bottom = rc.top + _lines[i]->GetLineHei();

        // 找到点所在的行
        if ((pt.y <= rc.bottom && pt.y >= rc.top) || i == (int)_lines.Length() - 1)
        {
            // 记下当前光标所在行
            _caretLine = i;
            // 光标所在行偏移，序号从1开始，如果光标在行末尾
            // 返回当前行的元素个数，如果光标在行首，返回0
            SetCaretOffset(_lines[i]->PointIn(_owner, &pt));

            // 计算光标位置
            bRet = NotifyCaretPos(_caretLine);

            break;
        }
        rc.top = rc.bottom;
    }
    if (bSel)
    {
        return true;
    }
    return bRet;
}

bool TextBoxDoc::SelectWord(Point * lppt)
{
    int beg = 0, end = 0;
    SetCaret(lppt);

    if (_lines[_caretLine]->GetCount() == 0)
    {
        return false;
    }

    beg = _caretOffset;
    _lines[_caretLine]->SelectWord(beg, end);
    _trackLine = _caretLine;
    SetCaretOffset(beg);
    _trackOffset = end;

    NotifyCaretPos(_caretLine);

    return true;
}

void TextBoxDoc::TrackCaret(Point* lppt)
{
    Rect rc;
    Point pt(lppt);
    int i = 0;

    pt.x += _horizontalOffset;
    pt.y += _verticalOffset;

    // 处理选中时光标位置
    for (i = 0; i < (int)_lines.Length(); ++i)
    {
        rc.bottom = rc.top + _lines[i]->GetLineHei();

        if (pt.y <= rc.bottom && pt.y >= rc.top)
        {
            break;
        }
        else if (i < (int)_lines.Length() - 1)
        {
            rc.top = rc.bottom;
        }
    }

    if (i == (int)_lines.Length())
    {
        if (lppt->y < 0)
        {
            i = 0;
            rc.top = 0;
        }
        else
        {
            i = (int)_lines.Length() - 1;
        }
    }

    // 记下选中的位置
    _trackLine = i;
    _trackOffset = _lines[i]->PointIn(_owner, &pt);

    // 计算光标位置
    NotifyCaretPos(_trackLine);
}

Rect TextBoxDoc::GetCaret(bool bUpDown)
{
    Rect caretPos;
    int nLine = _caretLine;
    int nOffset = _caretOffset;

    if (IsSelectMode())
    {
        nLine = _trackLine;
        nOffset = _trackOffset;
    }

    int nxOffset = _lines[nLine]->Offset(_owner, nOffset);
    int nyOffset = 0;

    if (_owner->GetSingleLine())
    {
        //nyOffset = (_rcVisual.Height() - GetLineHeight()) / 2;
    }

    for (int i = 0; i < nLine; ++i)
    {
        nyOffset += _lines[i]->GetLineHei();
    }

    caretPos.left = nxOffset - _horizontalOffset;
    caretPos.top = nyOffset - _verticalOffset;
    caretPos.bottom = caretPos.top + _lines[nLine]->GetLineHei();

    return caretPos;
}

bool TextBoxDoc::OnLineHome(bool bSel)
{
    if (_lines[_caretLine]->GetCount() == 0)
    {
        return false;
    }

    if (bSel)
    {
        if (_trackLine < 0)
        {
            _trackLine = _caretLine;
        }
        _trackOffset = 0;
    }
    else
    {
        SetCaretOffset(0);
    }

    if (NotifyCaretPos(_caretLine) || bSel)
    {
        return true;
    }
    return false;
}

bool TextBoxDoc::OnLineEnd(bool bSel)
{
    if (_lines[_caretLine]->GetCount() == 0)
    {
        return false;
    }

    if (bSel)
    {
        if (_trackLine < 0)
        {
            _trackLine = _caretLine;
        }
        _trackOffset = _lines[_trackLine]->GetCount();
    }
    else
    {
        SetCaretOffset(_lines[_caretLine]->GetCount());
    }
    if (NotifyCaretPos(_caretLine) || bSel)
    {
        return true;
    }
    return false;
}

bool TextBoxDoc::OnPageNext(bool bSel)
{
    return NotifyCaretPos(_caretLine);
}

bool TextBoxDoc::OnPagePrior(bool bSel)
{
    return NotifyCaretPos(_caretLine);
}

bool TextBoxDoc::OnVisualHome(bool bSel)
{
    return bSel;
}

bool TextBoxDoc::OnVisualEnd(bool bSel)
{
    return bSel;
}

bool TextBoxDoc::OnDocHome(bool bSel)
{
    if (_lines[_caretLine]->GetCount() == 0 && _lines.Length() < 2)
    {
        return false;
    }

    if (bSel)
    {
        _trackOffset = 0;
        _trackLine = 0;
    }
    else
    {
        SetCaretOffset(0);
        _caretLine = 0;
    }

    if (NotifyCaretPos(0) || bSel)
    {
        return true;
    }

    return false;
}

bool TextBoxDoc::OnDocEnd(bool bSel)
{
    if (_lines[_caretLine]->GetCount() == 0 && _lines.Length() < 2)
    {
        return false;
    }

    if (bSel)
    {
        _trackLine = (int)_lines.Length() - 1;
        _trackOffset = _lines[_trackLine]->GetCount();
    }
    else
    {
        _caretLine = (int)_lines.Length() - 1;
        SetCaretOffset(_lines[_caretLine]->GetCount());
    }
    if (NotifyCaretPos(_caretLine + 1) || bSel)
    {
        return true;
    }
    return false;
}

void TextBoxDoc::SetSelInfo(bool bSel, int * & nOffset, int * & nLine)
{
    if (bSel)
    {
        if (_trackLine < 0)
        {
            _trackLine = _caretLine;
            _trackOffset = _caretOffset;
        }
        nLine = &_trackLine;
        nOffset = &_trackOffset;
    }
    else
    {
        CancelSelect();
        nOffset = &_caretOffset;
        nLine = &_caretLine;
    }
}

bool TextBoxDoc::HasContent() const
{
    if (_lines.Length() > 0 && _lines[0]->GetCount() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

Size TextBoxDoc::Measure() const
{
    Size size;

    if (_lines.Length() > 0)
    {
        Rect rect = _lines[0]->GetRect();

        size.cx = rect.Width();
        size.cy = rect.Height() * (_lines.Length() + 0);

        for (int i = 1; i < _lines.Length(); ++i)
        {
            rect = _lines[i]->GetRect();
            size.cx = max(size.cx, rect.Width());
        }
    }

    return size;
}

int TextBoxDoc::GetHorizontalOffset() const
{
    return _horizontalOffset;
}

void TextBoxDoc::SetHorizontalOffset(int val)
{
    if (_caretOffset == 0)
    {
        _horizontalOffset = 0;
    }
    else
    {
        _horizontalOffset = val;
    }
}

int TextBoxDoc::GetVerticalOffset() const
{
    return _verticalOffset;
}

void TextBoxDoc::SetVerticalOffset(int val)
{
    _verticalOffset = val;
}

bool TextBoxDoc::OnLeftCaret(bool bSel)
{
    int * nOffset = NULL, * nLine = NULL;
    SetSelInfo(bSel, nOffset, nLine);

    if ((*nOffset) <= 0)
    {
        if (*nLine > 0)
        {
            --(*nLine);
            *nOffset = _lines[*nLine]->GetCount();
            NotifyCaretPos(_caretLine);
        }
    }
    else
    {
        --*nOffset;
    }

    SetCaretOffset(*nOffset);

    if (NotifyCaretPos(_caretLine) || bSel)
    {
        return true;
    }

    return false;
}

bool TextBoxDoc::OnUpCaret(bool bSel)
{
    int * nOffset = NULL, * nLine = NULL;
    SetSelInfo(bSel, nOffset, nLine);

    if (*nLine > 0)
    {
        *nOffset = _caretPreOffset;

        --(*nLine);

        if (*nOffset > (int)_lines[*nLine]->GetCount())
        {
            *nOffset = _lines[*nLine]->GetCount();
        }

        return NotifyCaretPos(_caretLine);
    }

    return bSel;
}

bool TextBoxDoc::OnRightCaret(bool bSel)
{
    int * nOffset = NULL, * nLine = NULL;
    SetSelInfo(bSel, nOffset, nLine);

    if (*nOffset >= (int)_lines[*nLine]->GetCount())
    {
        if (*nLine < (int)(_lines.Length() - 1))
        {
            ++(*nLine);
            *nOffset = 0;
            NotifyCaretPos(_caretLine);
        }
        else
        {
            return false;
        }
    }
    else
    {
        ++(*nOffset);
    }

    SetCaretOffset(*nOffset);

    if (NotifyCaretPos(_caretLine) || bSel)
    {
        return true;
    }
    return false;
}

bool TextBoxDoc::OnDownCaret(bool bSel)
{
    int* nOffset = NULL, * nLine = NULL;
    SetSelInfo(bSel, nOffset, nLine);

    if (*nLine < (int)_lines.Length() - 1)
    {
        *nOffset = _caretPreOffset;

        ++(*nLine);

        if (*nOffset > (int)_lines[*nLine]->GetCount())
        {
            *nOffset = _lines[*nLine]->GetCount();
        }

        return NotifyCaretPos(_caretLine + 1);
    }

    return false;
}

void TextBoxDoc::CancelSelect()
{
    int bline=-1, bl=0, br=0, eline=-1, el=0, er=0;
    bool bSel = CalcSelectPos(bline, bl, br, eline, el, er);

    if (bSel)
    {
        _caretLine = _trackLine;
        SetCaretOffset(_trackOffset);
    }

    _trackLine = -1;
    _trackOffset = -1;
}

void TextBoxDoc::CalcTextRect(Rect * lprc, int nLine)
{
}

void TextBoxDoc::RefleshCaret()
{
    SetCaretOffset(0);
    
    if (NULL != _owner)
    {
        _owner->NotifyVerticalOffset(0);
        _owner->NotifyHorizontalOffset(0);
    }
}

bool TextBoxDoc::NotifyCaretPos(int iLine)
{
    if (_owner != NULL)
    {
        int verticalOffset = 0;
        int horizontalOffset = 0;

        if (_caretLine < _lines.Length())
        {
            horizontalOffset = _lines[_caretLine]->Offset(_owner, _caretOffset);
        }

        verticalOffset = _lines[0]->GetLineHei() * _caretLine;

        _owner->NotifyVerticalOffset(verticalOffset);
        _owner->NotifyHorizontalOffset(horizontalOffset);

        return true;
    }
    else
    {
        return false;
    }
}

bool TextBoxDoc::CalcSelectPos(int & bline, int & bl, int & br, int & eline, int & el, int & er)
{
    // 如果有选中部分
    if (_trackLine != -1 && -1 != _trackOffset)
    {
        // 选中一行中的一部分
        if (_trackLine == _caretLine)
        {
            bline = _trackLine;
            eline = _trackLine;
            if (_trackOffset > _caretOffset)
            {
                bl = _caretOffset;
                br = _trackOffset;
            }
            else
            {
                bl = _trackOffset;
                br = _caretOffset;
            }
        }
        else if (_trackLine > _caretLine)
        {
            bline = _caretLine;
            bl = _caretOffset;
            br = _lines[_caretLine]->GetCount();

            eline = _trackLine;
            el = 0;
            er = _trackOffset;
        }
        else
        {
            bline = _trackLine;
            bl = _trackOffset;
            br = _lines[_trackLine]->GetCount();

            eline = _caretLine;
            el = 0;
            er = _caretOffset;
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool TextBoxDoc::DeleteSelect()
{
    int bline=0, bl=0, br=0, eline=0, el=0, er=0;

    if (CalcSelectPos(bline, bl, br, eline, el, er))
    {
        String oldText;
        GetText(oldText);

        if (bline == eline)
        {
            _lines[bline]->Remove(_owner, bl, br);
        }
        else
        {
            if (_lines[bline]->GetCount() > 0)
            {
                _lines[bline]->Remove(_owner, bl, br);
            }

            if (_lines[eline]->GetCount() == 0 || (er - el == _lines[eline]->GetCount()))
            {
                DeleteLine(eline);
            }
            else
            {
                _lines[bline]->AddLine(_owner, _lines[eline], er, _lines[eline]->GetCount());
                DeleteLine(eline);
            }

            if (eline - bline > 1)
            {
                DeleteLineRange(bline + 1, eline - 1);
            }
        }

        CancelSelect();
        _caretLine = bline;
        SetCaretOffset(bl);

        NotifyCaretPos(_caretLine);

        _owner->RaiseTextChanged(oldText);

        return true;
    }
    return false;
}

void TextBoxDoc::ReplaceSelect(const String & text)
{
    Add(text);
}

// 在光标处插入一行
void TextBoxDoc::InsertLine()
{
    DeleteSelect();

    TextBoxLine* newLine(new TextBoxLine());

    if (_owner)
    {
        newLine->RefleshRect(_owner);
    }

    if (_caretLine == -1)
    {
        _lines.Add(newLine);
    }
    else
    {
        if (_caretOffset <= 0)
        {
            _lines.Insert(_caretLine, newLine);
        }
        else
        {
            _lines.Insert(_caretLine + 1, newLine);
        }
    }

    _caretLine += 1;

    // 如果光标在一行的中间，需要把这行分为两行
    if (_caretLine > 0 && _caretOffset > 0 && _caretOffset < (int)_lines[_caretLine-1]->GetCount())
    {
        newLine->AddLine(_owner, _lines[_caretLine-1], _caretOffset, -1);
        _lines[_caretLine-1]->Remove(_owner, _caretOffset, -1);
    }

    SetCaretOffset(0);
    NotifyCaretPos(_caretLine);
}

// 向左删除一个元素
void TextBoxDoc::DelLeftOne()
{
    if (DeleteSelect())
    {
        return;
    }

    String oldText;

    GetText(oldText);

    // 删除一行
    if (_caretOffset <= 0)
    {
        if (_caretLine > 0)
        {
            _caretOffset = _lines[_caretLine - 1]->GetCount();

            // 把下一行合并到上一行
            _lines[_caretLine - 1]->AddLine(_owner, _lines[_caretLine]);
            DeleteLine(_caretLine);
            _caretLine -= 1;
        }
    }
    else
    {
        _lines[_caretLine]->Remove(_owner, _caretOffset - 1, _caretOffset);
        SetCaretOffset(_caretOffset - 1);
    }

    NotifyCaretPos(_caretLine);
    _owner->RaiseTextChanged(oldText);
}

// 向右删除一个元素
void TextBoxDoc::DelRightOne()
{
    if (DeleteSelect())
    {
        return;
    }

    String oldText;

    GetText(oldText);

    // 删除一行
    if (_caretOffset == _lines[_caretLine]->GetCount())
    {
        if (_caretLine < (int)_lines.Length() - 1)
        {
            // 把光标处下一行的数据合并到光标所在行
            _lines[_caretLine]->AddLine(_owner, _lines[_caretLine + 1]);
            DeleteLine(_caretLine + 1);
        }
    }
    else
    {
        // 空行，删除
        if (_lines[_caretLine]->GetCount() == 0)
        {
            DeleteLine(_caretLine);
        }
        else
        {
            // 向右删除一个字符
            _lines[_caretLine]->Remove(_owner, _caretOffset, _caretOffset + 1);
        }
    }

    NotifyCaretPos(_caretLine);
    _owner->RaiseTextChanged(oldText);
}

bool TextBoxDoc::IsSelectMode()
{
    bool bRet = true;

    if (_trackLine < 0 || _trackOffset < 0)
    {
        bRet = false;
    }
    else if (_trackLine == _caretLine && _trackOffset == _caretOffset)
    {
        bRet = false;
        _trackLine = -1;
        _trackOffset = -1;
    }

    return bRet;
}

}

