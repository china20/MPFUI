
#include <Framework/Controls/StackPanel.h>
#include <Framework/Controls/ScrollViewer.h>

#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>

#include <System/Tools/Debug.h>
#include <System/Tools/Math.h>

namespace suic
{

ImplementRTTIOfClass(StackPanel, Panel)

DpProperty* StackPanel::OrientationProperty;

StackPanel::StackPanel()
{
    _scrollInfo = NULL;
    _visibleOffset = 0;
    _visibleCount = 0;
    _visibleIndex = 0;
}

StackPanel::~StackPanel()
{
    if (NULL != _scrollInfo)
    {
        delete _scrollInfo;
    }
}

IScrollInfo* StackPanel::GetScrollInfo()
{
    if (NULL == _scrollInfo)
    {
        _scrollInfo = new ScrollInfo();
        _scrollInfo->SetOwner(this);
    }
    return _scrollInfo;
}

ScrollData* StackPanel::GetScrollData()
{
    GetScrollInfo();
    return _scrollInfo->GetScrollData();
}

bool StackPanel::IsScrolling()
{
    return (_scrollInfo != NULL && _scrollInfo->GetScrollOwner() != NULL);
}

void StackPanel::OnOrientationPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    //ResetScrolling(d);
}

void StackPanel::StaticInit()
{
    if (OrientationProperty == NULL)
    {
        Panel::StaticInit();
        OrientationProperty = DpProperty::Register(_T("Orientation"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(OrientationBox::VerticalBox, PropMetadataOptions::AffectsMeasure, &StackPanel::OnOrientationPropChanged));
        OrientationProperty->SetConvertValueCb(OrientationConvert::Convert);
    }
}

int StackPanel::GetVisibleChildrenCount()
{
    return _visibleCount;
}

Element* StackPanel::GetVisibleChild(int index)
{
    return Panel::GetVisualChild(_visibleIndex + index);
}

void StackPanel::OnInitialized(EventArg* e)
{
    Panel::OnInitialized(e);
}

void StackPanel::OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e)
{
    _visibleCount = 0;
    Panel::OnItemsChangedInternal(sender, e);
}

void StackPanel::NotifyItemChanged()
{
    Panel::NotifyItemChanged();
    _visibleCount = 0;
}

int StackPanel::ComputeOffsetFromItem(suic::Object* item, eItemDirection id, int& offset, int& itemSize)
{
    int index = -1;
    bool bHori = GetOrientation() == Orientation::Horizontal;
    ElementColl* children = GetChildren();

    bool bNext = false;
    int prevOffset = 0;

    offset = 0;

    for (int i = 0; i < children->GetCount(); ++i)
    {
        Element* child = children->GetAt(i);

        if (child != NULL)
        {
            if (bHori)
            {
                itemSize = child->GetDesiredSize().Width();
            }
            else
            {
                itemSize = child->GetDesiredSize().Height();
            }

            if (bNext || child->GetContainerItem() == item || child == item)
            {
                index = i;

                if (!bNext && eItemDirection::idNext == id)
                {
                    bNext = true;
                }
                else
                {
                    if (eItemDirection::idPrev == id && index > 0)
                    {
                        --index;
                        itemSize = offset - prevOffset;
                        offset = prevOffset;
                    }

                    break;
                }
            }

            prevOffset = offset;
            offset += itemSize;
        }
    }

    if (-1 == index)
    {
        offset = 0;
        itemSize = 0;
    }

    return index;
}

void StackPanel::EnsureScrollingData(Size viewport, Size extent, Point offset)
{
    bool flag = true;
    flag &= viewport == GetScrollData()->viewport;
    flag &= extent == GetScrollData()->extent;
    flag &= offset == GetScrollData()->computedOffset;
    GetScrollData()->offset = offset;

    if (!flag)
    {
        GetScrollData()->viewport = viewport;
        GetScrollData()->extent = extent;
        GetScrollData()->SetComputedOffset(offset);
        
        OnScrollChange();
    }
}

