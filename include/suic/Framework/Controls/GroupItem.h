// 华勤科技版权所有 2008-2022
// 
// 文件名：GroupItem.h
// 功  能：ItemsControl分组对象
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIGROUPITEM_H_
#define _UIGROUPITEM_H_

#include <Framework/Controls/Content.h>

namespace suic
{

class ItemContainerGenerator;

class SUICORE_API GroupItem : public suic::ContentControl
{
public:

	RTTIOfClass(GroupItem)

    static void StaticInit();

	GroupItem();
	~GroupItem();

    void ClearContainerForItem(Object* item);
    void PrepareItemContainer(Object* item);

    ItemContainerGenerator* GetGenerator();
    void SetGenerator(ItemContainerGenerator* val);

protected:

	virtual void OnSetGenerator(ItemContainerGenerator* generator)
	{
	}

private:

    ItemContainerGenerator* _generator;
};

class EmptyGroupItem : public GroupItem
{
public:
    
	void OnItemsChanged(Object* sender, ItemsChangedEventArg* e)
    {
        //CollectionViewGroup* group = (CollectionViewGroup*)GetValue(ItemContainerGenerator::ItemForItemContainerProperty);
        //if (group->GetItemCount() > 0)
        {
            //ItemContainerGenerator* generator = GetGenerator();
            //generator->ItemsChanged -= ItemsChangedEventHandler(this, &EmptyGroupItem::OnItemsChanged);
            //generator->GetParent->OnSubgroupBecameNonEmpty(this, group);
        }
    }

    void OnSetGenerator(ItemContainerGenerator* generator)
    {
        //generator->ItemsChanged += ItemsChangedEventHandler(this, &EmptyGroupItem::OnItemsChanged);
    }
};

}

#endif
