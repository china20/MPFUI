// 华勤科技版权所有 2008-2022
// 
// 文件名：RichInterface.h
// 功  能：实现文本编辑框
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// ============================================================================

#ifndef _UIRICHINTERFACE_H_
#define _UIRICHINTERFACE_H_

#include "TextHostView.h"

#include <TOM.h>
#include <Richedit.h>
#include <TextServ.h>

#include <Extend/Editor/TextContainer.h>
#include <Extend/Editor/TextHostView.h>
#include <Extend/Editor/RichTextDef.h>

const int LY_PER_INCH = 1440;
const int LX_PER_INCH = 1440;

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

#define UI_TWIPS_TO_FPPTS(x) (((float)(x)) * (float)0.05)

LONG UILXtoDX(LONG x);
LONG UIDXtoLX(LONG x);

namespace suic
{

class SUIWGX_API TextFontOp : public TextFont
{
public:

    TextFontOp()
    {
        _textFont = NULL;
    }

    TextFontOp(ITextFont* textFont)
    {
        _textFont = textFont;
        _textFont->AddRef();
    }

    virtual ~TextFontOp()
    {
        if (_textFont)
        {
            _textFont->Release();
        }
    }

    void SetPointer(ITextFont* textFont)
    {
        _textFont = textFont;
        _textFont->AddRef();
    }

    virtual void Commit()
    {

    }

    virtual bool CanChange()
    {
        return true;
    }

    virtual long GetStyle()
    {
        long lRet = 0;
        _textFont->GetStyle(&lRet);
        return lRet;
    }

    virtual void SetStyle(long val)
    {
        _textFont->SetStyle(val);
    }

    virtual long GetAllCaps()
    {
        long lRet = 0;
        _textFont->GetStyle(&lRet);
        return lRet;
    }

    virtual void SetAllCaps(long val)
    {
        _textFont->SetAllCaps(val);
    }

    virtual long GetBackColor()
    {
        long lRet = 0;
        _textFont->GetBackColor(&lRet);
        return lRet;
    }

    virtual void SetBackColor(long val)
    {
        _textFont->SetBackColor(val);
    }

    virtual bool GetBold()
    {
        long lRet = 0;
        _textFont->GetBold(&lRet);
        return lRet != 0;
    }

    virtual void SetBold(bool bBold)
    {
        _textFont->SetBold(bBold ? tomToggle : 0);
    }

    virtual long GetTextColor()
    {
        long lRet = 0;
        _textFont->GetForeColor(&lRet);
        return lRet;
    }
    virtual void SetTextColor(long val)
    {
        _textFont->SetForeColor(val);
    }

    virtual bool GetHidden()
    {
        long lRet = 0;
        _textFont->GetHidden(&lRet);
        return lRet != 0;
    }
    virtual void SetHidden(bool hidden)
    {
        _textFont->SetHidden(hidden ? tomToggle : 0);
    }

    virtual bool GetItalic()
    {
        long lRet = 0;
        _textFont->GetItalic(&lRet);
        return lRet != 0;
    }
    virtual void SetItalic(bool val)
    {
        _textFont->SetItalic(val ? tomToggle : 0);
    }

    virtual long GetLanguageId()
    {
        long lRet = 0;
        _textFont->GetStyle(&lRet);
        return lRet;
    }
    virtual void SetLanguageId(long val)
    {
        _textFont->SetLanguageID(val);
    }

    virtual suic::String GetName()
    {
        suic::String name;
        BSTR bstr = NULL;

        _textFont->GetName(&bstr);

        if (NULL != bstr)
        {
            name = (LPWSTR)bstr;
            ::SysFreeString(bstr);
        }

        return name;
    }

    virtual void SetName(suic::String name)
    {
        BSTR bstr = ::SysAllocStringLen((LPWSTR)name.c_str(), name.Length());
        if (bstr != NULL)
        {
            _textFont->SetName(bstr);
            ::SysFreeString(bstr);
        }
    }

    virtual long GetOutline()
    {
        long lRet = 0;
        _textFont->GetOutline(&lRet);
        return lRet;
    }
    virtual void SetOutline(long val)
    {
        _textFont->SetOutline(val);
    }

    virtual bool GetProtected()
    {
        long lRet = 0;
        _textFont->GetProtected(&lRet);
        return lRet != 0;
    }
    virtual void SetProtected(bool val)
    {
        _textFont->SetProtected(val ? tomToggle : 0);
    }

