
#ifndef _DCONTROL_H_
#define _DCONTROL_H_

#include <Core/TypeManager.h>
#include <Core/TypeControls.h>
#include <Main/XamlRootItem.h>

#include <Framework/Controls/RangeBase.h>

class SetterEditor;

class SetterEditorCmd : public Object
{
public:

    enum CmdType
    {
        ctReset,
        ctPropBinding,
        ctExpresion,
        ctStatic,
        ctDynamic,
        ctEditRes,
        ctDataBind,
        ctTemplateBind,
    };

    BeginMember(SetterEditorCmd, NotifyPropChanged)
        MemberGetBool(Enabled)
    EndMember()

    SetterEditorCmd(String name, CmdType cmd);
    ~SetterEditorCmd();

    void Execute(SetterEditor* target);

    void SetName(const String& name);
    String ToString();

    bool GetEnabled()
    {
        return _enabled;
    }

    void SetEnabled(bool v)
    {
        _enabled = v;
    }

protected:

    String GetExprestionName(const String& strVal, bool fromTemp);
    String GetAddName(const String& title, const String& name);

    void EditPropertyBinding(SetterEditor* setterEditor);

    void EditStaticResource(SetterEditor* setterEditor);
    void EditTemplateBinding(SetterEditor* setterEditor);
    void EditSelfExprestion(SetterEditor* setterEditor);

private:

    String _name;
    bool _enabled;
    CmdType _cmdType;
};

class EditRootPanel;

class SetterEditor : public Control
{
public:

    RTTIOfClass(SetterEditor)

    SetterEditor();
    virtual ~SetterEditor();

    void SetSetterNode(SetterNode* setter, ePropertyType fromTrigger);
    SetterNode* GetSetterNode(bool bCreate);

    void UpdateOperPanel(SetterNode* setter);
    void RefleshSetterToDesignUI();
    void RefleshSetterToDesignUI(bool bForce);

    void ResetDpItem();
    void Reset(DesignElement* resElem, DesignElement* target, bool bShow);

    virtual void SetTemplateMode(bool inTemplate);
    virtual void SetEditTarget(DesignElement* target);
    
    // 设置资源查询的目标元素
    // 对于模板来说，必须设置模板的模板父元素，系统会用其来查询资源
    void SetResourceTarget(DesignElement* target);
    void SetTargetType(RTTIOfInfo* rttiInfo);
    void ResetTarget();

    bool IsExtension() const;
    bool IsResetPending() const;

    EditRootPanel* GetEditPanel();

    DpItem* GetDpItem();
    RTTIOfInfo* GetTargetType();
    DesignElement* GetEditTarget() const;
    DesignElement* GetResourceTarget() const;

    DpProperty* GetProperty();
    PropMetadata* GetDefaultMeta();
    String GetPropName();

    void SetRealValue(Object* val, bool bCreate);

    virtual void SetDefaultValue(Object* val);

    virtual void ClearItems();

    virtual void ShowProp(bool val);
    virtual void ResetSetterEditor();
    virtual void ExpresionChanged();
    virtual void UpdateExpressValue(Object* val, bool bCreate);

    virtual void ResetSetter();
    virtual void InitSetterNode(RTTIOfInfo* owner, DpProperty* dp, SetterNode* pSetter);

    void OnEditValueChanged(DpObject* sender, RoutedEventArg* e);

    void OnUnlockExpresion(Element* sender, suic::RoutedEventArg* e);
    void OnSelectResource(Element* sender, suic::RoutedEventArg* e);

    void OnExpresionClick(Element* sender, suic::RoutedEventArg* e);

protected:

    void OnInitialized(EventArg* e);
    void EndRenderChildren(Drawing* drawing);

    void OnApplyTemplate();

    Size OnMeasure(const suic::Size& constraitSize);
    void OnArrange(const Size& arrangeSize);

    void InitializeBase();

    Button* CreateResButton(Style* expresionStyle);

    void HandleExpresion();
    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

