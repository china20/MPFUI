// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// VisualizePanel.cpp

#include <System/Tools/Debug.h>

#include <Framework/Controls/ScrollViewer.h>
#include <Framework/Controls/VisualizePanel.h>
//#include <Framework/Controls/ItemsElement.h>

namespace suic
{

//////////////////////////////////////////////////////////////
// VirtualizingPanel
//
ImplementRTTIOfClass(VirtualizingPanel, suic::Panel)

VirtualizingPanel::VirtualizingPanel()
{
}

VirtualizingPanel::~VirtualizingPanel()
{
}

void VirtualizingPanel::BringIndexIntoView(int index)
{

}

void VirtualizingPanel::GenerateChildren()
{
}

void VirtualizingPanel::ClearChildren()
{
}

VirtualizingPanel* VirtualizingPanel::Clone()
{
    VirtualizingPanel* pVp(DynamicCast<VirtualizingPanel>(suic::Panel::Clone()));
    return pVp;
}

void VirtualizingPanel::ClearAllContainers(ItemsControl* itemsControl)
{

}

void VirtualizingPanel::OnScrollChange(IScrollInfo* pScrollInfo)
{
    if (pScrollInfo->GetScrollOwner() != NULL)
    {
        pScrollInfo->GetScrollOwner()->InvalidateScrollInfo(true);
    }
}

void VirtualizingPanel::OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e)
{
}

}
