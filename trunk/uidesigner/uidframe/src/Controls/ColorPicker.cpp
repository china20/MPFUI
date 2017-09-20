
#include "stdafx.h"

#include "resource.h"

#include <Controls/ColorPicker.h>
#include <System/Input/Mouse.h>

ImplementRTTIOfClass(ColorSlider, suic::RangeBase)
ImplementRTTIOfClass(ColorSelector, suic::Control)
ImplementRTTIOfClass(ColorPicker, suic::Control)
ImplementRTTIOfClass(ColorPickerBox, suic::Control)
ImplementRTTIOfClass(ColorChannel, TextBoxRange)
ImplementRTTIOfClass(ColorButton, suic::ButtonBase)


DpProperty* ColorButton::ColorProperty;

suic::Brush* ColorSelector::_white;
suic::Brush* ColorSelector::_black;

suic::Brush* ColorSlider::_linearBrush;
suic::Float ColorSlider::_previousValue;

void ColorButton::StaticInit()
{
    if (NULL == ColorProperty)
    {
        ColorProperty = ColorProperty = suic::DpProperty::Register(_U("Color"), RTTIType(), suic::SolidColorBrush::RTTIType()
            , new suic::PropMetadata(new SolidColorBrush(suic::Colors::White), PropMetadataOptions::AffectsRender));
    }
}

ColorButton::ColorButton()
{

}

ColorButton::~ColorButton()
{

}

SolidColorBrush* ColorButton::GetColor()
{
    return RTTICast<SolidColorBrush>(GetValue(ColorProperty));
}

void ColorButton::SetColor(SolidColorBrush* brush)
{
    SetValue(ColorProperty, brush);
}

void ColorSelector::StaticInit()
{
    if (NULL == _white)
    {
        suic::LinearGradientBrush* brWhite = new suic::LinearGradientBrush();
        suic::LinearGradientBrush* brBlack = new suic::LinearGradientBrush();

        _white = brWhite;
        _black = brBlack;

        brWhite->SetStartPoint(fPoint(1, 0.5));
        brWhite->SetEndPoint(fPoint(0, 0.5));

        brWhite->AddGradientStop(0x00FFFFFF, 0);
        brWhite->AddGradientStop(0xFFFFFFFF, 1);

        brBlack->SetStartPoint(fPoint(0.5, 0));
        brBlack->SetEndPoint(fPoint(0.5, 1));

        brBlack->AddGradientStop(0x00FFFFFF, 0);
        brBlack->AddGradientStop(0xFF000000, 1);

        _white->ref();
        _black->ref();
    }
}

ColorSlider::ColorSlider()
{
}

void ColorSlider::OnThumbDragCompletedProp(DpObject* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
}

void ColorSlider::OnThumbDragDeltaProp(DpObject* sender, RoutedEventArg* e)
{
    ColorSlider* slider = RTTICast<ColorSlider>(sender);

    if (slider != NULL)
    {
        DragDeltaEventArg* dde = (DragDeltaEventArg*)e;
        suic::Float detaVal = (suic::Float)dde->GetVerticalChange() / (suic::Float)slider->GetActualHeight();
        suic::Float val = ColorSlider::_previousValue + detaVal;
        slider->SetValue(val);
    }
    e->SetHandled(true);
}

void ColorSlider::OnThumbDragStartedProp(DpObject* sender, RoutedEventArg* e)
{
    ColorSlider* slider = RTTICast<ColorSlider>(sender);

    if (slider != NULL)
    {
        ColorSlider::_previousValue = slider->GetValue();
    }
    e->SetHandled(true);
}

void ColorSlider::StaticInit()
{
    if (NULL == _linearBrush)
    {
        InitLinearGradientBrush();

        EventHelper::RegisterClassHandler(RTTIType(), Thumb::DragStartedEvent, new RoutedEventHandler(ColorSlider::OnThumbDragStartedProp), false);
        EventHelper::RegisterClassHandler(RTTIType(), Thumb::DragDeltaEvent, new RoutedEventHandler(ColorSlider::OnThumbDragDeltaProp), false);
        EventHelper::RegisterClassHandler(RTTIType(), Thumb::DragCompletedEvent, new RoutedEventHandler(ColorSlider::OnThumbDragCompletedProp), false);
    }
}

