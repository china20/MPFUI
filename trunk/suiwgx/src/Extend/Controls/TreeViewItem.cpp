
#include <Extend/Controls/TreeViewItem.h>
#include <Extend/Controls/TreeView.h>

#include <Framework/Controls/VisualizeStackPanel.h>
#include <Framework/Animation/AnimationAsSeries.h>

#include <System/Animation/Transform.h>
#include <System/Input/MouseDevice.h>
#include <System/Tools/EventHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>
#include <System/Input/KeyboardNavigation.h>

namespace suic
{

ImplementRTTIOfClass(TreeViewItem, HeaderedItemsControl)

RoutedEvent* TreeViewItem::ExpandedEvent;
RoutedEvent* TreeViewItem::CollapsedEvent;

DpProperty* TreeViewItem::IndentProperty;
DpProperty* TreeViewItem::IndentValueProperty;
DpProperty* TreeViewItem::ClickExpandProperty;
DpProperty* TreeViewItem::IsExpandedProperty;
DpProperty* TreeViewItem::IsSelectedProperty;
DpProperty* TreeViewItem::IsSelectionActiveProperty;

enum eTIPending
{
    tipExpand = 1,
    tipFocused = 2,
};

void TreeViewItem::OnIndentChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TreeViewItem* pItem = RTTICast<TreeViewItem>(d);
    if (pItem != NULL)
    {
        pItem->_indent = e->GetNewValue()->ToInt();
    }
}

void TreeViewItem::OnIndentValueChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TreeViewItem* pItem = RTTICast<TreeViewItem>(d);
    if (pItem != NULL)
    {
        pItem->_indentValue = e->GetNewValue()->ToInt();
    }
}

static void DoCollapsedTreeViewItem(TreeViewItem* pItem)
{
    TreeViewItem* parent = pItem->GetParentTreeViewItem();

    while (parent)
    {
        ItemEntry* ditem = parent->GetItemEntry();
        if (ditem != NULL)
        {
            ditem->ResetCache();
        }
        parent = parent->GetParentTreeViewItem();
    }
}

static void ResetItemCacheSize(TreeView* pTreeView, TreeViewItem* pItem)
{
    TreeViewItem* currItem = pItem;
    bool bHori = pTreeView->IsLogicalHorizontal();

    while (NULL != currItem)
    {
        ItemEntry* pLink = currItem->GetItemEntry();
        Size cacheSize = pLink->GetSize();

        if (bHori)
        {
            cacheSize.cy = 0;
        }
        else
        {
            cacheSize.cx = 0;
        }

        pLink->SetSize(cacheSize);
        currItem = currItem->GetParentTreeViewItem();
    }
}

void TreeViewItem::OnIsExpandedChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TreeViewItem* pItem = RTTICast<TreeViewItem>(d);
    bool newValue = e->GetNewValue()->ToBool();

    TreeView* pTreeView = pItem->GetParentTreeView();
    HierarchicalItem* ditem = RTTICast<HierarchicalItem>(pItem->GetContainerItem());

    if (ditem != NULL)
    {
        ditem->SetExpanded(newValue);
    }

    pItem->WriteItemFlags(IsExpandedFlag, newValue);

    if (pTreeView != NULL)
    {
        if ((pItem->_pending & eTIPending::tipExpand) == 0)
        {
            if (!newValue)
            {
                pTreeView->DoCollapsedAndSelection(pItem);
            }

            pTreeView->UpdateLayout();
        }
    }

    if (!newValue)
    {
        ResetItemCacheSize(pTreeView, pItem);
    }

    //DoCollapsedTreeViewItem(pItem);

    if (newValue)
    {
        RoutedEventArg re(pItem, ExpandedEvent);
        pItem->OnExpanded(&re);
    }
    else
    {
        RoutedEventArg re(pItem, CollapsedEvent);
        pItem->OnCollapsed(&re);
    }

    pItem->UpdateVisualState(false);
    //Element* toggle = pItem->GetTemplateChild(_U("Expander"));

    //if (NULL != toggle)
    //{
    //    if (newValue)
    //    {
    //        RotateTransform* trans = new RotateTransform(0, 0.5, 0.5);

    //        toggle->SetRenderTransform(trans);
    //        trans->BeginAnimation(RotateTransform::AngleProperty, new DoubleAnimation(0, 180, Duration(200), FillBehavior::fbHoldEnd));
    //        //RotateAnimation
    //    }
    //}
}

void TreeViewItem::OnIsSelectedChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TreeViewItem* pItem = RTTICast<TreeViewItem>(d);
    if (NULL != pItem)
    {
        bool newValue = e->GetNewValue()->ToBool();
        ItemEntry* ditem = pItem->GetItemEntry();
        TreeView* pTreeView = pItem->GetParentTreeView();

        if (ditem != NULL)
        {
            pTreeView->ChangeSelection(ditem->GetItem(), pItem, newValue, false);
        }
        else
        {
            pTreeView->_selCachedItem = pItem;
        }

        pItem->UpdateVisualState(false);
    }
}

