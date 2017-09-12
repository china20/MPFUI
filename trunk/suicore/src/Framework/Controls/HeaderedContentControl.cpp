
#include <Framework/Controls/HeaderedContentControl.h>

#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>

namespace suic
{

ImplementRTTIOfClass(HeaderedContentControl, ContentControl)

DpProperty* HeaderedContentControl::HasHeaderProperty;
DpProperty* HeaderedContentControl::HeaderProperty;
DpProperty* HeaderedContentControl::HeaderTemplateProperty;
DpProperty* HeaderedContentControl::HeaderTemplateSelectorProperty;

HeaderedContentControl::HeaderedContentControl()
{
}

HeaderedContentControl::~HeaderedContentControl()
{
}

void HeaderedContentControl::StaticInit()
{
    if (NULL == HasHeaderProperty)
    {
        ContentControl::StaticInit();

        HasHeaderProperty = DpProperty::Register(_U("HasHeader"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        HeaderProperty = DpProperty::Register(_U("Header"), RTTIType(), Object::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &HeaderedContentControl::OnHeaderPropChanged));
        HeaderTemplateProperty = DpProperty::Register(_U("HeaderTemplate"), RTTIType(), DataTemplate::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &HeaderedContentControl::OnHeaderTemplatePropChanged));
        HeaderTemplateSelectorProperty = DpProperty::Register(_U("HeaderTemplateSelector")
            , RTTIType(), Object::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
    }
}

void HeaderedContentControl::OnHeaderPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    HeaderedContentControl* elem = DynamicCast<HeaderedContentControl>(d);
    elem->SetValue(HasHeaderProperty, (e->GetNewValue() != DpProperty::UnsetValue()) ? Boolean::True : Boolean::False);
    elem->OnHeaderChanged(e->GetOldValue(), e->GetNewValue());
}

void HeaderedContentControl::OnHeaderTemplatePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    HeaderedContentControl* elem = DynamicCast<HeaderedContentControl>(d);
    DataTemplate* oldTemp = DynamicCast<DataTemplate>(e->GetOldValue());
    DataTemplate* newTemp = DynamicCast<DataTemplate>(e->GetNewValue());

    elem->OnHeaderTemplateChanged(oldTemp, newTemp);
}

void HeaderedContentControl::SetHeaderIsLogical(bool val)
{
    WriteItemFlags(Control::ItemBoolFlags::HeaderIsNotLogical, !val);
}

void HeaderedContentControl::SetHeaderIsItem(bool val)
{
    WriteItemFlags(Control::ItemBoolFlags::HeaderEqualItem, val);
}

void HeaderedContentControl::ClearHeaderedContentControl(Object* item)
{
    if (item != this)
    {
        if (ContentIsItem())
        {
            SetContent(NULL);
        }

        if (HeaderIsItem())
        {
            SetHeader(NULL);
        }
    }
}

void HeaderedContentControl::PrepareHeaderedContentControl(Object* item, DataTemplate* itemTemplate, DataTemplateSelector* itemTemplateSelector)
{
    if (item != this)
    {
        SetHeaderIsLogical(false);

        if (HeaderIsItem() || GetHeader() == DpProperty::UnsetValue())
        {
            SetHeader(item);
            SetHeaderIsItem(true);
        }

        if (itemTemplate != NULL)
        {
            SetValue(HeaderTemplateProperty, itemTemplate);
        }

        if (itemTemplateSelector != NULL)
        {
            SetValue(HeaderTemplateSelectorProperty, itemTemplateSelector);
        }
    }
    else
    {
        SetHeaderIsLogical(true);
    }
}

void HeaderedContentControl::OnHeaderChanged(Object* oldHeader, Object* newHeader)
{
    RemoveLogicalChild(oldHeader);
    AddLogicalChild(newHeader);
}

void HeaderedContentControl::OnHeaderTemplateChanged(DataTemplate* oldTemplate, DataTemplate* newTemplate)
{

}

}
