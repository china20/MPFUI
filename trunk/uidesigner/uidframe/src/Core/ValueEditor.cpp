
#include "stdafx.h"

#include "ValueEditor.h"

#include <Editor/CreateStyleWindow.h>
#include <Editor/EditRootPanel.h>
#include <Editor/ExpressionWindow.h>
#include <Editor/BindingEditorWindow.h>
#include <Editor/EditStaticResource.h>
#include <Editor/EditTemplateBinding.h>

#include <Main/AddNameWindow.h>

#include <Main/MainWindow.h>
#include <Main/ImageSelectorWindow.h>
#include <Editor/SystemResource.h>
#include <Editor/ThemeEditorWindow.h>

#include <System/Tools/ResourceParser.h>
#include <System/Resources/DynamicResource.h>
#include <System/Resources/StaticResource.h>

#include <Framework/Controls/DockPanel.h>

ImplementRTTIOfClass(SetterEditor, suic::Control)

ImplementRTTIOfClass(StringSetterEditor, SetterEditor)
ImplementRTTIOfClass(WidthSetterEditor, SetterEditor)
ImplementRTTIOfClass(RectSetterEditor, SetterEditor)
ImplementRTTIOfClass(fRectSetterEditor, SetterEditor)

ImplementRTTIOfClass(SizeSetterEditor, SetterEditor)

ImplementRTTIOfClass(ChoiceSetterEditor, SetterEditor)
ImplementRTTIOfClass(CursorSetterEditor, ChoiceSetterEditor)
ImplementRTTIOfClass(CheckSetterEditor, SetterEditor)

ImplementRTTIOfClass(FontWeightSetterEditor, CheckSetterEditor)
ImplementRTTIOfClass(FontStyleSetterEditor, CheckSetterEditor)
ImplementRTTIOfClass(FontUnderlineSetterEditor, CheckSetterEditor)

ImplementRTTIOfClass(FontSizeSetterEditor, ChoiceSetterEditor)

ImplementRTTIOfClass(FloatSetterEditor, SetterEditor)
ImplementRTTIOfClass(PosFloatSetterEditor, SetterEditor)
ImplementRTTIOfClass(RatioSetterEditor, SetterEditor)
ImplementRTTIOfClass(NumericSetterEditor, SetterEditor)
ImplementRTTIOfClass(PosNumericSetterEditor, SetterEditor)

ImplementRTTIOfClass(GridViewSetterEditor, SetterEditor)
ImplementRTTIOfClass(ExtensionSetterEditor, SetterEditor)

ImplementRTTIOfClass(AlignVertSetterEditor, SetterEditor)
ImplementRTTIOfClass(AlignHoriSetterEditor, SetterEditor)

ImplementRTTIOfClass(ButtonSetterEditor, SetterEditor)

ImplementRTTIOfClass(SolidColorBrushEditor, BrushEditor)
ImplementRTTIOfClass(ImageBrushEditor, BrushEditor)
ImplementRTTIOfClass(GradientBrushEditor, BrushEditor)
ImplementRTTIOfClass(LinearGradientBrushEditor, GradientBrushEditor)
ImplementRTTIOfClass(RadialGradientBrushEditor, GradientBrushEditor)

ImplementRTTIOfClass(BrushElement, suic::RadioButton)
ImplementRTTIOfClass(BrushSetterEditor, SetterEditor)
ImplementRTTIOfClass(BrushResSetterEditor, SetterEditor)
ImplementRTTIOfClass(BrushEditorPanel, suic::StackPanel)

ImplementRTTIOfClass(NullBrushSetterEditor, NullBrushEditor)
ImplementRTTIOfClass(SolidColorBrushSetterEditor, SolidColorBrushEditor)
ImplementRTTIOfClass(ImageBrushSetterEditor, ImageBrushEditor)
ImplementRTTIOfClass(LinearGradientBrushSetterEditor, LinearGradientBrushEditor)
ImplementRTTIOfClass(RadialGradientBrushSetterEditor, RadialGradientBrushEditor)

ImplementRTTIOfClass(STSetterEditor, SetterEditor)
ImplementRTTIOfClass(TemplateSetterEditor, STSetterEditor)
ImplementRTTIOfClass(DataTemplateSetterEditor, TemplateSetterEditor)
ImplementRTTIOfClass(ControlTemplateSetterEditor, TemplateSetterEditor)
ImplementRTTIOfClass(StyleSetterEditor, STSetterEditor)
ImplementRTTIOfClass(ItemContainerStyleSetterEditor, StyleSetterEditor)

ImplementRTTIOfClass(SourceSetterEditor, SetterEditor)
ImplementRTTIOfClass(TextBoxRangeSetterEditor, SetterEditor)

static EditRootPanel* FindEditRttoPanel(suic::FrameworkElement* elem)
{
    EditRootPanel* editPanel = NULL;
    suic::FrameworkElement* parent = CASTTOFE(elem->GetUIParent());

    while (parent)
    {
        editPanel = suic::DynamicCast<EditRootPanel>(parent);
        if (NULL != editPanel/* && editPanel->IsVisible()*/)
        {
            break;
        }
        parent = CASTTOFE(parent->GetUIParent());
    }

    return editPanel;
}

static SetterNode* CreateSetterNode(suic::FrameworkElement* elem, const suic::String& prop)
{
    EditRootPanel* editPanel = FindEditRttoPanel(elem);
    if (editPanel != NULL)
    {
        return editPanel->CreateSetterNode(prop);
    }
    else
    {
        return NULL;
    }
}

void SetterEditor::RefleshSetterToDesignUI()
{
    RefleshSetterToDesignUI(false);
}

void SetterEditor::RefleshSetterToDesignUI(bool bForce)
{
    EditRootPanel* editPanel = FindEditRttoPanel(this);
    SetterEditor* elemBase = this;

    if (NULL != editPanel && NULL != elemBase && !elemBase->IsResetPending())
    {
        SetterNode* pNode = elemBase->GetSetterNode(false);

        editPanel->UpdateSetterToUI(pNode);
        if (bForce && NULL == pNode)
        {
            editPanel->SetModified(true);
        }
    }
}

//===============================================

suic::ObservableCollection* SetterEditor::_ctxColl;

SetterEditorCmd::SetterEditorCmd(String name, CmdType cmd)
{
    _cmdType = cmd;
    _name = name;
    _enabled = true;
}

SetterEditorCmd::~SetterEditorCmd()
{

}

String SetterEditorCmd::GetExprestionName(const String& strVal, bool fromTemp)
{
    ExpressionWindow exprWnd(fromTemp);
    exprWnd.setAutoDelete(false);
    exprWnd.SetPreviousValue(strVal);

    suic::Builder::BuildUri(&exprWnd, "/mpfuid;/resource/uidesign/layout/editor/ExpressionWindow.xaml");
    
    if (0 == exprWnd.ShowDialog())
    {
        return exprWnd.GetValue();
    }
    else
    {
        return String();
    }
}

String SetterEditorCmd::GetAddName(const String& title, const String& name)
{
    AddNameWindow addWnd(title, name, 380);
    addWnd.setAutoDelete(false);

    suic::Builder::BuildUri(&addWnd, "/mpfuid;/resource/uidesign/layout/AddNameWindow.xaml");
    addWnd.ShowDialog();

    return addWnd.GetName();
}

void SetterEditorCmd::EditStaticResource(SetterEditor* setterEditor)
{
}

void SetterEditorCmd::EditTemplateBinding(SetterEditor* setterEditor)
{
}

void SetterEditorCmd::EditSelfExprestion(SetterEditor* setterEditor)
{
    bool bFromTemp = false;
    String strOldVal;
    SetterNode* setterNode = setterEditor->GetSetterNode(false);
    if (NULL != setterNode)
    {
        ResNode* resNode = setterNode->GetResNode();
        if (NULL != resNode)
        {
            strOldVal = resNode->GetSingleXml();
            strOldVal.Trim();
            if (strOldVal.Length() <= 2 || strOldVal[0] != _U('{'))
            {
                strOldVal = _U("");
            }
        }
    }

    if (NULL != setterEditor->GetEditTarget())
    {
        bFromTemp = setterEditor->GetEditTarget()->IsTemplate();
    }

    String strName = GetExprestionName(strOldVal, bFromTemp);
    if (!strName.Empty())
    {
        RTTIOfInfo* tempInfo = NULL;
        EditRootPanel* editPanel = setterEditor->GetEditPanel();
        TemplateRootItem* resElem = RTTICast<TemplateRootItem>(editPanel->GetRootItem());

        if (NULL != resElem && resElem->GetTargetType() != NULL)
        {
            tempInfo = resElem->GetTargetType();
        }

        suic::ResourceItem resItem;
        suic::ResourceParserOp::ParseExtension(tempInfo, NULL, strName, resItem);
        if (resItem.res.get() != suic::DpProperty::UnsetValue())
        {
            setterEditor->UpdateExpressValue(resItem.res.get(), true);
        }
        else
        {
            InfoBox::Show(_U("输入表达式错误！"), _U("错误"));
        }
    }
}

void SetterEditorCmd::EditPropertyBinding(SetterEditor* setterEditor)
{
    String strName = setterEditor->GetPropName();
    DpProperty* dp = setterEditor->GetProperty();
    EditRootPanel* editPanel = setterEditor->GetEditPanel();
    Project* pPrj = editPanel->GetProject();
    DesignElement* targetElem = setterEditor->GetEditTarget();

    SetterNode* pSetter = setterEditor->GetSetterNode(false);
    ResNode* resNode = NULL;

    ResourceDictionaryNode* resDicNode = NULL;

    if (NULL != pSetter)
    {
        resNode = pSetter->GetResNode();
        dp = pSetter->GetProperty();
    }

    if (resDicNode == NULL)
    {
        resDicNode = editPanel->GetResourceDicNode();
    }

    if (resDicNode == NULL && NULL != pPrj)
    {
        resDicNode = pPrj->GetApplicationRootItem()->GetApplicationNode()->GetResourceDictionary();
    }

    BindingEditorWindow bindingWnd(targetElem, dp, resDicNode, resNode);
    bindingWnd.setAutoDelete(false);

    bindingWnd.SetResourceElement(setterEditor->GetResourceTarget());
    bindingWnd.SetTemplateParent(editPanel->GetTemplateRoot());

    suic::Builder::BuildUri(&bindingWnd, "/mpfuid;/resource/uidesign/layout/Editor/BindingEditorWindow.xaml");
    bindingWnd.ShowDialog();

    ResNode* newResNode = bindingWnd.GetResNode();
    if (newResNode != resNode)
    {
        setterEditor->UpdateExpressValue(newResNode, true);
    }
}

void SetterEditorCmd::Execute(SetterEditor* setterEditor)
{
    switch (_cmdType)
    {
    case CmdType::ctReset:
        setterEditor->ResetSetter();
        break;

    case CmdType::ctPropBinding:
        EditPropertyBinding(setterEditor);
        break;

    case CmdType::ctExpresion:
        EditSelfExprestion(setterEditor);
        break;

    case CmdType::ctStatic:
        EditStaticResource(setterEditor);
        break;

    case CmdType::ctDynamic:
        {
            String strName = GetAddName(_U("动态资源"), _U("名称："));
            if (!strName.Empty())
            {
                DRExtension* drExt = new DRExtension(strName);
                setterEditor->UpdateExpressValue(drExt, true);
            }
        }
        break;

    case CmdType::ctEditRes:
        break;

    case CmdType::ctDataBind:
        break;

    case CmdType::ctTemplateBind:
        EditTemplateBinding(setterEditor);
        break;
    }
}

