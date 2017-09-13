
#include <Extend/Controls/TreeView.h>
#include <Framework/Controls/VisualizeStackPanel.h>

#include <System/Tools/Debug.h>
#include <System/Tools/EventHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>
#include <System/Input/KeyboardNavigation.h>

namespace suic
{

ImplementRTTIOfClass(TreeView, ItemsControl)

UncommonField<XArray>* TreeView::CachedContainerSizeCount;
UncommonField<ContainerSize>* TreeView::CachedEstimateContainerSize;

DpProperty* TreeView::SelectedItemProperty;
RoutedEvent* TreeView::SelectedItemChangedEvent;

TreeView::TreeView()
    : _containerSize(0)
{
    _containerSize.containerCount = 0;
    _selCachedItem = NULL;
    _selContainer = NULL;
    _isSelChangeActive = false;
}

TreeView::~TreeView()
{
    FREEREFOBJ(_selContainer);
}

void TreeView::StaticInit()
{
    if (NULL == SelectedItemProperty)
    {
        TreeViewItem::StaticInit();

        SelectedItemChangedEvent = EventHelper::RegisterRoutedEvent(_U("SelectedItemChanged"), RoutingStrategy::Bubble, RoutedPropChangedEventHandler::RTTIType(), RTTIType());
        SelectedItemProperty = DpProperty::Register(_T("SelectedItem"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, 0, &TreeView::OnSelectedItemPropChanged));

        CachedContainerSizeCount = new UncommonField<XArray>(NULL);
        CachedEstimateContainerSize = new UncommonField<ContainerSize>(NULL);

        KeyboardNavigation::DirectionalNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knContained)));
        KeyboardNavigation::TabNavigationProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knNone)));
    }
}

void TreeView::OnSelectedItemPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    TreeView* pTreeView = RTTICast<TreeView>(d);
    if (NULL != pTreeView)
    {
        RoutedPropChangedEventArg oe(pTreeView->GetSelectedItem(), e->GetNewValue(), SelectedItemChangedEvent);

        oe.OnOverrideOriginalSource(pTreeView);
        pTreeView->_selCachedItem = e->GetNewValue();
        pTreeView->OnSelectedItemChanged(&oe);
    }
}

XArray* TreeView::GetCachedContainerSizeCount()
{
    XArray* arrays = CachedContainerSizeCount->GetValue(this);
    if (arrays == NULL)
    {
        arrays = new XArray();
        CachedContainerSizeCount->SetValue(this, arrays);
    }
    return arrays;
}

bool TreeView::ComputeItemOffset(ItemCollection* items, bool bHori, Object* item, int& iOffset, int& iItemSize)
{
    bool bFind = false;
 
    for (int i = 0; i < items->GetCount(); ++i)
    {
        TreeViewItem* pViewItem = NULL;
        ItemEntry* dItem = items->GetItemEntry(i);
        int iHeaderSize = DEFAULTITEMSIZE;
        HierarchicalItem* hiItem = HiItemFromEntry(dItem);

        pViewItem = RTTICast<TreeViewItem>(dItem->GetItem());

        if (NULL != pViewItem)
        {
            iHeaderSize = pViewItem->GetHeaderSize(bHori);
        }
        else if (dItem->IsCached())
        {
            if (hiItem != NULL)
            {
                iHeaderSize = hiItem->GetHeaderSize();
            }
            else
            {
                iHeaderSize = bHori ? dItem->GetSize().cx : dItem->GetSize().cy;
            }
        }
        else
        {
            Size estimateSize = EstimatedContainerSize(bHori);
            iHeaderSize = bHori ? estimateSize.cx : estimateSize.cy;
        }

        if (dItem->GetItem() == item)
        {
            bFind = true;
            iItemSize = iHeaderSize;
            break;
        }

        bool bExpand = false;
        // 如果有子项
        ItemCollection* itemSrc = NULL;

        if (NULL != pViewItem)
        {
            itemSrc = pViewItem->GetItemsSource();
            bExpand = pViewItem->IsExpanded();
        }
        else if (NULL != hiItem)
        {
            itemSrc = hiItem->GetChildren();
            bExpand = hiItem->GetExpanded();
        }

        iOffset += iHeaderSize;

        if (itemSrc)
        {
            int currOffset = iOffset;

            bFind = ComputeItemOffset(itemSrc, bHori, item, iOffset, iItemSize);

            if (!bFind)
            {
                if (!bExpand) 
                {
                    iOffset = currOffset;
                }
            }
            else
            {
                Size cacheSize = dItem->GetSize();
                if (bHori)
                {
                    cacheSize.cx = iOffset - currOffset + iHeaderSize;
                }
                else
                {
                    cacheSize.cy = iOffset - currOffset + iHeaderSize;
                }
                dItem->SetSize(cacheSize);
                if (NULL != hiItem)
                {
                    hiItem->SetExpanded(true);
                }
                else if (NULL != pViewItem)
                {
                    pViewItem->SetIsExpanded(true);
                }
                break;
            }
        }
    }

    return bFind;
}

