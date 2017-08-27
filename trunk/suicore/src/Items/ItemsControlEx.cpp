
#include <Framework/Items/ItemsControlEx.h>
#include <Framework/Controls/Content.h>
#include <Framework/Controls/HeaderedItemsControl.h>
#include <Framework/Controls/HeaderedContentControl.h>
#include <Framework/Controls/VisualizeStackPanel.h>

#include <Framework/Presenter/ContentPresenter.h>

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Input/KeyboardNavigation.h>

#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>

namespace suic
{

ImplementRTTIOfClass(ItemsControlEx, suic::Control)

DpProperty* ItemsControlEx::AlternationCountProperty;
DpProperty* ItemsControlEx::AlternationIndexProperty;
DpProperty* ItemsControlEx::DisplayMemberPathProperty;
DpProperty* ItemsControlEx::GroupStyleSelectorProperty;
DpProperty* ItemsControlEx::HasItemsProperty;
DpProperty* ItemsControlEx::IsGroupingProperty;
DpProperty* ItemsControlEx::IsTextSearchCaseSensitiveProperty;
DpProperty* ItemsControlEx::IsTextSearchEnabledProperty;
DpProperty* ItemsControlEx::ItemBindingGroupProperty;
DpProperty* ItemsControlEx::ItemContainerStyleProperty;
DpProperty* ItemsControlEx::ItemContainerStyleSelectorProperty;
DpProperty* ItemsControlEx::ItemsPanelProperty;
DpProperty* ItemsControlEx::ItemsSourceProperty;
DpProperty* ItemsControlEx::ItemTemplateProperty;
DpProperty* ItemsControlEx::ItemTemplateSelectorProperty;

bool ItemsControlEx::StaticInit()
{
    //FrameworkElement.DefaultStyleKeyProperty.OverrideMetadata(typeof(ItemsControl), new FrameworkPropertyMetadata(typeof(ItemsControl)));
    //_dType = DependencyObjectType.FromSystemTypeInternal(typeof(ItemsControl));
    //EventManager.RegisterClassHandler(typeof(ItemsControl), Keyboard.GotKeyboardFocusEvent, new KeyboardFocusChangedEventHandler(ItemsControl.OnGotFocus));
    return true;
}

void ItemsControlEx::AddChild(Object* value)
{
    GetItems()->Add(value);
}

void ItemsControlEx::AddText(String text)
{
    GetItems()->Add(new OString(text));
}

void ItemsControlEx::ApplyItemContainerStyle(DpObject* container, Object* item)
{
    FrameworkElement* obj2 = CASTTOFE(container);
    if (obj2->ReadInnerFlag(InnerFlags::IsStyleSetFromGenerator) || 
        (container->ReadLocalValue(FrameworkElement::StyleProperty, false) == DpProperty::UnsetValue()))
    {
        Style* itemContainerStyle = GetItemContainerStyle();
        if ((itemContainerStyle == NULL) && (GetItemContainerStyleSelector() != NULL))
        {
            itemContainerStyle = GetItemContainerStyleSelector()->SelectStyle(item, obj2);
        }
        if (itemContainerStyle != NULL)
        {
            if (!itemContainerStyle->GetTargetType()->IsInstanceOfType(container))
            {
                throw InvalidOperationException(_U("StyleForWrongType"), __FILELINE__);
            }
            obj2->SetStyle(itemContainerStyle);
            obj2->WriteInnerFlag(InnerFlags::IsStyleSetFromGenerator, true);
        }
        else if (obj2->ReadInnerFlag(InnerFlags::IsStyleSetFromGenerator))
        {
            obj2->WriteInnerFlag(InnerFlags::IsStyleSetFromGenerator, false);
            container->ClearValue(FrameworkElement::StyleProperty);
        }
    }
}

void ItemsControlEx::BeginInit()
{
    suic::Control::BeginInit();
    if (_items != NULL)
    {
        _items->BeginInit();
    }
}

void ItemsControlEx::CheckTemplateSource()
{
    /*if (GetDisplayMemberPath().empty())
    {
    //Helper.CheckTemplateAndTemplateSelector("Item", ItemTemplateProperty, ItemTemplateSelectorProperty, this);
    }
    else
    {
    if (!(GetItemTemplateSelector() is DisplayMemberTemplateSelector))
    {
    throw new InvalidOperationException(SR.Get("ItemTemplateSelectorBreaksDisplayMemberPath"));
    }
    if (Helper.IsTemplateDefined(ItemTemplateProperty, this))
    {
    throw new InvalidOperationException(SR.Get("DisplayMemberPathAndItemTemplateDefined"));
    }
    }*/
}

void ItemsControlEx::ClearAlternationIndex(DpObject* d)
{
    d->ClearValue(AlternationIndexProperty);
}

void ItemsControlEx::ClearContainerForItemOverride(DpObject* d, Object* item)
{
    HeaderedContentControl* control = RTTICast<HeaderedContentControl>(d);
    if (control != NULL)
    {
        control->ClearHeaderedContentControl(item);
    }
    else
    {
        ContentControl* control2 = RTTICast<ContentControl>(d);
        if (control2 != NULL)
        {
            control2->ClearContentControl(item);
        }
        else
        {
            ContentPresenter* presenter = RTTICast<ContentPresenter>(d);
            if (presenter != NULL)
            {
                presenter->ClearContentPresenter(item);
            }
            else
            {
                HeaderedItemsControl* control4 = RTTICast<HeaderedItemsControl>(d);
                if (control4 != NULL)
                {
                    //control4->ClearHeaderedItemsControl(item);
                }
                else
                {
                    ItemsControlEx* control3 = RTTICast<ItemsControlEx>(d);
                    if ((control3 != NULL) && (control3 != this))
                    {
                        control3->ClearItemsControl(item);
                    }
                }
            }
        }
    }
}

void ItemsControlEx::ClearItemsControl(Object* item)
{
}

DpObject* ItemsControlEx::ContainerFromElement(DpObject* d)
{
    return ContainerFromElement(this, d);
}

DpObject* ItemsControlEx::ContainerFromElement(ItemsControlEx* itemsControl, DpObject* d)
{
    if (d == NULL)
    {
        //throw new ArgumentNullException("element");
    }
    if (IsContainerForItemsControl(d, itemsControl))
    {
        return d;
    }
    return NULL;
}

void ItemsControlEx::CreateItemCollectionAndGenerator()
{
    _items = new ItemCollectionEx(this);
    _itemContainerGenerator = new ItemContainerGeneratorEx(this);
    _itemContainerGenerator->ChangeAlternationCount(0);
    _items->CollChanged += NotifyCollChangedEventHandler(this, &ItemsControlEx::OnItemCollectionChanged);
    
    if (IsInitPending())
    {
        _items->BeginInit();
    }
    else if (IsInitialized())
    {
        _items->BeginInit();
        _items->EndInit();
    }
    //_groupStyle->CollectionChanged += NotifyCollChangedEventHandler(this, &ItemsControlEx::OnGroupStyleChanged);
}

void ItemsControlEx::DoAutoScroll()
{
    DoAutoScroll(GetFocusedItem());
}

void ItemsControlEx::DoAutoScroll(Object* startingItem)
{
    FrameworkElement* relativeTo = GetScrollHost();
    if (NULL == relativeTo)
    {
        relativeTo = GetItemsHost();
    }

    if (relativeTo != NULL)
    {
        Point position = Mouse::GetPosition(relativeTo);
        Rect rect(Point(), relativeTo->GetRenderSize());
        bool flag = false;
        ItemNavigateArg e(MouseDevice::Current(), Keyboard::GetModifiers());

        if (position.y < rect.top)
        {
            NavigateByLine(startingItem, FocusNavDirection::fndUp, e);
            flag = startingItem != GetFocusedItem();
        }
        else if (position.y >= rect.bottom)
        {
            NavigateByLine(startingItem, FocusNavDirection::fndDown, e);
            flag = startingItem != GetFocusedItem();
        }
        if (!flag)
        {
            if (position.x < rect.left)
            {
                FocusNavDirection left = FocusNavDirection::fndLeft;
                if (IsRTL(relativeTo))
                {
                    left = FocusNavDirection::fndRight;
                }
                NavigateByLine(startingItem, left, e);
            }
            else if (position.x >= rect.right)
            {
                FocusNavDirection right = FocusNavDirection::fndRight;
                if (IsRTL(relativeTo))
                {
                    right = FocusNavDirection::fndLeft;
                }
                NavigateByLine(startingItem, right, e);
            }
        }
    }
}

void ItemsControlEx::EndInit()
{
    if (IsInitPending())
    {
        if (_items != NULL)
        {
            _items->EndInit();
        }
        suic::Control::EndInit();
    }
}

Object* ItemsControlEx::FindFocusable(int startIndex, int direction, int& foundIndex)
{
    if (HasItems())
    {
        int count = GetItems()->GetCount();
        while ((startIndex >= 0) && (startIndex < count))
        {
            FrameworkElement* element = CASTTOFE(GetItemContainerGenerator()->ContainerFromIndex(startIndex));
            if ((element == NULL) || Keyboard::IsFocusable(element))
            {
                foundIndex = startIndex;
                return GetItems()->GetItem(startIndex);
            }
            startIndex += direction;
        }
    }
    foundIndex = -1;
    return NULL;
}

void ItemsControlEx::FocusItem(Object* item, ItemNavigateArg& e)
{
    if (item != NULL)
    {
        Element* element = CASTTOE(GetItemContainerGenerator()->ContainerFromItem(item));
        if (element != NULL)
        {
            Keyboard::Focus(element);
        }
    }
    /*if (itemNavigateArgs->GetDeviceUsed() is KeyboardDevice)
    {
        KeyboardNavigation::ShowFocusVisual();
    }*/
}

int ItemsControlEx::GetAlternationIndex(DpObject* element)
{
    if (element == NULL)
    {
        //throw new ArgumentNullException("element");
    }
    return element->GetValue(AlternationIndexProperty)->ToInt();
}

DpObject* ItemsControlEx::GetContainerForItemOverride()
{
    return new ContentPresenter();
}

ItemsPanelTemplate* ItemsControlEx::GetDefaultItemsPanelTemplate()
{
    /*ItemsPanelTemplate* template = new ItemsPanelTemplate(new FEFactory(StackPanel::RTTIType()));
    template.Seal();
    return template;*/
    return NULL;
}

Object* ItemsControlEx::GetEncapsulatingItem(FrameworkElement* element)
{
    Object* unsetValue = DpProperty::UnsetValue();
    while ((unsetValue == DpProperty::UnsetValue()) && (element != NULL))
    {
        unsetValue = GetItemContainerGenerator()->ItemFromContainer(element);
        element = CASTTOFE(VisualTreeOp::GetParent(element));
    }
    return unsetValue;
}

Object* ItemsControlEx::GetFirstItemOnCurrentPage(Object* startingItem, FocusNavDirection direction, int& foundIndex)
{
    foundIndex = -1;
    if (IsLogicalVertical())
    {
        if (direction == FocusNavDirection::fndUp)
        {
            return FindFocusable(GetScrollHost()->GetVerticalOffset(), 1, foundIndex);
        }
        return FindFocusable(((GetScrollHost()->GetVerticalOffset() + GetScrollHost()->GetViewportHeight()) - 1.0), -1, foundIndex);
    }
    if (IsLogicalHorizontal())
    {
        if (direction == FocusNavDirection::fndUp)
        {
            return FindFocusable(GetScrollHost()->GetHorizontalOffset(), 1, foundIndex);
        }
        return FindFocusable(((GetScrollHost()->GetHorizontalOffset() + GetScrollHost()->GetViewportWidth()) - 1.0), -1, foundIndex);
    }
    FrameworkElement* element = CASTTOFE(GetItemContainerGenerator()->ContainerFromItem(startingItem));
    FrameworkElement* element2 = element;
    FrameworkElement* element3 = NULL;
    if (element == NULL)
    {
        return NULL;
    }
    while ((element2 != NULL) && !IsOnCurrentPage(element2, direction))
    {
        element3 = element2;
        element2 = CASTTOFE(KeyboardNavigation::Current()->PredictFocusedElement(element2, direction));
    }
    while ((element2 != NULL) && IsOnCurrentPage(element2, direction))
    {
        element3 = element2;
        element2 = CASTTOFE(KeyboardNavigation::Current()->PredictFocusedElement(element2, direction));
    }
    return GetEncapsulatingItem(element3);
}

Object* ItemsControlEx::GetItemOrContainerFromContainer(DpObject* container)
{
    Object* obj2 = GetItemContainerGenerator()->ItemFromContainer(container);
    if ((obj2 == DpProperty::UnsetValue()) && 
        (ItemsControlFromItemContainer(container) == this) && 
        IsItemItsOwnContainer(container))
    {
        obj2 = container;
    }
    return obj2;
}

ItemsControlEx* ItemsControlEx::GetItemsOwner(DpObject* element)
{
    ItemsControlEx* control = NULL;
    Panel* panel = RTTICast<Panel>(element);
    if ((panel == NULL) || !panel->IsItemsHost())
    {
        return control;
    }
    ItemsPresenter* presenter = ItemsPresenter::FromPanel(panel);
    if (presenter != NULL)
    {
        return (ItemsControlEx*)(presenter->GetOwner());
    }
    return RTTICast<ItemsControlEx>(panel->GetTemplatedParent());
}

bool ItemsControlEx::IsContainerForItemsControl(DpObject* element, ItemsControlEx* itemsControl)
{
    if (!element->ContainsValue(ItemContainerGeneratorEx::ItemForItemContainerProperty) || 
        ((itemsControl != NULL) && (itemsControl != ItemsControlFromItemContainer(element))))
    {
        return false;
    }
    return true;
}

bool ItemsControlEx::IsItemItsOwnContainerOverride(Object* item)
{
    return item->GetRTTIType()->InheritFrom(Element::RTTIType());
}

bool ItemsControlEx::IsOnCurrentPage(Object* item, FocusNavDirection axis)
{
    FrameworkElement* element = CASTTOFE(GetItemContainerGenerator()->ContainerFromItem(item));
    if (element == NULL)
    {
        return false;
    }
    return IsOnCurrentPage(element, axis, false);
}

bool ItemsControlEx::IsOnCurrentPage(FrameworkElement* element, FocusNavDirection axis)
{
    return IsOnCurrentPage(element, axis, false);
}

bool ItemsControlEx::IsOnCurrentPage(FrameworkElement* elem, FocusNavDirection axis, bool fullyVisible)
{
    FrameworkElement* scrollHost = GetScrollHost();
    if (scrollHost == NULL)
    {
        scrollHost = GetItemsHost();
    }
    if (scrollHost != NULL)
    {
        if (elem == NULL || !scrollHost->IsAncestorOf(elem))
        {
            return false;
        }

        Rect rect(Point(), scrollHost->GetRenderSize());
        Rect rect2(elem->TransformToAncestor(scrollHost), elem->GetRenderSize());

        if (fullyVisible)
        {
            return rect.Contains(rect2);
        }
        if ((axis == FocusNavDirection::fndUp) || (axis == FocusNavDirection::fndDown))
        {
            if (rect.top<= rect2.top && rect2.bottom <= rect.bottom)
            {
                return true;
            }
        }
        else if (((axis == FocusNavDirection::fndRight) || (axis == FocusNavDirection::fndLeft)) && 
            (rect.left <= rect2.left && rect2.right <= rect.right))
        {
            return true;
        }
    }
    return false;
}

bool ItemsControlEx::IsRTL(FrameworkElement* element)
{
    return (element->GetFlowDirection() == FlowDirection::RightToLeft);
}

ItemsControlEx* ItemsControlEx::ItemsControlFromItemContainer(DpObject* container)
{
    Element* current = CASTTOE(container);
    if (current == NULL)
    {
        return NULL;
    }
    ItemsControlEx* parent = RTTICast<ItemsControlEx>(LogicalTreeOp::GetParent(current));
    if (parent != NULL)
    {
        IGeneratorHost* host = parent;
        if (host->IsItemItsOwnContainer(current))
        {
            return parent;
        }
        return NULL;
    }
    current = CASTTOE(VisualTreeOp::GetParent(current));
    return GetItemsOwner(current);
}

bool ItemsControlEx::MakeVisible(int index)
{
    return MakeVisible(index, false, false);
}

bool ItemsControlEx::MakeVisible(int index, bool alwaysAtTopOfViewport, bool alignMinorAxisToo)
{
    if (index == -1)
    {
        return false;
    }

    ScrollViewer* scrollView = GetScrollHost();
    if (scrollView == NULL)
    {
        return false;
    }
    bool flag = false;
    int horizontalOffset = scrollView->GetHorizontalOffset();
    int verticalOffset = scrollView->GetVerticalOffset();
    int num3 = horizontalOffset;
    int num4 = verticalOffset;

    if (IsLogicalVertical())
    {
        if (alwaysAtTopOfViewport)
        {
            num4 = index;
        }
        else
        {
            if (index + 1 > verticalOffset + scrollView->GetViewportHeight())
            {
                num4 = max((double) 0.0, (double) ((index + 1) - scrollView->GetViewportHeight()));
            }
            if (index < verticalOffset)
            {
                num4 = index;
            }
        }
        if (alignMinorAxisToo)
        {
            num3 = 0.0;
        }
        if (horizontalOffset != num3)
        {
            scrollView->ScrollToHorizontalOffset(num3);
            flag = true;
        }
        if (verticalOffset != num4)
        {
            scrollView->ScrollToVerticalOffset(num4);
            flag = true;
        }
        return flag;
    }
    if (IsLogicalHorizontal())
    {
        if (alwaysAtTopOfViewport)
        {
            num3 = index;
        }
        else
        {
            if (index + 1 > horizontalOffset + scrollView->GetViewportWidth())
            {
                num3 = max(0, ((index + 1) - scrollView->GetViewportWidth()));
            }
            if (index < horizontalOffset)
            {
                num3 = index;
            }
        }
        if (alignMinorAxisToo)
        {
            num4 = 0;
        }
        if (horizontalOffset != num3)
        {
            scrollView->ScrollToHorizontalOffset(num3);
            flag = true;
        }
        if (verticalOffset != num4)
        {
            scrollView->ScrollToVerticalOffset(num4);
            flag = true;
        }
        return flag;
    }
    FrameworkElement* element = CASTTOFE(GetItemContainerGenerator()->ContainerFromIndex(index));
    if (element != NULL)
    {
        element->BringIntoView();
        flag = horizontalOffset != scrollView->GetHorizontalOffset() || 
            verticalOffset != scrollView->GetVerticalOffset();
    }
    return flag;
}

// 实现 IGeneratorHost接口
void ItemsControlEx::ClearContainerForItem(DpObject* container, Object* item)
{
    GroupItem* item2 = RTTICast<GroupItem>(container);
    if (item2 == NULL)
    {
        ClearContainerForItemOverride(container, item);
    }
    else
    {
        ItemContainerGeneratorEx* generator = item2->GetGenerator();
        if (generator != NULL)
        {
            //generator->RemoveAll();
        }
    }

    /*if ((GetRTTIType()->IsInstanceOfType(TreeViewItem) || (GetRTTIType()->IsInstanceOfType(TreeView))) && IsVirtualizing())
    {
        ItemsControlEx* itemsControl = RTTICast<ItemsControlEx>(container);
        if ((itemsControl != NULL) && VirtualizingStackPanel::GetIsVirtualizing(container))
        {
            VirtualizingStackPanel* itemsHost = RTTICast<VirtualizingStackPanel>(itemsControl->GetItemsHost());
            if (itemsHost != NULL)
            {
                itemsHost->ClearAllContainers(itemsControl);
            }
        }
        //StoreItemValues(container, item, ItemValueStorageIndices);
    }*/
}

DpObject* ItemsControlEx::GetContainerForItem(Object* item)
{
    DpObject* containerForItemOverride;
    if (IsItemItsOwnContainerOverride(item))
    {
        containerForItemOverride = RTTICast<DpObject>(item);
    }
    else
    {
        containerForItemOverride = GetContainerForItemOverride();
    }

    Element* reference = CASTTOE(containerForItemOverride);
    if (reference != NULL)
    {
        Element* parent = VisualTreeOp::GetParent(reference);
        if (parent == NULL)
        {
            return containerForItemOverride;
        }

        Panel* panel = RTTICast<Panel>(parent);
        if (panel != NULL)
        {
            //panel->GetChildren()->RemoveNoVerify(reference);
            return containerForItemOverride;
        }
        ((FrameworkElement*)parent)->SetTemplateChild(NULL);
    }
    return containerForItemOverride;
}

GroupStyle* ItemsControlEx::GetGroupStyle(CollectionViewGroup* group, int level)
{
    GroupStyle* style = NULL;
    if (GetGroupStyleSelector() != NULL)
    {
        style = GetGroupStyleSelector()->SelectStyle(group, level);
    }
    if (style == NULL)
    {
        if (level >= GetGroupStyle().GetCount())
        {
            level = GetGroupStyle().GetCount() - 1;
        }
        if (level >= 0)
        {
            style = GetGroupStyle().GetItem(level);
        }
    }
    return style;
}

bool ItemsControlEx::IsHostForItemContainer(DpObject* container)
{
    ItemsControlEx* control = ItemsControlFromItemContainer(container);
    if (control != NULL)
    {
        return (control == this);
    }
    if (LogicalTreeOp::GetParent(container) != NULL)
    {
        return false;
    }
    return ((IsItemItsOwnContainerOverride(container) && HasItems()) && GetItems()->Contains(container));
}

bool ItemsControlEx::IsItemItsOwnContainer(Object* item)
{
    return IsItemItsOwnContainerOverride(item);
}

void ItemsControlEx::PrepareItemContainer(DpObject* container, Object* item)
{
    GroupItem* item2 = RTTICast<GroupItem>(container);
    if (item2 != NULL)
    {
        item2->PrepareItemContainer(item);
    }
    else
    {
        if (ShouldApplyItemContainerStyle(container, item))
        {
            ApplyItemContainerStyle(container, item);
        }
        PrepareContainerForItemOverride(container, item);
        /*if (!Helper.HasUnmodifiedDefaultValue(this, ItemBindingGroupProperty) && Helper.HasUnmodifiedDefaultOrInheritedValue(container, FrameworkElement.BindingGroupProperty))
        {
        BindingGroup itemBindingGroup = ItemBindingGroup;
        BindingGroup group2 = (itemBindingGroup != NULL) ? new BindingGroup(itemBindingGroup) : NULL;
        container.SetValue(FrameworkElement.BindingGroupProperty, group2);
        }
        if (((container == item) && TraceData.IsEnabled) && ((ItemTemplate != NULL) || (ItemTemplateSelector != NULL)))
        {
        TraceData.Trace(TraceEventType.Error, TraceData.ItemTemplateForDirectItem, AvTrace.TypeName(item));
        }
        if (((this is TreeViewItem) || (this is TreeView)) && IsVirtualizing)
        {
        SetItemValuesOnContainer(container, item, ItemValueStorageIndices);
        }*/
    }
}

void ItemsControlEx::SetIsGrouping(bool isGrouping)
{
    SetValue(IsGroupingProperty, BOOLTOBOOLEAN(isGrouping));
}

void ItemsControlEx::NavigateByLine(FocusNavDirection direction, ItemNavigateArg& itemNavigateArgs)
{
    NavigateByLine(GetFocusedItem(), direction, itemNavigateArgs);
}

void ItemsControlEx::NavigateByLine(Object* startingItem, FocusNavDirection direction, ItemNavigateArg& itemNavigateArgs)
{
    if (GetItemsHost() != NULL)
    {
        if ((startingItem != NULL) && !IsOnCurrentPage(startingItem, direction))
        {
            MakeVisible(GetItems()->IndexOf(startingItem));
            GetItemsHost()->UpdateLayout();
        }
        NavigateByLineInternal(startingItem, direction, itemNavigateArgs);
    }
}

void ItemsControlEx::NavigateByLineInternal(Object* startingItem, FocusNavDirection direction, ItemNavigateArg& itemNavigateArgs)
{
    if (startingItem == NULL)
    {
        NavigateToStart(itemNavigateArgs);
    }
    else
    {
        FrameworkElement* descendant = NULL;
        FrameworkElement* element2 = NULL;
        descendant = CASTTOFE(GetItemContainerGenerator()->ContainerFromItem(startingItem));
        if ((descendant == NULL) || !GetItemsHost()->IsAncestorOf(descendant))
        {
            descendant = GetScrollHost();
        }
        element2 = CASTTOFE(KeyboardNavigation::Current()->PredictFocusedElement(descendant, direction));
        if ((element2 != NULL) && GetItemsHost()->IsAncestorOf(element2))
        {
            Object* encapsulatingItem = GetEncapsulatingItem(element2);
            if (encapsulatingItem != DpProperty::UnsetValue())
            {
                NavigateToItem(encapsulatingItem, itemNavigateArgs);
            }
        }
    }
}

void ItemsControlEx::NavigateByPage(FocusNavDirection direction, ItemNavigateArg& itemNavigateArgs)
{
    NavigateByPage(GetFocusedItem(), direction, itemNavigateArgs);
}

void ItemsControlEx::NavigateByPage(Object* startingItem, FocusNavDirection direction, ItemNavigateArg& itemNavigateArgs)
{
    if (GetItemsHost() != NULL)
    {
        if ((startingItem != NULL) && !IsOnCurrentPage(startingItem, direction))
        {
            ScrollViewer* scrollView = GetScrollHost();
            while (MakeVisible(GetItems()->IndexOf(startingItem)))
            {
                int horizontalOffset = scrollView->GetHorizontalOffset();
                int verticalOffset = scrollView->GetVerticalOffset();
                GetItemsHost()->UpdateLayout();
                if (horizontalOffset == scrollView->GetHorizontalOffset() && 
                    verticalOffset == scrollView->GetVerticalOffset())
                {
                    break;
                }
            }
        }
        NavigateByPageInternal(startingItem, direction, itemNavigateArgs);
    }
}

void ItemsControlEx::NavigateByPageInternal(Object* startingItem, FocusNavDirection direction, ItemNavigateArg& itemNavigateArgs)
{
    if (startingItem == NULL)
    {
        NavigateToFirstItemOnCurrentPage(startingItem, direction, itemNavigateArgs);
    }
    else
    {
        int num;
        Object* obj2 = GetFirstItemOnCurrentPage(startingItem, direction, num);
        if (!startingItem->Equals(obj2))
        {
            if (obj2 != DpProperty::UnsetValue())
            {
                NavigateToItem(obj2, num, itemNavigateArgs);
            }
        }
        else
        {
            bool flag = false;
            if (GetScrollHost() != NULL)
            {
                switch (direction)
                {
                case FocusNavDirection::fndUp:
                    if (!IsLogicalHorizontal())
                    {
                        GetScrollHost()->PageUp();
                    }
                    else
                    {
                        GetScrollHost()->PageLeft();
                    }
                    flag = true;
                    break;

                case FocusNavDirection::fndDown:
                    if (!IsLogicalHorizontal())
                    {
                        GetScrollHost()->PageDown();
                    }
                    else
                    {
                        GetScrollHost()->PageRight();
                    }
                    flag = true;
                    break;
                }
            }
            if (flag && (GetItemsHost() != NULL))
            {
                GetItemsHost()->UpdateLayout();
                NavigateToFirstItemOnCurrentPage(startingItem, direction, itemNavigateArgs);
            }
        }
    }
}

void ItemsControlEx::NavigateToEnd(ItemNavigateArg& itemNavigateArgs)
{
    if (HasItems())
    {
        int num;
        Object* item = FindFocusable(GetItems()->GetCount() - 1, -1, num);
        NavigateToItem(item, num, itemNavigateArgs);
    }
}

void ItemsControlEx::NavigateToFirstItemOnCurrentPage(Object* startingItem, FocusNavDirection direction, ItemNavigateArg& itemNavigateArgs)
{
    int num;
    Object* item = GetFirstItemOnCurrentPage(startingItem, direction, num);
    if (item != DpProperty::UnsetValue())
    {
        FocusItem(item, itemNavigateArgs);
    }
}

void ItemsControlEx::NavigateToItem(Object* item, ItemNavigateArg& itemNavigateArgs)
{
    NavigateToItem(item, -1, itemNavigateArgs, false);
}

void ItemsControlEx::NavigateToItem(Object* item, int itemIndex, ItemNavigateArg& itemNavigateArgs)
{
    NavigateToItem(item, itemIndex, itemNavigateArgs, false);
}

void ItemsControlEx::NavigateToItem(Object* item, ItemNavigateArg& itemNavigateArgs, bool alwaysAtTopOfViewport)
{
    NavigateToItem(item, -1, itemNavigateArgs, alwaysAtTopOfViewport);
}

void ItemsControlEx::NavigateToItem(Object* item, int elementIndex, ItemNavigateArg& itemNavigateArgs, bool alwaysAtTopOfViewport)
{
    if (item != DpProperty::UnsetValue())
    {
        if (elementIndex != -1)
        {
            goto Label_0069;
        }
        elementIndex = GetItems()->IndexOf(item);
        if (elementIndex != -1)
        {
            goto Label_0069;
        }
    }
    return;
Label_0069:

    ScrollViewer* scrollView = GetScrollHost();
    while (MakeVisible(elementIndex, alwaysAtTopOfViewport, false))
    {
        int horizontalOffset = scrollView->GetHorizontalOffset();
        int verticalOffset = scrollView->GetVerticalOffset();
        GetItemsHost()->UpdateLayout();
        if (horizontalOffset == scrollView->GetHorizontalOffset() && 
            verticalOffset == scrollView->GetVerticalOffset())
        {
            break;
        }
    }
    FocusItem(item, itemNavigateArgs);
}

void ItemsControlEx::NavigateToStart(ItemNavigateArg& e)
{
    if (HasItems())
    {
        int num;
        Object* item = FindFocusable(0, 1, num);
        NavigateToItem(item, num, e);
    }
}

void ItemsControlEx::OnAlternationCountChanged(int oldAlternationCount, int newAlternationCount)
{
    GetItemContainerGenerator()->ChangeAlternationCount(0);
}

void ItemsControlEx::OnAlternationCountChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ItemsControlEx* control = (ItemsControlEx*)d;
    int oldValue = e->GetOldValue()->ToInt();
    int newValue = e->GetNewValue()->ToInt();
    //control->OnAlternationCountChanged(oldValue, newValue);
}

