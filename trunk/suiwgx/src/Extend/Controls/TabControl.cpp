
#include <Extend/Controls/TabControl.h>
#include <Framework/Controls/Button.h>

#include <System/Tools/VisualTreeOp.h>
#include <System/Input/KeyboardNavigation.h>

namespace suic
{

DpProperty* TabControl::ContentTemplateProperty;
DpProperty* TabControl::ContentTemplateSelectorProperty;
DpProperty* TabControl::SelectedContentProperty;
DpProperty* TabControl::SelectedContentTemplateProperty;
DpProperty* TabControl::SelectedContentTemplateSelectorProperty;

DpProperty* TabControl::TabArrangeModeProperty;
DpProperty* TabControl::TabStripPlacementProperty;

ImplementRTTIOfClass(TabControl, Selector)

void TabControl::StaticInit()
{
    if (NULL == TabStripPlacementProperty)
    {
        TabItem::StaticInit();
        TabPanel::StaticInit();

        SelectedContentProperty = DpProperty::RegisterReadOnly("SelectedContent", RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL));
        ContentTemplateProperty = DpProperty::Register("ContentTemplate", RTTIType(), DataTemplate::RTTIType(), DpPropMemory::GetPropMeta(NULL));

        SelectedContentTemplateProperty = DpProperty::RegisterReadOnly("SelectedContentTemplate", RTTIType(), DataTemplate::RTTIType(), DpPropMemory::GetPropMeta(NULL));
        SelectedContentTemplateSelectorProperty = DpProperty::RegisterReadOnly("SelectedContentTemplateSelector", RTTIType(), DataTemplateSelector::RTTIType(), DpPropMemory::GetPropMeta(NULL));
        ContentTemplateSelectorProperty = DpProperty::Register("ContentTemplateSelector", RTTIType(), DataTemplateSelector::RTTIType(), DpPropMemory::GetPropMeta(NULL));

        /*SelectedContentTemplateSelectorProperty = SelectedContentTemplateSelectorPropertyKey.DependencyProperty;
        SelectedContentStringFormatPropertyKey = DependencyProperty.RegisterReadOnly("SelectedContentStringFormat", typeof(string), typeof(TabControl), new FrameworkPropertyMetadata(null));
        SelectedContentStringFormatProperty = SelectedContentStringFormatPropertyKey.DependencyProperty;
        ContentStringFormatProperty = DependencyProperty.Register("ContentStringFormat", typeof(string), typeof(TabControl), new FrameworkPropertyMetadata(null));
        */

        TabStripPlacementProperty = DpProperty::Register("TabStripPlacement", RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(Dock::dockTop), 0, &OnTabStripPlacementPropChanged));
        TabArrangeModeProperty = TabPanel::TabArrangeModeProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(TabArrangeMode::tamMultiple), 0, &OnTabArrangeModePropChanged));

        TabStripPlacementProperty->SetConvertValueCb(DockConvert::Convert);
        IsTabStopProperty->OverrideMetadata(RTTIType(), new PropMetadata(Boolean::False));

        KeyboardNavigation::DirectionalNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knContained)));
    }
}

TabControl::TabControl()
    : _tabPanel(NULL)
{
    StaticInit();
}

TabControl::~TabControl()
{

}

TabItem* TabControl::FindNextTabItem(int startIndex, int direction)
{
    if (direction != 0)
    {
        int index = startIndex;
        for (int i = 0; i < GetCount(); i++)
        {
            index += direction;
            if (index >= GetCount())
            {
                index = 0;
            }
            else if (index < 0)
            {
                index = GetCount() - 1;
            }

            TabItem* pItem = RTTICast<TabItem>(GetItemContainerGenerator()->ContainerFromIndex(index));
            if (((pItem != NULL) && pItem->IsEnabled()) && 
                (pItem->GetVisibility() == Visibility::Visible))
            {
                return pItem;
            }
        }
    }

    return NULL;
}

Element* TabControl::GetContainerForItemOverride()
{
    TabItem* pItem = new TabItem();
    return pItem;
}

TabItem* TabControl::GetSelectedTabItem()
{
    Object* selectedItem = GetSelectedItem();
    if (selectedItem == NULL)
    {
        return NULL;
    }

    TabItem* container = RTTICast<TabItem>(selectedItem);
    if (container == NULL)
    {
        container = RTTICast<TabItem>(GetItemContainerGenerator()->ContainerFromIndex(GetSelectedIndex()));
        Object* pSelItem = container->GetContainerItem();

        if ((container != NULL) && !selectedItem->Equals(pSelItem))
        {
            container = RTTICast<TabItem>(GetItemContainerGenerator()->ContainerFromItem(selectedItem));
        }
    }
    return container;
}

bool TabControl::IsItemItsOwnContainer(Object* item)
{
    return (item->GetRTTIType()->InheritFrom(TabItem::RTTIType()));
}