void ColorSlider::InitLinearGradientBrush()
{
    if (NULL == _linearBrush)
    {
        suic::LinearGradientBrush* linearGradientBrush = new suic::LinearGradientBrush();

        linearGradientBrush->SetStartPoint(fPoint(.5, 0));
        linearGradientBrush->SetEndPoint(fPoint(.5, 1));
        linearGradientBrush->AddGradientStop(Color::FromArgb(255,255,0,0),0.020f);
        linearGradientBrush->AddGradientStop(Color::FromArgb(255,255,255,0),0.167f);
        linearGradientBrush->AddGradientStop(Color::FromArgb(255,0,255,0),0.334f);
        linearGradientBrush->AddGradientStop(Color::FromArgb(255,0,255,255),0.501f);
        linearGradientBrush->AddGradientStop(Color::FromArgb(255,0,0,255),0.668f);
        linearGradientBrush->AddGradientStop(Color::FromArgb(255,255,0,255),0.835f);
        linearGradientBrush->AddGradientStop(Color::FromArgb(255,255,0,0),0.975f);

        _linearBrush = linearGradientBrush;
        _linearBrush->ref();
    }
}

suic::Color ColorSlider::GetColor()
{
    suic::Color color = suic::Colors::Red;
    suic::Float val = GetValue();
    HslColor hlsColor(val, 1.0f, 0.5f);

    color = hlsColor.ToColor();

    return color;
}

void ColorSlider::OnApplyTemplate()
{
    suic::Control::OnApplyTemplate();

    _thumb = RTTICast<suic::Thumb>(GetTemplateChild(_U("PART_Thumb")));
}

void ColorSlider::OnRender(suic::Drawing* drawing)
{
    suic::fRect rect(0, 0, GetActualWidth(), GetActualHeight());
    drawing->DrawRect(suic::DrawCtx::DefDraw, _linearBrush, NULL, &rect);
}

void ColorSlider::OnArrange(const suic::Size& arrangeSize)
{
    suic::Float val = GetValue();
    suic::Rect rect;
    suic::Size desiredSize;

    if (NULL != _thumb)
    {
        desiredSize = _thumb->GetDesiredSize();

        rect.top = arrangeSize.Height() * GetValue() - desiredSize.Height() / 2.0f;
        rect.bottom = rect.top + desiredSize.Height();
        rect.left = (arrangeSize.Width() - desiredSize.Width()) / 2.0f;
        rect.right = rect.left + desiredSize.Width();

        _thumb->Arrange(rect);
    }
}

void ColorSlider::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    Point pt = PointFromScreen(e->GetMousePoint());
    Float val = (Float)(pt.y) / GetActualHeight();
    SetValue(val);
    UpdateLayout();
    e->SetHandled(true);
}

void ColorSlider::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

ColorSelector::ColorSelector()
{
    _isMouseTracking = false;
    _background = new suic::SolidColorBrush(suic::Colors::Red);
    _background->ref();
}

ColorSelector::~ColorSelector()
{
    if (NULL != _background)
    {
        _background->unref();
    }
}

suic::Color ColorSelector::GetColor()
{
    HslColor color(_background->GetColor());
    suic::Rect rect(0, 0, GetActualWidth(), GetActualHeight());
    Float saturation = (Float)((Float)_colorPoint.x / (Float)rect.Width());
    Float luminance = GetLuminance((Float)((rect.Height() - _colorPoint.y) / (Float)rect.Height()), saturation);
    
    color = HslColor(color.GetHue(), saturation, luminance);

    return color.ToColor();
}

void ColorSelector::UpdateBackground(suic::Color color)
{
    _background->SetColor(color);
    InvalidateVisual();
}

Float ColorSelector::GetBrightness(Float saturation, Float luminance)
{
    return (luminance / (1.0f - (saturation * 0.5f)));
}

Float ColorSelector::GetLuminance(Float brightness, Float saturation)
{
    return (brightness * (1.0f - (saturation * 0.5f)));
}