Object* ItemsControlEx::OnBringItemIntoView(Object* e)
{
    FrameworkElement* element = CASTTOFE(GetItemContainerGenerator()->ContainerFromItem(e));
    if (element != NULL)
    {
        element->BringIntoView();
    }
    else if (!IsGrouping() && GetItems()->Contains(e))
    {
        VirtualizingPanel* itemsHost = RTTICast<VirtualizingPanel>(GetItemsHost());
        if (itemsHost != NULL)
        {
            itemsHost->BringIndexIntoView(GetItems()->IndexOf(e));
        }
    }
    return NULL;
}

void ItemsControlEx::OnDisplayMemberPathChanged(String oldDisplayMemberPath, String newDisplayMemberPath)
{
}

void ItemsControlEx::OnDisplayMemberPathChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ItemsControlEx* control = (ItemsControlEx*) d;
    //control->OnDisplayMemberPathChanged(e->GetOldValue()->ToString(), e->GetNewValue()->ToString());
    //control->UpdateDisplayMemberTemplateSelector();
}

void ItemsControlEx::OnGotFocus(Object* sender, KeyboardFocusEventArg* e)
{
    ItemsControlEx* control = (ItemsControlEx*)sender;
    Element* originalSource = CASTTOE(e->GetOriginalSource());
    if ((originalSource != NULL) && (originalSource != control))
    {
        Object* obj2 = control->GetItemContainerGenerator()->ItemFromContainer(originalSource);
        if (obj2 != DpProperty::UnsetValue())
        {
            control->_focusedItem = obj2;
        }
    }
}

