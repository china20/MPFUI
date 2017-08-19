// 华勤科技版权所有 2010-2011
// 
// 文件名：IGeneratorHost.h
// 功  能：ItemsControl分组对象
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIIGENERATORHOST_H_
#define _UIIGENERATORHOST_H_

#include <Framework/Items/CollectionViewGroup.h>
#include <Framework/Items/ItemCollectionEx.h>

namespace suic
{

class IGeneratorHost
{
public:

	virtual ~IGeneratorHost() {}

	virtual void SetIsGrouping(bool isGrouping) = 0;
	virtual bool IsItemItsOwnContainer(Object* item) = 0;
	virtual DpObject* GetContainerForItem(Object* item) = 0;
	virtual bool IsHostForItemContainer(DpObject* container) = 0;

	virtual void PrepareItemContainer(DpObject* container, Object* item) = 0;
    virtual void ClearContainerForItem(DpObject* container, Object* item) = 0;
    virtual GroupStyle* GetGroupStyle(CollectionViewGroup* group, int level) = 0;

    virtual int GetAlternationCount() = 0;
    virtual ItemCollectionEx* GetView() = 0;
};

}

#endif
