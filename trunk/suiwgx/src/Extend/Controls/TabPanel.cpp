
#include <Extend/Controls/TabPanel.h>
#include <Extend/Controls/TabControl.h>

#include <System/Input/KeyboardNavigation.h>

namespace suic
{

ImplementRTTIOfClass(TabPanel, Panel)

DpProperty* TabPanel::TabArrangeModeProperty;

class TabArrangeModeConvert : public IConvertValue
{
public:

    static TabArrangeModeConvert* Convert;

    static TabArrangeModeConvert* GetConvert()
    {
        if (NULL == Convert)
        {
            Convert = new TabArrangeModeConvert();
        }
        return Convert;
    }

    AutoObj ConvertValue(DpProperty* dp, Object* prop)
    {
        AutoObj obj;
        suic::String val(prop->ToString());

        if (val.Equals(_T("Single")))
        {
            obj = Integer::GetPosInt(TabArrangeMode::tamSingle);
        }
        else if (val.Equals(_T("Menu")))
        {
            obj = Integer::GetPosInt(TabArrangeMode::tamMenu);
        }
        else
        {
            obj = Integer::GetPosInt(TabArrangeMode::tamMultiple);
        }

        return obj;
    }

protected:

    TabArrangeModeConvert()
    {

    }
};

TabArrangeModeConvert* TabArrangeModeConvert::Convert = NULL;

void TabPanel::StaticInit()
{
    static bool s_init = false;

    if (!s_init)
    {
        s_init = true;

        TabArrangeModeProperty = DpProperty::Register("TabArrangeMode", RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(TabArrangeMode::tamMultiple), 0));
        TabArrangeModeProperty->SetConvertValueCb(TabArrangeModeConvert::GetConvert());

        KeyboardNavigation::TabNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knOnce)));
        KeyboardNavigation::DirectionalNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knCycle)));
    }
}

TabPanel::TabPanel()
{
    _numRows = 0;
    _allSize = 0;
    _lastVisibleTab = -1;
    _firstVisibleTab = -1;

    TabPanel::StaticInit();
}

TabPanel::~TabPanel()
{

}

TabArrangeMode TabPanel::GetTabArrangeMode()
{
    return (TabArrangeMode)(GetValue(TabArrangeModeProperty)->ToInt());
}

void TabPanel::SetTabArrangeMode(TabArrangeMode val)
{
    SetValue(TabArrangeModeProperty, Integer::GetPosInt(val));
}

Dock TabPanel::GetTabStripPlacement()
{
    Dock top = Dock::dockTop;
    TabControl* templatedParent = RTTICast<TabControl>(GetTemplatedParent());
    if (templatedParent != NULL)
    {
        top = templatedParent->GetTabStripPlacement();
    }
    return top;
}

void TabPanel::UpdateTabPanel()
{
    Element* pParent = RTTICast<Element>(GetTemplatedParent());
    if (NULL != pParent)
    {
        pParent->UpdateLayout();
    }
    else
    {
        this->GetParent()->UpdateLayout();
    }
}

bool TabPanel::AllTabsInVisible()
{
    int rendSize = 0;
    switch (GetTabStripPlacement())
    {
    case Dock::dockTop:
    case Dock::dockBottom:
        rendSize = GetRenderSize().Width();
        break;

    case Dock::dockLeft:
    case Dock::dockRight:
        rendSize = GetRenderSize().Height();
        break;
    }

    return (_allSize <= rendSize);
}

void TabPanel::ScrollToPrev()
{
    if (_firstVisibleTab > 0 || _numRows > 0)
    {
        ElementColl* elemsColl = GetInternalChildren();
        if (_firstVisibleTab > 0 && _firstVisibleTab < elemsColl->GetCount())
        {
            switch (GetTabStripPlacement())
            {
            case Dock::dockTop:
            case Dock::dockBottom:
                _numRows -= elemsColl->GetAt(_firstVisibleTab)->GetDesiredSize().Width();
                break;

            case Dock::dockLeft:
            case Dock::dockRight:
                _numRows -= elemsColl->GetAt(_firstVisibleTab)->GetDesiredSize().Height();
                break;
            }
        }
        else
        {
            _numRows = 0;
        }

        if (_numRows < 0)
        {
            _numRows = 0;
        }

        UpdateTabPanel();
    }
}

