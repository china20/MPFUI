
#include <Framework/Controls/GroupItem.h>
#include <Framework/Presenter/ItemContainerGenerator.h>

namespace suic
{

ImplementRTTIOfClass(GroupItem, suic::ContentControl)

void GroupItem::StaticInit()
{
    static bool s_init = false;
    if (!s_init)
    {
        s_init = true;
        Element::StaticInit();
        Element::FocusableProperty->OverrideMetadata(GroupItem::RTTIType(), new PropMetadata(Boolean::False));
    }
}

GroupItem::GroupItem()
{
    _generator = NULL;
    WriteDoFlag(ViewFlags::IsGroupItemVF, true);
}

GroupItem::~GroupItem()
{
    if (NULL != _generator)
    {
        _generator->unref();
    }
}

ItemContainerGenerator* GroupItem::GetGenerator()
{
    return _generator;
}

void GroupItem::SetGenerator(ItemContainerGenerator* val)
{
    SETREFOBJ(_generator, val);
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
    }
}

void GroupItem::PrepareItemContainer(Object* item)
{
    if (GetGenerator() != NULL)
    {
        GroupStyle* groupStyle = NULL;
        Style* containerStyle = NULL;

        groupStyle = GetGenerator()->GetParent()->GetGroupStyle();

        if (NULL == groupStyle)
        {
            groupStyle = GroupStyle::Default();
        }

        containerStyle = groupStyle->GetContainerStyle();

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

        SetContent(item);

        if (NULL != groupStyle->GetHeaderTemplate())
        {
            SetContentTemplate(groupStyle->GetHeaderTemplate());
        }

        if (NULL != groupStyle->GetHeaderTemplateSelector())
        {
            SetContentTemplateSelector(groupStyle->GetHeaderTemplateSelector());
        }
    }
}

}
