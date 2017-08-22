// 华勤科技版权所有 2010-2012
// 
// 文件名：DeferredRef.h
// 功  能：依赖属性的基类，完成依赖属性实现等逻辑。
// 
// 作  者：MPF开发组
// 时  间：2011-05-01
// 
// ============================================================================

#ifndef _UIDEFERREDREFERENCE_H_
#define _UIDEFERREDREFERENCE_H_

#include <System/Windows/EffectiveValue.h>

namespace suic
{

class DeferredRef : public Object
{
public:

    RTTIOfAbstractClass(DeferredRef)

    virtual ~DeferredRef() {}
    virtual Object* GetValue(BaseValueType valueSource) = 0;
};

}

#endif