Size StackPanel::OnMeasure(const Size& constraint)
{
    Size size;

    DWORD dwTime = Environment::GetSystemTick();

        ElementColl* children = GetChildren();
        ScrollData* scrollData = GetScrollData();
        Size availableSize = constraint;
        bool bHori = GetOrientation() == Orientation::Horizontal;
        Point offset = GetScrollData()->offset;
        int iOffset = 0;
        _visibleOffset = 0;
        _visibleIndex = -1;
        _visibleCount = 0;

        // 确定传给子元素的限制大小
        if (bHori)
        {
            availableSize.cx = Numeric::MeasureInt;
            if (IsScrolling() && _scrollInfo->CanVerticalScroll())
            {
                availableSize.cy = Numeric::MeasureInt;
                iOffset = offset.x;
            }
        }
        else
        {
            availableSize.cy = Numeric::MeasureInt;
            if (IsScrolling() && _scrollInfo->CanHorizontalScroll())
            {
                availableSize.cx = Numeric::MeasureInt;
                iOffset = offset.y;
            }
        }

        int num5 = 0;
        int count = children->GetCount();

        while (num5 < count)
        {
            Size desiredSize;
            Element* element = children->GetAt(num5);
            if (element != NULL)
            {
                MeasureChild(element, availableSize);
                desiredSize = element->GetDesiredSize();
                ItemEntry* itemb = element->GetItemEntry();

                if (itemb != NULL)
                {
                    itemb->SetSize(desiredSize);
                }

                if (bHori)
                {
                    size.cx += desiredSize.Width();
                    size.cy = max(size.Height(), desiredSize.Height());
                    iOffset -= desiredSize.Width();
                }
                else
                {
                    size.cx = max(size.Width(), desiredSize.Width());
                    size.cy += desiredSize.Height();
                    iOffset -= desiredSize.Height();
                }
            }

            if (_visibleIndex == -1 && iOffset < 0)
            {
                _visibleIndex = num5;
                if (bHori)
                {
                    iOffset = size.cx - offset.x;
                    _visibleOffset = iOffset - desiredSize.Width();
                    iOffset = constraint.cx - iOffset;
                }
                else
                {
                    iOffset = size.cy - offset.y;
                    _visibleOffset = iOffset - desiredSize.Height();
                    iOffset = constraint.cy - iOffset;
                }
            }
            else if (_visibleCount == 0 && iOffset < 0)
            {
                _visibleCount = num5 - _visibleIndex + 1;
            }

            num5++;
        }

        if (_visibleIndex == -1)
        {
            _visibleIndex = 0;
        }

        if (_visibleCount == 0)
        {
            _visibleCount = count - _visibleIndex;
        }

        if (!IsScrolling())
        {
            return size;
        }

        Size viewport = constraint;
        Size extent = size;

        if (bHori)
        {
            scrollData->viewport.cx = viewport.Width();
            offset.y = max(0.0, min(offset.x, extent.Height() - viewport.Height()));
        }
        else
        {
            scrollData->viewport.cy = viewport.Height();
            offset.x = max(0.0, min(offset.x, extent.Width() - viewport.Width()));
        }

        size.cx = min(size.Width(), constraint.Width());
        size.cy = min(size.Height(), constraint.Height());

    EnsureScrollingData(viewport, extent, offset);

    return size;
}

void StackPanel::OnArrange(const Size& arrangeSize)
{
    //try
    {
        //DWORD dwTime = Environment::GetSystemTick();

        bool bLeftToRight = (GetFlowDirection() == FlowDirection::LeftToRight);
        bool bHori = GetOrientation() == Orientation::Horizontal;
        Rect finalRect(0, 0, arrangeSize.cx, arrangeSize.cy);
        int width = 0;
        bool bEquals = _finalArrange.Width() == arrangeSize.cx && _finalArrange.Height() == arrangeSize.cy;

        if (IsScrolling())
        {
            if (bHori)
            {
                if (bLeftToRight)
                {
                    finalRect.left = _visibleOffset;
                }
                else
                {
                    finalRect.right = arrangeSize.cx - _visibleOffset;
                }
                finalRect.top = -GetScrollData()->computedOffset.y;
            }
            else
            {
                finalRect.left = -GetScrollData()->computedOffset.x;
                finalRect.top = _visibleOffset;
            }
        }

        int index = 0;
        int count = GetVisibleChildrenCount();
        ElementColl* children = GetChildren();

        for (index = 0; index < count; ++index)
        {
            Element* child = children->GetAt(_visibleIndex + index);

            if (child != NULL)
            {
                if (bHori)
                {
                    if (bLeftToRight)
                    {
                        finalRect.left += width;
                        width = child->GetDesiredSize().Width();
                        finalRect.right = finalRect.left + width;
                    }
                    else
                    {
                        finalRect.right -= width;
                        width = child->GetDesiredSize().Width();
                        finalRect.left = finalRect.right - width;
                    }

                    finalRect.bottom = max(arrangeSize.Height(), child->GetDesiredSize().Height());
                }
                else
                {
                    finalRect.top += width;
                    width = child->GetDesiredSize().Height();
                    finalRect.bottom = finalRect.top + width;
                    finalRect.right = max(arrangeSize.Width(), child->GetDesiredSize().Width());
                }

                child->Arrange(finalRect);
            }
        }

        //SystemLog()->Warn(String().Format(_U("StackPanel Arrange Cost : %ld, Count:%d\n")
        //    , Environment::GetSystemTick()-dwTime, count));
    }
    /*catch (Exception& e)
    {
        Debug::Trace(e.GetErrorMsg().c_str());
    }*/
}

void StackPanel::OnScrollChange()
{
    if (GetScrollInfo()->GetScrollOwner() != NULL)
    {
        GetScrollInfo()->GetScrollOwner()->InvalidateScrollInfo(true);
    }
}

}