void TabPanel::ScrollToNext()
{
    int rendSize = 0;
    ElementColl* elemsColl = GetInternalChildren();
    if (_lastVisibleTab > 0 && _lastVisibleTab < elemsColl->GetCount())
    {
        switch (GetTabStripPlacement())
        {
        case Dock::dockTop:
        case Dock::dockBottom:
            rendSize = GetRenderSize().Width();
            _numRows += elemsColl->GetAt(_lastVisibleTab)->GetDesiredSize().Width();
            break;

        case Dock::dockLeft:
        case Dock::dockRight:
            rendSize = GetRenderSize().Height();
            _numRows += elemsColl->GetAt(_lastVisibleTab)->GetDesiredSize().Height();
            break;
        }

        if (_numRows > _allSize - rendSize)
        {
            _numRows = _allSize - rendSize;
        }

        if (_numRows < 0)
        {
            _numRows = 0;
        }

        UpdateTabPanel();
    }
}

void TabPanel::ScrollToHome()
{
    if (_numRows != 0)
    {
        _numRows = 0;
        UpdateTabPanel();
    }
}

void TabPanel::ScrollToEnd()
{
    int rendSize = 0;
    switch (GetTabStripPlacement())
    {
    case Dock::dockTop:
    case Dock::dockBottom:
        rendSize = GetRenderSize().Width();
        break;

    case Dock::dockLeft:
    case Dock::dockRight:
        rendSize = GetRenderSize().Height();
        break;
    }

    if (_allSize > rendSize && _numRows != _allSize - rendSize)
    {
        _numRows = _allSize - rendSize;
        UpdateTabPanel();
    }
}

void TabPanel::ScrollToIntoView(Element* pItem)
{
    bool needUpdate = false;
    int iOffet = -_numRows;
    FrameworkElement* elem = NULL;
    ElementColl* elemsColl = GetInternalChildren();
    Dock tabStripPlacement = GetTabStripPlacement();

    for (int i = 0; i < elemsColl->GetCount(); ++i)
    {
        elem = CASTTOFE(elemsColl->GetAt(i));
        if (elem->GetVisibility() == Visibility::Collapsed)
        {
            continue;
        }

        if (tabStripPlacement == Dock::dockTop || 
            tabStripPlacement == Dock::dockBottom)
        {
            if (elem == pItem)
            {
                if (iOffet < 0)
                {
                    _numRows += iOffet;
                    needUpdate = true;
                }
                else
                {
                    iOffet += elem->GetDesiredSize().Width() + _numRows;
                    if (iOffet >= GetRenderSize().Width())
                    {
                        _numRows = iOffet - GetRenderSize().Width();
                        needUpdate = true;
                    }
                }
                break;
            }
            iOffet += elem->GetDesiredSize().Width();
        }
        else
        {
            if (elem == pItem)
            {
                if (iOffet < 0)
                {
                    _numRows += iOffet;
                    needUpdate = true;
                }
                else
                {
                    iOffet += elem->GetDesiredSize().Height() + _numRows;
                    if (iOffet >= GetRenderSize().Height())
                    {
                        _numRows = iOffet - GetRenderSize().Height();
                        needUpdate = true;
                    }
                }
                break;
            }
            iOffet += elem->GetDesiredSize().Height();
        }
    }

    if (needUpdate)
    {
        if (_numRows < 0)
        {
            _numRows = 0;
        }
        UpdateTabPanel();
    }
}

void TabPanel::OnArrange(const Size& arrangeSize)
{
    switch (GetTabStripPlacement())
    {
    case Dock::dockTop:
    case Dock::dockBottom:
        ArrangeHorizontal(arrangeSize);
        break;

    case Dock::dockLeft:
    case Dock::dockRight:
        ArrangeVertical(arrangeSize);
        break;
    }
}

