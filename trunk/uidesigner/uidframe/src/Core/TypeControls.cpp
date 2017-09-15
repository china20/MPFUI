
#include "stdafx.h"

#include <Core/TypeControls.h>
#include <Core/TypeManager.h>
#include <Editor/EditRootPanel.h>
#include <System/Tools/EventHelper.h>
#include <Main/Project.h>
#include <Main/ImageSelectorWindow.h>
#include <Editor/EditRootPanel.h>
#include <Editor/ImageEditWindow.h>

RoutedEvent* StringControl::BaseValueChangedEvent;
RoutedEvent* GridStarControl::GridValueChangedEvent;

ImplementRTTIOfClass(StringControl, suic::TextBox)
ImplementRTTIOfClass(FloatControl, StringControl)
ImplementRTTIOfClass(PosFloatControl, FloatControl)
ImplementRTTIOfClass(RatioControl, FloatControl)
ImplementRTTIOfClass(IntegerControl, StringControl)
ImplementRTTIOfClass(PosIntegerControl, IntegerControl)
ImplementRTTIOfClass(WidthControl, IntegerControl)
ImplementRTTIOfClass(WidthFloatControl, FloatControl)
ImplementRTTIOfClass(EditorControl, suic::Control)

ImplementRTTIOfClass(GridWidthControl, WidthControl)
ImplementRTTIOfClass(GridStarControl, Control)
ImplementRTTIOfClass(GridFloatControl, FloatControl)

ImplementRTTIOfClass(GradientStop, suic::FrameworkElement)

ImplementRTTIOfClass(StringEditor, EditorControl)
ImplementRTTIOfClass(FloatEditor, EditorControl)
ImplementRTTIOfClass(PosFloatEditor, FloatEditor)
ImplementRTTIOfClass(RatioEditor, FloatEditor)
ImplementRTTIOfClass(IntegerEditor, EditorControl)
ImplementRTTIOfClass(PosIntegerEditor, IntegerEditor)
ImplementRTTIOfClass(WidthEditor, IntegerEditor)
ImplementRTTIOfClass(WidthFloatEditor, FloatEditor)

ImplementRTTIOfClass(BooleanEditor, EditorControl)

ImplementRTTIOfClass(SizeEditor, EditorControl)
ImplementRTTIOfClass(fSizeEditor, EditorControl)
ImplementRTTIOfClass(PointEditor, EditorControl)
ImplementRTTIOfClass(fPointEditor, EditorControl)
ImplementRTTIOfClass(RectEditor, EditorControl)
ImplementRTTIOfClass(fRectEditor, EditorControl)

ImplementRTTIOfClass(ImageSourceEditor, EditorControl)

ImplementRTTIOfClass(BrushEditor, EditorControl)
ImplementRTTIOfClass(NullBrushEditor, BrushEditor)
ImplementRTTIOfClass(StyleEditor, EditorControl)

ImplementRTTIOfClass(LayoutEditor, EditorControl)
ImplementRTTIOfClass(TemplateEditor, EditorControl)

ImplementRTTIOfClass(TextBoxRangeEditor, EditorControl)

void StringControl::StaticInit()
{
    BaseValueChangedEvent = EventHelper::RegisterRoutedEvent(_U("ValueChanged"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
    GridStarControl::GridValueChangedEvent = EventHelper::RegisterRoutedEvent(_U("GridValueChanged"), RoutingStrategy::Bubble, Object::RTTIType(), GridStarControl::RTTIType());

}

StringControl::StringControl()
{

}

StringControl::~StringControl()
{

}

void StringControl::InitText(const String& text)
{
    _oldValue = text;
    SetText(text);
}

void StringControl::SetString(const String& text)
{
    _oldValue = text;
    SetText(text);
}

void StringControl::NotifyValueChanged()
{
    OnNumericValueChanged();
}

void StringControl::AddBaseValueChanged(RoutedEventHandler* h)
{
    AddHandler(BaseValueChangedEvent, h);
}

void StringControl::OnNumericValueChanged()
{
    RoutedEventArg e(this, BaseValueChangedEvent);
    RaiseEvent(&e);
}

bool StringControl::CheckString(String& val)
{
    return _oldValue.Equals(val);
}

void StringControl::RaiseTextChanged(bool bForeNotify)
{
    suic::String strText = GetText();

    if (!CheckString(strText) || bForeNotify)
    {
        _oldValue = strText;
        NotifyValueChanged();
    }
}

void StringControl::OnLostFocus(suic::RoutedEventArg* e)
{
    suic::TextBox::OnLostFocus(e);
    RaiseTextChanged(false);
}

void StringControl::OnTextInput(suic::KeyboardEventArg* e)
{
    suic::TextBox::OnTextInput(e);
}

void StringControl::OnKeyDown(suic::KeyboardEventArg* e)
{
    suic::TextBox::OnKeyDown(e);
    if (e->GetKey() == Key::kReturn)
    {
        RaiseTextChanged(true);
    }
}

//=====================================================
// IntegerControl

IntegerControl::IntegerControl()
{

}

IntegerControl::~IntegerControl()
{

}

int IntegerControl::GetInt()
{
    return GetText().ToInt();
}

int IntegerControl::StringToInt(const String& val)
{
    return val.ToInt();
}

bool IntegerControl::CheckString(String& val)
{
    int oldVal = StringToInt(_oldValue);
    int newVal = StringToInt(val);

    val = CheckInteger(newVal);
    SetText(val);

    return (oldVal == newVal);
}

void IntegerControl::InitText(const String& text)
{
    int iVal = StringToInt(text);
    SetString(CheckInteger(iVal));
}

void IntegerControl::InitInteger(int val)
{
    SetString(CheckInteger(val));
}

String IntegerControl::CheckInteger(int val)
{
    String strText;
    strText.Format(_U("%d"), val);
    return strText;
}

//=====================================================
// PosIntegerControl

PosIntegerControl::PosIntegerControl()
{

}

PosIntegerControl::~PosIntegerControl()
{

}

String PosIntegerControl::CheckInteger(int val)
{
    if (val < 0)
    {
        val = 0;
    }
    return IntegerControl::CheckInteger(val);
}

//=====================================================
// WidthControl

WidthControl::WidthControl()
{

}

WidthControl::~WidthControl()
{

}

int WidthControl::GetInt()
{
    String strText = GetText();
    if (!strText.Empty() && StringIsInt(strText))
    {
        return GetText().ToInt();
    }
    else
    {
        return -1;
    }
}

int WidthControl::StringToInt(const String& val)
{
    return val.ToInt();
}

bool WidthControl::CheckString(String& val)
{
    int oldVal = -1;
    int newVal = -1;

    if (StringIsInt(_oldValue))
    {
        oldVal = _oldValue.ToInt();
    }

    if (StringIsInt(val))
    {
        newVal = val.ToInt();
    }

    val = CheckInteger(newVal);
    SetText(val);

    if (_oldValue.Equals(val))
    {
        return true;
    }

    return (oldVal ==newVal);
}

String WidthControl::CheckInteger(int val)
{
    String strText;
    if (val < 0)
    {
        strText = _U("Auto");
    }
    else
    {
        strText.Format(_U("%d"), val);
    }

    return strText;
}

bool WidthControl::StringIsInt(const String& strText)
{
    if (strText.Empty())
    {
        return false;
    }

    bool dotCount = 0;

    for (int i = 0; i < strText.Length(); ++i)
    {
        int c = strText[i];

        if (c < '0' || c > '9')
        {
            return false;
        }

        if (c == '-' && i != 0)
        {
            return false;
        }
    }

    return true;
}

void WidthControl::NotifyValueChanged()
{
    StringControl::NotifyValueChanged();
}

//=====================================================
// WidthFloatControl

WidthFloatControl::WidthFloatControl()
{

}

WidthFloatControl::~WidthFloatControl()
{

}

Float WidthFloatControl::GetFloat()
{
    String strText = GetText();
    if (!strText.Empty() && StringIsFloat(strText))
    {
        return (Float)((int)GetText().ToFloat());
    }
    else
    {
        return FloatUtil::PosInfinity;
    }
}

Float WidthFloatControl::StringToFloat(const String& val)
{
    if (!StringIsFloat(val))
    {
        return FloatUtil::PosInfinity;
    }
    return (Float)((int)val.ToFloat());
}

bool WidthFloatControl::CheckString(String& val)
{
    suic::Float oldVal = FloatUtil::PosInfinity;
    suic::Float newVal = FloatUtil::PosInfinity;

    if (StringIsFloat(_oldValue))
    {
        oldVal = _oldValue.ToFloat();
    }

    if (StringIsFloat(val))
    {
        newVal = val.ToFloat();
    }

    val = CheckFloat(newVal);
    SetText(val);

    if (_oldValue.Equals(val))
    {
        return true;
    }

    return suic::FloatUtil::AreClose(oldVal, newVal);
}

String WidthFloatControl::CheckFloat(Float val)
{
    String strText;
    if (FloatUtil::IsPosInfinity(val) || (int)val < 0)
    {
        strText = _U("Auto");
    }
    else
    {
        strText.Format(_U("%d"), (int)val);
    }

    return strText;
}

bool WidthFloatControl::StringIsFloat(const String& strText)
{
    if (strText.Empty())
    {
        return false;
    }

    bool dotCount = 0;

    for (int i = 0; i < strText.Length(); ++i)
    {
        int c = strText[i];
        if (c == '.')
        {
            if (dotCount != 0)
            {
                return false;
            }

            ++dotCount;

            if (i == 0)
            {
                return false;
            }
            return true;
        }

        if (c < '0' || c > '9')
        {
            return false;
        }
        if (c == '-' && i != 0)
        {
            return false;
        }
    }

    return true;
}

void WidthFloatControl::NotifyValueChanged()
{
    StringControl::NotifyValueChanged();
}

//=====================================================
// GridFloatControl

GridFloatControl::GridFloatControl()
{
    _sizeType = suic::GridSizeType::Star;
}

String GridFloatControl::CheckFloat(suic::Float val)
{
    String strText;
    switch (_sizeType)
    {
        case suic::GridSizeType::Pixel:
            if (FloatUtil::LessThan(val, 0))
            {
                val = 0;
            }
            break;

        case suic::GridSizeType::Star:
            if (FloatUtil::LessThan(val, 1))
            {
                val = 1;
            }
            break;
    }
    strText.Format(_U("%d"), (int)val);
    return strText;
}

//=====================================================
// GridStarControl

GridStarControl::GridStarControl()
{
    _textBox = NULL;
    _comboBox = NULL;
    _pending = false;
}

GridStarControl::~GridStarControl()
{

}

void GridStarControl::RaisedGridValue()
{
    RoutedEventArg e(this, GridValueChangedEvent);
    RaiseEvent(&e);
}

suic::Float GridStarControl::GetGridValue()
{
    return _textBox->GetText().ToFloat();
}

suic::GridSizeType GridStarControl::GetGridType()
{
    return _textBox->GetGridSizeType();
}

void GridStarControl::UpdateValue(suic::Float val, suic::GridSizeType type)
{
    if (NULL == _textBox)
    {
        ApplyTemplate();
    }

    _textBox->InitFloat(val);
    _textBox->SetGridSizeType(type);
    switch (type)
    {
        case suic::GridSizeType::Auto:
            _comboBox->SetSelectedIndex(0);
            break;

        case suic::GridSizeType::Star:
            _comboBox->SetSelectedIndex(2);
            break;

        case suic::GridSizeType::Pixel:
            _comboBox->SetSelectedIndex(1);
            break;
    }
}

void GridStarControl::AddGridValueChanged(suic::RoutedEventHandler* handler)
{
    AddHandler(GridValueChangedEvent, handler);
}

void GridStarControl::OnInitialized(EventArg* e)
{
    AddHandler(StringControl::BaseValueChangedEvent, new RoutedEventHandler(this, &GridStarControl::OnBaseValueChanged));
}

void GridStarControl::OnBaseValueChanged(DpObject* sender, RoutedEventArg* e)
{
    if (!_pending)
    {
        _pending = true;
        RaisedGridValue();
        _pending = false;
    }
}

void GridStarControl::OnApplyTemplate()
{
    _textBox = RTTICast<GridFloatControl>(GetTemplateChild(_U("PART_TextBox")));
    _comboBox = RTTICast<suic::ComboBox>(GetTemplateChild(_U("PART_ComboBox")));

    _comboBox->SetSelectedIndex(2);
    _comboBox->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &GridStarControl::OnComboBoxSelChanged));
}

