// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Canvas.cpp

#include <Framework/Controls/Canvas.h>
#include <System/Tools/Debug.h>

namespace suic
{

ImplementRTTIOfClass(Canvas, Panel)

DpProperty* Canvas::LeftProperty;
DpProperty* Canvas::TopProperty;
DpProperty* Canvas::RightProperty;
DpProperty* Canvas::BottomProperty;

Canvas::Canvas()
{
}

Canvas::~Canvas()
{
}

void Canvas::StaticInit()
{
    if (LeftProperty == NULL)
    {
        Panel::StaticInit();
        LeftProperty = DpProperty::RegisterAttached(_U("Left")
            , RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsParentArrange));
        TopProperty = DpProperty::RegisterAttached(_U("Top")
            , RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsParentArrange));
        RightProperty = DpProperty::RegisterAttached(_U("Right")
            , RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsParentArrange));
        BottomProperty = DpProperty::RegisterAttached(_U("Bottom")
            , RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsParentArrange));
    }
}

int Canvas::GetLeft(suic::Element* element)
{ 
    return element->GetValue(LeftProperty)->ToInt();
}

int Canvas::GetTop(suic::Element* element)
{ 
    return element->GetValue(TopProperty)->ToInt();
}

int Canvas::GetRight(suic::Element* element)
{ 
    return element->GetValue(RightProperty)->ToInt();
}

int Canvas::GetBottom(suic::Element* element)
{ 
    return element->GetValue(BottomProperty)->ToInt();
}

void Canvas::SetLeft(Element* element, int val)
{
    element->SetValue(LeftProperty, new Integer(val));
}

void Canvas::SetTop(Element* element, int val)
{
    element->SetValue(TopProperty, new Integer(val));
}

void Canvas::SetRight(Element* element, int val)
{
    element->SetValue(RightProperty, new Integer(val));
}

void Canvas::SetBottom(Element* element, int val)
{
    element->SetValue(BottomProperty, new Integer(val));
}

suic::Size Canvas::OnMeasure(const suic::Size& size)
{
    ElementColl* coll = GetInternalChildren();

    for (int i = 0; i < coll->GetCount(); ++i)
    {
        suic::Element* child(coll->GetAt(i));

        if (child == NULL) 
        { 
            continue; 
        }

        child->Measure(size);
    }

    return suic::Size();
}

void Canvas::OnArrange(const suic::Size& arrangeSize)
{
    int count = 0;
    suic::ElementColl* children = GetInternalChildren();

    for(int i = 0, count = children->GetCount(); i < count; i++)
    {
        suic::Element* child = children->GetAt(i);

        if (child == NULL) 
        {
            continue;
        }

        int x = 0; 
        int y = 0; 

        int left = GetLeft(child);

        if (left != 0) 
        { 
            x = left;
        } 
        else
        {
            Float right = GetRight(child);

            if (right != 0)
            { 
                x = arrangeSize.Width() - child->GetDesiredSize().Width() - right; 
            }
        } 

        int top = GetTop(child);

        if (top != 0)
        { 
            y = top;
        } 
        else 
        {
            int bottom = GetBottom(child); 

            if (bottom != 0)
            {
                y = arrangeSize.Height() - child->GetDesiredSize().Height() - bottom; 
            }
        } 

        child->Arrange(suic::Rect(x, y, child->GetDesiredSize().Width(), child->GetDesiredSize().Height()));
    }
}

}
