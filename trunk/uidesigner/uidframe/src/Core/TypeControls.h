///
/// 文件名：TypeControls.h
///
/// 功  能：实现资源类型的编辑控件
///
/// 作  者：汪荣
///

#ifndef _TYPECONTROLS_H
#define _TYPECONTROLS_H

#include "StyleNode.h"

#include "SetterCollectionNode.h"
#include "TriggerCollectionNode.h"

#include <Controls/ColorPicker.h>
#include <Controls/TextBoxRange.h>

class StringControl : public suic::TextBox
{
public:

    static RoutedEvent* BaseValueChangedEvent;

    static void StaticInit();

    RTTIOfClass(StringControl)

    StringControl();
    ~StringControl();

    virtual void InitText(const String& text);
    void AddBaseValueChanged(RoutedEventHandler* h);

protected:

    virtual void NotifyValueChanged();
    virtual bool CheckString(String& val);

    void SetString(const String& text);
    void RaiseTextChanged(bool bForeNotify);

    void OnNumericValueChanged();
    void OnLostFocus(suic::RoutedEventArg* e);

    void OnTextInput(suic::KeyboardEventArg* e);
    void OnKeyDown(suic::KeyboardEventArg* e);

protected:

    suic::String _oldValue;
};

class IntegerControl : public StringControl
{
public:

    IntegerControl();
    ~IntegerControl();

    RTTIOfClass(IntegerControl)

    virtual int GetInt();
    virtual String CheckInteger(int val);

    void InitText(const String& text);
    void InitInteger(int val);

protected:

    virtual bool CheckString(String& val);
    virtual int StringToInt(const String& val);
};

class PosIntegerControl : public IntegerControl
{
public:

    PosIntegerControl();
    ~PosIntegerControl();

    RTTIOfClass(PosIntegerControl)

    String CheckInteger(int val);
};

class FloatControl : public StringControl
{
public:

    FloatControl();
    ~FloatControl();

    RTTIOfClass(FloatControl)

    virtual Float GetFloat();

    void InitFloat(suic::Float val);

    void InitText(const String& text);
    bool CheckString(String& val);
    virtual String CheckFloat(suic::Float val);

protected:

    void NotifyValueChanged();
};

class WidthControl : public IntegerControl
{
public:

    WidthControl();
    ~WidthControl();

    RTTIOfClass(WidthControl)

    int GetInt();
    String CheckInteger(int val);;
    bool CheckString(String& val);

protected:

    void NotifyValueChanged();
    int StringToInt(const String& val);
    bool StringIsInt(const String& strText);
};

class WidthFloatControl : public FloatControl
{
public:

    WidthFloatControl();
    ~WidthFloatControl();

    RTTIOfClass(WidthFloatControl)

    Float GetFloat();
    String CheckFloat(suic::Float val);
    bool CheckString(String& val);

protected:

    void NotifyValueChanged();
    bool StringIsFloat(const String& strText);
    Float StringToFloat(const String& val);
};

class GridWidthControl : public WidthFloatControl
{
public:

    RTTIOfClass(GridWidthControl)
};

class GridFloatControl : public FloatControl
{
public:

    RTTIOfClass(GridFloatControl)

    GridFloatControl();

    suic::GridSizeType GetGridSizeType()
    {
        return _sizeType;
    }

    void SetGridSizeType(suic::GridSizeType val)
    {
        _sizeType = val;
        suic::String strText = GetText();
        CheckString(strText);
    }

protected:

    String CheckFloat(suic::Float val);

private:

    suic::GridSizeType _sizeType;
};

class GridStarControl : public Control
{
public:

    static RoutedEvent* GridValueChangedEvent;

    RTTIOfClass(GridStarControl)

    GridStarControl();
    ~GridStarControl();

    suic::Float GetGridValue();
    suic::GridSizeType GetGridType();

    void UpdateValue(suic::Float val, suic::GridSizeType type);

    void AddGridValueChanged(suic::RoutedEventHandler* handler);

protected:

    void OnInitialized(EventArg* e);
    void OnApplyTemplate();
    void OnComboBoxSelChanged(suic::Element*, suic::SelectionChangedEventArg*);
    void OnBaseValueChanged(DpObject* sender, RoutedEventArg* e);

