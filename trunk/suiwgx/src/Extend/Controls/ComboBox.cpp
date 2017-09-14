/////////////////////////////////////////////////////////////////////////
// ComboBox.cpp

#include <Extend/Controls/ComboBox.h>
#include <Extend/Controls/ComboBoxItem.h>
#include <Framework/Controls/Popup.h>

#include <System/Windows/CoreTool.h>
#include <System/Tools/HwndHelper.h>
#include <System/Tools/VisualTreeOp.h>

#include <System/Input/Mouse.h>
#include <System/Input/KeyboardNavigation.h>

namespace suic
{

ImplementRTTIOfClass(ComboBox, Selector)

DpProperty* ComboBox::TextProperty;
DpProperty* ComboBox::IsEditableProperty;
DpProperty* ComboBox::IsReadOnlyProperty;
DpProperty* ComboBox::IsDropDownOpenProperty;
DpProperty* ComboBox::MaxDropDownHeightProperty;
DpProperty* ComboBox::SelectionBoxItemProperty;
DpProperty* ComboBox::SelectionBoxItemTemplateProperty;

void ComboBoxItem::OnMouseMove(MouseButtonEventArg* e)
{
    ListBoxItem::OnMouseMove(e);

    ComboBox* pParent = RTTICast<ComboBox>(GetParent());

    if (pParent)
    {
        if (MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress)
        {
            pParent->SetCaptureMouse();
        }
    }
}

void ComboBoxItem::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    ListBoxItem::OnMouseLeftButtonUp(e);
    ComboBox* pParent = RTTICast<ComboBox>(GetParent());

    if (pParent)
    {
        pParent->OnMouseLeftButtonUp(e);
    }

    if (IsEnabled()/*!GetItemEntry()->HasFlag(ItemEntry::fDisabled)*/)
    {
        SetSelectedFlag(ListBoxItem::sfFromMouse, true);
        SetValue(IsSelectedProperty, Boolean::True);
        SetSelectedFlag(ListBoxItem::sfFromMouse, false);
        HwndHelper::CloseWindow(this, true);
    }
}

//================================================================
// DropDownEventArg
//
DropDownEventArg::DropDownEventArg(PopupPtr popup)
    : _popup(popup)
{
    ;
}

PopupPtr DropDownEventArg::GetPopup()
{
    return _popup;
}

//////////////////////////////////////////////////////////
// ComboBox
//
ComboBox::ComboBox()
{
    _updatingText = false;
    _textInput = NULL;
    _dropDownPopup = NULL;
    ComboBox::StaticInit();
}

ComboBox::~ComboBox()
{
}

void ComboBox::OnIsDropDownOpenChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ComboBox* box = RTTICast<ComboBox>(d);
    bool bIsOpen = e->GetNewValue()->ToBool();

    if (NULL != box && box->_dropDownPopup)
    {
        if (box->GetScrollHost())
        {
            Object* selItem = box->GetSelectedItem();
            if (NULL == DpProperty::UnsetValue())
            {
                box->GetScrollHost()->ScrollToVerticalOffset(0);
            }
            else
            {
                ItemNavigateArg e(MouseDevice::Current(), ModifierKey::mkNone);
                box->NavigateToItem(selItem, -1, 0, &e, true);
            }
        }

        if (bIsOpen)
        {
            int iMinWidth = SystemParameters::TransformXFromDevice(box->GetRenderSize().cx) + 0.5;
            box->_dropDownPopup->SetMinWidth(iMinWidth);
            box->_dropDownPopup->SetMaxHeight(box->GetDropDownHeight());

            Assigner::Current()->PushTrackPopup(box->_dropDownPopup);
            box->GetAssigner()->GetTopAssignerFrame()->RegisterHook(box, MessageHook(box, &ComboBox::OnMessageFilter));

            DropDownEventArg eo(box->_dropDownPopup);

            box->OnDropDownOpened(&eo);
            box->UpdateVisualState(false);
        }
        else
        {
            box->GetAssigner()->GetTopAssignerFrame()->RemoveHookFromObject(box);
            Assigner::Current()->RemoveTrackPopup(box->_dropDownPopup);

            DropDownEventArg eo(box->_dropDownPopup);
            box->OnDropDownClosed(&eo);
        }
    }
}

void ComboBox::OnIsEditableChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ComboBox* box = RTTICast<ComboBox>(d);

    if (NULL != box)
    {
        box->Update(false);
        box->UpdateVisualState(false);
    }
}

void ComboBox::OnTextPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ComboBox* box = RTTICast<ComboBox>(d);
    if (NULL != box && !box->_updatingText)
    {
        OString* pText = RTTICast<OString>(e->GetNewValue());

        if (box->IsEditable())
        {
            box->Update(true);
        }
        else if (NULL != pText)
        {
            pText->SetString(e->GetOldValue()->ToString());
        }
    }
}

