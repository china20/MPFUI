
//#include "stdafx.h"

#include <Extend/Controls/HyperPanel.h>
#include <System/Tools/Math.h>
#include <System/Types/Numeric.h>
#include <System/Animation/Transform.h>

namespace suic
{

ImplementRTTIOfClass(HyperPanel, suic::Panel)

suic::DpProperty* HyperPanel::CenterProperty;
suic::DpProperty* HyperPanel::DistributionProperty;
suic::DpProperty* HyperPanel::OrientationProperty;
suic::DpProperty* HyperPanel::ApplyOpacityProperty;
suic::DpProperty* HyperPanel::MinElementScaleProperty;
suic::DpProperty* HyperPanel::VerticalContentAlignmentProperty;
suic::DpProperty* HyperPanel::HorizontalContentAlignmentProperty;

void HyperPanel::StaticInit()
{
    if (NULL == CenterProperty)
    {
        CenterProperty = suic::DpProperty::Register(_U("Center"), RTTIType(), suic::OFloat::RTTIType(), suic::DpPropMemory::GetPropMeta(new suic::OFloat(0.5), suic::PropMetadataOptions::AffectsMeasure, &HyperPanel::OnCenterPropChanged));
        DistributionProperty = suic::DpProperty::Register(_U("Distribution"), RTTIType(), suic::OFloat::RTTIType(), suic::DpPropMemory::GetPropMeta(new suic::OFloat(0.5), suic::PropMetadataOptions::AffectsMeasure, &HyperPanel::OnDistributionPropChanged));
        OrientationProperty = suic::DpProperty::Register(_U("Orientation"), RTTIType(), suic::Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::OrientationBox::HorizontalBox, suic::PropMetadataOptions::AffectsMeasure, &HyperPanel::OnOrientationPropChanged));
        ApplyOpacityProperty = suic::DpProperty::Register(_U("ApplyOpacity"), RTTIType(), suic::Boolean::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Boolean::True, suic::PropMetadataOptions::AffectsMeasure, &HyperPanel::OnApplyOpacityPropChanged));
        MinElementScaleProperty = suic::DpProperty::Register(_U("MinElementScale"), RTTIType(), suic::OFloat::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::OFloat::GetZeroFloat(), suic::PropMetadataOptions::AffectsMeasure, &HyperPanel::OnMinElementScalePropChanged));
        VerticalContentAlignmentProperty = suic::DpProperty::Register(_U("VerticalContentAlignment"), RTTIType(), suic::Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::VertAlignBox::CenterBox, suic::PropMetadataOptions::AffectsMeasure, &HyperPanel::OnVerticalContentAlignmentPropChanged));
        HorizontalContentAlignmentProperty = suic::DpProperty::Register(_U("HorizontalContentAlignment"), RTTIType(), suic::Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::HoriAlignBox::CenterBox, suic::PropMetadataOptions::AffectsMeasure, &HyperPanel::OnHorizontalContentAlignmentPropChanged));
    }
}

void HyperPanel::OnCenterPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    HyperPanel* panel = suic::RTTICast<HyperPanel>(d);
    suic::Float oldValue = e->GetOldValue()->ToFloat();
    panel->OnCenterChanged(oldValue);
}

void HyperPanel::OnDistributionPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    HyperPanel* panel = suic::RTTICast<HyperPanel>(d);
    suic::Float oldValue = e->GetOldValue()->ToFloat();
    panel->OnDistributionChanged(oldValue);
}

void HyperPanel::OnDistributionChanged(suic::Float oldValue)
{
    suic::Float num = (suic::Float)(UFMAX(0.1f, UFMIN(1.0f, GetDistribution())));
    if (!suic::FloatUtil::AreClose(num, GetDistribution()))
    {
        SetDistribution(num);
    }
    InvalidateMeasure();
}

void HyperPanel::OnOrientationPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    HyperPanel* panel = suic::RTTICast<HyperPanel>(d);
    if (e->GetOldValue()->ToInt() != e->GetNewValue()->ToInt())
    {
        panel->InvalidateMeasure();
    }
}

void HyperPanel::OnApplyOpacityPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    HyperPanel* panel = suic::RTTICast<HyperPanel>(d);
    bool oldValue = e->GetOldValue()->ToBool();
    panel->OnApplyOpacityChanged(oldValue);
}

