
#include <Framework/Controls/HeaderedItemsControl.h>
#include <Framework/Controls/HierarchicalItem.h>

#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>

#include <System/Windows/MemberVisitor.h>

namespace suic
{

ImplementRTTIOfClass(HeaderedItemsControl, ItemsControl)

DpProperty* HeaderedItemsControl::HasHeaderProperty;
DpProperty* HeaderedItemsControl::HeaderProperty;
DpProperty* HeaderedItemsControl::HeaderTemplateProperty;
DpProperty* HeaderedItemsControl::HeaderTemplateSelectorProperty;

HeaderedItemsControl::HeaderedItemsControl()
    : _headerElement(NULL)
{
    _header = DpProperty::UnsetValue();
}

HeaderedItemsControl::~HeaderedItemsControl()
{
}

void HeaderedItemsControl::StaticInit()
{
    if (NULL == HasHeaderProperty)
    {
        ItemsControl::StaticInit();

        HasHeaderProperty = DpProperty::Register(_U("HasHeader"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        HeaderProperty = DpProperty::Register(_U("Header"), RTTIType(), Object::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &HeaderedItemsControl::OnHeaderPropChanged));
        HeaderTemplateProperty = DpProperty::Register(_U("HeaderTemplate"), RTTIType(), DataTemplate::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &HeaderedItemsControl::OnHeaderTemplatePropChanged));
        HeaderTemplateSelectorProperty = DpProperty::Register(_U("HeaderTemplateSelector"), RTTIType(), Object::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
    }
}

void HeaderedItemsControl::OnHasHeaderPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    HeaderedItemsControl* elem = DynamicCast<HeaderedItemsControl>(d);
    if (NULL != elem)
    {
        elem->WriteItemFlags(HasHeaderFlag, e->GetNewValue()->ToBool());
    }
}

void HeaderedItemsControl::OnHeaderPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    HeaderedItemsControl* elem = DynamicCast<HeaderedItemsControl>(d);
    if (NULL != elem)
    {
        elem->_header = e->GetNewValue();
        elem->SetValue(HasHeaderProperty, (elem->_header != DpProperty::UnsetValue()) ? Boolean::True : Boolean::False);
        elem->OnHeaderChanged(e->GetOldValue(), e->GetNewValue());
    }
}

void HeaderedItemsControl::OnHeaderTemplatePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    HeaderedItemsControl* elem = RTTICast<HeaderedItemsControl>(d);
    DataTemplate* oldTemp = RTTICast<DataTemplate>(e->GetOldValue());
    DataTemplate* newTemp = RTTICast<DataTemplate>(e->GetNewValue());

    if (NULL != elem)
    {
        elem->OnHeaderTemplateChanged(oldTemp, newTemp);
    }
}

void HeaderedItemsControl::OnApplyTemplate()
{
    ItemsControl::OnApplyTemplate();
    _headerElement = GetTemplateChild("PART_Header");
}

void HeaderedItemsControl::SetHeaderIsLogical(bool val)
{
    WriteItemFlags(Control::ItemBoolFlags::HeaderIsNotLogical, !val);
}

void HeaderedItemsControl::SetHeaderIsItem(bool val)
{
    WriteItemFlags(Control::ItemBoolFlags::HeaderEqualItem, val);
}

int HeaderedItemsControl::GetHeaderSize(bool isHorizontal)
{
    if (_headerElement != NULL)
    {
        if (isHorizontal)
        {
            return _headerElement->GetDesiredSize().cx;
        }
        else
        {
            return _headerElement->GetDesiredSize().cy;
        }
    }

    return DEFAULTITEMSIZE;
}

void HeaderedItemsControl::ClearHeaderedItemsControl(ItemEntry* item)
{
    ClearItemsControl(item);

    if ((item->GetItem() != this) && HeaderIsItem())
    {
        SetHeader(NULL);
    }

    if (!HeaderIsLogical())
    {
        SetItemsSource(NULL);
    }

    SetValue(HasItemsProperty, Boolean::False);
}

void HeaderedItemsControl::PrepareHeaderedItemsControl(ItemEntry* item, ItemsControl* parentItemsControl)
{
    bool set = item->GetItem() != this;
    SetHeaderIsLogical(!set);

    DataTemplate* itemTemplate = NULL;
    DataTemplateSelector* itemTemplateSelector = parentItemsControl->GetItemTemplateSelector();

    // 如果设置了模版选择器，设置到子元素
    if (itemTemplateSelector != NULL)
    {
        SetItemTemplateSelector(itemTemplateSelector);
        itemTemplate = itemTemplateSelector->SelectTemplate(item->GetItem(), this);
    }

    if (NULL == itemTemplate)
    {
        itemTemplate = parentItemsControl->GetItemTemplate();
    }

    if (itemTemplate != GetItemTemplate())
    {
        SetItemTemplate(itemTemplate);
        SetValue(HeaderTemplateProperty, itemTemplate);
    }

    Style* itemContainerStyle = parentItemsControl->GetItemContainerStyle();

    if (itemContainerStyle != NULL)
    {
        SetValue(ItemContainerStyleProperty, itemContainerStyle);
    }

    if (set)
    {
        if (HeaderIsItem() || GetHeader() == DpProperty::UnsetValue())
        {
            SetHeader(item->GetItem());
            SetHeaderIsItem(true);
        }

        PrepareHierarchy(item, parentItemsControl);
    }
    else
    {
        SetHeaderIsLogical(set);
        SetValue(HasItemsProperty, BOOLTOBOOLEAN(GetItemsSource()->GetCount() > 0));
    }
}

void HeaderedItemsControl::PrepareHierarchy(ItemEntry* item, ItemsControl* parentItemsControl)
{
    HierarchicalItem* hiItem = HiItemFromEntry(item);
    if (hiItem != NULL && !hiItem->IsLeafItem())
    {
        if (hiItem->GetExpanded())
        {
            SetItemsSource(hiItem->GetChildren());
        }
        else
        {
            //SetItemsSource(NULL);
        }

        SetValue(HasItemsProperty, Boolean::True);
    }
    else if (HasItems())
    {
        SetValue(HasItemsProperty, Boolean::False);
    }
}

void HeaderedItemsControl::OnHeaderChanged(Object* oldHeader, Object* newHeader)
{
    if (!ReadItemFlags(Control::ItemBoolFlags::HeaderIsNotLogical))
    {
        RemoveLogicalChild(oldHeader);
        AddLogicalChild(newHeader);
    }
}

void HeaderedItemsControl::OnHeaderTemplateChanged(DataTemplate* oldTemplate, DataTemplate* newTemplate)
{

}

String HeaderedItemsControl::ToString()
{
    Object* header = GetHeader();

    if (!header)
    {
        return _U("");
    }
    else
    {
        return header->ToString();
    }
}

}
