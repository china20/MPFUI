
#include <Framework/Controls/VisualizeStackPanel.h>
#include <Framework/Controls/HeaderedItemsControl.h>
#include <Framework/Controls/HeaderedContentControl.h>

#include <Framework/Controls/Popup.h>

#include <Framework/Presenter/ScrollContentPresenter.h>
#include <Framework/Presenter/ItemContainerGenerator.h>

#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>
#include <System/Tools/Debug.h>
#include <System/Input/Keyboard.h>

#include <System/Windows/MemberVisitor.h>

namespace suic
{

ImplementRTTIOfClass(VirtualizingStackPanel, VirtualizingPanel)

DpProperty* VirtualizingStackPanel::OrientationProperty;

VirtualizingStackPanel::VirtualizingStackPanel()
    : _visibleStart(0)
    , _visibleCount(0)
    , _visibleOffset(0)
{
}

VirtualizingStackPanel::~VirtualizingStackPanel()
{
}

void VirtualizingStackPanel::StaticInit()
{
    if (NULL == OrientationProperty)
    {
        Panel::StaticInit();
        OrientationProperty = DpProperty::Register(_T("Orientation"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(OrientationBox::VerticalBox, PropMetadataOptions::AffectsMeasure
            , &VirtualizingStackPanel::OnOrientationPropChanged));
        OrientationProperty->SetConvertValueCb(OrientationConvert::Convert);
    }
}

void VirtualizingStackPanel::OnOrientationPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    //ResetScrolling(d);
}

void VirtualizingStackPanel::ClearAllContainers(ItemsControl* itemsControl)
{
    _visibleCount = 0;
    _visibleStart = 0;
    GetChildren()->Reset();
}

int VirtualizingStackPanel::CalcFirstVisibleItemOffset(ItemsControl* itemsOwner, MeasureData& measureData, bool bHori, int& offset, Size& stackOffset)
{
    int i = 0;
    offset = 0;

    bool calcWid = !itemsOwner->ReadDoFlag(ViewFlags::IsNotCalcMaxWidth);
    int iViewport = bHori ? measureData.GetViewPort().left : measureData.GetViewPort().top;
    int itemCount = (itemsOwner != NULL) ? itemsOwner->GetCount() : 0;

    if (iViewport < 0)
    {
        iViewport = 0;
    }

    for (i = 0; i < itemCount; ++i)
    {
        Size size;
        ItemEntry* item = itemsOwner->ItemEntryFromIndex(i);
        itemsOwner->ReadItemSize(item, bHori, i, size);

        if (bHori)
        {
            stackOffset.cx += size.Width();
            if (calcWid)
            {
                stackOffset.cy = max(stackOffset.cy, size.Height());
            }

            if (stackOffset.cx >= iViewport)
            {
                stackOffset.cx -= size.Width();
                offset = stackOffset.cx - iViewport;
                break;
            }
        }
        else
        {
            stackOffset.cy += size.Height();
            if (calcWid)
            {
                stackOffset.cx = max(stackOffset.cx, size.Width());
            }

            if (stackOffset.cy >= iViewport)
            {
                stackOffset.cy -= size.Height();
                offset = stackOffset.cy - iViewport;
                break;
            }
        }
    }

    return i;
}

void VirtualizingStackPanel::AdjustViewportOffset(MeasureData& measureData, ItemsControl* itemsControl, bool isHori)
{
    int num = 0;
    Rect viewport = measureData.GetViewPort();
    IProvideStackingSize* virtualItem = dynamic_cast<IProvideStackingSize*>(itemsControl);

    if (NULL != virtualItem)
    {
        num = virtualItem->GetHeaderSize(isHori);
        if (isHori)
        {
            viewport.left -= num;
            viewport.right -= num;
        }
        else
        {
            viewport.top -= num;
            viewport.bottom -= num;
        }
    }

    measureData.SetViewPort(viewport);
}

int ScrollContentPresenter::CoerceOffset(int offset, int extent, int viewport)
{
    if (offset > (extent - viewport))
    {
        offset = extent - viewport;
    }
    if (offset < 0)
    {
        offset = 0;
    }
    return offset;
}

void VirtualizingStackPanel::OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e)
{
    _visibleCount = 0;
    _visibleStart = 0;
    VirtualizingPanel::OnItemsChangedInternal(sender, e);

    bool flag = false;
    switch (e->GetAction())
    {
        case NotifyCollectionChangedAction::Remove:
            flag = true;
            break;

        case NotifyCollectionChangedAction::Replace:
            flag = true;
            break;

        case NotifyCollectionChangedAction::Move:
            break;

        case NotifyCollectionChangedAction::Reset:
            flag = true;
            break;
    }

    if (flag && IsScrolling())
    {
        _scrollInfo->GetScrollData()->extent = Size();
        UpdateLayout();
    }
}

