// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����GroupItem.h
// ��  �ܣ�ItemsControl�������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UIGROUPITEM_H_
#define _UIGROUPITEM_H_

#include <Framework/Controls/Content.h>

namespace suic
{

class ItemContainerGeneratorEx;

class GroupItem : public suic::ContentControl
{
public:

	RTTIOfClass(GroupItem)

    static bool StaticInit();

	GroupItem();
	~GroupItem();

    void ClearContainerForItem(Object* item);
    void PrepareItemContainer(Object* item);

    ItemContainerGeneratorEx* GetGenerator();
    void SetGenerator(ItemContainerGeneratorEx* val);

protected:

	virtual void OnSetGenerator(ItemContainerGeneratorEx* generator)
	{
	}

private:

    ItemContainerGeneratorEx* _generator;
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

    void OnSetGenerator(ItemContainerGeneratorEx* generator)
    {
        //generator->ItemsChanged += ItemsChangedEventHandler(this, &EmptyGroupItem::OnItemsChanged);
    }
};

}

#endif