    virtual void UpdateDefaultSetter(EditorControl* pText);

    virtual void NotifyUnlockExpresion();
    virtual void NotifySetterChanged();
    virtual void OnValueChanged(EditorControl* sender);

    virtual void OnSetterValueChanged();
    virtual void OnHandleExpresion(FrameworkElement* tempChild);

private:

    int _childCount;

    bool _inTemplate;
    bool _resetPending;

    DpItem* _dpItem;
    DesignElement* _resTarget;
    DesignElement* _editTarget;
    RTTIOfInfo* _targetType;
    SetterNode* _setter;
    StackPanel* _operPanel;

    Button* _resetResBtn;
    Button* _selectResBtn;
    static suic::ObservableCollection* _ctxColl;
};

class ButtonSetterEditor : public SetterEditor
{
public:

    static suic::DpProperty* ContentProperty;
    static void StaticInit();

    RTTIOfClass(ButtonSetterEditor)

    ButtonSetterEditor();

    void AddClick(suic::ClickEventHandler* handler);

protected:

    void OnApplyTemplate();
    void OnInitialized(suic::EventArg* e);

private:

    suic::Button* _button;
};

class StringSetterEditor : public SetterEditor
{
public:

    RTTIOfClass(StringSetterEditor)

protected:

    void OnValueChanged(EditorControl* sender);

    void OnApplyTemplate();
    void OnInitialized(EventArg* e);
};

class NumericSetterEditor : public SetterEditor
{
public:

    RTTIOfClass(NumericSetterEditor)

protected:

    void OnValueChanged(EditorControl* sender);
};

class PosNumericSetterEditor : public SetterEditor
{
public:

    RTTIOfClass(PosNumericSetterEditor)

protected:

    void OnValueChanged(EditorControl* sender);
};

class FloatSetterEditor : public SetterEditor
{
public:

    RTTIOfClass(FloatSetterEditor)

protected:

    void OnValueChanged(EditorControl* sender);
};

class PosFloatSetterEditor : public SetterEditor
{
public:

    RTTIOfClass(PosFloatSetterEditor)

protected:

    void OnValueChanged(EditorControl* sender);
};

class RatioSetterEditor : public SetterEditor
{
public:

    RTTIOfClass(RatioSetterEditor)
};

class TextBoxRangeSetterEditor : public SetterEditor
{
public:

    TextBoxRangeSetterEditor();
    ~TextBoxRangeSetterEditor();

    RTTIOfClass(TextBoxRangeSetterEditor)

protected:

    void OnValueChanged(EditorControl* sender);
};


class WidthSetterEditor : public SetterEditor
{
public:

    RTTIOfClass(WidthSetterEditor)

    void OnValueChanged(EditorControl* sender);

protected:

    void UpdateDefaultSetter(EditorControl* pText);
};

class SizeSetterEditor : public SetterEditor
{
public:

    SizeSetterEditor();

    RTTIOfClass(SizeSetterEditor)

protected:

    void OnValueChanged(EditorControl* sender);
};

class RectSetterEditor : public SetterEditor
{
public:

    RectSetterEditor();

    RTTIOfClass(RectSetterEditor)

protected:

    void OnValueChanged(EditorControl* sender);
};

class fRectSetterEditor : public SetterEditor
{
public:

    fRectSetterEditor();

    RTTIOfClass(fRectSetterEditor)

protected:

    void OnValueChanged(EditorControl* sender);
};

class AlignVertSetterEditor : public SetterEditor
{
public:

    static DpProperty* GroupNameProperty;

    AlignVertSetterEditor();

    RTTIOfClass(AlignVertSetterEditor)

    void NotifySetterChanged();

    void OnCheckedChanged(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    void OnInitialized(suic::EventArg* e);
    void OnApplyTemplate();

private:

    bool _isCallOnTemplate;
    bool _updatingCheckValue;
};

class AlignHoriSetterEditor : public SetterEditor
{
public:

    static DpProperty* GroupNameProperty;
    static void StaticInit();

    AlignHoriSetterEditor();