    void RaisedGridValue();

private:

    bool _pending;
    suic::ComboBox* _comboBox;
    GridFloatControl* _textBox;
};

class PosFloatControl : public FloatControl
{
public:

    PosFloatControl();
    ~PosFloatControl();

    RTTIOfClass(PosFloatControl)

    virtual String CheckFloat(suic::Float val);
};

// 0-1编辑框
class RatioControl : public FloatControl
{
public:

    RatioControl();
    ~RatioControl();

    RTTIOfClass(RatioControl)
    
    void InitText(const String& text);
    bool CheckString(String& val);

protected:

    String CheckRatio(Float val);
};

class EditorControl : public suic::Control
{
public:

    static RoutedEvent* EditValueChangedEvent;
    static void StaticInit();

    RTTIOfClass(EditorControl)

    EditorControl();
    ~EditorControl();
    
    void Dispose();

    ResNodePtr GetResValue();
    Object* GetRealValue();

    void UpdateRealValue(Object* val);
    void ResetRealValue();

    void SetResValue(const ResNodePtr& val);

    virtual ResNodePtr GetCloneResValue();

    // 调用此函数需要更新到UI显示
    virtual void UpdateResValue(const ResNodePtr& val);
    virtual void CreateDefaultValue(ResNodePtr& obj);
    virtual void RefleshEditorUI();

    void OnBaseValueChanged(DpObject* sender, RoutedEventArg* e);

protected:

    void NotifyValueToEditRootPanel();

    void UpdateDefaultValue(Object* val);
    void UpdateRealValueOverride(Object* val, bool isPending);

    void OnApplyTemplate();

    virtual void OnInitialized(EventArg* e);
    virtual void OnUpdateValue();

    virtual bool OnBaseValueChangedOverride(Element* sender);
    virtual void OnEditValueChangedOverride(Element* sender);

protected:

    bool _isPending;
    ResNodePtr _resValue;
};

class StringEditor : public EditorControl
{
public:

    static DpProperty* MaxLinesProperty;
    static DpProperty* TextWrappingProperty;

    static void StaticInit();

    StringEditor();
    ~StringEditor();

    RTTIOfClass(StringEditor)

    void SetOString(suic::OString* v);
    suic::OString* GetOString();

    virtual void OnUpdateValue();
    virtual void CreateDefaultValue(ResNodePtr& obj);

protected:

    void OnApplyTemplate();
    bool OnBaseValueChangedOverride(Element* sender);
};

class IntegerEditor : public EditorControl
{
public:

    IntegerEditor();
    ~IntegerEditor();

    RTTIOfClass(IntegerEditor)

    void SetInteger(suic::Integer* v);
    suic::Integer* GetInteger();

    virtual void CreateDefaultValue(ResNodePtr& obj);

protected:

    void OnApplyTemplate();

    virtual void OnUpdateValue();
    virtual bool OnBaseValueChangedOverride(Element* sender);
    virtual int IntFromTextBox(Element* sender);

    virtual int GetInitInteger();
};

class PosIntegerEditor : public IntegerEditor
{
public:

    PosIntegerEditor();
    ~PosIntegerEditor();

    RTTIOfClass(PosIntegerEditor)
};

class FloatEditor : public EditorControl
{
public:

    FloatEditor();
    ~FloatEditor();

    RTTIOfClass(FloatEditor)

    void SetOFloat(suic::OFloat* v);
    suic::OFloat* GetOFloat();

protected:

    void CreateDefaultValue(ResNodePtr& obj);
    void OnUpdateValue();
    void OnApplyTemplate();
    bool OnBaseValueChangedOverride(Element* sender);

    virtual suic::Float GetInitFloat();
    virtual suic::Float FloatFromTextBox(Element* sender);
};

class WidthEditor : public IntegerEditor
{
public:

    WidthEditor();
    ~WidthEditor();

    RTTIOfClass(WidthEditor)

protected:

    virtual int GetInitInteger();
    virtual void CreateDefaultValue(ResNodePtr& obj);
    virtual int IntFromTextBox(Element* sender);
};