void ColorSelector::UpdateIndicator(suic::Color color)
{
    HslColor hlsColor(color);
    double length = hlsColor.GetSaturation() * GetActualWidth();
    Float brightness = GetBrightness(hlsColor.GetSaturation(), hlsColor.GetLuminance());
    double num3 = GetActualHeight() - (GetActualHeight() * brightness);

    _colorPoint.x = length;
    _colorPoint.y = num3;

    InvalidateVisual();
}

void ColorSelector::UpdateCurrentPoint(fPoint point)
{
    point.x = (point.x < 0) ? 0 : ((point.x > GetActualWidth()) ? GetActualWidth() : point.x);
    point.y = (point.y < 0) ? 0 : ((point.y > GetActualHeight()) ? GetActualHeight() : point.y);

    _colorPoint = point;
    if (PositionChanged.safe_bool())
    {
        PositionChanged(this, &suic::EventArg::Empty);
    }
}

void ColorSelector::OnMouseMove(suic::MouseButtonEventArg* e)
{
    if (_isMouseTracking)
    {
        suic::Point pt = PointFromScreen(e->GetMousePoint());
        UpdateCurrentPoint(pt.TofPoint());
        InvalidateVisual();
    }
    e->SetHandled(true);
}

void ColorSelector::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);

    suic::Rect rect(0, 0, GetActualWidth(), GetActualHeight());
    suic::Point pt = PointToScreen(suic::Point());

    SetCaptureMouse();
    rect.Offset(pt);
    _isMouseTracking = true;

    if (rect.PointIn(e->GetMousePoint()))
    {
        pt.x = e->GetMousePoint().x - pt.x;
        pt.y = e->GetMousePoint().y - pt.y;
        UpdateCurrentPoint(pt.TofPoint());
    }
}

void ColorSelector::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
    _isMouseTracking = false;
    ReleaseCaptureMouse();
}

void ColorSelector::OnRender(suic::Drawing* drawing)
{
    suic::fRect rect(0, 0, GetActualWidth(), GetActualHeight());

    drawing->DrawRect(suic::DrawCtx::DefDraw, _background, NULL, &rect);
    drawing->DrawRect(suic::DrawCtx::DefDraw, _white, NULL, &rect);
    drawing->DrawRect(suic::DrawCtx::DefDraw, _black, NULL, &rect);

    drawing->Save();
    drawing->ClipRect(&rect, ClipOp::OpIntersect);
    suic::Pen pen;
    drawing->DrawCircle(suic::DrawCtx::DefDraw, NULL, &pen, _colorPoint, 5);
    drawing->Restore();
}

//==============================================
// ColorChannel

void ColorChannel::StaticInit()
{
}

ColorChannel::ColorChannel()
{
    SetMaximum(255);
}

void ColorChannel::SetChannel(suic::Byte val)
{
    suic::RangeBase::SetValue(val);
}

suic::Byte ColorChannel::GetChannel()
{
    return (suic::Byte)(suic::RangeBase::GetValue());
}

void ColorChannel::UpdateTextBox(suic::Float newValue)
{
    suic::String strText;
    strText.Format(_U("%d"), (suic::Byte)newValue);
    SetTextValue(strText);
}

void ColorChannel::OnRangeChanged(suic::Float val)
{
    CheckInputValue();
}

void ColorChannel::CheckInputValue()
{
    suic::Byte val = GetChannel();
    ColorPicker* clrPicker = RTTICast<ColorPicker>(GetTemplatedParent());
    if (NULL != clrPicker)
    {
        clrPicker->UpdateColorChannel(val);
    }
}

ColorPickerBox::ColorPickerBox()
{
    _cursor = suic::OCursor::Hand;
}

suic::OCursor* ColorPickerBox::GetCursor()
{
    if (NULL == _cursor)
    {
        _cursor = suic::RTTICast<suic::OCursor>(FindRes(_U("pickcursor")));
    }
    return _cursor;
}

void ColorPickerBox::OnSetCursor(suic::CursorEventArg* e)
{
    e->SetHandled(true);
    e->SetCursor(GetCursor());
    suic::Control::OnSetCursor(e);
}

