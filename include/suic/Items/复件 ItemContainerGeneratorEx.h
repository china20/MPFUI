// 华勤科技版权所有 2010-2011
// 
// 文件名：ItemContainerGeneratorEx.h
// 功  能：ItemsControl分组对象
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
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
	Object* item;
	DpObject* container;
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

struct ItemBlock;

// 项生成状态
struct GeneratorState
{
    // 当前相对于Block的偏移
    int offset;
    // 项的索引
    int index;
    int count;
    // 当前Block
    ItemBlock* block;

    GeneratorState()
    {
        index = 0;
        offset = 0;
        block = NULL;
    }

    GeneratorState(int i, int pos)
    {
        index = i;
        offset = pos;
        block = NULL;
    }
};

struct ItemBlock
{
	// 已保存的Item个数
	int count;
	// 已经计算的缓存的大小
	Size cacheSize;
	// 每个块保存16个数据项
	ItemEntry* entry;
	ItemBlock* prev;
	ItemBlock* next;

	ItemBlock();
    ~ItemBlock();

    int ContainerCount();

    DpObject* ContainerAt(int index)
    {
        return entry[index].container;
    }

	int OffsetOfItem(Object* item);

    void AddItem(Object* item);
    void RemoveAt(int index);

	void RealizeItem(int index, Object* item, DpObject* container);

	ItemBlock* InsertAfter();
	ItemBlock* InsertBefore();

	void MoveForward(GeneratorState& state);
	void MoveBackward(GeneratorState& state);
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

	// 获取项个数
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

    Object* ItemFromContainer(DpObject* container);
    int IndexFromContainer(DpObject* container);
    DpObject* ContainerFromItem(Object* item);
    DpObject* ContainerFromIndex(int index);

	DpObject* PopFromRecycle();
	bool ShouldHide(CollectionViewGroup* group) const;

	DpObject* ContainerForGroup(CollectionViewGroup* group);
	void AddEmptyGroupItem(CollectionViewGroup* group);
	void SetAlternationCount();
	// 奇偶行，进行校正
	void ChangeAlternationCount(int newAlternationCount);
	void LinkContainerToItem(DpObject* container, Object* item);

	// 移动位置
	int StartAt(int itemIndex);
	// 生成一个项容器对象
	DpObject* GenerateNext();

	void SetAlternationIndex(ItemBlock* block, int offset, GeneratorDirection direction);
	void Realize(ItemBlock* block, int offset, Object* item, DpObject* container);

	void GetBlockAndPosition(int itemIndex, GeneratorPosition& position, ItemBlock*& block, int& offsetFromBlockStart);
	void GetBlockAndPosition(Object* item, GeneratorPosition& position, ItemBlock*& block, int& offsetFromBlockStart, int& correctIndex);

protected:

    bool DoLinearSearch(DpObject* container, Object* item, int& itemIndex);
    void OnCollectionChanged(Object* sender, NotifyCollChangedEventArg* e);

    void OnItemAdded(Object* item, int index);
    void OnItemRemoved(Object* item, int index);
    void OnItemReplaced(Object* oldItem, Object* newItem, int newIndex);
    void OnItemMoved(Object* oldItem, int oldIndex, int newIndex);
    void OnRefresh();

    friend class ItemsControlEx;

private:

    // 组所在层次
    int _level;
    int _alternationCount;

    IList* _items;

    // 缓存已经生成的项
    ItemBlock* _itemMap;
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