class WidthFloatEditor : public FloatEditor
{
public:

    WidthFloatEditor();
    ~WidthFloatEditor();

    RTTIOfClass(WidthFloatEditor)

protected:

    virtual Float GetInitFloat();
    virtual void CreateDefaultValue(ResNodePtr& obj);
    virtual suic::Float FloatFromTextBox(Element* sender);
};

class PosFloatEditor : public FloatEditor
{
public:

    PosFloatEditor();
    ~PosFloatEditor();

    RTTIOfClass(PosFloatEditor)
};

class TextBoxRangeEditor : public EditorControl
{
public:

    RTTIOfClass(TextBoxRangeEditor)

    TextBoxRangeEditor();
    virtual ~TextBoxRangeEditor();

    void CreateDefaultValue(ResNodePtr& obj);
    suic::OFloat* GetOFloat();

protected:

    void OnUpdateValue();
    void OnApplyTemplate();
    void OnValueChanged(suic::Element* sender, suic::FloatPropChangedEventArg* e);
};

class RatioEditor : public FloatEditor
{
public:

    RatioEditor();
    ~RatioEditor();

    RTTIOfClass(RatioEditor)

protected:

    suic::Float GetInitFloat();
    void CreateDefaultValue(ResNodePtr& obj);
};

class BooleanEditor : public EditorControl
{
public:

    BooleanEditor();
    ~BooleanEditor();

    RTTIOfClass(BooleanEditor)

    void SetBoolean(suic::Boolean* v);
    suic::Boolean* GetBoolean();

    void OnCheckBoxClick(Element* sender, RoutedEventArg* e);

protected:

    void CreateDefaultValue(ResNodePtr& obj);
    void OnUpdateValue();
    void OnApplyTemplate();
    bool OnBaseValueChangedOverride(Element* sender);
};

class SizeEditor : public EditorControl
{
public:

    SizeEditor();
    ~SizeEditor();

    RTTIOfClass(SizeEditor)

    OSize* GetSize()
    {
        return RTTICast<OSize>(GetRealValue());
    }

protected:

    void OnUpdateValue();
    void CreateDefaultValue(ResNodePtr& obj);
    bool OnBaseValueChangedOverride(Element* sender);
};

class fSizeEditor : public EditorControl
{
public:

    fSizeEditor();
    ~fSizeEditor();

    RTTIOfClass(fSizeEditor)

    OfSize* GetfSize()
    {
        return RTTICast<OfSize>(GetRealValue());
    }

protected:

    void OnUpdateValue();
    void CreateDefaultValue(ResNodePtr& obj);
    bool OnBaseValueChangedOverride(Element* sender);
};

class PointEditor : public EditorControl
{
public:

    PointEditor();
    ~PointEditor();

    RTTIOfClass(PointEditor)

    OPoint* GetPoint()
    {
        return RTTICast<OPoint>(GetRealValue());
    }

protected:

    void OnUpdateValue();
    void CreateDefaultValue(ResNodePtr& obj);
    bool OnBaseValueChangedOverride(Element* sender);
};

class fPointEditor : public EditorControl
{
public:

    static RoutedEvent* PointChangedEvent;

    fPointEditor();
    ~fPointEditor();

    static void StaticInit();

    RTTIOfClass(fPointEditor)

    OfPoint* GetfPoint()
    {
        return RTTICast<OfPoint>(GetRealValue());
    }

    void SetfPoint(fPoint point);
    void AddPointChanged(RoutedEventHandler* h);

protected:

    void OnUpdateValue();
    void CreateDefaultValue(ResNodePtr& obj);
    void OnApplyTemplate();
    bool OnBaseValueChangedOverride(Element* sender);
};

class RectEditor : public EditorControl
{
public:

    EventHandler RectValueChanged;

    RectEditor();
    ~RectEditor();

    RTTIOfClass(RectEditor)

    void SetORect(ORect* val);
    ORect* GetORect();

    String GetChangedVal() const;

protected:

    void OnUpdateValue();
    void CreateDefaultValue(ResNodePtr& obj);

    bool UpdateORect();
    bool UpdateORect(Element* sender);

    void OnApplyTemplate();
    bool OnBaseValueChangedOverride(Element* sender);

private:

