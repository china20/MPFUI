// 华勤科技版权所有 2008-2022
// 
// 文件名：TextBoxBase.h
// 功  能：居于TextBoxBase实现标准的富文本控件
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// ============================================================================

#ifndef _UITEXTBOXBASE_H_
#define _UITEXTBOXBASE_H_

#include <Extend/Editor/RichTextDef.h>
#include <Extend/Editor/TextContainer.h>
#include <Framework/Controls/TextEditor.h>
#include <Framework/Controls/TextElement.h>

namespace suic
{

/// <summary>
///  TextBoxBase，可编辑文本的基类。
/// </summary>
class SUIWGX_API TextBoxBase : public suic::TextEditor,
                               public ITextCallback
{
public:

    static suic::DpProperty* UndoLimitProperty;    
    static suic::DpProperty* IsUndoEnabledProperty;
    static suic::DpProperty* TextDecorationsProperty;
    static suic::DpProperty* AcceptsTabProperty;
    static suic::DpProperty* AcceptsReturnProperty;
    static suic::DpProperty* AutoWordSelectionProperty;

    static suic::RoutedCommand* UndoCommand;
    static suic::RoutedCommand* RedoCommand;

    static suic::RoutedCommand* CutCommand;
    static suic::RoutedCommand* CopyCommand;
    static suic::RoutedCommand* PasteCommand;
    static suic::RoutedCommand* SelectAllCommand;

    static void OnHandleCommand(suic::Object* target, suic::ExecutedRoutedEventArg* e);

    static void OnAcceptsReturnPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnAutoWordSelectionPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnAcceptsTabPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnIsUndoEnabledPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnUndoLimitPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static void OnPaddingPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnForegroundPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnFontFamilyPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnFontWeightPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnFontSizePropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnFontStylePropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static void OnAllowDropPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnTextDecorationsPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static bool StaticInit();

    RTTIOfAbstractClass(TextBoxBase)

    TextBoxBase();
    virtual ~TextBoxBase();

    bool Undo();
    bool Redo();
    
    bool GetAcceptsReturn();
    void SetAcceptsReturn(bool val);

    bool GetAcceptsTab();
    void SetAcceptsTab(bool val);

    bool GetAutoWordSelection();
    void SetAutoWordSelection(bool val);

    suic::TextDecorationCollection* GetTextDecorations();
    void SetTextDecorations(suic::TextDecorationCollection* val);

    bool CanRedo();
    bool CanUndo();

    //---------------------------------------------------------

    void Select(int start, int length);

    bool IsUndoEnabled();
    void SetIsUndoEnabled(bool val);

    int GetUndoLimit();
    void SetUndoLimit(int val);

    //---------------------------------------------------

    void SetCaretPos(int x, int y);
    void ShowCaret(bool fShow);

    void NotifyTextChanged();
    void NotifyTextFontChanged(int flag);
    void NotifyTextSelectionChanged();

    TextContainer* GetTextContainer();

public:

    void Cut();
    void Copy();
    void Paste();
    void SelectAll();

protected:

    void OnScrollToPos(suic::eScrollType iType, int iFlag);

    void OnApplyTemplate();

    void OnLoaded(suic::LoadedEventArg* e);

    void OnGotKeyboardFocus(suic::KeyboardFocusEventArg* e);
    void OnLostKeyboardFocus(suic::KeyboardFocusEventArg* e);

    void OnMouseEnter(suic::MouseButtonEventArg* e);
    void OnMouseMove(suic::MouseButtonEventArg* e);
    void OnMouseLeave(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDbclk(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(suic::MouseButtonEventArg* e);

    void OnTextInput(suic::KeyboardEventArg* e);
    void OnImeEvent(suic::MessageEventArg* e);
    void OnKeyDown(suic::KeyboardEventArg* e);
    void OnKeyUp(suic::KeyboardEventArg* e);

    void OnDragEnter(suic::DragEventArg* e);
    void OnDragOver(suic::DragEventArg* e);
    void OnDragLeave(suic::DragEventArg* e);
    void OnDrop(suic::DragEventArg* e);

    void OnIsReadOnlyChanged(suic::DpPropChangedEventArg* e);
    void OnIsReadOnlyCaretVisibleChanged(suic::DpPropChangedEventArg* e);
    void OnSelectionBrushChanged(suic::DpPropChangedEventArg* e);
    void OnSelectionTextBrushChanged(suic::DpPropChangedEventArg* e);
    void OnTextWrappingChanged(suic::DpPropChangedEventArg* e);

    void OnTextChanged(suic::TextChangedEventArg* e);
    void OnCaretBrushChanged(suic::Brush* brush);
    void OnScrollChanged(suic::Element* sender, suic::ScrollChangedEventArg* e);

protected:

    virtual void InitTextHost();
    virtual TextContainer* CreateTestHost();

private:

    suic::Byte _textPending;
    TextContainer* _textContainer;
};

}

#endif
