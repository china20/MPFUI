/////////////////////////////////////////////////////////////////////////
// GridView.cpp

#include <Framework/Controls/GridView.h>

namespace suic
{

ImplementRTTIOfClass(GridView, ViewBase)

DpProperty* GridView::ColumnsProperty;
DpProperty* GridView::ColumnHeaderToolTipProperty;
DpProperty* GridView::ColumnHeaderTemplateProperty;
DpProperty* GridView::ColumnHeaderContainerStyleProperty;

void GridView::StaticInit()
{
    if (NULL == ColumnsProperty)
    {
        ColumnsProperty = DpProperty::RegisterAttached(_U("Columns"), RTTIType(), GridViewColumnCollection::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));

        ColumnHeaderToolTipProperty = DpProperty::Register(_T("ColumnHeaderToolTip"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
        ColumnHeaderTemplateProperty = DpProperty::Register(_T("ColumnHeaderTemplate"), RTTIType(), ControlTemplate::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
        ColumnHeaderContainerStyleProperty = DpProperty::Register(_T("ColumnHeaderContainerStyle"), RTTIType(), Style::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
    }
}

//----------------------------------------------------

GridView::GridView()
{
    _columns = NULL;
    _owner = NULL;
}

GridView::~GridView()
{
}

void GridView::AddChild(Object* obj)
{
    GridViewColumn* column = RTTIEqualCast<GridViewColumn>(obj);
    if (NULL != column)
    {
        column->SetColumnIndex(GetColumns()->GetCount());
        GetColumns()->AddItem(column);
    }
}

void GridView::AddText(String val)
{

}

void GridView::RemoveChild(Object* child)
{
    GridViewColumn* pCol = RTTICast<GridViewColumn>(child);
    GetColumns()->RemoveItem(pCol);
}

GridViewColumnCollection* GridView::GetColumns()
{
    if (NULL == _columns)
    {
        _columns = new GridViewColumnCollection();
        SetValue(ColumnsProperty, _columns);
    }
    return _columns;
}

void GridView::LinkViewOwner(DpObject* owner)
{
    owner->SetValue(ColumnsProperty, GetColumns());
    _owner = RTTICast<Element>(owner);
}

void GridView::UnlinkViewOwner(DpObject* owner)
{
    owner->ClearValue(ColumnsProperty);
    _owner = NULL;
}

void GridView::PrepareItem(DpObject* item)
{
    ViewBase::PrepareItem(item);
    item->SetValue(ColumnsProperty, GetColumns());
}

}