void GridStarControl::OnComboBoxSelChanged(suic::Element* sender, suic::SelectionChangedEventArg* e)
{
    _pending = true;

    if (_comboBox != NULL && e->GetAddedItems()->GetCount() > 0)
    {
        suic::Object* selItem = e->GetAddedItems()->GetItem(0);
        suic::String strItem = selItem->ToString();
        if (strItem.Equals(_U("Auto")))
        {
            _textBox->SetGridSizeType(suic::GridSizeType::Auto);
            _textBox->Enable(false);
        }
        else
        {
            if (strItem.Equals(_U("Star")))
            {
                _textBox->SetGridSizeType(suic::GridSizeType::Star);
            }
            else
            {
                _textBox->SetGridSizeType(suic::GridSizeType::Pixel);
            }
            _textBox->Enable(true);
        }
    }

    _pending = false;

    RaisedGridValue();

    e->SetHandled(true);
}

//=====================================================
// FloatControl

FloatControl::FloatControl()
{
}

FloatControl::~FloatControl()
{
}

Float FloatControl::GetFloat()
{
    return GetText().ToFloat();
}

void FloatControl::InitFloat(suic::Float val)
{
    SetString(CheckFloat(val));
}

bool FloatControl::CheckString(String& val)
{
    suic::Float oldVal = _oldValue.ToFloat();
    suic::Float newVal = val.ToFloat();

    val = CheckFloat(newVal);
    SetText(val);

    return suic::FloatUtil::AreClose(oldVal, newVal);
}

void FloatControl::InitText(const String& text)
{
    suic::Float fVal = text.ToFloat();
    SetString(CheckFloat(fVal));
}

String FloatControl::CheckFloat(suic::Float val)
{
    String strText;
    if (FloatUtil::AreClose(val, 0))
    {
        strText = _U("0");
    }
    else
    {
        strText.Format(_U("%g"), val);
    }

    return strText;
}

void FloatControl::NotifyValueChanged()
{
    StringControl::NotifyValueChanged();
}

PosFloatControl::PosFloatControl()
{

}

PosFloatControl::~PosFloatControl()
{

}

String PosFloatControl::CheckFloat(suic::Float val)
{
    if (FloatUtil::LessThan(val, 0))
    {
        val = 0;
    }
    return FloatControl::CheckFloat(val);
}

RatioControl::RatioControl()
{
}

RatioControl::~RatioControl()
{
}

bool RatioControl::CheckString(String& val)
{
    suic::Float oldVal = _oldValue.ToFloat();
    suic::Float newVal = val.ToFloat();

    val = CheckRatio(newVal);
    SetText(val);

    return suic::FloatUtil::AreClose(oldVal, newVal);
}

void RatioControl::InitText(const String& text)
{
    suic::Float fVal = text.ToFloat();
    SetString(CheckRatio(fVal));
}

String RatioControl::CheckRatio(Float val)
{
    String strText;

    if (FloatUtil::LessThan(val, 0) || FloatUtil::GreaterThan(val, 1))
    {
        val = 1;
    }

    strText.Format(_U("%g"), val);

    return strText;
}

//=====================================================
// EditorControl

RoutedEvent* EditorControl::EditValueChangedEvent;

static EditRootPanel* FindEditRttoPanel(suic::FrameworkElement* elem)
{
    EditRootPanel* editPanel = NULL;
    suic::Element* parent = elem->GetUIParent();

    while (parent)
    {
        editPanel = suic::DynamicCast<EditRootPanel>(parent);
        if (NULL != editPanel)
        {
            break;
        }
        parent = parent->GetUIParent();
    }

    return editPanel;
}

void EditorControl::StaticInit()
{
    suic::Control::FocusableProperty->OverrideMetadata(RTTIType(), new suic::PropMetadata(Boolean::False));
    EditValueChangedEvent = EventHelper::RegisterRoutedEvent(_U("EditValueChanged"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
}

EditorControl::EditorControl()
{
    _isPending = false;
}

EditorControl::~EditorControl()
{

}

void EditorControl::Dispose()
{
    _resValue = NULL;
    suic::Control::Dispose();
}

ResNodePtr EditorControl::GetResValue()
{
    return _resValue;
}

ResNodePtr EditorControl::GetCloneResValue()
{
    return _resValue;
}

Object* EditorControl::GetRealValue()
{
    if (NULL == _resValue.get())
    {
        return NULL;
    }
    else
    {
        return _resValue->GetValue();
    }
}

void EditorControl::UpdateRealValue(Object* val)
{
    UpdateRealValueOverride(val, false);
}

void EditorControl::ResetRealValue()
{
    _resValue = NULL;
}

void EditorControl::UpdateRealValueOverride(Object* val, bool isPending)
{
    _isPending = isPending;
    ResNodePtr resNode;

    if (val == NULL)
    {
        UpdateResValue(ResNode::CreateResNode(val, resNode));
    }
    else
    {
        //UpdateResValue(ResNode::CreateResNode(val));
        if (!_resValue || _resValue->GetRTTIType() == ExtensionResNode::RTTIType())
        {
            UpdateResValue(ResNode::CreateResNode(val, resNode));
        }
        else
        {
            _resValue->SetValue(val);
        }

        if (!_isPending)
        {
            OnUpdateValue();
        }
    }

    _isPending = false;
}

void EditorControl::SetResValue(const ResNodePtr& val)
{
    _resValue = val;
}

void EditorControl::UpdateResValue(const ResNodePtr& val)
{
    /*if (NULL != val.get())
    {
        _resValue = ResNode::CreateResNode(val->GetValue());
    }
    else*/
    {
        _resValue = val;
    }

    if (!_isPending)
    {
        OnUpdateValue();
    }
}

void EditorControl::CreateDefaultValue(ResNodePtr& obj)
{
}

void EditorControl::RefleshEditorUI()
{
    OnUpdateValue();
}

void EditorControl::UpdateDefaultValue(Object* val)
{
    ResNodePtr resNode;
    UpdateResValue(ResNode::CreateResNode(val, resNode));
}

void EditorControl::OnUpdateValue()
{

}

void EditorControl::NotifyValueToEditRootPanel()
{
    Object* tag = GetTag();
    if (tag->ToString().Equals(_U("RES")))
    {
        EditRootPanel* editPanel = FindEditRttoPanel(this);
        if (editPanel != NULL)
        {
            // 通知资源已经改变
            editPanel->UpdateResource(GetResValue().get());
        }
    }
}

void EditorControl::OnBaseValueChanged(DpObject* sender, RoutedEventArg* e)
{
    Element* oriElem = RTTICast<Element>(e->GetOriginalSource());

    _isPending = true;
    if (OnBaseValueChangedOverride(oriElem))
    {
        OnEditValueChangedOverride(this);
    }
    _isPending = false;
}

bool EditorControl::OnBaseValueChangedOverride(Element* sender)
{
    return false;
}

void EditorControl::OnEditValueChangedOverride(Element* sender)
{
    RoutedEventArg e(this, EditValueChangedEvent);
    RaiseEvent(&e);
}

void EditorControl::OnApplyTemplate()
{
    OnUpdateValue();
}

void EditorControl::OnInitialized(EventArg* e)
{
    AddHandler(StringControl::BaseValueChangedEvent, new RoutedEventHandler(this, &EditorControl::OnBaseValueChanged));
}

//=====================================================
// StringEditor

StringEditor::StringEditor()
{
}

StringEditor::~StringEditor()
{
}

DpProperty* StringEditor::MaxLinesProperty;
DpProperty* StringEditor::TextWrappingProperty;

void StringEditor::StaticInit()
{
    MaxLinesProperty = DpProperty::Register(_U("MaxLines"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(NULL));
    TextWrappingProperty = DpProperty::Register(_U("TextWrapping"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(NULL));//TextEditor::TextWrappingProperty->AddOwner(RTTIType(), NULL);
    TextWrappingProperty->SetConvertValueCb(TextWrappingConvert::Convert);
}

void StringEditor::OnUpdateValue()
{
    StringControl* elem = RTTICast<StringControl>(GetTemplateChild(_U("PART_TextBox")));
    suic::OString* oStr = GetOString();

    if (NULL == elem)
    {
        // error:模版不对
        return;
    }

    if (NULL != oStr)
    {
        elem->InitText(oStr->ToString());
    }
    else
    {
        elem->InitText(_U(""));
    }
}

void StringEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(OString::EmptyString, obj);
}

void StringEditor::SetOString(suic::OString* v)
{
    UpdateRealValue(v);
}

OString* StringEditor::GetOString()
{
    return RTTICast<OString>(GetRealValue());
}

void StringEditor::OnApplyTemplate()
{
    OnUpdateValue();
}

bool StringEditor::OnBaseValueChangedOverride(Element* sender)
{
    StringControl* textBox = RTTICast<StringControl>(sender);
    suic::String strText = textBox->GetText();

    if (GetOString() == NULL || !GetOString()->ToString().Equals(strText))
    {
        UpdateRealValue(new OString(strText));
        NotifyValueToEditRootPanel();
        return true;
    }
    else
    {
        return false;
    }
}

//=====================================================
// IntegerEditor

IntegerEditor::IntegerEditor()
{
}

IntegerEditor::~IntegerEditor()
{
}

void IntegerEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(Integer::GetZeroInt(), obj);
}

int IntegerEditor::GetInitInteger()
{
    return 0;
}

void IntegerEditor::OnUpdateValue()
{
    IntegerControl* elem = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_TextBox")));
    if (NULL != elem)
    {
        if (NULL != GetInteger())
        {
            elem->InitInteger(GetInteger()->ToInt());
        }
        else
        {
            elem->InitInteger(GetInitInteger());
        }
    }
}

void IntegerEditor::SetInteger(suic::Integer* v)
{
    UpdateRealValue(v);
}

suic::Integer* IntegerEditor::GetInteger()
{
    return RTTICast<suic::Integer>(GetRealValue());
}

void IntegerEditor::OnApplyTemplate()
{
    OnUpdateValue();
}

bool IntegerEditor::OnBaseValueChangedOverride(Element* sender)
{
    int iVal = IntFromTextBox(sender);

    UpdateRealValue(new Integer(iVal));
    NotifyValueToEditRootPanel();
    return true;
}

int IntegerEditor::IntFromTextBox(Element* sender)
{
    IntegerControl* textBox = RTTICast<IntegerControl>(sender);
    int iVal = textBox->GetInt();
    return iVal;
}

//void IntegerEditor::CreateInteger()
//{
//    if (NULL == GetRealValue())
//    {
//        UpdateRealValue(new Integer());
//    }
//}

//=====================================================
// PosIntegerEditor

PosIntegerEditor::PosIntegerEditor()
{

}

PosIntegerEditor::~PosIntegerEditor()
{

}

//=====================================================
// WidthEditor

WidthEditor::WidthEditor()
{
}

WidthEditor::~WidthEditor()
{
}

void WidthEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(new Integer(-1), obj);
}

int WidthEditor::GetInitInteger()
{
    return -1;
}

int WidthEditor::IntFromTextBox(Element* sender)
{
    suic::TextBox* textBox = RTTICast<suic::TextBox>(sender);
    suic::String strText = textBox->GetText();
    int iVal = -1;

    if (!strText.Equals(_U("Auto")))
    {
        iVal = strText.ToInt();
        if (iVal < 0)
        {
            iVal = -1;
        }
    }
    return iVal;
}

//=====================================================
// WidthFloatEditor

WidthFloatEditor::WidthFloatEditor()
{
}

WidthFloatEditor::~WidthFloatEditor()
{
}

void WidthFloatEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(new OFloat(FloatUtil::PosInfinity), obj);
}

