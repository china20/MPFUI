/////////////////////////////////////////////////////////////////////////
// GridViewColumnHeader.cpp

#include <Framework/Controls/GridViewColumn.h>
#include <Framework/Controls/GridViewColumnHeader.h>

namespace suic
{

ImplementRTTIOfClass(GridViewColumnHeader, ButtonBase)

int GridViewColumnHeader::_trackValue = 0;
DpProperty* GridViewColumnHeader::ColumnProperty;

void GridViewColumnHeader::StaticInit()
{
    if (NULL == ColumnProperty)
    {
        ButtonBase::StaticInit();

        ColumnProperty = DpProperty::Register(_U("Column"), RTTIType(), GridViewColumn::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsParentArrange));
    }
}

//----------------------------------------------------

GridViewColumnHeader::GridViewColumnHeader()
{
    _headerGripper = NULL;
    _owner = NULL;
}

GridViewColumnHeader::~GridViewColumnHeader()
{

}

void GridViewColumnHeader::OnThumbDragCompleted(DpObject* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
}

void GridViewColumnHeader::OnDoubleClickThumbDrag(Element* sneder, MouseButtonEventArg* e)
{
    GridViewColumn* column = GetColumn();

    if (column->GetWidth() != column->GetActualWidth())
    {
    }

    e->SetHandled(true);
}

void GridViewColumnHeader::OnThumbDragDelta(DpObject* sender, RoutedEventArg* dde)
{
    GridViewColumn* column = GetColumn();
    DragDeltaEventArg* e = (DragDeltaEventArg*)dde;

    e->SetHandled(true);

    if (column)
    {
        int iGripper = _headerGripper->GetActualWidth();
        int iNewWid = _trackValue + e->GetHorizontalChange();
        int iMin = column->GetMinWidth();
        int iMax = column->GetMaxWidth();

        if (iMin <= iGripper)
        {
            iMin = iGripper;
        }

        if (iMax < iMin)
        {
            iMax = iMin;
        }

        if (iNewWid >= iMin && iNewWid <= iMax)
        {
            column->SetWidth(iNewWid);
            if (_owner)
            {
                _owner->UpdateLayout();
            }
            else
            {
                throw InvalidValueException(_U("ListView is null"), __FILELINE__);
            }
        }
    }
}

void GridViewColumnHeader::OnThumbDragStarted(DpObject* sender, RoutedEventArg* e)
{
    GridViewColumn* column = GetColumn();
    if (column)
    {
        _trackValue = column->GetWidth();
    }
    else
    {
        _trackValue = 0;
    }
    e->SetHandled(true);
}

void GridViewColumnHeader::OnApplyTemplate()
{
    ButtonBase::OnApplyTemplate();
    _headerGripper = RTTICast<Thumb>(GetTemplateChild("PART_HeaderGripper"));

    if (_headerGripper)
    {
        RoutedEventHandler* pStarted = new RoutedEventHandler(this, &GridViewColumnHeader::OnThumbDragStarted);
        RoutedEventHandler* pDelta = new RoutedEventHandler(this, &GridViewColumnHeader::OnThumbDragDelta);
        RoutedEventHandler* pCompleted = new RoutedEventHandler(this, &GridViewColumnHeader::OnThumbDragCompleted);
        MouseButtonEventHandler* dblClick = new MouseButtonEventHandler(this, &GridViewColumnHeader::OnDoubleClickThumbDrag);

        _headerGripper->RemoveDragStarted(*pStarted);
        _headerGripper->RemoveDragDelta(*pDelta);
        _headerGripper->RemoveDragCompleted(*pCompleted);

        _headerGripper->AddDragStarted(pStarted);
        _headerGripper->AddDragDelta(pDelta);
        _headerGripper->AddDragCompleted(pCompleted);

        _headerGripper->RemoveMouseDoubleClick(dblClick);
        _headerGripper->AddMouseDoubleClick(dblClick);
    }
}

}