void ItemsControlEx::OnGroupStyleChanged(Object* sender, NotifyCollChangedEventArg* e)
{
    if (_itemContainerGenerator != NULL)
    {
        _itemContainerGenerator->Refresh();
    }
}

void ItemsControlEx::OnGroupStyleSelectorChanged(GroupStyleSelector* oldGroupStyleSelector, GroupStyleSelector* newGroupStyleSelector)
{
    if (_itemContainerGenerator != NULL)
    {
        _itemContainerGenerator->Refresh();
    }
}

void ItemsControlEx::OnGroupStyleSelectorChanged(DpObject* d, DpPropChangedEventArg* e)
{
    //((ItemsControl*)d)->OnGroupStyleSelectorChanged((GroupStyleSelector*)e->GetOldValue(), (GroupStyleSelector*)e->GetNewValue());
}

void ItemsControlEx::OnItemCollectionChanged(Object* sender, NotifyCollChangedEventArg* e)
{
    bool hasItems = (_items != NULL) && !_items->IsEmpty();
    SetValue(HasItemsProperty, BOOLTOBOOLEAN(hasItems));
    if ((((e->GetAction() == NotifyCollectionChangedAction::Remove) && 
        (_focusedItem.IsAlive())) && _focusedItem.GetTarget()->Equals(e->GetOldItem())) || 
        (e->GetAction() == NotifyCollectionChangedAction::Reset))
    {
        _focusedItem = NULL;
    }
    _itemContainerGenerator->OnCollectionChanged(sender, e);
    OnItemsChanged(e);
}

