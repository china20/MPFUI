/////////////////////////////////////////////////////////////////////////
// ToolBar.cpp

#include <Extend/Controls/ToolBar.h>
#include <Extend/Controls/ToolBarPanel.h>
#include <Framework/Controls/Border.h>
#include <System/Windows/VisualStates.h>
#include <System/Tools/StyleHelper.h>
#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>

namespace suic
{

ImplementRTTIOfClass(ToolBar, HeaderedItemsControl)

DpProperty* ToolBar::BandIndexProperty;
DpProperty* ToolBar::BandProperty;
DpProperty* ToolBar::HasOverflowItemsProperty;
DpProperty* ToolBar::IsOverflowItemProperty;
DpProperty* ToolBar::IsOverflowOpenProperty;
DpProperty* ToolBar::OrientationProperty;
DpProperty* ToolBar::OverflowModeProperty;

ToolBar::ToolBar()
{
    ToolBar::StaticInit();
}

ToolBar::~ToolBar()
{
}

void ToolBar::StaticInit()
{
    if (NULL == OrientationProperty)
    {
        suic::HeaderedItemsControl::StaticInit();
        ToolBarPanel::StaticInit();

        OrientationProperty = DpProperty::Register(_T("Orientation"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(OrientationBox::HorizontalBox, PropMetadataOptions::AffectsMeasure));

        BandIndexProperty = DpProperty::Register(_T("BandIndex"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(0), PropMetadataOptions::AffectsParentMeasure));
        BandProperty = DpProperty::Register(_T("Band"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(0), PropMetadataOptions::AffectsParentMeasure));
        HasOverflowItemsProperty = DpProperty::Register(_T("HasOverflowItems"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        IsOverflowItemProperty = DpProperty::Register(_T("IsOverflowItem"), RTTIType(), Object::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
        IsOverflowOpenProperty = DpProperty::Register(_T("IsOverflowOpen"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        OverflowModeProperty = DpProperty::Register(_T("OverflowMode"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(OverflowMode::AsNeeded), PropMetadataOptions::AffectsNone));
    }
}

bool ToolBar::CoerceIsOverflowOpen(DpObject* d, bool val)
{
    if (val)
    {
        ToolBar* bar = DynamicCast<ToolBar>(d);

        if (!bar->IsLoaded())
        {
            bar->RegisterToOpenOnLoad();
            return false;
        }
    }
    return val;
}

Orientation ToolBar::CoerceOrientation(DpObject* d, Orientation val)
{
    ToolBar* toolBar = DynamicCast<ToolBar>(d);
    ToolBarTray* toolBarTray = toolBar->GetToolBarTray();

    if (toolBarTray == NULL)
    {
        return val;
    }
    else
    {
        return toolBarTray->GetOrientation();
    }
}

bool ToolBar::CoerceToolTipIsEnabled(DpObject* d, bool val)
{
    ToolBar* bar = DynamicCast<ToolBar>(d);

    if (!bar->IsOverflowOpen())
    {
        return val;
    }
    else
    {
        return false;
    }
}

bool ToolBar::GetIsOverflowItem(DpObject* elem)
{
    if (elem == NULL)
    {
        throw new ArgumentNullException("element", __FILELINE__);
    }

    return elem->GetValue(IsOverflowItemProperty)->ToBool();
}

bool ToolBar::IsValidOverflowMode(int val)
{
    OverflowMode mode = (OverflowMode)val;

    if ((mode != OverflowMode::AsNeeded) && (mode != OverflowMode::Always))
    {
        return (mode == OverflowMode::Never);
    }
    else
    {
        return true;
    }
}

void ToolBar::SetIsOverflowItem(DpObject* elem, Object* val)
{
    elem->SetValue(IsOverflowItemProperty, val);
}

void ToolBar::SetOverflowMode(DpObject* elem, OverflowMode mode)
{
    if (elem == NULL)
    {
        throw new ArgumentNullException("elem", __FILELINE__);
    }
    elem->SetValue(OverflowModeProperty, new Integer(mode));
}

OverflowMode ToolBar::GetOverflowMode(DpObject* elem)
{
    if (elem == NULL)
    {
        throw ArgumentNullException("elem", __FILELINE__);
    }
    return (OverflowMode)elem->GetValue(OverflowModeProperty)->ToInt();
}

Orientation ToolBar::GetOrientation()
{
    return (Orientation)GetValue(OrientationProperty)->ToInt();
}

ToolBarTray* ToolBar::GetToolBarTray()
{
    return DynamicCast<ToolBarTray>(GetParent());
}

int ToolBar::GetBand()
{
    return GetValue(BandProperty)->ToInt();
}

void ToolBar::SetBand(int val)
{
    SetValue(BandProperty, new Integer(val));
}

int ToolBar::GetBandIndex()
{
    return GetValue(BandIndexProperty)->ToInt();
}

void ToolBar::SetBandIndex(int val)
{
    SetValue(BandIndexProperty, new Integer(val));
}

bool ToolBar::IsOverflowOpen()
{
    return (bool)GetValue(IsOverflowOpenProperty)->ToBool();
}

void ToolBar::SetIsOverflowOpen(bool val)
{
    SetValue(IsOverflowOpenProperty, BOOLTOBOOLEAN(val));
}

int ToolBar::GetMinLength() const
{
    return _minLength;
}

int ToolBar::GetMaxLength() const
{
    return _maxLength;
}

ToolBarOverflowPanel* ToolBar::GetToolBarOverflowPanel()
{
    if (_toolBarOverflowPanel == NULL)
    {
        _toolBarOverflowPanel = FindToolBarOverflowPanel();
    }
    return _toolBarOverflowPanel;
}

ToolBarPanel* ToolBar::GetToolBarPanel()
{
    if (_toolBarPanel == NULL)
    {
        _toolBarPanel = FindToolBarPanel();
    }
    return _toolBarPanel;
}

ToolBarOverflowPanel* ToolBar::FindToolBarOverflowPanel()
{
    DpObject* templateChild = GetTemplateChild("PART_ToolBarOverflowPanel");
    ToolBarOverflowPanel* panel = DynamicCast<ToolBarOverflowPanel>(templateChild);

    if ((templateChild != NULL) && (panel == NULL))
    {
        throw ;
    }

    return panel;
}

ToolBarPanel* ToolBar::FindToolBarPanel()
{
    DpObject* templateChild = GetTemplateChild("PART_ToolBarPanel");
    ToolBarPanel* panel = DynamicCast<ToolBarPanel>(templateChild);

    if ((templateChild != NULL) && (panel == NULL))
    {
        throw;
    }
    return panel;
}

void ToolBar::InvalidateLayout()
{
    _minLength = 0;
    _maxLength = 0;
    InvalidateMeasure();
    ToolBarPanel* toolBarPanel = GetToolBarPanel();

    if (toolBarPanel != NULL)
    {
        toolBarPanel->InvalidateMeasure();
    }
}

void ToolBar::Close()
{
    SetValue(IsOverflowOpenProperty, Boolean::False);
}

Size ToolBar::OnMeasure(const Size& constraint)
{
    Size size = HeaderedItemsControl::OnMeasure(constraint);
    ToolBarPanel* toolBarPanel = GetToolBarPanel();

    if (toolBarPanel != NULL)
    {
        int num = 0;
        Rect margin = toolBarPanel->GetMargin();

        if (toolBarPanel->GetOrientation() == Orientation::Horizontal)
        {
            num = max((int)0, (int)(((size.Width() - toolBarPanel->GetDesiredSize().Width()) + margin.left) + margin.right));
        }
        else
        {
            num = max((int)0, (int)(((size.Height() - toolBarPanel->GetDesiredSize().Height()) + margin.top) + margin.bottom));
        }

        _minLength = toolBarPanel->GetMinLength() + num;
        _maxLength = toolBarPanel->GetMaxLength() + num;
    }

    return size;
}

void ToolBar::OnItemsChanged(NotifyCollChangedEventArg* e)
{
    InvalidateLayout();
    HeaderedItemsControl::OnItemsChanged(e);
}

void ToolBar::OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate)
{
    _toolBarPanel = NULL;
    _toolBarOverflowPanel = NULL;
    HeaderedItemsControl::OnTemplateChangedInternal(oldTemplate, newTemplate);
}

void ToolBar::OnMouseDown(Element* sender, MouseButtonEventArg* e)
{
    ToolBar* bar = DynamicCast<ToolBar>(sender);
    if (!e->IsHandled())
    {
        bar->Close();
        e->SetHandled(true);
    }
}

void ToolBar::OnOverflowModeChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ToolBar* bar = DynamicCast<ToolBar>(ItemsControl::ItemsControlFromItemContainer(d));
    if (bar != NULL)
    {
        bar->InvalidateLayout();
    }
}

void ToolBar::OnOverflowOpenChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ToolBar* bar = DynamicCast<ToolBar>(d);
    if (e->GetNewValue()->ToBool())
    {
        //Mouse.Capture(bar, CaptureMode.SubTree);
        //bar.SetFocusOnToolBarOverflowPanel();
    }
    else
    {
        ToolBarOverflowPanel* toolBarOverflowPanel = bar->GetToolBarOverflowPanel();
        if ((toolBarOverflowPanel != NULL) && toolBarOverflowPanel->IsKeyboardFocusWithin())
        {
            Keyboard::Focus(NULL);
        }
        if (Mouse::GetCaptured() == bar)
        {
            Mouse::SetCaptured(NULL);
        }
    }

    //bar->CoerceValue(ToolTipService.IsEnabledProperty);
}

void ToolBar::RegisterToOpenOnLoad()
{
    //LoadedEvent += new RoutedEventHandler(this.OpenOnLoad);
}

//----------------------------------------------------------------
//

DpProperty* ToolBarTray::OrientationProperty;

ToolBarCollection::ToolBarCollection(ToolBarTray* parent)
    : _parent(parent)
{
    _toolBars.Resize(4);
}

ToolBarCollection::~ToolBarCollection()
{
    Clear();
}

void ToolBarCollection::Clear()
{
    int count = GetCount();

    if (count > 0)
    {
        for (int i = 0; i < count; i++)
        {
            ToolBar* child = GetToolBar(i);

            _parent->RemoveVisualChild(child);
            _parent->RemoveLogicalChild(child);
            child->unref();
        }

        _parent->InvalidateMeasure();
    }

    _toolBars.Clear();
}

int ToolBarCollection::GetCount() const
{
    return _toolBars.GetCount();
}

ToolBar* ToolBarCollection::GetToolBar(int index) const
{
    return _toolBars[index];
}

bool ToolBarCollection::Contains(Object* value)
{
    return (IndexOf(value) != -1);
}

int ToolBarCollection::IndexOf(Object* val)
{
    for (int i = 0; i < _toolBars.Length(); ++i)
    {
        if (_toolBars[i] == val)
        {
            return i;
        }
    }
    return -1;
}

void ToolBarCollection::AddToolBar(ToolBar* toolBar)
{
    toolBar->ref();
    _toolBars.Add(toolBar);
    AddToolBarItem(toolBar);
}

void ToolBarCollection::AddToolBarItem(ToolBar* toolBar)
{
    _parent->AddLogicalChild(toolBar);
    _parent->AddVisualChild(toolBar);
    _parent->InvalidateMeasure();
}

void ToolBarCollection::InsertToolBar(int index, ToolBar* toolBar)
{
    toolBar->ref();
    _toolBars.Insert(index, toolBar);
    AddToolBarItem(toolBar);
}

void ToolBarCollection::RemoveToolBar(int index)
{
    ToolBar* child = GetToolBar(index);
    RemoveToolBarItem(child);
    _toolBars.RemoveAt(index);
}

void ToolBarCollection::SetToolBar(int index, ToolBar* toolBar)
{
    ToolBar* child = GetToolBar(index);

    if (toolBar != child)
    {
        RemoveToolBarItem(child);
        toolBar->ref();
        _toolBars.SetItem(index, toolBar);
        AddToolBarItem(toolBar);
    }
}

void ToolBarCollection::RemoveToolBarItem(ToolBar* child)
{
    _parent->RemoveVisualChild(child);
    _parent->RemoveLogicalChild(child);
    _parent->InvalidateMeasure();
    child->unref();
}

ToolBarTray::ToolBarTray()
    : _bandsDirty(false)
    , _toolBarsCollection(NULL)
{

}

ToolBarTray::~ToolBarTray()
{
    for (int i = 0; i < _bands.Length(); ++i)
    {
        delete _bands[i];
    }

    if (_toolBarsCollection)
    {
        delete _toolBarsCollection;
    }
}

void ToolBarTray::StaticInit()
{
    if (NULL == OrientationProperty)
    {
        OrientationProperty = DpProperty::Register(_T("Orientation"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(OrientationBox::HorizontalBox, PropMetadataOptions::AffectsMeasure));
    }
}

ToolBarCollection* ToolBarTray::GetToolBars()
{
    if (_toolBarsCollection == NULL)
    {
        _toolBarsCollection = new ToolBarCollection(this);
    }
    return _toolBarsCollection;
}

Orientation ToolBarTray::GetOrientation()
{
    return (Orientation)GetValue(OrientationProperty)->ToInt();
}

void ToolBarTray::SetOrientation(Orientation val)
{
    SetValue(OrientationProperty, OrientationBox::From(val));
}

BandInfo* ToolBarTray::CreateBand(int startIndex)
{
    ToolBarCollection* toolBars = GetToolBars();
    BandInfo* info = new BandInfo();
    ToolBar* item = toolBars->GetToolBar(startIndex);
    info->GetBand().Add(item);

    int band = item->GetBand();

    for (int i = startIndex + 1; i < toolBars->GetCount(); i++)
    {
        item = toolBars->GetToolBar(i);

        if (band == item->GetBand())
        {
            InsertToolBar(item, info->GetBand());
        }
    }

    return info;
}

void ToolBarTray::ExpandToolBars(Array<ToolBar*, false>& band, int startIndex, int endIndex, int expandAmount)
{
    if (GetOrientation() == Orientation::Horizontal)
    {
        for (int i = endIndex; i >= startIndex; i--)
        {
            ToolBar* bar = band[i];
            if (bar->GetRenderSize().Width() + expandAmount <= bar->GetMaxLength())
            {
                bar->SetWidth(bar->GetRenderSize().Width() + expandAmount);
                return;
            }

            bar->SetWidth(bar->GetMaxLength());
            expandAmount -= bar->GetMaxLength() - bar->GetRenderSize().Width();
        }
    }
    else
    {
        for (int j = endIndex; j >= startIndex; j--)
        {
            ToolBar* bar2 = band[j];
            if (bar2->GetRenderSize().Height() + expandAmount <= bar2->GetMaxLength())
            {
                bar2->SetHeight(bar2->GetRenderSize().Height() + expandAmount);
                return;
            }

            bar2->SetHeight(bar2->GetMaxLength());
            expandAmount -= bar2->GetMaxLength() - bar2->GetRenderSize().Height();
        }
    }
}

void ToolBarTray::GenerateBands()
{
    if (IsBandsDirty())
    {
        ToolBarCollection* toolBars = GetToolBars();
        _bands.Clear();

        for (int i = 0; i < toolBars->GetCount(); i++)
        {
            InsertBand(toolBars->GetToolBar(i), i);
        }

        for (int j = 0; j < _bands.Length(); j++)
        {
            Array<ToolBar*, false>& band = _bands[j]->GetBand();
            for (int k = 0; k < band.Length(); k++)
            {
                ToolBar* bar = band[k];
                bar->SetBand(j);
                bar->SetBandIndex(k);
            }
        }

        _bandsDirty = false;
    }
}

int ToolBarTray::GetBandFromOffset(int toolBarOffset)
{
    if (toolBarOffset < 0)
    {
        return -1;
    }

    int num = 0.0;
    for (int i = 0; i < _bands.Length(); i++)
    {
        num += _bands[i]->GetThickness();
        if (num > toolBarOffset)
        {
            return i;
        }
    }
    return _bands.Length();
}

void ToolBarTray::InsertBand(ToolBar* toolBar, int toolBarIndex)
{
    int band = toolBar->GetBand();

    for (int i = 0; i < _bands.Length(); i++)
    {
        int num3 = _bands[i]->GetBand().GetItem(0)->GetBand();

        if (band == num3)
        {
            return;
        }

        if (band < num3)
        {
            _bands.Insert(i, CreateBand(toolBarIndex));
            return;
        }
    }

    _bands.Add(CreateBand(toolBarIndex));
}

void ToolBarTray::InsertToolBar(ToolBar* toolBar, Array<ToolBar*, false>& band)
{
    for (int i = 0; i < band.Length(); i++)
    {
        if (toolBar->GetBandIndex() < band[i]->GetBandIndex())
        {
            band.Insert(i, toolBar);
            return;
        }
    }

    band.Add(toolBar);
}

bool ToolBarTray::IsBandsDirty()
{
    if (_bandsDirty)
    {
        return true;
    }

    int num = 0;
    ToolBarCollection* toolBars = GetToolBars();

    for (int i = 0; i < _bands.Length(); i++)
    {
        Array<ToolBar*, false>& band = _bands[i]->GetBand();

        for (int j = 0; j < band.Length(); j++)
        {
            ToolBar* item = band[j];
            if (((item->GetBand() != i) || (item->GetBandIndex() != j)) || !toolBars->Contains(item))
            {
                return true;
            }
        }
        num += band.Length();
    }

    return (num != toolBars->GetCount());
}

Element* ToolBarTray::GetVisualChild(int index)
{
    if (_toolBarsCollection == NULL)
    {
        throw ArgumentException("index", __FILELINE__);
    }

    return _toolBarsCollection->GetToolBar(index);
}

int ToolBarTray::GetVisualChildrenCount()
{
    if (_toolBarsCollection == NULL)
    {
        return 0;
    }
    else
    {
        return _toolBarsCollection->GetCount();
    }
}

void ToolBarTray::AddText(String val)
{

}

void ToolBarTray::AddChild(Object* child)
{
    if (child == NULL)
    {
        throw ArgumentNullException("child", __FILELINE__);
    }

    ToolBar* item = RTTICast<ToolBar>(child);

    if (item == NULL)
    {
        throw ArgumentException("UnexpectedParameterType", __FILELINE__);
    }

    GetToolBars()->AddToolBar(item);
}

void ToolBarTray::OnArrange(const Size& arrangeSize)
{
    bool flag = GetOrientation() == Orientation::Horizontal;
    Rect finalRect;

    for (int i = 0; i < _bands.Length(); i++)
    {
        Array<ToolBar*, false>& band = _bands[i]->GetBand();
        int thickness = _bands[i]->GetThickness();

        if (flag)
        {
            finalRect.left = 0;
        }
        else
        {
            finalRect.top = 0;
        }

        for (int j = 0; j < band.Length(); j++)
        {
            ToolBar* bar = band[j];
            Size size(flag ? bar->GetDesiredSize().Width() : thickness, flag ? thickness : bar->GetDesiredSize().Height());
            
            finalRect.right = finalRect.left + size.cx;
            finalRect.bottom = finalRect.top + size.cy;

            bar->Arrange(finalRect);

            if (flag)
            {
                finalRect.left += size.Width();
                finalRect.right += size.Width();
            }
            else
            {
                finalRect.top += size.Height();
                finalRect.bottom += size.Height();
            }
        }

        if (flag)
        {
            finalRect.top += thickness;
            finalRect.bottom += thickness;
        }
        else
        {
            finalRect.left += thickness;
            finalRect.right += thickness;
        }
    }
}

Size ToolBarTray::OnMeasure(const Size& constraint)
{
    GenerateBands();

    Size size;
    bool flag = GetOrientation() == Orientation::Horizontal;
    Size availableSize(Numeric::MeasureInt, Numeric::MeasureInt);

    for (int i = 0; i < _bands.Length(); i++)
    {
        int num3 = flag ? constraint.Width() : constraint.Height();
        Array<ToolBar*, false>& band = _bands[i]->GetBand();
        int num4 = 0;
        int num5 = 0;
        int num2 = 0;

        while (num2 < band.Length())
        {
            ToolBar* bar = band[num2];
            num3 -= bar->GetMinLength();

            if (num3 < 0)
            {
                num3 = 0;
                break;
            }
            num2++;
        }

        for (num2 = 0; num2 < band.Length(); num2++)
        {
            ToolBar* bar2 = band[num2];
            Size desiredSize = bar2->GetDesiredSize();

            num3 += bar2->GetMinLength();

            if (flag)
            {
                availableSize.cx = num3;
            }
            else
            {
                availableSize.cy = num3;
            }

            MeasureChild(bar2, availableSize);
            num4 = Math::Max(num4, flag ? desiredSize.Height() : desiredSize.Width());
            num5 += flag ? desiredSize.Width() : desiredSize.Height();
            num3 -= flag ? desiredSize.Width() : desiredSize.Height();

            if (num3 < 0)
            {
                num3 = 0.0;
            }
        }

        _bands[i]->SetThickness(num4);

        if (flag)
        {
            size.cy += num4;
            size.cx = max(size.Width(), num5);
        }
        else
        {
            size.cx += num4;
            size.cy = max(size.Height(), num5);
        }
    }

    return size;
}

}
