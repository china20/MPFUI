// 华勤科技版权所有 2010-2011
// 
// 文件名：TextElement.h
// 功  能：实现标准的win32编辑控件。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UITEXTELEMENT_H_
#define _UITEXTELEMENT_H_

#include <Framework/Controls/TextElement.h>
#include <Framework/Controls/TextEditDoc.h>
#include <Framework/Controls/CaretElement.h>

namespace suic
{

typedef delegate<void(int,int,bool)> TextCaretChangedEventHandler;

class SUICORE_API TextSimple : public TextElement
{
public:

    static DpProperty* PasswordCharProperty;
    static DpProperty* TextProperty;

    static RoutedEvent* TextChangedEvent;

    static bool StaticInit();

    static void OnTextPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnPasswordCharPropChanged(DpObject* d, DpPropChangedEventArg* e);

    TextCaretChangedEventHandler TextCaretChangedEvent;

    TextSimple();
    ~TextSimple();

    RTTIOfClass(TextSimple)

    void AddTextChanged(RoutedEventHandler* e);
    void RemoveTextChanged(RoutedEventHandler* e);

public:

    String GetText();
    void SetText(String text);

    void AppendText(String& text);
    void GetText(String& text);

    Char GetPasswordChar();
    String GetMaskText();

    bool GetSingleLine();
    bool GetReadOnly();

// 属性访问定义
public:

    /// <summary>
    ///     正则表达式检测
    /// </summary>
    String GetRegularExp() const;
    void SetRegularExp(String val);

    void InitFormattedText(FormattedText* fmtText);

public:

    Rect DocumentRect(Point pt);

    virtual void Cut();
    virtual void Copy();
    virtual void Paste();
    virtual void SelectAll();

public:

    Size OnMeasure(const Size& size);
    void OnArrange(const Size& arrangeSize);

    void OnRender(Drawing * drawing);

    void OnTextInput(KeyboardEventArg* e);
    void OnKeyDown(KeyboardEventArg* e);
    void OnKeyUp(KeyboardEventArg* e);

    void OnMouseMove(MouseButtonEventArg* e);

    void OnGotFocus(RoutedEventArg* e);
    void OnLostFocus(RoutedEventArg* e);

    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseLeftButtonDbclk(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);

    void OnMouseRightButtonDown(MouseButtonEventArg* e);

public:

    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

    void OnInitialized(EventArg* e);

    void ResetCaretPos(bool bUpDown=false);

protected:

    void UpdateScrollInfo(bool bUpdate);
    void RaiseTextChanged(String oldText);

    void NotifyHorizontalOffset(int val);
    void NotifyVerticalOffset(int val);

    friend class TextBoxDoc;

protected:

    // 编辑框委托实现
    TextBoxDoc _document;
    CaretElement _caret;
    String _inChars;

    Char _password;
};

inline Char TextSimple::GetPasswordChar()
{
    return _password;
}

inline String TextSimple::GetMaskText()
{
    return String();
}

};

#endif