
#include <Extend/Controls/ToolBarPanel.h>
#include <Extend/Controls/ToolBar.h>
#include <Framework/Controls/Separator.h>

#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>

#include <System/Tools/Debug.h>
#include <System/Tools/Math.h>

namespace suic
{

ImplementRTTIOfClass(ToolBarPanel, StackPanel)

ToolBarPanel::ToolBarPanel()
    : _minLength(0)
    , _maxLength(0)
{
}

ToolBarPanel::~ToolBarPanel()
{
}

Array<Element*, false>& ToolBarPanel::GetGeneratedItemsCollection()
{
    return _generatedItemsCollection;
}

IScrollInfo* ToolBarPanel::GetScrollInfo()
{
    return NULL;
}

bool ToolBarPanel::IsScrolling()
{
    return false;
}

void ToolBarPanel::OnOrientationPropChanged(DpObject* d, DpPropChangedEventArg* e)
{

}

void ToolBarPanel::StaticInit()
{
}

int ToolBarPanel::GetMinLength() const
{
    return _minLength;
}

void ToolBarPanel::SetMinLength(int val)
{
    _minLength = val;
}

int ToolBarPanel::GetMaxLength() const
{
    return _maxLength;
}

void ToolBarPanel::SetMaxLength(int val)
{
    _maxLength = val;
}

ToolBar* ToolBarPanel::GetToolBar()
{
    return DynamicCast<ToolBar>(GetTemplatedParent());
}

ToolBarOverflowPanel* ToolBarPanel::GetToolBarOverflowPanel()
{
    ToolBar* toolBar = GetToolBar();

    if (toolBar != NULL)
    {
        return toolBar->GetToolBarOverflowPanel();
    }
    else
    {
        return NULL;
    }
}

void ToolBarPanel::OnPreApplyTemplate()
{
    StackPanel::OnPreApplyTemplate();
}

void ToolBarPanel::OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e)
{
    switch (e->GetAction())
    {
    case NotifyCollectionChangedAction::Add:
        AddChildren(e->GetNewPos(), e->GetCount());
        return;

    case NotifyCollectionChangedAction::Remove:
        RemoveChildren(e->GetOldPos(), e->GetCount());
        return;

    case NotifyCollectionChangedAction::Replace:
        ReplaceChildren(e->GetNewPos(), e->GetCount(), e->GetCount());
        return;

    case NotifyCollectionChangedAction::Move:
        MoveChildren(e->GetOldPos(), e->GetNewPos(), e->GetCount());
        return;

    case NotifyCollectionChangedAction::Reset:
        StackPanel::OnItemsChangedInternal(sender, e);
        return;
    }
}

void ToolBarPanel::AddChildren(int pos, int itemCount)
{
    ItemContainerGenerator* generator = GetGenerator();

    if (generator != NULL)
    {
        int iEnd = pos + itemCount;
        // 根据用户数据项生成实际的UI元素
        // 移动到开始位置
        if (generator->StartAt(pos, iEnd - 1))
        {
            while (generator->HasNext())
            {
                Element* elem = generator->Generate();
                if (NULL != elem)
                {
                    _generatedItemsCollection.Insert(generator->GetPos(), elem);
                    elem->unref();
                    // 根据项容器和项生存容器内容
                    generator->PrepareItemContainer(elem);
                }
            }
        }
    }
}

void ToolBarPanel::MoveChildren(int fromPos, int toPos, int containerCount)
{
    if (fromPos != toPos)
    {
        Array<Element*, false> elemArr;
        ItemContainerGenerator* generator = GetGenerator();

        for (int i = 0; i < containerCount; i++)
        {
            Element* child = _generatedItemsCollection[fromPos + i];
            elemArr.Add(child);
            child->ref();
            RemoveAt(child);
        }

        _generatedItemsCollection.RemoveRange(fromPos, containerCount);

        for (int j = 0; j < containerCount; j++)
        {
            Element* child = elemArr[j];
            _generatedItemsCollection.Insert(toPos + j, child);
            child->unref();
        }

        elemArr.Clear();
    }
}

