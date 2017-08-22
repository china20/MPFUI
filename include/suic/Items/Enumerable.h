// 华勤科技版权所有 2010-2011
// 
// 文件名：ItemsControl.h
// 功  能：实现Item容器的基类
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIENUMERABLE_H_
#define _UIENUMERABLE_H_

#include <Framework/Controls/Panel.h>

namespace suic
{

class IEnumerator
{
public:

    virtual ~IEnumerator() {}
    virtual bool MoveNext() = 0;
    virtual Object* Current() = 0;
    virtual void Reset() = 0;
};

class IEnumerable
{
public:

    virtual ~IEnumerable() {}
    virtual IEnumerator* GetEnumerator() = 0;
};

class IList : public IEnumerable
{
public:

    virtual ~IList() {}

    virtual int GetCount() = 0;

    virtual int Add(Object* value) = 0;
    virtual void Clear() = 0;
    virtual bool Contains(Object* value) = 0;
    virtual int IndexOf(Object* value) = 0;
    virtual void Insert(int index, Object* value) = 0;
    virtual void Remove(Object* value) = 0;
    virtual void RemoveAt(int index) = 0;

    virtual Object* GetItem(int index) = 0;
    virtual void SetItem(int index, Object* item) = 0;
};

}

#endif