void VirtualizingStackPanel::ResetChildMeasureData(MeasureData* childMeasureData, const MeasureData& measureData, Size layoutSize, Size stackSize, bool bHori)
{
    Rect viewport = measureData.GetViewPort();

    if (bHori)
    {
        viewport.left -= stackSize.cx;
        viewport.right -= stackSize.cx;
    }
    else
    {
        viewport.top -= stackSize.cy;
        viewport.bottom -= stackSize.cy;
    }

    childMeasureData->SetAvailableSize(layoutSize);
    childMeasureData->SetViewPort(viewport);
}

Size VirtualizingStackPanel::OnMeasure(const Size& constraint)
{
    Size extent;
    ScrollData* scrollData = GetScrollData();
 
    if (scrollData && scrollData->scrollOwner && 
        scrollData->scrollOwner->GetInChildInvalidateMeasure())
    {
        Point offset;
        bool bMeasure = false;
        bool bScrolling = IsScrolling();
        Size layoutSize = constraint;
        ItemsControl* itemsOwner = ItemsControl::GetItemsOwner(this);
        bool bHori = GetOrientation() == Orientation::Horizontal;

        extent = scrollData->extent;
        offset = scrollData->offset;

        if (bHori)
        {
            layoutSize.cx = Numeric::MeasureInt;
            if (bScrolling && _scrollInfo->CanVerticalScroll())
            {
                layoutSize.cy = Numeric::MeasureInt;
            }
            else
            {
                bMeasure = true;
            }
        }
        else
        {
            layoutSize.cy = Numeric::MeasureInt;
            if (bScrolling && _scrollInfo->CanHorizontalScroll())
            {
                layoutSize.cx = Numeric::MeasureInt;
            }
            else
            {
                bMeasure = true;
            }
        }

        if (bMeasure)
        {
            int iWidth = 0;
            int index = 0;
            int childIndex = _visibleStart;
            ElementColl* children = GetChildren();
            while (index < _visibleCount)
            {
                Element* elem = children->GetAt(index);
                Size desiredSize;
                if (elem != NULL)
                {
                    ItemEntry* item = itemsOwner->ItemEntryFromIndex(childIndex);

                    MeasureChild(elem, layoutSize);

                    desiredSize = elem->GetDesiredSize();

                    if (bHori)
                    {
                        extent.cy = max(extent.cy, desiredSize.Height());
                    }
                    else
                    {
                        extent.cx = max(extent.cx, desiredSize.Width());
                    }

                    // 缓存实时计算的大小
                    itemsOwner->StoreItemSize(item, childIndex, desiredSize);
                }

                ++index;
                ++childIndex;
            }
        }

        offset.x = max(0, min(offset.x, extent.cx - constraint.cx));
        offset.y = max(0, min(offset.y, extent.cy - constraint.cy));

        scrollData->SetComputedOffset(offset);
        scrollData->viewport = constraint;

        OnScrollChange(GetScrollInfo());
    }
    else
    {
        extent = MeasureCommon(constraint);
    }

    return extent;
}