    virtual long GetShadow()
    {
        long lRet = 0;
        _textFont->GetShadow(&lRet);
        return lRet;
    }
    virtual void SetShadow(long val)
    {
        _textFont->SetShadow(val);
    }

    virtual float GetSize()
    {
        float fRet = 0;
        _textFont->GetSize(&fRet);
        return UILXtoDX(fRet);
    }

    virtual void SetSize(float val)
    {
        _textFont->SetSize(UI_TWIPS_TO_FPPTS(UIDXtoLX(val)));
    }

    virtual float GetSpacing()
    {
        float fRet = 0;
        _textFont->GetSpacing(&fRet);
        return fRet;
    }
    virtual void SetSpacing(float val)
    {
        _textFont->SetSpacing(val);
    }

    virtual bool GetUnderline()
    {
        long lRet = 0;
        _textFont->GetUnderline(&lRet);
        return lRet != 0;
    }
    virtual void SetUnderline(bool val)
    {
        _textFont->SetUnderline(val ? tomToggle : 0);
    }

    virtual long GetWeight()
    {
        long lRet = 0;
        _textFont->GetWeight(&lRet);
        return lRet;
    }

    virtual void SetWeight(long val)
    {
        _textFont->SetWeight(val);
    }

private:

    ITextFont* _textFont;
};

class TextParaOp : public TextPara
{
public:

    TextParaOp(ITextPara* txtPara)
    {
        _txtPara = txtPara;

        if (_txtPara != NULL)
        {
            _txtPara->AddRef();
        }
    }

    virtual ~TextParaOp()
    {
        if (_txtPara != NULL)
        {
            _txtPara->Release();
        }
    }

    TextParaOp* ToTextParaOp(TextPara* para)
    {
        return (TextParaOp*)para;
    }

    virtual void Begin()
    {

    }

    virtual void Commit()
    {

    }

    virtual long CanChange()
    {
        long lRet = 0;
        _txtPara->CanChange(&lRet);
        return lRet;
    }

    virtual bool IsEqual(TextPara *pPara)
    {
        long lRet = 0;
        _txtPara->IsEqual(ToTextParaOp(pPara)->_txtPara, &lRet);
        return (lRet != 0);
    }

    virtual void Reset(long value)
    {
        _txtPara->Reset(value);
    }

    virtual long GetStyle()
    {
        long lRet = 0;
        _txtPara->GetStyle(&lRet);
        return lRet;
    }

    virtual void SetStyle(long value)
    {
        _txtPara->SetStyle(value);
    }

    virtual TPAlign GetAlignment()
    {
        long lRet = 0;
        _txtPara->GetAlignment(&lRet);
        return (TPAlign)lRet;
    }

    virtual void SetAlignment(TPAlign value)
    {
        _txtPara->SetAlignment(value);
    }

    virtual long GetHyphenation()
    {
        long lRet = 0;
        _txtPara->GetHyphenation(&lRet);
        return lRet;
    }

    virtual void SetHyphenation(long value)
    {
        _txtPara->SetHyphenation(value);
    }

    virtual long GetFirstLineIndent()
    {
        float fRet = 0;
        _txtPara->GetFirstLineIndent(&fRet);
        return UILXtoDX(fRet);
    }

    virtual long GetKeepTogether()
    {
        long lRet = 0;
        _txtPara->GetKeepTogether(&lRet);
        return lRet;
    }

    virtual void SetKeepTogether(long value)
    {
        _txtPara->SetKeepTogether(value);
    }

    virtual long GetKeepWithNext()
    {
        long lRet = 0;
        _txtPara->GetKeepWithNext(&lRet);
        return lRet;
    }

    virtual void SetKeepWithNext(long value)
    {
        _txtPara->SetKeepWithNext(value);
    }

    virtual long GetLeftIndent()
    {
        float fRet = 0;
        _txtPara->GetLeftIndent(&fRet);
        return UILXtoDX(fRet);
    }

    virtual long GetLineSpace()
    {
        float fRet = 0;
        _txtPara->GetLineSpacing(&fRet);
        return UILXtoDX(fRet);
    }

    virtual TPLineRule GetLineRule()
    {
        long lRet = 0;
        _txtPara->GetLineSpacingRule(&lRet);
        return (TPLineRule)lRet;
    }

