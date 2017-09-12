/////////////////////////////////////////////////////////////////////////
// Decorator.cpp

#include <System/Tools/VisualTreeOp.h>

#include <Framework/Controls/Decorator.h>
#include <Framework/Presenter/ScrollContentPresenter.h>

#include <System/Interop/System.h>

namespace suic
{

ImplementRTTIOfClass(Adorner, FrameworkElement)
ImplementRTTIOfClass(AdornerLayer, FrameworkElement)

Adorner::Adorner(Element* elem)
    : _adornedElement(elem)
{
    
}

Adorner::Adorner()
    : _adornedElement(NULL)
{

}

Adorner::~Adorner()
{

}

Element* Adorner::GetAdornedElement() const
{
    return _adornedElement;
}

Size Adorner::OnMeasure(const Size& constraint)
{
    Size availableSize(constraint);
    int iCount = GetVisualChildrenCount();

    if (GetAdornedElement() != NULL)
    {
        availableSize = GetAdornedElement()->ComputeRenderSize();
    }

    for (int i = 0; i < iCount; i++)
    {
        Element* elem = GetVisualChild(i);

        if (elem != NULL)
        {
            MeasureChild(elem, availableSize);
        }
    }

    return availableSize;
}

//----------------------------------------------------
//

AdornerInfo::AdornerInfo(Adorner* val)
{
    adorner = val;
    adorner->ref();
    zOrder = 0x7fff;
}

AdornerInfo::~AdornerInfo()
{
    adorner->unref();
}

AdornerLayer::AdornerLayer()
{
    SetHitTestFilterAction(HitTestFilterAction::ContinueSkipSelfAndChildren);
}

AdornerLayer::~AdornerLayer()
{
    Clear();
}

void AdornerLayer::Clear()
{
    int iCount = _adornerInfos.Length();

    for (int i = 0; i < iCount; i++)
    {
        AdornerInfo* aInfo = _adornerInfos[i];

        if (aInfo != NULL)
        {
            delete aInfo;
        }
    }

    _adornerInfos.Clear();
}

AdornerLayer* AdornerLayer::GetAdornerLayer(Element* elem)
{
    AdornerLayer* adornerLayer = NULL;

    if (elem == NULL)
    {
        throw ArgumentNullException("visual", __FILELINE__);
    }
    
    Element* parent = VisualTreeOp::GetParent(elem);

    while (NULL != parent)
    {
        if (NULL != RTTICast<AdornerDecorator>(parent))
        {
            adornerLayer = RTTICast<AdornerDecorator>(parent)->GetAdornerLayer();
            break;
        }

        ScrollContentPresenter* scrollPresenter = RTTICast<ScrollContentPresenter>(parent);
        if (NULL != scrollPresenter)
        {
            adornerLayer = scrollPresenter->GetAdornerLayer();
            break;
        }

        parent = parent->GetUIParent();
    }

    return adornerLayer;
}

void AdornerLayer::Add(Adorner* val)
{
    Add(val, 0x7fff);
}

void AdornerLayer::Add(Adorner* adorner, int zOrderIndex)
{
    int iPos = -1;
    int iCount = _adornerInfos.GetCount();
    AdornerInfo* aInfo = new AdornerInfo(adorner);

    aInfo->zOrder = zOrderIndex;

    for (int i = iCount - 1; i >= 0; --i)
    {
        AdornerInfo* tmInfo = _adornerInfos[i];
        if (zOrderIndex >= tmInfo->zOrder)
        {
            iPos = i + 1;
            break;
        }
    }

    if (iPos == -1 || iPos >= iCount)
    {
        _adornerInfos.Add(aInfo);
    }
    else
    {
        _adornerInfos.Insert(iPos, aInfo);
    }

    AddVisualChild(adorner);
    AddLogicalChild(adorner);
}

void AdornerLayer::Remove(Adorner* adorner)
{
    int iCount = _adornerInfos.Length();

    for (int i = 0; i < iCount; i++)
    {
        AdornerInfo* aInfo = _adornerInfos[i];

        if (aInfo->adorner == adorner)
        {
            RemoveVisualChild(adorner);
            RemoveLogicalChild(adorner);

            delete aInfo;
            _adornerInfos.RemoveAt(i);
            break;
        }
    }
}

int AdornerLayer::GetVisualChildrenCount()
{
    return _adornerInfos.Length();
}

Element* AdornerLayer::GetVisualChild(int index)
{
    return _adornerInfos.GetItem(index)->adorner;
}

Size AdornerLayer::OnMeasure(const Size& constraint)
{
    int iCount = _adornerInfos.Length();

    for (int i = 0; i < iCount; i++)
    {
        AdornerInfo* aInfo = _adornerInfos[i];

        if (aInfo != NULL)
        {
            MeasureChild(aInfo->adorner, constraint);
        }
    }

    return Size();
}

static void __TransformAncestorRect(Element* fe, Element* ancestor, Matrix& m)
{
    Element* root = fe;
    LayoutTransformInfo* data = NULL;

    while (NULL != fe)
    {
        data = fe->GetLayoutTransformInfo();
        Point offset = fe->GetVisualOffset();

        if (data != NULL)
        {
            if (m.IsIdentity())
            {
                m.SetMatrix(data->transform);
            }
            else
            {
                m.PostConcat(data->transform);
            }
            m.PostTranslate(-data->offset.x, -data->offset.y);
        }
        else if (m.IsIdentity())
        {
            m.SetTranslate(0, 0);
        }

        m.PostTranslate(offset.x, offset.y);

        fe = fe->GetUIParent();
        if (ancestor == fe)
        {
            break;
        }
    }
}

void AdornerLayer::OnArrange(const Size& arrangeSize)
{
    int iCount = _adornerInfos.Length();
 
    for (int i = 0; i < iCount; i++)
    {
        AdornerInfo* aInfo = _adornerInfos[i];

        if (aInfo != NULL)
        {
            fRect rect;
            Matrix matrix;
            Element* adorned = aInfo->adorner->GetAdornedElement();

            aInfo->adorner->SetVisualTransform(NULL);

            if (NULL == adorned)
            {
                aInfo->adorner->Arrange(rect.ToRect());
            }
            else
            {
                if (adorned->IsVisible())
                {
                    if (adorned->ReadFlag(CoreFlags::IsArrangeDirty))
                    {
                        aInfo->adorner->Arrange(Rect());
                    }
                    else
                    {
                        aInfo->size = adorned->ComputeRenderSize();
                        ComputeClipGeometry(aInfo->adorner, adorned);

                        __TransformAncestorRect(adorned, GetUIParent(), matrix);

                        rect.SetXYWH(0, 0, aInfo->size.cx, aInfo->size.cy);

                        rect = matrix.TransformRect(rect);
                        aInfo->pos = rect.ToRect().LeftTop();

                        matrix.PostTranslate(-rect.left, -rect.top);
                        aInfo->adorner->Arrange(Rect(aInfo->pos, aInfo->size));

                        if (!matrix.IsIdentity())
                        {
                            aInfo->adorner->SetVisualTransform(new MatrixTransform(matrix));
                        }
                    }
                }
            }
        }
    }
}

void AdornerLayer::ComputeClipGeometry(Adorner* adorner, Element* elem)
{
    Element* decorator = GetUIParent();

    if (elem != NULL && decorator->IsAncestorOf(elem))
    {
        Rect rect(Point(), elem->ComputeRenderSize());
        Element* ancestor = elem->GetUIParent();
        Point offset = elem->TransformToAncestor(decorator);

        rect.Offset(offset);

        while (ancestor != NULL)
        {
            if (ancestor->IsClipToBounds())
            {
                Rect temp(Point(), ancestor->ComputeRenderSize());
                temp.Offset(ancestor->TransformToAncestor(decorator));
                rect.IntersectRect(&temp);
                break;
            }

            if (this == ancestor)
            {
                break;
            }

            ancestor = ancestor->GetUIParent();
        }

        PathGeometry* pathClip = new PathGeometry();
        rect.Offset(-offset.x, -offset.y);
        pathClip->AddRect(rect.TofRect());
        adorner->SetClip(pathClip);
    }
}

void AdornerLayer::UpdateAdornerInfo(AdornerInfo* aInfo)
{
    if (aInfo != NULL)
    {
        Element* decorator = NULL;
        Element* target = aInfo->adorner->GetAdornedElement();
        Point pos;

        decorator = GetUIParent();
        target = aInfo->adorner->GetAdornedElement();

        if (NULL != target)
        {
            pos = target->TransformToAncestor(decorator);
            aInfo->pos = pos;
            aInfo->size = target->ComputeRenderSize();
            ComputeClipGeometry(aInfo->adorner, target);
        }
        else
        {
            aInfo->pos = pos;
            aInfo->size = ComputeRenderSize();
        }
    }
}

void AdornerLayer::UpdateElementAdorners(Element* elem)
{
}

//---------------------------------------------------------------
//

ImplementRTTIOfClass(Decorator, FrameworkElement)

Decorator::Decorator()
    : _child(NULL)
{

}

Decorator::~Decorator()
{
    FREEREFOBJ(_child);
}

void Decorator::SetChild(Element* child)
{
    if (_child != child)
    {
        RemoveVisualChild(_child);
        RemoveLogicalChild(_child);
        SETREFOBJ(_child, child);
        AddLogicalChild(_child);
        AddVisualChild(_child);
        InvalidateMeasure();
    }
}

Size Decorator::OnMeasure(const Size& constraint)
{
    if (_child)
    {
        MeasureChild(_child, constraint);
        return _child->GetDesiredSize();
    }
    else
    {
        return Size();
    }
}

void Decorator::OnArrange(const Size& arrangeSize)
{
    if (_child)
    {
        Rect arrangeRect(0, 0, arrangeSize.cx, arrangeSize.cy);
        _child->Arrange(arrangeRect);
    }
}

void Decorator::AddChild(Object* child)
{
    Element* elem(RTTICast<Element>(child));
    if (NULL == elem)
    {
        throw InvalidOperationException(_U("child is not a element"), __FILELINE__);
    }
    SetChild(elem);
}

void Decorator::AddText(String val)
{
    AddChild(new OString(val));
}

void Decorator::RemoveChild(Object* child)
{
    SetChild(NULL);
}

Element* Decorator::GetVisualChild(int index)
{
    return (_child);
}

int Decorator::GetVisualChildrenCount()
{
    return (_child ? 1 : 0);
}

Element* Decorator::GetLogicalChild(int index)
{
    return (_child);
}

int Decorator::GetLogicalChildrenCount()
{
    return (_child ? 1 : 0);
}

//---------------------------------------------------
//
ImplementRTTIOfClass(AdornerDecorator, Decorator)

AdornerDecorator::AdornerDecorator()
{
    _adornerLayer = new AdornerLayer();
    _adornerLayer->ref();
}

AdornerDecorator::~AdornerDecorator()
{
    _adornerLayer->unref();
}

AdornerLayer* AdornerDecorator::GetAdornerLayer() const
{
    return _adornerLayer;
}

int AdornerDecorator::GetVisualChildrenCount()
{
    if (Decorator::GetChild() != NULL)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

Element* AdornerDecorator::GetVisualChild(int index)
{
    Element* child = NULL;

    switch (index)
    {
    case 0:
        child = Decorator::GetChild();
        if (NULL == child)
        {
            child = _adornerLayer;
        }
        break;

    case 1:
        child = _adornerLayer;
        break;

    default:
        ;
    }

    return child;
}

Element* AdornerDecorator::GetChild()
{
    return Decorator::GetChild();
}

void AdornerDecorator::SetChild(Element* elem)
{
    Element* child = GetChild();

    if (child != elem)
    {
        if (elem == NULL)
        {
            Decorator::SetChild(NULL);
            Decorator::RemoveVisualChild(_adornerLayer);
        }
        else
        {
            Decorator::SetChild(elem);
            if (child == NULL)
            {
                Decorator::AddVisualChild(_adornerLayer);
            }
        }
    }
}

Size AdornerDecorator::OnMeasure(const Size& constraint)
{
    Size size = Decorator::OnMeasure(constraint);
    if (NULL != _adornerLayer->GetUIParent())
    {
        MeasureChild(_adornerLayer, constraint);
    }
    return size;
}

void AdornerDecorator::OnArrange(const Size& arrangeSize)
{
    Decorator::OnArrange(arrangeSize);

    if (NULL != _adornerLayer->GetUIParent())
    {
        _adornerLayer->Arrange(Rect(Point(), arrangeSize));
    }
}

}
