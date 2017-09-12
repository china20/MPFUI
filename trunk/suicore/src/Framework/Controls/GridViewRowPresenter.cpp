/////////////////////////////////////////////////////////////////////////
// GridViewRowPresenter.cpp

#include <Framework/Controls/GridViewRowPresenter.h>
#include <Framework/Controls/TextBlock.h>
#include <Framework/Controls/Content.h>
#include <Framework/Controls/GridView.h>
#include <Framework/Presenter/ContentPresenter.h>

namespace suic
{

ImplementRTTIOfClass(GridViewRowPresenter, GridViewRowPresenterBase)

DpProperty* GridViewRowPresenter::ContentProperty;

void GridViewRowPresenter::StaticInit()
{
    if (NULL == ContentProperty)
    {
        GridViewRowPresenterBase::StaticInit();
        ContentControl::StaticInit();

        ContentProperty = ContentControl::ContentProperty->AddOwner(RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsMeasure, &GridViewRowPresenter::OnContentPropChanged));
    }
}

void GridViewRowPresenter::OnContentPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    GridViewRowPresenter* control = DynamicCast<GridViewRowPresenter>(d);
    if (control != NULL)
    {
        control->_content = e->GetNewValue();
        control->UpdateCells();
    }
}

//----------------------------------------------------

GridViewRowPresenter::GridViewRowPresenter()
    : _content(NULL)
{

}

GridViewRowPresenter::~GridViewRowPresenter()
{

}

void GridViewRowPresenter::UpdateCells()
{
    ElementColl* children = GetInternalChildren();
    ContentControl* templatedParent = RTTICast<ContentControl>(GetTemplatedParent());

    for (int i = 0; i < children->GetCount(); i++)
    {
        FrameworkElement* elem = RTTICast<FrameworkElement>(children->GetAt(i));
        ContentPresenter* presenter = RTTICast<ContentPresenter>(elem);

        if (presenter != NULL)
        {
            presenter->SetContent(GetContent());
        }
        else
        {
            elem->SetDataContext(GetContent());
        }

        if (templatedParent != NULL)
        {
            elem->SetVerticalAlignment(templatedParent->GetVerticalContentAlignment());
            elem->SetHorizontalAlignment(templatedParent->GetHorizontalContentAlignment());
        }
    }
}

void GridViewRowPresenter::CreateCellElement(ElementColl* children, GridViewColumn* column)
{
    FrameworkElement* cell = NULL;

    Binding* bind = column->GetDisplayMember();
    
    if (NULL != bind)
    {
        TextBlock* txt = new TextBlock();
        cell = txt;
        cell->ref();
        cell->SetDataContext(GetContent());
        cell->SetBinding(TextBlock::TextProperty, bind);
        txt->SetHorizontalAlignment(suic::HoriAlign::haLeft);
        //txt->SetTextAlignment(suic::TextAlignment::tCenter);
        cell->SetMargin(Rect(2, 0, 3, 0));
    }
    else
    {
        DataTemplate* cellTemplate = column->GetCellTemplate();
        ContentPresenter* contp = new ContentPresenter();

        if (NULL != cellTemplate)
        {
            contp->SetContentTemplate(cellTemplate);
        }

        contp->ref();
        contp->SetContent(GetContent());

        cell = contp;
    }

    if (NULL != cell)
    {
        ContentControl* tp = RTTICast<ContentControl>(GetTemplatedParent());
        if (NULL != tp)
        {
            cell->SetVerticalAlignment(tp->GetVerticalContentAlignment());
            cell->SetHorizontalAlignment(tp->GetHorizontalContentAlignment());
        }

        if (!cell->IsClipToBounds()) 
        {
            cell->SetClipToBounds(true);
        }

        children->Add(cell);
        cell->unref();
    }
}

void GridViewRowPresenter::OnPreApplyTemplate()
{
    GridViewRowPresenterBase::OnPreApplyTemplate();
    // 
    // 根据ColumnCollections和绑定的DataContext生成控件
    //
    if (_needReflesh)
    {
        _needReflesh = false;
        GridViewColumnCollection* colls = GetColumns();
        ElementColl* children = GetInternalChildren();

        children->Clear();

        if (NULL != colls)
        {
            for (int i = 0; i < colls->GetCount(); ++i)
            {
                GridViewColumn* column = colls->GetItem(i);
                CreateCellElement(children, column);
            }
        }
    }
}

Size GridViewRowPresenter::OnMeasure(const Size& constraintSize)
{
    Size size;
    bool shouldHide = false;
    GridViewColumnCollection* colls = GetColumns();
    int iSumSize = 0;
    ElementColl* children = GetInternalChildren();
    if (NULL != colls->GetOwner())
    {
        iSumSize = -((ScrollViewer*)colls->GetOwner())->GetHorizontalOffset();
    }

    for (int i = 0; i < children->GetCount(); ++i)
    {
        Element* elem = children->GetAt(i);
        GridViewColumn* column = colls->GetItem(i);
        Size measureSize;

        measureSize.cx = column->GetWidth();
        measureSize.cy = constraintSize.cy;

        iSumSize += measureSize.cx;

        if (iSumSize >= 0 && !shouldHide)
        {
            column->WriteDoFlag(ViewFlags::IsShouldHideColumn, false);
        }
        else
        {
            column->WriteDoFlag(ViewFlags::IsShouldHideColumn, true);
        }

        if (elem != NULL)
        {
            Size desiredSize;
            if (!column->ReadDoFlag(ViewFlags::IsShouldHideColumn))
            {
                MeasureChild(elem, measureSize);
            }
            desiredSize = elem->GetDesiredSize();
            column->SetActualWidth(max(desiredSize.Width(), column->GetActualWidth()));
            size.cy = max(size.cy, desiredSize.Height());
            size.cx += measureSize.Width();
        }
        else 
        {
            size.cx += column->GetWidth();
        }

        if (iSumSize > constraintSize.cx)
        {
            shouldHide = true;
        }
    }

    return size;
}

void GridViewRowPresenter::OnArrange(const Size& arrangeSize)
{
    int i = 0;
    Rect rect;
    ElementColl* children = GetInternalChildren();
    GridViewColumnCollection* colls = GetColumns();

    rect.bottom = arrangeSize.cy;

    for (i = 0; i < children->GetCount(); ++i)
    {
        Element* elem = children->GetAt(i);

        if (elem)
        {
            GridViewColumn* column = colls->GetItem(i);
            rect.right = rect.left + column->GetWidth();

            if (!column->ReadDoFlag(ViewFlags::IsShouldHideColumn))
            {
                elem->Arrange(rect);
                elem->WriteFlag(CoreFlags::IsNotRender, false);
            }
            else
            {
                elem->WriteFlag(CoreFlags::IsNotRender, true);
            }
        }

        rect.left = rect.right;
    }
}

}