    virtual long GetListAlignment()
    {
        long lRet = 0;
        _txtPara->GetListAlignment(&lRet);
        return lRet;
    }

    virtual void SetListAlignment(long value)
    {
        _txtPara->SetListAlignment(value);
    }

    virtual long GetListLevelIndex()
    {
        long lRet = 0;
        _txtPara->GetListLevelIndex(&lRet);
        return lRet;
    }

    virtual void SetListLevelIndex(long value)
    {
        _txtPara->SetListLevelIndex(value);
    }

    virtual long GetListStart()
    {
        long lRet = 0;
        _txtPara->GetListStart(&lRet);
        return lRet;
    }

    virtual void SetListStart(long value)
    {
        _txtPara->SetListStart(value);
    }

    virtual long GetListTab()
    {
        float fRet = 0;
        _txtPara->GetListTab(&fRet);
        return UILXtoDX(fRet);
    }

    virtual void SetListTab(long value)
    {
        _txtPara->SetListTab(UI_TWIPS_TO_FPPTS(UIDXtoLX(value)));
    }

    virtual long GetListType()
    {
        long lRet = 0;
        _txtPara->GetListType(&lRet);
        return lRet;
    }

    virtual void SetListType(long value)
    {
        _txtPara->SetListType(value);
    }

    virtual long GetNoLineNumber()
    {
        long lRet = 0;
        _txtPara->GetNoLineNumber(&lRet);
        return lRet;
    }

    virtual void SetNoLineNumber(long value)
    {
        _txtPara->SetNoLineNumber(value);
    }

    virtual long GetPageBreakBefore()
    {
        long lRet = 0;
        _txtPara->GetPageBreakBefore(&lRet);
        return lRet;
    }

    virtual void SetPageBreakBefore(long value)
    {
        _txtPara->SetPageBreakBefore(value);
    }

    virtual long GetRightIndent()
    {
        float fRet = 0;
        _txtPara->GetRightIndent(&fRet);
        return UILXtoDX(fRet);
    }

    virtual void SetRightIndent(long value)
    {
        _txtPara->SetRightIndent(UI_TWIPS_TO_FPPTS(UIDXtoLX(value)));
    }

    virtual void SetIndents(long startIndent, long leftIndent, long rightIndent)
    {
        startIndent = UI_TWIPS_TO_FPPTS(UIDXtoLX(startIndent));
        leftIndent = UI_TWIPS_TO_FPPTS(UIDXtoLX(leftIndent));
        rightIndent = UI_TWIPS_TO_FPPTS(UIDXtoLX(rightIndent));

        _txtPara->SetIndents(startIndent, leftIndent, rightIndent);
    }

    virtual void SetLineSpace(TPLineRule lineSpaceRule, long lineSpace)
    {
        if (lineSpaceRule == suic::TPLineRule::tpLineMultiple)
        {
            _txtPara->SetLineSpacing(lineSpaceRule, UI_TWIPS_TO_FPPTS(lineSpace));
        }
        else
        {
            _txtPara->SetLineSpacing(lineSpaceRule, UI_TWIPS_TO_FPPTS(UIDXtoLX(lineSpace)));
        }
    }

    virtual long GetSpaceAfter()
    {
        float fRet = 0;
        _txtPara->GetSpaceAfter(&fRet);
        return UILXtoDX(fRet);
    }

    virtual void SetSpaceAfter(long val)
    {
        _txtPara->SetSpaceAfter(UI_TWIPS_TO_FPPTS(UIDXtoLX(val)));
    }

    virtual long GetSpaceBefore()
    {
        float fRet = 0;
        _txtPara->GetSpaceBefore(&fRet);
        return UILXtoDX(fRet);
    }

    virtual void SetSpaceBefore(long val)
    {
        _txtPara->SetSpaceBefore(UI_TWIPS_TO_FPPTS(UIDXtoLX(val)));
    }

    virtual long GetTabCount()
    {
        long lRet = 0;
        _txtPara->GetTabCount(&lRet);
        return lRet;
    }

    virtual void AddTab(suic::Float tbPos, long tbAlign, long tbLeader)
    {
        _txtPara->AddTab(tbPos, tbAlign, tbLeader);
    }

    virtual void ClearAllTabs( void)
    {
        _txtPara->ClearAllTabs();
    }