void ColorPickerBox::OnMouseMove(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
    if (IsMouseCaptured())
    {
        GetCursor()->SetCursor();
        //::SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_PICKER)));
        ColorPicker* clrPicker = RTTICast<ColorPicker>(GetTemplatedParent());
        if (NULL != clrPicker)
        {
            POINT pt;
            HDC hdc = ::GetDC(NULL);
            ::GetCursorPos(&pt);
            COLORREF clr = ::GetPixel(hdc, pt.x, pt.y);
            ::ReleaseDC(NULL, hdc);

            clrPicker->SetColor(new suic::SolidColorBrush(suic::Color::FromRgb(clr)));
        }
    }
}

void ColorPickerBox::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
    SetCaptureMouse();
    _cursor = suic::RTTICast<suic::OCursor>(FindRes(_U("pickcursor")));
}

void ColorPickerBox::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
    ReleaseCaptureMouse();
    _cursor = suic::OCursor::Hand;
}

DpProperty* ColorPicker::ColorProperty;
DpProperty* ColorPicker::InitColorProperty;
DpProperty* ColorPicker::LastColorProperty;

RoutedEvent* ColorPicker::ColorChangedEvent;

void ColorPicker::StaticInit()
{
    if (NULL == InitColorProperty)
    {
        ColorProperty = suic::DpProperty::Register(_U("Color"), RTTIType(), suic::SolidColorBrush::RTTIType()
            , new suic::PropMetadata(new SolidColorBrush(suic::Colors::White), PropMetadataOptions::AffectsRender, &ColorPicker::OnColorPropChanged));
        InitColorProperty = suic::DpProperty::Register(_U("InitColor"), RTTIType(), suic::SolidColorBrush::RTTIType()
            , new suic::PropMetadata(new SolidColorBrush(suic::Colors::White), PropMetadataOptions::AffectsRender, &ColorPicker::OnInitColorPropChanged));
        LastColorProperty = suic::DpProperty::Register(_U("LastColor"), RTTIType(), suic::SolidColorBrush::RTTIType()
            , new suic::PropMetadata(new SolidColorBrush(suic::Colors::White), PropMetadataOptions::AffectsRender));

        ColorChangedEvent = suic::EventHelper::RegisterRoutedEvent(_U("ColorChanged"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
    }
}

void ColorPicker::OnColorPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    ColorPicker* colorPicker = RTTICast<ColorPicker>(d);
    if (NULL != colorPicker && !colorPicker->_initializingColor)
    {
        suic::SolidColorBrush* pColor = RTTICast<suic::SolidColorBrush>(e->GetNewValue());
        if (NULL != pColor)
        {
            suic::SolidColorBrush* lastColor = RTTICast<suic::SolidColorBrush>(e->GetOldValue());

            colorPicker->_updatingColor = true;
            colorPicker->SetValue(LastColorProperty, lastColor);
            colorPicker->UpdateColorInternal(pColor->GetColor());
            colorPicker->UpdateARGBColor(pColor->ToColor(), false);
            colorPicker->OnColorChanged(pColor);
            colorPicker->_updatingColor = false;
        }
    }
}

void ColorPicker::OnInitColorPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    ColorPicker* colorPicker = RTTICast<ColorPicker>(d);
    if (NULL != colorPicker)
    {
        SolidColorBrush* initColor = RTTICast<SolidColorBrush>(e->GetNewValue());

        colorPicker->SetLastColor(initColor);
        colorPicker->SetColor(initColor);
    }
}

ColorPicker::ColorPicker()
{
    _colorSelector = NULL;
    _colorSlider = NULL;
    _colorPick = NULL;
    _colorHex = NULL;

    _hueDraging = false;
    _updatingColor = false;
    
    _needNotify = false;
    _initializingColor = false;
}

void ColorPicker::UpdateCurrentColor(suic::Color color)
{
    suic::SolidColorBrush* clrBr = ColorPicker::GetColor();
    if (NULL != clrBr)
    {
        clrBr->SetColor(color);
        UpdateColor(color);
        UpdateARGBColor(color, false);
    }
}

suic::SolidColorBrush* ColorPicker::GetColor()
{
    return RTTICast<suic::SolidColorBrush>(GetValue(ColorProperty));
}

