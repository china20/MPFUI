// 华勤科技版权所有 2010-2012
// 
// 文件名：TemplateBindingExpression.h
// 功  能：模版属性绑定实现，自动套用父模版属性。
// 
// 作  者：MPF开发组
// 时  间：2011-05-01
// 
// ============================================================================

#ifndef _UITEMPLATEBINDEXPRESSION_H_
#define _UITEMPLATEBINDEXPRESSION_H_

#include <System/Windows/Expression.h>
#include <System/Windows/Extension.h>

namespace suic
{

class TBExpression : public Expression
{
public:

    RTTIOfClass(TBExpression)

    TBExpression();
    TBExpression(TemplateBindingExtension* tempExten);

    ~TBExpression();

    DpProperty* GetProp() const;
    void SetProp(DpProperty* prop);
    String GetPropName() const;

    TBExpression* Clone();

protected:

    bool GetValue(DpObject* d, DpProperty* dp, ObjectPtr& val);
    bool SetValue(DpObject* d, DpProperty* dp, Object* value);

    friend class DpObject;

private:

    TemplateBindingExtension* _bindExtension;
};

inline DpProperty* TBExpression::GetProp() const
{
    if (NULL == _bindExtension)
    {
        return NULL;
    }
    else
    {
        return _bindExtension->GetProp();
    }
}

inline void TBExpression::SetProp(DpProperty* prop)
{
    SETREFOBJ(_bindExtension, new TemplateBindingExtension(prop));
}

inline String TBExpression::GetPropName() const
{
    return _T("");
}

} 

#endif