    virtual void DeleteTab(suic::Float tbPos)
    {
        _txtPara->DeleteTab(tbPos);
    }

    virtual void GetTab(long iTab, suic::Float *ptbPos, long *ptbAlign, long *ptbLeader)
    {
        _txtPara->GetTab(iTab, ptbPos, ptbAlign, ptbLeader);
    }

private:

    ITextPara* _txtPara;
};

/// <summary>
///  ITextRange接口实现。
/// </summary>
class TextRangeOp : public TextRange
{
public:

    RTTIOfClass(TextRangeOp)

    TextRangeOp()
        : _textPara(NULL)
        , _textFont(NULL)
        , _textRange(NULL)
    {
    }

    TextRangeOp(ITextRange* pTextRange)
        : _textPara(NULL)
        , _textFont(NULL)
    {
        _textRange = pTextRange;
    }

    ~TextRangeOp()
    {
        if (NULL != _textPara)
        {
            delete _textPara;
        }
        if (NULL != _textFont)
        {
            delete _textFont;
        }
        if (_textRange)
        {
            _textRange->Release();
        }
    }

    virtual suic::String GetText()
    {
        suic::String text;
        BSTR bstr = NULL;
        _textRange->GetText(&bstr);
        if (NULL != bstr)
        {
            text = (LPWSTR)bstr;
            ::SysFreeString(bstr);
        }
        return text;
    }

    virtual void SetText(suic::String text)
    {
        if (text.Length() > 0)
        {
            BSTR bstr = ::SysAllocStringLen((LPWSTR)text.c_str(), text.Length());
            if (bstr != NULL)
            {
                _textRange->SetText(bstr);
                ::SysFreeString(bstr);
            }
        }
        else
        {
            _textRange->SetText(NULL);
        }
    }

    virtual bool GetHidden()
    {
        return GetTextFont()->GetHidden();
    }

    virtual void SetHidden(bool val)
    {
        GetTextFont()->SetHidden(val);
    }

    virtual void Collapse (long start)
    {
        _textRange->Collapse(start);
    }

    virtual void Expand (long unit, long *delta)
    {
        _textRange->Expand(unit, delta);
    }

    virtual void Select()
    {
        _textRange->Select();
    }

    virtual TextRange* GetClone()
    {
        ITextRange* tr = NULL;
        TextRange* pClone = NULL;
        
        _textRange->GetDuplicate(&tr);
        pClone = new TextRangeOp(tr);
        pClone->ref();

        return pClone;
    }

    virtual TextFont* GetTextFont()
    {
        if (NULL == _textFont)
        {
            ITextFont* txtFont = NULL;
            _textRange->GetFont(&txtFont);
            _textFont = new TextFontOp(txtFont);
            txtFont->Release();
        }

        return _textFont;
    }

    virtual TextPara* GetTextPara()
    {
        if (NULL == _textPara)
        {
            ITextPara* txtPara = NULL;
            _textRange->GetPara(&txtPara);
            _textPara = new TextParaOp(txtPara);
            txtPara->Release();
        }
        return _textPara;
    }

    virtual void SetTextPara(TextPara* para)
    {
        ;
    }

    virtual int IsEqual(TextRange* Other)
    {
        long lRet = 0;
        //_textRange->IsEqual(Other->_s, &lRet);
        return lRet;
    }

    virtual int FindText(suic::String text, long cch, long flags)
    {
        long lRet = 0;
        _textRange->FindText(NULL, cch, flags, &lRet);
        return lRet;
    }

    virtual void Delete(int index, int count)
    {
        long lRet = 0;
         _textRange->Delete(index, count, &lRet);
    }

    virtual bool CanEdit()
    {
        long lRet = 0;
        _textRange->CanEdit(&lRet);
        return lRet != 0;
    }

    virtual bool CanPaste()
    {
        long lRet = 0;
        _textRange->CanPaste(0, 0, &lRet);
        return lRet != 0;
    }

    virtual void Cut()
    {
        _textRange->Cut(0);
    }

    virtual void Copy()
    {
        _textRange->Copy(0);
    }

    virtual void Paste()
    {
        _textRange->Paste(0, 0);
    }

    virtual suic::Rect GetBound()
    {
        suic::Rect rect;
        return rect;
    }