String SetterEditorCmd::ToString()
{
    return _name;
}

void SetterEditorCmd::SetName(const String& name)
{
    _name = name;
}

SetterEditor::SetterEditor()
{
    _targetType = NULL;
    _editTarget = NULL;
    _resTarget = NULL;
    _childCount = 0;
    _setter = NULL;
    _resetPending = false;
    _operPanel = NULL;
    _selectResBtn = NULL;
    _resetResBtn = NULL;
    _dpItem = NULL;
    _inTemplate = false;
}

SetterEditor::~SetterEditor()
{
    FREEREFOBJ(_operPanel);
}

void SetterEditor::UpdateOperPanel(SetterNode* setter)
{
    if (NULL == _operPanel)
    {
        return;
    }
}

void SetterEditor::SetSetterNode(SetterNode* setter, ePropertyType fromTrigger)
{
    SETREFOBJ(_setter, setter);
    NotifySetterChanged();
    HandleExpresion();
    UpdateOperPanel(setter);

    if (setter != NULL)
    {
        setter->SetFromTriggerProp(fromTrigger);
    }
}

EditRootPanel* SetterEditor::GetEditPanel()
{
    return FindEditRttoPanel(this);
}

String SetterEditor::GetPropName()
{
    String strName = GetName();
    int iPos = strName.IndexOf(_U("."));
    if (-1 != iPos)
    {
        strName = strName.Substring(iPos + 1);
    }
    return strName;
}

RTTIOfInfo* SetterEditor::GetTargetType()
{
    return _targetType;
}

DesignElement* SetterEditor::GetEditTarget() const
{
    return _editTarget;
}

DesignElement* SetterEditor::GetResourceTarget() const
{
    if (NULL == _resTarget && _editTarget != NULL)
    {
        if (!_editTarget->IsTemplate())
        {
            return _editTarget;
        }
    }
    return _resTarget;
}

void SetterEditor::ResetDpItem()
{
    _dpItem = NULL;
}

DpItem* SetterEditor::GetDpItem()
{
    if (NULL == _dpItem)
    {
        String strName;

        if (_setter != NULL)
        {
            strName = _setter->GetName();
            _dpItem = DpManager::Ins()->FindDpItem(strName);
        }

        if (NULL == _dpItem)
        {
            strName = GetPropName();
            _dpItem = DpManager::Ins()->FindDpItem(strName);
        }
    }

    return _dpItem;
}

DpProperty* SetterEditor::GetProperty()
{
    DpItem* dpItem = GetDpItem();
    if (NULL != dpItem)
    {
        return dpItem->GetDp(GetTargetType(), true);
    }
    else
    {
        return NULL;
    }
}

PropMetadata* SetterEditor::GetDefaultMeta()
{
    DpItem* dpItem = GetDpItem();
    if (NULL != dpItem && NULL != GetTargetType())
    {
        return dpItem->GetMetatata(GetTargetType());
    }
    else
    {
        return NULL;
    }
}

void SetterEditor::OnSetterValueChanged()
{

}

void SetterEditor::UpdateExpressValue(Object* val, bool bCreate)
{
    SetRealValue(val, bCreate);
    HandleExpresion();
    
    OnSetterValueChanged();
}

void SetterEditor::SetRealValue(Object* val, bool bCreate)
{
    SetterNode* pSetterNode = GetSetterNode(bCreate);

    if (val != NULL)
    {
        val->ref();
    }

    if (pSetterNode != NULL)
    {
        ResNode* resVal = RTTICast<ResNode>(val);

        if (NULL == resVal)
        {
            pSetterNode->SetValue(val);
        }
        else
        {
            pSetterNode->SetResNode(resVal);
        }

        RefleshSetterToDesignUI();

        SetterCollectionNode* pSetterColl = NULL;
        
        if (NULL == val)
        {
            pSetterColl = suic::RTTICast<SetterCollectionNode>(pSetterNode->GetParent());
            if (NULL != pSetterColl)
            {
                pSetterColl->RemoveItem(pSetterNode);
            }
        }
    }

    if (val != NULL)
    {
        val->unref();
    }
}

void SetterEditor::HandleExpresion()
{
    FrameworkElement* tempChild = GetTemplateChild();
    OnHandleExpresion(tempChild);
}

void SetterEditor::OnHandleExpresion(FrameworkElement* tempChild)
{
    if (IsExtension())
    {
        String strTip = _setter->GetResNode()->GetSingleXml();
        if (NULL != tempChild)
        {
            tempChild->Enable(false);
        }
        SetToolTip(strTip);
    }
    else
    {
        SetToolTip(NULL);
        if (NULL != tempChild)
        {
            tempChild->Enable(true);
        }
    }
}

void SetterEditor::EndRenderChildren(Drawing* drawing)
{
}

Button* SetterEditor::CreateResButton(Style* expresionStyle)
{
    Button* pBtn = new Button();

    _operPanel->AddChild(pBtn);
    pBtn->SetStyle(expresionStyle);

    return pBtn;
}

void SetterEditor::OnApplyTemplate()
{
    Control::OnApplyTemplate();

    SetClipToBounds(true);
    
    FrameworkElement* fe = GetTemplateChild();
    if (NULL != fe)
    {
        Rect rcMgr = fe->GetMargin();
        rcMgr.right += 16;
        fe->SetMargin(rcMgr);
    }

    HandleExpresion();
}

Size SetterEditor::OnMeasure(const suic::Size& constraitSize)
{
    Size size = Control::OnMeasure(constraitSize);
    if (NULL != _operPanel)
    {
        _operPanel->Measure(constraitSize);
    }
    return size;
}

void SetterEditor::OnArrange(const Size& arrangeSize)
{
    Control::OnArrange(arrangeSize);
    FrameworkElement* fe = GetTemplateChild();

    if (NULL != fe && NULL != _operPanel)
    {
        int hei = arrangeSize.Height();
        int x = fe->GetVisualOffset().x + fe->GetRenderSize().Width() + 3;
        Rect rect(Point(x, 0), Size(_operPanel->GetDesiredSize().Width(), hei));
        _operPanel->Arrange(rect);
    }
}

SetterNode* SetterEditor::GetSetterNode(bool bCreate)
{
    if (NULL == _setter && bCreate)
    {
        _setter = CreateSetterNode(this, GetName());
        if (NULL != _setter)
        {
            _setter->ref();
        }
    }

    UpdateOperPanel(_setter);

    return _setter;
}

void SetterEditor::SetTemplateMode(bool inTemplate)
{
    _inTemplate = inTemplate;
}

void SetterEditor::SetEditTarget(DesignElement* target)
{
    _editTarget = target;
}

void SetterEditor::ResetTarget()
{
    _resTarget = NULL;
    _targetType = NULL;
    SetEditTarget(NULL);
}

void SetterEditor::SetResourceTarget(DesignElement* target)
{
    _resTarget = target;
}

void SetterEditor::SetTargetType(RTTIOfInfo* rttiInfo)
{
    _targetType = rttiInfo;
}

void SetterEditor::Reset(DesignElement* resElem, DesignElement* target, bool bShow)
{
    _editTarget = target;
    _resTarget = resElem;

    if (NULL != _editTarget)
    {
        if (_editTarget->GetUIElement() != NULL)
        {
            SetTargetType(_editTarget->GetUIElement()->GetRTTIType());
        }
    }
    else if (!bShow)
    {
        SetTargetType(NULL);
    }

    if (bShow)
    {
        if (_setter != NULL)
        {
            _setter->unref();
            _setter = NULL;
        }

        SetToolTip(_U(""));

        _resetPending = true;
        NotifySetterChanged();
        HandleExpresion();
        _resetPending = false;
    }

    ShowProp(bShow);
}

void SetterEditor::SetDefaultValue(Object* val)
{

}

void SetterEditor::ClearItems()
{

}

void SetterEditor::ShowProp(bool val)
{
    if (GetParent() != NULL)
    {
        if (val)
        {
            GetParent()->SetVisibility(suic::Visibility::Visible);
        }
        else
        {
            GetParent()->SetVisibility(suic::Visibility::Collapsed);
        }
    }
}

void SetterEditor::ResetSetterEditor()
{
    ResetDpItem();
}

bool SetterEditor::IsExtension() const
{
    return (_setter && _setter->IsExtension());
}

bool SetterEditor::IsResetPending() const
{
    return _resetPending;
}

void SetterEditor::NotifyUnlockExpresion()
{
    NotifySetterChanged();
}

void SetterEditor::NotifySetterChanged()
{
    SetterNode* pSetter = GetSetterNode(false);
    EditorControl* pText = RTTICast<EditorControl>(GetTemplateChild(_U("PART_Editor")));

    if (NULL == pText)
    {
        ApplyTemplate();
    }

    if (NULL != pText)
    {
        if (NULL != pSetter)
        {
            pText->UpdateResValue(pSetter->GetResNode());
        }
        else
        {
            UpdateDefaultSetter(pText);
        }
    }
}

void SetterEditor::UpdateDefaultSetter(EditorControl* pText)
{
    // 创建默认值
    ResNodePtr resNode;
    pText->CreateDefaultValue(resNode);
    pText->UpdateResValue(resNode);
}

void SetterEditor::InitializeBase()
{
    ;
}

void SetterEditor::OnUnlockExpresion(Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    SetterNode* pSetter = GetSetterNode(false);
    if (pSetter != NULL && IsExtension())
    {
        pSetter->SetValue(NULL);
        NotifyUnlockExpresion();
        UpdateLayout();
    }
}

void SetterEditor::OnExpresionClick(Element* sender, suic::RoutedEventArg* e)
{
    Element* elem = DynamicCast<Element>(e->GetOriginalSource());
    SetterEditorCmd* pCmd = DynamicCast<SetterEditorCmd>(elem->GetDataContext());
    if (NULL != pCmd)
    {
        pCmd->Execute(this);
    }
}

void SetterEditor::OnSelectResource(Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    SetterNode* pSetter = GetSetterNode(false);

    if (NULL == _ctxColl)
    {
        _ctxColl = new suic::ObservableCollection();
        _ctxColl->ref();
        suic::MenuItem* propTxt = new suic::MenuItem();
        suic::Style* style = RTTICast<suic::Style>(FindRes(_U("TopMenuItem")));

        _ctxColl->AddItem(propTxt);
        propTxt->SetStyle(style);

        _ctxColl->AddItem(new SetterEditorCmd(_U("重置"), SetterEditorCmd::ctReset));
        _ctxColl->AddItem(new SetterEditorCmd(_U("自定义表达式"), SetterEditorCmd::ctExpresion));
        _ctxColl->AddItem(new Separator());
        _ctxColl->AddItem(new SetterEditorCmd(_U("属性绑定"), SetterEditorCmd::ctPropBinding));
    }

    if (NULL != _ctxColl)
    {
        suic::ContextMenu* ctxMenu = new suic::ContextMenu();
        ctxMenu->ref();

        suic::MenuItem* propTxt = dynamic_cast<suic::MenuItem*>(_ctxColl->GetItem(0));
        SetterEditorCmd* setterCmd = NULL;

        if (_setter != NULL)
        {
            propTxt->SetHeader(new OString(_setter->GetName()));
        }
        else
        {
            propTxt->SetHeader(new OString(GetName()));
        }

        ctxMenu->SetItemsSource(_ctxColl);
        ctxMenu->SetHorizontalOffset(3);
        ctxMenu->SetPlacement(PlacementMode::pmRight);
        ctxMenu->SetPlacementTarget(sender);
        ctxMenu->TrackContextMenu();
        ctxMenu->unref();

        ExpresionChanged();
    }
}