void TreeView::RegisterContainerSize(int size)
{
    XArray* arrays = GetCachedContainerSizeCount();

    if (arrays != NULL)
    {
        bool flag = false;
        ContainerSize* newSize = new ContainerSize(size);
        ContainerSize* fitSize = newSize;

        newSize->ref();

        for (int i = 0; i < arrays->GetCount(); i++)
        {
            ContainerSize* temp = (ContainerSize*)(arrays->GetItem(i));
            if (temp->IsCloseTo(size) && (temp->containerCount < Numeric::MeasureInt))
            {
                temp->containerCount++;
                flag = true;
            }
            if (temp->containerCount > fitSize->containerCount)
            {
                fitSize = temp;
            }
        }

        if (!flag && (arrays->GetCount() < 5))
        {
            arrays->Add(newSize);
        }

        _containerSize.size = fitSize->size;
        _containerSize.containerCount = fitSize->containerCount;
        //CachedEstimateContainerSize->SetValue(this, fitSize);

        newSize->unref();
    }
}

Size TreeView::EstimatedContainerSize(bool isHorizontal)
{
    Size size(DEFAULTITEMSIZE, DEFAULTITEMSIZE);

    if (_containerSize.containerCount > 0)
    {
        if (isHorizontal)
        {
            size.cx = _containerSize.size;
        }
        else
        {
            size.cy = _containerSize.size;
        }
    }
    return size;
}

void TreeView::DoCollapsedAndSelection(TreeViewItem* pTreeItem)
{
    if ((_selContainer != NULL) && (_selContainer != pTreeItem) && !pTreeItem->IsExpanded())
    {
        TreeViewItem* parentItem = _selContainer;

        do
        {
            parentItem = parentItem->GetParentTreeViewItem();

            if (parentItem == pTreeItem)
            {
                TreeViewItem* item2 = _selContainer;
                ChangeSelection(pTreeItem->GetContainerItem(), pTreeItem, true, true);
                
                if (item2->IsKeyboardFocusWithin())
                {
                    _selContainer->Focus();
                    return;
                }

                break;
            }
        }
        while (parentItem != NULL);
    }
}

bool TreeView::IsItemItsOwnContainer(Object* item)
{
    return (item->GetRTTIType() == TreeViewItem::RTTIType());
}

Element* TreeView::GetContainerForItemOverride()
{
    return new TreeViewItem();
}

void TreeView::OnPrepareContainer()
{
    FREEREFOBJ(_selContainer);
}

void TreeView::PrepareContainerForItemOverride(DpObject* elem, ItemEntry* item)
{
    ItemsControl::PrepareContainerForItemOverride(elem, item);
    if (IsVirtualizing())
    {
        SetItemValuesOnContainer(elem, item, NULL);
    }
}

void TreeView::ClearContainerForItemOverride(DpObject* container, ItemEntry* item)
{
    ItemsControl::ClearContainerForItemOverride(container, item);

    if (IsVirtualizing())
    {
        ItemsControl* itemsControl = RTTICast<ItemsControl>(container);
        if ((itemsControl != NULL) && itemsControl->IsVirtualizing())
        {
            VirtualizingStackPanel* itemsHost = RTTICast<VirtualizingStackPanel>(itemsControl->GetItemsHost());
            if (itemsHost != NULL)
            {
                itemsHost->ClearAllContainers(itemsControl);
            }
        }
    }
}

