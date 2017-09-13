// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// VirtualizingWrapPanel.cpp

#include <Extend/Controls/VisualizeWrapPanel.h>

#include <System/Input/KeyboardNavigation.h>

#include <Framework/Controls/ItemsControl.h>
#include <Framework/Presenter/ScrollContentPresenter.h>
#include <Framework/Presenter/ItemContainerGenerator.h>

namespace suic
{

ImplementRTTIOfClass(VirtualizingWrapPanel, suic::VirtualizingPanel)

suic::DpProperty* VirtualizingWrapPanel::OrientationProperty;

void VirtualizingWrapPanel::StaticInit()
{
    if (OrientationProperty == NULL)
    {
        OrientationProperty = suic::DpProperty::Register(_T("Orientation"), RTTIType(), suic::Integer::RTTIType()
            , suic::DpPropMemory::GetPropMeta(suic::OrientationBox::HorizontalBox, suic::PropMetadataOptions::AffectsMeasure));
        OrientationProperty->SetConvertValueCb(suic::OrientationConvert::Convert);
    }
}

VirtualizingWrapPanel::VirtualizingWrapPanel()
    : _visibleStart(0)
    , _visibleCount(0)
    , _visibleOffset(0)
    , _countPerShow(1)
    , _itemWid(80)
    , _itemHei(80)
{
}

VirtualizingWrapPanel::~VirtualizingWrapPanel()
{
}

suic::Orientation VirtualizingWrapPanel::GetOrientation()
{
    return (suic::Orientation)GetValue(OrientationProperty)->ToInt();
}

void VirtualizingWrapPanel::SetOrientation(suic::Orientation val)
{
    SetValue(OrientationProperty, suic::OrientationBox::From(val));
}

int VirtualizingWrapPanel::GetLogicalOrientation()
{
    return ((GetOrientation() + 1) % 2);
}

int VirtualizingWrapPanel::GetItemWidth()
{
    return _itemWid;
}

void VirtualizingWrapPanel::SetItemWidth(int wid)
{
    _itemWid = wid;
}

int VirtualizingWrapPanel::GetItemHeight()
{
    return _itemHei;
}

void VirtualizingWrapPanel::SetItemHeight(int hei)
{
    _itemHei = hei;
}

void VirtualizingWrapPanel::ResetChildMeasureData(suic::MeasureData* childMeasureData, const suic::MeasureData& measureData, suic::Size layoutSize)
{
    suic::Rect viewport = measureData.GetViewPort();

    childMeasureData->SetAvailableSize(layoutSize);
    childMeasureData->SetViewPort(viewport);
}

int VirtualizingWrapPanel::ComputeOffsetFromItem(suic::Object* item, eItemDirection id, int& offset, int& itemSize)
{
    int index = -2;
    bool bHori = GetOrientation() == suic::Orientation::Horizontal;
    suic::ItemContainerGenerator* icGenerator = GetGenerator();

    if (NULL == icGenerator || _countPerShow <= 0)
    {
        return index;
    }

    offset = 0;
    itemSize = 0;

    suic::ItemCollection* itemColl = icGenerator->GetItems();

    if (NULL == itemColl) 
    {
        return index;
    }

    int iCount = itemColl->GetCount();

    for (int i = 0; i < iCount; ++i)
    {
        if (item == itemColl->GetItem(i))
        {
            index = i;
            break;
        }
    }

    if (index >= 0)
    {
        switch (id)
        {
        case eItemDirection::idPrev:
            if (index > 0)
            {
                index -= 1;
            }
            break;

        case eItemDirection::idNext:
            if (index < iCount - 1)
            {
                index += 1;
            }
            break;

        default:
            ;
        }

        if (!bHori)
        {
            itemSize = GetItemWidth();
        }
        else
        {
            itemSize = GetItemHeight();
        }

        offset = itemSize * (index / _countPerShow);
    }

    return index;
}

void VirtualizingWrapPanel::OnKeyDown(suic::KeyboardEventArg* e)
{
    suic::ItemsControl* itemsOwner = NULL;

    itemsOwner = suic::ItemsControl::GetItemsOwner(this);

    if (NULL == itemsOwner || itemsOwner->GetCount() == 0)
    {
        return ;
    }

    suic::ScrollViewer* scrollView = itemsOwner->GetScrollHost();

    if (scrollView == NULL)
    {
        return ;
    }

    if (e->GetKey() == suic::Key::kSpace || 
        (e->GetKey() == suic::Key::kReturn && suic::KeyboardNavigation::IsAcceptsReturn(itemsOwner)) )
    {
        itemsOwner->HandleSelectedItem(e->GetOriginalSource(), suic::MouseButton::mbUnknown);
        e->SetHandled(true);
        return;
    }

    if (e->GetKey() != suic::Key::kLeft && 
        e->GetKey() != suic::Key::kRight && 
        e->GetKey() != suic::Key::kUp && 
        e->GetKey() != suic::Key::kDown)
    {
        return;
    }

    e->SetHandled(true);

    suic::ItemEntry* itemb = itemsOwner->GetFocusedItem();
    suic::Object* currentItem = NULL;
    suic::Size itemSize;
    bool bHori = GetOrientation() == suic::Orientation::Horizontal;
    suic::AxisDirection dAxis = bHori ? suic::AxisDirection::yAxis : suic::AxisDirection::xAxis;

    int offset = 0;
    int itemLen = 0;
    int skipIndex = 0;
    int currIndex = 0;

    if (NULL == itemb)
    {
        currentItem = itemsOwner->GetItem(0);
    }
    else
    {
        currentItem = itemb->GetItem();
    }

    currIndex = ComputeOffsetFromItem(currentItem, eItemDirection::idCurr, offset, itemLen);

    if (e->GetKey() == suic::Key::kLeft)
    {
        skipIndex = currIndex - 1;
    }
    else if (e->GetKey() == suic::Key::kRight)
    {
        skipIndex = currIndex + 1;
    }
    else if (e->GetKey() == suic::Key::kUp)
    {
        skipIndex = currIndex - _countPerShow;
    }
    else
    {
        skipIndex = currIndex + _countPerShow;
    }

    if (skipIndex >= 0 && skipIndex < itemsOwner->GetCount())
    {
        suic::ItemEntry* moveItem = itemsOwner->ItemEntryFromIndex(skipIndex);
        suic::Element* moveElem = itemsOwner->GetContainerFromItem(moveItem->GetItem());

        if (!itemsOwner->IsOnCurrentPage(moveElem, dAxis, true))
        {
            int iCurr = currIndex / _countPerShow;
            int iNext = skipIndex / _countPerShow;

            if (iCurr > iNext)
            {
                offset -= GetItemHeight();
            }
            else if (iCurr < iNext)
            {
                offset += itemLen;
                offset = max(0, offset - scrollView->GetViewportHeight() + itemLen);
            }

            scrollView->ScrollToVerticalOffset(offset);
            itemsOwner->UpdateFocusItem(moveItem);
        }
        else
        {
            moveElem->Focus();
        }

        itemsOwner->UpdateLayout();
    }
}

suic::Size VirtualizingWrapPanel::OnMeasure(const suic::Size& constraint)
{
    suic::Rect empty;
    suic::Size extent;
    suic::Size layoutSize = constraint;

    int viewPortSize = 0;
    int childIndex = 0;
    int visibleMeasure = 0;

    bool bScrolling = IsScrolling();
    bool bHori = GetOrientation() == suic::Orientation::Horizontal;

    suic::ElementColl* children = GetChildren();
    suic::ScrollData* scrollData = GetScrollData();
    suic::ItemContainerGenerator* icGenerator = GetGenerator();
    suic::ItemsControl* itemsOwner = suic::ItemsControl::GetItemsOwner(this);

    if (NULL == itemsOwner || NULL == icGenerator)
    {
        return extent;
    }

    suic::ItemCollection* itemColl = icGenerator->GetItems();

    if (NULL == itemColl) 
    {
        return extent;
    }

    if (bScrolling)
    {
        empty = suic::Rect(scrollData->offset.x, scrollData->offset.y, constraint.Width(), constraint.Height());
    }

    int iItemWid = GetItemWidth();
    int iItemHei = GetItemHeight();

    suic::Point offset(empty.left, empty.top);
    suic::MeasureData measureData = *GetMeasureData();

    _countPerShow = 1;
    visibleMeasure = 0;

    if (!bHori)
    {
        viewPortSize = constraint.Width() + iItemWid;
        _countPerShow = constraint.Height() / iItemHei;
        _visibleOffset = scrollData->offset.x % iItemWid;
    }
    else
    {
        viewPortSize = constraint.Height() + iItemHei;
        _countPerShow = constraint.Width() / iItemWid;
        _visibleOffset = scrollData->offset.y % iItemHei;
    }

    if (_countPerShow <= 0)
    {
        _countPerShow = 1;
    }

    int iAllCount = itemsOwner->GetCount() / _countPerShow;
    if ((itemsOwner->GetCount() % _countPerShow) != 0)
    {
        iAllCount += 1;
    }

    _visibleCount = 0;

    if (!bHori)
    {
        layoutSize.cx = iItemWid;
        layoutSize.cy = constraint.Height() / _countPerShow;
    }
    else
    {
        layoutSize.cx = constraint.Width() / _countPerShow;
        layoutSize.cy = iItemHei;
    }

    // 
    // 计算第一个在可是范围的元素
    // 
    _visibleStart = scrollData->offset.y / iItemHei * _countPerShow;
    childIndex = _visibleStart;

    //
    // 计算在可视范围内的元素
    //
    if (icGenerator->StartAt(childIndex, -1))
    {
        while (icGenerator->HasNext())
        {
            suic::Element* container = NULL;
            suic::ItemEntry* item = itemColl->GetItemEntry(childIndex);

            if (itemsOwner->IsItemItsOwnContainer(item->GetItem()))
            {
                ++_realedCount;
                container = suic::RTTICast<suic::Element>(item->GetItem());
                InsertContainer(_visibleCount, container);
                icGenerator->LinkContainerToItem(container, item, false);
            }
            else
            {
                bool isNeedRecycle = false;
                container = RecycleContainer(_visibleCount, item);
                if (NULL == container)
                {
                    container = icGenerator->GenerateContainer(isNeedRecycle);
                    InsertContainer(_visibleCount, container);
                    container->unref();
                }
                else
                {
                    icGenerator->LinkContainerToItem(container, item, false);
                }
            }

            ++childIndex;
            ++_visibleCount;

            if (NULL != container)
            {
                ResetChildMeasureData(container->GetMeasureData(), measureData, layoutSize);

                // 根据项容器和项生存容器内容
                icGenerator->PrepareItemContainer(container, item);
                container->Measure(layoutSize);

                suic::Size desiredSize = container->GetDesiredSize();

                // 
                // 刚好布局了一行
                // 
                if ((childIndex % _countPerShow) == 0)
                {
                    if (!bHori)
                    {
                        visibleMeasure += desiredSize.Width();
                    }
                    else
                    {
                        visibleMeasure += desiredSize.Height();
                    }
                }

                // 缓存实时计算的大小
                itemsOwner->StoreItemSize(item, childIndex, desiredSize);

                if (visibleMeasure > viewPortSize)
                {
                    break;
                }
            }
        }
    }

    if (!bHori)
    {
        extent.cy = constraint.Height();
        extent.cx = iAllCount * iItemWid;
    }
    else
    {
        extent.cx = constraint.Width();
        extent.cy = iAllCount * iItemHei;
    }

    if (bScrolling)
    {
        SetAndVerifyScrollingData(empty.ToSize(), extent, offset);
    }

    return extent;
}

void VirtualizingWrapPanel::OnArrange(const suic::Size& arrangesize)
{
    int x = 0;
    int y = 0;
    bool bHori = GetOrientation() == suic::Orientation::Horizontal;
    int width = 0;

    suic::Rect finalRect;

    int index = 0;
    int count = _visibleCount;
    int iItemWid = GetItemWidth();
    int iItemHei = GetItemHeight();
    suic::ElementColl* children = GetChildren();

    if (!bHori)
    {
        x = -_visibleOffset;
        iItemHei = arrangesize.Height() / _countPerShow;
    }
    else
    {
        y = -_visibleOffset;
        iItemWid = arrangesize.Width() / _countPerShow;;
    }

    finalRect.left = x;
    finalRect.top = y;

    while (index < count)
    {
        suic::Element* element = children->GetAt(index);
        int iStart = index % _countPerShow;

        index++;

        if (element != NULL)
        {
            if (!bHori)
            {
                finalRect.right = finalRect.left + iItemWid;
                finalRect.top = iStart * iItemHei;
                finalRect.bottom = finalRect.top + iItemHei;
            }
            else
            {
                finalRect.bottom = finalRect.top + iItemHei;
                finalRect.left = iStart * iItemWid;
                finalRect.right = finalRect.left + iItemWid;
            }

            element->Arrange(finalRect);

            if ((index % _countPerShow) == 0)
            {
                if (!bHori)
                {
                    finalRect.left += iItemWid;
                }
                else
                {
                    finalRect.top += iItemHei;
                }
            }
        }
    }
}

void VirtualizingWrapPanel::OnItemsChangedInternal(suic::Object* sender, suic::ItemsChangedEventArg* e)
{
    _visibleCount = 0;
    _visibleStart = 0;
    suic::VirtualizingPanel::OnItemsChangedInternal(sender, e);

    bool flag = false;
    switch (e->GetAction())
    {
        case suic::NotifyCollectionChangedAction::Remove:
            flag = true;
            break;

        case suic::NotifyCollectionChangedAction::Replace:
            flag = true;
            break;

        case suic::NotifyCollectionChangedAction::Move:
            break;

        case suic::NotifyCollectionChangedAction::Reset:
            flag = true;
            break;
    }

    if (flag && IsScrolling())
    {
        _scrollInfo->GetScrollData()->extent = suic::Size();
        UpdateLayout();
    }
}

void VirtualizingWrapPanel::InvalidateChildrenResourceReference()
{
    suic::ElementColl* children = GetChildren();
    int count = children->GetCount();

    for (int j = 0; j < count; ++j)
    {
        suic::FrameworkElement* fe = CASTTOFE(children->GetAt(j));
        if (fe != NULL)
        {
            fe->InvalidateResourceReference(false);
        }
    }
}

}