void SetterEditor::ExpresionChanged()
{
    HandleExpresion();
    if (IsExtension())
    {
        ResetSetterEditor();
        NotifySetterChanged();
    }
}

void SetterEditor::ResetSetter()
{
    SetRealValue(NULL, false);
    OnSetterValueChanged();
}

void SetterEditor::InitSetterNode(RTTIOfInfo* owner, DpProperty* dp, SetterNode* pSetter)
{
    pSetter->SetValue(dp->GetMetadata(owner)->GetDefaultValue());
}

void SetterEditor::OnInitialized(EventArg* e)
{
    Control::OnInitialized(e);
    InitializeBase();

    if (NULL == _operPanel)
    {
        _operPanel = new StackPanel();
        _operPanel->ref();

        AddVisualChild(_operPanel);
        AddLogicalChild(_operPanel);

        _operPanel->SetVerticalAlignment(VertAlign::vaCenter);

        Style* expresionStyle = RTTICast<Style>(FindRes(_U("UnlockExpresionStyle")));

        //_resetResBtn = CreateResButton(expresionStyle);
        //_resetResBtn->SetMargin(Rect(0,0,0,3));

        _selectResBtn = CreateResButton(expresionStyle);
        _selectResBtn->AddClick(new suic::ClickEventHandler(this, &SetterEditor::OnSelectResource));
    }

    AddHandler(MenuItem::ClickEvent, new ClickEventHandler(this, &SetterEditor::OnExpresionClick));
    AddHandler(EditorControl::EditValueChangedEvent, new RoutedEventHandler(this, &SetterEditor::OnEditValueChanged));
}

int SetterEditor::GetVisualChildrenCount()
{
    if (GetTemplateChild() == NULL)
    {
        return 0;
    }
    _childCount = 1;
    if (_operPanel != NULL)
    {
        ++_childCount;
    }
    return _childCount;
}

Element* SetterEditor::GetVisualChild(int index)
{
    if (index == 0)
    {
        return Control::GetVisualChild(index);
    }
    else
    {
        return _operPanel;
    }
}

void SetterEditor::OnEditValueChanged(DpObject* sender, RoutedEventArg* e)
{
    EditorControl* editCtrl = RTTICast<EditorControl>(e->GetOriginalSource());

    // 值已经改变，回调，让具体子类处理
    OnValueChanged(editCtrl);
}

void SetterEditor::OnValueChanged(EditorControl* sender)
{
    SetterNode* pSetter = GetSetterNode(true);
    if (NULL != pSetter)
    {
        ResNodePtr resNode = sender->GetResValue();
        // 对应的值已经改变，重新设置
        if (pSetter->GetResNode() != resNode.get())
        {
            pSetter->SetResNode(resNode.get());
        }
        RefleshSetterToDesignUI();
    }
}

//==============================================
// ButtonSetterEditor

ButtonSetterEditor::ButtonSetterEditor()
{
    _button = NULL;
}

suic::DpProperty* ButtonSetterEditor::ContentProperty;

void ButtonSetterEditor::StaticInit()
{
    ContentProperty = suic::DpProperty::Register(_U("Content"), RTTIType(), suic::Object::RTTIType(), 
        new suic::PropMetadata(NULL));
}

void ButtonSetterEditor::OnInitialized(suic::EventArg* e)
{

}

void ButtonSetterEditor::OnApplyTemplate()
{
    SetterEditor::OnApplyTemplate();
    _button = suic::RTTICast<suic::Button>(GetTemplateChild(_U("PART_Button")));
}

void ButtonSetterEditor::AddClick(suic::ClickEventHandler* handler)
{
    if (NULL == _button)
    {
        ApplyTemplate();
    }
    if (NULL != _button)
    {
        _button->AddClick(handler);
    }
}

//==============================================
// StringSetterEditor

void StringSetterEditor::OnValueChanged(EditorControl* sender)
{
    SetterEditor::OnValueChanged(sender);
}

void StringSetterEditor::OnInitialized(EventArg* e)
{
    SetterEditor::OnInitialized(e);
}

void StringSetterEditor::OnApplyTemplate()
{
    SetterEditor::OnApplyTemplate();
}

void NumericSetterEditor::OnValueChanged(EditorControl* sender)
{
    SetterEditor::OnValueChanged(sender);
}

void PosNumericSetterEditor::OnValueChanged(EditorControl* sender)
{
    SetterEditor::OnValueChanged(sender);
}

void FloatSetterEditor::OnValueChanged(EditorControl* sender)
{
    SetterEditor::OnValueChanged(sender);
}

void PosFloatSetterEditor::OnValueChanged(EditorControl* sender)
{
    SetterEditor::OnValueChanged(sender);
}


//==============================================
// TextBoxRangeSetterEditor

TextBoxRangeSetterEditor::TextBoxRangeSetterEditor()
{
}

TextBoxRangeSetterEditor::~TextBoxRangeSetterEditor()
{
}

void TextBoxRangeSetterEditor::OnValueChanged(EditorControl* sender)
{
    SetterEditor::OnValueChanged(sender);
}

//==============================================
// WidthSetterEditor

void WidthSetterEditor::OnValueChanged(EditorControl* sender)
{
    SetterEditor::OnValueChanged(sender);
}

void WidthSetterEditor::UpdateDefaultSetter(EditorControl* pText)
{
    // 创建默认值
    ResNodePtr resNode;
    pText->CreateDefaultValue(resNode);
    pText->UpdateResValue(resNode);
}

//==============================================
// SizeSetterEditor

SizeSetterEditor::SizeSetterEditor()
{
}

void SizeSetterEditor::OnValueChanged(EditorControl* sender)
{
    SetterEditor::OnValueChanged(sender);
}

//==============================================
// RectSetterEditor

RectSetterEditor::RectSetterEditor()
{
}

void RectSetterEditor::OnValueChanged(EditorControl* sender)
{
    SetterEditor::OnValueChanged(sender);
}

//==============================================
// fRectSetterEditor

fRectSetterEditor::fRectSetterEditor()
{
}

void fRectSetterEditor::OnValueChanged(EditorControl* sender)
{
    SetterEditor::OnValueChanged(sender);
}

//==============================================
// AlignVertSetterEditor

AlignVertSetterEditor::AlignVertSetterEditor()
{
    _updatingCheckValue = false;
    _isCallOnTemplate = false;
}

void AlignVertSetterEditor::NotifySetterChanged()
{
    suic::String strName;
    suic::RadioButton* pRadio = NULL;

    SetterNode* pSetter = GetSetterNode(false);
    if (NULL != pSetter)
    {
        DpItem* dpItem = DpManager::Ins()->FindDpItem(pSetter->GetProperty()->GetName());
        if (NULL != dpItem)
        {
            strName = dpItem->ValueToString(pSetter->GetResNode());
        }
        pRadio = RTTICast<RadioButton>(GetTemplateChild(strName));
    }
    else
    {
        if (GetName().IndexOf(_U("Content")) != -1)
        {
            strName = _U("Top");
        }
        else
        {
            strName = _U("Stretch");
        }
        pRadio = RTTICast<RadioButton>(GetTemplateChild(strName));
    }

    if (pRadio != NULL)
    {
        _updatingCheckValue = true;
        pRadio->SetChecked(suic::Boolean::True);
        _updatingCheckValue = false;
    }
}

void AlignVertSetterEditor::OnInitialized(suic::EventArg* e)
{
    InitializeBase();
    AddHandler(suic::RadioButton::CheckedEvent, new suic::RoutedEventHandler(this, &AlignVertSetterEditor::OnCheckedChanged));
}

void AlignVertSetterEditor::OnApplyTemplate()
{
    SetterEditor::OnApplyTemplate();

    SetterNode* pSetter = GetSetterNode(false);
    if (NULL != pSetter && !pSetter->GetValue())
    {
        pSetter->SetValue(suic::VertAlignBox::StretchBox);
    }

    NotifySetterChanged();
    _isCallOnTemplate = true;
}

void AlignVertSetterEditor::OnCheckedChanged(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    if (!_updatingCheckValue && _isCallOnTemplate)
    {
        Element* selRadio = RTTICast<Element>(e->GetOriginalSource());
        SetterNode* pSetter = GetSetterNode(true);
        if (NULL != pSetter && NULL != selRadio)
        {
            OString oVal(selRadio->GetName());
            oVal.setAutoDelete(false);
            ObjectPtr pVal;
            pSetter->GetProperty()->ConvertValue(&oVal, pVal);
            pSetter->SetValue(pVal.get());
            RefleshSetterToDesignUI();
        }
    }
}

//==============================================
// AlignHoriSetterEditor

DpProperty* AlignVertSetterEditor::GroupNameProperty;
DpProperty* AlignHoriSetterEditor::GroupNameProperty;

void AlignHoriSetterEditor::StaticInit()
{
    AlignVertSetterEditor::GroupNameProperty = DpProperty::Register(_U("GroupName"), 
        AlignVertSetterEditor::RTTIType(), OString::RTTIType(), DpPropMemory::GetPropMeta(NULL));
    AlignHoriSetterEditor::GroupNameProperty = DpProperty::Register(_U("GroupName"), 
        RTTIType(), OString::RTTIType(), DpPropMemory::GetPropMeta(NULL));
}

AlignHoriSetterEditor::AlignHoriSetterEditor()
{
    _updatingCheckValue = false;
    _isCallOnTemplate = false;
}

void AlignHoriSetterEditor::NotifySetterChanged()
{
    suic::String strName;
    suic::RadioButton* pRadio = NULL;

    SetterNode* pSetter = GetSetterNode(false);
    if (NULL != pSetter)
    {
        DpItem* dpItem = DpManager::Ins()->FindDpItem(pSetter->GetProperty()->GetName());
        if (NULL != dpItem)
        {
            strName = dpItem->ValueToString(pSetter->GetResNode());
        }

        pRadio = RTTICast<RadioButton>(GetTemplateChild(strName));
    }
    else
    {
        if (GetName().IndexOf(_U("Content")) != -1)
        {
            strName = _U("Left");
        }
        else
        {
            strName = _U("Stretch");
        }
        pRadio = RTTICast<RadioButton>(GetTemplateChild(strName));
    }

    if (pRadio != NULL)
    {
        _updatingCheckValue = true;
        pRadio->SetChecked(suic::Boolean::True);
        _updatingCheckValue = false;
    }
}

void AlignHoriSetterEditor::OnApplyTemplate()
{
    SetterEditor::OnApplyTemplate();
    SetterNode* pSetter = GetSetterNode(false);
    if (NULL != pSetter && !pSetter->GetValue())
    {
        pSetter->SetValue(suic::VertAlignBox::StretchBox);
    }
    NotifySetterChanged();
    _isCallOnTemplate = true;
}

void AlignHoriSetterEditor::OnInitialized(suic::EventArg* e)
{
    InitializeBase();
    AddHandler(suic::RadioButton::CheckedEvent, new suic::RoutedEventHandler(this, &AlignHoriSetterEditor::OnCheckedChanged));
}