void TreeView::ItemSetIsSelected(ItemEntry* item, bool value)
{
    if (IsItemItsOwnContainer(item->GetItem()))
    {
        Element* elem = RTTICast<Element>(item->GetItem());
        elem->SetValue(TreeViewItem::IsSelectedProperty, BOOLTOBOOLEAN(value));
    }

    if (NULL != item)
    {
        SetSelectedItem(item->GetItem());
    }
    else
    {
        SetSelectedItem(item);
    }
}

void TreeView::SetItemIsExpanded(Object* item, bool val)
{
    ItemEntry* dItem = NULL;
    TreeViewItem* pItem = RTTICast<TreeViewItem>(item);

    if (NULL != pItem)
    {
        pItem->SetIsExpanded(val);
        dItem = pItem->GetItemEntry();
    }
    else
    {
        dItem = ItemEntryFromItem(item);
    }

    if (dItem != NULL)
    {
        HierarchicalItem* hiItem = HiItemFromEntry(dItem);
        if (NULL != hiItem)
        {
            hiItem->SetExpanded(val);
        }
        UpdateLayout();
    }
}

int TreeView::ScrollIntoView(Object* item, bool atTopOfViewport)
{
    if (NULL == item)
    {
        return -1;
    }

    int iItemSize = 0;
    int iOffset = 0;
    int iTimes = 0;
    bool bHori = IsLogicalHorizontal();
    ScrollViewer* scrollHost = GetScrollHost();
    bool find = ComputeItemOffset(GetItemsSource(), bHori, item, iOffset, iItemSize);
    int iItemOffset = iOffset + iItemSize;

    if (NULL == scrollHost)
    {
        return -1;
    }

    if (bHori)
    {
        int iStart = scrollHost->GetHorizontalOffset();
        if (iItemOffset <= (iStart + scrollHost->GetViewportWidth()) && 
            iOffset >= iStart)
        {
            return -2;
        }
    }
    else
    {
        int iStart = scrollHost->GetVerticalOffset();
        if (iItemOffset <= (iStart + scrollHost->GetViewportHeight()) && 
            iOffset >= iStart)
        {
            return -1;
        }
    }

    while (find && MakeVisible(item, iOffset, iItemSize, atTopOfViewport))
    {
        int extentWidth = scrollHost->GetExtentWidth();
        int extentHeight = scrollHost->GetExtentHeight();

        scrollHost->InvalidateMeasure();
        scrollHost->UpdateArrange();

        if (IsLogicalVertical())
        {
            if (extentHeight == scrollHost->GetExtentHeight())
            {
                break;
            }
        }
        else if (extentWidth == scrollHost->GetExtentWidth())
        {
            break;
        }

        if (iTimes > 3) {
            break;
        }

        ++iTimes;
        iItemSize = 0;
        iOffset = 0;
        find = ComputeItemOffset(GetItemsSource(), bHori, item, iOffset, iItemSize);
    }

    return -1;
}

void TreeView::ScrollToEdge(bool bEnd)
{
    ScrollViewer* scollHost = GetScrollHost();

    if (NULL != scollHost)
    {
        int iOffset = 0;
        int extentWidth = 0;
        int extentHeight = 0;

        do
        {
            extentWidth = scollHost->GetExtentWidth();
            extentHeight = scollHost->GetExtentHeight();

            if (IsLogicalHorizontal())
            {
                if (bEnd)
                {
                    iOffset = scollHost->GetExtentWidth();
                }
                scollHost->ScrollToHorizontalOffset(iOffset);
            }
            else if (IsLogicalVertical())
            {
                if (bEnd)
                {
                    iOffset = scollHost->GetExtentHeight();
                }
                scollHost->ScrollToVerticalOffset(iOffset);
            }
        }
        while (extentWidth != scollHost->GetExtentWidth() || 
            extentHeight != scollHost->GetExtentHeight());
    }
}

void TreeView::AddSelectedItemChanged(RoutedPropChangedEventHandler* handler)
{
    AddHandler(SelectedItemChangedEvent, handler, false);
}

void TreeView::RemoveSelectedItemChanged(RoutedPropChangedEventHandler* handler)
{
    RemoveHandler(SelectedItemChangedEvent, handler);
}