void ItemsControlEx::OnItemContainerStyleChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ((ItemsControlEx*)d)->OnItemContainerStyleChanged((Style*)e->GetOldValue(), (Style*)e->GetNewValue());
}

void ItemsControlEx::OnItemContainerStyleChanged(Style* oldItemContainerStyle, Style* newItemContainerStyle)
{
    //Helper.CheckStyleAndStyleSelector("ItemContainer", ItemContainerStyleProperty, ItemContainerStyleSelectorProperty, this);
    if (_itemContainerGenerator != NULL)
    {
        _itemContainerGenerator->Refresh();
    }
}

void ItemsControlEx::OnItemContainerStyleSelectorChanged(StyleSelector* oldItemContainerStyleSelector, StyleSelector* newItemContainerStyleSelector)
{
    //Helper.CheckStyleAndStyleSelector("ItemContainer", ItemContainerStyleProperty, ItemContainerStyleSelectorProperty, this);
    if ((_itemContainerGenerator != NULL) && (GetItemContainerStyle() == NULL))
    {
        _itemContainerGenerator->Refresh();
    }
}

void ItemsControlEx::OnItemContainerStyleSelectorChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ((ItemsControlEx*)d)->OnItemContainerStyleSelectorChanged((StyleSelector*)e->GetOldValue(), (StyleSelector*)e->GetNewValue());
}

