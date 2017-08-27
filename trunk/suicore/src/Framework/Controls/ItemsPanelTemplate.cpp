
#include <Framework/Presenter/ItemsPresenter.h>
#include <Framework/Controls/ItemsPanelTemplate.h>
#include <System/Windows/FrameworkElementFactory.h>
#include <Framework/Controls/Panel.h>

namespace suic
{

ImplementRTTIOfClass(ItemsPanelTemplate, FrameworkTemplate)

ItemsPanelTemplate::ItemsPanelTemplate()
{
    SetTargetType(ItemsPresenter::RTTIType());
}

ItemsPanelTemplate::ItemsPanelTemplate(FEFactory* root)
{
    SetVisualTree(root);
}

ItemsPanelTemplate::~ItemsPanelTemplate()
{
}

RTTIOfInfo* ItemsPanelTemplate::GetTargetTypeInternal()
{
    return ItemsPresenter::RTTIType();
}

void ItemsPanelTemplate::ProcessTemplateBeforeSeal()
{
    FEFactory* visualTree = GetVisualTree();

    if (visualTree != NULL)
    {
        visualTree->SetValue(Panel::IsItemsHostProperty, Boolean::True);
    }
}

void ItemsPanelTemplate::ValidateTemplatedParent(FrameworkElement* templatedParent)
{
    if (templatedParent == NULL)
    {
        throw ArgumentNullException(_U("templatedParent"), __FILELINE__);
    }
    if (templatedParent->GetRTTIType() != ItemsPresenter::RTTIType())
    {
        throw ArgumentException(_U("TemplateTargetTypeMismatch"), __FILELINE__);
    }
}

}