void TabPanel::ArrangeVertical(Size arrangeSize)
{
    if (GetTabArrangeMode() == TabArrangeMode::tamSingle)
    {
        ArrangeSingle(arrangeSize);
    }
    else
    {
        ArrangeMultiVertical(arrangeSize);
    }
}

void TabPanel::ArrangeHorizontal(Size arrangeSize)
{
    if (GetTabArrangeMode() == TabArrangeMode::tamSingle)
    {
        ArrangeSingle(arrangeSize);
    }
    else
    {
        ArrangeMultiHorizontal(arrangeSize);
    }
}

void TabPanel::ArrangeSingle(Size arrangeSize)
{
    suic::Size desiredSize;
    FrameworkElement* elem = NULL;
    ElementColl* elemsColl = GetInternalChildren();
    Dock tabStripPlacement = GetTabStripPlacement();
    suic::Rect rect;

    _allSize = 0;
    _lastVisibleTab = -1;
    _firstVisibleTab = -1;

    if (tabStripPlacement == Dock::dockTop || tabStripPlacement == Dock::dockBottom)
    {
        rect.left = -_numRows;
        rect.right = rect.left;
        rect.bottom = _rowHeight;

        for (int i = 0; i < elemsColl->GetCount(); ++i)
        {
            elem = CASTTOFE(elemsColl->GetAt(i));
            if (elem->GetVisibility() == Visibility::Collapsed)
            {
                continue;
            }
            
            desiredSize = elem->GetDesiredSize();
            rect.right += desiredSize.Width();
            _allSize += desiredSize.Width();

            elem->Arrange(rect);

            if (-1 == _firstVisibleTab && rect.right >= 0)
            {
                _firstVisibleTab = i;
            }

            if (-1 == _lastVisibleTab && rect.right >= GetRenderSize().Width())
            {
                _lastVisibleTab = i;
            }

            rect.left = rect.right;
        }
    }
    else
    {
        rect.top = -_numRows;
        rect.bottom = rect.top;
        rect.right = _rowHeight;

        for (int i = 0; i < elemsColl->GetCount(); ++i)
        {
            elem = CASTTOFE(elemsColl->GetAt(i));
            if (elem->GetVisibility() == Visibility::Collapsed)
            {
                continue;
            }

            desiredSize = elem->GetDesiredSize();
            rect.bottom += desiredSize.Height();
            _allSize += desiredSize.Height();

            elem->Arrange(rect);

            if (-1 == _firstVisibleTab && rect.bottom >= 0)
            {
                _firstVisibleTab = i;
            }

            if (-1 == _lastVisibleTab && rect.bottom >= GetRenderSize().Height())
            {
                _lastVisibleTab = i;
            }

            rect.top = rect.bottom;
        }
    }
}

