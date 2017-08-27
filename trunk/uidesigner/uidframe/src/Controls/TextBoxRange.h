///
/// 文件名：TextBoxRange.h
///
/// 功  能：实现资源类型的编辑控件
///
/// 作  者：汪荣
///

#ifndef _TEXTBOXRANGE_H
#define _TEXTBOXRANGE_H

#include <Framework/Controls/RangeBase.h>

class TextBoxRange : public suic::RangeBase
{
public:

    RTTIOfClass(TextBoxRange)

    TextBoxRange();
    virtual ~TextBoxRange();

protected:

    void OnApplyTemplate();
    suic::Size OnMeasure(const suic::Size& constraint);
    void OnRenderSizeChanged(suic::SizeChangedInfo& e);

    void OnValueChanged(suic::Float oldValue, suic::Float newValue);
    void OnTextChanged(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnLostTextKeyboardFocus(suic::Element* sender, suic::KeyboardFocusEventArg* e);

    void OnRender(Drawing* drawing);

    void OnPreviewKeyDown(suic::KeyboardEventArg* e);
    void OnMouseMove(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(suic::MouseButtonEventArg* e);

    void SetTextValue(const String& val);
    void UpdateSelectionRange(suic::Float val);

    virtual void OnRangeChanged(suic::Float val);
    virtual void UpdateTextBox(suic::Float newValue);

private:

    bool _isPending;
    suic::Int32 _trackingPos;
    suic::Point _lastPt;
    suic::TextBox* _textBox;
    FrameworkElement* _selRangeElem;
    FrameworkElement* _indicatorElem;
};

#endif
