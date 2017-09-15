// uidframe.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <Core/ValueEditor.h>
#include <Main/ElementTree.h>
#include <Main/PropertyWindow.h>
#include <Main/ImageSelectorWindow.h>

#include <Core/TypeControls.h>
#include <Editor/EditRootPanel.h>
#include <Editor/ImageEditPanel.h>

#include <Extend/Controls/ToolBar.h>
#include <Extend/Controls/TabControl.h>
#include <Extend/Controls/GridSplitter.h>

#include <Controls/UserFrameworkElement.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

void InitControls()
{
    suic::Canvas::StaticInit();
    suic::PlayBox::StaticInit();
    suic::ListView::StaticInit();
    suic::ListBox::StaticInit();
    suic::TreeView::StaticInit();
    suic::TabControl::StaticInit();
    suic::Hyperlink::StaticInit();
    suic::GridSplitter::StaticInit();
    suic::ComboBox::StaticInit();
    suic::WrapPanel::StaticInit();
    suic::ToolBar::StaticInit();
    suic::TextBoxBase::StaticInit();
    suic::TextBox::StaticInit();
    suic::PasswordBox::StaticInit();
    suic::RichTextBox::StaticInit();
    suic::FlashBox::StaticInit();
}

void InitDControls()
{
    InitControls();

    DpManager::Ins();

    StringControl::StaticInit();
    StringEditor::StaticInit();
    EditorControl::StaticInit();
    CheckSetterEditor::StaticInit();
    ChoiceSetterEditor::StaticInit();
    AlignHoriSetterEditor::StaticInit();
    ButtonSetterEditor::StaticInit();
    
    suic::BuildFactory::Ins()->Add(ImageTreeViewItem::RTTIType());
    suic::BuildFactory::Ins()->Add(ImageTreeView::RTTIType());

    suic::BuildFactory::Ins()->Add(ButtonSetterEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(StringControl::RTTIType());
    suic::BuildFactory::Ins()->Add(IntegerControl::RTTIType());
    suic::BuildFactory::Ins()->Add(PosIntegerControl::RTTIType());

    suic::BuildFactory::Ins()->Add(WidthControl::RTTIType());
    suic::BuildFactory::Ins()->Add(WidthFloatControl::RTTIType());
    suic::BuildFactory::Ins()->Add(FloatControl::RTTIType());
    suic::BuildFactory::Ins()->Add(PosFloatControl::RTTIType());
    suic::BuildFactory::Ins()->Add(RatioControl::RTTIType());

    suic::BuildFactory::Ins()->Add(EditorControl::RTTIType());

    suic::BuildFactory::Ins()->Add(StringEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(WidthEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(WidthFloatEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(IntegerEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(PosIntegerEditor::RTTIType());
    
    suic::BuildFactory::Ins()->Add(FloatEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(PosFloatEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(RatioEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(BooleanEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(SizeEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(fSizeEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(PointEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(fPointEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(RectEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(fRectEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(ImageSourceEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(BrushEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(NullBrushEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(NullBrushSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(StyleEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(LayoutEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(TemplateEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(StringSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(FloatSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(PosFloatSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(RatioSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(NumericSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(PosNumericSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(WidthSetterEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(GridWidthControl::RTTIType());
    suic::BuildFactory::Ins()->Add(GridStarControl::RTTIType());
    suic::BuildFactory::Ins()->Add(GridFloatControl::RTTIType());

    suic::BuildFactory::Ins()->Add(RectSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(fRectSetterEditor::RTTIType());
    
    suic::BuildFactory::Ins()->Add(SizeSetterEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(ChoiceSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(CursorSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(CheckSetterEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(FontWeightSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(FontStyleSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(FontUnderlineSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(FontSizeSetterEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(AlignVertSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(AlignHoriSetterEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(SolidColorBrushEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(GradientBrushEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(LinearGradientBrushEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(RadialGradientBrushEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(ImageBrushEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(ImageBrushSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(SolidColorBrushSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(LinearGradientBrushSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(RadialGradientBrushSetterEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(BrushElement::RTTIType());
    suic::BuildFactory::Ins()->Add(BrushSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(BrushResSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(BrushEditorPanel::RTTIType());

    suic::BuildFactory::Ins()->Add(STSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(StyleSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(TemplateSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(DataTemplateSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(ControlTemplateSetterEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(ItemContainerStyleSetterEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(SourceSetterEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(ProjectTree::RTTIType());
    suic::BuildFactory::Ins()->Add(ElementTree::RTTIType());
    suic::BuildFactory::Ins()->Add(ElementTreeItem::RTTIType());
    suic::BuildFactory::Ins()->Add(TreeItemGrid::RTTIType());
    suic::BuildFactory::Ins()->Add(TreeItemDockPanel::RTTIType());
    
    suic::BuildFactory::Ins()->Add(NameEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(GradientStop::RTTIType());
    
    suic::BuildFactory::Ins()->Add(EditRootPanel::RTTIType());

    suic::BuildFactory::Ins()->Add(GridViewSetterEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(ExtensionSetterEditor::RTTIType());

    suic::BuildFactory::Ins()->Add(DImageGrid::RTTIType());
    suic::BuildFactory::Ins()->Add(ImageEditPanel::RTTIType());

    suic::BuildFactory::Ins()->Add(TextBoxRange::RTTIType());
    suic::BuildFactory::Ins()->Add(TextBoxRangeEditor::RTTIType());
    suic::BuildFactory::Ins()->Add(TextBoxRangeSetterEditor::RTTIType());
    
    ElementTreeItem::StaticInit();

    BrushSetterEditor::StaticInit();

    ColorChannel::StaticInit();
    ColorSlider::StaticInit();
    ColorSelector::StaticInit();
    ColorPicker::StaticInit();
    ColorButton::StaticInit();
    fPointEditor::StaticInit();

    suic::Control::FocusableProperty->OverrideMetadata(SetterEditor::RTTIType(), new suic::PropMetadata(Boolean::False));

    suic::BuildFactory::Ins()->Add(ColorSlider::RTTIType());
    suic::BuildFactory::Ins()->Add(ColorChannel::RTTIType());
    suic::BuildFactory::Ins()->Add(ColorPicker::RTTIType());
    suic::BuildFactory::Ins()->Add(ColorPickerBox::RTTIType());
    
    suic::BuildFactory::Ins()->Add(ColorSelector::RTTIType());
    suic::BuildFactory::Ins()->Add(ColorButton::RTTIType());

    suic::BuildFactory::Ins()->Add(UserFrameworkElement::RTTIType());
    
    BrushEditor::StaticInit();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    if (DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        //::InitializeComponents();
        //InitDControls();
    }

    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