Element* TreeView::GetHeaderContainerFromItem(Object* item)
{
    TreeViewItem* pItem = RTTICast<TreeViewItem>(GetContainerFromItem(item));

    if (NULL != pItem)
    {
        return pItem->GetTemplateChild(_T("PART_Header"));
    }
    else
    {
        return NULL;
    }
}

void TreeView::SetSelectedItem(Object* selItem)
{
    Object* item = GetSelectedItem();
    if (item != selItem)
    {
        SetValue(SelectedItemProperty, selItem);
    }
}

Object* TreeView::GetSelectedItem()
{
    return _selCachedItem;
}

void TreeView::UpdateSelectedValue(Object* selectedItem)
{
}

void TreeView::HandleMouseButtonDown(TreeViewItem* item)
{
    if (!IsKeyboardFocusWithin())
    {
        if (item != NULL)
        {
            item->Focus();
        }
        else
        {
            Focus();
        }
    }
}

void TreeView::ChangeSelection(Object* selItem, TreeViewItem* container, bool selected, bool bNeedSet)
{
    if (!_isSelChangeActive/* && !container->HasItems()*/)
    {
        Object* oldValue = NULL;
        Object* newValue = NULL;
        bool flag = false;
        TreeViewItem* oldTreeItem = NULL;
        
        oldTreeItem = _selContainer;
        _isSelChangeActive = true;

        oldValue = GetSelectedItem();

        if (selected)
        {
            if (container != _selContainer)
            {
                newValue = selItem;

                if (oldTreeItem != NULL)
                {
                    oldTreeItem->SetIsSelected(false);
                    oldTreeItem->UpdateContainsSelection(false);
                }

                if (oldTreeItem != NULL)
                {
                    oldTreeItem->SetIsSelected(false);
                }

                SETREFOBJ(_selContainer, container);
                SetSelectedItem(selItem);

                if (NULL != _selContainer && bNeedSet)
                {
                    _selContainer->SetIsSelected(selected);
                }

                container->UpdateContainsSelection(true);
                
                flag = true;
            }
        }
        else if (_selContainer == container && bNeedSet)
        {
            _selContainer->SetIsSelected(false);
            FREEREFOBJ(_selContainer);
        }

        _isSelChangeActive = false;
    }
}

void TreeView::OnSelectedItemChanged(RoutedPropChangedEventArg* e)
{
    RaiseEvent(e);
}

void TreeView::OnItemsChanged(NotifyCollChangedEventArg* e)
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

ItemEntry* TreeView::FindPrevLastItem(ItemEntry* pItem)
{
    bool bExpanded = false;
    ItemCollection* itemColl = NULL;
    TreeViewItem* pViewItem = NULL;
    HierarchicalItem* hiItem = NULL;

    hiItem = RTTICast<HierarchicalItem>(pItem->GetItem());
    if (NULL == hiItem)
    {
        pViewItem = RTTICast<TreeViewItem>(pItem->GetItem());
        if (NULL != pViewItem)
        {
            bExpanded = pViewItem->IsExpanded();
            itemColl = pViewItem->GetItemsSource();
        }
    }
    else
    {
        bExpanded = hiItem->GetExpanded();
        itemColl = hiItem->GetChildren();
    }

    if (!bExpanded || NULL == itemColl || itemColl->GetCount() == 0)
    {
        return pItem;
    }
    else
    {
        return FindPrevLastItem(itemColl->GetItemEntry(itemColl->GetCount() - 1));
    }
}

ItemEntry* TreeView::FindPrevItem(TreeViewItem* pItem)
{
    ItemsControl* pParent = this;
    int index = 0;
    TreeViewItem* pParentItem = pItem->GetParentTreeViewItem();

    if (NULL != pParentItem)
    {
        pParent = pParentItem;
    }

    index = pParent->GetItemsSource()->IndexOf(pItem->GetContainerItem());

    if (index <= 0)
    {
        if (NULL == pParentItem)
        {
            return NULL;
        }

        return pParentItem->GetItemEntry();
    }
    else
    {
        ItemEntry* itemEntry = NULL;
        itemEntry = pParent->GetItemsSource()->GetItemEntry(index - 1);
        return FindPrevLastItem(itemEntry);
    }
}