void ColorPicker::SetColor(suic::SolidColorBrush* color)
{
    SetValue(ColorProperty, color);
}

suic::SolidColorBrush* ColorPicker::GetInitColor()
{
    return RTTICast<suic::SolidColorBrush>(GetValue(InitColorProperty));
}

void ColorPicker::SetInitColor(suic::SolidColorBrush* color)
{
    SetValue(InitColorProperty, color);
}

suic::SolidColorBrush* ColorPicker::GetLastColor()
{
    return RTTICast<suic::SolidColorBrush>(GetValue(LastColorProperty));
}

void ColorPicker::AddColorChanged(RoutedEventHandler* h)
{
    AddHandler(ColorChangedEvent, h);
}

void ColorPicker::RemoveColorChanged(RoutedEventHandler* h)
{
    RemoveHandler(ColorChangedEvent, h);
}

void ColorPicker::OnColorChanged(suic::SolidColorBrush* color)
{
    if (_needNotify)
    {
        suic::RoutedEventArg e(this, ColorChangedEvent);
        RaiseEvent(&e);
    }
}

void ColorPicker::UpdateColorChannel(suic::Byte val)
{
    suic::Byte a;
    suic::Byte r;
    suic::Byte g;
    suic::Byte b;

    ColorChannel* rEdit = RTTICast<ColorChannel>(GetTemplateChild(_U("PART_R")));
    if (NULL != rEdit)
    {
        r = rEdit->GetChannel();
    }

    ColorChannel* gEdit = RTTICast<ColorChannel>(GetTemplateChild(_U("PART_G")));
    if (NULL != gEdit)
    {
        g = (suic::Byte)(gEdit->GetChannel());
    }

    ColorChannel* bEdit = RTTICast<ColorChannel>(GetTemplateChild(_U("PART_B")));
    if (NULL != bEdit)
    {
        b = (suic::Byte)(bEdit->GetChannel());
    }

    ColorChannel* aEdit = RTTICast<ColorChannel>(GetTemplateChild(_U("PART_A")));
    if (NULL != aEdit)
    {
        a = (suic::Byte)(aEdit->GetChannel());
    }

    SetColor(new SolidColorBrush(suic::Color::FromArgb(a, r, g, b)));
}

void ColorPicker::UpdateColor(suic::Color color)
{
    if (!_updatingColor)
    {
        UpdateColorInternal(color);
    }
}

void ColorPicker::UpdateColorInternal(suic::Color color)
{
    _initializingColor = true;

    suic::Color back = suic::Colors::Red;

    if (_colorSlider != NULL)
    {
        HslColor hlsClr(color);
        Float newVal = hlsClr.GetHue();

        if (!_hueDraging && !suic::FloatUtil::IsNaN(newVal))
        {
            _colorSlider->SetValue(newVal);
        }
        back = _colorSlider->GetColor();
    }
    if (_colorSelector != NULL)
    {
        _colorSelector->UpdateBackground(back);
        if (!_hueDraging)
        {
            _colorSelector->UpdateIndicator(color);
        }
    }

    _initializingColor = false;
}

void ColorPicker::SetLastColor(suic::SolidColorBrush* color)
{
    SetEqualValue(LastColorProperty, color);
}

void ColorPicker::OnPositionChanged(Object* sender, EventArg* e)
{
    if (_colorSelector != NULL)
    {
        SetColor(new SolidColorBrush(_colorSelector->GetColor()));
    }
}

void ColorPicker::OnHueValueChanged(Element* sender, FloatPropChangedEventArg* e)
{
    e->SetHandled(true);
    if (_colorSelector != NULL && !_updatingColor && !_initializingColor)
    {
        _hueDraging = true;
        _colorSelector->UpdateBackground(_colorSlider->GetColor());
        SetColor(new SolidColorBrush(_colorSelector->GetColor()));
        _hueDraging = false;
    }
}

void ColorPicker::OnLoaded(LoadedEventArg* e)
{
    UpdateColor(GetColor()->GetColor());
}

void ColorPicker::OnRenderSizeChanged(SizeChangedInfo& e)
{
    UpdateColor(GetColor()->GetColor());
}

