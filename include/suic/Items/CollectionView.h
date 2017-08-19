// 华勤科技版权所有 2010-2011
// 
// 文件名：CollectionView.h
// 功  能：ItemsControl分组对象
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UICOLLECTIONVIEWEX_H_
#define _UICOLLECTIONVIEWEX_H_

#include <System/Windows/Binding.h>
#include <Framework/Items/Enumerable.h>

namespace suic
{

class NotifyCollChanged
{
public:

    virtual ~NotifyCollChanged() {}
    NotifyCollChangedEventHandler CollChanged;
};

class CollectionView : public NotifyCollChanged
{
public:

    virtual bool IsEmpty() { return false; }
    virtual IList* GetView() = 0;
};

}

#endif
