
#include <Framework/Controls/HierarchicalItem.h>

namespace suic
{

ImplementRTTIOfClass(HierarchicalItem, NotifyPropChanged)

HierarchicalItem::HierarchicalItem()
    : _size(0)
    , _flag(0)
    , _children(NULL)
{
}

HierarchicalItem::~HierarchicalItem()
{
    if (NULL != _children)
    {
        _children->unref();
    }
}

void HierarchicalItem::OnSetExpanded(bool val)
{
    SetExpandedFlag(val);
}

ItemCollection* HierarchicalItem::GetChildren()
{
    if (NULL == _children)
    {
        _children = new ObservableCollection();
        _children->ref();
        _children->CollectionChanged += NotifyCollChangedEventHandler(this, &HierarchicalItem::OnItemsChanged);
    }
    return _children;
}

void HierarchicalItem::HandleItemRemoved()
{

}

void HierarchicalItem::OnItemsChanged(NotifyCollChangedEventArg* e)
{

}

void HierarchicalItem::OnItemsChanged(Object* sender, NotifyCollChangedEventArg* e)
{
    switch (e->GetAction())
    {
    case NotifyCollectionChangedAction::Reset:
    case NotifyCollectionChangedAction::Remove:
        HandleItemRemoved();
        break;

    case NotifyCollectionChangedAction::Add:
    case NotifyCollectionChangedAction::Insert:
    case NotifyCollectionChangedAction::Replace:
        {
            HierarchicalItem* pItem = RTTICast<HierarchicalItem>(e->GetNewItem());
            if (NULL != pItem)
            {
                pItem->_parent = this;
            }
        }
        break;
    }

    OnItemsChanged(e);
}

}
