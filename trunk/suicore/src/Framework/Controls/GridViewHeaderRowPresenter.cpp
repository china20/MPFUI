/////////////////////////////////////////////////////////////////////////
// GridViewHeaderRowPresenter.cpp

#include <Framework/Controls/GridView.h>
#include <Framework/Controls/GridViewColumnHeader.h>
#include <Framework/Controls/GridViewHeaderRowPresenter.h>

namespace suic
{

ImplementRTTIOfClass(GridViewHeaderRowPresenter, GridViewRowPresenterBase)

DpProperty* GridViewHeaderRowPresenter::ColumnHeaderToolTipProperty;
DpProperty* GridViewHeaderRowPresenter::ColumnHeaderTemplateProperty;
DpProperty* GridViewHeaderRowPresenter::ColumnHeaderContainerStyleProperty;

void GridViewHeaderRowPresenter::StaticInit()
{
    if (NULL == ColumnHeaderToolTipProperty)
    {
        GridView::StaticInit();
        GridViewRowPresenterBase::StaticInit();

        ColumnHeaderToolTipProperty = GridView::ColumnHeaderToolTipProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
        ColumnHeaderTemplateProperty = GridView::ColumnHeaderTemplateProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
        ColumnHeaderContainerStyleProperty = GridView::ColumnHeaderContainerStyleProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
    }
}

//----------------------------------------------------

GridViewHeaderRowPresenter::GridViewHeaderRowPresenter()
    : _svMain(NULL)
    , _svHeader(NULL)
    , _scollIsPending(0)
{

}

GridViewHeaderRowPresenter::~GridViewHeaderRowPresenter()
{

}

Style* GridViewHeaderRowPresenter::GetColumnHeaderContainerStyle()
{
    return RTTICast<Style>(GetValue(ColumnHeaderContainerStyleProperty));
}

void GridViewHeaderRowPresenter::SetColumnHeaderContainerStyle(Style* val)
{
    SetValue(ColumnHeaderContainerStyleProperty, val);
}

ControlTemplate* GridViewHeaderRowPresenter::GetColumnHeaderTemplate()
{
    return RTTICast<ControlTemplate>(GetValue(ColumnHeaderTemplateProperty));
}

void GridViewHeaderRowPresenter::SetColumnHeaderTemplate(ControlTemplate* val)
{
    SetValue(ColumnHeaderTemplateProperty, val);
}

void GridViewHeaderRowPresenter::OnMainScrollChanged(Element* sender, ScrollChangedEventArg* e)
{
    if (0 == _scollIsPending && (NULL != _svHeader) && (_svMain == e->GetOriginalSource()))
    {
        _scollIsPending = 1;
        _svHeader->SetHorizontalOffset(e->GetHoriScrollOffset());
        _scollIsPending = 0;
    }
}

void GridViewHeaderRowPresenter::OnHeaderScrollChanged(Element* sender, ScrollChangedEventArg* e)
{
    if (0 == _scollIsPending && (NULL != _svMain) && (_svHeader == e->GetOriginalSource()))
    {
        _scollIsPending = 2;
        _scollIsPending = 0;
    }
}

void GridViewHeaderRowPresenter::AttatchToScrollView()
{
    // 直接滚动视图
    ScrollViewer* svParent = RTTICast<ScrollViewer>(GetParent());
    // ListView的滚动视图
    ScrollViewer* svListView = RTTICast<ScrollViewer>(GetTemplatedParent());

    if (svParent != _svHeader)
    {
        if (_svHeader)
        {
            ScrollChangedEventHandler scEv(this, &GridViewHeaderRowPresenter::OnHeaderScrollChanged);
            _svHeader->RemoveScrollChanged(&scEv);
        }

        if (svParent)
        {
            ScrollChangedEventHandler* scEv = new ScrollChangedEventHandler(this, &GridViewHeaderRowPresenter::OnHeaderScrollChanged);
            svParent->AddScrollChanged(scEv);
        }

        _svHeader = svParent;
    }

    if (svListView != _svMain)
    {
        if (_svMain)
        {
            ScrollChangedEventHandler scEv(this, &GridViewHeaderRowPresenter::OnMainScrollChanged);
            _svMain->RemoveScrollChanged(&scEv);
        }

        if (svListView)
        {
            ScrollChangedEventHandler* scEv = new ScrollChangedEventHandler(this, &GridViewHeaderRowPresenter::OnMainScrollChanged);
            svListView->AddScrollChanged(scEv);
        }

        _svMain = svListView;
    }
}

void GridViewHeaderRowPresenter::OnPreApplyTemplate()
{
    GridViewRowPresenterBase::OnPreApplyTemplate();

    // 
    // 根据ColumnCollections和Header生成控件
    // GridViewHeaderColumn
    //
    if (_needReflesh)
    {
        GridViewColumnCollection* colls = GetColumns();

        _needReflesh = false;
        // 挂接滚动事件
        AttatchToScrollView();
        
        if (NULL != colls)
        {
            ElementColl* children = GetInternalChildren();
            for (int i = 0; i < colls->GetCount(); ++i)
            {
                GridViewColumn* column = colls->GetItem(i);
                Object* header = column->GetHeader();
                GridViewColumnHeader* hcoll = new GridViewColumnHeader();
                ControlTemplate* ctrlTemp = GetColumnHeaderTemplate();

                hcoll->ref();

                hcoll->SetStyle(GetColumnHeaderContainerStyle());

                if (header)
                {
                    hcoll->SetContent(header);
                }

                if (ctrlTemp)
                {
                    hcoll->SetTemplate(ctrlTemp);
                }

                if (column->GetHeaderTemplate())
                {
                    hcoll->SetContentTemplate(column->GetHeaderTemplate());
                }

                hcoll->_owner = _svMain;
                colls->SetOwner(_svMain);
                hcoll->SetColumn(column);
                children->Add(hcoll);

                hcoll->unref();
            }
        }
    }
}

Size GridViewHeaderRowPresenter::OnMeasure(const Size& constraintSize)
{
    Size size;
    bool shouldHide = false;
    int iSumSize = -_svHeader->GetHorizontalOffset();
    ElementColl* children = GetInternalChildren();

    for (int i = 0; i < children->GetCount(); ++i)
    {
        GridViewColumnHeader* elem = RTTICast<GridViewColumnHeader>(children->GetAt(i));
        GridViewColumn* column = elem->GetColumn();
        Size measureSize;

        measureSize.cx = column->GetWidth();

        if (elem != NULL)
        { 
            measureSize.cy = constraintSize.cy;
            iSumSize += measureSize.cx;

            if (iSumSize >= 0 && !shouldHide)
            {
                MeasureChild(elem, measureSize);
                column->WriteDoFlag(ViewFlags::IsShouldHideColumn, false);
            }
            else
            {
                column->WriteDoFlag(ViewFlags::IsShouldHideColumn, true);
            }

            column->SetActualWidth(elem->GetDesiredSize().Width());
            size.cy = max(size.cy, elem->GetDesiredSize().Height());
            if (iSumSize > constraintSize.cx)
            {
                shouldHide = true;
            }
        }

        size.cx += measureSize.cx;
    }

    return size;
}

void GridViewHeaderRowPresenter::OnArrange(const Size& arrangeSize)
{
    Rect rect;
    ElementColl* children = GetInternalChildren();

    rect.bottom = arrangeSize.cy;

    for (int i = 0; i < children->GetCount(); ++i)
    {
        GridViewColumnHeader* elem = RTTICast<GridViewColumnHeader>(children->GetAt(i));

        if (elem)
        {
            GridViewColumn* column = elem->GetColumn();
            Size desiredSize = elem->GetDesiredSize();

            rect.right = rect.left + column->GetWidth();

            if (!column->ReadDoFlag(ViewFlags::IsShouldHideColumn))
            {
                elem->Arrange(rect);
            }
        }

        rect.left = rect.right;
    }
}

}
