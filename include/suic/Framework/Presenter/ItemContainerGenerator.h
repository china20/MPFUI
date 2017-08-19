// 华勤科技版权所有 2010-2011
// 
// 文件名：ItemContainerGenerator.h
// 功  能：实现Item的生成
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIITEMCONTAINERGENERATOR_H_
#define _UIITEMCONTAINERGENERATOR_H_

#include <System/Tools/ArrayList.h>

#include <System/Windows/Object.h>
#include <System/Windows/ItemsChangedEventArg.h>

#include <Framework/Controls/ViewGroup.h>
#include <Framework/Controls/ItemSource.h>
#include <Framework/Presenter/ItemsPresenter.h>

namespace suic
{

class Element;
class ItemsControl;

/// <summary>
/// ItemContainerGenerator,所有项的基类。
/// </summary>
class SUICORE_API ItemContainerGenerator : public DpObject
{
public:

    EventHandler PanelChanged;
    ItemsChangedEventHandler ItemsChanged;

    ItemContainerGenerator();
    ItemContainerGenerator(ItemsControl* parent);
    ItemContainerGenerator(ItemContainerGenerator* parent, GroupItem* groupItem);
    ItemContainerGenerator(ItemContainerGenerator* parent, ItemsControl* host, DpObject* peer, int level);

    virtual ~ItemContainerGenerator();

    void Dispose();
    void RemoveAll();

    int GetLevel() const;
    bool IsGrouping() const;

    bool StartAt(int iStart, int iEnd);
    int GetPos() const;
    bool HasNext();

    DpObject* GetPeer() const;
    ItemsControl* GetHost() const;
    ItemContainerGenerator* GetParent() const;

    ItemCollection* GetItems() const;
    
    ItemEntry* GetItem();
    GroupStyle* GetGroupStyle() const;

    Element* Generate();
    Element* Generate(bool& isNeedRecycle);
    Element* GenerateGroup(ViewGroup* vGroup);
    Element* GenerateContainer(bool& isNeedRecycle);

    Element* ContainerFromIndex(int index);
    Element* ContainerFromItem(Object* item);

    void Recycle(Element* container);
    
    void PrepareItemContainer(Element* elem);
    void PrepareItemContainer(Element* elem, ItemEntry* item);

    ItemContainerGenerator* GetItemContainerGeneratorForPanel(Panel* panel);

    void LinkContainerToItem(Element* container, ItemEntry* item);
    void LinkContainerToItem(Element* d, ItemEntry* item, bool isRecycled);

protected:

    void Refresh();
    void OnRefresh();
    void PrepareGrouping();
    void AddEmptyGroupItem(ViewGroup* group);
    bool ShouldHide(ViewGroup* group) const;
    Element* ContainerForGroup(ItemEntry* itemEntry, ViewGroup* group);

    Element* Dequeue();

    Element* DequeueOverride();
    void RecycleOverride(Element* container);

    void SetItems(ItemCollection* items);
    void OnPanelChanged();

    void NotifyItemHost(Panel* newPanel);
    void NotifyItemCollection(ItemCollection* newColl);

    void OnItemCollectionChanged(Object* sender,  NotifyCollChangedEventArg* e);

    friend class Panel;
    friend class ItemsControl;
    friend class ItemsPresenter;
    
private:

    int _level;
    int _current;
    int _endIndex;

    DpObject* _peer;
    GroupStyle* _groupStyle;
    
    Panel* _itemHost;
    ItemsControl* _host;
    ItemCollection* _items;
    ItemContainerGenerator* _parent;

    ArrayList _emptyGroupItems;
    Array<Element*> _recyclableContainers;
};

inline int ItemContainerGenerator::GetLevel() const
{
    return _level;
}

inline ItemCollection* ItemContainerGenerator::GetItems() const
{
    return _items;
}

}

#endif
