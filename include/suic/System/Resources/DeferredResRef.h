// 华勤科技版权所有 2010-2012
// 
// 文件名：DeferredResourceReference.h
// 功  能：资源属性值。
// 
// 作  者：MPF开发组
// 时  间：2011-05-01
// 
// ============================================================================

#ifndef _UIDEFERRESOURCEREDREFERENCE_H_
#define _UIDEFERRESOURCEREDREFERENCE_H_

#include <System/Windows/DeferredRef.h>

namespace suic
{

class ResourceDictionary;
class DeferredResRef : public DeferredRef
{
public:

    DeferredResRef(String key, Object* val);
    DeferredResRef(ResourceDictionary* dictionary, String key);
    ~DeferredResRef();

    Object* GetValue(BaseValueType valueSource);

private:

    ResourceDictionary* _dictionary;
    String _key;
    Object* _value;

};

}

#endif