void HyperPanel::OnApplyOpacityChanged(bool oldValue)
{
    if (GetApplyOpacity())
    {
        InvalidateMeasure();
    }
    else
    {
        suic::ElementColl* children = GetChildren(); 

        for (int i = 0; i < children->GetCount(); i++)
        {
            suic::Element* elem = children->GetAt(i);
            if (elem == NULL)
            {
                elem->SetOpacity(1.0f);
            }
        }
    }
}

void HyperPanel::OnMinElementScalePropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    HyperPanel* panel = suic::RTTICast<HyperPanel>(d);
    suic::Float oldValue = e->GetOldValue()->ToFloat();
    panel->OnMinElementScaleChanged(oldValue);
}

void HyperPanel::OnMinElementScaleChanged(suic::Float oldValue)
{
    suic::Float num = (suic::Float)(UFMAX(0.1f, UFMIN(1.0f, GetMinElementScale())));
    if (!suic::FloatUtil::AreClose(num, GetMinElementScale()))
    {
        SetMinElementScale(num);
    }
    InvalidateMeasure();
}

void HyperPanel::OnHorizontalContentAlignmentPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    HyperPanel* panel = suic::RTTICast<HyperPanel>(d);
    if (e->GetOldValue()->ToInt() != e->GetNewValue()->ToInt())
    {
        panel->InvalidateMeasure();
    }
}

void HyperPanel::OnVerticalContentAlignmentPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    HyperPanel* panel = suic::RTTICast<HyperPanel>(d);
    if (e->GetOldValue()->ToInt() != e->GetNewValue()->ToInt())
    {
        panel->InvalidateMeasure();
    }
}

HyperPanel::HyperPanel()
{
    StaticInit();
}

HyperPanel::~HyperPanel()
{

}

suic::Orientation HyperPanel::GetOrientation()
{
    return (suic::Orientation)(GetValue(OrientationProperty)->ToInt());
}

void HyperPanel::SetOrientation(suic::Orientation val)
{
    SetValue(OrientationProperty, suic::OrientationBox::From(val));
}

bool HyperPanel::GetApplyOpacity()
{
    return GetValue(ApplyOpacityProperty)->ToBool();
}

void HyperPanel::SetApplyOpacity(bool val)
{
    SetValue(ApplyOpacityProperty, BOOLTOBOOLEAN(val));
}

suic::Float HyperPanel::GetCenter()
{
    return GetValue(CenterProperty)->ToFloat();
}

void HyperPanel::SetCenter(suic::Float val)
{
    SetValue(CenterProperty, new suic::OFloat(val));
}

suic::Float HyperPanel::GetDistribution()
{
    return GetValue(DistributionProperty)->ToFloat();
}

void HyperPanel::SetDistribution(suic::Float val)
{
    SetValue(DistributionProperty, new suic::OFloat(val));
}

suic::Float HyperPanel::GetMinElementScale()
{
    return GetValue(MinElementScaleProperty)->ToFloat();
}
void HyperPanel::SetMinElementScale(suic::Float val)
{
    SetValue(MinElementScaleProperty, new suic::OFloat(val));
}

int HyperPanel::GetVerticalContentAlignment()
{
    return GetValue(VerticalContentAlignmentProperty)->ToInt();
}

void HyperPanel::SetVerticalContentAlignment(int val)
{
    SetValue(VerticalContentAlignmentProperty, suic::Integer::GetPosInt(val));
}

int HyperPanel::GetHorizontalContentAlignment()
{
    return GetValue(HorizontalContentAlignmentProperty)->ToInt();
}

void HyperPanel::SetHorizontalContentAlignment(int val)
{
    SetValue(HorizontalContentAlignmentProperty, suic::Integer::GetPosInt(val));
}

