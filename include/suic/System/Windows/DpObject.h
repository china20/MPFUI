// 华勤科技版权所有 2010-2012
// 
// 文件名：dpobject.h
// 功  能：依赖属性的基类，完成依赖属性实现等逻辑。
// 
// 作  者：MPF开发组
// 时  间：2011-05-01
// 
// ============================================================================

#ifndef _UIDPOBJECT_H_
#define _UIDPOBJECT_H_

#include <System/Tools/Array.h>
#include <System/Types/Numeric.h>

#include <System/Windows/Object.h>
#include <System/Windows/EffectiveValue.h>

namespace suic
{

class Assigner;
class Expression;
class DpProperty;
class PropMetadata;
class UncommonProperty;

class SUICORE_API IAddChild
{
public:

    virtual ~IAddChild() {}

    virtual void AddText(String val) = 0;
    virtual void AddChild(Object* child) = 0;
    virtual void RemoveChild(Object* child) = 0;
};

class EffectiveArray : public Array<EffectiveValue, true>
{
public:

    EffectiveArray()
        : _inhritCount(0)
    {
    }

    void Clear();
    void RemoveAt(int index, DpProperty* dp);
    void Insert(int index, const EffectiveValue& entry);

    int GetInhritCount() const;
    void SetInhritCount(int val);

private:

    int _inhritCount;
};

inline int EffectiveArray::GetInhritCount() const
{
    return _inhritCount; 
}

inline void EffectiveArray::SetInhritCount(int val)
{
    _inhritCount = val;
}

class SUICORE_API DpObject : public Object 
{    
public:

    static DpProperty* DirectDpProperty;
    static UncommonProperty* DependentDpField;

    static bool StaticInit();

    static Expression* GetExpression(DpObject* d, DpProperty* dp, PropMetadata* metadata);

    RTTIOfAbstractClass(DpObject)

    DpObject();
    ~DpObject();

    bool CheckAccess();
    void VerifyAccess(char* f, int l);

    /// <summary>
    ///     获得当前线程的分配器对象。
    /// </summary>
    /// <returns>分配器对象</returns>
    Assigner* GetAssigner() const;

    void ClearValue(DpProperty* dp);
    void CoerceValue(DpProperty* dp);

    void ClearValueDirectly(DpProperty* dp);
    void NotifyDpPropertyChanged(DpPropChangedEventArg* e);

    int GetBaseValueSource(DpProperty* dp);

    void UpdateDepProp(DpProperty* dp);
    void InvalidateProp(DpProperty* dp);

    Object* GetValue(DpProperty* dp);
    Object* GetDefaultValue(DpProperty* dp);
    Object* ReadLocalValue(DpProperty* dp, bool allowDefer);

    void SetValue(DpProperty* dp, Object* value);

    /// <summary>
    ///  更新元素对应属性的值
    /// </summary>
    /// <remarks>
    ///  此方法不论新值是否和旧值相等都会触发一次值的改变
    /// </remarks>
    /// <param name="dp">属性对象</param>
    /// <param name="value">新属性值</param>
    /// <returns>无</returns> 
    void SetEqualValue(DpProperty* dp, Object* value);
    void SetCurrentValue(DpProperty* dp, Object* value);
    
    /// <summary>
    ///  触发一次元素属性改变通知事件
    /// </summary>
    /// <remarks>
    ///  此方法会强行触发属性改变通知（不管属性值是否改变都会）
    /// </remarks>
    /// <param name="dp">属性对象</param>
    /// <returns>无</returns> 
    void RaiseDpPropertyChanged(DpProperty* dp);

    bool HasNonDefaultValue(DpProperty* dp);
    bool HasDefaultValue(DpProperty* dp, bool ignoreModifiers);
    bool HasDefaultOrInheritedValue(DpProperty* dp, bool checkInherited, bool ignoreModifiers);

    bool IsSealed();

    bool InDesignMode() const;

    bool ReadDoFlag(Uint32 key) const;
    void WriteDoFlag(Uint32 key, bool add);

    bool ContainsValue(DpProperty* dp);
    DpObject* GetInheritParent() const;

    virtual void Seal();
    virtual void Dispose();
    virtual DpObject* GetContextParent() const;
    virtual void SetAttribute(const String& key, Object* val);

protected:

    void ClearValueFromStyle(DpProperty* dp);
    void RemoveEV(Uint32 index, DpProperty* dp);
    void InsertEV(EffectiveValue& entry, Uint32 index);

    void SetEV(EntryIndex entryi, DpProperty* dp, PropMetadata* metadata, EffectiveValue& newEntry);
    void SetEV(EntryIndex entryi, DpProperty* dp, int iTarget, PropMetadata* metadata, Object* val, BaseValueType source);
    void EvaluateExpr(EntryIndex entryi, DpProperty* dp, Expression* expr, PropMetadata* metadata, EffectiveValue& newEntry);

    int GetBaseValueSourceByEntryIndex(EntryIndex& entryi);
    int UpdateEV(EntryIndex entryi, DpProperty* dp, EffectiveValue& oldEntry, EffectiveValue& newEntry, bool exprHoldflag, bool coerceWithCurr, OperationType operType);

    PropMetadata* FindMetadata(EntryIndex& entryi, DpProperty* dp);
    EntryIndex FindEntryIndex(int target);
    void CheckEntryIndex(EntryIndex& entryi, int target);

    bool GetRawEV(DpProperty* dp, EffectiveValue& entry);

    void GetEV(EntryIndex entryi, DpProperty* dp, PropMetadata*& metadata, RequestFlags requests, EffectiveValue& entry);
    void GetFlattenedEntry(EntryIndex entryi, DpProperty* dp, RequestFlags requests, EffectiveValue& flattenedEntry);

    Object* GetLocalValue(Uint32 index);
    void InternalSetValue(DpProperty* dp, Object* value);
    Object* InternalReadLocalValue(EntryIndex entryi, DpProperty* dp, bool allowDefer);

    void InvalidateTrigger(DpProperty* dp);

    friend class StyleHelper;
    friend class MemberVisitor;

protected:

    /// <summary>
    ///  元素属性值变化时进行回调（定义在元素本身节点的属性）
    /// </summary>
    /// <param name="e">属性信息事件</param>
    /// <returns>无</returns>
    virtual void OnDpPropertyChanged(DpPropChangedEventArg* e);

    virtual void UpdateInheritanceContext(const EffectiveValue& val);
    virtual void RemoveInheritanceContext(const DpProperty* dp);

private:

    PropMetadata* SetupPropMetadata(DpProperty* dp);

    void SetExprValue(EntryIndex entryi, Object* value, Object* baseValue);
    void InternalClearValue(EntryIndex entryi, DpProperty* dp, PropMetadata* metadata);
    void InternalSetValue(DpProperty* dp, Object* value, bool coerceWithCurr, OperationType operationType, bool isInternal);

protected:

    Uint32 _viewflag;
    Assigner* _assigner;
    WeakRef _contextStorage;

    mutable EffectiveArray _effects;
    friend class MemberVisitor;
};

inline Assigner* DpObject::GetAssigner() const
{
    return _assigner;
}

inline bool DpObject::IsSealed()
{
    return ReadDoFlag(ViewFlags::IsObjectSealedVF); 
}

inline bool DpObject::ReadDoFlag(suic::Uint32 key) const
{
	return (key == (_viewflag & key));
}

}

#endif
