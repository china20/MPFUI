// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ItemContainerGeneratorEx.h
// ��  �ܣ�ItemsControl�������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UIITEMCONTAINERGENERATOREX_H_
#define _UIITEMCONTAINERGENERATOREX_H_

#include <Framework/Items/GroupItem.h>
#include <Framework/Items/Enumerable.h>
#include <Framework/Items/IGeneratorHost.h>
#include <Framework/Items/CollectionViewGroup.h>
#include <Framework/Controls/ItemCollection.h>
#include <Framework/Controls/ItemNavigateArg.h>

namespace suic
{

struct ItemEntry
{
	Size size;
	DpObject* container;

    ItemEntry()
    {
        container = NULL;
    }
    ItemEntry(DpObject* c)
    {
        container = c;
    }
};

enum GeneratorDirection
{
	Forward,
	Backward,
};

struct GeneratorPosition
{
    int index;
    int offset;

    GeneratorPosition(int i, int o)
    {
        index = i;
        offset = o;
    }
};

// ������״̬
struct GeneratorState
{
    // �������
    int index;

    GeneratorState()
    {
        index = 0;
    }

    GeneratorState(int i)
    {
        index = i;
    }
};

class ItemContainerGeneratorEx : public DpObject
{
public:

    EventHandler PanelChanged;
    ItemsChangedEventHandler ItemsChanged;

    static DpProperty* ItemForItemContainerProperty;

    static void LinkContainerToItem(DpObject* container, Object* item, bool isRecycled);

	ItemContainerGeneratorEx(IGeneratorHost* host);
    ItemContainerGeneratorEx(ItemContainerGeneratorEx* parent, GroupItem* groupItem);
    ItemContainerGeneratorEx(ItemContainerGeneratorEx* parent, IGeneratorHost* host, DpObject* peer, int level);

	void InitData(DpObject* peer, int level);

	int GetLevel()
	{
		return _level;
	}

	// ��ȡ�����
	int GetCount()
	{
		return GetItems()->GetCount();
	}

	int GetAlternationCount() const
	{
		return _alternationCount;
	}

	Object* GetItem(int index)
	{
		return GetItems()->GetItem(index);
	}

	IGeneratorHost* GetHost() const
	{
		return _host;
	}

    void SetHost(IGeneratorHost* gHost)
    {
        _host = gHost;
        _items = _host->GetView();
    }

    bool IsGrouping()
    {
        return true;
    }

    IList* GetItems() const
    {
        return _items;
    }

    void SetItems(IList* val);

	DpObject* GetPeer() const
	{
		return _peer;
	}

	GroupStyle* GetGroupStyle() const
	{
		return _groupStyle;
	}

	ItemContainerGeneratorEx* GetParent() const
	{
		return _parent;
	}

	GeneratorState* GetCacheState()
	{
		return &_cacheState;
	}

	void Recycle();
    void Refresh();

    ItemEntry* GetItemEntry(int index);

    Object* ItemFromContainer(DpObject* container);
    int IndexFromContainer(DpObject* container);
    DpObject* ContainerFromItem(Object* item);
    DpObject* ContainerFromIndex(int index);

	DpObject* PopFromRecycle();
	bool ShouldHide(CollectionViewGroup* group) const;

	DpObject* ContainerForGroup(CollectionViewGroup* group);
	void AddEmptyGroupItem(CollectionViewGroup* group);
	void SetAlternationCount();
	// ��ż�У�����У��
	void ChangeAlternationCount(int newAlternationCount);
	void LinkContainerToItem(DpObject* container, Object* item);

	// �ƶ�λ��
	int StartAt(int itemIndex);
	// ����һ������������
	DpObject* GenerateNext();

protected:

    int DoLinearSearch(DpObject*& container, Object* item);
    void OnCollectionChanged(Object* sender, NotifyCollChangedEventArg* e);

    void OnItemAdded(Object* item, int index);
    void OnItemRemoved(Object* item, int index);
    void OnItemReplaced(Object* oldItem, Object* newItem, int newIndex);
    void OnItemMoved(Object* oldItem, int oldIndex, int newIndex);
    void OnRefresh();

    friend class ItemsControlEx;

private:

    // �����ڲ��
    int _level;
    int _alternationCount;

    IList* _items;

    // �����Ѿ����ɵ���
    Array<ItemEntry> _itemMap;
    GroupStyle* _groupStyle;
    GeneratorState _cacheState;
    GeneratorDirection _direction;
    Array<DpObject*> _emptyGroupItems;
    Array<DpObject*> _recyclableContainers;

    DpObject* _peer;
    IGeneratorHost* _host;
    ItemContainerGeneratorEx* _parent;

};

}

#endif