void AlignHoriSetterEditor::OnCheckedChanged(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    if (!_updatingCheckValue && _isCallOnTemplate)
    {
        Element* selRadio = RTTICast<Element>(e->GetOriginalSource());
        SetterNode* pSetter = GetSetterNode(true);
        if (NULL != pSetter && NULL != selRadio)
        {
            OString oVal(selRadio->GetName());
            oVal.setAutoDelete(false);
            ObjectPtr pVal;
            pSetter->GetProperty()->ConvertValue(&oVal, pVal);
            pSetter->SetValue(pVal.get());
            RefleshSetterToDesignUI();
        }
    }
}

//==============================================
// GridViewSetterEditor

GridViewSetterEditor::GridViewSetterEditor()
{
}

void GridViewSetterEditor::NotifySetterChanged()
{
}


void GridViewSetterEditor::OnInitialized(suic::EventArg* e)
{
}

void GridViewSetterEditor::OnApplyTemplate()
{
}

//==============================================
// ExtensionSetterEditor

ExtensionSetterEditor::ExtensionSetterEditor()
{
    _extensionBox = NULL;
}

void ExtensionSetterEditor::OnSetterValueChanged()
{
    NotifySetterChanged();
}

void ExtensionSetterEditor::OnHandleExpresion(FrameworkElement* tempChild)
{
    SetterEditor::OnHandleExpresion(tempChild);
    if (NULL != tempChild)
    {
        tempChild->Enable(true);
    }
}

void ExtensionSetterEditor::NotifySetterChanged()
{
    suic::String strText;
    SetterNode* pSetter = GetSetterNode(false);
    if (NULL == _extensionBox)
    {
        _extensionBox = RTTICast<suic::TextBox>(GetTemplateChild(_U("PART_TextBox")));
    }

    if (NULL != pSetter)
    {
        suic::Extension* pExt = RTTICast<suic::Extension>(pSetter->GetValue());
        if (NULL != pExt)
        {
            strText = pExt->GetRTTIType()->typeName;
        }
    }

    if (NULL != _extensionBox)
    {
        _extensionBox->SetText(strText);
    }
}

void ExtensionSetterEditor::OnInitialized(suic::EventArg* e)
{
    SetterEditor::OnInitialized(e);
}

void ExtensionSetterEditor::OnApplyTemplate()
{
    SetterEditor::OnApplyTemplate();
    NotifySetterChanged();
}

//==============================================
// ChoiceSetterEditor

DpProperty* ChoiceSetterEditor::SelectIndexProperty;

void ChoiceSetterEditor::StaticInit()
{
    SelectIndexProperty = DpProperty::Register(_U("SelectIndex"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(NULL));
}

ChoiceSetterEditor::ChoiceSetterEditor()
{
    _comboBox = NULL;
    _updatingCheckValue = false;
    _items = new ObservableCollection();
    _items->ref();
}

ChoiceSetterEditor::~ChoiceSetterEditor()
{
     _items->unref();
}

int ChoiceSetterEditor::FindIndexByText(const String& text)
{
    int iSel = -1;
    ItemCollection* itemColl = _comboBox->GetItemsSource();

    for (int i = 0; i < itemColl->GetCount(); ++i)
    {
        String strText = itemColl->GetItem(i)->ToString();
        if (strText.Equals(text))
        {
            iSel = i;
            break;
        }
    }

    return iSel;
}

void ChoiceSetterEditor::NotifySetterChanged()
{
    SetterNode* pSetter = GetSetterNode(false);

    _updatingCheckValue = true;

    if (NULL != _comboBox)
    {
        int index = -1;

        DpItem* dpItem = GetDpItem();
        OptionDpItem* optDpItem = dynamic_cast<OptionDpItem*>(dpItem);

        if (NULL != dpItem)
        {
            index = dpItem->GetSelectIndex();
        }

        if (NULL != pSetter)
        {
            suic::String strText;
            DpItem* dpItem = pSetter->GetDpItem();

            if (dpItem != NULL)
            {
                strText = dpItem->ValueToString(pSetter->GetResNode());
                index = FindIndexByText(strText);
            }
        }

        if (index < 0)
        {
            PropMetadata* meta = GetDefaultMeta();
            if (NULL != meta)
            {
                index = meta->GetDefaultValue()->ToInt();
                if (optDpItem != NULL)
                {
                    index = optDpItem->ValueToIndex(index);
                }
            }
        }

        if (index >= 0)
        {
            _comboBox->SetSelectedIndex(index);
        }
    }

    _updatingCheckValue = false;
}

void ChoiceSetterEditor::UpdateChoiceItem()
{
    NotifySetterChanged();
}

void ChoiceSetterEditor::AddChoiceItem(Object* item)
{
    _items->AddItem(item);
}

void ChoiceSetterEditor::AddChild(Object* obj)
{
    obj->ref();
    AddChoiceItem(obj);
    obj->unref();
}

void ChoiceSetterEditor::AddText(String val)
{
    AddChild(new OString(val));
}

void ChoiceSetterEditor::RemoveChild(Object* child)
{
    _items->RemoveItem(child);
}

void ChoiceSetterEditor::ResetSetterEditor()
{
    SetterEditor::ResetSetterEditor();
    //_items->Clear();
}

void ChoiceSetterEditor::OnSelItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e)
{
    e->SetHandled(true);

    if (!_updatingCheckValue)
    {
        SetterNode* pSetter = GetSetterNode(true);

        if (NULL != pSetter)
        {
            Object* selObj = _comboBox->GetSelectedItem();
            suic::DpProperty* dp = pSetter->GetProperty();
            suic::OString tmp(selObj->ToString());
            tmp.setAutoDelete(false);
            suic::ObjectPtr pVal;
            dp->ConvertValue(&tmp, pVal);

            if (NULL == pVal.get())
            {
                pSetter->SetValue(selObj);
            }
            else
            {
                pSetter->SetValue(pVal.get());
            }

            RefleshSetterToDesignUI();
        }
    }
}

void ChoiceSetterEditor::ClearItems()
{
    _items->Clear();
}

int ChoiceSetterEditor::InitComboBoxItems()
{
    int index = 0;
    if (_items->GetCount() == 0)
    {
        DpItem* dpItem = GetDpItem();
        if (NULL != dpItem)
        {
            _items->Clear();
            dpItem->FillAddChild(this);
            index = dpItem->GetSelectIndex();
        }
    }
    return index;
}

void ChoiceSetterEditor::OnApplyTemplate()
{
    SetterEditor::OnApplyTemplate();

    _comboBox = RTTICast<ComboBox>(GetTemplateChild(_U("PART_ComboBox")));
    int index = InitComboBoxItems();
    _comboBox->SetItemsSource(_items);
    UpdateChoiceItem();
    _comboBox->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &ChoiceSetterEditor::OnSelItemChanged));
}

void ChoiceSetterEditor::OnInitialized(suic::EventArg* e)
{
    SetterEditor::OnInitialized(e);
}

//==============================================
// CursorSetterEditor

CursorSetterEditor::CursorSetterEditor()
{

}

CursorSetterEditor::~CursorSetterEditor()
{

}

void CursorSetterEditor::ClearItems()
{
}

void CursorSetterEditor::NotifySetterChanged()
{
    SetterNode* pSetter = GetSetterNode(false);

    _updatingCheckValue = true;

    if (NULL != _comboBox)
    {
        int index = 0;

        if (NULL != pSetter)
        {
            suic::String strText;
            suic::OCursor* pCursor = RTTICast<suic::OCursor>(pSetter->GetValue());

            if (pCursor != NULL)
            {
                strText = CursorResNode::CursorToString(pCursor);
                index = FindIndexByText(strText);
            }
        }

        if (index < 0)
        {
            PropMetadata* meta = GetDefaultMeta();
            if (NULL != meta)
            {
                index = meta->GetDefaultValue()->ToInt();
            }
            else
            {
                // meta为空表示可能来自资源编辑
                index = 0;
            }
        }

        _comboBox->SetSelectedIndex(index);
    }

    _updatingCheckValue = false;
}

void CursorSetterEditor::OnSelItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e)
{
    e->SetHandled(true);

    if (!_updatingCheckValue)
    {
        SetterNode* pSetter = GetSetterNode(true);

        if (NULL != pSetter)
        {
            Object* selObj = _comboBox->GetSelectedItem();
            suic::DpProperty* dp = pSetter->GetProperty();
            suic::String tmp = selObj->ToString();
            
            suic::OCursor* pCursor = suic::OCursor::GetCusor(tmp);
            if (NULL != pCursor)
            {
                pSetter->SetValue(pCursor);
            }
            else
            {
                pSetter->SetValue(NULL);
            }

            RefleshSetterToDesignUI();
        }
    }
}

void CursorSetterEditor::OnApplyTemplate()
{
    SetterEditor::OnApplyTemplate();

    if (_items->GetCount() == 0)
    {
        _items->AddItem(new suic::OString(_U("Arrow")));
        _items->AddItem(new suic::OString(_U("Hand")));
        _items->AddItem(new suic::OString(_U("Wait")));
        _items->AddItem(new suic::OString(_U("Cross")));
        _items->AddItem(new suic::OString(_U("Resize")));
        _items->AddItem(new suic::OString(_U("Splith")));
        _items->AddItem(new suic::OString(_U("Splitv")));
        _items->AddItem(new suic::OString(_U("Ibeam")));
        _items->AddItem(new suic::OString(_U("SizeNwse")));
        _items->AddItem(new suic::OString(_U("SizeNesw")));
        _items->AddItem(new suic::OString(_U("SizeWe")));
        _items->AddItem(new suic::OString(_U("SizeNs")));
        _items->AddItem(new suic::OString(_U("SizeAll")));
    }

    _comboBox = RTTICast<ComboBox>(GetTemplateChild(_U("PART_ComboBox")));
    _comboBox->SetItemsSource(_items);
    
    NotifySetterChanged();

    _comboBox->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &CursorSetterEditor::OnSelItemChanged));
}

//==============================================
// CheckSetterEditor

DpProperty* CheckSetterEditor::ContentProperty;

void CheckSetterEditor::StaticInit()
{
    ContentProperty = DpProperty::Register(_U("Content"), RTTIType(), OString::RTTIType(), DpPropMemory::GetPropMeta(NULL));
}

CheckSetterEditor::CheckSetterEditor()
{
    _updatingCheckValue = false;
    _checkBox = NULL;
}

void CheckSetterEditor::OnChecked(DpObject* sender, RoutedEventArg* e)
{
    OnCheckedOverride();
}

void CheckSetterEditor::OnUnchecked(DpObject* sender, RoutedEventArg* e)
{
    OnUncheckedOverride();
}

void CheckSetterEditor::SetDefaultValue(Object* val)
{
    _updatingCheckValue = true;

    if (NULL == _checkBox)
    {
        ApplyTemplate();
    }

    suic::Boolean* pBool = RTTICast<Boolean>(val);
    if (pBool && pBool == Boolean::True)
    {
        _checkBox->SetChecked(Boolean::True);
    }
    else
    {
        _checkBox->SetChecked(Boolean::False);
    }

    _updatingCheckValue = false;
}

void CheckSetterEditor::OnCheckedOverride()
{
    if (!_updatingCheckValue)
    {
        SetterNode* pSetter = GetSetterNode(true);
        if (NULL != pSetter)
        {
            pSetter->SetValue(suic::Boolean::True);
            RefleshSetterToDesignUI();
        }
    }
}