void ItemsControlEx::OnItemsChanged(NotifyCollChangedEventArg* e)
{
}

void ItemsControlEx::OnItemsPanelChanged(ItemsPanelTemplate* oldItemsPanel, ItemsPanelTemplate* newItemsPanel)
{
    //GetItemContainerGenerator()->OnPanelChanged();
}

void ItemsControlEx::OnItemsPanelChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ((ItemsControlEx*)d)->OnItemsPanelChanged((ItemsPanelTemplate*)e->GetOldValue(), (ItemsPanelTemplate*)e->GetNewValue());
}

void ItemsControlEx::OnItemsSourceChanged(IEnumerable* oldValue, IEnumerable* newValue)
{
}

void ItemsControlEx::OnItemsSourceChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ItemsControlEx* control = (ItemsControlEx*) d;
    IEnumerable* oldValue = (IEnumerable*)e->GetOldValue();
    IEnumerable* newValue = (IEnumerable*)e->GetNewValue();
    if ((e->GetNewValue() == NULL))
    {
        //control->GetItems()->ClearItemsSource();
    }
    else
    {
        //control->GetItems()->SetItemsSource(newValue);
    }
    control->OnItemsSourceChanged(oldValue, newValue);
}

void ItemsControlEx::OnItemTemplateChanged(DataTemplate* oldItemTemplate, DataTemplate* newItemTemplate)
{
    CheckTemplateSource();
    if (_itemContainerGenerator != NULL)
    {
        _itemContainerGenerator->Refresh();
    }
}