Element* TabControl::FindName(const String& name)
{
    Element* elem = Selector::FindName(name);
    if (NULL == elem)
    {
        ItemCollection* items = GetItemsSource();
        for (int i = 0; i < items->GetCount(); i++)
        {
            TabItem* item = RTTICast<TabItem>(GetItemContainerGenerator()->ContainerFromIndex(i));
            if (item != NULL)
            {
                elem = item->FindName(name);
                if (NULL != elem)
                {
                    break;
                }
            }
        }
    }
    return elem;
}

void TabControl::OnApplyTemplate()
{
    Selector::OnApplyTemplate();
    UpdateSelectedContent();

    AddButtonClick(_U("PART_PrevTab"));
    AddButtonClick(_U("PART_NextTab"));
}

void TabControl::AddButtonClick(const String& name)
{
    Button* tabBtn = RTTICast<Button>(GetTemplateChild(name));
    if (NULL != tabBtn)
    {
        ClickEventHandler* h = new ClickEventHandler(this, &TabControl::OnTabNavigateClick);
        tabBtn->RemoveClick(h);
        tabBtn->AddClick(h);
    }
}

void TabControl::OnInitialized(EventArg* e)
{
    Selector::OnInitialized(e);
    //SetSelectMultiple(false);
    //ItemContainerGenerator.StatusChanged += new EventHandler(this.OnGeneratorStatusChanged);
}

void TabControl::OnArrange(const Size& arrangeSize)
{
    Selector::OnArrange(arrangeSize);
    Element* pPrev = GetTemplateChild(_U("PART_PrevTab"));
    Element* pNext = GetTemplateChild(_U("PART_NextTab"));

    if (_tabPanel != NULL && NULL != pPrev && NULL != pNext)
    {
        if (_tabPanel->AllTabsInVisible())
        {
            pPrev->SetVisibility(Visibility::Collapsed);
            pNext->SetVisibility(Visibility::Collapsed);
        }
        else
        {
            pPrev->SetVisibility(Visibility::Visible);
            pNext->SetVisibility(Visibility::Visible);
        }
    }
}

void TabControl::OnItemsChanged(NotifyCollChangedEventArg* e)
{
    Selector::OnItemsChanged(e);

    if ((e->GetAction() == NotifyCollectionChangedAction::Remove) && (GetSelectedIndex() == -1))
    {
        int startIndex = e->GetOldIndex() + 1;
        if (startIndex > GetCount())
        {
            startIndex = 0;
        }

        TabItem* item = FindNextTabItem(startIndex, -1);
        if (item != NULL)
        {
            item->SetCurrentValue(TabItem::IsSelectedProperty, Boolean::True);
        }
    }
}

void TabControl::OnKeyDown(KeyboardEventArg* e)
{
    TabItem* item = NULL;
    int direction = 0;
    int startIndex = -1;
    switch (e->GetKey())
    {
    case Key::kEnd:
        direction = -1;
        startIndex = GetCount();
        break;

    case Key::kHome:
        direction = 1;
        startIndex = -1;
        break;

    case Key::kTab:
        if ((e->GetModifierKey() & ModifierKey::mkControl) == ModifierKey::mkControl)
        {
            startIndex = GetItemsSource()->IndexOf(GetSelectedItem());
            
            if ((e->GetModifierKey() & ModifierKey::mkShift) == ModifierKey::mkShift)
            {
                direction = -1;
            }
            else
            {
                direction = 1;
            }
        }
        break;
    }

    item = FindNextTabItem(startIndex, direction);

    if ((item != NULL) && (item != GetSelectedItem()))
    {
        e->SetHandled(item->SetFocus());
    }

    if (!e->IsHandled())
    {
        Selector::OnKeyDown(e);
    }
}

void TabControl::OnMouseWheel(MouseWheelEventArg* e)
{
    if (GetTabArrangeMode() == TabArrangeMode::tamSingle && 
        _tabPanel != NULL)
    {
        int iDeta = e->GetDelta();
        if (iDeta < 0)
        {
            _tabPanel->ScrollToNext();
        }
        else
        {
            _tabPanel->ScrollToPrev();
        }

        e->SetHandled(true);
    }
}

void TabControl::OnSelectionChanged(SelectionChangedEventArg* e)
{
    Selector::OnSelectionChanged(e);
    if (IsKeyboardFocusWithin())
    {
        TabItem* selectedTabItem = GetSelectedTabItem();
        if (selectedTabItem != NULL)
        {
            selectedTabItem->Focus();

            if (GetTabArrangeMode() == TabArrangeMode::tamSingle && _tabPanel != NULL)
            {
                _tabPanel->ScrollToIntoView(selectedTabItem);
            }
        }
    }

    UpdateSelectedContent();
}

void TabControl::PrepareContainerForItemOverride(DpObject* elem, ItemEntry* item)
{
    Selector::PrepareContainerForItemOverride(elem, item);

    if (HasItems() && (_selectedItems.GetCount() == 0))
    {
        SetCurrentValue(Selector::SelectedIndexProperty, 0);
    }

    if (NULL != item && GetSelectedItem() == item->GetItem())
    {
        TabItem* tabItem = RTTICast<TabItem>(elem);
        tabItem->SetCurrentValue(TabItem::IsSelectedProperty, Boolean::True);
    }

    UpdateSelectedContent();
}

