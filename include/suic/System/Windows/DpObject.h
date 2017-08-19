// ���ڿƼ���Ȩ���� 2010-2012
// 
// �ļ�����dpobject.h
// ��  �ܣ��������ԵĻ��࣬�����������ʵ�ֵ��߼���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-05-01
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
    ///     ��õ�ǰ�̵߳ķ���������
    /// </summary>
    /// <returns>����������</returns>
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
    ///  ����Ԫ�ض�Ӧ���Ե�ֵ
    /// </summary>
    /// <remarks>
    ///  �˷���������ֵ�Ƿ�;�ֵ��ȶ��ᴥ��һ��ֵ�ĸı�
    /// </remarks>
    /// <param name="dp">���Զ���</param>
    /// <param name="value">������ֵ</param>
    /// <returns>��</returns> 
    void SetEqualValue(DpProperty* dp, Object* value);
    void SetCurrentValue(DpProperty* dp, Object* value);
    
    /// <summary>
    ///  ����һ��Ԫ�����Ըı�֪ͨ�¼�
    /// </summary>
    /// <remarks>
    ///  �˷�����ǿ�д������Ըı�֪ͨ����������ֵ�Ƿ�ı䶼�ᣩ
    /// </remarks>
    /// <param name="dp">���Զ���</param>
    /// <returns>��</returns> 
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
    ///  Ԫ������ֵ�仯ʱ���лص���������Ԫ�ر���ڵ�����ԣ�
    /// </summary>
    /// <param name="e">������Ϣ�¼�</param>
    /// <returns>��</returns>
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

}

#endif