void ItemsControlEx::OnItemTemplateChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ((ItemsControlEx*)d)->OnItemTemplateChanged((DataTemplate*)e->GetOldValue(), (DataTemplate*)e->GetNewValue());
}

void ItemsControlEx::OnItemTemplateSelectorChanged(DataTemplateSelector* oldItemTemplateSelector, DataTemplateSelector* newItemTemplateSelector)
{
    CheckTemplateSource();
    if ((_itemContainerGenerator != NULL) && (GetItemTemplate() == NULL))
    {
        _itemContainerGenerator->Refresh();
    }
}

void ItemsControlEx::OnItemTemplateSelectorChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ((ItemsControlEx*)d)->OnItemTemplateSelectorChanged((DataTemplateSelector*)e->GetOldValue(), (DataTemplateSelector*)e->GetNewValue());
}

void ItemsControlEx::OnKeyDown(KeyboardEventArg* e)
{
    suic::Control::OnKeyDown(e);
    if (IsTextSearchEnabled() && (e->GetKey() == Key::kBack))
    {
        /*TextSearch search = TextSearch.EnsureInstance(this);
        if (search != NULL)
        {
        search.DeleteLastCharacter();
        }*/
    }
}

void ItemsControlEx::OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate)
{
    _itemsHost = NULL;
    _scrollHost = NULL;
    suic::Control::WriteItemFlags(Control::ItemBoolFlags::ScrollHostValid, false);
    suic::Control::OnTemplateChangedInternal(oldTemplate, newTemplate);
}