void CheckSetterEditor::OnUncheckedOverride()
{
    if (!_updatingCheckValue)
    {
        SetterNode* pSetter = GetSetterNode(true);
        if (NULL != pSetter)
        {
            pSetter->SetValue(suic::Boolean::False);
            RefleshSetterToDesignUI();
        }
    }
}

void CheckSetterEditor::NotifySetterChanged()
{
    SetterNode* pSetter = GetSetterNode(false);

    _updatingCheckValue = true;

    if (NULL == _checkBox)
    {
        ApplyTemplate();
    }

    if (NULL != pSetter)
    {
        suic::Boolean* pBool = RTTICast<Boolean>(pSetter->GetValue());
        if (pBool && pBool == Boolean::True)
        {
            _checkBox->SetChecked(Boolean::True);
        }
        else
        {
            _checkBox->SetChecked(Boolean::False);
        }
    }
    else
    {
        _checkBox->SetChecked(Boolean::True);
    }

    _updatingCheckValue = false;
}

void CheckSetterEditor::OnApplyTemplate()
{
    SetterEditor::OnApplyTemplate();

    _checkBox = RTTICast<CheckBox>(GetTemplateChild(_U("PART_CheckBox")));
    _checkBox->AddCheckedEvent(new RoutedEventHandler(this, &CheckSetterEditor::OnChecked));
    _checkBox->AddUncheckedEvent(new RoutedEventHandler(this, &CheckSetterEditor::OnUnchecked));
}

//==============================================
// FontWeightSetterEditor

FontWeightSetterEditor::FontWeightSetterEditor()
{

}

void FontWeightSetterEditor::NotifySetterChanged()
{
    SetterNode* pSetter = GetSetterNode(false);

    _updatingCheckValue = true;

    if (NULL == _checkBox)
    {
        ApplyTemplate();
    }

    if (NULL != pSetter)
    {
        if (NULL == pSetter->GetValue())
        {
            _checkBox->SetChecked(Boolean::False);
        }
        else
        {
            int iVal = pSetter->GetValue()->ToInt();
            if (iVal > FontWeightStyle::fwNormal)
            {
                _checkBox->SetChecked(Boolean::True);
            }
            else
            {
                _checkBox->SetChecked(Boolean::False);
            }
        }
    }
    else
    {
        _checkBox->SetChecked(Boolean::False);
    }

    _updatingCheckValue = false;
}

void FontWeightSetterEditor::OnCheckedOverride()
{
    if (!_updatingCheckValue)
    {
        SetterNode* pSetter = GetSetterNode(true);
        if (NULL != pSetter)
        {
            pSetter->SetValue(new suic::Integer(FontWeightStyle::fwExtraBold));
            RefleshSetterToDesignUI();
        }
    }
}

void FontWeightSetterEditor::OnUncheckedOverride()
{
    if (!_updatingCheckValue)
    {
        SetterNode* pSetter = GetSetterNode(true);
        if (NULL != pSetter)
        {
            pSetter->SetValue(new suic::Integer(FontWeightStyle::fwNormal));
            RefleshSetterToDesignUI();
        }
    }
}

//==============================================
// FontItalicSetterEditor

FontStyleSetterEditor::FontStyleSetterEditor()
{

}

void FontStyleSetterEditor::NotifySetterChanged()
{
    SetterNode* pSetter = GetSetterNode(false);

    _updatingCheckValue = true;

    if (NULL == _checkBox)
    {
        ApplyTemplate();
    }

    if (NULL != pSetter)
    {
        if (NULL == pSetter->GetValue())
        {
            _checkBox->SetChecked(Boolean::False);
        }
        else
        {
            int iVal = pSetter->GetValue()->ToInt();
            if (iVal == FontStyleStyle::fssItalic)
            {
                _checkBox->SetChecked(Boolean::True);
            }
            else
            {
                _checkBox->SetChecked(Boolean::False);
            }
        }
    }
    else
    {
        _checkBox->SetChecked(Boolean::False);
    }

    _updatingCheckValue = false;
}

void FontStyleSetterEditor::OnCheckedOverride()
{
    if (!_updatingCheckValue)
    {
        SetterNode* pSetter = GetSetterNode(true);
        if (NULL != pSetter)
        {
            pSetter->SetValue(new suic::Integer(FontStyleStyle::fssItalic));
            RefleshSetterToDesignUI();
        }
    }
}

void FontStyleSetterEditor::OnUncheckedOverride()
{
    if (!_updatingCheckValue)
    {
        SetterNode* pSetter = GetSetterNode(true);
        if (NULL != pSetter)
        {
            pSetter->SetValue(new suic::Integer(FontStyleStyle::fssNormal));
            RefleshSetterToDesignUI();
        }
    }
}

//==============================================
// FontUnderlineSetterEditor

FontUnderlineSetterEditor::FontUnderlineSetterEditor()
{

}

void FontUnderlineSetterEditor::NotifySetterChanged()
{

}

void FontUnderlineSetterEditor::OnChecked(Element* sender, RoutedEventArg* e)
{

}

void FontUnderlineSetterEditor::OnUnchecked(Element* sender, RoutedEventArg* e)
{

}

//==============================================
// FontSizeSetterEditor

FontSizeSetterEditor::FontSizeSetterEditor()
{

}

void FontSizeSetterEditor::NotifySetterChanged()
{
    ChoiceSetterEditor::NotifySetterChanged();
}

void FontSizeSetterEditor::ClearItems()
{
}

void FontSizeSetterEditor::OnApplyTemplate()
{
    _items->Clear();
    for (int i = 0; i <= 63; ++i)
    {
        _items->AddItem(new Integer(i + 5));
    }
    ChoiceSetterEditor::OnApplyTemplate();
}

//==============================================
// BrushElement

BrushElement::BrushElement()
{
}

/*void BrushElement::NotifySetterChanged(BrushSetterEditor* brushEdit)
{
    SetterNode* pSetter = brushEdit->GetSetterNode(false);

    InitBrushEditor();

    if (NULL != pSetter)
    {
        suic::Brush* pBrush = suic::RTTICast<suic::Brush>(pSetter->GetValue());

        if (!brushEdit->IsExtension())
        {
            SetBackground(pBrush);
        }
        else
        {
            // 查找资源
            _brushEditor->ResetBrushElement(this);
        }

        if (NULL == _brushEditor->GetCurrentBrushElement() || _brushEditor->IsBrushReset())
        {
            SetChecked(Boolean::True);
            _brushEditor->SetCurrentBrushElement(this);
            _brushEditor->RefleshBrushElement(this, pSetter);
        }
    }
    else
    {
        SetBackground(NULL);
        if (NULL != _brushEditor)
        {
            _brushEditor->ResetBrushElement(this);
            if (NULL == _brushEditor->GetCurrentBrushElement() && _brushEditor->IsBrushReset())
            {
                SetChecked(Boolean::True);
                _brushEditor->SetCurrentBrushElement(this);
            }
        }
    }
}*/

/*void BrushElement::CheckBrushIsChanged(BrushEditor* brushEditor)
{
    BrushSetterEditor* parent = RTTICast<BrushSetterEditor>(GetTemplatedParent());
    ResNodePtr resTmp = brushEditor->GetResValue();
    SetterNode* pSetter = parent->GetSetterNode(true);
    if (NULL == pSetter)
    {
        pSetter = NULL;
    }

    if (pSetter->GetResNode() != resTmp.get())
    {
        Brush* pBrush = NULL;

        if (NullBrushSetterEditor::RTTIType() == brushEditor->GetRTTIType())
        {
            pSetter->SetValue(pBrush);
        }
        else
        {
            if (!brushEditor->GetRealValue())
            {
                brushEditor->CreateDefaultValue();
                resTmp = brushEditor->GetResValue();
            }

            pSetter->SetResNode(resTmp.get());
            pBrush = RTTICast<Brush>(brushEditor->GetRealValue());
        }

        SetBackground(pBrush);
    }

    RefleshSetterToDesignUI(parent);
    // 通知上层画刷已经改变
    parent->NotifyBrushChanged();
}
*/
/*void BrushElement::UpdateBrushElement(Brush* brush)
{
    BrushSetterEditor* parent = RTTICast<BrushSetterEditor>(GetTemplatedParent());

    SetterNode* pSetter = parent->GetSetterNode(true);
    if (NULL != pSetter)
    {
        pSetter->SetValue(brush);
    }

    SetBackground(brush);
    RefleshSetterToDesignUI(parent);
    UpdateLayout();

    // 通知上层画刷已经改变
    parent->NotifyBrushChanged();
}*/

void BrushElement::OnClick()
{
    suic::RadioButton::OnClick();

    BrushSetterEditor* brushEditor = RTTICast<BrushSetterEditor>(GetTemplatedParent());
    if (brushEditor != NULL)
    {
        brushEditor->SwitchToEdit(true);
    }

    /*InitBrushEditor();
    if (_brushEditor != NULL && IsChecked() == Boolean::True)
    {
        BrushSetterEditor* parent = RTTICast<BrushSetterEditor>(GetTemplatedParent());
        SetterNode* pSetter = parent->GetSetterNode(true);

        _brushEditor->RefleshBrushElement(this, pSetter);
        _brushEditor->SetCurrentBrushElement(this);
    }*/
}

//==============================================
// BrushSetterEditor

DpProperty* BrushSetterEditor::ContentProperty;

void BrushSetterEditor::StaticInit()
{
    ContentProperty = suic::DpProperty::Register(_U("Content"), RTTIType(), suic::OString::RTTIType()
        , new suic::PropMetadata(suic::OString::EmptyString, PropMetadataOptions::AffectsRender));
}

BrushSetterEditor::BrushSetterEditor()
{
    _brushEditor = NULL;
    _isPending = false;
}

void BrushSetterEditor::InitBrushEditor()
{
    if (NULL == _brushEditor)
    {
        FrameworkElement* layHolder = CASTTOFE(GetTemplatedParent());

        if (NULL == layHolder)
        {
            layHolder = GetParent()->GetParent()->GetParent();
            if (NULL != layHolder)
            {
                _brushEditor = RTTICast<BrushEditorPanel>(layHolder);//layHolder->FindElem<BrushEditorPanel>(_U("BrushEditor"));
            }
        }
        else
        {
            _brushEditor = RTTICast<BrushEditorPanel>(layHolder->GetTemplateChild(_U("BrushEditor")));
        }
        
    }
}

void BrushSetterEditor::SetChecked()
{
    BrushElement* elem = RTTICast<BrushElement>(GetTemplateChild(_U("PART_BrushElement")));
    if (NULL == elem)
    {
        ApplyTemplate();
        elem = RTTICast<BrushElement>(GetTemplateChild(_U("PART_BrushElement")));
    }

    if (NULL != elem)
    {
        elem->SetChecked(suic::Boolean::True);
    }
}

void BrushSetterEditor::UpdateToNullDisplay()
{
    SetBackground(NULL);

    if (NULL != _brushEditor)
    {
        // 首先选中自己
        SetChecked();
        _brushEditor->SwitchToBrushSetterEditor(this, true);
    }
}

void BrushSetterEditor::ResetSetter()
{
    SetterEditor::ResetSetter();
    UpdateToNullDisplay();
}

