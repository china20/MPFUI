// 华勤科技版权所有 2010-2011
// 
// 文件名：ItemEntry.h
// 功  能：定义核心库的子项类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIITEMENTRY_H_
#define _UIITEMENTRY_H_

#include <System/Windows/Object.h>
#include <System/Windows/Delegate.h>
#include <System/Windows/EventArg.h>
#include <System/Windows/Binding.h>

#define DEFAULTITEMSIZE 16

namespace suic
{

/// <Summary>
/// ItemsControl子项基类
/// </Summary>
class SUICORE_API ItemEntry : public Object
{
public:

    ItemEntry();
    ItemEntry(Object* item);
    ~ItemEntry();

    RTTIOfClass(ItemEntry)

    Size GetSize()
    {
        return _size;
    }

    void SetSize(Size size)
    {
        _size = size;
    }

    Object* GetItem()
    {
        return _item;
    }

    void SetItem(Object* item)
    {
        SETREFOBJ(_item, item);
    }

    bool IsCached() const
    {
        return _size.cx >= 0;
    }

    void ResetCache()
    {
        _size.cx = -1;
    }

    String ToString()
    {
        return _item->ToString();
    }

private:

    Size _size;
    Object* _item;
};

}

#endif