void ItemsControlEx::OnTextInput(KeyboardEventArg* e)
{
    suic::Control::OnTextInput(e);
    /*if ((!String.IsNullOrEmpty(e.Text) && IsTextSearchEnabled) && ((e.OriginalSource == this) || (ItemsControlFromItemContainer(e.OriginalSource as DpObject*) == this)))
    {
    TextSearch search = TextSearch.EnsureInstance(this);
    if (search != NULL)
    {
    search.DoSearch(e.Text);
    e.Handled = true;
    }
    }*/
}

void ItemsControlEx::PrepareContainerForItemOverride(DpObject* d, Object* item)
{
    HeaderedContentControl* control = RTTICast<HeaderedContentControl>(d);
    if (control != NULL)
    {
        control->PrepareHeaderedContentControl(item, GetItemTemplate(), GetItemTemplateSelector());
    }
    else
    {
        ContentControl* control2 = RTTICast<ContentControl>(d);
        if (control2 != NULL)
        {
            control2->PrepareContentControl(item, GetItemTemplate(), GetItemTemplateSelector());
        }
        else
        {
            ContentPresenter* presenter = RTTICast<ContentPresenter>(d);
            if (presenter != NULL)
            {
                presenter->PrepareContentPresenter(item, GetItemTemplate(), GetItemTemplateSelector());
            }
            else
            {
                HeaderedItemsControl* control4 = RTTICast<HeaderedItemsControl>(d);
                if (control4 != NULL)
                {
                    //control4->PrepareHeaderedItemsControl(item, this);
                }
                else
                {
                    ItemsControlEx* control3 = RTTICast<ItemsControlEx>(d);
                    if ((control3 != NULL) && (control3 != this))
                    {
                        //control3->PrepareItemsControl(item, this);
                    }
                }
            }
        }
    }
}

void ItemsControlEx::PrepareItemsControl(Object* item, ItemsControlEx* parentItemsControl)
{
    if (item != this)
    {
        DataTemplate* itemTemplate = parentItemsControl->GetItemTemplate();
        DataTemplateSelector* itemTemplateSelector = parentItemsControl->GetItemTemplateSelector();

        Style* itemContainerStyle = parentItemsControl->GetItemContainerStyle();
        StyleSelector* itemContainerStyleSelector = parentItemsControl->GetItemContainerStyleSelector();
        int alternationCount = parentItemsControl->GetAlternationCount();

        if (itemTemplate != NULL)
        {
            SetValue(ItemTemplateProperty, itemTemplate);
        }
        if (itemTemplateSelector != NULL)
        {
            SetValue(ItemTemplateSelectorProperty, itemTemplateSelector);
        }

        if ((itemContainerStyle != NULL) && HasDefaultValue(ItemContainerStyleProperty, true))
        {
            SetValue(ItemContainerStyleProperty, itemContainerStyle);
        }
        if ((itemContainerStyleSelector != NULL) && HasDefaultValue(ItemContainerStyleSelectorProperty, true))
        {
            SetValue(ItemContainerStyleSelectorProperty, itemContainerStyleSelector);
        }
        if ((alternationCount != 0) && HasDefaultValue(AlternationCountProperty, true))
        {
            SetValue(AlternationCountProperty, new Integer(alternationCount));
        }
    }
}

void ItemsControlEx::RemoveItemContainerStyle(DpObject* container)
{
    FrameworkElement* obj2 = CASTTOFE(container);
    if (obj2->ReadInnerFlag(InnerFlags::IsStyleSetFromGenerator))
    {
        container->ClearValue(FrameworkElement::StyleProperty);
    }
}

void ItemsControlEx::SetAlternationIndex(DpObject* d, int value)
{
    d->SetValue(AlternationIndexProperty, new Integer(value));
}

bool ItemsControlEx::ShouldApplyItemContainerStyle(DpObject* container, Object* item)
{
    return true;
}

bool ItemsControlEx::ShouldSerializeGroupStyle()
{
    return (GetGroupStyle().GetCount() > 0);
}

void ItemsControlEx::UpdateDisplayMemberTemplateSelector()
{
    /*String displayMemberPath = GetDisplayMemberPath();
    String itemStringFormat = GetItemStringFormat();

    if (!displayMemberPath.IsEmpty() || !itemStringFormat.IsEmpty())
    {
        DataTemplateSelector itemTemplateSelector = ItemTemplateSelector;
        if (((itemTemplateSelector != NULL) && !(itemTemplateSelector is DisplayMemberTemplateSelector)) && ((base.ReadLocalValue(ItemTemplateSelectorProperty) != DpProperty::UnsetValue()) || (base.ReadLocalValue(DisplayMemberPathProperty) == DpProperty::UnsetValue())))
        {
            throw new InvalidOperationException(SR.Get("DisplayMemberPathAndItemTemplateSelectorDefined"));
        }
        ItemTemplateSelector = new DisplayMemberTemplateSelector(DisplayMemberPath, ItemStringFormat);
    }
    else if (ItemTemplateSelector is DisplayMemberTemplateSelector)
    {
        ClearValue(ItemTemplateSelectorProperty);
    }*/
}

