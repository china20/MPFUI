/////////////////////////////////////////////////////////////////////////
// GridViewRowPresenterBase.cpp

#include <Framework/Controls/GridViewColumnCollection.h>
#include <Framework/Controls/GridViewRowPresenterBase.h>

namespace suic
{

ImplementRTTIOfClass(GridViewRowPresenterBase, FrameworkElement)

DpProperty* GridViewRowPresenterBase::ColumnsProperty;

void GridViewRowPresenterBase::OnColumnsPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    GridViewRowPresenterBase* pGrid = DynamicCast<GridViewRowPresenterBase>(d);
    if (pGrid != NULL)
    {
        pGrid->_columns = DynamicCast<GridViewColumnCollection>(e->GetNewValue());
    }
}

void GridViewRowPresenterBase::StaticInit()
{
    if (NULL == ColumnsProperty)
    {
        FrameworkElement::StaticInit();
        ColumnsProperty = DpProperty::Register(_U("Columns"), RTTIType(), GridViewColumnCollection::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &GridViewRowPresenterBase::OnColumnsPropChanged));
    }
}

//----------------------------------------------------

GridViewRowPresenterBase::GridViewRowPresenterBase()
    : _needReflesh(true)
    , _columns(NULL)
{
    _uiElems = NULL;
}

GridViewRowPresenterBase::~GridViewRowPresenterBase()
{
    if (_uiElems)
    {
        delete _uiElems;
    }
}

GridViewColumnCollection* GridViewRowPresenterBase::GetColumns()
{
    return _columns;
}

int GridViewRowPresenterBase::GetVisualChildrenCount()
{
    if (_uiElems == NULL)
    {
        return 0;
    }
    else
    {
        return _uiElems->GetCount();
    }
}

Element* GridViewRowPresenterBase::GetVisualChild(int index)
{
    return (*_uiElems)[index];
}

Element* GridViewRowPresenterBase::GetLogicalChild(int index)
{
    return GetVisualChild(index);
}

int GridViewRowPresenterBase::GetLogicalChildrenCount()
{
    return GetVisualChildrenCount();
}

ElementColl* GridViewRowPresenterBase::GetInternalChildren()
{
    if (NULL == _uiElems)
    {
        _uiElems = new ElementColl(this, this);
    }
    return _uiElems;
}

Element* GridViewRowPresenterBase::GetInternalChild(int index)
{
    return GetInternalChildren()->GetAt(index);
}

}
