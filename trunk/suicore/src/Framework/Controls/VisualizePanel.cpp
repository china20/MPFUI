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
#include <Framework/Controls/ItemsControl.h>

namespace suic
{

//////////////////////////////////////////////////////////////
// VirtualizingPanel
//
ImplementRTTIOfClass(VirtualizingPanel, suic::Panel)

VirtualizingPanel::VirtualizingPanel()
    :_scrollInfo(NULL)
    , _realedCount(0)
{
}

VirtualizingPanel::~VirtualizingPanel()
{
    if (NULL != _scrollInfo)
    {
        delete _scrollInfo;
    }
}

void VirtualizingPanel::BringIndexIntoView(int index)
{

}

bool VirtualizingPanel::IsScrolling()
{
    return (_scrollInfo != NULL && _scrollInfo->GetScrollOwner() != NULL);
}

IScrollInfo* VirtualizingPanel::GetScrollInfo()
{
    if (NULL == _scrollInfo)
    {
        _scrollInfo = new ScrollInfo();
        _scrollInfo->SetOwner(this);
    }
    return _scrollInfo;
}

ScrollData* VirtualizingPanel::GetScrollData()
{
    GetScrollInfo();
    return _scrollInfo->GetScrollData();
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

void VirtualizingPanel::ClearRealizedContainer()
{
    ElementColl* coll = GetChildren();
    if (_realedCount > 0)
    {
        if (_realedCount == coll->GetCount())
        {
            coll->Reset();
        }
        else
        {
            for (int i = 0; i < coll->GetCount(); ++i)
            {
                Element* elem = coll->GetAt(i);
                if (elem->GetContainerItem() == elem)
                {
                    coll->RemoveAt(i);
                    --i;
                    --_realedCount;
                }
                if (_realedCount == 0)
                {
                    break;
                }
            }
        }
    }
}

Element* VirtualizingPanel::RecycleContainer(int index, ItemEntry* item)
{
    ElementColl* coll = GetChildren();
    if (index < coll->GetCount())
    {
        for (int i = index; i < coll->GetCount(); ++i)
        {
            Element* elem = coll->GetAt(i);
            ItemEntry* entry = elem->GetItemEntry();
            if (!elem->ReadDoFlag(ViewFlags::IsContainerItemVF))
            {
                coll->RemoveAt(i);
                --i;
            }
            else
            {
                if (item != entry)
                {
                    ItemsControl::ClearBeforeLinkContainer(elem);
                }
                return elem;                
            }
        }
    }
    return NULL;
}

void VirtualizingPanel::InsertContainer(int index, Element* conainer)
{
    ElementColl* children = GetChildren();
    if (children->GetCount() <= index)
    {
        children->AddInternal(conainer);
    }
    else 
    {
        children->Insert(index, conainer);
    }
}


void VirtualizingPanel::SetAndVerifyScrollingData(Size viewport, Size extent, Point offset)
{
    ScrollData* scrollData = NULL;
    
    if (NULL != _scrollInfo)
    {
        scrollData = _scrollInfo->GetScrollData();

        offset.x = min(offset.x, extent.cx - viewport.cx);
        offset.y = min(offset.y, extent.cy - viewport.cy);

        offset.x = max(0, offset.x);
        offset.y = max(0, offset.y);

        bool flag = viewport != scrollData->viewport;
        bool flag2 = extent != scrollData->extent;
        bool flag3 = offset != scrollData->computedOffset;

        if ((flag || flag2) || flag3)
        {
            Point oldViewportOffset = scrollData->computedOffset;
            Size oldViewportSize = scrollData->viewport;
            scrollData->viewport = viewport;
            scrollData->extent = extent;
            scrollData->SetComputedOffset(offset);

            if (flag)
            {
                OnViewportSizeChanged(oldViewportSize, viewport);
            }

            if (flag3)
            {
                OnViewportOffsetChanged(oldViewportOffset, offset);
            }

            OnScrollChange(_scrollInfo);
        }
    }
}

void VirtualizingPanel::OnScrollChange(IScrollInfo* pScrollInfo)
{
    if (pScrollInfo->GetScrollOwner() != NULL)
    {
        pScrollInfo->GetScrollOwner()->InvalidateScrollInfo(true);
    }
}

void VirtualizingPanel::OnViewportSizeChanged(Size oldViewportSize, Size newViewportSize)
{
}

void VirtualizingPanel::OnViewportOffsetChanged(Point oldViewportOffset, Point newViewportOffset)
{
}

void VirtualizingPanel::OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e)
{
}

}