void BrushSetterEditor::UpdateExpressValue(Object* val, bool bCreate)
{
    SetterEditor::UpdateExpressValue(val, bCreate);
    UpdateToNullDisplay();
}

void BrushSetterEditor::SwitchToEdit(bool bForce)
{
    // 上一次操作未完成，直接返回
    if (_isPending)
    {
        return;
    }

    _isPending = true;

    InitBrushEditor();

    if (NULL != _brushEditor)
    {
        // 首先选中自己
        SetChecked();
        _brushEditor->SwitchToBrushSetterEditor(this, bForce);
    }

    _isPending = false;
}

void BrushSetterEditor::NotifyUnlockExpresion()
{
    /*BrushResSetterEditor* resSetter = RTTICast<BrushResSetterEditor>(this->GetTemplatedParent());
    if (NULL != resSetter)
    {
        resSetter->NotifySetterChanged();
    }*/
    SetterNode* pSetter = GetSetterNode(false);
    if (NULL != pSetter)
    {
        pSetter->SetValue(SolidColorBrush::White);
    }

    NotifySetterChanged();
}

void BrushSetterEditor::NotifySetterChanged()
{
    // 表达式不允许编辑
    /*if (this->IsExtension())
    {
        if (NULL == _brushEditor)
        {
            InitBrushEditor();
        }
        _brushEditor->EnableExtensionFlag();
        return;
    }*/

    Brush* brush = NULL;
    // 第一次初始化
    // 先设置其背景刷等
    SetterNode* pSetter = GetSetterNode(false);
    if (NULL != pSetter)
    {
        // 初始有值，设置背景
        brush = RTTICast<Brush>(pSetter->GetValue());
    }
    
    // 设置背景刷
    SetBackground(brush);

    InitBrushEditor();

    if (NULL != _brushEditor)
    {
        if (IsResetPending())
        {
            _brushEditor->SwitchToNullBrushEditor(this);
        }
        else
        {
            if (brush != NULL)
            {
                if (!_brushEditor->HasSetBrush())
                {
                    SwitchToEdit(true);
                }
            }
            else if (NULL == _brushEditor->GetBrushSetterEditor())
            {
                SwitchToEdit(false);
            }
        }
    }

    if (NULL != brush)
    {
        //SetChecked();
    }
}

void BrushSetterEditor::NotifyBrushChanged()
{
}

//void BrushSetterEditor::UpdateBrushElement(Brush* brush)
//{
//    BrushElement* elem = RTTICast<BrushElement>(GetTemplateChild(_U("PART_BrushElement")));
//    if (NULL != elem)
//    {
//        elem->UpdateBrushElement(brush);
//    }
//}

//==============================================
// BrushEditorPanel

BrushEditorPanel::BrushEditorPanel()
{
    _isBrushReset = true;
    _isRefleshing = false;

    _nullBrush = NULL;
    _solidBrush = NULL;
    _linearBrush = NULL;
    _radialBrush = NULL;
    _imageBrush = NULL;
    _brushEditor = NULL;
}

void BrushEditorPanel::ResetBrushEditorPanel()
{
    _brushEditor = NULL;
    _isRefleshing = true;
    
    if (NULL != _solidBrush)
    {
        _solidBrush->ResetRealValue();
    }
    if (NULL != _linearBrush)
    {
        _linearBrush->ResetRealValue();
    }
    if (NULL != _radialBrush)
    {
        _radialBrush->ResetRealValue();
    }
    if (NULL != _imageBrush)
    {
        _imageBrush->ResetRealValue();
    }

    RadioButton* pBtn = FindElem<RadioButton>(_U("NullBrush_R"));
    if (NULL != pBtn)
    {
        pBtn->SetChecked(Boolean::True);
    }

    _isRefleshing = false;
    _isBrushReset = true;
}

bool BrushEditorPanel::HasSetBrush() const
{
    return (!_isBrushReset && _brushEditor != NULL);
}

BrushSetterEditor* BrushEditorPanel::GetBrushSetterEditor() const
{
    return _brushEditor;
}

void BrushEditorPanel::SetCheckBrushEditor(BrushEditor* brushEditor)
{
    String strName = brushEditor->GetName();
    strName.Replace(_U("_N"), _U("_R"));
    RadioButton* pBtn = RTTICast<RadioButton>(VisualTreeOp::FindName(this, strName));
    if (NULL != pBtn)
    {
        pBtn->SetChecked(Boolean::True);
    }
}

void BrushEditorPanel::SwitchToNullBrushEditor(BrushSetterEditor* brushEditor)
{
    if (NULL == _brushEditor)
    {
        SetCheckBrushEditor(_nullBrush);
        brushEditor->SetChecked();
        _brushEditor = brushEditor;
    }
}

void BrushEditorPanel::SwitchToExpressionBrushEditor(BrushSetterEditor* brushEditor)
{
}

void BrushEditorPanel::EnableExtensionFlag()
{
    _brushEditor = NULL;
    if (_nullBrush != NULL)
    {
        SetCheckBrushEditor(_nullBrush);
    }
    Enable(false);
}

void BrushEditorPanel::SwitchToBrushSetterEditor(BrushSetterEditor* brushEditor, bool bForce)
{
    if (bForce || _brushEditor != brushEditor)
    {
        _isRefleshing = true;

        _brushEditor = NULL;
        Brush* brush = NULL;

        // 根据brushEditor的Brush值定义初始的Brush编辑页面
        SetterNode* pSetter = brushEditor->GetSetterNode(false);
        if (NULL != pSetter)
        {
            brush = RTTICast<Brush>(pSetter->GetValue());
        }
        
        if (NULL == brush)
        {
            if (NULL != _nullBrush)
            {
                SetCheckBrushEditor(_nullBrush);
            }
            _brushEditor = brushEditor;
        }
        else
        {
            String strType = brush->GetRTTIType()->typeName;
            String strName;

            strName.Format(_U("%s_R"), strType.c_str());

            RadioButton* pBtn = RTTICast<RadioButton>(VisualTreeOp::FindName(this, strName));
            if (NULL != pBtn)
            {
                strName.Format(_U("%s_N"), strType.c_str());
                BrushEditor* editor = RTTICast<BrushEditor>(VisualTreeOp::FindName(this, strName));

                if (NULL != editor)
                {
                    ResNodePtr resNode;
                    _isBrushReset = false;

                    if (NULL != pSetter->GetResNode())
                    {
                        ResNode::CreateResNode(pSetter->GetResNode()->GetValue(), resNode);
                    }
                    else
                    {
                        resNode = pSetter->GetResNode();
                    }

                    editor->UpdateResValue(resNode);
                    _brushEditor = brushEditor;
                }

                pBtn->SetChecked(Boolean::True);
            }
        }

        _isRefleshing = false;
    }
}

void BrushEditorPanel::UpdateSetterToDesignUI(ResNode* resNode)
{
    if (NULL != resNode)
    {
        Brush* brush = RTTICast<Brush>(resNode->GetValue());

        _brushEditor->SetBackground(brush);
        _brushEditor->UpdateLayout();
        // 刷新改变值到界面显示
        _brushEditor->RefleshSetterToDesignUI();
    }
}

void BrushEditorPanel::SwitchToBrushEditor(BrushEditor* brushEditor)
{
    if (NULL != _brushEditor && !_isRefleshing && 
        !_brushEditor->IsExtension())
    {
        SetterNode* pSetter = _brushEditor->GetSetterNode(true);
        ResNodePtr resNode = brushEditor->GetCloneResValue();

        if (NULL == pSetter)
        {
            return;
        }

        _isRefleshing = true;

        if (!resNode)
        {
            brushEditor->CreateDefaultValue(resNode);
        }

        brushEditor->UpdateResValue(resNode);
        pSetter->SetResNode(resNode.get());
        UpdateSetterToDesignUI(resNode.get());

        _isRefleshing = false;
    }
}

void BrushEditorPanel::OnBrushChanged(DpObject* sender, RoutedEventArg* e)
{
    BrushEditor* brushEditor = RTTICast<BrushEditor>(e->GetOriginalSource());
    if (NULL != _brushEditor && !_brushEditor->IsExtension() && !_isRefleshing)
    {
        ResNodePtr resNode = brushEditor->GetCloneResValue();
        SetterNode* pSetter = _brushEditor->GetSetterNode(true);
        if (NULL != pSetter && pSetter->GetResNode() != resNode.get())
        {
            pSetter->SetResNode(resNode.get());
        }
        UpdateSetterToDesignUI(resNode.get());
    }
}

void BrushEditorPanel::OnRadioChecked(DpObject* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
}

void BrushEditorPanel::OnInitialized(EventArg* e)
{
    suic::StackPanel::OnInitialized(e);

    _nullBrush = FindElem<BrushEditor>(_U("NullBrush_N"));
    _solidBrush = FindElem<BrushEditor>(_U("SolidColorBrush_N"));
    _linearBrush = FindElem<BrushEditor>(_U("LinearGradientBrush_N"));
    _radialBrush = FindElem<BrushEditor>(_U("RadialGradientBrush_N"));
    _imageBrush = FindElem<BrushEditor>(_U("ImageBrush_N"));

    // 如果没有空画刷，当成属性的独立编辑模式
    if (NULL == _nullBrush)
    {
        _brushEditor = RTTICast<BrushSetterEditor>(VisualTreeOp::FindName(this, _U("PART_BrushSetterEditor")));
        _brushEditor->SetChecked();
    }

    AddHandler(BrushEditor::BrushChangedEvent, new RoutedEventHandler(this, &BrushEditorPanel::OnBrushChanged));
    AddHandler(ToggleButton::CheckedEvent, new RoutedEventHandler(this, &BrushEditorPanel::OnRadioChecked));
}

void BrushEditorPanel::OnApplyTemplate()
{
    suic::StackPanel::OnApplyTemplate();
}

static BrushEditorPanel* GetBrushEditorPanel(Element* child)
{
    BrushEditorPanel* brushPanel = RTTICast<BrushEditorPanel>(child);

    while (brushPanel == NULL)
    {
        if (child->GetUIParent() == NULL)
        {
            break;
        }
        child = child->GetUIParent();
        brushPanel = RTTICast<BrushEditorPanel>(child);
    }

    return brushPanel;
}

void ImageBrushSetterEditor::OnIsVisibleChanged(IsVisibleChangedEventArg* e)
{
    if (e->GetValue())
    {
        BrushEditorPanel* brPanel = GetBrushEditorPanel(this);
        if (NULL != brPanel)
        {
            brPanel->SwitchToBrushEditor(this);
        }
    }
}

void NullBrushSetterEditor::OnIsVisibleChanged(IsVisibleChangedEventArg* e)
{
    if (e->GetValue())
    {
        BrushEditorPanel* brPanel = GetBrushEditorPanel(this);
        if (NULL != brPanel)
        {
            brPanel->SwitchToBrushEditor(this);
        }
    }
}

void SolidColorBrushSetterEditor::OnIsVisibleChanged(IsVisibleChangedEventArg* e)
{
    if (e->GetValue())
    {
        BrushEditorPanel* brPanel = GetBrushEditorPanel(this);
        if (NULL != brPanel)
        {
            brPanel->SwitchToBrushEditor(this);
        }
    }
}

