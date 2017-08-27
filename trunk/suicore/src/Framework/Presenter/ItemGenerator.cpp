//////////////////////////////////////////////////////////////////////////////
// uiitemgenerator.cpp

#include <System/Tools/Debug.h>
#include <Framework/Controls/ItemsElement.h>
#include <Framework/Presenter/ItemGenerator.h>

namespace suic
{

ImplementTypeofClass(ItemGenerator, RefBase)

ItemGenerator::ItemGenerator(ItemsElement* owner)
    : _currPos(0)
    , _itemsOwner(owner)
    , _bCache(false)
{
}

ItemGenerator::~ItemGenerator()
{
}

ItemsList* ItemGenerator::GetItemsList()
{
    return _itemsOwner->GetItems();
}

Element* ItemGenerator::GetNext()
{
    if (_currPos < _itemsOwner->GetCount())
    {
        RefBase* item(_itemsOwner->GetItem(_currPos));
        Element* elem(ContainerFromItem(item, _currPos));

        _currPos++;
        return elem;
    }
    else
    {
        return NULL;
    }
}

RefBase* ItemGenerator::GetItem()
{
    return _itemsOwner->GetItem(_currPos - 1);
}

Size ItemGenerator::Estimate(RefBase* item)
{
    return Size(20,20);
}

Element* ItemGenerator::ContainerFromIndex(int pos)
{
    RefBase* item(GetItemsList()->GetItem(pos));
    Element* elem = ContainerFromItem(item, pos);
    elem->SetItemIndex(pos);
    return elem;
}

void ItemGenerator::CacheRecycElement(Element* elem)
{
    if (elem->IsRecycCacheElement())
    {
        _recycCache.push_back(elem);
        elem->__addRef();
        /*if (elem->IsFocused() && !_itemsOwner->GetFocusedItem())
        {
            _itemsOwner->SetFocus();
        }*/

        //
        // 通知元素已经被循环利用
        //
        elem->OnOffVisualView(_itemsOwner);
    }
    else
    {
        Debug::Assert(false, _U("not a recyc element"));
    }
}

Element* ItemGenerator::ContainerFromItem(RefBase* item, int index)
{
    Element* container = NULL;

    // 子项本身不是容器
    if (!_itemsOwner->ItemIsContainer(item))
    {
        if (_bCache && _recycCache.size() > 0)
        {
            //
            // 为了提高访问效率，从后面取缓存的元素
            //
            int iSize = _recycCache.size() - 1;
            container = _recycCache[iSize];
            bool autodel = container->__setAutoDelete(false);

            //
            // 移除最后一个元素
            //
            _recycCache.erase(_recycCache.begin() + iSize);

            //
            // 把子项关联到对应界面元素
            //
            container->SetDataContext(item);
            container->OnInVisualView(_itemsOwner);

            container->__release();
            container->__setAutoDelete(autodel);
        }
        else
        {
            //
            // 取得子项的容器对象，如果子项本身就是容器对象
            // 则直接返回
            //
            container = _itemsOwner->GetContainer();
            //
            // 把子项关联到对应界面元素
            //
            container->SetDataContext(item);
            container->OnInVisualView(_itemsOwner);
            container->WriteViewFlag(ViewFlags::IsRecycCacheElement, true);
        }
    }
    else
    {
        container = CastElem<Element>(item);
        container->OnInVisualView(_itemsOwner);
        container->WriteViewFlag(ViewFlags::IsRecycCacheElement, false);
    }

    return container;
}

}