Float WidthFloatEditor::GetInitFloat()
{
    return FloatUtil::PosInfinity;
}

suic::Float WidthFloatEditor::FloatFromTextBox(Element* sender)
{
    suic::TextBox* textBox = RTTICast<suic::TextBox>(sender);
    suic::String strText = textBox->GetText();
    suic::Float fVal = FloatUtil::PosInfinity;
    if (!strText.Equals(_U("Auto")))
    {
        fVal = strText.ToFloat();
        if (FloatUtil::LessThan(fVal, 0))
        {
            fVal = FloatUtil::PosInfinity;
        }
    }
    return fVal;
}

//=====================================================
// FloatEditor

PosFloatEditor::PosFloatEditor()
{
}

PosFloatEditor::~PosFloatEditor()
{
}


FloatEditor::FloatEditor()
{
}

FloatEditor::~FloatEditor()
{
}

void FloatEditor::SetOFloat(suic::OFloat* v)
{
    UpdateRealValue(v);
}

suic::OFloat* FloatEditor::GetOFloat()
{
    return RTTICast<suic::OFloat>(GetRealValue());
}

void FloatEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(OFloat::GetZeroFloat(), obj);
}

suic::Float FloatEditor::GetInitFloat()
{
    return 0;
}

void FloatEditor::OnUpdateValue()
{
    FloatControl* elem = NULL;
    OFloat* val = GetOFloat();
    elem = RTTICast<FloatControl>(GetTemplateChild(_U("PART_TextBox")));
    if (NULL != elem)
    {
        if (NULL != val)
        {
            elem->InitFloat(val->ToFloat());
        }
        else
        {
            elem->InitFloat(GetInitFloat());
        }
    }
}

void FloatEditor::OnApplyTemplate()
{
    OnUpdateValue();
}

suic::Float FloatEditor::FloatFromTextBox(Element* sender)
{
    suic::TextBox* textBox = RTTICast<suic::TextBox>(sender);
    suic::Float fVal = textBox->GetText().ToFloat();
    return fVal;
}

bool FloatEditor::OnBaseValueChangedOverride(Element* sender)
{
    suic::Float fVal = FloatFromTextBox(sender);

    if (GetOFloat() == NULL || !suic::FloatUtil::AreClose(GetOFloat()->ToFloat(), fVal))
    {
        UpdateRealValue(new OFloat(fVal));
        NotifyValueToEditRootPanel();
        return true;
    }
    else
    {
        return false;
    }
}

//=====================================================
// RatioEditor

RatioEditor::RatioEditor()
{

}

RatioEditor::~RatioEditor()
{

}

void RatioEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(new OFloat(1.0f), obj);
}

suic::Float RatioEditor::GetInitFloat()
{
    return 1;
}

//==============================================
// TextBoxRangeEditor

TextBoxRangeEditor::TextBoxRangeEditor()
{
}

TextBoxRangeEditor::~TextBoxRangeEditor()
{
}

void TextBoxRangeEditor::CreateDefaultValue(ResNodePtr& obj)
{
    suic::Float fVal = 1.0;
    TextBoxRange* elem = NULL;
    elem = RTTICast<TextBoxRange>(GetTemplateChild(_U("PART_TextBoxRange")));
    if (NULL == elem)
    {
        ApplyTemplate();
        elem = RTTICast<TextBoxRange>(GetTemplateChild(_U("PART_TextBoxRange")));
    }

    if (NULL != elem)
    {
        fVal = elem->GetMaximum();
    }

    ResNode::CreateResNode(new OFloat(fVal), obj);
}

suic::OFloat* TextBoxRangeEditor::GetOFloat()
{
    return RTTICast<suic::OFloat>(GetRealValue());
}

void TextBoxRangeEditor::OnUpdateValue()
{
    TextBoxRange* elem = NULL;
    OFloat* val = GetOFloat();
    elem = RTTICast<TextBoxRange>(GetTemplateChild(_U("PART_TextBoxRange")));
    if (NULL != elem)
    {
        if (NULL != val)
        {
            elem->SetValue(val->ToFloat());
        }
        else
        {
            elem->SetValue(elem->GetMaximum());
        }
    }
}

void TextBoxRangeEditor::OnApplyTemplate()
{
    OnUpdateValue();
    TextBoxRange* elem = RTTICast<TextBoxRange>(GetTemplateChild(_U("PART_TextBoxRange")));

    if (NULL != elem)
    {
        elem->AddValueChanged(new suic::FloatPropChangedEventHandler(this, &TextBoxRangeEditor::OnValueChanged));
    }
}

void TextBoxRangeEditor::OnValueChanged(suic::Element* sender, suic::FloatPropChangedEventArg* e)
{
    TextBoxRange* elem = RTTICast<TextBoxRange>(sender);
    suic::Float fVal = elem->GetValue();

    if (GetOFloat() == NULL || !suic::FloatUtil::AreClose(GetOFloat()->ToFloat(), fVal))
    {
        UpdateRealValue(new OFloat(fVal));
        NotifyValueToEditRootPanel();

        // 发出EditValueChangedEvent事件，这样SetterEditor就会收到值改变事件，
        // 然后通知界面更新
        OnEditValueChangedOverride(this);
    }
}

//=====================================================
// BooleanEditor
BooleanEditor::BooleanEditor()
{
}

BooleanEditor::~BooleanEditor()
{
}

void BooleanEditor::SetBoolean(suic::Boolean* v)
{
    UpdateRealValue(v);
}

suic::Boolean* BooleanEditor::GetBoolean()
{
    return RTTICast<suic::Boolean>(GetRealValue());
}

void BooleanEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(Boolean::False, obj);
}

void BooleanEditor::OnUpdateValue()
{
    CheckBox* elem = NULL;
    Boolean* val = GetBoolean();
    elem = RTTICast<CheckBox>(GetTemplateChild(_U("PART_CheckBox")));
    if (NULL != elem && val)
    {
        elem->SetChecked(val);
    }
}

void BooleanEditor::OnApplyTemplate()
{
    OnUpdateValue();
    CheckBox* elem = RTTICast<CheckBox>(GetTemplateChild(_U("PART_CheckBox")));
    if (NULL != elem)
    {
        elem->AddClick(new ClickEventHandler(this, &BooleanEditor::OnCheckBoxClick));
    }
}

bool BooleanEditor::OnBaseValueChangedOverride(Element* sender)
{
    return true;
}

void BooleanEditor::OnCheckBoxClick(Element* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
    CheckBox* elem = RTTICast<CheckBox>(sender);
    ResNodePtr resNode = GetResValue();
    if (!resNode)
    {
        ResNode::CreateResNode(elem->GetChecked(), _resValue);
    }
    else
    {
        _resValue->SetValue(elem->GetChecked());
    }
}

//=====================================================
// SizeEditor
SizeEditor::SizeEditor()
{
}

SizeEditor::~SizeEditor()
{
}

void SizeEditor::OnUpdateValue()
{
    IntegerControl* xCtrl = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_cx")));
    IntegerControl* yCtrl = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_cy")));
    OSize* val = GetSize();

    if (xCtrl != NULL && yCtrl != NULL && NULL != val)
    {
        xCtrl->InitInteger(val->ToSize().cx);
        yCtrl->InitInteger(val->ToSize().cy);
    }
}

void SizeEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(OSize::GetEmpty(), obj);
}

bool SizeEditor::OnBaseValueChangedOverride(Element* sender)
{
    bool bChanged = false;
    IntegerControl* elem = suic::RTTICast<IntegerControl>(sender);
    String strName = elem->GetName();

    if (NULL != elem)
    {
        Size size;
        if (GetSize())
        {
            size = GetSize()->ToSize();
        }

        Size oldSize = size;

        if (strName.Equals(_U("PART_cx")))
        {
            size.cx = elem->GetInt();
        }
        else if (strName.Equals(_U("PART_cy")))
        {
            size.cy = elem->GetInt();
        }

        if (oldSize != size)
        {
            UpdateRealValue(new OSize(size));
            bChanged = true;
        }
    }

    if (bChanged)
    {
        NotifyValueToEditRootPanel();
    }

    return bChanged;
}

/*void SizeEditor::OnPreviewKeyDown(suic::KeyboardEventArg* e)
{
    EditorControl::OnPreviewKeyDown(e);
    if (e->GetKey() == suic::Key::kReturn)
    {
        e->SetHandled(true);

        IntegerControl* xCtrl = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_cx")));
        IntegerControl* yCtrl = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_cy")));

        Size size;
        if (GetSize())
        {
            size = GetSize()->ToSize();
        }
        size.cx = xCtrl->GetInt();
        size.cy = yCtrl->GetInt();
        UpdateRealValue(new OSize(size));
        NotifyValueToEditRootPanel();
    }
}*/