    RTTIOfClass(AlignHoriSetterEditor)

    void NotifySetterChanged();

    void OnCheckedChanged(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    void OnInitialized(suic::EventArg* e);
    void OnApplyTemplate();

private:

    bool _isCallOnTemplate;
    bool _updatingCheckValue;
};

class GridViewSetterEditor : public SetterEditor
{
public:

    GridViewSetterEditor();

    RTTIOfClass(GridViewSetterEditor)

    void NotifySetterChanged();

protected:

    void OnInitialized(suic::EventArg* e);
    void OnApplyTemplate();
};

class ExtensionSetterEditor : public SetterEditor
{
public:

    ExtensionSetterEditor();

    RTTIOfClass(ExtensionSetterEditor)

    void NotifySetterChanged();

protected:

    void OnInitialized(suic::EventArg* e);
    void OnApplyTemplate();

    void OnSetterValueChanged();
    void OnHandleExpresion(FrameworkElement* tempChild);

private:

    suic::TextBox* _extensionBox;
};

class ChoiceSetterEditor : public SetterEditor
                         , public IAddChild
{
public:

    static DpProperty* SelectIndexProperty;
    static void StaticInit();

    ChoiceSetterEditor();
    ~ChoiceSetterEditor();

    RTTIOfClass(ChoiceSetterEditor)

    void NotifySetterChanged();
    void OnSelItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e);

    virtual void ClearItems();

    virtual void AddChild(Object* obj);
    virtual void AddText(String val);
    virtual void RemoveChild(Object* child);

    void ResetSetterEditor();

protected:

    void OnApplyTemplate();
    void OnInitialized(suic::EventArg* e);
    int FindIndexByText(const String& text);

    int InitComboBoxItems();
    void UpdateChoiceItem();
    void AddChoiceItem(Object* item);

protected:

    ObservableCollection* _items;
    ComboBox* _comboBox;
    bool _updatingCheckValue;
};

class CursorSetterEditor : public ChoiceSetterEditor
{
public:

    CursorSetterEditor();
    ~CursorSetterEditor();

    RTTIOfClass(CursorSetterEditor)

    void OnApplyTemplate();

    void NotifySetterChanged();
    void ClearItems();
    void OnSelItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e);
};

class CheckSetterEditor : public SetterEditor
{
public:

    static DpProperty* ContentProperty;
    static void StaticInit();

    CheckSetterEditor();

    RTTIOfClass(CheckSetterEditor)

    void NotifySetterChanged();

    void OnChecked(DpObject* sender, RoutedEventArg* e);
    void OnUnchecked(DpObject* sender, RoutedEventArg* e);

    void SetDefaultValue(Object* val);

protected:

    void OnApplyTemplate();

    virtual void OnCheckedOverride();
    virtual void OnUncheckedOverride();

protected:

    bool _updatingCheckValue;
    CheckBox* _checkBox;
};

class FontWeightSetterEditor : public CheckSetterEditor
{
public:

    FontWeightSetterEditor();

    RTTIOfClass(FontWeightSetterEditor)

protected:

    void NotifySetterChanged();

    void OnCheckedOverride();
    void OnUncheckedOverride();
};

class FontStyleSetterEditor : public CheckSetterEditor
{
public:

    FontStyleSetterEditor();

    RTTIOfClass(FontStyleSetterEditor)

protected:

    void NotifySetterChanged();

    void OnCheckedOverride();
    void OnUncheckedOverride();
};

class FontUnderlineSetterEditor : public CheckSetterEditor
{
public:

    FontUnderlineSetterEditor();

    RTTIOfClass(FontUnderlineSetterEditor)

protected:

    void NotifySetterChanged();

    void OnChecked(Element* sender, RoutedEventArg* e);
    void OnUnchecked(Element* sender, RoutedEventArg* e);
};

class FontSizeSetterEditor : public ChoiceSetterEditor
{
public:

    FontSizeSetterEditor();

    RTTIOfClass(FontSizeSetterEditor)

protected:

    void OnApplyTemplate();
    void ClearItems();
    void NotifySetterChanged();
};

class BrushEditorPanel;
class BrushSetterEditor;

class BrushElement : public suic::RadioButton
{
public:

    BrushElement();

    RTTIOfClass(BrushElement)

    void OnClick();
};

class BrushSetterEditor : public SetterEditor
{
public:

    static DpProperty* ContentProperty;

    static void StaticInit();

    BrushSetterEditor();

    RTTIOfClass(BrushSetterEditor)

    void NotifySetterChanged();
    void NotifyBrushChanged();

    void SetChecked();
    void SwitchToEdit(bool bForce);

    void ResetSetter();

protected:
    
    void InitBrushEditor();
    void NotifyUnlockExpresion();

    void UpdateToNullDisplay();
    void UpdateExpressValue(Object* val, bool bCreate);

protected:

    // 标记是否有操作正在进行
    bool _isPending;
    BrushEditorPanel* _brushEditor;
};

class BrushEditorPanel : public suic::StackPanel
{
public:

    BrushEditorPanel();

    RTTIOfClass(BrushEditorPanel)

    bool HasSetBrush() const;
    BrushSetterEditor* GetBrushSetterEditor() const;

    void SwitchToBrushEditor(BrushEditor* brushEditor);
    void SwitchToBrushSetterEditor(BrushSetterEditor* brushEditor, bool bForce);

    void SwitchToNullBrushEditor(BrushSetterEditor* brushEditor);
    void SwitchToExpressionBrushEditor(BrushSetterEditor* brushEditor);

    void EnableExtensionFlag();
    void ResetBrushEditorPanel();

    void OnBrushChanged(DpObject* sender, RoutedEventArg* e);
    void OnRadioChecked(DpObject* sender, RoutedEventArg* e);

protected:

    void OnInitialized(EventArg* e);
    void OnApplyTemplate();

    void SetCheckBrushEditor(BrushEditor* brushEditor);
    void UpdateSetterToDesignUI(ResNode* resNode);

private:

    bool _isBrushReset;
    bool _isRefleshing;

    BrushEditor* _nullBrush;
    BrushEditor* _solidBrush;
    BrushEditor* _linearBrush;
    BrushEditor* _radialBrush;
    BrushEditor* _imageBrush;

    // 当前正在编辑的属性
    BrushSetterEditor* _brushEditor;
};

class ImageBrushSetterEditor : public ImageBrushEditor
{
public:

    RTTIOfClass(ImageBrushSetterEditor)

    void OnIsVisibleChanged(IsVisibleChangedEventArg* e);
};

class NullBrushSetterEditor : public NullBrushEditor
{
public:

    RTTIOfClass(NullBrushSetterEditor)

    void OnIsVisibleChanged(IsVisibleChangedEventArg* e);
};

class SolidColorBrushSetterEditor : public SolidColorBrushEditor
{
public:

    RTTIOfClass(SolidColorBrushSetterEditor)

    void OnIsVisibleChanged(IsVisibleChangedEventArg* e);
};

class LinearGradientBrushSetterEditor : public LinearGradientBrushEditor
{
public:

    RTTIOfClass(LinearGradientBrushSetterEditor)

    void OnIsVisibleChanged(IsVisibleChangedEventArg* e);
};

class RadialGradientBrushSetterEditor : public RadialGradientBrushEditor
{
public:

    RTTIOfClass(RadialGradientBrushSetterEditor)

    void OnIsVisibleChanged(IsVisibleChangedEventArg* e);
};

class BrushResSetterEditor : public SetterEditor
{
public:

    BrushResSetterEditor();

    RTTIOfClass(BrushResSetterEditor)

    void NotifySetterChanged();
    void UpdateBrushElement(Brush* brush);
    
    void ResetBrushEditor();

    void SetTargetElement(DesignElement* target);

protected:

    void OnInitialized(suic::EventArg* e);
    void OnApplyTemplate();
    int GetVisualChildrenCount();

