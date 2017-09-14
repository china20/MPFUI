// 华勤科技版权所有 2010-2011
// 
// 文件名：ColorPicker.h
// 功  能：颜色选择器。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _COLORPICKER_H_
#define _COLORPICKER_H_

#include <Controls/HlsColor.h>
#include <Controls/TextBoxRange.h>
#include <Framework/Controls/Thumb.h>

class ColorSlider : public suic::RangeBase
{
public:

    static void OnThumbDragCompletedProp(DpObject* sender, RoutedEventArg* e);
    static void OnThumbDragDeltaProp(DpObject* sender, RoutedEventArg* e);
    static void OnThumbDragStartedProp(DpObject* sender, RoutedEventArg* e);

    static void StaticInit();

    RTTIOfClass(ColorSlider)

    ColorSlider();

    suic::Color GetColor();

protected:

    static void InitLinearGradientBrush();

    void OnApplyTemplate();

    void OnArrange(const Size& arrangeSize);
    void OnRender(suic::Drawing* drawing);

    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(suic::MouseButtonEventArg* e);

private:

    suic::Thumb* _thumb;
    static suic::Float _previousValue;
    static suic::Brush* _linearBrush;
};

class ColorSelector : public suic::Control
{
public:

    static void StaticInit();

    suic::EventHandler PositionChanged;

    RTTIOfClass(ColorSelector)

    ColorSelector();
    ~ColorSelector();

    suic::Color GetColor();
    void UpdateBackground(suic::Color color);
    void UpdateIndicator(suic::Color color);

protected:

    void OnRender(suic::Drawing* drawing);

    void OnMouseMove(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(suic::MouseButtonEventArg* e);

    Float GetBrightness(Float saturation, Float luminance);
    Float GetLuminance(Float brightness, Float saturation);

    void UpdateCurrentPoint(fPoint point);

    static void AddColorSelectorGradient();

private:

    static suic::Brush* _white;
    static suic::Brush* _black;

    bool _isMouseTracking;
    suic::fPoint _colorPoint;
    suic::SolidColorBrush* _background;
};

class ColorButton : public suic::ButtonBase
{
public:

    static DpProperty* ColorProperty;
    static void StaticInit();

    ColorButton();
    virtual ~ColorButton();

    RTTIOfClass(ColorButton)

    SolidColorBrush* GetColor();
    void SetColor(SolidColorBrush* brush);
};

class ColorChannel : public TextBoxRange
{
public:

    static void StaticInit();

    ColorChannel();

    RTTIOfClass(ColorChannel)

    void SetChannel(suic::Byte val);
    suic::Byte GetChannel();

protected:

    void CheckInputValue();
    void UpdateTextBox(suic::Float newValue);
    void OnRangeChanged(suic::Float val);
};

class ColorPickerBox : public suic::Control
{
public:

    ColorPickerBox();

    RTTIOfClass(ColorPickerBox)

    void OnMouseMove(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(suic::MouseButtonEventArg* e);

private:

    suic::OCursor* GetCursor();
    void OnSetCursor(suic::CursorEventArg* e);

private:

    suic::OCursor* _cursor;
};

class ColorPicker : public suic::Control
{
public:

    static DpProperty* ColorProperty;
    static DpProperty* InitColorProperty;
    static DpProperty* LastColorProperty;

    static RoutedEvent* ColorChangedEvent;

    static void OnColorPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnInitColorPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static void StaticInit();

    RTTIOfClass(ColorPicker)

    ColorPicker();

    void UpdateCurrentColor(suic::Color color);

    suic::SolidColorBrush* GetColor();
    void SetColor(suic::SolidColorBrush* color);

    suic::SolidColorBrush* GetInitColor();
    void SetInitColor(suic::SolidColorBrush* color);

    suic::SolidColorBrush* GetLastColor();

    void AddColorChanged(RoutedEventHandler* h);
    void RemoveColorChanged(RoutedEventHandler* h);

    void UpdateColorChannel(suic::Byte val);

    void OnPositionChanged(Object* sender, EventArg* e);
    void OnHueValueChanged(Element* sender, FloatPropChangedEventArg* e);

    void OnLastColorClick(Element* sender, RoutedEventArg* e);
    void OnInitColorClick(Element* sender, RoutedEventArg* e);

    void OnHexColorChanged(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnPickColorButtonChanged(suic::Element* sender, suic::RoutedEventArg* e);

protected:

    void OnApplyTemplate();
    void OnLoaded(LoadedEventArg* e);
    void OnRenderSizeChanged(SizeChangedInfo& e);

    void UpdateARGBColor(suic::Color color, bool bForce);
    void UpdateColor(suic::Color color);
    void UpdateColorInternal(suic::Color color);

    void SetLastColor(suic::SolidColorBrush* color);

    virtual void OnColorChanged(suic::SolidColorBrush* color);

    friend class ColorSelector;

private:

    bool _hueDraging;    
    bool _updatingColor;
    bool _initializingColor;

    suic::TextBox* _colorHex;
    ColorPickerBox* _colorPick;
    ColorSlider* _colorSlider;
    ColorSelector* _colorSelector;

    bool _needNotify;
};

#endif
