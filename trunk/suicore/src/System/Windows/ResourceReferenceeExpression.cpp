
#include <System/Windows/FrameworkElement.h>
#include <System/Windows/ResourceReferenceExpression.h>

#include <System/Tools/DesignHelper.h>
#include <System/Tools/StyleHelper.h>
#include <System/Tools/VisualTreeOp.h>

#include <System/Interop/System.h>

namespace suic
{

ImplementRTTIOfClass(RRExpression, Expression)

RRExpression::RRExpression(const String& val)
{
    _resourceKey = val;
}

RRExpression::~RRExpression()
{

}

RRExpression* RRExpression::Clone()
{
    RRExpression* pExpr = new RRExpression(_resourceKey);
    return pExpr;
}

bool RRExpression::GetValue(DpObject* d, DpProperty* dp, ObjectPtr& val)
{
    FrameworkElement* fe = RTTICast<FrameworkElement>(d);

    if (fe == NULL)
    {
        fe = RTTICast<FrameworkElement>(VisualTreeOp::GetParent(d));
    }

    if (fe != NULL)
    {
        Object* resObj = fe->FindRes(_resourceKey);
        if (DpProperty::UnsetValue() != resObj)
        {
            val = resObj;
            return true;
        }
    }

    val = NoValue;

    return false;
}

bool RRExpression::SetValue(DpObject* d, DpProperty* dp, Object* value) 
{
    return false;
}

ARRExpression::ARRExpression(const String& name)
    : RRExpression(name)
{

}

ARRExpression::~ARRExpression()
{

}

Object* ARRExpression::GetValue(DpObject* d, DpProperty* dp)
{
    Application* pApp = NULL;

    if (d != NULL && d->InDesignMode())
    {
        pApp = DesignHelper::GetDesignApplication();
    }
    else
    {
        pApp = Application::Current();
    }
    
    if (pApp && pApp->GetResources())
    {
        return pApp->GetResources()->Lookup(GetResourceKey());
    }
    else
    {
        return DpProperty::UnsetValue();
    }
}

}
