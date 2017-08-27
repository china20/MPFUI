
#include <Framework/Presenter/ItemContainerGenerator.h>
#include <Framework/Controls/Panel.h>
#include <Framework/Controls/ItemsControl.h>

namespace suic
{

ItemContainerGenerator::ItemContainerGenerator()
    : _current(0)
    , _level(0)
    , _items(0)
    , _peer(NULL)
    , _parent(NULL)
    , _groupStyle(NULL)
    , _host(NULL)
    , _itemHost(NULL)
{

}

ItemContainerGenerator::ItemContainerGenerator(ItemsControl* host)
    : _current(0)
    , _endIndex(0)
    , _level(0)
    , _items(0)
    , _peer(host)
    , _parent(NULL)
    , _groupStyle(NULL)
    , _host(host)
    , _itemHost(NULL)
{
    OnRefresh();
    _recyclableContainers.Resize(8);
}

ItemContainerGenerator::ItemContainerGenerator(ItemContainerGenerator* parent, GroupItem* groupItem)
    : _current(0)
    , _endIndex(0)
    , _level(0)
    , _items(0)
    , _peer(groupItem)
    , _parent(parent)
    , _groupStyle(NULL)
    , _host(parent->GetHost())
    , _itemHost(NULL)
{
    _level = parent->GetLevel() + 1;
    OnRefresh();
    _recyclableContainers.Resize(8);
}

ItemContainerGenerator::ItemContainerGenerator(ItemContainerGenerator* parent, ItemsControl* host, DpObject* peer, int level)
    : _current(0)
    , _endIndex(0)
    , _level(level)
    , _parent(parent)
    , _items(0)
    , _peer(peer)
    , _groupStyle(GroupStyle::Default())
    , _host(host)
{
    OnRefresh();
    _recyclableContainers.Resize(8);
}

ItemContainerGenerator::~ItemContainerGenerator()
{
    if (NULL != _items)
    {
        _items->unref();
    }
}

DpObject* ItemContainerGenerator::GetPeer() const
{
    return _peer;
}

ItemsControl* ItemContainerGenerator::GetHost() const
{
    return _host;
}

ItemContainerGenerator* ItemContainerGenerator::GetParent() const
{
    return _parent;
}

Element* ItemContainerGenerator::ContainerFromIndex(int index)
{
    if (NULL != GetHost() && !IsGrouping())
    {
        Object* item = GetHost()->GetItem(index);
        return GetHost()->GetContainerFromItem(item);
    }
    else
    {
        return NULL;
    }
}

Element* ItemContainerGenerator::ContainerFromItem(Object* item)
{
    if (NULL != GetHost() && GetHost()->IsItemItsOwnContainer(item))
    {
        return RTTICast<Element>(item);
    }

    if (NULL != _itemHost)
    {
        for (int i = 0; i < _itemHost->GetVisibleChildrenCount(); ++i)
        {
            Element* elem = _itemHost->GetVisibleChild(i);

            if (elem->GetContainerItem() == item)
            {
                return elem;
            }
            else
            {
                ItemsControl* pItem = RTTICast<ItemsControl>(elem);
                if (NULL != pItem)
                {
                    if (NULL != pItem->GetItemContainerGenerator())
                    {
                        elem = pItem->GetItemContainerGenerator()->ContainerFromItem(item);
                        if (NULL != elem)
                        {
                            return elem;
                        }
                    }
                }
                else
                {
                    GroupItem* groupItem = RTTICast<GroupItem>(elem);
                    if (NULL != groupItem && NULL != groupItem->GetGenerator())
                    {
                        elem = groupItem->GetGenerator()->ContainerFromItem(item);
                        if (NULL != elem)
                        {
                            return elem;
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

bool ItemContainerGenerator::StartAt(int iStart, int iEnd)
{
    ItemCollection* items = GetItems();
    if (NULL == items)
    {
        return false;
    }

    if (iStart >= 0)
    {
        _current = iStart;
        _endIndex = iEnd;
        if (_endIndex < 0 || _endIndex >= items->GetCount())
        {
            _endIndex = items->GetCount() - 1;
        }
        if (_current <= _endIndex)
        {
            _current -= 1;
            return true;
        }
    }
    return false;
}

int ItemContainerGenerator::GetPos() const
{
    return _current;
}

bool ItemContainerGenerator::HasNext()
{
    ++_current;
    if (_current <= _endIndex)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//bool ItemContainerGenerator::HasNext(int iEndPos)
//{
//    ItemCollection* enumer = _itemsControl->GetItemsSource();
//    if (NULL == enumer)
//    {
//        return false;
//    }
//    if (iEndPos < 0 || iEndPos > enumer->GetCount())
//    {
//        iEndPos = enumer->GetCount();
//    }
//    ++_current;
//    return (_current < iEndPos);
//}

void ItemContainerGenerator::LinkContainerToItem(Element* container, ItemEntry* item)
{
    LinkContainerToItem(container, item, false);
}

void ItemContainerGenerator::LinkContainerToItem(Element* d, ItemEntry* item, bool isRecycled)
{
    d->SetItemEntry(item);
    //d->SetItemIndex(_current);
}

GroupStyle* ItemContainerGenerator::GetGroupStyle() const
{
    return _groupStyle;
}

ItemEntry* ItemContainerGenerator::GetItem()
{
    ItemCollection* items = GetItems();
    if (NULL == items)
    {
        return NULL;
    }
    else
    {
        return items->GetItemEntry(_current);
    }
}

Element* ItemContainerGenerator::Dequeue()
{
    ItemContainerGenerator* generator = GetHost()->GetItemContainerGenerator();
    if (NULL != generator)
    {
        return generator->DequeueOverride();
    }
    return NULL;
}

void ItemContainerGenerator::Recycle(Element* container)
{
    ItemContainerGenerator* generator = GetHost()->GetItemContainerGenerator();
    generator->RecycleOverride(container);
}

Element* ItemContainerGenerator::DequeueOverride()
{
    int index = _recyclableContainers.GetCount() - 1;
    Element* container = _recyclableContainers[index];
    _recyclableContainers.RemoveAt(index);
    return container;
}

void ItemContainerGenerator::RecycleOverride(Element* container)
{
    container->ref();
    _recyclableContainers.Add(container);
    ItemsControl::ClearBeforeLinkContainer(container);
}

Element* ItemContainerGenerator::Generate()
{
    bool isNeedRecycle = false;
    return Generate(isNeedRecycle);
}

Element* ItemContainerGenerator::GenerateGroup(ViewGroup* vGroup)
{
    Element* container = NULL;
    ItemCollection* items = GetItems();
    ItemEntry* itemEntry = items->GetItemEntry(_current);

    // 创建分组对象
    if (NULL != vGroup)
    {
        container = ContainerForGroup(itemEntry, vGroup);
    }

    if (container)
    {
        LinkContainerToItem(container, itemEntry, false);
    }

    return container;
}

Element* ItemContainerGenerator::Generate(bool& isNeedRecycle)
{
    Element* container = NULL;

    ItemCollection* items = GetItems();
    ItemEntry* itemEntry = items->GetItemEntry(_current);
    ViewGroup* vGroup = RTTICast<ViewGroup>(itemEntry->GetItem());

    isNeedRecycle = false;

    // 创建分组对象
    if (NULL != vGroup)
    {
        container = ContainerForGroup(itemEntry, vGroup);
    }
    else if (GetHost()->IsItemItsOwnContainer(itemEntry->GetItem()))
    {
        container = RTTICast<Element>(itemEntry->GetItem());
        container->ref();
    }
    else
    {
        if (_recyclableContainers.GetCount() > 0)
        {
            container = Dequeue();
        }
        else
        {
            container = _host->GetContainerForItem(itemEntry->GetItem());
            container->WriteDoFlag(ViewFlags::IsContainerItemVF, true);
        }
        isNeedRecycle = true;
    }

    if (container)
    {
        LinkContainerToItem(container, itemEntry, isNeedRecycle);
    }

    return container;
}

Element* ItemContainerGenerator::GenerateContainer(bool& isNeedRecycle)
{
    Element* container = NULL;
    ItemCollection* items = GetItems();
    ItemEntry* itemEntry = items->GetItemEntry(_current);

    isNeedRecycle = false;

    if (_recyclableContainers.GetCount() > 0)
    {
        container = Dequeue();
    }
    else
    {
        isNeedRecycle = true;
        container = _host->GetContainerForItem(itemEntry->GetItem());
        container->WriteDoFlag(ViewFlags::IsContainerItemVF, true);
    }

    if (container)
    {
        LinkContainerToItem(container, itemEntry, isNeedRecycle);
    }

    return container;
}

void ItemContainerGenerator::PrepareItemContainer(Element* container)
{
    if (_host != NULL)
    {
        ItemEntry* item = container->GetItemEntry();
        if (container != item->GetItem())
        {
            container->SetDataContext(item->GetItem());
        }
        _host->PrepareItemContainer(container, item);
    }
}

void ItemContainerGenerator::PrepareItemContainer(Element* container, ItemEntry* item)
{
    if (_host != NULL)
    {
        if (container != item->GetItem())
        {
            container->SetDataContext(item->GetItem());
        }
        _host->PrepareItemContainer(container, item);
    }
}

bool ItemContainerGenerator::IsGrouping() const
{ 
    return (GetItems() != GetHost()->GetItemsSource()); 
}

void ItemContainerGenerator::PrepareGrouping()
{
    GroupStyle* groupStyle = NULL; 
    ItemCollection* items = NULL;

    if (GetLevel() == 0) 
    {
        groupStyle = GetHost()->GetGroupStyle(NULL, 0);
        items = GetHost()->GetItemsSource();
    } 
    else 
    {
        GroupItem* groupItem = RTTICast<GroupItem>(GetPeer());
        ViewGroup* group = RTTICast<ViewGroup>(groupItem->GetContainerItem());

        if (group != NULL)
        { 
            if (group->IsBottomLevel())
            { 
                groupStyle = NULL; 
            }
            else 
            {
                groupStyle = GetHost()->GetGroupStyle(group, GetLevel());
            }

            items = group->GetItems();
        } 
        else 
        {
            groupStyle = NULL; 
            items = GetHost()->GetItemsSource();
        }
    }

    _groupStyle = groupStyle;
    NotifyItemCollection(items);

    if ((GetLevel() == 0) && (GetHost() != NULL))
    {
        GetHost()->SetIsGrouping(IsGrouping());
    }
}

ItemContainerGenerator* ItemContainerGenerator::GetItemContainerGeneratorForPanel(Panel* panel)
{
    if (!panel->IsItemsHost())
    {
        throw ArgumentException(_U("PanelIsNotItemsHost"), __FILELINE__);
    }

    ItemsPresenter* presenter = ItemsPresenter::FromPanel(panel);
    if (presenter != NULL)
    {
        return presenter->GetGenerator();
    }

    if (panel->GetTemplatedParent() != NULL)
    {
        return this;
    }
    else
    {
        return NULL;
    }
}

bool ItemContainerGenerator::ShouldHide(ViewGroup* group) const
{
    return false;
    return (GetGroupStyle()->HidesIfEmpty() && group->GetItemCount() == 0);
}

Element* ItemContainerGenerator::ContainerForGroup(ItemEntry* itemEntry, ViewGroup* group)
{
    if (!ShouldHide(group))
    {
        GroupItem* container = new GroupItem();
        container->ref();
        container->SetGenerator(new ItemContainerGenerator(this, container));
        return container;
    }
    return NULL;
}

void ItemContainerGenerator::NotifyItemCollection(ItemCollection* newColl)
{
    if (_items != newColl)
    {
        SetItems(newColl);
    }
}

void ItemContainerGenerator::OnItemCollectionChanged(Object* sender,  NotifyCollChangedEventArg* e)
{
    ItemsChangedEventArg ice(e->GetAction(), e->GetOldIndex(), e->GetNewIndex(), 1);
    if (ItemsChanged.safe_bool())
    {
        ItemsChanged(sender, &ice);
    }
}

void ItemContainerGenerator::NotifyItemHost(Panel* newPanel)
{
    _itemHost = newPanel;
}

void ItemContainerGenerator::SetItems(ItemCollection* items)
{
    NotifyCollChangedEventHandler e(this, &ItemContainerGenerator::OnItemCollectionChanged);
    if (_items != NULL)
    {
        _items->CollectionChanged -= e;
        _items->unref();
    }
    _items = items;
    if (NULL != _items)
    {
        _items->ref();
        _items->CollectionChanged += e;
    }
}

void ItemContainerGenerator::OnPanelChanged()
{
    if (PanelChanged.safe_bool())
    {
        PanelChanged(this, &EventArg::Empty);
    }
}

void ItemContainerGenerator::AddEmptyGroupItem(ViewGroup* group)
{
    EmptyGroupItem* container = new EmptyGroupItem();
    ItemEntry* itemEntry = new ItemEntry(container);

    _emptyGroupItems.Add(itemEntry);
    LinkContainerToItem(container, itemEntry);
    container->SetGenerator(new ItemContainerGenerator(this, container));
}

void ItemContainerGenerator::Dispose()
{
    for (int i = 0; i < _recyclableContainers.GetCount(); ++i)
    {
        _recyclableContainers[i]->unref();
    }
    _recyclableContainers.Reset();
}

void ItemContainerGenerator::RemoveAll()
{
    PrepareGrouping();
}

void ItemContainerGenerator::Refresh()
{
    OnRefresh();
}

void ItemContainerGenerator::OnRefresh()
{
    RemoveAll();
    ItemsChangedEventArg ice(NotifyCollectionChangedAction::Reset, 0, 0, 0);
    if (ItemsChanged.safe_bool())
    {
        ItemsChanged(this, &ice);
    }
}

}
