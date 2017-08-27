
#include <Framework/Controls/ViewGroup.h>
#include <Framework/Controls/VisualizeStackPanel.h>

namespace suic
{

ImplementRTTIOfClass(GroupStyle, suic::DpObject)
ImplementRTTIOfClass(GroupStyleSelector, suic::DpObject)
ImplementRTTIOfClass(ViewGroup, suic::NotifyPropChanged)

GroupStyle* GroupStyle::s_DefaultGroupStyle = new GroupStyle();
ItemsPanelTemplate* GroupStyle::DefaultGroupPanel;

ViewGroup::ViewGroup()
{
    _expanded = false;
    _items = new ObservableCollection();
    _items->ref();
}

ViewGroup::~ViewGroup()
{
    _items->unref();
}

GroupStyle::GroupStyle()
    : _containerStyle(NULL)
    , _panel(NULL)
    , _headerTemplate(NULL)
    , _containerStyleSelector(NULL)
    , _headerTemplateSelector(NULL)
{
    _hidesIfEmpty = false;
}

GroupStyle::~GroupStyle()
{
}

GroupStyle* GroupStyle::Default()
{
    if (NULL == s_DefaultGroupStyle)
    {
        s_DefaultGroupStyle = new GroupStyle();
        s_DefaultGroupStyle->ref();
    }
    return s_DefaultGroupStyle;
}

ItemsPanelTemplate* GroupStyle::GetPanel()
{
    if (NULL == DefaultGroupPanel)
    {
        DefaultGroupPanel = new ItemsPanelTemplate(new FEFactory(VirtualizingStackPanel::RTTIType()));
        DefaultGroupPanel->ref();
        DefaultGroupPanel->Seal();
    }

    if (NULL == _panel)
    {
        return DefaultGroupPanel;
    }
    return _panel;
}

void GroupStyle::SetPanel(ItemsPanelTemplate* val)
{
    _panel = val;
}

void GroupStyle::SetContainerStyle(Style* val)
{
    _containerStyle = val;
}

void GroupStyle::SetContainerStyleSelector(StyleSelector* val)
{
    _containerStyleSelector = val;
}

void GroupStyle::SetHeaderTemplate(DataTemplate* val)
{
    _headerTemplate = val;
}

void GroupStyle::SetHeaderTemplateSelector(DataTemplateSelector* val)
{
    _headerTemplateSelector = val;
}

}