void LinearGradientBrushSetterEditor::OnIsVisibleChanged(IsVisibleChangedEventArg* e)
{
    if (e->GetValue())
    {
        BrushEditorPanel* brPanel = GetBrushEditorPanel(this);
        if (NULL != brPanel)
        {
            brPanel->SwitchToBrushEditor(this);
        }
    }
}

void RadialGradientBrushSetterEditor::OnIsVisibleChanged(IsVisibleChangedEventArg* e)
{
    if (e->GetValue())
    {
        BrushEditorPanel* brPanel = GetBrushEditorPanel(this);
        if (NULL != brPanel)
        {
            brPanel->SwitchToBrushEditor(this);
        }
    }
}

BrushResSetterEditor::BrushResSetterEditor()
{
}

void BrushResSetterEditor::NotifySetterChanged()
{
    BrushSetterEditor* pSetterEditor = RTTICast<BrushSetterEditor>(GetTemplateChild(_U("PART_BrushSetterEditor")));
    if (NULL != pSetterEditor)
    {
        SetterNode* pSetter = GetSetterNode(false);
        if (NULL == pSetter)
        {
            pSetterEditor->SetSetterNode(pSetter, ePropertyType::ePropStyle);
        }
        else
        {
            pSetterEditor->SetSetterNode(pSetter, pSetter->GetFromTrigger());
        }
    }
}

void BrushResSetterEditor::SetTargetElement(DesignElement* target)
{
    BrushSetterEditor* pSetterEditor = RTTICast<BrushSetterEditor>(GetTemplateChild(_U("PART_BrushSetterEditor")));
    if (NULL == pSetterEditor)
    {
        ApplyTemplate();
        pSetterEditor = RTTICast<BrushSetterEditor>(GetTemplateChild(_U("PART_BrushSetterEditor")));
    }

    if (NULL != pSetterEditor)
    {
        pSetterEditor->SetEditTarget(target);
    }
}

void BrushResSetterEditor::UpdateBrushElement(Brush* brush)
{
}

void BrushResSetterEditor::OnApplyTemplate()
{
    NotifySetterChanged();
}

int BrushResSetterEditor::GetVisualChildrenCount()
{
    return suic::Control::GetVisualChildrenCount();
}

void BrushResSetterEditor::OnHandleExpresion(FrameworkElement* tempChild)
{

}

void BrushResSetterEditor::EndRenderChildren(Drawing* drawing)
{
}

void BrushResSetterEditor::ResetBrushEditor()
{
    BrushEditorPanel* brushEditor = RTTICast<BrushEditorPanel>(GetTemplateChild(_U("BrushEditor")));
    if (NULL != brushEditor)
    {
        brushEditor->ResetBrushEditorPanel();
    }
}

void BrushResSetterEditor::OnInitialized(suic::EventArg* e)
{
}

//==============================================
// STSetterEditor

STSetterEditor::STSetterEditor()
{
    _info = NULL;
    _editBtn = NULL;
}

void STSetterEditor::OnSetterValueChanged()
{
    UpdateShow();
}

void STSetterEditor::UpdateShow()
{
    if (NULL != _info)
    {
        SetterNode* pSetter = GetSetterNode(false);

        if (pSetter == NULL || (IsSetterEmpty(pSetter)))
        {
            _info->SetText(_U("no value"));
        }
        else
        {
            _info->SetText(_U("has value"));
        }
    }
}

bool STSetterEditor::IsSetterEmpty(SetterNode* pSetter)
{
    return (pSetter->GetValue() == NULL);
}

void STSetterEditor::NotifySetterChanged()
{
    SetterEditor::NotifySetterChanged();

    UpdateShow();
    UpdateDesignPanel();
}

void STSetterEditor::UpdateDesignPanel()
{
    DesignElement* target = GetEditTarget();
    EditRootPanel* editPanel = FindEditRttoPanel(this);
    if (NULL != target && editPanel != NULL)
    {
        editPanel->UpdateDesignPanel(target);
    }
}

suic::Button* STSetterEditor::GetEditButton()
{
    if (NULL == _editBtn)
    {
        ApplyTemplate();
        _editBtn = RTTICast<suic::Button>(GetTemplateChild(_U("PART_EditStyle")));
    }
    return _editBtn;
}

void STSetterEditor::OnApplyTemplate()
{
    SetterEditor::OnApplyTemplate();

    _editBtn = RTTICast<suic::Button>(GetTemplateChild(_U("PART_EditStyle")));
    if (NULL != _editBtn)
    {
        _editBtn->AddClick(new suic::ClickEventHandler(this, &STSetterEditor::OnEditStyleTemplateClick));
    }

    _info = RTTICast<suic::TextBlock>(GetTemplateChild(_U("PART_BindingInfo")));
    if (NULL != _info)
    {
        _info->SetWidth(-1);
    }
}

void STSetterEditor::OnEditStyleTemplateClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    OnEditClick();
    OnSetterValueChanged();
}

void STSetterEditor::OnEditClick()
{

}

void STSetterEditor::GetMainReturnEvent(suic::EventHandler& h)
{
    h += suic::EventHandler(this, &STSetterEditor::OnMainReturnEventClick);
}

void STSetterEditor::OnMainReturnEventClick(suic::Object* sender, suic::EventArg* e)
{
    OnMainReturnClick(sender);
}

void STSetterEditor::OnMainReturnClick(suic::Object* sender)
{
}

//==============================================
// TemplateSetterEditor

TemplateSetterEditor::TemplateSetterEditor()
{
}

void TemplateSetterEditor::NotifySetterChanged()
{
    STSetterEditor::NotifySetterChanged();
}

bool TemplateSetterEditor::IsSetterEmpty(SetterNode* pSetter)
{
    TemplateRootItem* tempItem = suic::RTTICast<TemplateRootItem>(pSetter->GetResNode());
    return (tempItem == NULL || !tempItem->HasContent());
}

void TemplateSetterEditor::InitSetterNode(RTTIOfInfo* owner, DpProperty* dp, SetterNode* pSetter)
{
    TemplateRootItem* pTemplate = new TemplateRootItem();
    pTemplate->SetTargetType(owner);
    pTemplate->SetTemplateType(dp->GetPropType());
    pSetter->SetValue(pTemplate);
}

void TemplateSetterEditor::ResetSetter()
{
    SetterEditor::ResetSetter();
    /*suic::DpProperty* dp = GetProperty();
    suic::FrameworkElement* fe = GetTargetElement()->GetUIElement();
    if (NULL != fe)
    {
        Style* pStyle = fe->GetStyle();
        if (pStyle != NULL)
        {
            for (int i = 0; i < pStyle->GetSetters()->GetCount(); ++i)
            {
                Setter* pSetter = pStyle->GetSetters()->GetSetter(i);
                if (pSetter->GetProp() == dp)
                {
                    fe->SetValue(dp, pSetter->GetValue());
                    break;
                }
            }
        }
    }*/
}

TemplateRootItem* TemplateSetterEditor::CreateFrameworkTemplate(RTTIOfInfo* ownerRtti)
{
    return NULL;
}

void TemplateSetterEditor::OnMainReturnClick(suic::Object* sender)
{
    ThemeEditorWindow* themeWnd = (ThemeEditorWindow*)sender;
    TemplateRootItem* pTemp = themeWnd->GetTemplateNode();
    EditRootPanel* editPanel = FindEditRttoPanel(this);
    DesignElement* target = themeWnd->GetResourceElement();

    if (pTemp->HasContent())
    {
        SetterNode* pSetter = GetSetterNode(false);
        suic::Control* ctrl = RTTICast<suic::Control>(target->GetUIElement());

        if (NULL == pSetter)
        {
            pSetter = GetSetterNode(true);
        }
        pSetter->SetResNode(pTemp);
        ctrl->SetTemplate(NULL);
    }

    RefleshSetterToDesignUI();
    editPanel->UpdateDesignPanel(target);
}

void TemplateSetterEditor::OnEditClick()
{
    /*suic::Control* ctrl = NULL;
    DesignElement* target = GetEditTarget();
    if (NULL != target)
    {
        ctrl = RTTICast<suic::Control>(target->GetUIElement());
    }

    if (NULL != ctrl)
    {
        EditRootPanel* editPanel = FindEditRttoPanel(this);
        RTTIOfInfo* ownerRtti = ctrl->GetRTTIType();
        SetterNode* pSetter = GetSetterNode(false);
        TemplateRootItem* pTemp = RTTICast<TemplateRootItem>(GetSetterNode(true)->GetResNode());
        const String strPath = "/mpfuid;/resource/uidesign/layout/ThemeEditor.xaml";
        ThemeEditorWindow* themeWnd = new ThemeEditorWindow(editPanel->GetRootItem(), NULL);

        if (NULL == pTemp)
        {
            pTemp = CreateFrameworkTemplate(ownerRtti);
            if (NULL == pTemp)
            {
                return ;
            }
        }
        else
        {
            pTemp->ref();
        }

        themeWnd->ref();

        pTemp->SetTargetType(ownerRtti);
        themeWnd->SetTemplateNode(pTemp);
        themeWnd->SetResourceElement(target);
        themeWnd->ShowDialog(strPath);

        if (pTemp->HasContent())
        {
            if (NULL == pSetter)
            {
                pSetter = GetSetterNode(true);
            }
            pSetter->SetResNode(pTemp);
            ctrl->SetTemplate(NULL);
        }

        RefleshSetterToDesignUI();
        editPanel->UpdateDesignPanel(target);

        pTemp->unref();
        themeWnd->unref();
    }*/

    suic::Control* ctrl = NULL;
    DesignElement* target = GetEditTarget();
    if (NULL != target)
    {
        ctrl = RTTICast<suic::Control>(target->GetUIElement());
    }

    if (NULL != ctrl)
    {
        EditRootPanel* editPanel = FindEditRttoPanel(this);
        RTTIOfInfo* ownerRtti = ctrl->GetRTTIType();
        TemplateRootItem* pTemp = RTTICast<TemplateRootItem>(GetSetterNode(true)->GetResNode());
        ThemeEditorWindow* themeWnd = new ThemeEditorWindow(editPanel->GetRootItem(), NULL);

        if (NULL == pTemp)
        {
            pTemp = CreateFrameworkTemplate(ownerRtti);
            if (NULL == pTemp)
            {
                return ;
            }
        }
        else
        {
            pTemp->ref();
        }

        themeWnd->ref();

        pTemp->SetTargetType(ownerRtti);
        themeWnd->SetTemplateNode(pTemp);
        themeWnd->SetResourceElement(target);

        MainWindow* mainWnd = dynamic_cast<MainWindow*>(suic::Application::Current()->GetMainWindow());
        if (NULL != mainWnd)
        {
            suic::EventHandler h;
            GetMainReturnEvent(h);
            themeWnd->SetMainReturnEvent(h);
            mainWnd->SwitchToThemeView(themeWnd, String().Format(_U("编辑模板 - %s：%s"), 
                target->GetTypeName().c_str(), 
                pTemp->GetTemplateType()->typeName));
        }

        pTemp->unref();
        themeWnd->unref();
    }
}

//==============================================
// ControlTemplateSetterEditor

ControlTemplateSetterEditor::ControlTemplateSetterEditor()
{
}

void ControlTemplateSetterEditor::NotifySetterChanged()
{
    STSetterEditor::NotifySetterChanged();
}

