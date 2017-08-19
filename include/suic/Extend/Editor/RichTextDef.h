// ���ڿƼ���Ȩ���� 2010-2012
// 
// �ļ�����RicheditDef.h
// ��  �ܣ����ı��ӿڶ��塣
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-05-01
// 
// ============================================================================

#ifndef _UIRICHTEXTDEF_H_
#define _UIRICHTEXTDEF_H_

#include <Extend/suiwgxconfig.h>
#include <System/Windows/Object.h>
#include <Extend/Editor/Embbed.h>

namespace suic
{

/// <summary>
/// ���帻�ı���������ӿ�
/// </summary>
class SUIWGX_API TextFont
{
public:

    virtual ~TextFont() {}

    virtual void Commit() = 0;
    virtual bool CanChange() = 0;

    virtual long GetStyle() = 0;
    virtual void SetStyle(long Value) = 0;

    virtual long GetAllCaps() = 0;
    virtual void SetAllCaps(long Value) = 0;

    virtual long GetBackColor() = 0;
    virtual void SetBackColor(long Value) = 0;

    virtual bool GetBold() = 0;
    virtual void SetBold(bool bBold) = 0;

    virtual long GetTextColor() = 0;
    virtual void SetTextColor(long Value) = 0;

    virtual bool GetHidden() = 0;
    virtual void SetHidden(bool hidden) = 0;

    virtual bool GetItalic() = 0;
    virtual void SetItalic(bool val) = 0;

    virtual long GetLanguageId() = 0;
    virtual void SetLanguageId(long val) = 0;

    virtual suic::String GetName() = 0;
    virtual void SetName(suic::String name) = 0;

    virtual long GetOutline() = 0;
    virtual void SetOutline(long val) = 0;

    virtual bool GetProtected() = 0;
    virtual void SetProtected(bool val) = 0;

    virtual long GetShadow() = 0;
    virtual void SetShadow(long val) = 0;

    virtual float GetSize() = 0;
    virtual void SetSize(float val) = 0;

    virtual float GetSpacing() = 0;
    virtual void SetSpacing(float val) = 0;

    virtual bool GetUnderline() = 0;
    virtual void SetUnderline(bool val) = 0;

    virtual long GetWeight() = 0;
    virtual void SetWeight(long val) = 0;
};

enum TPAlign
{
    tpLeft = 0,
    tpCenter = 1,
    tpRight = 2,
};

enum TPLineRule
{
    tpLineSingle = 0,
    tpLine1pt5 = 1,
    tpLineDouble = 2,
    tpLineAtLeast = 3,
    tpLineExactly = 4,
    tpLineMultiple = 5,
};

/// <summary>
/// �����ı������ʽ���ýӿ�
/// </summary>
class SUIWGX_API TextPara
{
public:

    virtual ~TextPara() {}

    virtual void Begin() = 0;
    virtual void Commit() = 0;

    virtual bool IsEqual(TextPara *pPara) = 0;

    virtual long GetStyle() = 0;
    virtual void SetStyle(long value) = 0;

    virtual TPAlign GetAlignment() = 0;
    virtual void SetAlignment(TPAlign value) = 0;

    virtual long GetLineSpace() = 0;
    virtual TPLineRule GetLineRule() = 0;

    virtual void SetLineSpace(TPLineRule lineSpaceRule, long lineSpace) = 0;

    virtual long GetListAlignment() = 0;
    virtual void SetListAlignment(long value) = 0;

    virtual long GetListLevelIndex() = 0;
    virtual void SetListLevelIndex(long value) = 0;

    virtual long GetListStart() = 0;
    virtual void SetListStart(long value) = 0;

    virtual long GetListTab() = 0;
    virtual void SetListTab(long value) = 0;

    virtual long GetListType() = 0;
    virtual void SetListType(long value) = 0;

    virtual long GetNoLineNumber() = 0;
    virtual void SetNoLineNumber(long value) = 0;

    virtual long GetRightIndent() = 0;
    virtual void SetRightIndent(long value) = 0;

    virtual long GetFirstLineIndent() = 0;
    virtual long GetLeftIndent() = 0;

    virtual void SetIndents(long startIndent, long leftIndent, long rightIndent) = 0;
    
    virtual long GetSpaceAfter() = 0;
    virtual void SetSpaceAfter(long value) = 0;