void HyperPanel::UpdateTransforms(suic::Size availableSize)
{
    suic::Float num = 0.0f;
    suic::Float allSize = 0.0f;
    suic::Element* pChild = NULL;
    bool isHori = GetOrientation() == suic::Orientation::Horizontal;
    suic::ElementColl* children = GetChildren(); 

    for (int i = 0; i < children->GetCount(); i++)
    {
        pChild = children->GetAt(i);
        if ((pChild != NULL) && (pChild->GetVisibility() == suic::Visibility::Visible))
        {
            MeasureChild(pChild, availableSize);
            //pChild->Measure(availableSize);
            allSize += isHori ? pChild->GetDesiredSize().Width() : pChild->GetDesiredSize().Height();
            num++;
        }
    }

    suic::ArrayType<suic::Float, false> numArray;
    suic::Float sumScale = 0.0f;
    suic::Float fDistri = (2.0f * GetDistribution()) * GetDistribution();
    int index = 0;

    numArray.Reset(num, true);

    for (int j = 0; j < children->GetCount(); j++)
    {
        pChild = children->GetAt(j);
        if ((pChild != NULL) && (pChild->GetVisibility() == suic::Visibility::Visible))
        {
            suic::Float num8 = GetCenter() - ((index + 0.5f) / ((suic::Float)num));
            suic::Float num9 = (num8 * num8) / fDistri;
            numArray[index] = UFMAX(GetMinElementScale(), exp(-num9));
            sumScale += numArray[index];
            index++;
        }
    }

    suic::Float rScale = ((suic::Float)num) / sumScale;
    if (allSize > 0.0f)
    {
        if (!isHori && GetHeight() >= 0)
        {
            rScale *= GetHeight() / allSize;
        }
        else if (isHori && GetWidth() >= 0)
        {
            rScale *= GetWidth() / allSize;
        }
    }
    if ((rScale == 0.0f) || (rScale > 1.0f))
    {
        rScale = 1.0f;
    }
    index = 0;
    for (int k = 0; k < children->GetCount(); k++)
    {
        pChild = children->GetAt(k);
        if ((pChild != NULL) && (pChild->GetVisibility() == suic::Visibility::Visible))
        {
            suic::ScaleTransform* renderTransform = suic::RTTICast<suic::ScaleTransform>(pChild->GetRenderTransform());

            if ((renderTransform == NULL) || renderTransform == suic::Transform::GetIdentity())
            {
                renderTransform = new suic::ScaleTransform();
                renderTransform->SetCenterX(0.5f);
                renderTransform->SetCenterY(0.5f);
                pChild->SetRenderTransform(renderTransform);
            }
            
            renderTransform->SetScaleX(numArray[index] * rScale);
            renderTransform->SetScaleY(renderTransform->GetScaleX());

            if (GetApplyOpacity())
            {
                pChild->SetOpacity(UFMIN(1.0f, numArray[index]));
            }
            index++;
        }
    }
}

void HyperPanel::OnCenterChanged(suic::Float oldValue)
{
    suic::Float num = UFMAX(0.0, UFMIN(1.0, GetCenter()));
    if (!suic::FloatUtil::AreClose(num, GetCenter()))
    {
        SetCenter(num);
    }

    UpdateLayout();
}

suic::Size HyperPanel::OnMeasure(const suic::Size& availableSize)
{
    suic::Element* pChild = NULL;

    UpdateTransforms(availableSize);

    _totalWidth = 0.0f;
    _totalHeight = 0.0f;

    suic::ElementColl* children = GetChildren(); 

    for (int i = 0; i < children->GetCount(); i++)
    {
        pChild = children->GetAt(i);

        if ((pChild != NULL) && (pChild->GetVisibility() == suic::Visibility::Visible))
        {
            MeasureChild(pChild, availableSize);
            //pChild->Measure(availableSize);
            suic::Size scaledSize = GetScaledSize(pChild);

            if (GetOrientation() == suic::Orientation::Horizontal)
            {
                _totalWidth += scaledSize.Width();
                _totalHeight = UFMAX(pChild->GetDesiredSize().Height(), _totalHeight);
            }
            else
            {
                _totalHeight += scaledSize.Height();
                _totalWidth = UFMAX(pChild->GetDesiredSize().Width(), _totalWidth);
            }
        }
    }
    _scaledChildSize = (GetOrientation() == suic::Orientation::Horizontal) ? _totalWidth : _totalHeight;
    return suic::Size(_totalWidth, _totalHeight);
}

