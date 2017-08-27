
#include <Framework/Controls/ScrollViewer.h>
#include <Framework/Presenter/ItemsPresenter.h>
#include <Framework/Presenter/ScrollContentPresenter.h>

namespace suic
{

ImplementRTTIOfClass(ScrollContentPresenter, ContentPresenter)

ScrollContentPresenter::ScrollContentPresenter()
    : _scrollInfo(NULL)
    , _scrollClient(false)
{
    _adornerLayer = new AdornerLayer();
    _adornerLayer->ref();
}

ScrollContentPresenter::~ScrollContentPresenter()
{
    if (_scrollInfo && _scrollClient)
    {
        delete _scrollInfo;
    }
    _adornerLayer->unref();
}

int ScrollContentPresenter::GetVisualChildrenCount()
{
    if (GetTemplateChild() != NULL)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

Element* ScrollContentPresenter::GetVisualChild(int index)
{
    switch (index)
    {
    case 0:
        return ContentPresenter::GetTemplateChild();

    case 1:
        return _adornerLayer;

    default:

        return NULL;
    }
}

bool ScrollContentPresenter::CanContentScroll()
{
    return true;
}

IScrollInfo* ScrollContentPresenter::GetScrollInfo()
{
    if (_scrollClient)
    {
        return _scrollInfo;
    }
    else
    {
        return NULL;
    }
}

ScrollData* ScrollContentPresenter::GetScrollData()
{
    ScrollInfo* info = dynamic_cast<ScrollInfo*>(_scrollInfo);
    if (NULL == info)
    {
        return NULL;
    }
    else
    {
        return info->GetScrollData();
    }
}

void ScrollContentPresenter::EnsureScrollInfo()
{
    if (!_scrollInfo)
    {
        _scrollInfo = new ScrollInfo();
        ((ScrollInfo*)_scrollInfo)->SetOwner(this);
    }
}

void ScrollContentPresenter::OnApplyTemplate()
{
}

void ScrollContentPresenter::LinkScrollInfo()
{
    if (_scrollClient && _scrollInfo)
    {
        delete _scrollInfo;
        _scrollInfo = NULL;
    }

    // 取得模板父元素
    // 此元素作为ScrollViewer的ControlTemplate一部分存在
    ScrollViewer* templatedParent = DynamicCast<ScrollViewer>(GetTemplatedParent());

    if (templatedParent != NULL)
    {
        FrameworkElement* fe = NULL;
        IScrollInfo* content = NULL;

        // 是否支持滚动（其值绑定ScrollViewer得到）
        if (CanContentScroll())
        {
            // 实现接口支持IScrollInfo接口
            content = DynamicCast<IScrollInfo>(GetContent());
            fe = DynamicCast<FrameworkElement>(GetContent());

            if (content == NULL && fe != NULL)
            {
                // 通过组合ScrollInfo实现IScrollInfo
                content = fe->GetScrollInfo();

                // 本身支持滚动
                if (content == NULL)
                {
                    ItemsPresenter* reference = DynamicCast<ItemsPresenter>(GetContent());

                    if (reference == NULL)
                    {
                        FrameworkElement* element = DynamicCast<FrameworkElement>(templatedParent->GetTemplatedParent());
                        if (element != NULL)
                        {
                            reference = DynamicCast<ItemsPresenter>(element->GetTemplateChild(_U("ItemsPresenter")));
                        }
                    }

                    if (reference != NULL)
                    {
                        FrameworkElement* itemPanel = NULL;
                        // 应用模板
                        // ItemsPresenter只有一个子元素，而且必须从Panel继承
                        reference->ApplyTemplate();
                        itemPanel = reference->GetTemplateChild();

                        if (itemPanel != NULL)
                        {
                            content = DynamicCast<IScrollInfo>(itemPanel);
                            if (!content)
                            {
                                content = itemPanel->GetScrollInfo();
                            }
                        }
                    }
                    else
                    {
                        fe = templatedParent->FindElem<FrameworkElement>(_U("PART_ScrollHost"));
                        content = DynamicCast<IScrollInfo>(fe);
                        if (content)
                        {
                            content = fe->GetScrollInfo();
                        }
                    }
                }
            }
        }

        // 没有找到IScrollInfo接口
        // 将使用自身实现的IScrollInfo
        if (content == NULL)
        {
            _scrollClient = true;
            EnsureScrollInfo();
        }
        else
        {
            // 找到，保存指针
            _scrollClient = false;
            _scrollInfo = content;
        }

        // 设置ScrollViewer
        _scrollInfo->SetScrollOwner(templatedParent);
        templatedParent->SetScrollInfo(_scrollInfo);
    }
    else if (_scrollInfo != NULL)
    {
        if (_scrollInfo->GetScrollOwner() != NULL)
        {
            _scrollInfo->GetScrollOwner()->SetScrollInfo(NULL);
        }

        _scrollInfo->SetScrollOwner(NULL);

        // 如果是本身实现，删除
        if (_scrollClient)
        {
            delete _scrollInfo;
        }

        _scrollInfo = NULL;
    }

    //MessageBox(NULL, _T("1"), _T("tt"), MB_OK);
}

Size ScrollContentPresenter::OnMeasure(const Size& constraint)
{
    Size size;
    int count = GetVisualChildrenCount();
    ScrollData* scrollData = GetScrollData();

    if (NULL == scrollData)
    {
        LinkScrollInfo();
        scrollData = GetScrollData();
    }

    if (count > 0)
    {
        _adornerLayer->Measure(constraint);

        if (!IsScrollClient())
        {
            size = ContentPresenter::OnMeasure(constraint);
        }
        else
        {
            Size measureSize = constraint;

            if (_scrollInfo->CanHorizontalScroll())
            {
                measureSize.cx = suic::Numeric::MeasureInt;
            }
            if (_scrollInfo->CanVerticalScroll())
            {
                measureSize.cy = suic::Numeric::MeasureInt;
            }

            size = ContentPresenter::OnMeasure(measureSize);
        }
    }

    if (IsScrollClient())
    {
        VerifyScrollData(constraint, size);
    }

    size.cx = min(constraint.cx, size.cx);
    size.cy = min(constraint.cy, size.cy);

    return size;
}

void ScrollContentPresenter::OnArrange(const Size& arrangeSize)
{
    int count = GetVisualChildrenCount();
    ScrollData* scrollData = GetScrollData();

    if (IsScrollClient())
    {
        VerifyScrollData(arrangeSize, scrollData->extent);
    }

    if (count <= 0)
    {
        return;
    }

    _adornerLayer->Arrange(Rect(Point(), arrangeSize));

    Element* child = GetVisualChild(0);
    if (child != NULL)
    {
        int x = 0;
        int y = 0;
        int w = max(child->GetDesiredSize().Width(), arrangeSize.Width());
        int h = max(child->GetDesiredSize().Height(), arrangeSize.Height());
        
        if (IsScrollClient())
        {
            x = -scrollData->computedOffset.x;
            y = -scrollData->computedOffset.y;
        }

        Rect finalRect(x, y, w, h);
        child->Arrange(finalRect);
    }
}

FrameworkElement* ScrollContentPresenter::GetTemplateChild()
{
    return ContentPresenter::GetTemplateChild();
}

void ScrollContentPresenter::SetTemplateChild(FrameworkElement* elem)
{
    FrameworkElement* templateChild = ContentPresenter::GetTemplateChild();
    
    if (elem != templateChild)
    {
        if ((templateChild != NULL) && (elem == NULL))
        {
            ContentPresenter::RemoveVisualChild(_adornerLayer);
        }

        ContentPresenter::SetTemplateChild(elem);

        if ((templateChild == NULL) && (elem != NULL))
        {
            ContentPresenter::AddVisualChild(_adornerLayer);
        }
    }
}

void ScrollContentPresenter::VerifyScrollData(Size viewport, Size extent)
{
    int flag = 0;
    if (viewport.cx < 0)
    {
        viewport.cx = extent.cx;
    }
    if (viewport.cy < 0)
    {
        viewport.cy = extent.cy;
    }

    ScrollData* scrollData = GetScrollData();

    if (viewport != scrollData->viewport)
    {
        flag++;
    }
    if (extent != scrollData->extent)
    {
        flag++;
    }

    Point offset = scrollData->offset;

    offset.x = max(0.0, min(offset.x, extent.Width() - viewport.Width()));
    offset.y = max(0.0, min(offset.y, extent.Height() - viewport.Height()));

    //scrollData->offset = offset;
    scrollData->viewport = viewport;
    scrollData->extent = extent;
    scrollData->SetComputedOffset(offset);

    if (flag > 0)
    {
        GetScrollInfo()->GetScrollOwner()->InvalidateScrollInfo(true);
    }
}

}
