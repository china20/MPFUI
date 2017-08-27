
#include "stdafx.h"
#include <Controls/TextBoxRange.h>

ImplementRTTIOfClass(TextBoxRange, suic::RangeBase)

//==============================================
// TextBoxRange
TextBoxRange::TextBoxRange()
    : _isPending(false)
    , _textBox(NULL)
    , _indicatorElem(NULL)
    , _selRangeElem(NULL)
{
}

TextBoxRange::~TextBoxRange()
{

}

void TextBoxRange::OnRenderSizeChanged(suic::SizeChangedInfo& e)
{
    UpdateSelectionRange(suic::RangeBase::GetValue());
}

suic::Size TextBoxRange::OnMeasure(const suic::Size& constraint)
{
    if (NULL != _selRangeElem)
    {
        _selRangeElem->SetWidth(0);
    }
    suic::Size size = suic::RangeBase::OnMeasure(constraint);
    UpdateSelectionRange(suic::RangeBase::GetValue());
    return size;
}

void TextBoxRange::UpdateSelectionRange(suic::Float val) 
{
    suic::Int32 selWid = 0;

    if (NULL != _indicatorElem && _selRangeElem != NULL)
    {
        suic::Float fillWid = _indicatorElem->GetActualWidth();
        selWid = (suic::Int32)(fillWid * (val / GetMaximum()));
        _selRangeElem->SetWidth(selWid);
    }
}

void TextBoxRange::SetTextValue(const String& val)
{
    if (NULL != _textBox)
    {
        _textBox->SetText(val);
    }
}

void TextBoxRange::UpdateTextBox(suic::Float newValue)
{
    suic::String strText;
    if (NULL != _textBox)
    {
        strText.Format(_U("%g"), newValue);
        SetTextValue(strText);
    }
}

void TextBoxRange::OnApplyTemplate()
{
    _textBox = suic::RTTICast<suic::TextBox>(GetTemplateChild(_U("PART_TextBox")));
    _selRangeElem  = suic::RTTICast<suic::FrameworkElement>(GetTemplateChild(_U("PART_SelectionRange")));
    _indicatorElem  = suic::RTTICast<suic::FrameworkElement>(GetTemplateChild(_U("PART_Indicator")));
    if (NULL != _textBox)
    {
        _textBox->AddLostKeyboardFocus(new suic::KeyboardFocusEventHandler(this, &TextBoxRange::OnLostTextKeyboardFocus));
        _textBox->AddTextChanged(new suic::RoutedEventHandler(this, &TextBoxRange::OnTextChanged));
    }

    suic::Float val = RangeBase::GetValue();

    _isPending = true;
    UpdateTextBox(val);
    UpdateSelectionRange(val);
    _isPending = false;
}

void TextBoxRange::OnValueChanged(suic::Float oldValue, suic::Float newValue)
{
    if (!_isPending)
    {
        _isPending = true;
        UpdateTextBox(newValue);
        _isPending = false;
    }

    UpdateSelectionRange(newValue);
    suic::RangeBase::OnValueChanged(oldValue, newValue);

    OnRangeChanged(newValue);
}

void TextBoxRange::OnRangeChanged(suic::Float val)
{

}

void TextBoxRange::OnTextChanged(suic::DpObject* sender, RoutedEventArg* e)
{
    if (!_isPending)
    {
        suic::String strText = _textBox->GetText();

        suic::Float fMin = GetMinimum();
        suic::Float fMax = GetMaximum();
        suic::Float fCurr = strText.ToFloat();

        _isPending = true;

        // 需要校正值
        if (suic::FloatUtil::LessThan(fCurr, fMin) || 
            suic::FloatUtil::GreaterThan(fCurr, fMax))
        {
            UpdateTextBox(GetValue());
        }
        else
        {
            //SetValue(fCurr);
        }

        _isPending = false;
    }
}

void TextBoxRange::OnLostTextKeyboardFocus(suic::Element* sender, suic::KeyboardFocusEventArg* e)
{
    _textBox->SetVisibility(suic::Visibility::Hidden);
    _selRangeElem->SetVisibility(suic::Visibility::Visible);
    SetValue(_textBox->GetText().ToFloat());
    e->SetHandled(true);
}

void TextBoxRange::OnRender(Drawing* drawing)
{
}

void TextBoxRange::OnPreviewKeyDown(suic::KeyboardEventArg* e)
{
    suic::RangeBase::OnKeyDown(e);
    if (e->GetKey() == Key::kReturn)
    {
        ;
    }
}

void TextBoxRange::OnMouseMove(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);

    if (IsMouseCaptured() && NULL != _indicatorElem)
    {
        suic::Float fVal = 0;
        int fillWid = _indicatorElem->GetActualWidth();
        int xOffset = e->GetMousePoint().x - _lastPt.x;
        int iWidth = _trackingPos + xOffset;

        if (iWidth < 0)
        {
            iWidth = 0;
        }

        if (iWidth > fillWid)
        {
            iWidth = fillWid;
        }

        fVal = (suic::Float)(iWidth / (suic::Float)fillWid);
        SetValue(fVal * GetMaximum());
    }
}

void TextBoxRange::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
    Focus();
    SetCaptureMouse();

    _lastPt = e->GetMousePoint();
    if (NULL != _selRangeElem)
    {
        _trackingPos = _selRangeElem->GetActualWidth();
    }
}

void TextBoxRange::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
    ReleaseCaptureMouse();

    int xOffset = ::abs(_lastPt.x - e->GetMousePoint().x);
    int yOffset = ::abs(_lastPt.y - e->GetMousePoint().y);

    if (xOffset < 2 && yOffset < 2)
    {
        _selRangeElem->SetVisibility(suic::Visibility::Hidden);
        _textBox->SetVisibility(suic::Visibility::Visible);
        _textBox->Focus();
    }
}