    virtual suic::Point GetPoint(bool bStart)
    {
        suic::Point pt;

        if (bStart)
        {
            _textRange->GetPoint(tomStart | TA_LEFT | TA_TOP, &(pt.x), &(pt.y));
        }
        else
        {
            _textRange->GetPoint(tomEnd | TA_BOTTOM | TA_RIGHT, &(pt.x), &(pt.y));
        }

        return pt;
    }

    virtual int GetStart()
    {
        long lRet = 0;
        _textRange->GetStart(&lRet);
        return lRet;
    }

    virtual void SetStart(int index)
    {
        _textRange->SetStart(index);
    }

    virtual int GetEnd()
    {
        long lRet = 0;
        _textRange->GetEnd(&lRet);
        return lRet;
    }

    virtual void SetEnd(int index)
    {
        _textRange->SetEnd(index);
    }

    virtual void ScrollIntoView(long val)
    {
        _textRange->ScrollIntoView(val);
    }

    virtual long Move(int iUnit, int iCount)
    {
        long lRet = 0;
        _textRange->Move(iUnit, iCount, &lRet);
        return lRet;
    }

    virtual long MoveStart(int iUnit, int iCount)
    {
        long lRet = 0;
        _textRange->MoveStart(iUnit, iCount, &lRet);
        return lRet;
    }

    virtual long MoveEnd(int iUnit, int iCount)
    {
        long lRet = 0;
        _textRange->MoveEnd(iUnit, iCount, &lRet);
        return lRet;
    }

private:

    TextFontOp* _textFont;
    ITextRange* _textRange;
    TextPara* _textPara;
};

/// <summary>
///  TextSelection接口实现。
/// </summary>
class TextSelectionOp : public TextSel
{
public:

    RTTIOfClass(TextSelectionOp)

    TextSelectionOp()
        : _textFont(NULL)
        , _textPara(NULL)
        , _textSel(NULL)
    {

    }

    TextSelectionOp(ITextSelection* pTextSel)
        : _textFont(NULL)
        , _textPara(NULL)
    {
        _textSel = pTextSel;
    }

    ~TextSelectionOp()
    {
        if (_textPara)
        {
            delete _textPara;
        }

        if (_textFont)
        {
            delete _textFont;
        }

        if (_textSel)
        {
            _textSel->Release();
        }
    }

    virtual suic::String GetText()
    {
        suic::String text;
        BSTR bstr = NULL;
        _textSel->GetText(&bstr);
        if (NULL != bstr)
        {
            text = (LPWSTR)bstr;
            ::SysFreeString(bstr);
        }
        return text;
    }

    virtual void SetText(suic::String text)
    {
        if (text.Length() > 0)
        {
            BSTR bstr = ::SysAllocStringLen((LPWSTR)text.c_str(), text.Length());
            if (bstr != NULL)
            {
                _textSel->SetText(bstr);
                ::SysFreeString(bstr);
            }
        }
        else
        {
            _textSel->SetText(NULL);
        }
    }

    virtual bool GetHidden()
    {
        return GetTextFont()->GetHidden();
    }

    virtual void SetHidden(bool val)
    {
        GetTextFont()->SetHidden(val);
    }

    virtual void Collapse (long start)
    {
        _textSel->Collapse(start);
    }

    virtual void Expand (long unit, long *delta)
    {
        _textSel->Expand(unit, delta);
    }

    virtual void Select()
    {
        _textSel->Select();
    }

    virtual TextRange* GetClone()
    {
        ITextRange* tr = NULL;
        TextRange* pClone = NULL;

        _textSel->GetDuplicate(&tr);
        pClone = new TextRangeOp(tr);
        pClone->ref();

        return pClone;
    }

    virtual TextFont* GetTextFont()
    {
        if (NULL == _textFont)
        {
            ITextFont* txtFont = NULL;
            _textSel->GetFont(&txtFont);
            _textFont = new TextFontOp(txtFont);
            txtFont->Release();
        }

        return _textFont;
    }

    virtual TextPara* GetTextPara()
    {
        if (NULL == _textPara)
        {
            ITextPara* txtPara = NULL;
            _textSel->GetPara(&txtPara);
            _textPara = new TextParaOp(txtPara);
            txtPara->Release();
        }
        return _textPara;
    }