TemplateRootItem* ControlTemplateSetterEditor::CreateFrameworkTemplate(RTTIOfInfo* ownerRtti)
{
    TemplateRootItem* pTemp = NULL;
    CreateStyleWindow tempWnd(false);
    const String strPath = "/mpfuid;/resource/uidesign/layout/Editor/CreateStyleWindow.xaml";

    tempWnd.setAutoDelete(false);

    if (0 == tempWnd.ShowDialog(strPath))
    {
        if (!tempWnd.IsFromBlank())
        {
            TemplateRootItem* tempNode = SystemResource::Ins()->FindControlTemplate(ownerRtti->typeName);
            if (NULL != tempNode)
            {
                ResNodePtr resNode;
                tempNode->CloneNode(resNode);
                pTemp = suic::RTTICast<TemplateRootItem>(resNode.get());
                pTemp->ref();
            }
        }

        if (NULL == pTemp)
        {
            pTemp = new TemplateRootItem();
            pTemp->ref();
        }

        pTemp->SetTemplateType(ControlTemplate::RTTIType());
    }

    return pTemp;
}

//==============================================
// DataTemplateSetterEditor

DataTemplateSetterEditor::DataTemplateSetterEditor()
{
}

void DataTemplateSetterEditor::NotifySetterChanged()
{
    STSetterEditor::NotifySetterChanged();
}

TemplateRootItem* DataTemplateSetterEditor::CreateFrameworkTemplate(RTTIOfInfo* ownerRtti)
{
    TemplateRootItem* pTemp = new TemplateRootItem();

    pTemp->ref();
    pTemp->SetTemplateType(DataTemplate::RTTIType());
    return pTemp;
}

//==============================================
// StyleSetterEditor

StyleSetterEditor::StyleSetterEditor()
{
}

void StyleSetterEditor::ResetSetter()
{
    STSetterEditor::ResetSetter();
}

void StyleSetterEditor::NotifySetterChanged()
{
    STSetterEditor::NotifySetterChanged();
}

bool StyleSetterEditor::IsSetterEmpty(SetterNode* pSetter)
{
    StyleNode* styleItem = suic::RTTICast<StyleNode>(pSetter->GetResNode());
    return (styleItem == NULL || !styleItem->HasContent());
}

void StyleSetterEditor::InitSetterNode(RTTIOfInfo* owner, DpProperty* dp, SetterNode* pSetter)
{
    StyleNode* pStyle = new StyleNode();
    pStyle->SetTargetType(owner);
    pSetter->SetValue(pStyle);
}

void StyleSetterEditor::SetTemplateMode(bool inTemplate)
{
    STSetterEditor::SetTemplateMode(inTemplate);
    if (NULL != GetEditButton())
    {
        // 来自于模版，隐藏编辑Style的按钮，不让编辑
        if (inTemplate)
        {
            GetEditButton()->SetVisibility(Visibility::Collapsed);
        }
        else
        {
            GetEditButton()->SetVisibility(Visibility::Visible);
        }
    }
}

StyleNode* StyleSetterEditor::CreateNewStyle(suic::RTTIOfInfo* targetRtti)
{
    StyleNode* pStyle = NULL;
    CreateStyleWindow styleWnd(true);
    const String strPath = "/mpfuid;/resource/uidesign/layout/Editor/CreateStyleWindow.xaml";

    styleWnd.setAutoDelete(false);

    if (0 == styleWnd.ShowDialog(strPath))
    {
        if (!styleWnd.IsFromBlank())
        {
            StyleNode* tempSty = SystemResource::Ins()->FindStyle(targetRtti->typeName);
            if (NULL != tempSty)
            {
                ResNodePtr resStyle;
                tempSty->CloneNode(resStyle);
                pStyle = suic::RTTICast<StyleNode>(resStyle.get());
                pStyle->ref();
            }
        }

        if (NULL == pStyle)
        {
            pStyle = new StyleNode();
            pStyle->ref();
        }

        pStyle->SetTargetType(targetRtti);
    }
    
    return pStyle;
}

suic::RTTIOfInfo* StyleSetterEditor::GetStyleTargetRTTIInfo()
{
    suic::RTTIOfInfo* rttiInfo = NULL;
    suic::FrameworkElement* fe = NULL;
    DesignElement* target = GetEditTarget();
    if (NULL != target)
    {
        fe = target->GetUIElement();
    }

    if (NULL != fe)
    {
        rttiInfo = fe->GetRTTIType();
    }

    return rttiInfo;
}

void StyleSetterEditor::OnMainReturnClick(suic::Object* sender)
{
    ThemeEditorWindow* themeWnd = (ThemeEditorWindow*)sender;
    StyleNode* pStyle = themeWnd->GetStyleNode();
    EditRootPanel* editPanel = FindEditRttoPanel(this);
    DesignElement* target = themeWnd->GetResourceElement();

    // 如果控件样式有内容，则设置到控件本身的资源集合
    if (pStyle->HasContent())
    {
        GetSetterNode(true)->SetResNode(pStyle);
    }
    else
    {
        GetSetterNode(true)->SetValue(NULL);
    }

    // 刷新控件的新样式资源
    RefleshSetterToDesignUI();

    if (NULL != editPanel)
    {
        editPanel->UpdateDesignPanel(target);
    }

    UpdateShow();
}

void StyleSetterEditor::OnEditClick()
{
    /*DesignElement* target = GetEditTarget();
    suic::RTTIOfInfo* rttiInfo = GetStyleTargetRTTIInfo();

    if (NULL != rttiInfo && NULL != Project::GetCurrentProject())
    {
        EditRootPanel* editPanel = FindEditRttoPanel(this);
        StyleNode* pStyle = RTTICast<StyleNode>(GetSetterNode(true)->GetResNode());
        const String strPath = "/mpfuid;/resource/uidesign/layout/ThemeEditor.xaml";
        ThemeEditorWindow* themeWnd = NULL;

        // 如果控件本身没有Style，则重新生成一个
        // Style是放在控件的.Resources资源节点下
        if (NULL == pStyle)
        {
            pStyle = CreateNewStyle(rttiInfo);
            if (NULL == pStyle)
            {
                return;
            }
        }
        else 
        {
            pStyle->ref();
        }

        themeWnd = new ThemeEditorWindow(editPanel->GetRootItem(), NULL);
        
        themeWnd->ref();
        themeWnd->SetStyleNode(pStyle);
        // 设置资源查询的目标元素
        // 对于模板来说，必须设置模板的模板父元素，系统会用其来查询资源
        themeWnd->SetResourceElement(target);

        // 打开控件样式编辑对话框，进行样式编辑
        themeWnd->ShowDialog(strPath);

        // 如果控件样式有内容，则设置到控件本身的资源集合
        if (pStyle->HasContent())
        {
            GetSetterNode(true)->SetResNode(pStyle);
        }
        else
        {
            GetSetterNode(true)->SetValue(NULL);
        }

        // 刷新控件的新样式资源
        RefleshSetterToDesignUI();

        editPanel->UpdateDesignPanel(target);
        UpdateShow();

        pStyle->unref();
        themeWnd->unref();
    }*/

    DesignElement* target = GetEditTarget();
    suic::RTTIOfInfo* rttiInfo = GetStyleTargetRTTIInfo();

    if (NULL != rttiInfo && NULL != Project::GetCurrentProject())
    {
        EditRootPanel* editPanel = FindEditRttoPanel(this);
        StyleNode* pStyle = RTTICast<StyleNode>(GetSetterNode(true)->GetResNode());
        const String strPath = "/mpfuid;/resource/uidesign/layout/ThemeEditor.xaml";
        ThemeEditorWindow* themeWnd = NULL;

        // 如果控件本身没有Style，则重新生成一个
        // Style是放在控件的.Resources资源节点下
        if (NULL == pStyle)
        {
            pStyle = CreateNewStyle(rttiInfo);
            if (NULL == pStyle)
            {
                return;
            }
        }
        else 
        {
            pStyle->ref();
        }

        themeWnd = new ThemeEditorWindow(editPanel->GetRootItem(), NULL);
        
        themeWnd->ref();
        themeWnd->SetStyleNode(pStyle);
        // 设置资源查询的目标元素
        // 对于模板来说，必须设置模板的模板父元素，系统会用其来查询资源
        themeWnd->SetResourceElement(target);

        MainWindow* mainWnd = dynamic_cast<MainWindow*>(suic::Application::Current()->GetMainWindow());
        if (NULL != mainWnd)
        {
            suic::EventHandler h;
            GetMainReturnEvent(h);
            themeWnd->SetMainReturnEvent(h);
            mainWnd->SwitchToThemeView(themeWnd, String().Format(_U("编辑样式 - %s"), target->GetTypeName().c_str()));
        }

        pStyle->unref();
        themeWnd->unref();
    }
}

ItemContainerStyleSetterEditor::ItemContainerStyleSetterEditor()
{

}

suic::RTTIOfInfo* ItemContainerStyleSetterEditor::GetStyleTargetRTTIInfo()
{
    suic::RTTIOfInfo* rttiInfo = NULL;
    suic::ItemsControl* fe = NULL;
    suic::Element* itemContainer = NULL;
    DesignElement* target = GetEditTarget();

    if (NULL != target)
    {
        fe = suic::RTTICast<suic::ItemsControl>(target->GetUIElement());
    }

    if (NULL != fe)
    {
        itemContainer = fe->GetContainerForItem(suic::DpProperty::UnsetValue());
        if (NULL != itemContainer)
        {
            rttiInfo = itemContainer->GetRTTIType();
            itemContainer->unref();
        }
    }

    return rttiInfo;
}

//==============================================
// SourceSetterEditor

SourceSetterEditor::SourceSetterEditor()
{
    _info = NULL;
}

void SourceSetterEditor::UpateSource()
{
    SetterNode* pSetter = GetSetterNode(false);
    if (NULL != pSetter && NULL != _info)
    {
        ImageSourceResNode* sourceRes = RTTICast<ImageSourceResNode>(pSetter->GetResNode());
        if (NULL != sourceRes)
        {
            _info->SetText(sourceRes->GetImageSource()->GetUri().ToString());
        }
    }
}

void SourceSetterEditor::NotifySetterChanged()
{
    SetterEditor::NotifySetterChanged();
    UpateSource();
}

void SourceSetterEditor::OnEditClick()
{
    EditRootPanel* pEditPanel = FindEditRttoPanel(this);

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
        SetterNode* pSetter = GetSetterNode(true);
        ImageSourceResNode* sourceRes = RTTICast<ImageSourceResNode>(pSetter->GetResNode());
        if (NULL == sourceRes)
        {
            sourceRes = new ImageSourceResNode(NULL);
            pSetter->SetResNode(sourceRes);
        }

        _info->SetText(imgSource);
        sourceRes->SetResPath(pEditPanel->GetResName(), imgSource);

        RefleshSetterToDesignUI();
    }
}

void SourceSetterEditor::OnSourceClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    OnEditClick();
    e->SetHandled(true);
}

void SourceSetterEditor::OnApplyTemplate()
{
    SetterEditor::OnApplyTemplate();

    suic::Button* pBtn = RTTICast<suic::Button>(GetTemplateChild(_U("PART_EditStyle")));
    if (NULL != pBtn)
    {
        pBtn->AddClick(new suic::ClickEventHandler(this, &SourceSetterEditor::OnSourceClick));
    }
    _info = RTTICast<suic::TextBox>(GetTemplateChild(_U("PART_BindingInfo")));
    UpateSource();
}

void SourceSetterEditor::OnSetterValueChanged()
{

}
