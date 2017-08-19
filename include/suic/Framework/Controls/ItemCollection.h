// 华勤科技版权所有 2010-2011
// 
// 文件名：ItemCollection.h
// 功  能：实现Item容器
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIITEMCOLLECTION_H_
#define _UIITEMCOLLECTION_H_

#include <Framework/Controls/Panel.h>
#include <Framework/Controls/ScrollViewer.h>
#include <Framework/Controls/DataTemplate.h>

#include <System/Windows/ItemsChangedEventArg.h>

namespace suic
{

/// <summary>
/// ItemCollection，实现ItemsControl项容器逻辑。
/// </summary>
class SUICORE_API ItemCollection : public Object
{
public:

    NotifyCollChangedEventHandler CollectionChanged;

    RTTIOfAbstractClass(ItemCollection)

    virtual ~ItemCollection() {}

public:

    virtual bool IsEmpty() = 0;

    virtual int GetCount() const = 0;
    virtual bool Contains(Object* item) = 0;
    virtual Object* GetItem(int index) const = 0;
    virtual ItemEntry* GetItemEntry(int index) const = 0;

    virtual int IndexOf(Object* value) = 0;
    virtual int IndexOf(Object* value, int startIndex, int count) = 0;

    virtual int AddItem(Object* value) = 0;
    virtual void SetItem(int index, Object* value) = 0;
    virtual void MoveItem(int oriPos, int dstPos) = 0;
    virtual void SwapItem(int index1, int index2) = 0;
    virtual void InsertItem(int index, Object* value) = 0;

    virtual bool RemoveItem(Object* obj) = 0;
    virtual bool RemoveItemAt(int index) = 0;
    virtual void RemoveRange(int index, int count) = 0;

    virtual void Clear() = 0;

    virtual void BeginInit();
    virtual void EndInit();

private:

    void SetLogicalParent(DpObject* parent);

private:

    WeakRef _logicalParent;
    friend class ItemsControl;
};

}

#endif
