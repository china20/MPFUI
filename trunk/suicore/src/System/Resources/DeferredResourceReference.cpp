

#include <System/Resources/DeferredResourceReference.h>
#include <System/Resources/ResourceDictionary.h>

namespace suic
{

DeferredResRef::DeferredResRef(ResourceDictionary* dictionary, String key)
    : _dictionary(dictionary)
    , _key(key)
{
}

DeferredResRef::DeferredResRef(String key, RefBase* val)
    : _dictionary(NULL)
    , _key(key)
{
    _value = val;
    AddRefObject(_value);
}

DeferredResRef::~DeferredResRef()
{
    FreeRefObject(_value);
}

RefBase* DeferredResRef::GetValue(BaseValueType valueSource)
{
    bool flag;

    if (_dictionary == NULL)
    {
        return _value;
    }

    RefBase* obj2 = _dictionary->GetValueWithoutLock(_key, flag);
    if (flag)
    {
        SetRefObject(_value, obj2);
        _dictionary = NULL;
    }
    return obj2;
}

}