void TreeViewItem::OnMouseButtonDown(Element* sender, MouseButtonEventArg* e)
{
    TreeViewItem* item = DynamicCast<TreeViewItem>(sender);
    TreeView* parentTreeView = item->GetParentTreeView();
    if (parentTreeView != NULL && !e->IsHandled())
    {
        parentTreeView->HandleMouseButtonDown(item);
    }
}

void TreeViewItem::StaticInit()
{
    if (NULL == IndentProperty)
    {
        ExpandedEvent = EventHelper::RegisterRoutedEvent(_U("Expanded"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
        CollapsedEvent = EventHelper::RegisterRoutedEvent(_U("Collapsed"), RoutingStrategy::Bubble, RTTIType(), RTTIType());

        IndentProperty = DpProperty::Register(_U("Indent"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(0), PropMetadataOptions::AffectsNone, &TreeViewItem::OnIndentChanged));
        IndentValueProperty = DpProperty::Register(_U("IndentValue"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(0), PropMetadataOptions::AffectsNone, &TreeViewItem::OnIndentValueChanged));
        ClickExpandProperty = DpProperty::Register(_U("ClickExpand"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsMeasure));
        IsExpandedProperty = DpProperty::Register(_U("IsExpanded"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsMeasure, &TreeViewItem::OnIsExpandedChanged));
        IsSelectedProperty = DpProperty::RegisterAttached(_U("IsSelected"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender, &TreeViewItem::OnIsSelectedChanged));
        IsSelectionActiveProperty = DpProperty::RegisterReadOnly(_U("IsSelectionActive"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));

        //EventHelper::RegisterClassHandler(RTTIType(), Element::MouseDownEvent, new MouseEventHandler(&TreeViewItem::OnMouseButtonDown), true);

        KeyboardNavigation::DirectionalNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knContinue)));
        KeyboardNavigation::TabNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knNone)));
    }
}

TreeViewItem::TreeViewItem()
    : _indent(0)
    , _indentValue(0)
    , _pending(0)
{
    WriteItemFlags(Control::CantCachedFocusedItem, true);
    WriteFlag(CoreFlags::IsForbidInvalidateDC, true);
}

TreeViewItem::~TreeViewItem()
{
}

ItemsControl* TreeViewItem::GetParentItemsControl()
{
    return ItemsControl::ItemsControlFromItemContainer(this);
}

TreeView* TreeViewItem::GetParentTreeView()
{
    if (_parentTreeView.IsAlive())
    {
        return (TreeView*)_parentTreeView.GetTarget();
    }

    ItemsControl* itemsControl = GetParentItemsControl();
    while (itemsControl != NULL)
    {
        TreeView* pTreeView = RTTICast<TreeView>(itemsControl);
        if (NULL != pTreeView)
        {
            _parentTreeView = pTreeView;
            return pTreeView;
        }
        itemsControl = ItemsControl::ItemsControlFromItemContainer(itemsControl);
    }
    return NULL;
}

Size TreeViewItem::ComputeRenderSize()
{
    Size size;
    Element* elem = GetTemplateChild("PART_Header");

    if (NULL != elem)
    {
        size = elem->ComputeRenderSize();
        if (IsLogicalHorizontal())
        {
            size.cy = GetActualHeight();
        }
        else
        {
            size.cx = GetActualWidth();
        }
    }
    else
    {
        size = HeaderedItemsControl::ComputeRenderSize();
    }

    return size;
}

int TreeViewItem::GetHeaderSize(bool isHorizontal)
{
    return HeaderedItemsControl::GetHeaderSize(isHorizontal);
}

Size TreeViewItem::EstimatedContainerSize(bool isHorizontal)
{
    TreeView* parentTreeView = GetParentTreeView();

    if (parentTreeView != NULL)
    {
        Size size = parentTreeView->EstimatedContainerSize(isHorizontal);
        return size;
    }
    else
    {
        return Size(DEFAULTITEMSIZE, DEFAULTITEMSIZE);
    }
}

TreeViewItem* TreeViewItem::GetParentTreeViewItem()
{
    return RTTICast<TreeViewItem>(GetParentItemsControl());
}

void TreeViewItem::Select(bool selected)
{
    TreeView* parentTreeView = GetParentTreeView();
    ItemsControl* parentItemsControl = GetParentItemsControl();

    if (((parentTreeView != NULL) && (parentItemsControl != NULL)) && !parentTreeView->_isSelChangeActive)
    {
        Object* pItem = GetContainerItem();
        parentTreeView->ChangeSelection(pItem, this, selected, true);
        if ((selected && parentTreeView->IsKeyboardFocusWithin()))
        {
            Focus();
        }
    }
}

void TreeViewItem::SetIsExpandedDynamic(bool val)
{
    _pending |= eTIPending::tipExpand;
    SetIsExpanded(val);
    _pending &= ~eTIPending::tipExpand;
}

void TreeViewItem::UpdateContainsSelection(bool selected)
{
    //for (TreeViewItem* item = GetParentTreeViewItem(); item != NULL; item = item->GetParentTreeViewItem())
    //{
    //    //item->SetContainsSelection(selected);
    //}
}

