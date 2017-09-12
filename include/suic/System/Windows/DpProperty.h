// 华勤科技版权所有 2010-2012
// 
// 文件名：DpProperty.h
// 功  能：MPF属性实现等逻辑。
// 
// 作  者：MPF开发组
// 时  间：2010-05-01
// 
// ============================================================================

#ifndef _UIDPPROPERTY_H_
#define _UIDPPROPERTY_H_

#include <System/Types/Integer.h>
#include <System/Types/XamlNode.h>
#include <System/Tools/Collection.h>
#include <System/Windows/Event.h>
#include <System/Windows/ValueConvert.h>

namespace suic
{

class DpObject;
class DpProperty;
class DpPropChangedEventArg;

template<typename R>
struct ValueCallback
{
    typedef R(*fnFuncType)(DpObject*,Object*);

    ValueCallback() : func(NULL) {}
    ValueCallback(fnFuncType ft) : func(ft) {}

    bool safe_bool() const 
    { 
        return (func != NULL); 
    }

    R operator()(DpObject* d, Object* e) 
    { 
        return func(d, e); 
    }

    void operator+=(const ValueCallback& Other)
    {
        func = Other.func;
    }

    fnFuncType func;
};

typedef ValueCallback<bool> ValidateValueCallback;
typedef ValueCallback<Object*> CoerceValueCallback;

struct PropChangedCallback
{
    typedef void(*fnFuncType)(DpObject*,DpPropChangedEventArg*);

    PropChangedCallback() : func(NULL) {}
    PropChangedCallback(fnFuncType ft) : func(ft) {}

    bool safe_bool() const 
    {
        return (func != NULL);
    }

    void operator()(DpObject* d, DpPropChangedEventArg* e)
    {
        func(d, e);
    }

    void operator+=(const PropChangedCallback& Other)
    {
        func = Other.func;
    }

    fnFuncType func;
};

enum PropMetadataOptions
{
    AffectsNone = 0,
    AffectsMeasure = 0x1,
    AffectsArrange = 0x2,
    AffectsRender = 0x4,
    AffectsInherits = 0x8,

    AffectsParentMeasure = 0x100,
    AffectsParentArrange = 0x200,
    AffectsParentRender = 0x400,
};

class SUICORE_API PropMetadata
{
public:

    PropMetadata();
    PropMetadata(Object* val);
    PropMetadata(Object* val, int flag);
    PropMetadata(Object* val, int flag, PropChangedCallback propChangedCb);
    PropMetadata(Object* val, int flag, PropChangedCallback propChangedCb, CoerceValueCallback coerceValueCb);

    ~PropMetadata();

    RTTIOfInfo* GetType();
    void SetType(RTTIOfInfo* val);

    bool IsInherited();
    void SetInherited(bool val);

    bool IsAffectsArrange();
    bool IsAffectsMeasure();
    bool IsAffectsRender();

    bool IsAffectsParentArrange();
    bool IsAffectsParentMeasure();
    bool IsAffectsParentRender();
    
    void SetAffectsArrange(bool val);
    void SetAffectsMeasure(bool val);
    void SetAffectsRender(bool val);

    void SetAffectsParentArrange(bool val);
    void SetAffectsParentMeasure(bool val);
    void SetAffectsParentRender(bool val);

public:

    Object* GetDefaultValue();
    void SetDefaultValue(Object* val);

protected:

    void ConvertFlag(int flag);

private:

    Uint32 _flag;
    RTTIOfInfo* _forType;
    Object* _defaultValue;

    CoerceValueCallback _coerceValueCallback;
    PropChangedCallback _dpPropChangedCallback;

    friend class DpObject;
    friend class DpProperty;
};

inline RTTIOfInfo* PropMetadata::GetType()
{
    return _forType;
}

inline void PropMetadata::SetType(RTTIOfInfo* val)
{
    _forType = val;
}

inline Object* PropMetadata::GetDefaultValue()
{
    return _defaultValue;
}

inline void PropMetadata::SetDefaultValue(Object* val)
{
    SETREFOBJ(_defaultValue, val);
}

class SUICORE_API DpProperty
{    
public:

    static void StaticInit();

    static Object* NullValue;

    static DpProperty* Register(const String& name, RTTIOfInfo* ownerType, RTTIOfInfo* propType, PropMetadata* defValue);
    static DpProperty* Register(const String& name, RTTIOfInfo* ownerType, RTTIOfInfo* propType, PropMetadata* defValue, ValidateValueCallback validateValueCb);
    static DpProperty* Register(const String& name, RTTIOfInfo* ownerType, RTTIOfInfo* propType, PropMetadata* defValue, ValidateValueCallback validateValueCb, int flag);
    