void ToolBarPanel::RemoveAt(Element* child)
{
    DpObject* parent = VisualTreeOp::GetParent(child);

    if (parent == this)
    {
        GetChildren()->Remove(child);
    }
    else
    {
        ToolBarOverflowPanel* toolBarOverflowPanel = GetToolBarOverflowPanel();
        if ((parent == toolBarOverflowPanel) && (toolBarOverflowPanel != NULL))
        {
            toolBarOverflowPanel->GetChildren()->Remove(child);
        }
    }
}

void ToolBarPanel::RemoveChildren(int pos, int containerCount)
{
    for (int i = 0; i < containerCount; i++)
    {
        RemoveAt(_generatedItemsCollection[pos + i]);
    }
    _generatedItemsCollection.RemoveRange(pos, containerCount);
}

void ToolBarPanel::ReplaceChildren(int pos, int itemCount, int containerCountt)
{
    ItemContainerGenerator* generator = GetGenerator();

    if (generator != NULL)
    {
        int iEnd = pos + itemCount;
        // 根据用户数据项生成实际的UI元素
        // 移动到开始位置
        if (generator->StartAt(pos, iEnd - 1))
        {
            while (generator->HasNext())
            {
                int iCurPos = generator->GetPos();
                Element* elem = generator->Generate();

                if (NULL != elem)
                {
                    RemoveAt(_generatedItemsCollection[iCurPos]);
                    _generatedItemsCollection[iCurPos] = elem;
                    elem->unref();
                    // 根据项容器和项生存容器内容
                    generator->PrepareItemContainer(elem);
                }
            }
        }
    }
}

void ToolBarPanel::GenerateChildren()
{
    StackPanel::GenerateChildren();
    ElementColl* children = GetChildren();

    _generatedItemsCollection.Clear();

    ToolBarOverflowPanel* toolBarOverflowPanel = GetToolBarOverflowPanel();

    if (toolBarOverflowPanel != NULL)
    {
        toolBarOverflowPanel->GetChildren()->Clear();
        toolBarOverflowPanel->InvalidateMeasure();
    }

    int count = children->GetCount();

    for (int i = 0; i < count; i++)
    {
        Element* elem = children->GetAt(i);
        ToolBar::SetIsOverflowItem(elem, Boolean::False);
        _generatedItemsCollection.Add(elem);
    }
}

Size ToolBarPanel::OnMeasure(const Size& constraint)
{
    Size panelDesiredSize;

    if (IsItemsHost())
    {
        int width = 0;
        int num2 = 0;
        Size size2 = constraint;
        bool bHori = GetOrientation() == Orientation::Horizontal;

        if (bHori)
        {
            size2.cx = Numeric::MeasureInt;
            width = constraint.Width();
        }
        else
        {
            size2.cy = Numeric::MeasureInt;
            width = constraint.Height();
        }

        bool flag2 = MeasureGeneratedItems(false, size2, bHori, width, panelDesiredSize, num2);
        SetMinLength(bHori ? panelDesiredSize.Width() : panelDesiredSize.Height());
        bool flag3 = MeasureGeneratedItems(true, size2, bHori, width, panelDesiredSize, num2);
        SetMaxLength((bHori ? panelDesiredSize.Width() : panelDesiredSize.Height()) + num2);

        ToolBar* toolBar = GetToolBar();

        if (toolBar != NULL)
        {
            toolBar->SetValue(ToolBar::HasOverflowItemsProperty, BOOLTOBOOLEAN(flag2 || flag3));
        }

        return panelDesiredSize;
    }

    return StackPanel::OnMeasure(constraint);
}

void ToolBarPanel::OnArrange(const Size& arrangeSize)
{
    ElementColl* children = GetChildren();
    bool bHori = GetOrientation() == Orientation::Horizontal;
    Rect finalRect(Point(), arrangeSize);
    int width = 0;
    int index = 0;
    int count = children->GetCount();

    while (index < count)
    {
        Element* elem = children->GetAt(index);

        if (bHori)
        {
            finalRect.left += width;
            width = elem->GetDesiredSize().Width();
            finalRect.right = finalRect.left + width;
            finalRect.bottom = finalRect.top + max(arrangeSize.Height(), elem->GetDesiredSize().Height());
        }
        else
        {
            finalRect.top += width;
            width = elem->GetDesiredSize().Height();
            finalRect.bottom = finalRect.top + width;
            finalRect.right = finalRect.left + max(arrangeSize.Width(), elem->GetDesiredSize().Width());
        }

        elem->Arrange(finalRect);
        index++;
    }
}