bool TreeViewItem::IsItemItsOwnContainer(Object* item)
{
    return (item->GetRTTIType() == TreeViewItem::RTTIType());
}

Element* TreeViewItem::GetContainerForItemOverride()
{
    return new TreeViewItem();
}

void TreeViewItem::PrepareContainerForItemOverride(DpObject* elem, ItemEntry* item)
{
    HeaderedItemsControl::PrepareContainerForItemOverride(elem, item);
    SetItemValuesOnContainer(elem, item, NULL);
}

void TreeViewItem::ClearContainerForItemOverride(DpObject* container, ItemEntry* item)
{
    HeaderedItemsControl::ClearContainerForItemOverride(container, item);

    ItemsControl* itemsControl = RTTICast<ItemsControl>(container);
    if (itemsControl != NULL && IsVirtualizing())
    {
        VirtualizingPanel* itemsHost = RTTICast<VirtualizingPanel>(itemsControl->GetItemsHost());
        if (itemsHost != NULL)
        {
            itemsHost->ClearAllContainers(itemsControl);
        }
    }
}

void TreeViewItem::PrepareHeaderedItemsControl(ItemEntry* item, ItemsControl* parentItemsControl)
{
    HeaderedItemsControl::PrepareHeaderedItemsControl(item, parentItemsControl);

    int indent = 0;
    bool val = false;

    TreeViewItem* parentItem = GetParentTreeViewItem();
    TreeView* parentTreeView = GetParentTreeView();
    HierarchicalItem* hiItem = HiItemFromEntry(item);

    _pending |= eTIPending::tipExpand;

    if (NULL != parentItem)
    {
        indent = parentItem->GetIndentValue() + GetIndent();
        if (_indentValue != indent)
        {
            SetValue(IndentValueProperty, new Integer(indent));
        }
    }
    else
    {
        SetValue(IndentValueProperty, Integer::GetZeroInt());
    }

    if (NULL != hiItem)
    {
        SetIsExpandedDynamic(hiItem->GetExpanded());
    }

    if (parentTreeView->_selCachedItem == item->GetItem())
    {
        Select(true);
    }
    else
    {
        SetIsSelected(false);
    }

    _pending &= ~eTIPending::tipExpand;
}

Size TreeViewItem::OnMeasure(const Size& constraint)
{
    Size size = HeaderedItemsControl::OnMeasure(constraint);
    ItemEntry* dItem = GetItemEntry();
    HierarchicalItem* hiItem = HiItemFromEntry(dItem);
    bool bHori = IsLogicalHorizontal();
    int iHeaderSize = GetHeaderSize(bHori);

    if (NULL != hiItem)
    {
        hiItem->SetHeaderSize(iHeaderSize);
    }

    if (IsVirtualizing() && !IsExpanded() && IsVisible())
    {
        TreeView* parentTreeView = GetParentTreeView();
        if (parentTreeView != NULL)
        {
            parentTreeView->RegisterContainerSize(iHeaderSize);
        }
    }

    return size;
}

void TreeViewItem::OnItemsChanged(NotifyCollChangedEventArg* e)
{
    switch (e->GetAction())
    {
    case NotifyCollectionChangedAction::Add:
    case NotifyCollectionChangedAction::Move:
        break;

    case NotifyCollectionChangedAction::Remove:
    case NotifyCollectionChangedAction::Reset:
        return;

    case NotifyCollectionChangedAction::Replace:
        break;
    }
}

void TreeViewItem::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    bool isHori = IsLogicalHorizontal();
    suic::Rect rect(PointToScreen(suic::Point()), GetRenderSize());

    if (isHori)
    {
        rect.right = rect.left + GetHeaderSize(isHori);
    }
    else
    {
        rect.bottom = rect.top + GetHeaderSize(isHori);
    }

    if (!e->IsHandled() && IsEnabled() && rect.PointIn(e->GetMousePoint()))
    {
        _pending |= eTIPending::tipFocused;
        if (Focus())
        {
            e->SetHandled(true);
        }
        _pending &= ~eTIPending::tipFocused;

        bool bExpand = GetValue(ClickExpandProperty)->ToBool();

        if (HasItems() && (bExpand || (e->GetClickCount() % 2) == 0))
        {
            bool bExpand = !IsExpanded();

            SetValue(IsExpandedProperty, BOOLTOBOOLEAN(bExpand));
            e->SetHandled(true);
        }
    }
    
    HeaderedItemsControl::OnMouseLeftButtonDown(e);
}

void TreeViewItem::OnGotFocus(RoutedEventArg* e)
{
    if ((_pending & eTIPending::tipFocused) == eTIPending::tipFocused)
    {
        Select(true);
    }
    Control::OnGotFocus(e);
    e->SetHandled(true);
}

void TreeViewItem::OnCollapsed(RoutedEventArg* e)
{
    RaiseEvent(e);
}

void TreeViewItem::OnExpanded(RoutedEventArg* e)
{
    RaiseEvent(e);
}

}
