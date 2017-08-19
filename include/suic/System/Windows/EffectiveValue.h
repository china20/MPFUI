// 华勤科技版权所有 2010-2012
// 
// 文件名：EffectiveValue.h
// 功  能：MPF属性实现等逻辑。
// 
// 作  者：MPF开发组
// 时  间：2010-05-01
// 
// ============================================================================

#ifndef _UIEFFECTIVEENTRY_H_
#define _UIEFFECTIVEENTRY_H_

#include <System/Windows/Delegate.h>
#include <System/Windows/DpProperty.h>

namespace suic
{

class DpProperty;

enum ValueSourceType : Uint32
{
    ValueSourceMask         = 0x000F,
    ModifiersMask           = 0x0070,
    IsExpression            = 0x0010,
    IsAnimated              = 0x0020,
    IsCoerced               = 0x0040,
    IsDeferredReference     = 0x0080,
    IsInheritedValue        = 0x0100,
    IsCoercedWithCurrentValue = 0x200,
    InnerUpdateFlag           = 0x800,
};

enum BaseValueType : Uint32
{
    btUnknown                 = 0,
    btDefault                 = 1,
    btInherited               = 2,
    btStyle                   = 3,
    btTemplateTrigger         = 4,
    btStyleTrigger            = 5,
    btParentTemplate          = 6,
    btParentTemplateTrigger   = 7,
    btLocal                   = 8,
};

enum RequestFlags
{
    rfAnimationBaseValue = 1,
    rfCoercionBaseValue = 2,
    rfDeferredReferences = 4,
    rfFullyResolved = 0,
    rfRawEntry = 0x10,
    rfSkipDefault = 8
};

enum OperationType : Byte
{
    otUnknown = 0,
    otInvalidateDp = 1,
    otAddChild = 2,
    otInherit = 3,
    otRemoveChild = 4,
    otForceUpdate = 5,
};

struct SUICORE_API EntryIndex
{
    Uint32 _store;

    EntryIndex();
    EntryIndex(Uint32 index);

    EntryIndex(Uint32 index, bool found);

    bool IsFound();
    Uint32 GetIndex();
};

class SUICORE_API ReviseValue : public Object
{
public:

    RTTIOfClass(ReviseValue)

    ReviseValue();
    ~ReviseValue();

    Object* GetBaseValue();
    void SetBaseValue(Object* val);

    Object* GetExprValue();
    void SetExprValue(Object* val);

    Object* GetAnimatedValue();
    void SetAnimatedValue(Object* val);

    Object* GetCoercedValue();
    void SetCoercedValue(Object* val);

private:

    Object* _baseValue;
    Object* _coercedValue; 
    Object* _animatedValue; 
    Object* _expressionValue;
};

typedef shared<ReviseValue> ReviseValuePtr;

inline Object* ReviseValue::GetBaseValue()
{ 
    return _baseValue;
}

inline Object* ReviseValue::GetExprValue()
{ 
    return _expressionValue;
}

inline Object* ReviseValue::GetAnimatedValue()
{
    return _animatedValue;
}

inline Object* ReviseValue::GetCoercedValue()
{
    return _coercedValue;
}

inline void ReviseValue::SetCoercedValue(Object* val)
{
    SETREFOBJ(_coercedValue, val);
}

class SUICORE_API EffectiveValue
{
public:

    ~EffectiveValue();

    EffectiveValue();
    EffectiveValue(const EffectiveValue& Other);
    EffectiveValue(DpProperty* dp, int bvType);
    EffectiveValue(DpProperty* dp, Object* val, int bvType);

    EffectiveValue& operator=(const EffectiveValue& Other);

    bool IsExpression() const;
    bool IsInherited() const;
    bool IsAnimated() const;
    bool IsCoerced() const;

    bool IsDeferredReference() const;

    bool HasRevise() const;

    int GetValueSourceType() const;
    BaseValueType GetBaseValueType() const;
    bool IsCoercedWithCurrentValue() const;

    PropMetadata* GetMetadata() const;

    void Reset();
    void ResetEV(DpProperty* dp, Object* val, int bvType);
    void ResetDefaultEV(DpProperty* dp, Object* value);

public:

    // 属性定义
    int GetPropertyIndex() const;

    DpProperty* GetProp() const;
    void SetPropertyAndIndex(DpProperty* val, int index);

protected:

    Object* GetLocalValue();
    void SetLocalValue(Object* val);

    Object* GetValue() const;
    void SetValue(Object* val);

