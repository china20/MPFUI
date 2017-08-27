
#include <System/Windows/FrameworkElement.h>
#include <System/Windows/BindingExpression.h>
#include <System/Windows/MemberVisitor.h>

#include <System/Tools/ResourceParser.h>
#include <System/Tools/StyleHelper.h>

#include <Framework/Controls/Content.h>
#include <Framework/Presenter/ContentPresenter.h>

namespace suic
{

ImplementRTTIOfClass(Expression, Object)
ImplementRTTIOfClass(BindingExpression, Expression)

BindingExpression::BindingExpression()
{
    _value = Expression::NoValue;
    _value->ref();
    _binding = NULL;
    _targetDp = NULL;
    _attaching = false;
    _pathCache = NULL;

    _status = BindingStatus::Unattached;
}

BindingExpression::~BindingExpression()
{
    if (_binding)
    {
        _binding->unref();
    }
    if (_value)
    {
        _value->unref();
    }
}

void BindingExpression::Attach(DpObject* d)
{

}

void BindingExpression::Attach(DpObject* target, DpProperty* dp)
{
    _target = WeakRef(target);
    _targetDp = dp;

    AttachToContext(AttachAttempt::First);

    if (GetStatus() == BindingStatus::Unattached)
    {
        Assigner::Current()->GetBindingWorker()->AddTask(this);
    }
}

void BindingExpression::OnAttach(DpObject* d, DpProperty* dp)
{
    Attach(d, dp);
}

void BindingExpression::OnDetach(DpObject* d, DpProperty* dp)
{

}

void BindingExpression::UpdateSource()
{

}

void BindingExpression::UpdateTarget()
{
    ;
}

void BindingExpression::AttachToContext(AttachAttempt attempt)
{
    bool bHasSource = true;
    DpObject* target = GetTargetElement();
    DpObject* current = _binding->GetSource();
    Object* dataItem = NULL;
    SourceRef* srcRef = _binding->GetSourceRef();

    _attaching = true;

    if (NULL == current)
    {
        // 明确指定了Source
        if (srcRef != NULL)
        {
            current = RTTICast<DpObject>(srcRef->GetSource(target, _targetDp, attempt == AttachAttempt::First));

            if (NULL != current)
            {
                SetStatus(BindingStatus::Active);
            }
            else if (attempt != AttachAttempt::First)
            {
                SetStatus(BindingStatus::PathError);
            }
        }
        else
        {
            FrameworkElement* fe = RTTICast<FrameworkElement>(target);
            DpObject* tfe = fe->GetTemplatedParent();

            // 如果是控件模版，则绑定到父模版
            if (tfe != NULL)
            {
                current = tfe;
                SetStatus(BindingStatus::Active);
            }
            else
            {
                // 否则向上查找第一个DataContext不为空的元素
                current = target;
            }

            bHasSource = false;
            _binding->SetXPath(_binding->GetPath().Path);
        }
    }
    else
    {
        SetStatus(BindingStatus::Active);
    }

    // 如果数据源不为空
    if (NULL != current)
    {
        if (bHasSource)
        {
            DpProperty* dp = NULL;
            DpObject* dObj = NULL;
            StringArray vec;
            String strVal;

            dObj = current;
            vec.SplitString(GetBinding()->GetPath().Path, _U("."));

            for (int i = 0; i < vec.Length(); ++i)
            {
                strVal = vec[i];
                strVal.Trim();

                if (i == 0 && strVal.IndexOf(_U("TemplatedParent")) == 0)
                {
                    FrameworkElement* fe = RTTICast<FrameworkElement>(dObj);
                    if (NULL == fe)
                    {
                        break;
                    }
                    current = fe->GetTemplatedParent();
                    dObj = current;
                    continue;
                }

                DpProperty* dp = DpProperty::Lookup(strVal, dObj->GetRTTIType());
                if (NULL == dp)
                {
                    break;
                }

                if (_pathCache == NULL)
                {
                    _pathCache = dp;
                }
                else
                {
                    _ppaths.Add(dp);
                }
                
                dObj = RTTICast<DpObject>(dObj->GetValue(dp));
                if (NULL == dObj)
                {
                    break;
                }
            }
        }
        else
        {
            Element* feObj = RTTICast<Element>(current);
            if (NULL != feObj)
            {
                dataItem = feObj->GetDataContext();
            }
            else
            {
                dataItem = current->GetValue(Element::DataContextProperty);
            }

            if (DpProperty::UnsetValue() != dataItem && NULL != dataItem)
            {
                _dataItem = WeakRef(dataItem);
            }
            
            // 对于数据绑定，不再进行二次检测
            SetStatus(BindingStatus::Active);
        }

        if (!bHasSource)
        {
            StyleHelper::UpdateBindingSource(current, Element::DataContextProperty, target, _targetDp, this, true);
        }
        else if (NULL != current)
        {
            _ctxElement = WeakRef(current);

            if (NULL != _pathCache)
            {
                SetStatus(BindingStatus::Active);
                StyleHelper::UpdateBindingSource(current, _pathCache, target, _targetDp, this, true);
            }
            if (attempt != AttachAttempt::First)
            {
                target->InvalidateProp(_targetDp);
            }
        }
    }

    _attaching = false;
}

void BindingExpression::SetStatus(BindingStatus status)
{
    _status = status;
}

DpObject* BindingExpression::GetTargetElement()
{
    return RTTICast<DpObject>(_target.GetTarget());
}

DpProperty* BindingExpression::GetTargetProperty()
{
    return _targetDp;
}

BindingExpression* BindingExpression::Clone()
{
    BindingExpression* pExpr = new BindingExpression();
    return pExpr;
}

bool BindingExpression::GetBindingData(DpProperty* dp, ObjectPtr& val)
{
    bool bFind = false;

    if (NULL != _binding && NULL == _binding->GetSourceRef())
    {
        Object* dataCtx = _dataItem.GetTarget();
        if (dataCtx)
        {
            if (_binding->GetXPath().Empty())
            {
                val = new OString(dataCtx->ToString());
                bFind = true;
            }
            else
            {
                bFind = MemberVisitor::GetMember(dataCtx, _binding->GetXPath(), val);
            }
        }

        if (bFind)
        {
            IBindConverter* cvtBind = _binding->GetConverter();
            if (NULL != cvtBind)
            {
                val = cvtBind->Convert(val.get(), _binding->GetConverterParam());
            }
        }
    }

    if (!bFind || val.get() == NULL)
    {
        val = Expression::NoValue;
    }

    return bFind;
}

bool BindingExpression::GetValue(DpObject* d, DpProperty* dp, ObjectPtr& val)
{
    DpObject* dO = RTTICast<DpObject>(_ctxElement.GetTarget());

    if (_pathCache != NULL && dO != NULL)
    {
        val = dO->GetValue(_pathCache);

        if (_ppaths.Length() > 0)
        {
            int iPropIndex = _ppaths.Length() - 1;
            DpObject* dObj = RTTICast<DpObject>(val.get());

            for (int i = 0; i < iPropIndex; ++i)
            {
                if (NULL == dObj)
                {
                    break;
                }
                dObj = RTTICast<DpObject>(dObj->GetValue(_ppaths[i]));
            }

            if (dObj)
            {
                val = dObj->GetValue(_ppaths[iPropIndex]);
            }
            else
            {
                val = DpProperty::UnsetValue();
            }
        }

        if (val.get() != DpProperty::UnsetValue() && NULL != _binding)
        {
            IBindConverter* cvtBind = _binding->GetConverter();
            if (NULL != cvtBind)
            {
                val = cvtBind->Convert(val.get(), _binding->GetConverterParam());
            }
            return true;
        }
    }
    else if (_dataItem.IsAlive())
    {
        return GetBindingData(dp, val);
    }
    
    val = _value;

    return false;
}

bool BindingExpression::SetValue(DpObject* d, DpProperty* dp, Object* val)
{
    Expression* expr = RTTICast<Expression>(val);
    bool bRet = false;
    DpObject* dO = RTTICast<DpObject>(_ctxElement.GetTarget());

    if (NULL != expr || !IsEnabled())
    {
        return false;
    }

    SetEnabled(false);

    if (_pathCache != NULL && dO != NULL)
    {
        dO->SetValue(_pathCache, val);
        bRet = true;
    }
    else if (_dataItem.IsAlive())
    {
        if (_binding)
        {
            Object* dataCtx = _dataItem.GetTarget();//dO->GetValue(Element::DataContextProperty);

            if (dataCtx && _binding->GetBindMode() >= BindingMode::TwoWay)
            {
                IBindConverter* cvtBind = _binding->GetConverter();
                if (NULL != cvtBind)
                {
                    ObjectPtr outVal;
                    outVal = cvtBind->ConvertBack(val, _binding->GetConverterParam());
                    bRet = MemberVisitor::SetMember(dataCtx, _binding->GetXPath(), outVal.get());
                }
                else
                {
                    bRet = MemberVisitor::SetMember(dataCtx, _binding->GetXPath(), val);
                }

                if (!bRet)
                {
                    SetEnabled(true);
                    throw InvalidOperationException(L"Binding SetValue", String().Format(L"dont find method[%s] called", _binding->GetXPath().c_str()), __FILELINE__);
                }
            }
        }
    }

    if (!bRet)
    {
        SETREFOBJ(_value, Expression::NoValue);
    }

    SetEnabled(true);

    return bRet;
}

bool BindingExpression::OnUpdateDataContext(DpObject* d, Object* data)
{
    if (_binding != NULL)
    {
        if (_binding->GetBindMode() <= BindingMode::TwoWay)
        {
            if (!_attaching)
            {
                SETREFOBJ(_value, Expression::NoValue);
                // 更新数据源
                _dataItem = WeakRef(data);
                return true;
            }
        }
    }
    return false;
}

bool BindingExpression::OnUpdateProp(DpObject* d, DpProperty* dp)
{
    bool bSucc = false;
    if (_binding != NULL)
    {
        if (_binding->GetBindMode() > BindingMode::TwoWay)
        {
            return false;
        }
        else if (_binding->GetUpdateSourceTrigger() == UpdateSourceTrigger::ustLostFocus)
        {
            Element* elem = RTTICast<Element>(d);
            if (elem != NULL && !elem->IsFocused())
            {
                return false;
            }
        }

        if (!_attaching)
        {
            SETREFOBJ(_value, Expression::NoValue);
            // 更新数据源
            Element* dO = RTTICast<Element>(d);
            if (NULL != dO)
            {
                bSucc = true;
                _dataItem = dO->GetDataContext();
            }
        }
    }
    else
    {
        bSucc = true;
    }

    return bSucc;
}

}
