/////////////////////////////////////////////////////////////////////////
// ComboBoxItem.cpp

#include <Extend/Controls/ComboBoxItem.h>
#include <Framework/Controls/Selector.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Windows/CoreTool.h>

namespace suic
{

ImplementRTTIOfClass(ComboBoxItem, ListBoxItem)

/// <summary>
///  组合框适配项，可加入任意类型元素
/// </summary>
ComboBoxItem::ComboBoxItem()
{
}

ComboBoxItem::~ComboBoxItem()
{
}

void ComboBoxItem::StaticInit()
{
    static bool s_init = false;
    if (!s_init)
    {
        s_init = true;
        IsMouseOverProperty->OverrideMetadata(RTTIType()
            , DpPropMemory::GetPropMeta(suic::Boolean::False, PropMetadataOptions::AffectsNone, &OnVisualStatePropChanged));
    }
}

}