void VirtualizingStackPanel::HandleMoreContainer()
{
    ElementColl* coll = GetChildren();
    int iStartIndex = _visibleCount + 8;
    if (iStartIndex < coll->GetCount())
    {
        for (int i = iStartIndex; i < coll->GetCount(); ++i)
        {
            Element* elem = coll->GetAt(i);
            if (elem->ReadDoFlag(ViewFlags::IsContainerItemVF))
            {
                GetGenerator()->Recycle(elem);
            }
            //elem->WriteDoFlag(ViewFlags::IsNotInVisibileRange, true);
        }
        coll->RemoveRange(iStartIndex, coll->GetCount() - iStartIndex);
    }
}

Size VirtualizingStackPanel::MeasureCommon(const Size& constraint)
{
    Rect empty;
    Size extent;
    Size layoutSize = constraint;
    int stackSize = 0;
    int viewPortSize = 0;
    int childIndex = 0;
    int firstItemIndex = 0;
    bool bScrolling = IsScrolling();
    bool bHori = GetOrientation() == Orientation::Horizontal;

    ElementColl* children = GetChildren();
    ScrollData* scrollData = GetScrollData();
    ItemContainerGenerator* icGenerator = GetGenerator();
    ItemsControl* itemsOwner = ItemsControl::GetItemsOwner(this);

    if (NULL == itemsOwner || NULL == icGenerator)
    {
        return extent;
    }

    ItemCollection* itemColl = icGenerator->GetItems();

    if (NULL == itemColl) 
    {
        return extent;
    }

    int visibleMeasure = 0;
    int itemCount = itemColl->GetCount();
    MeasureData measureData = *GetMeasureData();

    ClearRealizedContainer();
    itemsOwner->OnPrepareContainer();

    if (bScrolling)
    {
        empty = Rect(scrollData->offset.x, scrollData->offset.y, constraint.Width(), constraint.Height());
        measureData.SetAvailableSize(constraint);
        measureData.SetViewPort(empty);
    }
    else
    {
        AdjustViewportOffset(measureData, itemsOwner, bHori);
    }

    if (bHori)
    {
        layoutSize.cx = Numeric::MeasureInt;
        if (bScrolling && _scrollInfo->CanVerticalScroll())
        {
            layoutSize.cy = Numeric::MeasureInt;
        }
        if (measureData.GetViewPort().left > 0)
        {
            viewPortSize = measureData.GetViewPort().Width();
        }
        else
        {
            viewPortSize = measureData.GetViewPort().right;
        }
    }
    else
    {
        layoutSize.cy = Numeric::MeasureInt;
        if (bScrolling && _scrollInfo->CanHorizontalScroll())
        {
            layoutSize.cx = Numeric::MeasureInt;
        }
        if (measureData.GetViewPort().top > 0)
        {
            viewPortSize = measureData.GetViewPort().Height();
        }
        else
        {
            viewPortSize = measureData.GetViewPort().bottom;
        }
    }

    //
    // 先计算第一个可视元素的索引和偏移
    //
    firstItemIndex = CalcFirstVisibleItemOffset(itemsOwner, measureData, bHori, _visibleOffset, extent);
    childIndex = firstItemIndex;
    _visibleStart = firstItemIndex;
    _visibleCount = 0;
    _realedCount = 0;
    
    int visibleOffset = _visibleOffset;

    if (bHori)
    {
        stackSize = extent.cx;
    }
    else
    {
        stackSize = extent.cy;
    }

    if (!IsScrolling())
    {
        visibleOffset = stackSize;
    }

    //
    // 计算在可视范围内的元素
    //
    if (icGenerator->StartAt(childIndex, -1))
    {
        while (icGenerator->HasNext())
        {
            Element* container = NULL;
            ItemEntry* item = itemColl->GetItemEntry(childIndex);
            if (itemsOwner->IsItemItsOwnContainer(item->GetItem()))
            {
                ++_realedCount;
                container = RTTICast<Element>(item->GetItem());
                InsertContainer(_visibleCount, container);
                icGenerator->LinkContainerToItem(container, item, false);
            }
            else
            {
                bool isNeedRecycle = false;
                ViewGroup* vGroup = RTTICast<ViewGroup>(item->GetItem());
                if (NULL != vGroup)
                {
                    container = icGenerator->GenerateGroup(vGroup);
                    InsertContainer(_visibleCount, container);                    
                    container->unref();
                }
                else
                {
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
            }

            ++childIndex;
            ++_visibleCount;

            if (NULL != container)
            {
                //container->WriteDoFlag(ViewFlags::IsNotInVisibileRange, false);
                ResetChildMeasureData(container->GetMeasureData(), measureData, layoutSize, extent, bHori);

                // 根据项容器和项生存容器内容
                icGenerator->PrepareItemContainer(container, item);
                container->Measure(layoutSize);

                Size desiredSize = container->GetDesiredSize();

                if (bHori)
                {
                    extent.cx += desiredSize.Width();
                    extent.cy = max(extent.cy, desiredSize.Height());
                    visibleMeasure = extent.cx - stackSize + _visibleOffset;
                }
                else
                {
                    extent.cy += desiredSize.Height();
                    extent.cx = max(extent.cx, desiredSize.Width());
                    visibleMeasure = extent.cy - stackSize + _visibleOffset;
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

    _visibleOffset = visibleOffset;

    Point offset(empty.left, empty.top);
    Size prevSize = extent;
    bool calcWid = !itemsOwner->ReadDoFlag(ViewFlags::IsNotCalcMaxWidth);

    //
    // 计算余下的高度和宽度
    //
    while (childIndex < itemCount)
    {
        Size size;
        ItemEntry* item = itemColl->GetItemEntry(childIndex);
        itemsOwner->ReadItemSize(item, bHori, childIndex, size);

        if (bHori)
        {
            extent.cx += size.cx;
            if (calcWid)
            {
                extent.cy = max(extent.cy, size.Height());
            }
        }
        else
        {
            extent.cy += size.cy;
            if (calcWid)
            {
                extent.cx = max(extent.cx, size.Width());
            }
        }
        ++childIndex;
    }

    if (bScrolling)
    {
        SetAndVerifyScrollingData(empty.ToSize(), extent, offset);
    }

    HandleMoreContainer();
    //itemsOwner->OnGeneratorStatusChanged(_visibleStart, _visibleCount);

    return extent;
}

void VirtualizingStackPanel::OnArrange(const Size& arrangeSize)
{
    int x = 0;
    int y = 0;
    bool bHori = GetOrientation() == Orientation::Horizontal;
    int width = 0;

    if (IsScrolling())
    {
        ScrollData* scrollData = GetScrollData();

        if (bHori)
        {
            x = _visibleOffset;
            y = -scrollData->computedOffset.y;
        }
        else
        {
            x = -scrollData->computedOffset.x;
            y = _visibleOffset;
        }
    }
    else
    {
        if (bHori)
        {
            x = _visibleOffset;
        }
        else
        {
            y = _visibleOffset;
        }
    }

    Rect finalRect(x, y, arrangeSize.cx, arrangeSize.cy);

    int iWid = arrangeSize.Width();
    int iHei = arrangeSize.Height();
    int index = 0;
    int count = _visibleCount;
    ElementColl* children = GetChildren();

    while (index < count)
    {
        Element* element = children->GetAt(index);

        if (element != NULL)
        {
            Size desiredSize = element->GetDesiredSize();

            if (bHori)
            {
                finalRect.left += width;
                width = desiredSize.cx;
                finalRect.right = finalRect.left + width;
                finalRect.bottom = max(iHei, desiredSize.cy);
            }
            else
            {
                finalRect.top += width;
                width = desiredSize.cy;
                finalRect.bottom = finalRect.top + width;
                finalRect.right = max(iWid, desiredSize.cx);
            }

            element->Arrange(finalRect);
        }
        index++;
    }
}

void VirtualizingStackPanel::InvalidateChildrenResourceReference()
{
    ElementColl* children = GetChildren();
    int count = children->GetCount();

    for (int j = 0; j < count; ++j)
    {
        FrameworkElement* fe = CASTTOFE(children->GetAt(j));
        if (fe != NULL)
        {
            fe->InvalidateResourceReference(false);
        }
    }
}

}