void ColorPicker::OnLastColorClick(Element* sender, RoutedEventArg* e)
{
    SolidColorBrush* lastBrush = GetLastColor();
    SetValue(ColorProperty, lastBrush);
    e->SetHandled(true);
}

void ColorPicker::OnInitColorClick(Element* sender, RoutedEventArg* e)
{
    SolidColorBrush* initBrush = GetInitColor();
    SetValue(ColorProperty, initBrush);
    e->SetHandled(true);
}

void ColorPicker::OnHexColorChanged(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    if (!_updatingColor)
    {
        suic::String strHex = _colorHex->GetText().Trim();

        if (strHex.Length() >= 8)
        {
            strHex = _U("#") + strHex.Substring(0, 8);
            suic::Color clr = suic::Color::Parse(strHex);
            suic::String strLast = suic::Color::ToHex(clr).Substring(1);

            if (strLast.Length() == 6)
            {
                strLast = _U("FF") + strLast;
            }

            if (strLast.CompareI(strHex.Substring(1)) == 0)
            {
                SetColor(new suic::SolidColorBrush(clr));
            }
        }
    }
}

void ColorPicker::OnPickColorButtonChanged(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
}

void ColorPicker::OnApplyTemplate()
{
    suic::Control::OnApplyTemplate();

    ColorButton* prevBtn = RTTICast<ColorButton>(GetTemplateChild(_U("PART_Last")));
    if (NULL != prevBtn)
    {
        prevBtn->AddClick(new ClickEventHandler(this, &ColorPicker::OnLastColorClick));
    }

    ColorButton* initBtn = RTTICast<ColorButton>(GetTemplateChild(_U("PART_Init")));
    if (NULL != initBtn)
    {
        initBtn->AddClick(new ClickEventHandler(this, &ColorPicker::OnInitColorClick));
    }

    _colorHex = RTTICast<suic::TextBox>(GetTemplateChild(_U("PART_HexColor")));
    if (NULL != _colorHex)
    {
        _colorHex->AddTextChanged(new suic::RoutedEventHandler(this, &ColorPicker::OnHexColorChanged));
    }

    _colorPick = RTTICast<ColorPickerBox>(GetTemplateChild(_U("PART_PickColor")));
    _colorSelector = RTTICast<ColorSelector>(GetTemplateChild(_U("PART_Selector")));
    _colorSlider  = RTTICast<ColorSlider>(GetTemplateChild(_U("PART_ColorSlider")));

    if (_colorSlider != NULL)
    {
        _colorSlider->AddValueChanged(new FloatPropChangedEventHandler(this, &ColorPicker::OnHueValueChanged));
    }

    if (_colorSelector != NULL)
    {
        _colorSelector->PositionChanged += EventHandler(this, &ColorPicker::OnPositionChanged);
    }

    UpdateColor(GetColor()->GetColor());
    UpdateARGBColor(GetColor()->GetColor(), true);

    _needNotify = true;
}

void ColorPicker::UpdateARGBColor(suic::Color color, bool bForce)
{
    ColorChannel* rEdit = RTTICast<ColorChannel>(GetTemplateChild(_U("PART_R")));
    if (NULL != rEdit)
    {
        rEdit->SetChannel(suic::Color::R(color));
    }
    ColorChannel* gEdit = RTTICast<ColorChannel>(GetTemplateChild(_U("PART_G")));
    if (NULL != gEdit)
    {
        gEdit->SetChannel(suic::Color::G(color));
    }
    ColorChannel* bEdit = RTTICast<ColorChannel>(GetTemplateChild(_U("PART_B")));
    if (NULL != bEdit)
    {
        bEdit->SetChannel(suic::Color::B(color));
    }
    ColorChannel* aEdit = RTTICast<ColorChannel>(GetTemplateChild(_U("PART_A")));
    if (NULL != aEdit)
    {
        aEdit->SetChannel(suic::Color::A(color));
    }

    if (_colorHex != NULL)
    {
        suic::String strHex = Color::ToHex(color).Substring(1);
        if (strHex.Length() == 6)
        {
            strHex = _U("FF") + strHex;
        }
        _colorHex->SetText(strHex);
    }
}