//=====================================================
// fSizeEditor
fSizeEditor::fSizeEditor()
{
}

fSizeEditor::~fSizeEditor()
{
}

void fSizeEditor::OnUpdateValue()
{
    FloatControl* xCtrl = RTTICast<FloatControl>(GetTemplateChild(_U("PART_cx")));
    FloatControl* yCtrl = RTTICast<FloatControl>(GetTemplateChild(_U("PART_cy")));
    OfSize* val = GetfSize();

    if (xCtrl != NULL && yCtrl != NULL && NULL != val)
    {
        xCtrl->InitFloat(val->TofSize().cx);
        yCtrl->InitFloat(val->TofSize().cy);
    }
}

void fSizeEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(OfSize::GetEmpty(), obj);
}

bool fSizeEditor::OnBaseValueChangedOverride(Element* sender)
{
    bool bChanged = false;
    FloatControl* elem = suic::RTTICast<FloatControl>(sender);
    String strName = elem->GetName();

    if (NULL != elem)
    {
        fSize size;
        if (GetfSize())
        {
            size = GetfSize()->TofSize();
        }
        fSize oldSize = size;

        if (strName.Equals(_U("PART_cx")))
        {
            size.cx = elem->GetFloat();
        }
        else if (strName.Equals(_U("PART_cy")))
        {
            size.cy = elem->GetFloat();
        }

        if (!suic::FloatUtil::AreClose(oldSize.cx, size.cx) || 
            !suic::FloatUtil::AreClose(oldSize.cy, size.cy))
        {
            UpdateRealValue(new OfSize(size));
            bChanged = true;
        }
    }

    if (bChanged)
    {
        NotifyValueToEditRootPanel();
    }

    return bChanged;
}

//=====================================================
// PointEditor
PointEditor::PointEditor()
{
}

PointEditor::~PointEditor()
{
}

void PointEditor::OnUpdateValue()
{
    IntegerControl* xCtrl = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_x")));
    IntegerControl* yCtrl = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_y")));
    OPoint* val = GetPoint();

    if (xCtrl != NULL && yCtrl != NULL && NULL != val)
    {
        xCtrl->InitInteger(val->ToPoint().x);
        yCtrl->InitInteger(val->ToPoint().y);
    }
}

void PointEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(OPoint::GetEmpty(), obj);
}

bool PointEditor::OnBaseValueChangedOverride(Element* sender)
{
    bool bChanged = false;
    IntegerControl* elem = suic::RTTICast<IntegerControl>(sender);
    String strName = elem->GetName();

    if (NULL != elem)
    {
        Point pt;
        if (GetPoint())
        {
            pt = GetPoint()->ToPoint();
        }

        Point oldPoint = pt;

        if (strName.Equals(_U("PART_cx")))
        {
            pt.x = elem->GetInt();
        }
        else if (strName.Equals(_U("PART_cy")))
        {
            pt.y = elem->GetInt();
        }

        if (oldPoint != pt)
        {
            bChanged = true;
            UpdateRealValue(new OPoint(pt));
        }
    }

    if (bChanged)
    {
        NotifyValueToEditRootPanel();
    }

    return bChanged;
}

//=====================================================
// fPointEditor

RoutedEvent* fPointEditor::PointChangedEvent;

fPointEditor::fPointEditor()
{
}

fPointEditor::~fPointEditor()
{
}

void fPointEditor::StaticInit()
{
    PointChangedEvent = EventHelper::RegisterRoutedEvent(_U("PointChanged"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
}

void fPointEditor::OnUpdateValue()
{
    FloatControl* xCtrl = RTTICast<FloatControl>(GetTemplateChild(_U("PART_x")));
    FloatControl* yCtrl = RTTICast<FloatControl>(GetTemplateChild(_U("PART_y")));
    OfPoint* val = GetfPoint();

    if (xCtrl != NULL && yCtrl != NULL && NULL != val)
    {
        xCtrl->InitFloat(val->TofPoint().x);
        yCtrl->InitFloat(val->TofPoint().y);
    }
}

void fPointEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(OfPoint::GetEmpty(), obj);
}

void fPointEditor::SetfPoint(fPoint point)
{
    UpdateRealValue(new OfPoint(point));
}

void fPointEditor::AddPointChanged(RoutedEventHandler* h)
{
    AddHandler(PointChangedEvent, h);
}

void fPointEditor::OnApplyTemplate()
{
    OnUpdateValue();
}

bool fPointEditor::OnBaseValueChangedOverride(Element* sender)
{
    bool bChanged = false;
    FloatControl* elem = suic::RTTICast<FloatControl>(sender);
    String strName = elem->GetName();

    if (NULL != elem)
    {
        fPoint pt;
        if (GetfPoint())
        {
            pt = GetfPoint()->TofPoint();
        }

        fPoint oldPoint = pt;

        if (strName.Equals(_U("PART_x")))
        {
            pt.x = elem->GetFloat();
        }
        else if (strName.Equals(_U("PART_y")))
        {
            pt.y = elem->GetFloat();
        }

        if (!suic::FloatUtil::AreClose(oldPoint.x, pt.x) || 
            !suic::FloatUtil::AreClose(oldPoint.y, pt.y))
        {
            bChanged = true;
            UpdateRealValue(new OfPoint(pt));
        }
    }

    if (bChanged)
    {
        NotifyValueToEditRootPanel();
        RoutedEventArg e(this, PointChangedEvent);
        RaiseEvent(&e);
    }

    return bChanged;
}

//=====================================================
// RectEditor
RectEditor::RectEditor()
{
}

RectEditor::~RectEditor()
{
}

void RectEditor::SetORect(ORect* val)
{
    if (NULL != val)
    {
        UpdateRealValue(val);
    }
}

ORect* RectEditor::GetORect()
{
    return RTTICast<ORect>(GetRealValue());
}

void RectEditor::OnApplyTemplate()
{
    OnUpdateValue();
}

void RectEditor::OnUpdateValue()
{
    Rect rect;
    if (GetORect() != NULL)
    {
        rect = GetORect()->ToRect();
    }

    IntegerControl* elem = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_Left")));
    if (NULL != elem)
    {
        elem->InitInteger(rect.left);
    }

    elem = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_Top")));
    if (NULL != elem)
    {
        elem->InitInteger(rect.top);
    }

    elem = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_Right")));
    if (NULL != elem)
    {
        elem->InitInteger(rect.right);
    }

    elem = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_Bottom")));
    if (NULL != elem)
    {
        elem->InitInteger(rect.bottom);
    }
}

void RectEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(ORect::GetEmpty(), obj);
}

bool RectEditor::UpdateORect()
{
    bool bChanged = false;
    IntegerControl* pLeft = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_Left")));
    IntegerControl* pTop = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_Top")));
    IntegerControl* pRight = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_Right")));
    IntegerControl* pBottom = RTTICast<IntegerControl>(GetTemplateChild(_U("PART_Bottom")));

    if (NULL != pLeft)
    {
        Rect rect;
        if (GetORect() != NULL)
        {
            rect = GetORect()->ToRect();
        }

        Rect oldrc = rect;

        rect.left = pLeft->GetInt();
        rect.top = pTop->GetInt();
        rect.right = pRight->GetInt();
        rect.bottom = pBottom->GetInt();

        if (!oldrc.Equal(&rect))
        {
            bChanged = true;
            UpdateRealValue(new ORect(rect));
        }
    }

    return bChanged;
}

bool RectEditor::UpdateORect(Element* sender)
{
    bool bChanged = false;
    IntegerControl* elem = suic::RTTICast<IntegerControl>(sender);
    String strName = elem->GetName();

    if (NULL != elem)
    {
        Rect rect;
        if (GetORect() != NULL)
        {
            rect = GetORect()->ToRect();
        }

        Rect oldrc = rect;

        if (strName.Equals(_U("PART_Left")))
        {
            rect.left = elem->GetInt();
            _changedVal = _U("left");
        }
        else if (strName.Equals(_U("PART_Top")))
        {
            rect.top = elem->GetInt();
            _changedVal = _U("top");
        }
        else if (strName.Equals(_U("PART_Right")))
        {
            rect.right = elem->GetInt();
            _changedVal = _U("right");
        }
        else if (strName.Equals(_U("PART_Bottom")))
        {
            rect.bottom = elem->GetInt();
            _changedVal = _U("bottom");
        }

        if (!oldrc.Equal(&rect))
        {
            bChanged = true;
            UpdateRealValue(new ORect(rect));

            if (RectValueChanged.safe_bool())
            {
                RectValueChanged(this, &EventArg::Empty);
            }
        }
    }

    return bChanged;
}

bool RectEditor::OnBaseValueChangedOverride(Element* sender)
{
    if (UpdateORect(sender))
    {
        NotifyValueToEditRootPanel();
        return true;
    }
    else
    {
        return false;
    }
}

String RectEditor::GetChangedVal() const
{
    return _changedVal;
}

//=====================================================
// fRectEditor
fRectEditor::fRectEditor()
{
}

fRectEditor::~fRectEditor()
{
}

void fRectEditor::OnApplyTemplate()
{
    OnUpdateValue();
}

void fRectEditor::OnUpdateValue()
{
    fRect rect;
    if (GetfRect() != NULL)
    {
        rect = GetfRect()->TofRect();
    }

    FloatControl* elem = RTTICast<FloatControl>(GetTemplateChild(_U("PART_Left")));
    if (NULL != elem)
    {
        elem->InitFloat(rect.left);
    }

    elem = RTTICast<FloatControl>(GetTemplateChild(_U("PART_Top")));
    if (NULL != elem)
    {
        elem->InitFloat(rect.top);
    }

    elem = RTTICast<FloatControl>(GetTemplateChild(_U("PART_Right")));
    if (NULL != elem)
    {
        elem->InitFloat(rect.right);
    }

    elem = RTTICast<FloatControl>(GetTemplateChild(_U("PART_Bottom")));
    if (NULL != elem)
    {
        elem->InitFloat(rect.bottom);
    }
}

void fRectEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(OfRect::GetEmpty(), obj);
}

bool fRectEditor::OnBaseValueChangedOverride(Element* sender)
{
    bool bChanged = false;
    FloatControl* elem = suic::RTTICast<FloatControl>(sender);
    String strName = elem->GetName();

    if (NULL != elem)
    {
        fRect rect;
        if (GetfRect() != NULL)
        {
            rect = GetfRect()->TofRect();
        }

        fRect oldRect = rect;

        if (strName.Equals(_U("PART_Left")))
        {
            rect.left = elem->GetFloat();
            _changedVal = _U("left");
        }
        else if (strName.Equals(_U("PART_Top")))
        {
            rect.top = elem->GetFloat();
            _changedVal = _U("top");
        }
        else if (strName.Equals(_U("PART_Right")))
        {
            rect.right = elem->GetFloat();
            _changedVal = _U("right");
        }
        else if (strName.Equals(_U("PART_Bottom")))
        {
            rect.bottom = elem->GetFloat();
            _changedVal = _U("bottom");
        }

        if (oldRect != rect)
        {
            bChanged = true;
            UpdateRealValue(new OfRect(rect));
        }
    }

    if (bChanged)
    {
        NotifyValueToEditRootPanel();
        if (RectValueChanged.safe_bool())
        {
            RectValueChanged(this, &EventArg::Empty);
        }
    }

    return bChanged;
}