    void ResetValue(Object* val, bool flag);

    void SetCurrentValue(Object* val);

    void SetExprValue(Object* val, Object* baseVal);
    void SetAnimatedValue(Object* val, Object* baseVal);
    void SetCoercedValue(Object* val, bool coerceWithCurrentValue);

    void SetAnimationBaseValue(Object* animationBaseValue);

    void ResetCoercedValue();
    void ResetAnimatedValue();

    Object* GetBaseValue();
    ReviseValue* GetReviseValue();
    ReviseValue* EnsureReviseValue();

    void SetBaseValueType(BaseValueType bvType);

    void SetIsExpression(bool val);
    void SetIsAnimated(bool val);
    void SetIsCoerced(bool val);

    void SetIsDeferredReference(bool val);
    void SetIsCoercedWithCurrentValue(bool val);

    void WritePrivateFlag(bool val, int flag);
    void ReviseValueValue(ReviseValue* reviseValue, int flag);

    Object* GetFlattenedValue(RequestFlags requests);
    void FindFlattenedEntry(RequestFlags requests, EffectiveValue& entry);

    void SetMetadata(PropMetadata* meta)
    {
        _meta = meta;
    }

    void Clear();

    friend class DpObject;
    friend class MemberVisitor;
    friend class FrameworkElement;

private:

    void ComputeIsDeferred();
    void CheckReviseValue(ReviseValue* reviseValue);

private:

    // 内部标记
    int _source;
    int _propIndex;
    Object* _value;
    PropMetadata* _meta;
    DpProperty* _prop;
};

inline PropMetadata* EffectiveValue::GetMetadata() const
{
    return _meta;
}

inline DpProperty* EffectiveValue::GetProp() const
{
    return _prop;
}

inline int EffectiveValue::GetPropertyIndex() const
{
    return _propIndex;
}

inline void EffectiveValue::SetPropertyAndIndex(DpProperty* val, int index)
{
    _prop = val;
    if (_prop)
    {
        _propIndex = _prop->GetIndex();
    }
    else
    {
        _propIndex = index;
    }
}

inline int EffectiveValue::GetValueSourceType() const
{
    return _source;
}

inline BaseValueType EffectiveValue::GetBaseValueType() const
{
    return (BaseValueType)(_source & ValueSourceType::ValueSourceMask);
}

inline void EffectiveValue::SetBaseValueType(BaseValueType bvType)
{
    _source = (_source & (~ValueSourceType::ValueSourceMask)) | bvType;
}

inline bool EffectiveValue::IsExpression() const
{
    return ((_source & ValueSourceType::IsExpression) != 0);
}

inline bool EffectiveValue::IsInherited() const
{
    return ((_source & ValueSourceType::IsInheritedValue) != 0);
}

inline bool EffectiveValue::IsAnimated() const
{
    return ((_source & ValueSourceType::IsAnimated) != 0);
}

inline bool EffectiveValue::IsCoerced() const
{
    return ((_source & ValueSourceType::IsCoerced) != 0);
}

inline bool EffectiveValue::IsDeferredReference() const
{
    return ((_source & ValueSourceType::IsDeferredReference) != 0);
}

inline bool EffectiveValue::IsCoercedWithCurrentValue() const
{
    return ((_source & ValueSourceType::IsCoercedWithCurrentValue) != 0);
}

inline bool EffectiveValue::HasRevise() const
{
    return ((_source & ValueSourceType::ModifiersMask) != 0);
}

inline void EffectiveValue::WritePrivateFlag(bool val, int flag)
{
    if (val)
    {
        _source |= flag;
    }
    else
    {
        _source &= ~flag;
    }
}

inline void EffectiveValue::SetIsExpression(bool val)
{
    WritePrivateFlag(val, ValueSourceType::IsExpression);
}

inline void EffectiveValue::SetIsAnimated(bool val)
{
    WritePrivateFlag(val, ValueSourceType::IsAnimated);
}

inline void EffectiveValue::SetIsCoerced(bool val)
{
    WritePrivateFlag(val, ValueSourceType::IsCoerced);
}

inline void EffectiveValue::SetIsDeferredReference(bool val)
{
    WritePrivateFlag(val, ValueSourceType::IsDeferredReference);
}

inline void EffectiveValue::SetIsCoercedWithCurrentValue(bool val)
{
    WritePrivateFlag(val, ValueSourceType::IsCoercedWithCurrentValue);
}

}

#endif