void TabPanel::ArrangeMultiHorizontal(Size arrangeSize)
{
    Dock tabStripPlacement = GetTabStripPlacement();
    bool flag = _numRows > 1;
    int activeRow = 0;
    Point vector;

    Array<int, false> solution;
    Array<Float, false> headersSize;

    GetHeadersSize(headersSize);

    if (flag)
    {
        CalculateHeaderDistribution(arrangeSize.Width(), headersSize, solution);
        activeRow = GetActiveRow(solution);
        if (tabStripPlacement == Dock::dockTop)
        {
            vector.y = ((_numRows - 1) - activeRow) * _rowHeight;
        }
        if ((tabStripPlacement == Dock::dockBottom) && (activeRow != 0))
        {
            vector.y = (_numRows - activeRow) * _rowHeight;
        }
    }
    int index = 0;
    int num3 = 0;
    ElementColl* elemsColl = GetInternalChildren();

    for (int i = 0; i < elemsColl->GetCount(); ++i)
    {
        FrameworkElement* elem = CASTTOFE(elemsColl->GetAt(i));

        if (elem->GetVisibility() == Visibility::Collapsed)
        {
            continue;
        }

        Rect thickness = elem->GetMargin();
        double left = thickness.left;
        double right = thickness.right;
        double top = thickness.top;
        double bottom = thickness.bottom;

        bool flag2 = flag && (((num3 < solution.Length()) && (solution[num3] == index)) || (index == (_numHeaders - 1)));
        Size size(headersSize[index], _rowHeight);

        if (flag2)
        {
            size.cx = arrangeSize.Width() - vector.x;
        }

        elem->Arrange(Rect(vector.x, vector.y, size.Width(), size.Height()));
        Size size2 = size;

        size2.cy = UFMAX((Float)0.0, (Float)((size2.Height() - top) - bottom));
        size2.cx = UFMAX((Float)0.0, (Float)((size2.Width() - left) - right));
        vector.x += size.Width();

        if (flag2)
        {
            if (((num3 == activeRow) && (tabStripPlacement == Dock::dockTop)) || 
                ((num3 == (activeRow - 1)) && (tabStripPlacement == Dock::dockBottom)))
            {
                vector.y = 0.0;
            }
            else
            {
                vector.y += _rowHeight;
            }
            vector.x = 0.0;
            num3++;
        }
        index++;
    }
}

void TabPanel::ArrangeMultiVertical(Size arrangeSize)
{
    float y = 0.0;
    ElementColl* elemsColl = GetInternalChildren();

    for (int i = 0; i < elemsColl->GetCount(); ++i)
    {
        FrameworkElement* elem = CASTTOFE(elemsColl->GetAt(i));

        if (elem->GetVisibility() != Visibility::Collapsed)
        {
            Size desiredSizeWithoutMargin = GetDesiredSizeWithoutMargin(elem);
            elem->Arrange(Rect(0.0, y, arrangeSize.Width(), desiredSizeWithoutMargin.Height()));
            y += desiredSizeWithoutMargin.Height();
        }
    }
}

void TabPanel::CalculateHeaderDistribution(Float rowWidthLimit, Array<Float, false>& headerWidth, Array<int, false>& array)
{
    Float num = 0.0;
    int length = headerWidth.GetCount();
    int num3 = _numRows - 1;
    Float num4 = 0.0;
    int num5 = 0;
    Float num6 = 0.0;

    int index = 0;

    Array<int, false> numArray;
    Array<int, false> numArray3;

    Array<Float, false> numArray4;
    Array<Float, false> numArray5;
    Array<Float, false> numArray6;

    numArray.Relength(num3);
    array.Relength(num3);
    numArray3.Relength(_numRows);

    numArray4.Relength(_numRows);
    numArray5.Relength(_numRows);
    numArray6.Relength(_numRows);

    for (int i = 0; i < length; i++)
    {
        if (((num4 + headerWidth[i]) > rowWidthLimit) && (num5 > 0))
        {
            numArray4[index] = num4;
            numArray3[index] = num5;
            num6 = UFMAX((Float)0.0, (Float)((rowWidthLimit - num4) / ((Float)num5)));
            numArray5[index] = num6;
            numArray[index] = i - 1;

            if (num < num6)
            {
                num = num6;
            }
            index++;
            num4 = headerWidth[i];
            num5 = 1;
        }
        else
        {
            num4 += headerWidth[i];
            if (headerWidth[i] != 0.0)
            {
                num5++;
            }
        }
    }

    if (index == 0)
    {
        array.Relength(0);
        return;
    }

    numArray4[index] = num4;
    numArray3[index] = num5;
    num6 = (rowWidthLimit - num4) / ((double) num5);
    numArray5[index] = num6;

    if (num < num6)
    {
        num = num6;
    }

    numArray.CopyTo(&array, 0);
    numArray5.CopyTo(&numArray6, 0);

    for (;;)
    {
        int num9 = 0;
        Float num10 = 0.0;

        for (int k = 0; k < _numRows; k++)
        {
            if (num10 < numArray5[k])
            {
                num10 = numArray5[k];
                num9 = k;
            }
        }
        if (num9 == 0)
        {
            break;
        }
        int num12 = num9;
        int num13 = num12 - 1;
        int num14 = numArray[num13];
        Float num15 = headerWidth[num14];
        numArray4[num12] += num15;

        if (numArray4[num12] > rowWidthLimit)
        {
            break;
        }

        numArray[num13]--;
        numArray3[num12]++;
        numArray4[num13] -= num15;
        numArray3[num13]--;
        numArray5[num13] = (rowWidthLimit - numArray4[num13]) / ((Float)numArray3[num13]);
        numArray5[num12] = (rowWidthLimit - numArray4[num12]) / ((Float)numArray3[num12]);
        num10 = 0.0;

        for (int m = 0; m < _numRows; m++)
        {
            if (num10 < numArray5[m])
            {
                num10 = numArray5[m];
            }
        }

        if (num10 < num)
        {
            num = num10;
            numArray.CopyTo(&array, 0);
            numArray5.CopyTo(&numArray6, 0);
        }
    }

    index = 0;

    for (int j = 0; j < length; j++)
    {
        headerWidth[j] += numArray6[index];
        if ((index < num3) && (array[index] == j))
        {
            index++;
        }
    }
}