//=====================================================
// ImageSourceEditor
ImageSourceEditor::ImageSourceEditor()
{
    _info = NULL;
}

ImageSourceEditor::~ImageSourceEditor()
{
}

void ImageSourceEditor::OnUpdateValue()
{
    if (NULL != _info && NULL != GetImageSource())
    {
        _info->SetText(GetImageSource()->GetUri().ToString());
    }
}

void ImageSourceEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(new ImageSource(), obj);
}

void ImageSourceEditor::OnApplyTemplate()
{
    suic::Button* pBtn = RTTICast<suic::Button>(GetTemplateChild(_U("PART_EditSource")));
    if (NULL != pBtn)
    {
        pBtn->AddClick(new suic::ClickEventHandler(this, &ImageSourceEditor::OnSourceClick));
    }
    _info = RTTICast<suic::TextBox>(GetTemplateChild(_U("PART_Source")));

    OnUpdateValue();
}

bool ImageSourceEditor::OnBaseValueChangedOverride(Element* sender)
{
    return true;
}

void ImageSourceEditor::OnSourceClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    EditRootPanel* pEditPanel = FindEditRttoPanel(this);

    e->SetHandled(true);

    if (pEditPanel->GetRootItem() == NULL)
    {
        return;
    }

    suic::String imgSource;
    Project* pPrj = pEditPanel->GetRootItem()->GetProject();
    const String strPath = "/mpfuid;/resource/uidesign/layout/ImagesSelector.xaml";
    ItemCollection* obserItems = pPrj->GetItemColl();
    ImageSelectorWindow imgWindow(pPrj->GetSlnTreeManager(), obserItems, pEditPanel->GetResDir());

    imgWindow.setAutoDelete(false);
    imgWindow.ShowDialog(strPath);

    imgSource = imgWindow.GetSelectedImage();

    if (!imgSource.Empty())
    {
        ResourceUri resUri;

        resUri.SetComponent(pEditPanel->GetResName());
        resUri.SetResourcePath(imgSource);
        suic::ImageSource* imgSrc = new ImageSource();

        imgSrc->SetUri(resUri);
        UpdateRealValue(imgSrc);

        NotifyValueToEditRootPanel();
    }
}

//=====================================================
// BrushEditor

RoutedEvent* BrushEditor::BrushChangedEvent;

void BrushEditor::StaticInit()
{
    BrushChangedEvent = EventHelper::RegisterRoutedEvent(_U("BrushChanged"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
}

BrushEditor::BrushEditor()
{
    _needNotify = false;
}

BrushEditor::~BrushEditor()
{
    
}

void BrushEditor::OnUpdateValue()
{
}

void BrushEditor::NotifyBrushChanged()
{
    if (_needNotify)
    {
        RoutedEventArg e(this, BrushChangedEvent);
        RaiseEvent(&e);
        NotifyValueToEditRootPanel();
    }

    UpdateLayout();
}

void BrushEditor::AddBrushChanged(RoutedEventHandler* h)
{
    AddHandler(BrushChangedEvent, h);
}

//void BrushEditor::OnBrushValueChanged()
//{
//
//}

//====================================================
// SolidColorBrushEditor

SolidColorBrushEditor::SolidColorBrushEditor()
{
    _colorPicker = NULL;
}

void SolidColorBrushEditor::OnUpdateValue()
{
    SolidColorBrush* pVal = RTTICast<SolidColorBrush>(GetRealValue());
    if (_colorPicker == NULL)
    {
        ApplyTemplate();
    }

    if (_colorPicker)
    {
        if (NULL != pVal)
        {
            _colorPicker->SetInitColor(pVal);
        }
        else
        {
            if (!_resValue)
            {
                ResNode::CreateResNode(_colorPicker->GetColor(), _resValue);
            }
            else
            {
                _resValue->SetValue(_colorPicker->GetColor());
            }
            NotifyBrushChanged();
        }
    }
}

void SolidColorBrushEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ResNode::CreateResNode(SolidColorBrush::White, obj);
}

ResNodePtr SolidColorBrushEditor::GetCloneResValue()
{
    SolidColorBrush* pVal = RTTICast<SolidColorBrush>(GetRealValue());
    if (NULL != pVal)
    {
        return new SolidColorBrushResNode(new SolidColorBrush(pVal->GetColor()));
    }
    else
    {
        if (_colorPicker == NULL)
        {
            ApplyTemplate();
        }
       
        if (_colorPicker != NULL)
        {
            return new SolidColorBrushResNode(_colorPicker->GetColor());
        }
        else
        {
            return new SolidColorBrushResNode(new suic::SolidColorBrush(0xFFFFFFFF));
        }
    }
}

void SolidColorBrushEditor::OnColorChanged(DpObject* sender, RoutedEventArg* e)
{
    OnColorChanged(_colorPicker->GetColor());
}

void SolidColorBrushEditor::OnColorChanged(suic::SolidColorBrush* color)
{
    UpdateRealValue(color);
    NotifyBrushChanged();
}

void SolidColorBrushEditor::OnApplyTemplate()
{
    _colorPicker = RTTICast<ColorPicker>(GetTemplateChild(_U("PART_ColorPicker")));
    if (_colorPicker != NULL)
    {
        _colorPicker->AddColorChanged(new RoutedEventHandler(this, &SolidColorBrushEditor::OnColorChanged));
    }

    OnUpdateValue();

    _needNotify = true;
}

//====================================================
// ImageBrushEditor

ImageBrushEditor::ImageBrushEditor()
{
    _needClone = true;
    _imgShow = NULL;
}

ImageBrushResNode* ImageBrushEditor::GetImageBrushResNode()
{
    ImageBrushResNode* ibResNode = DynamicCast<ImageBrushResNode>(_resValue.get());
    return ibResNode;
}

Button* ImageBrushEditor::GetImageShowBtn()
{
    if (NULL == _imgShow)
    {
        _imgShow = RTTICast<suic::Button>(GetTemplateChild(_U("ImageShowBtn")));
    }
    return _imgShow;
}

void ImageBrushEditor::OnUpdateValue()
{
    _needClone = true;

    ImageBrush* imgBrush = GetImageBrush();

    if (NULL == imgBrush)
    {
        CreateDefaultValue(_resValue);
        imgBrush = GetImageBrush();
        _needClone = false;
    }

    ImageBrushResNode* imgResNode = GetImageBrushResNode();

    if (NULL != GetImageShowBtn())
    {
        GetImageShowBtn()->SetBackground(imgBrush);
    }

    ComboBox* pCmb = RTTICast<ComboBox>(GetTemplateChild(_U("cmbTileMode")));
    if (NULL != pCmb)
    {
        pCmb->SetSelectedIndex(imgBrush->GetTileMode());
    }

    pCmb = RTTICast<ComboBox>(GetTemplateChild(_U("cmbStretch")));
    if (NULL != pCmb)
    {
        pCmb->SetSelectedIndex(imgBrush->GetStretch());
    }

    suic::TextBox* pImgSource = RTTICast<suic::TextBox>(GetTemplateChild(_U("cmbImageSource")));
    if (NULL != pImgSource)
    {
        pImgSource->SetText(imgResNode->GetResPath()->GetResourcePath());
    }
}

void ImageBrushEditor::OnBrushChanged(Brush* brush)
{
}

void ImageBrushEditor::OnButtonClick(Element* sender, RoutedEventArg* e)
{
    e->SetHandled(true);

    ImageBrush* imgBrush = GetImageBrush();

    if (imgBrush != NULL && imgBrush->GetImageSource() != NULL)
    {
        ImageEditWindow imageWnd(GetImageShowBtn());
        imageWnd.setAutoDelete(false);

        suic::Builder::BuildUri(&imageWnd, "/mpfuid;/resource/uidesign/layout/ImageBrushEditor.xaml");
        imageWnd.SetEditBitmap(imgBrush);
        imageWnd.ShowDialog();

        if (imageWnd.IsCommit())
        {
            NotifyBrushChanged();
        }
    }
}

void ImageBrushEditor::RefleshImageShow() 
{
    suic::Button* pImgShow = RTTICast<suic::Button>(GetTemplateChild(_U("ImageShowBtn")));
    if (NULL != pImgShow)
    {
        pImgShow->SetBackground(GetImageBrush());
    }
}

void ImageBrushEditor::OnTileModeChanged(Element* sender, SelectionChangedEventArg* e)
{
    e->SetHandled(true);

    ImageBrush* imgBrush = CloneBrush();
    ComboBox* pCmb = RTTICast<ComboBox>(sender);
    imgBrush->SetTileMode(pCmb->GetSelectedIndex());

    RefleshImageShow();
    NotifyBrushChanged();
}

void ImageBrushEditor::OnStretchChanged(Element* sender, SelectionChangedEventArg* e)
{
    e->SetHandled(true);

    ImageBrush* imgBrush = CloneBrush();
    ComboBox* pCmb = RTTICast<ComboBox>(sender);
    imgBrush->SetStretch(pCmb->GetSelectedIndex());

    RefleshImageShow();
    NotifyBrushChanged();
}

void ImageBrushEditor::OnSelImageBtn(Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    EditRootPanel* pEditPanel = FindEditRttoPanel(this);

    if (pEditPanel->GetRootItem() == NULL)
    {
        return;
    }

    Project* pPrj = pEditPanel->GetRootItem()->GetProject();
    const String strPath = "/mpfuid;/resource/uidesign/layout/ImagesSelector.xaml";
    ItemCollection* obserItems = pPrj->GetItemColl();
    ImageSelectorWindow imgWindow(pPrj->GetSlnTreeManager(), obserItems, pEditPanel->GetResDir());

    imgWindow.setAutoDelete(false);
    imgWindow.ShowDialog(strPath);

    if (!imgWindow.GetSelectedImage().Empty())
    {
        suic::Button* pImgShow = RTTICast<suic::Button>(GetTemplateChild(_U("ImageShowBtn")));
        ImageBrushResNode* ibResNode = DynamicCast<ImageBrushResNode>(_resValue.get());

        if (NULL != pImgShow && NULL != ibResNode)
        {
            CloneBrush();

            suic::ImageBrush* imgBrush = ibResNode->GetImageBrush();
            suic::String imgSource = imgWindow.GetSelectedImage();
            suic::TextBox* pImgSource = RTTICast<suic::TextBox>(GetTemplateChild(_U("cmbImageSource")));
            ResourceUri* resPath = ibResNode->GetResPath();

            resPath->SetComponent(pEditPanel->GetResName());
            resPath->SetResourcePath(imgSource);

            imgBrush = ibResNode->RefleshImageSource();
 
            if (NULL != pImgSource)
            {
                pImgSource->SetText(resPath->GetResourcePath());
            }

            imgBrush->SetViewboxUnits(BrushMappingMode::bmmRelativeToBoundingBox);
            imgBrush->SetViewbox(suic::fRect(0, 0, 1, 1));
            imgBrush->SetViewcorner(suic::Rect());
            pImgShow->SetBackground(imgBrush);

            NotifyBrushChanged();
        }
    }
}

void ImageBrushEditor::OnInitialized(suic::EventArg* e)
{
    ;
}

ImageBrush* ImageBrushEditor::GetImageBrush()
{
    return RTTICast<ImageBrush>(GetRealValue());
}

void ImageBrushEditor::CreateDefaultValue(ResNodePtr& obj)
{
    ImageBrush* imgBrush = new ImageBrush();
    if (NULL != imgBrush)
    {
    }
    ResNode::CreateResNode(imgBrush, obj);
}

ImageBrush* ImageBrushEditor::CloneBrush()
{
    ImageBrush* imgBrush = GetImageBrush();
    ImageBrush* newBrush = NULL;

    if (NULL != imgBrush)
    {
        if (!_needClone)
        {
            _needClone = false;
            return imgBrush;
        }

        newBrush = new ImageBrush();

        newBrush->SetStretch(imgBrush->GetStretch());
        newBrush->SetTileMode(imgBrush->GetTileMode());

        newBrush->SetViewbox(imgBrush->GetViewbox());
        newBrush->SetViewboxUnits(imgBrush->GetViewboxUnits());
        newBrush->SetViewport(imgBrush->GetViewport());
        newBrush->SetViewportUnits(imgBrush->GetViewportUnits());
        newBrush->SetViewcorner(imgBrush->GetViewCorner());

        newBrush->SetImageSource(imgBrush->GetImageSource());

        _resValue->SetValue(newBrush);
    }
    else
    {
        CreateDefaultValue(_resValue);
        newBrush = GetImageBrush();
    }

    _needClone = false;

    return newBrush;
}

ResNodePtr ImageBrushEditor::GetCloneResValue()
{
    return new ImageBrushResNode(CloneBrush());
}

void ImageBrushEditor::OnApplyTemplate()
{
    OnUpdateValue();

    ComboBox* pCmb = RTTICast<ComboBox>(GetTemplateChild(_U("cmbTileMode")));
    if (NULL != pCmb)
    {
        pCmb->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &ImageBrushEditor::OnTileModeChanged));
    }

    pCmb = RTTICast<ComboBox>(GetTemplateChild(_U("cmbStretch")));
    if (NULL != pCmb)
    {
        pCmb->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &ImageBrushEditor::OnStretchChanged));
    }

    suic::Button* pSelImgBtn = RTTICast<suic::Button>(GetTemplateChild(_U("selImgBtn")));
    if (NULL != pSelImgBtn)
    {
        pSelImgBtn->AddClick(new suic::ClickEventHandler(this, &ImageBrushEditor::OnSelImageBtn));
    }

    suic::Button* pImgsHOWBtn = RTTICast<suic::Button>(GetTemplateChild(_U("ImageShowBtn")));
    if (NULL != pImgsHOWBtn)
    {
        pImgsHOWBtn->AddClick(new suic::ClickEventHandler(this, &ImageBrushEditor::OnButtonClick));
    }

    _needNotify = true;
}

