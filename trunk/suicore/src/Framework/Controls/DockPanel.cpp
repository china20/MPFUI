// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// DockPanel.cpp

#include <System/Tools/Math.h>
#include <Framework/Controls/DockPanel.h>

namespace suic
{

ImplementRTTIOfClass(DockPanel, Panel)

DpProperty* DockPanel::DockProperty;
DpProperty* DockPanel::LastChildFillProperty;

DockPanel::DockPanel()
    : _isLastChildFill(true)
{
}

DockPanel::~DockPanel()
{
}

void DockPanel::OnLastChildFillPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    DockPanel* elem = DynamicCast<DockPanel>(d);
    if (elem)
    {
        elem->_isLastChildFill = e->GetNewValue()->ToBool();
    }
}

void DockPanel::StaticInit()
{
    if (DockProperty == NULL)
    {
        Panel::StaticInit();

        DockProperty = DpProperty::RegisterAttached(_U("Dock"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsParentArrange));
        LastChildFillProperty = DpProperty::Register(_U("LastChildFill"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsParentArrange, &DockPanel::OnLastChildFillPropChanged));

        DockProperty->SetConvertValueCb(DockConvert::Convert);
    }
}

int DockPanel::GetDock(Element* child)
{
    if (child == NULL)
    {
        throw ArgumentNullException(_U("element"), __FILELINE__);
    }
    return child->GetValue(DockProperty)->ToInt();
}

Size DockPanel::OnMeasure(const Size& constraint)
{
    Size desiredSize;
    ElementColl* children = GetChildren();

    int parentWidth       = 0;
    int parentHeight      = 0;
    int accumulatedWidth  = 0;
    int accumulatedHeight = 0;

    for (int i = 0, count = children->GetCount(); i < count; ++i) 
    { 
        Element* child = children->GetAt(i);
        Size childConstraint;
        Size childDesiredSize;

        if (child == NULL) 
        { 
            continue; 
        } 

        childConstraint = Size(max(0.0, constraint.Width() - accumulatedWidth), 
            max(0.0, constraint.Height() - accumulatedHeight));

        child->GetMeasureData()->SetViewPort(Rect(Point(), childConstraint));
        child->Measure(childConstraint);

        childDesiredSize = child->GetDesiredSize();

        switch (DockPanel::GetDock(child))
        { 
        case Dock::dockLeft:
        case Dock::dockRight:
            parentHeight = max(parentHeight, accumulatedHeight + childDesiredSize.Height());
            accumulatedWidth += childDesiredSize.Width(); 
            break;

        case Dock::dockTop: 
        case Dock::dockBottom:
            parentWidth = max(parentWidth, accumulatedWidth + childDesiredSize.Width()); 
            accumulatedHeight += childDesiredSize.Height();
            break;
        }
    }

    parentWidth = max(parentWidth, accumulatedWidth); 
    parentHeight = max(parentHeight, accumulatedHeight);

    return (suic::Size(parentWidth, parentHeight));
}

void DockPanel::OnArrange(const Size& arrangeSize)
{
    ElementColl* children = GetChildren();
    int childCount = children->GetCount();
    int nonFillCount = childCount - (IsLastChildFill() ? 1 : 0); 

    int iLeft = 0; 
    int iTop = 0; 
    int iRight = 0;
    int iBottom = 0; 

    for (int i = 0; i < childCount; ++i)
    {
        Element* child = children->GetAt(i);

        if (child != NULL) 
        {
            Size childDesiredSize = child->GetDesiredSize(); 
            Rect rcChild(iLeft, iTop
                , max(0, arrangeSize.cx - (iLeft + iRight))
                , max(0, arrangeSize.cy - (iTop + iBottom)));

            if (i < nonFillCount)
            {
                switch (DockPanel::GetDock(child)) 
                {
                case Dock::dockLeft: 
                    iLeft += childDesiredSize.cx;
                    rcChild.right = rcChild.left + childDesiredSize.cx;
                    break;

                case Dock::dockRight:
                    iRight += childDesiredSize.cx; 
                    rcChild.left = max(0.0, arrangeSize.cx - iRight); 
                    rcChild.right = rcChild.left + childDesiredSize.cx;
                    break; 

                case Dock::dockTop:
                    iTop += childDesiredSize.cy;
                    rcChild.bottom = rcChild.top + childDesiredSize.cy; 
                    break;

                case Dock::dockBottom: 
                    iBottom += childDesiredSize.cy;
                    rcChild.top = max(0.0, arrangeSize.cy - iBottom); 
                    rcChild.bottom = rcChild.top + childDesiredSize.cy;
                    break;
                }
            }

            child->Arrange(rcChild); 
        }
    }
}

}