    void OnHandleExpresion(FrameworkElement* tempChild);
    void EndRenderChildren(Drawing* drawing);
};

class STSetterEditor : public SetterEditor
{
public:

    STSetterEditor();

    RTTIOfAbstractClass(STSetterEditor)

    void NotifySetterChanged();
    void OnEditStyleTemplateClick(suic::Element* sender, suic::RoutedEventArg* e);

    void OnMainReturnEventClick(suic::Object* sender, suic::EventArg* e);

protected:

    void OnApplyTemplate();
    void OnSetterValueChanged();
    void UpdateShow();
    void UpdateDesignPanel();

    void GetMainReturnEvent(suic::EventHandler& h);

    suic::Button* GetEditButton();

    virtual bool IsSetterEmpty(SetterNode* pSetter);
    virtual void OnEditClick();
    virtual void OnMainReturnClick(suic::Object* sender);

protected:

    suic::TextBlock* _info;
    suic::Button* _editBtn;
};

class TemplateSetterEditor : public STSetterEditor
{
public:

    TemplateSetterEditor();

    RTTIOfAbstractClass(TemplateSetterEditor)

    void NotifySetterChanged();
    void ResetSetter();
    void InitSetterNode(RTTIOfInfo* owner, DpProperty* dp, SetterNode* pSetter);

protected:

    void OnEditClick();
    void OnMainReturnClick(suic::Object* sender);

    virtual bool IsSetterEmpty(SetterNode* pSetter);
    virtual TemplateRootItem* CreateFrameworkTemplate(RTTIOfInfo* ownerRtti);
};

class ControlTemplateSetterEditor : public TemplateSetterEditor
{
public:

    ControlTemplateSetterEditor();

    RTTIOfClass(ControlTemplateSetterEditor)

    void NotifySetterChanged();

protected:
    
    TemplateRootItem* CreateFrameworkTemplate(RTTIOfInfo* ownerRtti);
};

class DataTemplateSetterEditor : public TemplateSetterEditor
{
public:

    DataTemplateSetterEditor();

    RTTIOfClass(DataTemplateSetterEditor)

    void NotifySetterChanged();

protected:

    TemplateRootItem* CreateFrameworkTemplate(RTTIOfInfo* ownerRtti);
};

//
// <Summary>
//   编辑FrameworkElement控件的Style属性
// </Summary>
//
class StyleSetterEditor : public STSetterEditor
{
public:

    StyleSetterEditor();

    RTTIOfClass(StyleSetterEditor)

    void ResetSetter();
    void NotifySetterChanged();
    void InitSetterNode(RTTIOfInfo* owner, DpProperty* dp, SetterNode* pSetter);

protected:

    void OnEditClick();
    void OnMainReturnClick(suic::Object* sender);

    StyleNode* CreateNewStyle(suic::RTTIOfInfo* targetRtti);

    virtual bool IsSetterEmpty(SetterNode* pSetter);
    // 模版里的控件元素不允许编辑Style属性
    virtual void SetTemplateMode(bool inTemplate);
    virtual suic::RTTIOfInfo* GetStyleTargetRTTIInfo();

private:

    suic::TextBlock* _info;
};

class ItemContainerStyleSetterEditor : public StyleSetterEditor
{
public:

    ItemContainerStyleSetterEditor();

    RTTIOfClass(ItemContainerStyleSetterEditor)

protected:

    virtual suic::RTTIOfInfo* GetStyleTargetRTTIInfo();
};

//
// <Summary>
//   编辑Image控件的Source属性
// </Summary>
//
class SourceSetterEditor : public SetterEditor
{
public:

    SourceSetterEditor();

    RTTIOfClass(SourceSetterEditor)

    void NotifySetterChanged();

    void OnSourceClick(suic::Element* sender, suic::RoutedEventArg* e);

protected:

    void UpateSource();
    void OnApplyTemplate();
    void OnEditClick();
    void OnSetterValueChanged();

private:

    suic::TextBox* _info;
};

#endif