//====================================================
// GradientStop

GradientStop::GradientStop()
{
    _isDraging = false;
    _isClicking = false;

    _startpos = 0;
    _currentStop = 0;
    _stops = NULL;
    _path = new PathGeometry();
    _path->ref();

    _path->MoveTo(5, 0);
    _path->LineTo(10,5);
    _path->LineTo(10,15);
    _path->LineTo(0,15);
    _path->LineTo(0,5);
    
    _path->Close();

    _bk = new LinearGradientBrush();
    _bk->ref();

    _bk->SetStartPoint(fPoint(0,0.5));
    _bk->SetEndPoint(fPoint(1,0.5));
}

GradientStop::~GradientStop()
{
    _bk->unref();
    _path->unref();
    if (NULL != _stops)
    {
        _stops->unref();
    }
}

suic::Color GradientStop::GetCurrentColor()
{
    suic::Color clr = Colors::White;
    GradientStops* stops = GetGradientStops();

    if (stops != NULL && _currentStop >= 0 && _currentStop < stops->colors.GetCount())
    {
        clr = stops->colors[_currentStop];
    }
    return clr;
}

void GradientStop::SetCurrentColor(suic::Color color)
{
    if (_isClicking)
    {
        return;
    }

    GradientStops* stops = GetGradientStops();
    if (stops != NULL && _currentStop >= 0 && 
        _currentStop < stops->colors.GetCount())
    {
        stops->colors[_currentStop] = color;
    }
}

void GradientStop::InitGradientBrush(suic::GradientBrush* brush)
{
    if (NULL == brush)
    {
        return;
    }
    if (NULL == _stops)
    {
        _stops = new suic::GradientStops();
        _stops->ref();
    }
    else
    {
        _stops->colors.Clear();
        _stops->ratios.Clear();
    }

    GradientStops* stops = brush->GetGradientStops();
    for (int i = 0; i < stops->GetCount(); ++i)
    {
        _stops->AddStops(stops->ratios[i], stops->colors[i]);
    }

    if (_currentStop < 0 || _currentStop >= _stops->GetCount())
    {
        _currentStop = 0;
    }
}

suic::GradientStops* GradientStop::GetGradientStops()
{
    if (NULL == _stops)
    {
        _stops = new suic::GradientStops();
        _stops->AddStops(0.0f, Colors::White);
        _stops->AddStops(1.0f, Colors::Black);
        _stops->ref();
    }
    return _stops;
}

int GradientStop::HitTestRatio(Point pt, Float& fDis)
{
    GradientStops* stops = GetGradientStops();

    if (NULL != stops)
    {
        Float fWid = GetActualWidth() - 10;

        for (int i = 0; i < stops->colors.GetCount(); ++i)
        {
            Float ratio = stops->ratios[i];
            Rect rctmp;

            rctmp.bottom = GetActualHeight();
            rctmp.left = (int)fWid * ratio;
            rctmp.right = rctmp.left + 10;
            if (rctmp.PointIn(pt))
            {
                fDis = rctmp.left + 5;
                return i;
            }
        }
    }

    return -1;
}

Float GradientStop::GetRatioDis(int index)
{
    GradientStops* stops = GetGradientStops();

    if (stops != NULL && index >= 0 && 
        index < stops->colors.GetCount())
    {
        Float fRatio = stops->ratios[index];
        Float fWid = GetActualWidth() - 10;
        fWid = fWid * fRatio + 5;
        return fWid;
    }
    else if (index > 0)
    {
        Float fWid = GetActualWidth() - 5;
        return fWid;
    }
    else
    {
        return 5;
    }
}

void GradientStop::NotifyStopsChanged()
{
    GradientBrushEditor* brush = RTTICast<GradientBrushEditor>(GetTemplatedParent());
    if (NULL != brush)
    {
        brush->OnGradientStopChanged();
        brush->NotifyBrushChanged();
    }
    UpdateLayout();
}

void GradientStop::UpdateStopRatio(Float fRatio)
{
    GradientStops* stops = GetGradientStops();

    if (stops != NULL && _currentStop >= 0 && 
        _currentStop < stops->colors.GetCount())
    {
        stops->ratios[_currentStop] = fRatio;
        NotifyStopsChanged();
    }
}

void GradientStop::UpdateCurrentStop(int iCurrStop)
{
    GradientStops* stops = GetGradientStops();
    _currentStop = iCurrStop;
    GradientBrushEditor* brush = RTTICast<GradientBrushEditor>(GetTemplatedParent());
    if (NULL != brush)
    {
        _isClicking = true;
        Color clr = stops->colors[iCurrStop];
        brush->UpdateCurrentColor(clr);
        _isClicking = false;
    }
}

void GradientStop::RemoveCurrentStop()
{
    GradientStops* stops = GetGradientStops();

    if (NULL != stops && stops->GetCount() > 2)
    {
        stops->colors.RemoveAt(_currentStop);
        stops->ratios.RemoveAt(_currentStop);
        UpdateCurrentStop(0);
        UpdateLayout();

        NotifyStopsChanged();
    }
}

void GradientStop::AddGradientStop(Point pt, Color clr)
{
    GradientStops* stops = GetGradientStops();

    if (NULL != stops)
    {        
        Float ratio = 0;
        Float fWid = GetActualWidth() - 10;
        int index = stops->colors.GetCount();

        for (int i = 0; i < stops->colors.GetCount(); ++i)
        {
            ratio = stops->ratios[i];
            Float fDis = fWid * ratio + 5;

            if (pt.x < fDis)
            {
                index = i;
                break;
            }
        }

        ratio = (pt.x - 5) / fWid; 

        if (index >= stops->colors.GetCount())
        {
            stops->colors.Add(clr);
            stops->ratios.Add(ratio);
        }
        else
        {
            stops->colors.Insert(index, clr);
            stops->ratios.Insert(index, ratio);
        }
        _currentStop = index;
        NotifyStopsChanged();
    }
}

void GradientStop::OnRender(suic::Drawing* drawing)
{
    const int DRAWWID = 10;
    const int DRAWWIDSELF = 5;
    Float fWid = GetActualWidth() - DRAWWID;
    GradientStops* stops = GetGradientStops();

    fRect rect(Point(), GetRenderSize());
    rect.left += DRAWWIDSELF;
    rect.right -= DRAWWIDSELF;

    if (stops != NULL)
    {
        rect.bottom = rect.top + 16;
        _bk->SetGradientStops(stops);
        drawing->DrawRect(suic::DrawCtx::DefDraw, _bk, NULL, &rect);
    }

    if (NULL != stops)
    {
        for (int i = 0; i < stops->colors.GetCount(); ++i)
        {
            Float ratio = stops->ratios[i];
            fRect rctmp;

            rctmp.top = rect.bottom - 5;
            rctmp.bottom = rctmp.top + 15;
            rctmp.left = fWid * ratio;
            rctmp.right = rctmp.left + DRAWWID;

            drawing->Save();
            Pen pen;
            SolidColorBrush fillBr(stops->colors[i]);
            fillBr.setAutoDelete(false);

            if (i == _currentStop)
            {
                pen.SetThickness(2);
            }
            else
            {
                pen.SetThickness(1);
            }

            fPoint pt(rctmp.left, rctmp.top);
            drawing->PushOffset(pt);
            drawing->DrawGeometry(suic::DrawCtx::DefDraw, SolidColorBrush::White, &pen, _path);           
            fRect rc(2,7,8,13);
            pen.SetThickness(2);
            drawing->DrawRect(suic::DrawCtx::DefDraw, &fillBr, &pen, &rc);          
            drawing->PopOffset();
            drawing->Restore();
            //drawing->FillRect(Colors::Red, &rctmp);
        }
    }
}

