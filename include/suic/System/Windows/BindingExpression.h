// 华勤科技版权所有 2010-2012
// 
// 文件名：BindingExpression.h
// 功  能：依赖属性的基类，完成依赖属性实现等逻辑。
// 
// 作  者：MPF开发组
// 时  间：2011-05-01
// 
// ============================================================================

#ifndef _UIBINDINGEXPRESSION_H_
#define _UIBINDINGEXPRESSION_H_

#include <System/Tools/Array.h>
#include <System/Windows/Binding.h>
#include <System/Windows/Expression.h>

namespace suic
{

class BindingExpression : public Expression
{
public:

    enum AttachAttempt
    {
        First,
        Again,
        Last,
    };

    RTTIOfClass(BindingExpression)

    BindingExpression();
    ~BindingExpression();

    BindingExpression* Clone();

    BindingStatus GetStatus() const;

    Binding* GetBinding() const;
    void SetBinding(Binding* bind);

    DpObject* GetTargetElement();
    DpProperty* GetTargetProperty();

    void Attach(DpObject* d);
    void Attach(DpObject* target, DpProperty* dp);

    void AttachToContext(AttachAttempt attempt);

    virtual void UpdateSource();
    virtual void UpdateTarget();

protected:

    void SetStatus(BindingStatus status);

    bool GetBindingData(DpProperty* dp, ObjectPtr& val);

    bool GetValue(DpObject* d, DpProperty* dp, ObjectPtr& val);
    bool SetValue(DpObject* d, DpProperty* dp, Object* value);
    
    bool OnUpdateProp(DpObject* d, DpProperty* dp);
    bool OnUpdateDataContext(DpObject* d, Object* data);

    void OnAttach(DpObject* d, DpProperty* dp);
    void OnDetach(DpObject* d, DpProperty* dp);

    friend class DpObject;

private:

    // 缓存值
    Object* _value;
    Binding* _binding;
    bool _attaching;

    WeakRef _target;
    DpProperty* _targetDp;

    WeakRef _dataItem;
    WeakRef _ctxElement;

    BindingStatus _status;
    DpProperty* _pathCache;

    Array<DpProperty*, false> _ppaths;
};

inline Binding* BindingExpression::GetBinding() const
{
    return _binding;
}

inline void BindingExpression::SetBinding(Binding* bind)
{
    SETREFOBJ(_binding, bind);
    _attaching = false;
}

inline BindingStatus BindingExpression::GetStatus() const
{
    return _status;
}

} 

#endif