int TabPanel::GetActiveRow(Array<int, false>& solution)
{
    int index = 0;
    int num2 = 0;

    if (solution.GetCount() > 0)
    {
        ElementColl* elemsColl = GetInternalChildren();

        for (int i = 0; i < elemsColl->GetCount(); ++i)
        {
            Element* elem = elemsColl->GetAt(i);
            if (elem->GetVisibility() != Visibility::Collapsed)
            {
                if (elem->GetValue(Selector::IsSelectedProperty)->ToBool())
                {
                    return index;
                }

                if ((index < solution.GetCount()) && (solution[index] == num2))
                {
                    index++;
                }
                num2++;
            }
        }
    }

    if (GetTabStripPlacement() == Dock::dockTop)
    {
        index = _numRows - 1;
    }

    return index;
}

Size TabPanel::GetDesiredSizeWithoutMargin(Element* elem)
{
    Rect rcMgr = CASTTOFE(elem)->GetMargin();
    Size size;
    size.cy = UFMAX((Float)0.0, (Float)((elem->GetDesiredSize().Height() - rcMgr.top) - rcMgr.bottom));
    size.cx = UFMAX((Float)0.0, (Float)((elem->GetDesiredSize().Width() - rcMgr.left) - rcMgr.right));
    return size;
}

void TabPanel::GetHeadersSize(Array<Float, false>& numArray)
{
    int index = 0;
    ElementColl* elemsColl = GetInternalChildren();

    numArray.Relength(_numHeaders);

    for (int i = 0; i < elemsColl->GetCount(); ++i)
    {
        Element* elem = elemsColl->GetAt(i);
        if (elem->GetVisibility() != Visibility::Collapsed)
        {
            Size desiredSizeWithoutMargin = GetDesiredSizeWithoutMargin(elem);
            numArray[index] = desiredSizeWithoutMargin.Width();
            index++;
        }
    }
}

void TabPanel::OnMouseWheel(MouseWheelEventArg* e)
{
    /*TabControl* tabCtrl = GetTabControl();
    if (tabCtrl->GetTabArrangeMode() == TabArrangeMode::tamSingle)
    {
        int iDeta = e->GetDelta();
        if (iDeta < 0)
        {
            ScrollToNext();
        }
        else
        {
            ScrollToPrev();
        }

        e->SetHandled(true);
    }*/
}

Size TabPanel::OnMeasure(const Size& constraint)
{
    TabControl* tabCtrl = RTTICast<TabControl>(GetTemplatedParent());

    if (NULL != tabCtrl)
    {
        tabCtrl->_tabPanel = this;
    }

    if (GetTabArrangeMode() == TabArrangeMode::tamSingle)
    {
        return MeasureSingle(constraint);
    }
    else
    {
        return MeasureMulti(constraint);
    }
}

