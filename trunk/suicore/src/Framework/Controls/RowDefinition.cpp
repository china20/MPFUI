// 华勤科技版权所有 2010-2020
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
//
// MPF界面引擎采用业界成熟的体系架构，其属性系统、事件系统以
// 及模版在目前c++界面库是一大特点，真正做到样式和代码分离，底层
// 更采用了SSE2指令进行加速，不论在普通窗口还是半透明窗口下性能都
// 非常优秀，完善、灵活的动画接口，完全可以满足各种商业使用。
// 
// 如果需购买技术支持服务或咨询请联系
// china0851@foxmail.com；QQ:386666951。
// ====================================================================

#include <Framework/Controls/RowDefinition.h>

namespace suic
{

ImplementRTTIOfClass(RowDefinition, DefinitionBase)

DpProperty* RowDefinition::MaxHeightProperty;
DpProperty* RowDefinition::MinHeightProperty;
DpProperty* RowDefinition::HeightProperty;

void RowDefinition::StaticInit()
{
    DefinitionBase::StaticInit();

    if (NULL == HeightProperty)
    {
        int iFlag = PropMetadataOptions::AffectsMeasure | PropMetadataOptions::AffectsParentMeasure;

        MaxHeightProperty = DpProperty::Register(_U("MaxHeight"), RTTIType(), OFloat::RTTIType()
            , DpPropMemory::GetPropMeta(new OFloat(Numeric::MeasureInt), iFlag, &RowDefinition::OnMaxHeightPropChanged));
        MinHeightProperty = DpProperty::Register(_U("MinHeight"), RTTIType(), OFloat::RTTIType()
            , DpPropMemory::GetPropMeta(new OFloat(0), iFlag, &RowDefinition::OnMinHeightPropChanged));
        HeightProperty = DpProperty::Register(_U("Height"), RTTIType(), GridSize::RTTIType()
            , DpPropMemory::GetPropMeta(new GridSize(GridSizeType::Star, 1.0f), iFlag, &RowDefinition::OnHeightPropChanged));

        HeightProperty->SetConvertValueCb(GridSizeConvert::Convert);
    }
}

void RowDefinition::OnHeightPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    RowDefinition* rowDef = DynamicCast<RowDefinition>(d);
    GridSize* gSize = RTTICast<GridSize>(e->GetNewValue());

    rowDef->SetUserSize(gSize, false);
}

void RowDefinition::OnMinHeightPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    RowDefinition* rowDef = DynamicCast<RowDefinition>(d);
    rowDef->SetUserMinSize(e->GetNewValue()->ToFloat(), false);
}

void RowDefinition::OnMaxHeightPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    RowDefinition* rowDef = DynamicCast<RowDefinition>(d);
    rowDef->SetUserMaxSize(e->GetNewValue()->ToFloat(), false);
}

RowDefinition::RowDefinition()
    : DefinitionBase(false)
{
}

RowDefinition::~RowDefinition()
{

}

RowDefinitions::RowDefinitions()
{
}

RowDefinitions::~RowDefinitions()
{

}

}
