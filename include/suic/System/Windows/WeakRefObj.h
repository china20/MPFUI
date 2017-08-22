// 华勤科技版权所有 2010-2011
// 
// 文件名：WeakRefObj
// 功  能：实现弱引用对象。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIWEAKREFOBJ_H_
#define _UIWEAKREFOBJ_H_

#include <System/Windows/Object.h>

namespace suic
{

class WeakRef;

class SUICORE_API WeakRefObj : public Object
{
public:

    WeakRefObj();
    WeakRefObj(Object* target);
    WeakRefObj(const WeakRef& Other);

    ~WeakRefObj();

    RTTIOfClass(WeakRefObj)

    WeakRefObj operator=(const WeakRef& Other);

    bool operator!=(const WeakRef& Other);
    bool operator==(const WeakRef& Other);
    bool Equal(const WeakRef& Other);
    bool Equals(Object* Other);

    bool IsAlive();
    Object* GetTarget();

    template<typename T>
    T* GetElem()
    {
        return dynamic_cast<T*>(_target.GetTarget());
    }

    operator Object*();

private:

    WeakRef _target;
};

}

#endif