    virtual long GetSpaceBefore() = 0;
    virtual void SetSpaceBefore(long value) = 0;
};

class ITextCallback
{
public:

    virtual ~ITextCallback() {}

    virtual void NotifyTextChanged() = 0;
    virtual void NotifyTextFontChanged(int flag) = 0;
    virtual void NotifyTextSelectionChanged() = 0;
};

class ITextRender
{
public:

    virtual ~ITextRender() {}

    virtual void Render(suic::Drawing* drawing) = 0;

    virtual TextFont* GetTextFont() = 0;

    virtual suic::Rect GetViewRect() const = 0;
    virtual suic::Rect GetPadding() const = 0;
    virtual void SetPadding(suic::Rect val) = 0;

    virtual void RefleshView() = 0;

    virtual bool IsHoriScrollBarVisible() const = 0;
    virtual bool IsVertScrollBarVisible() const = 0;
};

/// <summary>
/// �����ı���Ĳ����ӿڣ��ڴ��ı����ڣ���ʽ����ͬ
/// </summary>
class SUIWGX_API TextRange : public suic::Object
{
public:

    virtual ~TextRange() {}

    virtual int IsEqual(TextRange* Other) = 0;

    /// <summary>
    ///  ��ȡ�ı�����
    /// </summary>
    /// <remarks>
    ///  �˷�����һ���Ի�ȡ�ڴ˷�Χ�ڵ��ı�
    /// </remarks>
    /// <returns>�ı�����</returns> 
    virtual suic::String GetText() = 0;

    /// <summary>
    ///  �����ı�����
    /// </summary>
    /// <remarks>
    ///  �˷������滻���ı������������
    /// </remarks>
    /// <param name="text">�ı�����</param>
    /// <returns>��</returns> 
    virtual void SetText(suic::String text) = 0;

    /// <summary>
    ///  ��ȡ���ı����Ƿ�����
    /// </summary>
    /// <returns>�ı����Ƿ�����</returns> 
    virtual bool GetHidden() = 0;

    /// <summary>
    ///  ���ô��ı����Ƿ�����
    /// </summary>
    /// <remarks>
    ///  �������ô˷�������ĳ���ı��Ƿ���ʾ������
    /// </remarks>
    /// <param name="val">true:�����ı�;������ʾ</param>
    /// <returns>��</returns> 
    virtual void SetHidden(bool val) = 0;

    // ������������δʵ��
    virtual void Collapse (long start) = 0;
    virtual void Expand (long unit, long *delta) = 0;

    /// <summary>
    ///  ѡ�д��ı���
    /// </summary>
    /// <returns>��</returns> 
    virtual void Select() = 0;
    virtual TextRange* GetClone() = 0;
    
    /// <summary>
    ///  ��ȡ���ı����������Ϣ
    /// </summary>
    /// <remarks>
    ///  ���û�ȡ��������Ϣ���������ı�����������
    /// </remarks>
    /// <returns>������Ϣ����</returns> 
    virtual TextFont* GetTextFont() = 0;
    virtual TextPara* GetTextPara() = 0;

    virtual int FindText(suic::String text, long cch, long flags) = 0;
    virtual void Delete(int index, int count) = 0;

    virtual bool CanEdit() = 0;
    virtual bool CanPaste() = 0;
    virtual void Cut() = 0;
    virtual void Copy() = 0;
    virtual void Paste() = 0;

    virtual suic::Rect GetBound() = 0;
    virtual suic::Point GetPoint(bool bStart) = 0;
    virtual int GetStart() = 0;
    virtual void SetStart(int index) = 0;

    virtual int GetEnd() = 0;
    virtual void SetEnd(int index) = 0;

    virtual void ScrollIntoView(long val) = 0;

    virtual long Move(int iUnit, int iCount) = 0;
    virtual long MoveStart(int iUnit, int iCount) = 0;
    virtual long MoveEnd(int iUnit, int iCount) = 0;
};

/// <summary>
/// ����ѡ���ı��Ĳ����ӿڣ��ڴ��ı����ڣ���ʽ����ͬ
/// �����ô˽ӿڲ���ѡ�е��ı���
/// </summary>
class SUIWGX_API TextSel : public TextRange
{
public:

    virtual ~TextSel() {}

