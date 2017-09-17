/////////////////////////////////////////////////////////////////////////
// Visual.cpp

#include <System/Windows/DpObject.h>
#include <System/Windows/Binding.h>
#include <System/Windows/FrameworkElement.h>
#include <System/Windows/DeferredRef.h>
#include <System/Windows/DependentList.h>
#include <System/Windows/MemberVisitor.h>

#include <System/Input/Keyboard.h>
#include <System/Input/Mouse.h>
#include <System/Input/MouseDevice.h>

#include <System/Windows/DpProperty.h>
#include <System/Windows/PropertyValue.h>

#include <Framework/Controls/RangeBase.h>

#include <System/Tools/Debug.h>
#include <System/Tools/Thread.h>
#include <System/Tools/StyleHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <Framework/Animation/Storyboard.h>

/*--------内部使用头文件--------*/
#include <System/Interop/InternalOper.h>

#include <typeinfo>

bool __IsStaticInit();

namespace suic
{

ImplementRTTIOfClass(DpObject, Object)
ImplementRTTIOfClass(DeferredRef, Object)

DpProperty* DpObject::DirectDpProperty;
UncommonProperty* DpObject::DependentDpField;

bool DpDependent::IsValid()
{
    if (!expr.IsAlive() || !d.IsAlive())
    {
        return false;
    }
    else
    {
        return true;
    }
}

DpDependent::DpDependent()
    : d(NULL)
    , expr(NULL)
    , prop(NULL)
{

}

DpDependent::DpDependent(DpObject* o, DpProperty* p, Expression* e)
{
    if (o)
    {
        d = o;
    }

    if (e)
    {
        expr = e;
    }

    prop = p;
}

DpObject* DpDependent::GetDO()
{
    return DynamicCast<DpObject>(d.GetTarget());
}

DpProperty* DpDependent::GetDP()
{
    return prop;
}

ResourceItem::ResourceItem() 
    : res(DpProperty::UnsetValue())
    , type(Resource::ResType::resUnknown) 
{
}

Expression* DpDependent::GetExpr()
{
    return DynamicCast<Expression>(expr.GetTarget());
}

bool DpDependent::Equals(Object* o)
{
    DpDependent* dpd = RTTIEqualCast<DpDependent>(o);
    if (!dpd)
    {
        return false;
    }
    else
    {
        if (dpd->d.GetTarget() == d.GetTarget() &&
            dpd->expr.GetTarget() == expr.GetTarget() && 
            dpd->prop == prop)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool operator==(DpDependent& first, DpDependent& second)
{
    return first.Equals(&second);
}

bool operator!=(DpDependent& first, DpDependent& second)
{
    return !first.Equals(&second);
}

int DpDependent::GetHashCode()
{
    Expression* target = GetExpr();
    int num = (target == NULL) ? 0 : target->GetHashCode();
    DpObject* obj = GetDO();

    if (obj != NULL)
    {
        num += obj->GetHashCode();
    }

    if (prop != NULL)
    {
        num += prop->GetHashCode();
    }

    return num;
}

DpObject::DpObject()
    : _viewflag(0)
{
    _assigner = Assigner::Current();
    _assigner->ref();
}

DpObject::~DpObject()
{
    //DpObject::DependentDpField->SetValue(this, DpProperty::UnsetValue());
    _effects.Clear();
    DpObject::DependentDpField->ClearValue(this);
    
    if (_assigner)
    {
        _assigner->unref();
    }
}

void DpObject::StaticInit()
{
    if (DependentDpField == NULL)
    {
        DependentDpField = new UncommonProperty(NULL);
        DirectDpProperty = DpProperty::Register(_T("DirectDp"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
    }
}

Object* DpObject::GetDefaultValue(DpProperty* dp)
{
    PropMetadata* meta = SetupPropMetadata(dp);
    if (NULL == meta)
    {
        return DpProperty::UnsetValue();
    }
    else
    {
        return meta->GetDefaultValue();
    }
}

void DpObject::Dispose()
{
    _effects.Clear();
    //DpObject::DependentDpField->ClearValue(this);
}

bool DpObject::CheckAccess()
{
    if(_assigner != NULL)
    { 
        return _assigner->CheckAccess();
    }
    else
    {
        return true;
    }
}

void DpObject::VerifyAccess(char* f, int l)
{
    if (_assigner != NULL)
    { 
        _assigner->VerifyAccess(f, l);
    } 
}

void DpObject::InvalidateTrigger(DpProperty* dp)
{
    EntryIndex entryIndex = FindEntryIndex(dp->GetIndex());
    if (entryIndex.IsFound())
    {
        EffectiveValue& entry = _effects[entryIndex.GetIndex()];
        BaseValueType bType = entry.GetBaseValueType();
        if (bType >= BaseValueType::btStyle && bType < BaseValueType::btLocal)
        {
            PropMetadata* metadata = NULL;
            InternalClearValue(entryIndex, dp, metadata);
        }
        else if (entry.HasRevise())
        {
            EffectiveValue newEntry(dp, BaseValueType::btUnknown);
            UpdateEV(entryIndex, dp, entry, newEntry, false, false, OperationType::otUnknown);
        }
    }
}

void DpObject::InvalidateProp(DpProperty* dp)
{
    VerifyAccess(__FILELINE__);

    if (dp == NULL)
    {
        throw ArgumentNullException(_U("dp"), __FILELINE__);
    }

    EffectiveValue newEntry(dp, BaseValueType::btUnknown);
    UpdateEV(FindEntryIndex(dp->GetIndex()), dp, EffectiveValue(), newEntry, false, false, OperationType::otInvalidateDp);
}

void DpObject::UpdateDepProp(DpProperty* dp)
{
    Object* obj = DpObject::DependentDpField->GetValue(this);

    if (obj != NULL)
    {
        IntAutoDicObj* dpMap = RTTIEqualCast<IntAutoDicObj>(obj);
        if (NULL == dpMap)
        {
            throw InvalidValueException(_U("invalidate DependentDpField"), __FILELINE__);
        }

        Object* dBind = NULL;
        dpMap->TryGetValue(dp->GetIndex(), dBind);

        // 依赖此属性的所有属性列表
        DependentList* dpList = RTTIEqualCast<DependentList>(dBind);
        if (dpList != NULL)
        {
            dpList->InvalidateDeps(this, dp);
        }
    }
}

bool DpObject::GetRawEV(DpProperty* dp, EffectiveValue& entry)
{
    EntryIndex entryi = FindEntryIndex(dp->GetIndex());

    if (entryi.IsFound())
    {
        entry = _effects[entryi.GetIndex()];
        return true;
    }
    else
    {
        entry.Reset();
        return false;
    }
}

int DpObject::GetBaseValueSource(DpProperty* dp)
{
    EntryIndex entryi = FindEntryIndex(dp->GetIndex());
    if (entryi.IsFound())
    {
        return _effects[entryi.GetIndex()].GetBaseValueType();
    }
    else
    {
        return BaseValueType::btUnknown;
    }
}

int DpObject::GetBaseValueSourceByEntryIndex(EntryIndex& entryi)
{
    if (entryi.IsFound())
    {
        return _effects[entryi.GetIndex()].GetBaseValueType();
    }
    else
    {
        return BaseValueType::btUnknown;
    }
}

DpObject* DpObject::GetInheritParent() const
{
    return (DpObject*)_contextStorage.GetTarget();
}

Object* DpObject::GetValue(DpProperty* dp)
{
    VerifyAccess(__FILELINE__);

    if (dp == NULL)
    {
        throw ArgumentNullException(_U("dp is null"), __FILELINE__);
    }
    
    PropMetadata* meta = NULL; // dp->GetMetadata(GetClassType());
    EntryIndex entryIndex = FindEntryIndex(dp->GetIndex());
    EffectiveValue entry;
    Object* val = DpProperty::UnsetValue();

    if (entryIndex.IsFound())
    {
        GetFlattenedEntry(entryIndex, dp, RequestFlags::rfFullyResolved, entry);
        meta = entry.GetMetadata();
        val = entry.GetValue();
    }
    else
    {
        meta = SetupPropMetadata(dp);
        val = meta->GetDefaultValue();

        if (meta->IsInherited())
        {
            /*DpObject* inhrit = GetInheritanceParent();
            if (NULL != inhrit)
            {
                entryIndex = inhrit->FindEntryIndex(dp->GetIndex());
                if (entryIndex.IsFound())
                {
                    inhrit->GetFlattenedEntry(entryIndex, dp, (RequestFlags)(RequestFlags::FullyResolved & RequestFlags::DeferredReferences), entry);
                    val = entry.GetValue();
                }
            }*/

            DpObject* inhrit = GetContextParent();
            while (inhrit != NULL)
            {
                entryIndex = inhrit->FindEntryIndex(dp->GetIndex());
                if (entryIndex.IsFound())
                {
                    inhrit->GetFlattenedEntry(entryIndex, dp, (RequestFlags)(RequestFlags::rfFullyResolved & RequestFlags::rfDeferredReferences), entry);
                    val = entry.GetValue();
                    break;
                }
                inhrit = inhrit->GetContextParent();
            }
        }
    }

#ifdef _DEBUG
    if (NULL == val)
    {
        throw "value is NULL";
    }
#endif

    return val;
}

Expression* DpObject::GetExpression(DpObject* d, DpProperty* dp, PropMetadata* metadata)
{
    EntryIndex index = d->FindEntryIndex(dp->GetIndex());

    if (index.IsFound())
    {
        EffectiveValue& entry = d->_effects[index.GetIndex()];

        if (entry.IsExpression())
        {
            return RTTICast<Expression>(entry.GetLocalValue());
        }
    }

    return NULL;
}

void DpObject::SetValue(DpProperty* dp, Object* value)
{
    InternalSetValue(dp, value);
}

void DpObject::InternalSetValue(DpProperty* dp, Object* value)
{
    VerifyAccess(__FILELINE__);
    InternalSetValue(dp, value, false, OperationType::otUnknown, false);
}

void DpObject::SetExprValue(EntryIndex entryi, Object* value, Object* baseValue)
{
    EffectiveValue& entry = _effects[entryi.GetIndex()];

    MemberVisitor::SetExprValue(&entry, value, baseValue);
    MemberVisitor::ResetAnimatedValue(&entry);
    MemberVisitor::ResetCoercedValue(&entry);
}

PropMetadata* DpObject::SetupPropMetadata(DpProperty* dp)
{
    if (dp == NULL)
    {
        throw ArgumentNullException(_U("dp"), __FILELINE__);
    }

    // 只读，抛出异常
    return dp->GetMetadata(GetRTTIType());
}

void DpObject::SetCurrentValue(DpProperty* dp, Object* value)
{
    VerifyAccess(__FILELINE__);
    InternalSetValue(dp, value, true, OperationType::otForceUpdate, false);
}

void DpObject::CoerceValue(DpProperty* dp)
{
    VerifyAccess(__FILELINE__);

    EntryIndex entry = FindEntryIndex(dp->GetIndex());
    PropMetadata* metadata = NULL;

    if (entry.IsFound())
    {
        EffectiveValue entryVal;
        GetEV(entry, dp, metadata, RequestFlags::rfRawEntry, entryVal);
        if (entryVal.IsCoercedWithCurrentValue())
        {
            SetCurrentValue(dp, entryVal.GetReviseValue()->GetCoercedValue());
            return;
        }
    }

    EffectiveValue newEntry(dp, (BaseValueType)ValueSourceType::IsCoerced);
    UpdateEV(entry, dp, EffectiveValue(), newEntry, false, false, OperationType::otUnknown);
}

bool DpObject::HasDefaultOrInheritedValue(DpProperty* dp, bool checkInherited, bool ignoreModifiers)
{
    bool flag = false;

    if (ignoreModifiers && GetRTTIType() == FrameworkElement::RTTIType())
    {
        return false;
    }
    else
    {
        EntryIndex entry = FindEntryIndex(dp->GetIndex());

        if (!entry.IsFound())
        {
            PropMetadata* metadata = SetupPropMetadata(dp);
            if (metadata->GetDefaultValue() == DpProperty::UnsetValue())
            {
                return false;
            }
            return true;
        }
        else
        {
            EffectiveValue& entryVal = _effects.GetItem(entry.GetIndex());
            return !entryVal.HasRevise();
        }
    }
}

bool DpObject::HasDefaultValue(DpProperty* dp, bool ignoreModifiers)
{
    return HasDefaultOrInheritedValue(dp, false, ignoreModifiers);
}

bool DpObject::HasNonDefaultValue(DpProperty* dp)
{
    return !HasDefaultOrInheritedValue(dp, false, true);
}

void DpObject::SetEqualValue(DpProperty* dp, Object* value)
{
    Expression* oriExpr = RTTICast<Expression>(value);
    PropMetadata* metadata = NULL;
    EffectiveValue entry;
    EntryIndex entryIndex = FindEntryIndex(dp->GetIndex());

    if (value == NULL || value == DpProperty::UnsetValue())
    {
        InternalClearValue(entryIndex, dp, metadata);
    }
    else
    {
        // 获取原始的有效值
        GetEV(entryIndex, dp, metadata, RequestFlags::rfRawEntry, entry);

        if (entry.GetValue() == value)
        {
            if (oriExpr == NULL)
            {
                Object* val = entry.GetFlattenedValue(RequestFlags::rfFullyResolved);
                DpPropChangedEventArg e(dp, metadata, val, val);
                NotifyDpPropertyChanged(&e);
            }
        }
        else
        {
            SetValue(dp, value);
        }
    }
}

void DpObject::RaiseDpPropertyChanged(DpProperty* dp)
{
    // 
    // 值已经改变，触发事件
    //
    /*PropMetadata* metadata = NULL;
    EffectiveValue entry;
    EntryIndex entryIndex = FindEntryIndex(dp->GetIndex());
    if (entryIndex.IsFound())
    {
        Object* val = entry.GetFlattenedValue(RequestFlags::FullyResolved);

        // 获取原始的有效值
        GetEffectiveValue(entryIndex, dp, metadata, RequestFlags::RawEntry, entry);
        DpPropChangedEventArg e(dp, metadata, val, val);
        NotifyDpPropertyChanged(&e);
    }*/
}

void DpObject::InternalSetValue(DpProperty* dp, Object* val, bool coerceWithCurr, OperationType operType, bool isInternal)
{
    if (NULL == dp)
    {
        throw ArgumentNullException(_U("dp"), _U("DpProperty is null!"), __FILELINE__);
    }

    PropMetadata* metadata = NULL;
    ObjectPtr reserveVal = val;

    Expression* expr = NULL;
    EntryIndex entryIndex = FindEntryIndex(dp->GetIndex());

    if (val == NULL || val == DpProperty::UnsetValue())
    {
        InternalClearValue(entryIndex, dp, metadata);
    }
    else
    {
        EffectiveValue entry;
        EffectiveValue newEntry;

        bool validateFlag = true;
        bool flag = false;

        // 是否是一个表达式
        expr = RTTICast<Expression>(val);

        if (!expr)
        {
            validateFlag = isInternal ? dp->IsValidValueInternal(this, val) : dp->IsValidValue(this, val);

            if (!validateFlag)
            {
                ArgumentException validatee(_U("InvalidPropertyValue"), __FILELINE__);
                return;
            }

            /*if (!validateFlag)
            {
                flag = DynamicCast<DeferredRef>(val) != NULL;

                if (!flag && !validateFlag)
                {
                    ArgumentException validatee(_U("InvalidPropertyValue"), __FILELINE__);
                    return val;
                }
            }*/
        }

        // 获取原始的有效值
        GetEV(entryIndex, dp, metadata, RequestFlags::rfRawEntry, entry);

        Expression* oriExpr = entry.IsExpression() ? RTTICast<Expression>(entry.GetLocalValue()) : NULL;
        bool exprHoldflag = false;

        // 
        // 旧值是表达式，新值不是
        //
        if ((oriExpr != NULL) && (expr == NULL))
        {
            /*if (flag)
            {
                SETREFOBJ(val, ((DeferredRef*)val)->GetValue(BaseValueType::Local));
            }*/
            exprHoldflag = oriExpr->SetValue(this, dp, val);
            if (!exprHoldflag)
            {
                if (entry.GetBaseValueType() == BaseValueType::btLocal)
                {
                    exprHoldflag = true;
                }
            }
            else if (oriExpr->GetRTTIType() == BindingExpression::RTTIType())
            {
                BindingExpression* binding = (BindingExpression*)oriExpr;
                if (binding->GetBinding() && 
                    binding->GetBinding()->GetBindMode() == BindingMode::TwoWay)
                {
                    oriExpr->GetValue(this, dp, reserveVal);
                }
            }
        }
        else if (entry.GetValue() == val)
        {
            // 
            // 值已经改变，触发事件
            //
            //DpPropChangedEventArg e(dp, metadata, &newEntry, &entry);
            //NotifyDpPropertyChanged(&e);
            return;
        }

        // 
        // 值缓存在表达式中
        //
        if (exprHoldflag)
        {
            newEntry.ResetDefaultEV(dp, oriExpr);
            newEntry.SetExprValue(reserveVal.get(), oriExpr);
            newEntry.SetBaseValueType(entry.GetBaseValueType());
            coerceWithCurr = false;
        }
        else
        {
            if (!coerceWithCurr && NULL != oriExpr)
            {
                oriExpr->OnDetach(this, dp);
            }
            newEntry.ResetEV(dp, DpProperty::UnsetValue(), BaseValueType::btLocal);
            MemberVisitor::SetValue(&newEntry, val);
        }

        UpdateEV(entryIndex, dp, entry, newEntry, exprHoldflag, coerceWithCurr, operType);
    }
}

int DpObject::UpdateEV(EntryIndex entryi, DpProperty* dp, EffectiveValue& oldEntry, EffectiveValue& newEntry
                                   , bool exprHoldflag, bool coerceWithCurr, OperationType operType)
{
    if (dp == NULL)
    {
        throw ArgumentNullException(_U("dp"), __FILELINE__);
    }

    PropMetadata* metadata = NULL;
    int globalIndex = dp->GetIndex();

    // 
    // 如果旧值无效，先获取
    //
    if (oldEntry.GetBaseValueType() == BaseValueType::btUnknown)
    {
        GetEV(entryi, dp, metadata, RequestFlags::rfRawEntry, oldEntry);
        if (operType == OperationType::otInvalidateDp)
        {
            if (oldEntry.GetBaseValueType() == BaseValueType::btLocal && !oldEntry.HasRevise())
            {
                return 0;
            }//13885873836
            operType = OperationType::otUnknown;
        }
    }
    else
    {
        metadata = oldEntry.GetMetadata();
    }

    // 
    // 优先级不够
    //
    if (operType != OperationType::otForceUpdate && (newEntry.GetBaseValueType() != BaseValueType::btUnknown) && 
        (newEntry.GetBaseValueType() < oldEntry.GetBaseValueType()))
    {
        return 0;
    }

    Expression* newExpr = NULL;
    Expression* oldExpr = NULL;
 
    ObjectPtr newValue;
    Object* oldValue = oldEntry.GetFlattenedValue(RequestFlags::rfFullyResolved);

    if (NULL == metadata)
    {
        if (newEntry.GetMetadata() != NULL)
        {
            metadata = newEntry.GetMetadata();
        }
        else
        {
            metadata = SetupPropMetadata(dp);
        }
    }

    if (!exprHoldflag)
    {
        //
        // 如果是校正值或当前值
        //
        if (coerceWithCurr)
        {
            bool bOldRevise = oldEntry.HasRevise();
            newValue = newEntry.GetValue();
            newExpr = RTTICast<Expression>(newValue.get());

            //
            // 新值为表达式，先计算表达式值
            //
            if (NULL != newExpr)
            {
                newExpr->GetValue(this, dp, newValue);

                if (newValue.get() == Expression::NoValue)
                {
                    newValue = oldValue;
                }
            }

            newEntry.SetCoercedValue(newValue.get(), true);

            //
            // 旧值的BaseValue保存到新值
            //
            if (oldEntry.HasRevise())
            {
                Object* oldBaseVal = oldEntry.GetReviseValue()->GetBaseValue();
                newEntry.GetReviseValue()->SetBaseValue(oldBaseVal);
            }
            else
            {
                newEntry.GetReviseValue()->SetBaseValue(oldEntry.GetValue());
            }

            newEntry.SetBaseValueType(oldEntry.GetBaseValueType());
        }
        else
        {
            if (newEntry.IsExpression())
            {
                newExpr = RTTICast<Expression>(newEntry.GetReviseValue()->GetBaseValue());
            }
            else
            {
                newValue = newEntry.GetValue();
                newExpr = RTTICast<Expression>(newValue.get());
            }

            //
            // 新值为表达式，这里完全替换旧值
            //
            if (NULL != newExpr)
            {
                bool bSkip = false;
                bool bSetExpr = false;
                bool bNewLocal = newEntry.GetBaseValueType() == BaseValueType::btLocal;

                //
                // 旧值为表达式，先detach
                //
                if (oldEntry.IsExpression())
                {
                    if (oldExpr == newExpr)
                    {
                        bSkip = true;
                    }
                    else
                    {
                        //
                        // 新值是本地值
                        //
                        if (bNewLocal)
                        {
                            oldExpr = RTTICast<Expression>(oldEntry.GetReviseValue()->GetBaseValue());
                            oldExpr->OnDetach(this, dp);
                            newExpr->OnAttach(this, dp);
                        }
                        else
                        {
                            bSetExpr = true;
                        }
                    }
                }
                else
                {
                    newExpr->OnAttach(this, dp);
                }

                if (!bSkip)
                {
                    //
                    // 估算新值的实际值
                    //
                    EvaluateExpr(entryi, dp, newExpr, metadata, newEntry);
                    if (bSetExpr)
                    {
                        newEntry.GetReviseValue()->SetBaseValue(oldEntry.GetReviseValue()->GetBaseValue());
                    }
                }
            }
            else if (newEntry.GetBaseValueType() == BaseValueType::btLocal)
            {
                if (newValue.get() == DpProperty::UnsetValue())
                {
                    // 清除原有值
                    newEntry.ResetDefaultEV(dp, metadata->GetDefaultValue());
                }
            }
            else
            {
                //
                // 旧值为无效值，这里刷新旧值
                //
                if (newValue.get() == DpProperty::UnsetValue())
                {
                    /*if (oldBaseType >= BaseValueType::StyleTrigger && 
                        oldBaseType <= BaseValueType::ParentTemplateTrigger)
                    {
                        // 清除原有值
                        newEntry.ResetDefaultEV(dp, metadata->GetDefaultValue());
                    }
                    //
                    // 旧值是表达式或校正后的值
                    //
                    else */if (oldEntry.HasRevise())
                    {
                        Object* oldBase = oldEntry.GetReviseValue()->GetBaseValue();
                        oldExpr = RTTICast<Expression>(oldBase);

                        //
                        // 旧值为表达式，对表达式重新进行计算
                        //
                        if (oldExpr)
                        {
                            MemberVisitor::SetValue(&newEntry, oldBase);
                            EvaluateExpr(entryi, dp, oldExpr, metadata, newEntry);
                        }
                        else
                        {
                            // 恢复到之前存储的BaseValue
                            MemberVisitor::SetValue(&newEntry, oldBase);
                        }

                        newEntry.SetBaseValueType(oldEntry.GetBaseValueType());
                    }
                    else 
                    {
                        Object* oldLocal = oldEntry.GetLocalValue();
                        if (newEntry.GetBaseValueType() >= oldEntry.GetBaseValueType() || oldLocal == DpProperty::UnsetValue())
                        {
                            newEntry.ResetDefaultEV(dp, metadata->GetDefaultValue());
                        }
                        else
                        {
                            newEntry.ResetEV(dp, oldLocal, oldEntry.GetBaseValueType());
                        }
                    }
                }
                else
                {
                    bool exprHoldflag = false;
                    //
                    // 旧值为表达式，试着把新值缓存到表达式中
                    //
                    if (oldEntry.IsExpression())
                    {
                        Object* oldBase = oldEntry.GetReviseValue()->GetBaseValue();

                        if (oldEntry.GetValueSourceType() != newEntry.GetValueSourceType())
                        {
                            oldExpr = RTTICast<Expression>(oldBase);
                            exprHoldflag = oldExpr->SetValue(this, dp, newValue.get());

                            // 值缓存在表达式中
                            if (exprHoldflag)
                            {
                                newEntry.ResetDefaultEV(dp, oldExpr);
                                newEntry.SetExprValue(newValue.get(), oldExpr);
                                newEntry.SetBaseValueType(oldEntry.GetBaseValueType());
                            }
                        }
                    }
                }
            }
        }

        newEntry.SetPropertyAndIndex(dp, dp->GetIndex());
    }

    CheckEntryIndex(entryi, globalIndex);

    //
    // 如果不等于默认值则更新属性值 
    //
    if (newEntry.GetValueSourceType() != 1)
    {
        SetEV(entryi, dp, metadata, newEntry);
    }
    else
    {
        if (entryi.IsFound())
        {
            RemoveEV(entryi.GetIndex(), dp);
        }
    }

    int result = 0;
    bool isAValueChange = false;

    newValue = newEntry.GetFlattenedValue(RequestFlags::rfFullyResolved);

    if (newValue.get() != oldValue)
    {
        isAValueChange = !oldValue->Equals(newValue.get());
    }

    if (isAValueChange)
    {
        result |= 1;
    }

    if (isAValueChange)
    {
        // 
        // 值已经改变，触发事件
        //
        DpPropChangedEventArg e(dp, metadata, oldValue, newValue.get());
        NotifyDpPropertyChanged(&e);
    }

    return result;
}

void DpObject::OnDpPropertyChanged(DpPropChangedEventArg* e)
{
    PropChangedCallback& callbk = e->GetMetadata()->_dpPropChangedCallback;
    if (callbk.safe_bool())
    {
        callbk(this, e);
    }
}

Object* DpObject::ReadLocalValue(DpProperty* dp, bool allowDefer)
{
    VerifyAccess(__FILELINE__);

    if (dp == NULL)
    {
        throw ArgumentNullException(_U("dp"), __FILELINE__);
    }

    EntryIndex entry = FindEntryIndex(dp->GetIndex());
    return InternalReadLocalValue(entry, dp, false);
}

Object* DpObject::GetLocalValue(Uint32 index)
{
    return _effects[index].GetLocalValue();
}

Object* DpObject::InternalReadLocalValue(EntryIndex entryi, DpProperty* dp, bool allowDefer)
{
    if (!entryi.IsFound())
    {
        return DpProperty::UnsetValue();
    }

    EffectiveValue& entry = _effects[entryi.GetIndex()];
    Object* unsetValue = entry.IsCoercedWithCurrentValue() ? entry.GetReviseValue()->GetCoercedValue() : entry.GetLocalValue();
    
    if (!allowDefer && entry.IsDeferredReference())
    {
        DeferredRef* reference = DynamicCast<DeferredRef>(unsetValue);
        if (reference != NULL)
        {
            unsetValue = reference->GetValue(entry.GetBaseValueType());
        }
    }

    return unsetValue;
}

bool DpObject::InDesignMode() const
{
    return ReadDoFlag(1);
}

bool DpObject::ContainsValue(DpProperty* dp)
{
    EntryIndex index = FindEntryIndex(dp->GetIndex());
    if (!index.IsFound())
    {
        return false;
    }
    EffectiveValue& entry = _effects[index.GetIndex()];
    Object* objA = entry.IsCoercedWithCurrentValue() ? entry.GetReviseValue()->GetCoercedValue() : entry.GetLocalValue();
    return objA != DpProperty::UnsetValue();
}

void DpObject::ClearValueDirectly(DpProperty* dp)
{
    VerifyAccess(__FILELINE__);

    EntryIndex entry = FindEntryIndex(dp->GetIndex());
    if (entry.IsFound())
    {
        RemoveEV(entry.GetIndex(), dp);
    }
}

void DpObject::ClearValue(DpProperty* dp)
{
    VerifyAccess(__FILELINE__);

    PropMetadata* metadata = NULL;
    EntryIndex entry = FindEntryIndex(dp->GetIndex());
    InternalClearValue(entry, dp, metadata);
}

void DpObject::ClearValueFromStyle(DpProperty* dp)
{
    PropMetadata* metadata = NULL;
    EntryIndex entry = FindEntryIndex(dp->GetIndex());
    if (entry.IsFound())
    {
        EffectiveValue oldEntry;
        GetEV(entry, dp, metadata, RequestFlags::rfRawEntry, oldEntry);
        if (oldEntry.GetBaseValueType() != BaseValueType::btLocal)
        {
            EffectiveValue newEntry(dp, BaseValueType::btLocal);
            UpdateEV(entry, dp, oldEntry, newEntry, false, false, OperationType::otUnknown);
        }
    }
}

void DpObject::InternalClearValue(EntryIndex entry, DpProperty* dp, PropMetadata* metadata)
{
    EffectiveValue oldEntry;
    GetEV(entry, dp, metadata, RequestFlags::rfRawEntry, oldEntry);
    /*Object* localValue = oldEntry.GetLocalValue();
    Expression* expr = oldEntry.IsExpression() ? (RTTICast<Expression>(localValue)) : NULL;

    if (expr != NULL)
    {
        //DependencySource* sources = expr.GetSources();
        //UpdateSourceDependentLists(this, dp, sources, expr, false);
        //expr.OnDetach(this, dp);
        //expr.MarkDetached();
        CheckEntryIndex(entry, dp->GetIndex());
    }*/

    EffectiveValue newEntry(dp, BaseValueType::btLocal);
    UpdateEV(entry, dp, oldEntry, newEntry, false, false, OperationType::otUnknown);
}

void DpObject::GetFlattenedEntry(EntryIndex entryi, DpProperty* dp, RequestFlags requests, EffectiveValue& flattenedEntry)
{
    EffectiveValue& entry = _effects.GetItem(entryi.GetIndex());

    entry.FindFlattenedEntry(requests, flattenedEntry);

    if (((requests & (RequestFlags::rfRawEntry | RequestFlags::rfDeferredReferences)) == RequestFlags::rfFullyResolved) && flattenedEntry.IsDeferredReference())
    {
        if (!entry.HasRevise())
        {
            Object* obj3 = ((DeferredRef*)entry.GetValue())->GetValue(entry.GetBaseValueType());

            if (!dp->IsValidValue(this, obj3))
            {
                throw InvalidOperationException(_U("InvalidPropertyValue"), __FILELINE__);
            }

            CheckEntryIndex(entryi, dp->GetIndex());
            MemberVisitor::SetValue(&entry, obj3);
            flattenedEntry = entry;
            return;
        }

        ReviseValue* reviseValue = entry.GetReviseValue();
        DeferredRef* coercedValue = NULL;
        bool flag = false;

        if (entry.IsCoercedWithCurrentValue() && !entry.IsAnimated())
        {
            coercedValue = DynamicCast<DeferredRef>(reviseValue->GetCoercedValue());
        }

        if (((coercedValue == NULL) && entry.IsExpression()) && (!entry.IsAnimated() && !entry.IsCoerced()))
        {
            coercedValue = DynamicCast<DeferredRef>(reviseValue->GetExprValue());
            flag = true;
        }

        if (coercedValue == NULL)
        {
            return ;
        }

        Object* obj2 = coercedValue->GetValue(entry.GetBaseValueType());

        if (!dp->IsValidValue(this, obj2))
        {
            throw InvalidOperationException(_U("InvalidPropertyValue"), __FILELINE__);
        }

        CheckEntryIndex(entryi, dp->GetIndex());

        if (flag)
        {
            entry.SetExprValue(obj2, reviseValue->GetBaseValue());
        }
        else
        {
            entry.SetCoercedValue(obj2, entry.IsCoercedWithCurrentValue());
        }

        SETREFOBJ(flattenedEntry._value, obj2);
    }
}

void DpObject::GetEV(EntryIndex entryi, DpProperty* dp, PropMetadata*& metadata, RequestFlags requests, EffectiveValue& entry)
{
    if (entryi.IsFound())
    {
        if ((requests & RequestFlags::rfRawEntry) != RequestFlags::rfFullyResolved)
        {
            entry = _effects[entryi.GetIndex()];
        }
        else
        {
            GetFlattenedEntry(entryi, dp, requests, entry);
        }

        metadata = entry.GetMetadata();
    }
    else
    {
        entry.ResetEV(dp, DpProperty::UnsetValue(), BaseValueType::btUnknown);
    }

    if (entry.GetValue() != DpProperty::UnsetValue())
    {
        return;
    }

    if (metadata == NULL)
    {
        metadata = SetupPropMetadata(dp);//dp->GetMetadata(GetClassType());
    }

    if ((requests & RequestFlags::rfSkipDefault) != RequestFlags::rfFullyResolved)
    {
        entry.SetMetadata(metadata);
        return;
    }

    entry.ResetDefaultEV(dp, metadata->GetDefaultValue());
    entry.SetMetadata(metadata);
}

void DpObject::CheckEntryIndex(EntryIndex& entryi, int target)
{
    if (_effects.Length() > (int)entryi.GetIndex())
    {
        EffectiveValue& entry = _effects[entryi.GetIndex()];
        if (entry.GetPropertyIndex() == target)
        {
            entryi = EntryIndex(entryi.GetIndex());
            return;
        }
    }
    entryi = FindEntryIndex(target);
}

PropMetadata* DpObject::FindMetadata(EntryIndex& entryi, DpProperty* dp)
{
    if (!entryi.IsFound())
    {
        entryi = FindEntryIndex(dp->GetIndex());
    }

    if (entryi.IsFound())
    {
        return _effects[entryi.GetIndex()].GetMetadata();
    }
    else
    {
        return SetupPropMetadata(dp);
    }
}

EntryIndex DpObject::FindEntryIndex(int target)
{
    Uint32 index = 0;
    Uint32 effectiveValuesCount = _effects.Length();

    if (effectiveValuesCount > 0)
    {
        int propertyIndex = 0;

        while ((effectiveValuesCount - index) > 3)
        {
            Uint32 num4 = (effectiveValuesCount + index) / 2.0f;
            propertyIndex = _effects[num4].GetPropertyIndex();

            if (target == propertyIndex)
            {
                return EntryIndex(num4);
            }

            if (target <= propertyIndex)
            {
                effectiveValuesCount = num4;
            }
            else
            {
                index = num4 + 1;
            }
        }

    Label_Loop:

        propertyIndex = _effects[index].GetPropertyIndex();

        if (propertyIndex == target)
        {
            return EntryIndex(index);
        }

        if (propertyIndex <= target)
        {
            index++;

            if (index < effectiveValuesCount)
            {
                goto Label_Loop;
            }
        }

        return EntryIndex(index, false);
    }

    return EntryIndex(0, false);
}

void DpObject::SetEV(EntryIndex entryi, DpProperty* dp, int targetIndex, PropMetadata* metadata, Object* value, BaseValueType valueSource)
{
    EffectiveValue* entry = NULL;

    if (entryi.IsFound())
    {
        entry = &(_effects[entryi.GetIndex()]);
    }
    else
    {
        InsertEV(EffectiveValue(), entryi.GetIndex());
        entry = &(_effects[entryi.GetIndex()]);
        entry->SetPropertyAndIndex(dp, targetIndex);
    }

    if (entry->IsExpression() && (entry->GetReviseValue()->GetExprValue() == Expression::NoValue))
    {
        entry->SetExprValue(value, entry->GetReviseValue()->GetBaseValue());
    }
    else
    {
        entry->SetBaseValueType(valueSource);
        entry->ResetValue(value, 0);
    }
}

void DpObject::EvaluateExpr(EntryIndex entryi, DpProperty* dp, Expression* expr, PropMetadata* metadata, EffectiveValue& newEntry)
{
    ObjectPtr unsetValue;
    //bool enable = expr->_enabled;
    //bool autodel = expr->isAutoDelete();
    
    //expr->setAutoDelete(false);
    //expr->_enabled = false;
    expr->GetValue(this, dp, unsetValue);
    //expr->setAutoDelete(autodel);
    //expr->_enabled = enable;

    if (unsetValue.get() == DpProperty::UnsetValue())
    {
        unsetValue = metadata->GetDefaultValue();
    }
    else if (unsetValue.get() == Expression::NoValue)
    {
        unsetValue = metadata->GetDefaultValue();
    }

    newEntry.SetExprValue(unsetValue.get(), expr);
}

//void DpObject::EvaluateEffectiveValue(EntryIndex entryi, DpProperty* dp, PropMetadata* metadata, EffectiveValue& oldEntry, EffectiveValue& newEntry, OperationType operType)
//{
//    Expression* expr = NULL;
//    Object* unsetValue = DpProperty::UnsetValue();
//    bool newIsLocal = newEntry.GetBaseValueType() == BaseValueType::Local;
//    bool newIsUnset = newIsLocal && (newEntry.GetValue() == DpProperty::UnsetValue());
//
//    // 新值为本地值并且无效
//    // 这里可以允许用户重新定义其值，目前暂未实现
//    if (newIsUnset)
//    {
//        newEntry.SetBaseValueType(BaseValueType::Unknown);
//    }
//    else
//    {
//        // 优先取出本地值
//        // 如果旧值是一个表达式，则将会重新计算旧值表达式
//        unsetValue = newIsLocal ? newEntry.GetLocalValue() : oldEntry.GetLocalValue();
//        expr = RTTICast<Expression>(unsetValue);
//    }
//
//    if (unsetValue != DpProperty::UnsetValue())
//    {
//        newEntry.ResetEffectiveValue(dp, DpProperty::UnsetValue(), BaseValueType::Local);
//        MemberVisitor::SetValue(&newEntry, unsetValue);
//
//        if (expr != NULL)
//        {
//            EvaluateExpression(entryi, dp, expr, metadata, newEntry);
//            unsetValue = newEntry.GetReviseValue()->GetExprValue();
//        }
//    }
//    
//    if (!dp->IsReadOnly())
//    {
//        //EvaluateBaseValueCore(dp, metadata, newEntry);
//    }
//
//    // 到这里属性值仍然无效，则获取默认值
//    if (unsetValue == DpProperty::UnsetValue())
//    {
//        newEntry.ResetDefaultEffectiveValue(dp, metadata->GetDefaultValue());
//    }
//}

void DpObject::SetEV(EntryIndex entryi, DpProperty* dp, PropMetadata* metadata, EffectiveValue& newEntry)
{
    if (NULL == metadata)
    {
        newEntry.SetMetadata(SetupPropMetadata(dp));
    }
    else
    {
        newEntry.SetMetadata(metadata);
    }

    newEntry._source &= ~ValueSourceType::InnerUpdateFlag;

    if (entryi.IsFound())
    {
        _effects[entryi.GetIndex()] = newEntry;
    }
    else
    {
        InsertEV(newEntry, entryi.GetIndex());
    }
}

void EffectiveArray::Clear()
{
    for (int i = 0; i < GetCount(); ++i)
    {
        GetItem(i).Reset();
    }
    Array<EffectiveValue, true>::Clear();
    _inhritCount = 0;
}

void EffectiveArray::RemoveAt(int index, DpProperty* dp)
{
    Array<EffectiveValue, true>::RemoveAt(index);
    if (NULL == dp)
    {
    }
    else if (dp->GetDefaultMetadata()->IsInherited())
    {
        _inhritCount -= 1;
    }
}

void EffectiveArray::Insert(int index, const EffectiveValue& entry)
{
    Array<EffectiveValue, true>::Insert(index, entry);
    if (NULL == entry.GetProp())
    {
    }
    else if (entry.GetProp()->GetDefaultMetadata()->IsInherited())
    {
        _inhritCount += 1;
    }
}

void DpObject::InsertEV(EffectiveValue& entry, Uint32 index)
{
    if (_effects.GetCount() == 0)
    {
        _effects.Resize(8);
    }
    _effects.Insert(index, entry);

    /*if (_effects.GetInhritCount() == 1)
    {
        UpdateInheritanceContext(entry);
    }*/
}

void DpObject::RemoveEV(Uint32 index, DpProperty* dp)
{
    _effects.RemoveAt(index, dp);
}

void DpObject::UpdateInheritanceContext(const EffectiveValue& val)
{

}

void DpObject::RemoveInheritanceContext(const DpProperty* dp)
{

}

void DpObject::NotifyDpPropertyChanged(DpPropChangedEventArg* e)
{
    OnDpPropertyChanged(e);

    if (e->IsAValueChange())
    {
        DpProperty* directDpProperty = e->GetProp();
        if (directDpProperty != Element::DataContextProperty)
        {
            UpdateDepProp(directDpProperty);
        }
    }
}

void DpObject::Seal()
{
    _viewflag |= ViewFlags::IsObjectSealedVF;
}

DpObject* DpObject::GetContextParent() const
{
    return GetInheritParent();
}

void DpObject::SetAttribute(const String& key, Object* val)
{
}

}
