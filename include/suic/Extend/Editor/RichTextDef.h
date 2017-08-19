// 华勤科技版权所有 2010-2012
// 
// 文件名：RicheditDef.h
// 功  能：富文本接口定义。
// 
// 作  者：MPF开发组
// 时  间：2011-05-01
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
/// 定义富文本字体操作接口
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
/// 定义文本段落格式设置接口
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
/// 定义文本块的操作接口，在此文本块内，格式都相同
/// </summary>
class SUIWGX_API TextRange : public suic::Object
{
public:

    virtual ~TextRange() {}

    virtual int IsEqual(TextRange* Other) = 0;

    /// <summary>
    ///  获取文本内容
    /// </summary>
    /// <remarks>
    ///  此方法会一次性获取在此范围内的文本
    /// </remarks>
    /// <returns>文本内容</returns> 
    virtual suic::String GetText() = 0;

    /// <summary>
    ///  设置文本内容
    /// </summary>
    /// <remarks>
    ///  此方法会替换此文本块的所有内容
    /// </remarks>
    /// <param name="text">文本内容</param>
    /// <returns>无</returns> 
    virtual void SetText(suic::String text) = 0;

    /// <summary>
    ///  获取此文本块是否隐藏
    /// </summary>
    /// <returns>文本块是否隐藏</returns> 
    virtual bool GetHidden() = 0;

    /// <summary>
    ///  设置此文本块是否隐藏
    /// </summary>
    /// <remarks>
    ///  可以利用此方法设置某块文本是否显示或隐藏
    /// </remarks>
    /// <param name="val">true:隐藏文本;否则显示</param>
    /// <returns>无</returns> 
    virtual void SetHidden(bool val) = 0;

    // 下面两个方法未实现
    virtual void Collapse (long start) = 0;
    virtual void Expand (long unit, long *delta) = 0;

    /// <summary>
    ///  选中此文本块
    /// </summary>
    /// <returns>无</returns> 
    virtual void Select() = 0;
    virtual TextRange* GetClone() = 0;
    
    /// <summary>
    ///  获取此文本块的字体信息
    /// </summary>
    /// <remarks>
    ///  利用获取的字体信息可以设置文本的字体属性
    /// </remarks>
    /// <returns>字体信息对象</returns> 
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
/// 定义选中文本的操作接口，在此文本块内，格式都相同
/// 可利用此接口操作选中的文本块
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
    ///  获取文档范围的字体设置信息
    /// </summary>
    /// <remarks>
    ///  此方法获取的字体信息属于文档默认（当前字体信息）
    /// </remarks>
    /// <param name="val">true:隐藏文本;否则显示</param>
    /// <returns>无</returns> 
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
    ///  选中全部文本内容
    /// </summary>
    /// <returns>选中字符数</returns> 
    virtual int SelectAll() = 0;

    /// <summary>
    ///  选中指定范围的文本
    /// </summary>
    /// <remarks>
    ///  lStart和lCount必须大于等于0
    /// </remarks>
    /// <param name="lStart">开始文本索引</param>
    /// <param name="lCount">选中的文本字符数</param>
    /// <returns>小于0参数错误；否则选中字符数</returns> 
    virtual int Select(long lStart, long lCount) = 0;

    /// <summary>
    ///  移动光标到文档开始
    /// </summary>
    /// <remarks>
    /// <returns>无意义</returns> 
    virtual void SelectStart() = 0;

    /// <summary>
    ///  移动光标到文档末尾
    /// </summary>
    /// <remarks>
    /// <returns>无意义</returns> 
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
