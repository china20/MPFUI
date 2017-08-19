// ���ڿƼ���Ȩ���� 2010-2012
// 
// �ļ�����TemplateBindingExpression.h
// ��  �ܣ�ģ�����԰�ʵ�֣��Զ����ø�ģ�����ԡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-05-01
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
