
#include <Framework/Controls/Button.h>
#include <Framework/Controls/Window.h>
#include <Framework/Controls/TextBlock.h>

#include <System/Tools/HwndHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>

#include <System/Windows/CoreTool.h>
#include <System/Windows/Binding.h>

namespace suic
{

ImplementRTTIOfClass(Button, ButtonBase)

DpProperty* Button::IsCancelProperty;
DpProperty* Button::IsDefaultedProperty;
DpProperty* Button::IsDefaultProperty;

Button::Button()
{
    _animationType = 1;
}

Button::~Button()
{
}

void Button::OnIsDefaultedPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Button* btn = DynamicCast<Button>(d);
    if (btn != NULL)
    {
        if (e->GetNewValue()->ToInt() != 0)
        {
            btn->Focus();
        }
    }
}

static DataTemplate* CreateContentTemplate(RTTIOfInfo* rttiInfo)
{
    DataTemplate* temp = new DataTemplate();
    FEFactory* root = new FEFactory(TextBlock::RTTIType(), _U("TextBlock"));
    Binding* bind = new Binding();

    suic::TextBlock::StaticInit();

    root->SetValue(TextBlock::TextProperty, bind);
    root->SetValue(TextBlock::TextAlignmentProperty, Integer::GetPosInt(TextAlignment::tCenter));
    root->SetValue(TextBlock::HorizontalAlignmentProperty, HoriAlignBox::LeftBox);
    root->SetValue(TextBlock::VerticalAlignmentProperty, VertAlignBox::CenterBox);        

    temp->SetVisualTree(root);
    //temp->SetTargetType(rttiInfo);

    return temp;
}

void Button::StaticInit()
{
    if (NULL == IsDefaultedProperty)
    {
        ButtonBase::StaticInit();

        IsDefaultedProperty = DpProperty::Register(_T("IsDefaulted"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone, &Button::OnIsDefaultedPropChanged));
        IsDefaultProperty = DpProperty::Register(_T("IsDefault"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        ContentTemplateProperty->OverrideMetadata(RTTIType(), new PropMetadata(CreateContentTemplate(RTTIType())));
    }
}

void Button::OnClick()
{
    ButtonBase::OnClick();
}

void Button::OnDataContextChanged(DpPropChangedEventArg* e)
{
    ButtonBase::OnDataContextChanged(e);
}

//===========================================================
//
ImplementRTTIOfClass(SysButton, ButtonBase)
ImplementRTTIOfClass(MinimizeButton, SysButton)
ImplementRTTIOfClass(MaximizeButton, SysButton)
ImplementRTTIOfClass(CloseButton, SysButton)

//-------------------------------------------
MinimizeButton::MinimizeButton()
{
    _animationType = 1;
}

MinimizeButton::~MinimizeButton()
{
    ;
}

void MinimizeButton::OnClick()
{
    HwndHelper::MinimizeWindow(this);
}

//-------------------------------------------
MaximizeButton::MaximizeButton()
{
}

MaximizeButton::~MaximizeButton()
{
    ;
}

DpProperty* MaximizeButton::WindowStateProperty;

/*void MaximizeButton::OnWindowStateChanged(Object* sender, EventArg* e)
{
    Window* pWnd = RTTICast<Window>(sender);
    if (pWnd)
    {
        SetValue(WindowStateProperty, pWnd->GetValue(Window::WindowStateProperty));
    }
}*/

void MaximizeButton::StaticInit()
{
    if (NULL == WindowStateProperty)
    {
        WindowStateProperty = DpProperty::Register(_T("WindowState"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(WindowStateBox::NormalBox, PropMetadataOptions::AffectsRender));
        WindowStateProperty->SetConvertValueCb(WindowStateConvert::Convert);
        FocusableProperty->OverrideMetadata(RTTIType(), new PropMetadata(Boolean::False));
    }
}

SysButton::SysButton()
{
}

SysButton::~SysButton()
{

}

void SysButton::StaticInit()
{               
}

void MaximizeButton::OnInitialized(EventArg* e)
{
    SysButton::OnInitialized(e);
    Binding* binding = new Binding();
    RelativeSource* obj = new RelativeSource(RelativeSourceMode::FindAncestor);
    obj->AncestorType = Window::RTTIType();
    PropertyPath ppath;
    ppath.Path = _U("WindowState");
    binding->SetPath(ppath);
    binding->SetSourceRef(obj);
    SetBinding(WindowStateProperty, binding);
}

void MaximizeButton::OnClick()
{
    Window* pWnd = RTTICast<Window>(VisualTreeOp::GetVisualRoot(this));
    if (NULL != pWnd && pWnd->GetWindowStyle() != WindowStyle::wsNone && !pWnd->AllowsFullScreen())
    {
        if (HwndHelper::IsWindowMaximize(pWnd))
        {
            HwndHelper::RestoreWindow(pWnd);
        }
        else
        {
            HwndHelper::MaximizeWindow(pWnd);
        }
    }
}

//-------------------------------------------
CloseButton::CloseButton()
{
    _animationType = 1;
}

CloseButton::~CloseButton()
{
}

void CloseButton::OnClick()
{
    HwndHelper::CloseWindow(this, true);
}

}
