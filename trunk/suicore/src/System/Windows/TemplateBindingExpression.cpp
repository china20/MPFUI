
#include <System/Windows/FrameworkElement.h>
#include <System/Windows/TemplateBindingExpression.h>

#include <System/Tools/StyleHelper.h>
#include <System/Tools/VisualTreeOp.h>

#include <System/Interop/System.h>

namespace suic
{

ImplementRTTIOfClass(TBExpression, Expression)

TBExpression::TBExpression()
{
    _bindExtension = NULL;
}

TBExpression::TBExpression(TemplateBindingExtension* tempExten)
{
    _bindExtension = tempExten;
    ADDREFOBJ(_bindExtension);
}

TBExpression::~TBExpression()
{
    FREEREFOBJ(_bindExtension);
}

TBExpression* TBExpression::Clone()
{
    TBExpression* pExpr = new TBExpression(_bindExtension);
    return pExpr;
}

bool TBExpression::GetValue(DpObject* d, DpProperty* dp, ObjectPtr& val)
{
    bool bFind = false;
    FrameworkElement* tfe = NULL;
    FrameworkElement* fe = RTTICast<FrameworkElement>(d);

    if (fe == NULL)
    {
        fe = RTTICast<FrameworkElement>(VisualTreeOp::GetParent(d));
    }

    if (NULL != fe)
    {
        tfe = RTTICast<FrameworkElement>(fe->GetTemplatedParent());
 
        if (tfe)
        {
            val = tfe->GetValue(GetProp());
            if (val.get() == DpProperty::UnsetValue())
            {
                val = NoValue;
            }
            else
            {
                bFind = true;
            }
        }
        else
        {
            val = NoValue;
        }
    }
    else
    {
        val = NoValue;
    }

    return bFind;
}

bool TBExpression::SetValue(DpObject* d, DpProperty* dp, Object* value) 
{
    return false;
}

}
