
#include <Framework/Controls/HierarchicalDataTemplate.h>

namespace suic
{

ImplementRTTIOfClass(HierarchicalDataTemplate, DataTemplate)

HierarchicalDataTemplate::HierarchicalDataTemplate()
    : _itemTemplate(NULL)
    , _itemTempSelector(NULL)
{
    InitDefaultValue();
}

HierarchicalDataTemplate::HierarchicalDataTemplate(RTTIOfInfo* targetType)
    : DataTemplate(targetType)
    , _itemTemplate(NULL)
    , _itemTempSelector(NULL)
{
    InitDefaultValue();
}

HierarchicalDataTemplate::~HierarchicalDataTemplate()
{

}

void HierarchicalDataTemplate::SetItemSource(Binding* val)
{
    SETREFOBJ(_itemSource, val);
}

void HierarchicalDataTemplate::SetItemTemplate(DataTemplate* val)
{
    SETREFOBJ(_itemTemplate, val);
}

void HierarchicalDataTemplate::SetItemTemplateSelector(DataTemplateSelector* val)
{
    SETREFOBJ(_itemTempSelector, val);
}

void HierarchicalDataTemplate::InitDefaultValue()
{
    _itemSource = new Binding(_U("ItemSource"));
    _itemSource->ref();
}

}
