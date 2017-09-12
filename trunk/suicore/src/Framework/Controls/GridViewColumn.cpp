/////////////////////////////////////////////////////////////////////////
// GridViewColumn.cpp

#include <System/Windows/DpProperty.h>
#include <System/Resources/BindParser.h>
#include <Framework/Controls/DataTemplate.h>
#include <Framework/Controls/GridViewColumn.h>

namespace suic
{

ImplementRTTIOfClass(GridViewColumn, DpObject)

DpProperty* GridViewColumn::WidthProperty;
DpProperty* GridViewColumn::MinWidthProperty;
DpProperty* GridViewColumn::MaxWidthProperty;
DpProperty* GridViewColumn::HeaderProperty;
DpProperty* GridViewColumn::HeaderTemplateProperty;
DpProperty* GridViewColumn::CellTemplateProperty;

void GridViewColumn::OnWidthPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    GridViewColumn* gvCol = RTTICast<GridViewColumn>(d);
    if (NULL != gvCol)
    {
        gvCol->_iWidth = e->GetNewValue()->ToInt();
        gvCol->_iWidth = SystemParameters::TransformXToDevice(gvCol->_iWidth);
    }
}

void GridViewColumn::OnMinWidthPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    GridViewColumn* gvCol = RTTICast<GridViewColumn>(d);
    if (NULL != gvCol)
    {
        gvCol->_iMinWidth = e->GetNewValue()->ToInt();
        gvCol->_iMinWidth = SystemParameters::TransformXToDevice(gvCol->_iMinWidth);
    }
}

void GridViewColumn::OnMaxWidthPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    GridViewColumn* gvCol = RTTICast<GridViewColumn>(d);
    if (NULL != gvCol)
    {
        gvCol->_iMaxWidth = e->GetNewValue()->ToInt();
        gvCol->_iMaxWidth = SystemParameters::TransformXToDevice(gvCol->_iMaxWidth);
    }
}

void GridViewColumn::OnHeaderPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    GridViewColumn* gvCol = RTTICast<GridViewColumn>(d);
    if (NULL != gvCol)
    {
        gvCol->_header = e->GetNewValue();
    }
}

void GridViewColumn::OnCellTemplatePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    GridViewColumn* gvCol = RTTICast<GridViewColumn>(d);
    if (NULL != gvCol)
    {
        gvCol->_cellTemplate = RTTICast<DataTemplate>(e->GetNewValue());
    }
}

void GridViewColumn::OnHeaderTemplatePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    GridViewColumn* gvCol = RTTICast<GridViewColumn>(d);
    if (NULL != gvCol)
    {
        gvCol->_headerTemplate = RTTICast<DataTemplate>(e->GetNewValue());
    }
}

void GridViewColumn::StaticInit()
{
    if (NULL == WidthProperty)
    {
        WidthProperty = DpProperty::Register(_U("Width"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsNone, &GridViewColumn::OnWidthPropChanged));
        MinWidthProperty = DpProperty::Register(_U("MinWidth"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetPosInt(0), PropMetadataOptions::AffectsNone, &GridViewColumn::OnMinWidthPropChanged));
        MaxWidthProperty = DpProperty::Register(_U("MaxWidth"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(99999), PropMetadataOptions::AffectsNone, &GridViewColumn::OnMaxWidthPropChanged));

        HeaderProperty = DpProperty::Register(_U("Header"), RTTIType(), Object::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &GridViewColumn::OnHeaderPropChanged));
        HeaderTemplateProperty = DpProperty::Register(_U("HeaderTemplate"), RTTIType(), DataTemplate::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &GridViewColumn::OnHeaderTemplatePropChanged));
        
        CellTemplateProperty = DpProperty::Register(_U("CellTemplate"), RTTIType(), DataTemplate::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &GridViewColumn::OnCellTemplatePropChanged));
    }
}

//----------------------------------------------------

GridViewColumn::GridViewColumn()
    : _columnIndex(-1)
    , _displayMember(NULL)
    , _actualWidth(0)
    , _iWidth(0)
    , _iMinWidth(0)
    , _iMaxWidth(99999)
    , _header(DpProperty::UnsetValue())
    , _cellTemplate(NULL)
    , _headerTemplate(NULL)
{
}

GridViewColumn::~GridViewColumn()
{
    FREEREFOBJ(_displayMember);
}

void GridViewColumn::SetAttribute(const String& key, Object* val)
{
    String strVal = val->ToString();

    if (key.Equals(_U("DisplayMemberBinding")) && strVal.Length() >= 9)
    {
        BindParser bp;
        Binding* bind = NULL;

        strVal = strVal.Substring(8, strVal.Length() - 9);
        bind = bp.Parse(strVal);

        SetDisplayMember(bind);
    }
}

}