Size TabPanel::MeasureMulti(Size constraint)
{
    Size size;
    Dock tabStripPlacement = GetTabStripPlacement();
    _numRows = 1;
    _numHeaders = 0;
    _rowHeight = 0.0;

    ElementColl* elemsColl = GetInternalChildren();

    if ((tabStripPlacement == Dock::dockLeft) || (tabStripPlacement == Dock::dockRight))
    {
        for (int i = 0; i < elemsColl->GetCount(); ++i)
        {
            Element* elem = elemsColl->GetAt(i);
            if (elem->GetVisibility() != Visibility::Collapsed)
            {
                _numHeaders++;
                MeasureChild(elem, constraint);
                Size desiredSizeWithoutMargin = GetDesiredSizeWithoutMargin(elem);
                if (size.Width() < desiredSizeWithoutMargin.Width())
                {
                    size.cx = desiredSizeWithoutMargin.Width();
                }
                size.cy += desiredSizeWithoutMargin.Height();
            }
        }

        return size;
    }

    int num = 0;
    Float width = 0.0f;
    Float num3 = 0.0f;

    for (int i = 0; i < elemsColl->GetCount(); ++i)
    {
        Element* elem = elemsColl->GetAt(i);
        if (elem->GetVisibility() == Visibility::Collapsed)
        {
            continue;
        }

        _numHeaders++;

        MeasureChild(elem, constraint);
        Size size2 = GetDesiredSizeWithoutMargin(elem);

        if (_rowHeight < size2.Height())
        {
            _rowHeight = size2.Height();
        }

        if (((width + size2.Width()) > constraint.Width()) && (num > 0))
        {
            if (num3 < width)
            {
                num3 = width;
            }
            width = size2.Width();
            num = 1;
            _numRows++;
            continue;
        }

        width += size2.Width();
        num++;
    }

    if (num3 < width)
    {
        num3 = width;
    }

    size.cy = _rowHeight * _numRows;

    if (FloatUtil::IsInfinity(size.cy) || FloatUtil::IsNaN(size.cx) || num3 < constraint.Width())
    {
        size.cx = num3;
        return size;
    }

    size.cx = constraint.Width();
    return size;
}

Size TabPanel::MeasureMenu(Size constraint)
{
    return constraint;
}

Size TabPanel::MeasureSingle(Size constraint)
{
    int prevSize = 0;
    suic::Size size;
   
    Dock tabStripPlacement = GetTabStripPlacement();
    ElementColl* elemsColl = GetInternalChildren();

    _numHeaders = 0;
    _rowHeight = 0.0;

    for (int i = 0; i < elemsColl->GetCount(); ++i)
    {
        Element* elem = elemsColl->GetAt(i);
        if (elem->GetVisibility() != Visibility::Collapsed)
        {
            _numHeaders++;
            MeasureChild(elem, constraint);
            Size desiredSizeWithoutMargin = GetDesiredSizeWithoutMargin(elem);
            switch (tabStripPlacement)
            {
                case Dock::dockLeft:
                case Dock::dockRight:
                    if (_rowHeight < desiredSizeWithoutMargin.Width())
                    {
                        _rowHeight = desiredSizeWithoutMargin.Width();
                    }

                    prevSize = size.cy;
                    size.cx = _rowHeight;
                    size.cy += desiredSizeWithoutMargin.Height();
                    break;

                case Dock::dockTop:
                case Dock::dockBottom:
                    if (_rowHeight < desiredSizeWithoutMargin.Height())
                    {
                        _rowHeight = desiredSizeWithoutMargin.Height();
                    }

                    prevSize = size.cx;
                    size.cy = _rowHeight;
                    size.cx += desiredSizeWithoutMargin.Width();
                    break;
            }
            
        }
    }

    if (_numRows > prevSize)
    {
        _numRows = prevSize;
    }

    return size;
}

}