void TabControl::OnTabNavigateClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    String strName(sender->GetName());
    e->SetHandled(true);
    if (_tabPanel != NULL)
    {
        if (strName.Equals(_U("PART_PrevTab")))
        {
            _tabPanel->ScrollToPrev();
        }
        else if (strName.Equals(_U("PART_NextTab")))
        {
            _tabPanel->ScrollToNext();
        }
    }
}

void TabControl::OnTabArrangeModePropChanged(DpObject* d, DpPropChangedEventArg* e)
{

}

void TabControl::OnTabStripPlacementPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TabControl* control = RTTICast<TabControl>(d);
    control->CoerceTabItemStripPlacement();
}

void TabControl::CoerceTabItemStripPlacement()
{
    ItemCollection* items = GetItemsSource();

    for (int i = 0; i < items->GetCount(); i++)
    {
        TabItem* item = RTTICast<TabItem>(GetItemContainerGenerator()->ContainerFromIndex(i));
        if (item != NULL)
        {
            item->CoerceValue(TabItem::TabStripPlacementProperty);
        }
    }
}

void TabControl::UpdateSelectedContent()
{
    if (GetSelectedIndex() < 0)
    {
        SetSelectedContent(NULL);
        SetSelectedContentTemplate(NULL);
        SetSelectedContentTemplateSelector(NULL);
    }
    else
    {
        TabItem* selectedTabItem = GetSelectedTabItem();
        if (selectedTabItem != NULL)
        {
            FrameworkElement* parent = RTTICast<FrameworkElement>(VisualTreeOp::GetParent(selectedTabItem));
            if (parent != NULL)
            {
                KeyboardNavigation::SetTabOnceActiveElement(parent, selectedTabItem);
                KeyboardNavigation::SetTabOnceActiveElement(this, parent);
            }

            SetSelectedContent(selectedTabItem->GetContent());
            ContentPresenter* selectedContentPresenter = GetSelectedContentPresenter();
            if (selectedContentPresenter != NULL)
            {
                selectedContentPresenter->SetHorizontalAlignment(selectedTabItem->GetHorizontalContentAlignment());
                selectedContentPresenter->SetVerticalAlignment(selectedTabItem->GetVerticalContentAlignment());
            }

            if (((selectedTabItem->GetContentTemplate() != NULL) || 
                (selectedTabItem->GetContentTemplateSelector() != NULL)))
            {
                SetSelectedContentTemplate(selectedTabItem->GetContentTemplate());
                SetSelectedContentTemplateSelector(selectedTabItem->GetContentTemplateSelector());
            }
            else
            {
                SetSelectedContentTemplate(GetContentTemplate());
                SetSelectedContentTemplateSelector(GetContentTemplateSelector());
            }
        }
    }
}

DataTemplate* TabControl::GetContentTemplate()
{
    return RTTICast<DataTemplate>(GetValue(ContentTemplateProperty));
}

void TabControl::SetContentTemplate(DataTemplate* val)
{
    SetValue(ContentTemplateProperty, val);
}

DataTemplateSelector* TabControl::GetContentTemplateSelector()
{
    return RTTICast<DataTemplateSelector>(GetValue(ContentTemplateSelectorProperty));
}

void TabControl::SetContentTemplateSelector(DataTemplateSelector* val)
{
    SetValue(ContentTemplateSelectorProperty, val);
}

Object* TabControl::GetSelectedContent()
{
    return GetValue(SelectedContentProperty);
}

void TabControl::SetSelectedContent(Object* val)
{
    SetValue(SelectedContentProperty, val);
}

ContentPresenter* TabControl::GetSelectedContentPresenter()
{
    return RTTICast<ContentPresenter>(GetTemplateChild("PART_SelectedContentHost"));
}

TabArrangeMode TabControl::GetTabArrangeMode()
{
    return (TabArrangeMode)(GetValue(TabArrangeModeProperty)->ToInt());
}

void TabControl::SetTabArrangeMode(TabArrangeMode val)
{
    SetValue(TabArrangeModeProperty, Integer::GetPosInt(val));
}

DataTemplate* TabControl::GetSelectedContentTemplate()
{
    return RTTICast<DataTemplate>(GetValue(SelectedContentTemplateProperty));
}

void TabControl::SetSelectedContentTemplate(DataTemplate* val)
{
    SetValue(SelectedContentTemplateProperty, val);
}

DataTemplateSelector* TabControl::GetSelectedContentTemplateSelector()
{
    return RTTICast<DataTemplateSelector>(GetValue(SelectedContentTemplateSelectorProperty));
}

void TabControl::SetSelectedContentTemplateSelector(DataTemplateSelector* val)
{
    SetValue(SelectedContentTemplateSelectorProperty, val);
}

Dock TabControl::GetTabStripPlacement()
{
    return (Dock)GetValue(TabStripPlacementProperty)->ToInt();
}

void TabControl::SetTabStripPlacement(Dock val)
{
    SetValue(TabStripPlacementProperty, Integer::GetPosInt(val));
}

}