bool ToolBarPanel::MeasureGeneratedItems(bool asNeededPass, Size constraint, bool horizontal, int maxExtent, Size& panelDesiredSize, int& overflowExtent)
{
    ElementColl* children = GetChildren();
    ToolBarOverflowPanel* toolBarOverflowPanel = GetToolBarOverflowPanel();

    bool flag = false;
    bool flag2 = false;
    bool flag3 = false;
    int count = children->GetCount();
    int index = 0;

    overflowExtent = 0;

    for (int i = 0; i < _generatedItemsCollection.Length(); i++)
    {
        Element* elem = _generatedItemsCollection[i];
        OverflowMode overflowMode = ToolBar::GetOverflowMode(elem);
        bool flag4 = overflowMode == OverflowMode::AsNeeded;

        if (flag4 == asNeededPass)
        {
            DpObject* parent = VisualTreeOp::GetParent(elem);

            if ((overflowMode != OverflowMode::Always) && !flag)
            {
                ToolBar::SetIsOverflowItem(elem, Boolean::False);

                MeasureChild(elem, constraint);

                Size desiredSize = elem->GetDesiredSize();

                if (flag4)
                {
                    double num4;

                    if (horizontal)
                    {
                        num4 = desiredSize.Width() + panelDesiredSize.Width();
                    }
                    else
                    {
                        num4 = desiredSize.Height() + panelDesiredSize.Height();
                    }

                    if (num4 > maxExtent)
                    {
                        flag = true;
                    }
                }

                if (!flag)
                {
                    if (horizontal)
                    {
                        panelDesiredSize.cx += desiredSize.Width();
                        panelDesiredSize.cy = max(panelDesiredSize.Height(), desiredSize.Height());
                    }
                    else
                    {
                        panelDesiredSize.cx = max(panelDesiredSize.Width(), desiredSize.Width());
                        panelDesiredSize.cy += desiredSize.Height();
                    }

                    if (parent != this)
                    {
                        if ((parent == toolBarOverflowPanel) && (toolBarOverflowPanel != NULL))
                        {
                            toolBarOverflowPanel->GetChildren()->Remove(elem);
                        }

                        if (index < count)
                        {
                            children->Insert(index, elem);
                        }
                        else
                        {
                            children->AddInternal(elem);
                        }
                        count++;
                    }
                    index++;
                }
            }

            if ((overflowMode == OverflowMode::Always) || flag)
            {
                flag2 = true;
                if (elem->ReadFlag(CoreFlags::IsMeasureDirty))
                {
                    ToolBar::SetIsOverflowItem(elem, Boolean::False);

                    MeasureChild(elem, constraint);
                }

                Size size2 = elem->GetDesiredSize();

                if (horizontal)
                {
                    overflowExtent += size2.Width();
                    panelDesiredSize.cy = max(panelDesiredSize.Height(), size2.Height());
                }
                else
                {
                    overflowExtent += size2.Height();
                    panelDesiredSize.cx = max(panelDesiredSize.Width(), size2.Width());
                }

                ToolBar::SetIsOverflowItem(elem, Boolean::True);
                if (parent == this)
                {
                    children->Remove(elem);
                    count--;
                    flag3 = true;
                }
            }
        }
        else if ((index < count) && (children->GetAt(index) == elem))
        {
            index++;
        }
    }
    
    if (flag3 && (toolBarOverflowPanel != NULL))
    {
        toolBarOverflowPanel->InvalidateMeasure();
    }

    return flag2;
}

DpProperty* ToolBarOverflowPanel::WrapWidthProperty;

void ToolBarOverflowPanel::StaticInit()
{
}

ElementColl* ToolBarOverflowPanel::CreateUIElementCollection(FrameworkElement* logicalParent)
{
    return NULL;
}