    virtual int IsEqual(TextRange* Other)
    {
        long lRet = 0;
        //_textSel->IsEqual(Other->_s, &lRet);
        return lRet;
    }

    virtual int FindText(suic::String text, long cch, long flags)
    {
        long lRet = 0;
        _textSel->FindText(NULL, cch, flags, &lRet);
        return lRet;
    }

    virtual void Delete(int index, int count)
    {
        long lRet = 0;
        _textSel->Delete(index, count, &lRet);
    }

    virtual bool CanEdit()
    {
        long lRet = 0;
        _textSel->CanEdit(&lRet);
        return lRet != 0;
    }

    virtual bool CanPaste()
    {
        long lRet = 0;
        _textSel->CanPaste(0, 0, &lRet);
        return lRet != 0;
    }

    virtual void Cut()
    {
        _textSel->Cut(0);
    }

    virtual void Copy()
    {
        _textSel->Copy(0);
    }

    virtual void Paste()
    {
        _textSel->Paste(0, 0);
    }

    virtual suic::Rect GetBound()
    {
         suic::Rect rect;
         return rect;
    }

    virtual suic::Point GetPoint(bool bStart)
    {
        suic::Point pt;

        if (bStart)
        {
            _textSel->GetPoint(tomStart | TA_LEFT | TA_TOP, &(pt.x), &(pt.y));
        }
        else
        {
            _textSel->GetPoint(tomEnd | TA_BOTTOM | TA_RIGHT, &(pt.x), &(pt.y));
        }

        return pt;
    }

    virtual int GetStart()
    {
        long lRet = 0;
        _textSel->GetStart(&lRet);
        return lRet;
    }

    virtual void SetStart(int index)
    {
        _textSel->SetStart(index);
    }

    virtual int GetEnd()
    {
        long lRet = 0;
        _textSel->GetEnd(&lRet);
        return lRet;
    }

    virtual void SetEnd(int index)
    {
        _textSel->SetEnd(index);
    }

    virtual void ScrollIntoView(long val)
    {
        _textSel->ScrollIntoView(val);
    }

    virtual long Move(int iUnit, int iCount)
    {
        long lRet = 0;
        _textSel->Move(iUnit, iCount, &lRet);
        return lRet;
    }

    virtual long MoveStart(int iUnit, int iCount)
    {
        long lRet = 0;
        _textSel->MoveStart(iUnit, iCount, &lRet);
        return lRet;
    }

    virtual long MoveEnd(int iUnit, int iCount)
    {
        long lRet = 0;
        _textSel->MoveEnd(iUnit, iCount, &lRet);
        return lRet;
    }

    //--------------------------------------------------

    virtual long MoveLeft(int iUnit, int iCount, int iExtend)
    {
        long lRet = 0;
        _textSel->MoveLeft(iUnit, iCount, iExtend, &lRet);
        return lRet;
    }

    virtual long MoveRight(int iUnit, int iCount, int iExtend)
    {
        long lRet = 0;
        _textSel->MoveRight(iUnit, iCount, iExtend, &lRet);
        return lRet;
    }

    virtual long MoveUp(int iUnit, int iCount, int iExtend)
    {
        long lRet = 0;
        _textSel->MoveUp(iUnit, iCount, iExtend, &lRet);
        return lRet;
    }

    virtual long MoveDown(int iUnit, int iCount, int iExtend)
    {
        long lRet = 0;
        _textSel->MoveDown(iUnit, iCount, iExtend, &lRet);
        return lRet;
    }

    virtual long HomeKey(int iUnit, int iExtend)
    {
        long lRet = 0;
        _textSel->HomeKey(iUnit, iExtend, &lRet);
        return lRet;
    }

    virtual long EndKey(int iUnit, int iExtend)
    {
        long lRet = 0;
        _textSel->EndKey(iUnit, iExtend, &lRet);
        return lRet;
    }

private:

    TextFontOp* _textFont;
    TextPara* _textPara;
    ITextSelection* _textSel;