void GradientStop::OnMouseMove(MouseButtonEventArg* e)
{
    FrameworkElement::OnMouseMove(e);

    if (_isDraging && MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress)
    {
        suic::Point thumbPos = e->GetMousePoint();

        if (thumbPos.x != _prevPoint.x)
        {
            int horiChange = thumbPos.x - _originPoint.x;            

            _prevPoint = thumbPos;
            e->SetHandled(true);

            // 更新当前的比例
            Float fDis = _startpos + horiChange - 5;
            Float fRatio = fDis / (GetActualWidth() - 10);
            if (horiChange > 0)
            {
                Float fNextDis = GetRatioDis(_currentStop + 1);
                if (FloatUtil::LessThan(fDis + 5, fNextDis))
                {
                    UpdateStopRatio(fRatio);
                }
            }
            else
            {
                Float fPrevDis = GetRatioDis(_currentStop - 1);
                if (FloatUtil::GreaterThan(fDis + 5, fPrevDis))
                {
                    UpdateStopRatio(fRatio);
                }
            }
        }
    }
}

void GradientStop::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    Float fDis = 0;
    Point ptScreen = PointToScreen(Point());
    ptScreen.x = e->GetMousePoint().x - ptScreen.x;
    ptScreen.y = e->GetMousePoint().y - ptScreen.y;

    int iCurrStop = HitTestRatio(ptScreen, fDis);

    if (iCurrStop != -1 && !_isDraging)
    {
        _isDraging = true;
        UpdateCurrentStop(iCurrStop);

        e->SetHandled(true);
        // 开始拖动，捕获鼠标控制权
        SetCaptureMouse();

        // 初始化数据
        _startpos = fDis;
        _prevPoint = e->GetMousePoint();
        _originPoint = _prevPoint;
    }
    else if (iCurrStop == -1)
    {
        //Float fRatio = ptScreen.x - 5;
        AddGradientStop(ptScreen, GetCurrentColor());
    }

    FrameworkElement::OnMouseLeftButtonDown(e);
}

void GradientStop::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    if (IsMouseCaptured() && _isDraging)
    { 
        e->SetHandled(true);
        _isDraging = false;
        ReleaseCaptureMouse();

        Point currPt = PointFromScreen(e->GetMousePoint());
        if (currPt.y > GetActualHeight() + 10)
        {
            RemoveCurrentStop();
        }
    }
    else
    {
        _isDraging = false;
    }

    FrameworkElement::OnMouseLeftButtonUp(e); 
}

//====================================================
// GradientBrushEditor

GradientBrushEditor::GradientBrushEditor()
{
    _stops = NULL;
    _needClone = true;
    _colorPicker = NULL;
}

GradientBrushEditor::~GradientBrushEditor()
{

}

void GradientBrushEditor::UpdateCurrentColor(Color clr)
{
    if (_colorPicker != NULL)
    {
        _colorPicker->SetColor(new suic::SolidColorBrush(clr));
        //_colorPicker->UpdateCurrentColor(clr);
        //_colorPicker->UpdateLayout();
    }
}

void GradientBrushEditor::OnUpdateValue()
{
    SolidColorBrush* pVal = RTTICast<SolidColorBrush>(GetRealValue());
    if (_colorPicker && pVal)
    {
        _colorPicker->SetInitColor(pVal);
    }
}

void GradientBrushEditor::UpdateGradienStops(GradientBrush* brush)
{
    if (_stops != NULL)
    {
        GradientStops* stops = brush->GetGradientStops();
        GradientStops* stopsSelf = _stops->GetGradientStops();

        stops->colors.Clear();
        stops->ratios.Clear();

        for (int i = 0; i < stopsSelf->GetCount(); ++i)
        {
            stops->AddStops(stopsSelf->ratios[i], stopsSelf->colors[i]);
        }
    }
}

void GradientBrushEditor::UpdateGradienValue(GradientBrush* val)
{
    if (_stops != NULL)
    {
        _stops->InitGradientBrush(val);

        if (_colorPicker != NULL)
        {
            _stops->SetIsClicking(true);
            _colorPicker->SetColor(new SolidColorBrush(_stops->GetCurrentColor()));
            _stops->SetIsClicking(false);
        }
    }
}

void GradientBrushEditor::OnColorChanged(DpObject* sender, RoutedEventArg* e)
{
    _isPending = true;
    OnColorChanged(_colorPicker->GetColor());
    _isPending = false;
}

void GradientBrushEditor::OnColorChanged(suic::SolidColorBrush* color)
{
    if (_stops != NULL)
    {
        _stops->SetCurrentColor(color->GetColor());
    }

    OnGradientStopChanged();
    NotifyBrushChanged();
}

void GradientBrushEditor::OnGradientStopChanged()
{

}

void GradientBrushEditor::OnApplyTemplate()
{
    _stops = RTTICast<GradientStop>(GetTemplateChild(_U("PART_GradientStop")));
    _colorPicker = RTTICast<ColorPicker>(GetTemplateChild(_U("PART_ColorPicker")));

    UpdateGradienValue(GetGradientBrush());

    if (_colorPicker != NULL)
    {
        _colorPicker->AddColorChanged(new RoutedEventHandler(this, &GradientBrushEditor::OnColorChanged));
    }
}

//====================================================
// LinearGradientBrushEditor

LinearGradientBrushEditor::LinearGradientBrushEditor()
{
}

LinearGradientBrushEditor::~LinearGradientBrushEditor()
{

}

LinearGradientBrush* LinearGradientBrushEditor::CloneBrush()
{
    LinearGradientBrush* linearBr = GetLinearGradientBrush();
    LinearGradientBrush* newBrush = NULL;

    if (NULL != linearBr)
    {
        if (!_needClone)
        {
            _needClone = false;
            return linearBr;
        }

        newBrush = new LinearGradientBrush();

        GradientStops* stops = linearBr->GetGradientStops();
        newBrush->SetStartPoint(linearBr->GetStartPoint());
        newBrush->SetEndPoint(linearBr->GetEndPoint());

        for (int i = 0; i < stops->GetCount(); ++i)
        {
            newBrush->AddGradientStop(stops->colors[i], stops->ratios[i]);
        }

        _resValue->SetValue(newBrush);
    }
    else
    {
        CreateDefaultValue(_resValue);
        newBrush = GetLinearGradientBrush();
    }

    _needClone = false;

    return newBrush;
}

ResNodePtr LinearGradientBrushEditor::GetCloneResValue()
{
    return new LinearGradientBrushResNode(CloneBrush());
}

void LinearGradientBrushEditor::OnGradientStopChanged()
{
    LinearGradientBrush* linearBr = CloneBrush();
    UpdateGradienStops(linearBr);
}

void LinearGradientBrushEditor::OnfPointChanged(suic::DpObject* sender, RoutedEventArg* e)
{
    fPointEditor* elem = RTTICast<fPointEditor>(e->GetOriginalSource());
    LinearGradientBrush* linearBr = CloneBrush();

    if (elem->GetName().Equals(_U("PART_Start")))
    {
        linearBr->SetStartPoint(elem->GetfPoint()->TofPoint());
    }
    else if (elem->GetName().Equals(_U("PART_End")))
    {
        linearBr->SetEndPoint(elem->GetfPoint()->TofPoint());
    }

    NotifyBrushChanged();
}

LinearGradientBrush* LinearGradientBrushEditor::GetLinearGradientBrush()
{
    return RTTICast<LinearGradientBrush>(GetRealValue());
}

void LinearGradientBrushEditor::OnUpdateValue()
{
    _needClone = true;

    LinearGradientBrush* linearBr = GetLinearGradientBrush();
    if (NULL == linearBr)
    {
        CreateDefaultValue(_resValue);
        linearBr = GetLinearGradientBrush();
    }

    GradientBrushEditor::UpdateGradienValue(linearBr);

    fPointEditor* fEdit = RTTICast<fPointEditor>(GetTemplateChild(_U("PART_Start")));
    if (NULL != fEdit)
    {
        fEdit->SetfPoint(linearBr->GetStartPoint());
    }

    fEdit = RTTICast<fPointEditor>(GetTemplateChild(_U("PART_End")));
    if (NULL != fEdit)
    {
        fEdit->SetfPoint(linearBr->GetEndPoint());
    }
}

void LinearGradientBrushEditor::CreateDefaultValue(ResNodePtr& obj)
{
    LinearGradientBrush* linearBr = new LinearGradientBrush();
    if (NULL != linearBr)
    {
        linearBr->AddGradientStop(Colors::White, 0.0f);
        linearBr->AddGradientStop(Colors::Black, 1.0f);
    }
    ResNode::CreateResNode(linearBr, obj);
}

void LinearGradientBrushEditor::Dispose()
{
    _resValue = NULL;
    GradientBrushEditor::Dispose();
}

void LinearGradientBrushEditor::OnApplyTemplate()
{
    GradientBrushEditor::OnApplyTemplate();
    LinearGradientBrush* linearBr = GetLinearGradientBrush();

    if (NULL == linearBr)
    {
        CreateDefaultValue(_resValue);
        linearBr = GetLinearGradientBrush();
        NotifyBrushChanged();
    }

    if (NULL != linearBr)
    {
        GradientBrushEditor::UpdateGradienValue(linearBr);

        fPointEditor* elem = RTTICast<fPointEditor>(GetTemplateChild(_U("PART_Start")));

        if (NULL != elem)
        {
            elem->SetfPoint(linearBr->GetStartPoint());
            elem->AddPointChanged(new RoutedEventHandler(this, &LinearGradientBrushEditor::OnfPointChanged));
        }
        elem = RTTICast<fPointEditor>(GetTemplateChild(_U("PART_End")));
        if (NULL != elem)
        {
            elem->SetfPoint(linearBr->GetEndPoint());
            elem->AddPointChanged(new RoutedEventHandler(this, &LinearGradientBrushEditor::OnfPointChanged));
        }
    }

    _needNotify = true;
}

//====================================================
// RadialGradientBrushEditor

RadialGradientBrushEditor::RadialGradientBrushEditor()
{
}

void RadialGradientBrushEditor::OnColorChanged(suic::SolidColorBrush* color)
{
}

void RadialGradientBrushEditor::CreateDefaultValue(ResNodePtr& obj)
{
    RadialGradientBrush* radialBr = new RadialGradientBrush();
    if (NULL != radialBr)
    {
        radialBr->AddGradientStop(Colors::White, 0.0f);
        radialBr->AddGradientStop(Colors::Black, 1.0f);
    }
    ResNode::CreateResNode(radialBr, obj);
}

void RadialGradientBrushEditor::OnGradientStopChanged()
{
    RadialGradientBrush* radialBr = CloneBrush();
    UpdateGradienStops(radialBr);
}

