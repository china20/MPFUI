// 华勤科技版权所有 2010-2011
// 
// 文件名：TextBase.h
// 功  能：居于TextBase实现标准的富文本控件
// 
// 作  者：Sharpui开发组
// 时  间：2010-08-31
// 
// ============================================================================

#ifndef _UITEXTEBASE_H_
#define _UITEXTEBASE_H_

#include <System/Windows/TextInput.h>
#include <Framework/Controls/Control.h>
#include <Framework/Controls/ScrollViewer.h>

namespace suic
{

/// <summary>
///  TextEdit，可编辑文本的基类。
/// </summary>
class SUICORE_API TextEdit : public Control
{
public:

    static DpProperty* TextProperty;
    static DpProperty* CaretBrushProperty;
    static DpProperty* TextWrappingProperty;

    static DpProperty* HorizontalScrollBarVisibilityProperty;
    static DpProperty* VerticalScrollBarVisibilityProperty;

    static DpProperty* IsReadOnlyCaretVisibleProperty;
    static DpProperty* IsReadOnlyProperty;
    static DpProperty* SelectionBrushProperty;
    static DpProperty* SelectionTextBrushProperty;

    static RoutedEvent* SelectionChangedEvent;    
    static RoutedEvent* TextChangedEvent;

    static void OnTextPropChanged(DpObject* d, DpPropChangedEventArg& e);
    static void OnIsReadOnlyPropChanged(DpObject* d, DpPropChangedEventArg& e);
    static void OnIsReadOnlyCaretVisiblePropChanged(DpObject* d, DpPropChangedEventArg& e);
    static void OnSelectionBrushPropChanged(DpObject* d, DpPropChangedEventArg& e);
    static void OnSelectionTextBrushPropChanged(DpObject* d, DpPropChangedEventArg& e);
    static void OnTextWrappingPropChanged(DpObject* d, DpPropChangedEventArg& e);

    static bool StaticInit();

    RTTIOfAbstractClass(TextEdit)

    TextEdit();
    virtual ~TextEdit();

    String GetText();
    void SetText(const String& text);

    void LineDown();
    void LineLeft();
    void LineRight();
    void LineUp();

    void PageDown();
    void PageLeft();
    void PageRight();
    void PageUp();

    void ScrollToEnd();
    void ScrollToHome();
    void ScrollToHorizontalOffset(int offset);
    void ScrollToVerticalOffset(int offset);

    void ScrollToLine(int lineIndex);

    ScrollBarVisibility GetHorizontalScrollBarVisibility();
    void SetHorizontalScrollBarVisibility(ScrollBarVisibility val);

    ScrollBarVisibility GetVerticalScrollBarVisibility();
    void SetVerticalScrollBarVisibility(ScrollBarVisibility val);

    void Cut();
    void Copy();
    void Paste();

    Brush* GetCaretBrush();
    void SetCaretBrush(Brush* val);

    Brush* GetSelectionBrush();
    void SetSelectionBrush(Brush* val);

    int GetExtentHeight();
    int GetExtentWidth();

    int GetViewportHeight();
    int GetViewportWidth();

    int GetHorizontalOffset();
    int GetVerticalOffset();

    bool IsReadOnly();
    void SetIsReadOnly(bool val);

    bool IsReadOnlyCaretVisible();
    void SetIsReadOnlyCaretVisible(bool val);

    Float GetSelectionOpacity();
    void SetSelectionOpacity(Float val);

    bool GetTextWrapping();
    void SetTextWrapping(bool val);

    //---------------------------------------------------

    virtual void OnSelectionChanged(RoutedEventArg& e);
    virtual void OnTextChanged(TextChangedEventArg& e);

    virtual void OnScrollChanged(Element* sender, ScrollChangedEventArg& e);

    virtual void OnIsReadOnlyChanged(DpPropChangedEventArg& e);
    virtual void OnIsReadOnlyCaretVisibleChanged(DpPropChangedEventArg& e);
    virtual void OnSelectionBrushChanged(DpPropChangedEventArg& e);
    virtual void OnSelectionTextBrushChanged(DpPropChangedEventArg& e);
    virtual void OnTextWrappingChanged(DpPropChangedEventArg& e);

protected:

    virtual void OnCut();
    virtual void OnPaste();
    virtual void OnCopy();

    virtual void OnScrollToPos(eScrollType iType, int iFlag);

    virtual void OnTextChanged();
    virtual void OnTextSelectionChanged();

protected:

    ScrollViewer* _scrollViewer;
};

}

#endif