ItemEntry* TreeView::FindNextItem(TreeViewItem* pItem, bool bFirst)
{
    if (bFirst && pItem->IsExpanded() && pItem->GetCount() > 0)
    {
        return pItem->ItemEntryFromIndex(0);
    }
    else
    {
        int index = 0;
        ItemsControl* pParent = this;
        TreeViewItem* pParentItem = pItem->GetParentTreeViewItem();

        if (NULL != pParentItem)
        {
            pParent = pParentItem;
        }

        index = pParent->GetItemsSource()->IndexOf(pItem->GetContainerItem());

        if (index >= 0 && index < pParent->GetCount() - 1)
        {
            return pParent->ItemEntryFromIndex(index + 1);
        }
        else if (pParentItem != NULL)
        {
            return FindNextItem(pParentItem, false);
        }
        else
        {
            return NULL;
        }
    }
}

void TreeView::ScrollToItem(ItemEntry* pItem, bool bUp)
{
    if (pItem != NULL)
    {
        /*Element* itemHead = NULL;
        TreeViewItem* itemElem = RTTICast<TreeViewItem>(GetContainerFromItem(pItem->GetItem()));

        if (NULL != itemElem)
        {
            itemHead = itemElem->GetHeaderElement();
        }*/

        //if (!IsOnCurrentPage(itemHead, AxisDirection::xyAxis, true))
        {
            ScrollIntoView(pItem->GetItem(), bUp);
        }
        SetSelectedItem(pItem->GetItem());
    }
}

void TreeView::OnKeyDown(suic::KeyboardEventArg* e)
{
    ItemsControl::OnKeyDown(e);

    ScrollViewer* scrollView = GetScrollHost();

    if (NULL == scrollView || GetCount() == 0)
    {
        return;
    }

    KeyboardNavigation::Current()->HideFocusVisual();

    switch (e->GetKey())
    {
     case Key::kSpace:
        /*if (e->GetKey() == Key::kSpace)
        {
            if (GetFocusedItem())
            {
                SetSelectedItem(GetFocusedItem());
                e->SetHandled(true);
                UpdateLayout();
            }
            return;
        }*/
        break;
    case Key::kLeft:
        if (_selContainer != NULL)
        {
            _selContainer->SetIsExpanded(false);
        }
        break;

    case Key::kRight:
        if (_selContainer != NULL)
        {
            _selContainer->SetIsExpanded(true);
        }
        break;

    case Key::kUp:
    case Key::kDown:
        if (NULL == _selContainer)
        {
            ItemEntry* pSelItem = ItemEntryFromItem(GetSelectedItem());
            if (pSelItem)
            {
                ScrollToItem(pSelItem, e->GetKey() == Key::kUp);
            }
            else
            {
                ScrollToItem(ItemEntryFromIndex(0), true);
            }
        }
        else
        {
            ItemEntry* prevItem = NULL;
            if (e->GetKey() == Key::kUp)
            {
                //
                // 找到上一个元素
                //
                prevItem = FindPrevItem(_selContainer);
            }
            else
            {
                //
                // 找到上一个元素
                //
                prevItem = FindNextItem(_selContainer, true);
            }

            ScrollToItem(prevItem, e->GetKey() == Key::kUp);
        }
        break;

    case Key::kPrior:
        scrollView->PageUp();
        break;

    case Key::kNext:
        scrollView->PageDown();
        break;

    case Key::kHome:
        scrollView->ScrollToHome(AxisDirection::yAxis);
        break;

    case Key::kEnd:
        scrollView->ScrollToEnd(AxisDirection::yAxis);
        break;

    default:
        return;
    }

    UpdateLayout();
    e->SetHandled(true);
}

void TreeView::OnGotFocus(RoutedEventArg* e)
{
    Control::OnGotFocus(e);
    if (IsKeyboardFocusWithin() && 
        ((_selContainer != NULL) && !_selContainer->IsKeyboardFocusWithin()))
    {
        _selContainer->Focus();
    }
}

void TreeView::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    e->SetHandled(true);
    ItemsControl::OnMouseLeftButtonDown(e);
}

}