void RadialGradientBrushEditor::OnUpdateValue()
{
    _needClone = true;

    RadialGradientBrush* radialBr = GetRadialGradientBrush();
    if (NULL == radialBr)
    {
        CreateDefaultValue(_resValue);
        radialBr = GetRadialGradientBrush();
    }

    GradientBrushEditor::UpdateGradienValue(radialBr);

    fPointEditor* fEdit = RTTICast<fPointEditor>(GetTemplateChild(_U("PART_Origin")));
    if (NULL != fEdit)
    {
        fEdit->SetfPoint(radialBr->GetOrigin());
    }

    FloatControl* fCtrl = RTTICast<FloatControl>(GetTemplateChild(_U("PART_RadiusX")));
    if (NULL != fCtrl)
    {
        fCtrl->InitFloat(radialBr->GetRadiusX());
    }

    fCtrl = RTTICast<FloatControl>(GetTemplateChild(_U("PART_RadiusY")));
    if (NULL != fCtrl)
    {
        fCtrl->InitFloat(radialBr->GetRadiusY());
    }
}

RadialGradientBrush* RadialGradientBrushEditor::GetRadialGradientBrush()
{
    return RTTICast<RadialGradientBrush>(GetRealValue());
}

ResNodePtr RadialGradientBrushEditor::GetCloneResValue()
{
    return new RadialGradientBrushResNode(CloneBrush());
}

RadialGradientBrush* RadialGradientBrushEditor::CloneBrush()
{
    RadialGradientBrush* radialBr = GetRadialGradientBrush();
    RadialGradientBrush* newBrush = NULL;

    if (NULL != radialBr)
    {
        if (!_needClone)
        {
            _needClone = false;
            return radialBr;
        }

        newBrush = new RadialGradientBrush();

        GradientStops* stops = radialBr->GetGradientStops();
        fPoint fOrigin = radialBr->GetOrigin();
        newBrush->SetOrigin(fOrigin.x, fOrigin.y);
        newBrush->SetRadiusXY(radialBr->GetRadiusX(), radialBr->GetRadiusY());

        for (int i = 0; i < stops->GetCount(); ++i)
        {
            newBrush->AddGradientStop(stops->colors[i], stops->ratios[i]);
        }

        _resValue->SetValue(newBrush);
    }
    else
    {
        CreateDefaultValue(_resValue);
        newBrush = GetRadialGradientBrush();
    }

    _needClone = false;

    return newBrush;
}

void RadialGradientBrushEditor::OnfPointChanged(suic::DpObject* sender, RoutedEventArg* e)
{
    fPointEditor* elem = RTTICast<fPointEditor>(e->GetOriginalSource());
    RadialGradientBrush* radialBr = CloneBrush();

    if (NULL != elem && elem->GetName().Equals(_U("PART_Origin")))
    {
        fPoint fOrigin = elem->GetfPoint()->TofPoint();
        radialBr->SetOrigin(fOrigin.x, fOrigin.y);
    }
    else
    {
        FloatControl* fCtrl = RTTICast<FloatControl>(e->GetOriginalSource());

        if (NULL != fCtrl)
        {
            fPoint fRadius;
 
            if (fCtrl->GetName().Equals(_U("PART_RadiusX")))
            {
                fRadius.x = fCtrl->GetFloat();
                fRadius.y = radialBr->GetRadiusY();
            }
            else
            {
                fRadius.x = radialBr->GetRadiusX();
                fRadius.y = fCtrl->GetFloat();
            }

            radialBr->SetRadiusXY(fRadius.x, fRadius.y);
        }
    }

    NotifyBrushChanged();
}

void RadialGradientBrushEditor::OnApplyTemplate()
{
    GradientBrushEditor::OnApplyTemplate();
    RadialGradientBrush* radialBr = GetRadialGradientBrush();

    if (NULL == radialBr)
    {
        CreateDefaultValue(_resValue);
        radialBr = GetRadialGradientBrush();
        NotifyBrushChanged();
    }

    if (NULL != radialBr)
    {
        GradientBrushEditor::UpdateGradienValue(radialBr);

        fPointEditor* elem = RTTICast<fPointEditor>(GetTemplateChild(_U("PART_Origin")));

        if (NULL != elem)
        {
            elem->SetfPoint(radialBr->GetOrigin());
            elem->AddPointChanged(new RoutedEventHandler(this, &RadialGradientBrushEditor::OnfPointChanged));
        }

        FloatControl* fCtrl = RTTICast<FloatControl>(GetTemplateChild(_U("PART_RadiusX")));
        if (NULL != fCtrl)
        {
            fCtrl->InitFloat(radialBr->GetRadiusX());
            fCtrl->AddBaseValueChanged(new RoutedEventHandler(this, &RadialGradientBrushEditor::OnfPointChanged));
        }

        fCtrl = RTTICast<FloatControl>(GetTemplateChild(_U("PART_RadiusY")));
        if (NULL != elem)
        {
            fCtrl->InitFloat(radialBr->GetRadiusY());
            fCtrl->AddBaseValueChanged(new RoutedEventHandler(this, &RadialGradientBrushEditor::OnfPointChanged));
        }
    }

    _needNotify = true;
}

//=====================================================
// StyleEditor
StyleEditor::StyleEditor()
{
}

StyleEditor::~StyleEditor()
{
}

void StyleEditor::OnApplyTemplate()
{
    ListBox* elem = suic::RTTICast<ListBox>(GetTemplateChild(_U("PART_TypeList")));
    if (NULL != elem)
    {
        elem->AddSelectionChanged(new SelectionChangedEventHandler(this, &StyleEditor::OnSelChanged));
        TypePool::GetTypeItems(elem->GetItemsSource());
    }

    EditorControl::OnApplyTemplate();
}

static int FindIndexFrom(ItemCollection* items, const String& name)
{
    int index = -1;
    for (int i = 0; i < items->GetCount(); ++i)
    {
        if (items->GetItem(i)->ToString().Equals(name))
        {
            index = i;
            break;
        }
    }
    return index;
}

void StyleEditor::OnUpdateValue()
{
    StyleNode* pVal = RTTICast<StyleNode>(GetResValue().get());
    ListBox* listBox = suic::RTTICast<ListBox>(GetTemplateChild(_U("PART_TypeList")));
    StringControl* elem = suic::RTTICast<StringControl>(GetTemplateChild(_U("PART_TextBox")));

    if (NULL != elem && pVal != NULL)
    {
        String strType = pVal->GetTargetType()->typeName;
        elem->SetText(strType);

        if (pVal->GetSetterCollection()->GetCount() > 0 || 
            pVal->GetTriggerCollection()->GetCount() > 0)
        {
            listBox->SetVisibility(Visibility::Collapsed);
        }
        else
        {
            listBox->SetVisibility(Visibility::Visible);
        }

        int index = FindIndexFrom(listBox->GetItemsSource(), strType);
        if (index != -1)
        {
            listBox->SetSelectedIndex(index);
            listBox->ScrollIntoView(listBox->GetItem(index));
        }
    }
}

void StyleEditor::CreateDefaultValue(ResNodePtr& obj)
{
    obj = new StyleNode();
}

bool StyleEditor::OnBaseValueChangedOverride(Element* sender)
{
    return true;
}

void StyleEditor::OnSelChanged(Element* sender, SelectionChangedEventArg* e)
{
    StringControl* elem = suic::RTTICast<StringControl>(GetTemplateChild(_U("PART_TextBox")));
    XArray* addItems = e->GetAddedItems();
    TypeItem* typeItem = NULL;
    
    if (addItems->GetCount() > 0)
    {
        StyleNode* pVal = RTTICast<StyleNode>(GetResValue().get());
        typeItem = dynamic_cast<TypeItem*>(addItems->GetItem(0));

        if (NULL != elem && NULL != typeItem && NULL != pVal)
        {
            pVal->SetTargetType(typeItem->GetType());
            elem->InitText(typeItem->GetType()->typeName);
        }
    }
    e->SetHandled(true);
}

//=====================================================
// LayoutEditor
LayoutEditor::LayoutEditor()
{
}

LayoutEditor::~LayoutEditor()
{
}

void LayoutEditor::CreateDefaultValue(ResNodePtr& obj)
{
    obj = new DesignElement();
}

void LayoutEditor::OnApplyTemplate()
{
}

bool LayoutEditor::OnBaseValueChangedOverride(Element* sender)
{
    return true;
}

//=====================================================
// TemplateEditor
TemplateEditor::TemplateEditor()
{
}

TemplateEditor::~TemplateEditor()
{
}

void TemplateEditor::CreateDefaultValue(ResNodePtr& obj)
{
    obj = new TemplateRootItem();
}

void TemplateEditor::OnApplyTemplate()
{
    ListBox* elem = suic::RTTICast<ListBox>(GetTemplateChild(_U("PART_TypeList")));
    if (NULL != elem)
    {
        elem->AddSelectionChanged(new SelectionChangedEventHandler(this, &TemplateEditor::OnSelChanged));
        TypePool::GetTypeItems(elem->GetItemsSource());
    }
    EditorControl::OnApplyTemplate();
}

void TemplateEditor::OnUpdateValue()
{
    TemplateRootItem* pVal = RTTICast<TemplateRootItem>(GetResValue().get());
    ListBox* listBox = suic::RTTICast<ListBox>(GetTemplateChild(_U("PART_TypeList")));
    StringControl* elem = suic::RTTICast<StringControl>(GetTemplateChild(_U("PART_TextBox")));

    if (NULL != elem && NULL != pVal)
    {
        suic::RTTIOfInfo* feFac = pVal->GetTargetType();

        if (NULL != feFac)
        {
            String strType = feFac->typeName;

            elem->SetText(strType);

            if (pVal->GetTriggerCollection()->GetCount() > 0 || 
                feFac != NULL)
            {
                listBox->SetVisibility(Visibility::Collapsed);
            }
            else
            {
                listBox->SetVisibility(Visibility::Visible);
            }

            int index = FindIndexFrom(listBox->GetItemsSource(), strType);
            if (index != -1)
            {
                listBox->SetSelectedIndex(index);
                listBox->ScrollIntoView(listBox->GetItem(index));
            }
        }
    }
}

bool TemplateEditor::OnBaseValueChangedOverride(Element* sender)
{
    return true;
}

void TemplateEditor::OnSelChanged(Element* sender, SelectionChangedEventArg* e)
{
    StringControl* elem = suic::RTTICast<StringControl>(GetTemplateChild(_U("PART_TextBox")));
    XArray* addItems = e->GetAddedItems();
    TypeItem* typeItem = NULL;

    if (addItems->GetCount() > 0)
    {
        TemplateRootItem* pVal = RTTICast<TemplateRootItem>(GetResValue().get());
        typeItem = dynamic_cast<TypeItem*>(addItems->GetItem(0));

        if (NULL != elem && NULL != typeItem && NULL != pVal)
        {
            pVal->SetTargetType(typeItem->GetType());
            elem->InitText(typeItem->GetType()->typeName);
        }
    }
    e->SetHandled(true);
}