void ToolBarOverflowPanel::ArrangeLine(int y, int lineHeight, int start, int end)
{
    int x = 0;
    ElementColl* children = GetChildren();

    for (int i = start; i < end; i++)
    {
        Element* elem = children->GetAt(i);

        elem->Arrange(Rect(x, y, elem->GetDesiredSize().Width(), lineHeight));
        x += elem->GetDesiredSize().Width();
    }
}

void ToolBarOverflowPanel::OnArrange(const Size& arrangeBounds)
{
    Size size;
    int start = 0;
    int y = 0;
    ElementColl* children = GetChildren();

    _wrapWidth = min(_wrapWidth, arrangeBounds.Width());

    for (int i = 0; i < children->GetCount(); i++)
    {
        Size desiredSize = children->GetAt(i)->GetDesiredSize();

        if (size.Width() + desiredSize.Width() > _wrapWidth)
        {
            ArrangeLine(y, size.Height(), start, i);
            y += size.Height();
            start = i;
            size = desiredSize;
        }
        else
        {
            size.cx += desiredSize.Width();
            size.cy = max(desiredSize.Height(), size.Height());
        }
    }

    ArrangeLine(y, size.Height(), start, children->GetCount());
}

Size ToolBarOverflowPanel::OnMeasure(const Size& constraint)
{
    Size size;
    _panelSize = Size();
    _wrapWidth = GetWrapWidth() > 0 ? GetWrapWidth() : constraint.Width();
    ElementColl* children = GetChildren();
    int count = children->GetCount();
    ToolBarPanel* toolBarPanel = GetToolBarPanel();

    if (toolBarPanel != NULL)
    {
        Array<Element*, false>& generatedItemsCollection = toolBarPanel->GetGeneratedItemsCollection();
        int num2 = generatedItemsCollection.Length();
        int index = 0;

        for (int k = 0; k < num2; k++)
        {
            Element* elem = generatedItemsCollection[k];

            if (((elem != NULL) && ToolBar::GetIsOverflowItem(elem)) && !(elem->GetRTTIType() == Separator::RTTIType()))
            {
                if (index < count)
                {
                    if (children->GetAt(index) != elem)
                    {
                        children->Insert(index, elem);
                        count++;
                    }
                }
                else
                {
                    children->Add(elem);
                    count++;
                }
                index++;
            }
        }
    }

    for (int i = 0; i < count; i++)
    {
        Element* element2 = children->GetAt(i);

        MeasureChild(element2, constraint);
        Size desiredSize = element2->GetDesiredSize();

        if (desiredSize.Width() > _wrapWidth)
        {
            _wrapWidth = desiredSize.Width();
        }
    }

    _wrapWidth = min(_wrapWidth, constraint.Width());

    for (int j = 0; j < children->GetCount(); j++)
    {
        Element* element3 = children->GetAt(j);
        Size size3 = element3->GetDesiredSize();

        if (size.Width() + size3.Width() > _wrapWidth)
        {
            _panelSize.cx = max(size.Width(), _panelSize.Width());
            _panelSize.cy += size.Height();
            size = size3;

            if (size3.Width() > _wrapWidth)
            {
                _panelSize.cx = max(size3.Width(), _panelSize.Width());
                _panelSize.cy += size3.Height();
                size = Size();
            }
        }
        else
        {
            size.cx += size3.Width();
            size.cy = max(size3.Height(), size.Height());
        }
    }

    _panelSize.cx = max(size.Width(), _panelSize.Width());
    _panelSize.cy += size.Height();

    return _panelSize;
}

int ToolBarOverflowPanel::GetWrapWidth()
{
    return GetValue(WrapWidthProperty)->ToInt();
}

void ToolBarOverflowPanel::SetWrapWidth(int val)
{
    SetValue(WrapWidthProperty, new Integer(val));
}

ToolBar* ToolBarOverflowPanel::GetToolBar()
{
    return DynamicCast<ToolBar>(GetTemplatedParent());
}


ToolBarPanel* ToolBarOverflowPanel::GetToolBarPanel()
{
    ToolBar* toolBar = GetToolBar();

    if (toolBar != NULL)
    {
        return toolBar->GetToolBarPanel();
    }
    else
    {
        return NULL;
    }
}

}