    String _changedVal;
};

class fRectEditor : public EditorControl
{
public:

    EventHandler RectValueChanged;

    fRectEditor();
    ~fRectEditor();

    RTTIOfClass(fRectEditor)

    OfRect* GetfRect()
    {
        return RTTICast<OfRect>(GetRealValue());
    }

    void SetfRect(OfRect* val)
    {
        if (NULL != val)
        {
            UpdateRealValue(val);
        }
    }

    String GetChangedVal() const
    {
        return _changedVal;
    }

protected:

    void OnUpdateValue();
    void CreateDefaultValue(ResNodePtr& obj);

    void OnApplyTemplate();
    bool OnBaseValueChangedOverride(Element* sender);

private:

    String _changedVal;
};

class ImageSourceEditor : public EditorControl
{
public:

    ImageSourceEditor();
    ~ImageSourceEditor();

    RTTIOfClass(ImageSourceEditor)

    suic::ImageSource* GetImageSource()
    {
        return RTTICast<suic::ImageSource>(GetRealValue());
    }

protected:

    void OnUpdateValue();
    void CreateDefaultValue(ResNodePtr& obj);

    void OnApplyTemplate();
    bool OnBaseValueChangedOverride(Element* sender);

    void OnSourceClick(suic::Element* sender, suic::RoutedEventArg* e);

private:

    suic::TextBox* _info;
};

class BrushEditor : public EditorControl
{
public:

    static RoutedEvent* BrushChangedEvent;
    static void StaticInit();

    BrushEditor();
    ~BrushEditor();

    RTTIOfClass(BrushEditor)

    void NotifyBrushChanged();
    void AddBrushChanged(RoutedEventHandler* h);

protected:

    void OnUpdateValue();

protected:

    bool _needNotify;
};

class NullBrushEditor : public BrushEditor
{
public:

    NullBrushEditor()
    {
    }

    virtual ~NullBrushEditor() {}

    RTTIOfClass(NullBrushEditor)

    void OnUpdateValue()
    {
    }

    void CreateDefaultValue(ResNodePtr& obj)
    {
        ResNode::CreateResNode(NULL, obj);
    }
};

class SolidColorBrushEditor : public BrushEditor
{
public:

    SolidColorBrushEditor();
    virtual ~SolidColorBrushEditor() {}

    RTTIOfClass(SolidColorBrushEditor)
;
    ResNodePtr GetCloneResValue();
    void OnColorChanged(DpObject* sender, RoutedEventArg* e);

protected:

    void OnUpdateValue();
    void CreateDefaultValue(ResNodePtr& obj);

    void OnApplyTemplate();
    virtual void OnColorChanged(suic::SolidColorBrush* color);

private:

    ColorPicker* _colorPicker;
};

class ImageBrushEditor : public BrushEditor
{
public:

    ImageBrushEditor();
    virtual ~ImageBrushEditor() {}

    RTTIOfClass(ImageBrushEditor)

    ImageBrush* GetImageBrush();
    ImageBrush* CloneBrush();

    ResNodePtr GetCloneResValue();

    ImageBrushResNode* GetImageBrushResNode();

    void OnUpdateValue();
    void OnBrushChanged(Brush* brush);

    void OnButtonClick(Element* sender, RoutedEventArg* e);

    void OnTileModeChanged(Element* sender, SelectionChangedEventArg* e);
    void OnStretchChanged(Element* sender, SelectionChangedEventArg* e);

    void OnSelImageBtn(Element* sender, suic::RoutedEventArg* e);

protected:

    void OnApplyTemplate();
    void OnInitialized(suic::EventArg* e);

    void RefleshImageShow();
    void CreateDefaultValue(ResNodePtr& obj);

    Button* GetImageShowBtn();

private:

    bool _needClone;
    suic::Button* _imgShow;
};


class GradientStop : public FrameworkElement
{
public:

    GradientStop();
    virtual ~GradientStop();

    RTTIOfClass(GradientStop)

    suic::Color GetCurrentColor();
    void SetCurrentColor(suic::Color color);
    void InitGradientBrush(suic::GradientBrush* brush);

    suic::GradientStops* GetGradientStops();