void ComboBox::StaticInit()
{
    if (NULL == TextProperty)
    {
        Selector::StaticInit();
        ComboBoxItem::StaticInit();

        Element::IsMouseOverProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, 0, &OnVisualStatePropChanged));

        TextProperty  = DpProperty::Register(_T("Text"), RTTIType(), OString::RTTIType()
            , DpPropMemory::GetPropMeta(OString::EmptyString, PropMetadataOptions::AffectsArrange, &ComboBox::OnTextPropChanged));
        IsEditableProperty = DpProperty::Register(_T("IsEditable"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsArrange, &ComboBox::OnIsEditableChanged));
        IsReadOnlyProperty = DpProperty::Register(_T("IsReadOnly"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        IsDropDownOpenProperty = DpProperty::Register(_T("IsDropDownOpen"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone, &ComboBox::OnIsDropDownOpenChanged));
        MaxDropDownHeightProperty = DpProperty::Register(_T("MaxDropDownHeight"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(80), PropMetadataOptions::AffectsNone));
        SelectionBoxItemProperty = DpProperty::Register(_T("SelectionBoxItem"), RTTIType(), Object::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsParentArrange));
        SelectionBoxItemTemplateProperty = DpProperty::Register(_T("SelectionBoxItemTemplate"), RTTIType(), DataTemplate::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsParentArrange));

        KeyboardNavigation::TabNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knLocal)));
        KeyboardNavigation::ControlTabNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knNone)));
        KeyboardNavigation::DirectionalNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knNone)));
    }
}

bool ComboBox::OnMessageFilter(Object* sender, MessageParam* mp, bool& interrupt)
{
    ComboBox* cmb = RTTICast<ComboBox>(_dropDownPopup->GetTemplatedParent());

    if (NULL == cmb)
    {
        return false;
    }

    if (_dropDownPopup->GetPopupRoot() != sender)
    {
        if (mp->message == WM_MOUSEMOVE && !PointTransform::PointInElement(cmb, NULL))
        {
            return true;
        }
    }
    else
    {
        interrupt = true;
        return false;
    }

    if (mp->message == WM_KEYDOWN)
    {
        bool bInterrupt = true;
        int index = GetSelectedIndex();
        if (index < 0)
        {
            index = -1;
        }

        switch ((int)mp->wp)
        {
        case Key::kUp:
            if (index > 0)
            {
                SetSelectedIndex(--index);
                _dropDownPopup->UpdateLayout();
            }
            break;

        case Key::kDown:
            if (index < GetCount() - 1)
            {
                SetSelectedIndex(++index);
                _dropDownPopup->UpdateLayout();
            }
            break;

        case Key::kEscape:
            _dropDownPopup->AsyncClose();
            break;

        case Key::kReturn:
            _dropDownPopup->AsyncClose();
            break;

        default:
            bInterrupt  = false;
        }

        interrupt = bInterrupt;

        if (bInterrupt)
        {
            return bInterrupt;
        }
    }

    return _dropDownPopup->OnSysFilterMessage(sender, mp, interrupt);
}

void ComboBox::Dispose()
{
    if (_dropDownPopup)
    {
		_dropDownPopup->unref();
        _dropDownPopup = NULL;
    }

    Selector::Dispose();
}

bool ComboBox::IsItemItsOwnContainer(Object* item)
{
    return (ComboBoxItem::RTTIType() == item->GetRTTIType());
}

Element* ComboBox::GetContainerForItemOverride()
{
    return new ComboBoxItem();
}

int ComboBox::FindIndex(const String& text)
{
    for (int i = 0; i < GetCount(); ++i)
    {
        String strItem = GetItem(i)->ToString();
        if (strItem.Equals(text))
        {
            return i;
        }
    }
    return -1;
}

int ComboBox::FindIndex(Object* item)
{
    for (int i = 0; i < GetCount(); ++i)
    {
        Object* cmbItem = GetItem(i);
        if (item->Equals(cmbItem))
        {
            return i;
        }
    }
    return -1;
}

void ComboBox::SetReadOnly(bool bOnly)
{
    SetValue(IsReadOnlyProperty, BOOLTOBOOLEAN(bOnly));
}

bool ComboBox::IsReadOnly()
{
    return GetValue(IsReadOnlyProperty)->ToBool();
}

void ComboBox::SetEditable(bool bOnly)
{
    SetValue(IsEditableProperty, BOOLTOBOOLEAN(bOnly));
}

bool ComboBox::IsEditable()
{
    return GetValue(IsEditableProperty)->ToBool();
}

bool ComboBox::IsDropDownOpen()
{
    return GetValue(IsDropDownOpenProperty)->ToBool();
}

void ComboBox::SetIsDropDownOpen(bool val)
{
    SetValue(IsDropDownOpenProperty, BOOLTOBOOLEAN(val));
}

void ComboBox::SetDropDownHeight(int height)
{
    SetValue(MaxDropDownHeightProperty, new Integer(height));
}

void ComboBox::SetText(const String& text)
{
    SetValue(TextProperty, new OString(text));
}

TextEditor* ComboBox::GetTextBoxSite() const
{
    return _textInput;
}

