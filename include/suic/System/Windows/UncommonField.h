// 华勤科技版权所有 2008-2022
// 
// 文件名：UncommonField.h
// 功  能：一定一般依赖属性，此依赖属性没有名字和类型。
// 
// 作  者：MPF开发组
// 时  间：2012-08-02
// 
// ============================================================================

#ifndef _UIUNCOMMONFIELD_H_
#define _UIUNCOMMONFIELD_H_

#include <System/Windows/DpObject.h>
#include <System/Windows/Exception.h>

namespace suic
{

class DpObject;
class DpProperty;

class SUICORE_API UncommonProperty
{
public:

    UncommonProperty();
    UncommonProperty(Object* val);

    Object* GetValue(DpObject* d);
    void SetValue(DpObject* d, Object* val);

    void ClearValue(DpObject* d);

    int GetIndex()
    {
        return _globalIndex;
    }

protected:

    int _globalIndex; 
    bool _hasBeenSet;
    Object* _default;
};

template <typename T>
class UncommonField
{
public:

    UncommonField()
    {
    }

    UncommonField(T* def) : _prop(def)
    { 
    }

    void SetValue(DpObject* d, T* val)
    { 
        _prop.SetValue(d, val);
    }

    T* GetValue(DpObject* d) 
    {
        T* val = DynamicCast<T>(_prop.GetValue(d));
        return val;
    }

    void ClearValue(DpObject* d)
    {
        _prop.ClearValue(d);
    }

private:

    UncommonProperty _prop;
};

}

#endif
