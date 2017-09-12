// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// UniformGrid.cpp

#include <Framework/Controls/UniformGrid.h>
#include <System/Tools/Math.h>

namespace suic
{

DpProperty* UniformGrid::ColumnsProperty;
DpProperty* UniformGrid::RowsProperty;

ImplementRTTIOfClass(UniformGrid, Panel)

UniformGrid::UniformGrid()
    : _columns(1)
    , _rows(1)
    , _firstColumn(0)
{
}

UniformGrid::~UniformGrid()
{
}

void UniformGrid::StaticInit()
{
    if (ColumnsProperty == NULL)
    {
        Panel::StaticInit();
        ColumnsProperty = DpProperty::Register(_T("Columns")
            , RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsMeasure));
        RowsProperty = DpProperty::Register(_T("Rows")
            , RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsMeasure));
    }
}

Size UniformGrid::OnMeasure(const Size& constraint) 
{ 
    UpdateComputedValues();

    Size childConstraint((Float) constraint.cx / (Float) _columns, (Float) constraint.cy / (Float) _rows);
    int maxChildDesiredWidth = 0;
    int maxChildDesiredHeight = 0;

    ElementColl* children = GetChildren(); 

    for (int i = 0, count = children->GetCount(); i < count; ++i) 
    { 
        Element* child = children->GetAt(i);

        MeasureChild(child, childConstraint);

        Size childDesiredSize = child->GetDesiredSize();

        if (maxChildDesiredWidth < childDesiredSize.cx)
        { 
            maxChildDesiredWidth = childDesiredSize.cx; 
        }

        if (maxChildDesiredHeight < childDesiredSize.cy)
        {
            maxChildDesiredHeight = childDesiredSize.cy;
        } 
    }

    return Size((maxChildDesiredWidth * _columns),(maxChildDesiredHeight * _rows)); 
}

void UniformGrid::OnArrange(const Size& arrangeSize)
{
    ElementColl* children = GetChildren(); 
    int childCount = children->GetCount();

    Rect childBounds(0, 0, (Float) arrangeSize.cx / (Float) _columns, (Float) arrangeSize.cy / (Float) _rows);
    int xStep = childBounds.Width();
    int yStep = childBounds.Height();
    int xBound = xStep * _columns - 1;

    childBounds.left += childBounds.Width() * _firstColumn;

    for (int i = 0; i < childCount; ++i)
    {
        Element* child = children->GetAt(i); 

        child->Arrange(childBounds);

        childBounds.left += xStep;
        childBounds.right += xStep;

        if (childBounds.left >= xBound) 
        {
            childBounds.top += yStep;
            childBounds.bottom += yStep; 
            childBounds.left = 0;
            childBounds.right = xStep;
        }
    }
}

void UniformGrid::SetColumns(int val)
{
    SetValue(ColumnsProperty, new Integer(val));
}

void UniformGrid::SetRows(int val)
{
    SetValue(RowsProperty, new Integer(val));
}

int UniformGrid::GetColumns()
{
    return GetValue(ColumnsProperty)->ToInt();
}

int UniformGrid::GetRows()
{
    return GetValue(RowsProperty)->ToInt();
}

void UniformGrid::SetFirstColumn(int val)
{
    _firstColumn = val;
}

int UniformGrid::GetFirstColumn()
{
    return _firstColumn;
}

void UniformGrid::UpdateComputedValues()
{ 
    _columns = GetColumns();
    _rows = GetRows();

    if (GetFirstColumn() >= _columns)
    { 
        SetFirstColumn(0); 
    }

    if ((_rows == 0) || (_columns == 0))
    { 
        int nonCollapsedCount = 0;
        ElementColl* children = GetChildren(); 

        for (int i = 0, count = children->GetCount(); i < count; ++i)
        { 
            Element* child = children->GetAt(i);

            if (!child->IsCollapsed())
            {
                nonCollapsedCount++; 
            }
        }

        if (nonCollapsedCount == 0)
        {
            nonCollapsedCount = 1;
        } 

        if (_rows == 0) 
        { 
            if (_columns > 0)
            { 
                _rows = (nonCollapsedCount + GetFirstColumn() + (_columns - 1)) / (Float) _columns;
            }
            else 
            {
                _rows = (int)sqrt((Float)nonCollapsedCount);
                if ((_rows * _rows) < nonCollapsedCount)
                {
                    _rows++;
                }
                _columns = _rows;
            } 
        }
        else if (_columns == 0) 
        { 
            _columns = (nonCollapsedCount + (_rows - 1)) / (Float) _rows; 
        }
    }
}

}
