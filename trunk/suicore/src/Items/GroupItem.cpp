
#include <Framework/Items/GroupItem.h>
#include <Framework/Items/ItemContainerGeneratorEx.h>

namespace suic
{

ImplementRTTIOfClass(GroupItem, suic::ContentControl)

bool GroupItem::StaticInit()
{
    Element::FocusableProperty->OverrideMetadata(GroupItem::RTTIType(), new PropMetadata(Boolean::False));
    return true;
}

GroupItem::GroupItem()
{
    _generator = NULL;
}

GroupItem::~GroupItem()
{
    if (NULL != _generator)
    {
        delete _generator;
    }
}

ItemContainerGeneratorEx* GroupItem::GetGenerator()
{
    return _generator;
}

void GroupItem::SetGenerator(ItemContainerGeneratorEx* val)
{
    _generator = val;
}

void GroupItem::ClearContainerForItem(Object* item)
{
    if (GetGenerator() != NULL)
    {
        GroupStyle* groupStyle = GetGenerator()->GetParent()->GetGroupStyle();
        if (item->Equals(GetContent()))
        {
            ClearValue(ContentControl::ContentProperty);
        }
        if (GetContentTemplate() == groupStyle->GetHeaderTemplate())
        {
            ClearValue(ContentControl::ContentTemplateProperty);
        }
        if (GetContentTemplateSelector() == groupStyle->GetHeaderTemplateSelector())
        {
            ClearValue(ContentControl::ContentTemplateSelectorProperty);
        }
        _generator = NULL;
    }
}

void GroupItem::PrepareItemContainer(Object* item)
{
    if (GetGenerator() != NULL)
    {
        GroupStyle* groupStyle = GetGenerator()->GetParent()->GetGroupStyle();
        Style* containerStyle = groupStyle->GetContainerStyle();

        if ((containerStyle == NULL) && (groupStyle->GetContainerStyleSelector() != NULL))
        {
            containerStyle = groupStyle->GetContainerStyleSelector()->SelectStyle(item, this);
        }

        if (containerStyle != NULL)
        {
            if (!containerStyle->GetTargetType()->InheritFrom(RTTIType()))
            {
                throw InvalidOperationException(_U("StyleForWrongType"), __FILELINE__);
            }

            SetStyle(containerStyle);
            WriteInnerFlag(InnerFlags::IsStyleSetFromGenerator, true);
        }

        if (!HasNonDefaultValue(ContentControl::ContentProperty))
        {
            SetContent(item);
        }

        if (!HasNonDefaultValue(ContentControl::ContentTemplateProperty))
        {
            SetContentTemplate(groupStyle->GetHeaderTemplate());
        }

        if (!HasNonDefaultValue(ContentControl::ContentTemplateSelectorProperty))
        {
            SetContentTemplateSelector(groupStyle->GetHeaderTemplateSelector());
        }
    }
}

}