int ItemsControlEx::GetAlternationCount()
{
    return GetValue(AlternationCountProperty)->ToInt();
}

void ItemsControlEx::SetAlternationCount(int val)
{
    SetValue(AlternationCountProperty, new Integer(val));
}

String ItemsControlEx::GetDisplayMemberPath()
{
    return GetValue(DisplayMemberPathProperty)->ToString();
}

void ItemsControlEx::SetDisplayMemberPath(String val)
{
    SetValue(DisplayMemberPathProperty, new OString(val));
}

Object* ItemsControlEx::GetFocusedItem()
{
    return _focusedItem.GetTarget();
}

Array<GroupStyle*>& ItemsControlEx::GetGroupStyle()
{
    return _groupStyle;
}

GroupStyleSelector* ItemsControlEx::GetGroupStyleSelector()
{
    return RTTICast<GroupStyleSelector>(GetValue(GroupStyleSelectorProperty));
}

void ItemsControlEx::SetGroupStyleSelector(GroupStyleSelector* val)
{
    SetValue(GroupStyleSelectorProperty, val);
}

bool ItemsControlEx::HasItems()
{
    return GetValue(HasItemsProperty)->ToBool();
}

bool ItemsControlEx::IsGrouping()
{
    return GetValue(IsGroupingProperty)->ToBool();
}

bool ItemsControlEx::IsInitPending() const
{
    return ReadFlag(CoreFlags::IsInitPending);
}

bool ItemsControlEx::IsLogicalHorizontal()
{
    if (GetItemsHost() == NULL)
    {
        return false;
    }
    else
    {
        return GetItemsHost()->GetLogicalOrientation() == Orientation::Horizontal;
    }
}

bool ItemsControlEx::IsLogicalVertical()
{
    if (GetItemsHost() == NULL)
    {
        return false;
    }
    else
    {
        return GetItemsHost()->GetLogicalOrientation() == Orientation::Vertical;
    }
}

bool ItemsControlEx::IsTextSearchCaseSensitive()
{
    return GetValue(IsTextSearchCaseSensitiveProperty)->ToBool();
}
void ItemsControlEx::SetIsTextSearchCaseSensitive(bool val)
{
    SetValue(IsTextSearchCaseSensitiveProperty, BOOLTOBOOLEAN(val));
}

bool ItemsControlEx::IsTextSearchEnabled()
{
    return GetValue(IsTextSearchEnabledProperty)->ToBool();
}
void ItemsControlEx::SetIsTextSearchEnabled(bool val)
{
    SetValue(IsTextSearchEnabledProperty, BOOLTOBOOLEAN(val));
}

bool ItemsControlEx::IsVirtualizing()
{
    return VirtualizingStackPanel::GetIsVirtualizing(this);
}

ItemContainerGeneratorEx* ItemsControlEx::GetItemContainerGenerator()
{
    if (_itemContainerGenerator == NULL)
    {
        CreateItemCollectionAndGenerator();
    }
    return _itemContainerGenerator;
}

Style* ItemsControlEx::GetItemContainerStyle()
{
    return RTTICast<Style>(GetValue(ItemContainerStyleProperty));
}
void ItemsControlEx::SetItemContainerStyle(Style* val)
{
    SetValue(ItemContainerStyleProperty, val);
}

StyleSelector* ItemsControlEx::GetItemContainerStyleSelector()
{
    return RTTICast<StyleSelector>(GetValue(ItemContainerStyleSelectorProperty));
}

void ItemsControlEx::SetItemContainerStyleSelector(StyleSelector* val)
{
    SetValue(ItemContainerStyleSelectorProperty, val);
}

ItemCollectionEx* ItemsControlEx::GetItems()
{
    if (_items == NULL)
    {
        CreateItemCollectionAndGenerator();
    }
    return _items;
}

Panel* ItemsControlEx::GetItemsHost() const
{
    return _itemsHost;
}

void ItemsControlEx::SetItemsHost(Panel* val)
{
    _itemsHost = val;
}

ItemsPanelTemplate* ItemsControlEx::GetItemsPanel()
{
    return RTTICast<ItemsPanelTemplate>(GetValue(ItemsPanelProperty));
}
void ItemsControlEx::SetItemsPanel(ItemsPanelTemplate* val)
{
    SetValue(ItemsPanelProperty, val);
}

IEnumerable* ItemsControlEx::GetItemsSource()
{
    return NULL;//GetItems()->GetItemsSource();
}

void ItemsControlEx::SetItemsSource(IEnumerable* val)
{
    if (val == NULL)
    {
        ClearValue(ItemsSourceProperty);
    }
    else
    {
        //SetValue(ItemsSourceProperty, val);
    }
}

DataTemplate* ItemsControlEx::GetItemTemplate()
{
    return RTTICast<DataTemplate>(GetValue(ItemTemplateProperty));
}
void ItemsControlEx::SetItemTemplate(DataTemplate* val)
{
    SetValue(ItemTemplateProperty, val);
}

DataTemplateSelector* ItemsControlEx::GetItemTemplateSelector()
{
    return RTTICast<DataTemplateSelector>(GetValue(ItemTemplateSelectorProperty));
}

void ItemsControlEx::SetItemTemplateSelector(DataTemplateSelector* val)
{
    SetValue(ItemTemplateSelectorProperty, val);
}

ItemCollectionEx* ItemsControlEx::GetView()
{
    return GetItems();
}

ScrollViewer* ItemsControlEx::GetScrollHost()
{
    if (NULL == _scrollHost && GetItemsHost() != NULL)
    {
        Element* elem = GetItemsHost()->GetUIParent();
        while (elem != NULL)
        {
            ScrollViewer* sv = RTTICast<ScrollViewer>(elem);
            if (NULL != sv)
            {
                _scrollHost = sv;
                break;
            }
            elem = elem->GetUIParent();
        }
    }
    return _scrollHost;
}

}