    void SetIsClicking(bool val)
    {
        _isClicking = val;
    }

protected:

    void NotifyStopsChanged();

    int HitTestRatio(Point pt, Float& fDis);
    Float GetRatioDis(int index);

    void UpdateStopRatio(Float fRatio);
    void UpdateCurrentStop(int iCurrStop);

    void RemoveCurrentStop();
    void AddGradientStop(Point pt, Color clr);

    void OnRender(suic::Drawing* drawing);

    void OnMouseMove(MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);

protected:

    bool _isDraging;
    bool _isClicking;
    int _currentStop;
    Float _startpos;
    Point _prevPoint;
    Point _originPoint;

    suic::GradientStops* _stops;
    suic::PathGeometry* _path;
    suic::LinearGradientBrush* _bk;
};

class GradientBrushEditor : public BrushEditor
{
public:

    GradientBrushEditor();
    virtual ~GradientBrushEditor();

    RTTIOfClass(GradientBrushEditor)

    void UpdateCurrentColor(Color clr);

    GradientBrush* GetGradientBrush()
    {
        return RTTICast<GradientBrush>(GetRealValue());
    }

    void OnColorChanged(suic::SolidColorBrush* color);
    void OnColorChanged(DpObject* sender, RoutedEventArg* e);

protected:

    void UpdateGradienStops(GradientBrush* brush);
    void UpdateGradienValue(GradientBrush* brush);

    void OnUpdateValue();
    void OnApplyTemplate();

    virtual void OnGradientStopChanged();

    friend class GradientStop;

protected:

    bool _needClone;
    ColorPicker* _colorPicker;
    GradientStop* _stops;
};

class LinearGradientBrushEditor : public GradientBrushEditor
{
public:

    LinearGradientBrushEditor();
    virtual ~LinearGradientBrushEditor();

    RTTIOfClass(LinearGradientBrushEditor)

    LinearGradientBrush* CloneBrush();
    LinearGradientBrush* GetLinearGradientBrush();

    ResNodePtr GetCloneResValue();

    void OnfPointChanged(suic::DpObject* sender, RoutedEventArg* e);

protected:

    void OnUpdateValue();
    void CreateDefaultValue(ResNodePtr& obj);
    void OnApplyTemplate();
    void Dispose();

    virtual void OnGradientStopChanged();
};

class RadialGradientBrushEditor : public GradientBrushEditor
{
public:

    RadialGradientBrushEditor();

    RTTIOfClass(RadialGradientBrushEditor)

    void OnColorChanged(suic::SolidColorBrush* color);
    void OnfPointChanged(suic::DpObject* sender, RoutedEventArg* e);

    void OnUpdateValue();
    RadialGradientBrush* GetRadialGradientBrush();

    ResNodePtr GetCloneResValue();

protected:

    void CreateDefaultValue(ResNodePtr& obj);
    void OnGradientStopChanged();
    RadialGradientBrush* CloneBrush();
    void OnApplyTemplate();
};

class StyleEditor : public EditorControl
{
public:

    StyleEditor();
    ~StyleEditor();

    RTTIOfClass(StyleEditor)

    void OnSelChanged(Element* sender, SelectionChangedEventArg* e);

protected:

    void OnUpdateValue();
    void CreateDefaultValue(ResNodePtr& obj);
    void OnApplyTemplate();
    bool OnBaseValueChangedOverride(Element* sender);
};

class LayoutEditor : public EditorControl
{
public:

    LayoutEditor();
    ~LayoutEditor();

    RTTIOfClass(LayoutEditor)

protected:

    void CreateDefaultValue(ResNodePtr& obj);
    void OnApplyTemplate();
    bool OnBaseValueChangedOverride(Element* sender);
};

class TemplateEditor : public EditorControl
{
public:

    TemplateEditor();
    ~TemplateEditor();

    RTTIOfClass(TemplateEditor)

    void OnSelChanged(Element* sender, SelectionChangedEventArg* e);

protected:

    void OnUpdateValue();
    void CreateDefaultValue(ResNodePtr& obj);
    void OnApplyTemplate();
    bool OnBaseValueChangedOverride(Element* sender);
};

#endif
