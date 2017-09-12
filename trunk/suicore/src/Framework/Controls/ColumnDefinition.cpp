// 华勤科技版权所有 2008-2022
// 
// 文件名：ColumnDefinition.h
// 功  能：表格列。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#include <Framework/Controls/ColumnDefinition.h>

namespace suic
{

ImplementRTTIOfClass(ColumnDefinition, DefinitionBase)

DpProperty* ColumnDefinition::MaxWidthProperty;
DpProperty* ColumnDefinition::MinWidthProperty;
DpProperty* ColumnDefinition::WidthProperty;

void ColumnDefinition::StaticInit()
{
    DefinitionBase::StaticInit();

    if (NULL == WidthProperty)
    {
        int iFlag = PropMetadataOptions::AffectsMeasure | PropMetadataOptions::AffectsParentMeasure;

        MaxWidthProperty = DpProperty::Register(_U("MaxWidth"), RTTIType(), OFloat::RTTIType()
            , DpPropMemory::GetPropMeta(new OFloat(Numeric::MeasureInt), iFlag, &ColumnDefinition::OnMaxWidthPropChanged));
        MinWidthProperty = DpProperty::Register(_U("MinWidth"), RTTIType(), OFloat::RTTIType()
            , DpPropMemory::GetPropMeta(new OFloat(0), iFlag, &ColumnDefinition::OnMinWidthPropChanged));
        WidthProperty = DpProperty::Register(_U("Width"), RTTIType(), GridSize::RTTIType()
            , DpPropMemory::GetPropMeta(new GridSize(GridSizeType::Star, 1.0f), iFlag, &ColumnDefinition::OnWidthPropChanged));

        WidthProperty->SetConvertValueCb(GridSizeConvert::Convert);
    }
}

void ColumnDefinition::OnWidthPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ColumnDefinition* colDef = RTTICast<ColumnDefinition>(d);
    GridSize* gSize = RTTICast<GridSize>(e->GetNewValue());

    colDef->SetUserSize(gSize, true);
}

void ColumnDefinition::OnMinWidthPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ColumnDefinition* colDef = RTTICast<ColumnDefinition>(d);
    colDef->SetUserMinSize(e->GetNewValue()->ToFloat(), true);
}

void ColumnDefinition::OnMaxWidthPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ColumnDefinition* colDef = RTTICast<ColumnDefinition>(d);
    colDef->SetUserMaxSize(e->GetNewValue()->ToFloat(), true);
}

ColumnDefinition::ColumnDefinition()
{

}

ColumnDefinition::~ColumnDefinition()
{

}

ColumnDefinitions::ColumnDefinitions()
{
}

ColumnDefinitions::~ColumnDefinitions()
{
}

}