    static DpProperty* RegisterAttached(const String& name, RTTIOfInfo* ownerType, RTTIOfInfo* propType, PropMetadata* defValue);
    static DpProperty* RegisterReadOnly(const String& name, RTTIOfInfo* ownerType, RTTIOfInfo* propType, PropMetadata* defValue);

    static DpProperty* Lookup(const String& name, RTTIOfInfo* ownerType);
    static int GetUniqueGlobalIndex(RTTIOfInfo* ownerType, String name);

    static Object* UnsetValue();
    static void CheckUnsetValue(Object*& val);

    static void GetProperties(Collection<DpProperty*>& dps);

    static int CalcHashCode(const String& name, RTTIOfInfo* ownerType);

    DpProperty* AddOwner(RTTIOfInfo* ownerType);
    DpProperty* AddOwner(RTTIOfInfo* ownerType, PropMetadata* typeMetadata);
    PropMetadata* GetMetadata(RTTIOfInfo* forType);
    PropMetadata* FindMetadata(RTTIOfInfo* forType);
    PropMetadata* GetDefaultMetadata() const { return _defmetadata; }

    IResourceParser* GetResourceParser();

    bool IsValidValueInternal(DpObject* d, Object* val);
    bool IsValidValue(DpObject* d, Object* val);

public:

    virtual void OverrideMetadata(RTTIOfInfo* forType, PropMetadata* metadata);

public:

    bool IsReadOnly() const;
    bool IsAttached() const;

    Uint32 GetCacheType() const;
    void SetCacheType(Uint32 val);

    Object* GetValue();
    void SetValue(Object* val);

    int GetIndex();
    void SetIndex(int val);

    String GetName();
    void SetName(const String& val);

    int GetHashCode();

    bool CanConvert() const;
    void ConvertValue(Object* val, ObjectPtr& obj);

    void SetConvertValueCb(IConvertValue* convertValueCb);

    RTTIOfInfo* GetOwnerType();
    RTTIOfInfo* GetPropType();

protected:

    static Array<DpProperty*> _uncommonDpPropertyList;
    static ValueDic<int, DpProperty*> _propFromNames;
    static int _globalIndex;

    friend class UncommonProperty;

private:

    DpProperty(const String& strName, int flag, RTTIOfInfo* owner, RTTIOfInfo* prop, PropMetadata* defaultValue, ValidateValueCallback validateValueCb);
    ~DpProperty();
    friend class DpPropMemory;

private:

    static Object* _unsetValue;

    // 属性名称
    String _name;
    Uint32 _cacheFlag;
    // 属性所属的类
    RTTIOfInfo* _ownerType;
    RTTIOfInfo* _propType;

    // 属性标志
    int _flag;
    // 属性的全局索引
    int _index;
    // 属性hash值
    int _hashCode;

    // 默认值
    PropMetadata* _defmetadata;
    IConvertValue* _convertValue;
    IResourceParser* _resParser;
    ValidateValueCallback _validateValueCallback;
    mutable ObjectDic<RTTIOfInfo*, PropMetadata*> _metadatas;
};

inline Object* DpProperty::UnsetValue()
{
    return _unsetValue;
}

inline Uint32 DpProperty::GetCacheType() const
{
    return _cacheFlag;
}

inline void DpProperty::SetCacheType(Uint32 val)
{
    _cacheFlag = val;
}

inline Object* DpProperty::GetValue()
{
    return _defmetadata->GetDefaultValue();
}

inline void DpProperty::SetValue(Object* val)
{
    _defmetadata->SetDefaultValue(val);
}

inline int DpProperty::GetIndex()
{
    return _index;
}

inline void DpProperty::SetIndex(int val)
{
    _index = val;
}

inline RTTIOfInfo* DpProperty::GetOwnerType()
{
    return _ownerType;
}

inline RTTIOfInfo* DpProperty::GetPropType()
{
    return _propType;
}

class SUICORE_API DpPropMemory
{
public:

    static int GetPropCount();
    static int GetMetaCount();

    static PropMetadata* GetPropMeta(Object* val);
    static PropMetadata* GetPropMeta(Object* val, int flag);
    static PropMetadata* GetPropMeta(Object* val, int flag, PropChangedCallback propChangedCb);
    static PropMetadata* GetPropMeta(Object* val, int flag, PropChangedCallback propChangedCb, CoerceValueCallback coerceValueCb);

private:

    DpPropMemory();

    static int CheckMemory();
    static DpProperty* Create(String name, RTTIOfInfo* ownerType, RTTIOfInfo* propType, PropMetadata* defValue, ValidateValueCallback validateValueCb, int flag);

private:

    static int _capacity;
    static int _propCount;
    static int _metaCount;
    static Byte* _memory;
    static Byte* _mempos;

    friend class DpProperty;
};

}

#endif