    virtual long MoveLeft(int iUnit, int iCount, int iExtend) = 0;
    virtual long MoveRight(int iUnit, int iCount, int iExtend) = 0;
    virtual long MoveUp(int iUnit, int iCount, int iExtend) = 0;
    virtual long MoveDown(int iUnit, int iCount, int iExtend) = 0;
};

struct LineDetail
{
    int xCount;
    int xLeft;
    int xWidth;
    int xHeight;
    int xWhite;
    int xInPara;
};

class SUIWGX_API ITextDoc : public suic::Object
{
public:

    virtual ~ITextDoc() {}

    virtual void Undo(int layer) = 0;
    virtual void Redo(int layer) = 0;

    /// <summary>
    ///  ��ȡ�ĵ���Χ������������Ϣ
    /// </summary>
    /// <remarks>
    ///  �˷�����ȡ��������Ϣ�����ĵ�Ĭ�ϣ���ǰ������Ϣ��
    /// </remarks>
    /// <param name="val">true:�����ı�;������ʾ</param>
    /// <returns>��</returns> 
    virtual TextFont* GetTextFont() = 0;
    virtual TextPara* GetTextPara() = 0;

    virtual void Clear() = 0;

    virtual void GetStream(suic::ISStream* stm, bool atSel) = 0;
    virtual void SetStream(const suic::ISStream* stm, bool atSel) = 0;

    virtual TextSel* GetTextSel() = 0;
    virtual TextRange* GetRange(int index, int count) = 0;
    virtual TextRange* GetRangeFromPoint(int x, int y) = 0;

    virtual bool Redo() = 0;
    virtual bool Undo() = 0;
    virtual void Copy() = 0;
    virtual void Cut() = 0;
    virtual void Paste() = 0;

    virtual bool IsWordWrapping() = 0;
    virtual void SetWordWrapping(bool val) = 0;

    virtual bool IsReadOnly() = 0;
    virtual void SetReadOnly(bool val) = 0;

    virtual long GetCharCount() const = 0;
    virtual suic::String GetText() const = 0;

    virtual void SetText(suic::String strText, bool bCanUndo) = 0;
    virtual void AddText(suic::String strText, bool bCanUndo) = 0;

    virtual int GetLineCount() const = 0;
    virtual int GetLineLength(int line) const = 0;
    virtual suic::String GetLineText(int index) const = 0;

    /// <summary>
    ///  ѡ��ȫ���ı�����
    /// </summary>
    /// <returns>ѡ���ַ���</returns> 
    virtual int SelectAll() = 0;

    /// <summary>
    ///  ѡ��ָ����Χ���ı�
    /// </summary>
    /// <remarks>
    ///  lStart��lCount������ڵ���0
    /// </remarks>
    /// <param name="lStart">��ʼ�ı�����</param>
    /// <param name="lCount">ѡ�е��ı��ַ���</param>
    /// <returns>С��0�������󣻷���ѡ���ַ���</returns> 
    virtual int Select(long lStart, long lCount) = 0;

    /// <summary>
    ///  �ƶ���굽�ĵ���ʼ
    /// </summary>
    /// <remarks>
    /// <returns>������</returns> 
    virtual void SelectStart() = 0;

    /// <summary>
    ///  �ƶ���굽�ĵ�ĩβ
    /// </summary>
    /// <remarks>
    /// <returns>������</returns> 
    virtual void SelectEnd() = 0;

    virtual void RecalcView(bool bForce) = 0;
    virtual suic::Point GetPoint(suic::Uint32 cp, bool bLeftTop) = 0;
    virtual void GetLineDetail(int index, LineDetail* info, int* iYPos) = 0;

    virtual int GetSelLength() const = 0;
    virtual suic::String GetSelText() const = 0;

    virtual int LineFromCharIndex(int ch) const = 0;
    virtual int CharIndexFromLine(int line) const = 0;

    virtual suic::Point PosFromCharIndex(int ch) const = 0;
    virtual int CharIndexFromPos(suic::Point pt) const = 0;

    virtual int GetFirstVisibleLine() const = 0;
    virtual int GetFirstVisibleCharIndex() const = 0;
    virtual int GetLastVisibleCharIndex() const = 0;

    virtual bool SetZoom(int nNum, int nDen) = 0;
    virtual bool SetZoomOff() = 0;
};

}

#endif