suic::Size HyperPanel::GetScaledSize(suic::Element* elem)
{
    suic::Size desiredSize = elem->GetDesiredSize();
    suic::ScaleTransform* renderTransform = suic::RTTICast<suic::ScaleTransform>(elem->GetRenderTransform());

    if (renderTransform != NULL)
    {
        suic::FrameworkElement* pChild = CASTTOFE(elem);

        if (pChild != NULL)
        {
            suic::Float num = pChild->GetMargin().left + pChild->GetMargin().right;
            desiredSize.cx = ((desiredSize.Width() - num) * renderTransform->GetScaleX()) + num;
            num = pChild->GetMargin().top + pChild->GetMargin().bottom;
            desiredSize.cy = ((desiredSize.Height() - num) * renderTransform->GetScaleY()) + num;
            return desiredSize;
        }

        desiredSize.cx *= renderTransform->GetScaleX();
        desiredSize.cy *= renderTransform->GetScaleY();
    }

    return desiredSize;
}

void HyperPanel::OnArrange(const suic::Size& finalSize)
{
    switch (GetHorizontalContentAlignment())
    {
        case suic::HoriAlign::haLeft:
            _offsetX = 0.0f;
            break;

        case suic::HoriAlign::haCenter:
        case suic::HoriAlign::haStretch:
            _offsetX = (finalSize.Width() - _totalWidth) / 2.0f;
            break;

        case suic::HoriAlign::haRight:
            _offsetX = finalSize.Width() - _totalWidth;
            break;
    }
    switch (GetVerticalContentAlignment())
    {
    case suic::VertAlign::vaTop:
            _offsetY = 0.0f;
            break;

        case suic::VertAlign::vaCenter:
        case suic::VertAlign::vaStretch:
            _offsetY = (finalSize.Height() - _totalHeight) / 2.0f;
            break;

        case suic::VertAlign::vaBottom:
            _offsetY = finalSize.Height() - _totalHeight;
            break;
    }

    suic::fRect rect(_offsetX, _offsetY, finalSize.Width(), finalSize.Height());
    suic::fRect finalRect = rect;

    suic::ElementColl* children = GetChildren(); 

    for (int i = 0; i < children->GetCount(); i++)
    {
        suic::Element* elem = children->GetAt(i);

        if ((elem == NULL) || (elem->GetVisibility() != suic::Visibility::Visible))
        {
            continue;
        }

        suic::Size scaledSize = GetScaledSize(elem);
        suic::Size desiredSize = elem->GetDesiredSize();

        if (GetOrientation() == suic::Orientation::Horizontal)
        {
            finalRect.top = rect.top;

            switch (GetVerticalContentAlignment())
            {
                case suic::VertAlign::vaCenter:
                case suic::VertAlign::vaStretch:
                    finalRect.top += (desiredSize.Height() - scaledSize.Height()) / 2.0f;
                    finalRect.top += (finalSize.Height() - desiredSize.Height()) / 2.0f;
                    break;

                case suic::VertAlign::vaBottom:
                    finalRect.top += desiredSize.Height() - scaledSize.Height();
                    //finalRect.top = finalSize.Height() - desiredSize.Height();
                    break;
            }

            finalRect.right = finalRect.left + desiredSize.Width();
            finalRect.bottom = finalRect.top + desiredSize.Height();
            elem->Arrange(finalRect.ToRect());
            finalRect.left += scaledSize.Width();
            continue;
        }

        finalRect.left = rect.left;

        switch (GetHorizontalContentAlignment())
        {
            case suic::HoriAlign::haCenter:
            case suic::HoriAlign::haStretch:
                finalRect.left += (desiredSize.Width() - scaledSize.Width()) / 2.0f;
                finalRect.left += (finalSize.Width() - desiredSize.Width()) / 2.0f;
                break;

            case suic::HoriAlign::haRight:
                finalRect.left += desiredSize.Width() - scaledSize.Width();
                break;
        }

        finalRect.bottom = finalRect.top + desiredSize.Height();
        finalRect.right = finalRect.left + desiredSize.Width();
        elem->Arrange(finalRect.ToRect());
        finalRect.top += scaledSize.Height();
    }
}

void HyperPanel::OnMouseMove(suic::MouseButtonEventArg* e)
{
    if (!suic::FloatUtil::AreClose(_scaledChildSize, 0.0f))
    {
        bool flag = GetOrientation() == suic::Orientation::Horizontal;
        suic::Point position = PointFromScreen(e->GetMousePoint());
        SetCenter(flag ? ((position.x - _offsetX) / _scaledChildSize) : ((position.y - _offsetY) / _scaledChildSize));
    }

    e->SetHandled(true);
}

}
