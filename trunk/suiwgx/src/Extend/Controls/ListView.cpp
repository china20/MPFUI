/////////////////////////////////////////////////////////////////////////
// ListView.cpp


#include <Extend/Controls/ListView.h>
#include <Extend/Controls/ListViewItem.h>

namespace suic
{

ImplementRTTIOfClass(ListView, ListBox)

DpProperty* ListView::ViewProperty;

void ListView::StaticInit()
{
    if (NULL == ViewProperty)
    {
        ListBox::StaticInit();

        ViewProperty = DpProperty::Register(_U("View"), RTTIType(), ViewBase::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsMeasure, &ListView::OnViewPropChanged));
    }
}

void ListView::OnViewPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ListView* listView = RTTICast<ListView>(d);
    ViewBase* oldView = RTTICast<ViewBase>(e->GetOldValue());
    ViewBase* newView = RTTICast<ViewBase>(e->GetNewValue());

    if (oldView)
    {
        oldView->UnlinkViewOwner(listView);
    }

    if (newView)
    {
        newView->LinkViewOwner(listView);
    }
}

//----------------------------------------------------

ListView::ListView()
{
    ListView::StaticInit();
    WriteDoFlag(ViewFlags::IsNotCalcMaxWidth, true);
}

ListView::~ListView()
{

}

ViewBase* ListView::GetView()
{
    return DynamicCast<ViewBase>(GetValue(ViewProperty));
}

void ListView::SetView(ViewBase* view)
{
    SetValue(ViewProperty, view);
}

Element* ListView::GetContainerForItemOverride()
{
    return new ListViewItem();
}

bool ListView::IsItemItsOwnContainer(Object* item)
{
    return (NULL != item && item->GetRTTIType() == ListViewItem::RTTIType());
}

void ListView::PrepareContainerForItemOverride(DpObject* elem, ItemEntry* item)
{
    ListBox::PrepareContainerForItemOverride(elem, item);
    ListViewItem* listItem = RTTICast<ListViewItem>(elem);

    if (listItem != NULL)
    {
        ViewBase* view = GetView();
        if (view != NULL)
        {
            view->PrepareItem(listItem);
        }
    }
}

}