void ComboBox::Update(bool bFocus)
{
    if (IsEditable())
    {
        if (!_updatingText)
        {
            _updatingText = true;

            String text = GetText();

            if (_textInput != NULL)
            {
                _textInput->SetText(text);
                if (bFocus)
                {
                    _textInput->Focus();
                }
            }

            _updatingText = false;
        }
    }
    else
    {
        Object* selectedItem = GetSelectedItem();
        DataTemplate* itemTemplate = GetItemTemplate();
        ContentControl* control = RTTICast<ContentControl>(selectedItem);

        if (control != NULL)
        {
            selectedItem = control->GetContent();
            itemTemplate = control->GetContentTemplate();
        }

        if (selectedItem == NULL || selectedItem == DpProperty::UnsetValue())
        {
            selectedItem = OString::EmptyString;
        }

        SetSelectionBoxItem(selectedItem);
        SetSelectionBoxItemTemplate(itemTemplate);

        UpdateLayout();
    }
}

String ComboBox::GetText()
{
    String strText;
    strText = GetValue(TextProperty)->ToString();
    return strText;
}

bool ComboBox::StaysOpenOnEdit()
{
    return _dropDownPopup->IsOpen();
}

void ComboBox::OnTextSiteTextChanged(DpObject* sender, RoutedEventArg* e)
{
    SetCurrentValue(TextProperty, _textInput->GetValue(TextEditor::TextProperty));
}

void ComboBox::OnApplyTemplate()
{
    Selector::OnApplyTemplate();

    _textInput = RTTICast<TextEditor>(GetTemplateChild("PART_TextBox"));
    _dropDownPopup = RTTICast<Popup>(GetTemplateChild("PART_Popup"));

    if (_dropDownPopup)
    {
        _dropDownPopup->SetPlacementTarget(this);
    }

    if (_textInput)
    {
        _textInput->AddTextChanged(new RoutedEventHandler(this, &ComboBox::OnTextSiteTextChanged));
    }

    Update(false);
}

bool ComboBox::BlockRouteEvent()
{
    return true;
}

//////////////////////////////////////////////////////////////////////
// ComboBox

void ComboBox::OnKeyDown(KeyboardEventArg* e)
{
    if (e->GetKey() == Key::kReturn)
    {
        if (IsEditable() && _textInput)
        {
            _textInput->Focus();
        }
    }
    else if (e->GetKey() == Key::kDown)
    {
        if (_dropDownPopup != NULL)
        {
            SetIsDropDownOpen(true);
        }
    }
    else if (e->GetKey() == Key::kEscape && _dropDownPopup)
    {
        _dropDownPopup->Close();
        e->SetHandled(true);
    }
}

void ComboBox::OnTextInput(KeyboardEventArg* e)
{
    if (IsReadOnly())
    {
        e->SetHandled(true);
    }
}

void ComboBox::OnGotFocus(RoutedEventArg* e)
{
    Selector::OnGotFocus(e);
    if (IsEditable() && NULL != _textInput)
    {
        _textInput->Focus();
    }
}

void ComboBox::OnTextBoxCursor(Element*, CursorEventArg* e)
{
    if (IsReadOnly())
    {
        e->SetHandled(true);
    }
}

void ComboBox::OnSelectionChanged(SelectionChangedEventArg* e)
{
    Selector::OnSelectionChanged(e);

    if (!_updatingText)
    {
        Object* selItem = GetSelectedItem();
        _updatingText = true;
        SetText(selItem->ToString());
        _updatingText = false;

        Update(true);

        if (IsEditable())
        {
            _updatingText = true;
            if (_selectedItems.GetCount() > 0)
            {
                //SetSelectedHelper(_selectedItems.GetItem(0), false);
            }
            _updatingText = false;
        }
    }
}

void ComboBox::OnDropDownClosed(DropDownEventArg* e)
{
    if (DropDownClosed)
    {
        DropDownClosed(this, e);
    }

    if (IsEditable() && _textInput)
    {
        _textInput->Focus();
    }
    else
    {
        Focus();
    }
}

void ComboBox::OnDropDownOpened(DropDownEventArg* e)
{
    if (DropDownOpened)
    {
        DropDownOpened(this, e);
    }
}

void ComboBox::OnPreviewSetCursor(CursorEventArg* e)
{
    if (IsReadOnly())
    {
        e->SetHandled(true);
    }
}

void ComboBox::OnPreviewMouseLeftButtonDown(MouseButtonEventArg* e)
{
    Selector::OnPreviewMouseLeftButtonDown(e);
}

void ComboBox::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    Selector::OnMouseLeftButtonDown(e);

    e->SetHandled(true);
    Focus();

    if (NULL == _dropDownPopup)
    {
        return ;
    }

	// 下来列表已经打开，先关闭
    if (IsDropDownOpen())
    {
        SetIsDropDownOpen(false);
    }
    else
    {
        SetIsDropDownOpen(true);
    }
}

void ComboBox::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    Selector::OnMouseLeftButtonUp(e);

    e->SetHandled(true);
    InvalidateVisual();
}

void ComboBox::OnPreviewMouseMove(MouseButtonEventArg* e)
{
    if (MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress)
    {
    }
}

}