    friend class TextDocOp;
};

// 从Stream读取内容到Rich

extern "C" DWORD CALLBACK RichDocStreamInCb(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
extern "C" DWORD CALLBACK RichDocStreamOutCb(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

class TextDocOp : public ITextDoc
{
public:

    TextDocOp()
    {
        _textHost = NULL;
        _textSelOp = NULL;
        _textRangeOp = NULL;
    }

    TextDocOp(TextRenderHost* textHost)
    {
        ITextSelection* textSel = NULL;

        _textHost = textHost;
        _textRangeOp = NULL;
        GetTextDoc()->GetSelection(&textSel);
        _textSelOp = new TextSelectionOp(textSel);
        _textSelOp->ref();
    }

    ~TextDocOp()
    {
        _textSelOp->unref();
        if (NULL != _textRangeOp)
        {
            _textRangeOp->unref();
        }
    }

    TextRenderHost* GetTextHost() const;
    ITextDocument* GetTextDoc();

    virtual void Undo(int layer)
    {
        long lRet = 0;
        GetTextDoc()->Undo(layer, &lRet);
    }

    virtual void Redo(int layer)
    {
        long lRet = 0;
        GetTextDoc()->Redo(layer, &lRet);
    }

    virtual TextFont* GetTextFont();
    virtual TextPara* GetTextPara();

    virtual void Clear();

    virtual void GetStream(suic::ISStream* stm, bool atSel);
    virtual void SetStream(const suic::ISStream* stm, bool atSel);

    virtual TextSel* GetTextSel()
    {
        if (_textSelOp != NULL)
        {
            _textSelOp->ref();
        }
        return _textSelOp;
    }

    virtual TextRange* GetRange(int index, int count)
    {
        TextRangeOp* textRangeOp = NULL;
        if (NULL == textRangeOp)
        {
            ITextRange* textRange = NULL;

            GetTextDoc()->Range(index, index + count, &textRange);
            textRangeOp = new TextRangeOp(textRange);
            //_textRangeOp->ref();
        }

        //textRangeOp->SetStart(index);
        //textRangeOp->SetEnd(index + count);

        textRangeOp->ref();

        return textRangeOp;
    }

    virtual TextRange* GetRangeFromPoint(int x, int y)
    {
        TextRangeOp* trOp = NULL;
        ITextRange* textRange = NULL;

        GetTextDoc()->RangeFromPoint(x, y, &textRange);
        trOp = new TextRangeOp(textRange);

        trOp->ref();

        return trOp;
    }

    virtual int GetStoryCount()
    {
        long lRet = 0;
        GetTextDoc()->GetStoryCount(&lRet);
        return lRet;
    }

    //-----------------------------------------------------

    virtual bool Redo();
    virtual bool Undo();

    virtual void Copy();
    virtual void Cut();
    virtual void Paste();

    virtual bool IsWordWrapping();
    virtual void SetWordWrapping(bool val);

    virtual bool IsReadOnly();
    virtual void SetReadOnly(bool val);

    virtual long GetCharCount() const;
    virtual suic::String GetText() const;

    virtual void SetText(suic::String strText, bool bCanUndo);
    virtual void AddText(suic::String strText, bool bCanUndo);

    virtual int GetSelLength() const;
    virtual suic::String GetSelText() const;
    
    virtual suic::String GetLineText(int index) const;
    virtual int GetLineCount() const;
    virtual int GetLineLength(int line) const;
    virtual int GetCharIndexOfLineStart(int line) const;

    virtual int Select(long lStart, long lCount);
    virtual int SelectAll();
    virtual void SelectStart();
    virtual void SelectEnd();

    virtual void RecalcView(bool bForce);
    virtual suic::Point GetPoint(suic::Uint32 cp, bool bLeftTop);
    virtual void GetLineDetail(int index, LineDetail* info, int* iYPos);

    virtual void ReplaceSel(const suic::String& strText, bool bCanUndo);

    virtual int LineFromCharIndex(int ch) const;
    virtual int CharIndexFromLine(int line) const;
    virtual suic::Point PosFromCharIndex(int ch) const;
    virtual int CharIndexFromPos(suic::Point pt) const;

    virtual int GetFirstVisibleLine() const;
    virtual int GetFirstVisibleCharIndex() const;
    virtual int GetLastVisibleCharIndex() const;

    virtual bool SetZoom(int nNum, int nDen);
    virtual bool SetZoomOff();

private:

    void OnTxPropertyBitsChange(UINT key, UINT val) const;
    LRESULT TxSendMessage(UINT message, WPARAM wp, LPARAM lp) const;

private:

    TextRenderHost* _textHost;
    TextRangeOp* _textRangeOp;
    TextSelectionOp* _textSelOp;
};

}

#endif
