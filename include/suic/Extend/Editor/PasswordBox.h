// 华勤科技版权所有 2008-2022
// 
// 文件名：PasswordBox.h
// 功  能：实现文本编辑框
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// ============================================================================

#ifndef _UIPASSWORDBOXT_H_
#define _UIPASSWORDBOXT_H_

#include <Framework/Controls/Control.h>
#include <Extend/Editor/TextContainer.h>

namespace suic
{

/// <summary>
///  PasswordBox，实现密码控件。
/// </summary>
class SUIWGX_API PasswordBox : public suic::Control
                             , public ITextCallback
{
public:

    RTTIOfClass(PasswordBox)

    static suic::DpProperty* PasswordCharProperty;
    static suic::DpProperty* SelectionBrushProperty;
    static suic::DpProperty* SelectionTextBrushProperty;

    static suic::RoutedEvent* PasswordChangedEvent;

    static void OnPasswordCharPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnSelectionBrushPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnSelectionTextBrushPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static void OnPaddingPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnForegroundPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnFontFamilyPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnFontWeightPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnFontSizePropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static bool StaticInit();

    PasswordBox();
    virtual ~PasswordBox();

    suic::String GetPassword();
    void SetPassword(suic::String val);

    suic::Char GetPasswordChar();
    void SetPasswordChar(suic::Char ch);

    void AddPasswordChanged(suic::RoutedEventHandler* h);
    void RemovePasswordChanged(suic::RoutedEventHandler* h);

    void NotifyTextChanged();
    void NotifyTextFontChanged(int flag);
    void NotifyTextSelectionChanged();

protected:

    void OnApplyTemplate();

    void OnTextInput(suic::KeyboardEventArg* e);
    void OnKeyDown(suic::KeyboardEventArg* e);
    void OnKeyUp(suic::KeyboardEventArg* e);

    void OnMouseEnter(suic::MouseButtonEventArg* e);
    void OnMouseMove(suic::MouseButtonEventArg* e);
    void OnMouseLeave(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDbclk(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(suic::MouseButtonEventArg* e);

    void OnGotKeyboardFocus(suic::KeyboardFocusEventArg* e);
    void OnLostKeyboardFocus(suic::KeyboardFocusEventArg* e);

    void OnPasswordChanged(suic::TextChangedEventArg* e);

private:

    suic::Byte _textPending;
    suic::TextContainer* _textView;
    suic::ScrollViewer* _scrollViewer;
};

typedef suic::shared<PasswordBox> PasswordBoxPtr;

}

#endif
