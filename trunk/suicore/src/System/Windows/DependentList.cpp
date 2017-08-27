

#include <System/Windows/DependentList.h>

namespace suic
{

int DependentList::_skipper = 0;

ImplementRTTIOfClass(DependentList, XArray)
ImplementRTTIOfClass(DpDependent, Object)

DependentList::DependentList()
{
}

DependentList::~DependentList()
{
    for (int i = 0; i < _items.GetCount(); ++i)
    {
        _items[i]->unref();
    }
    _items.Clear();
}

void DependentList::Add(DpObject* d, DpProperty* dp, Expression* expr)
{
    if ((++_skipper % (1 + (_items.GetCount() / 4))) == 0)
    {
        ClearDeadWeakRefs(true);
    }

    if (NULL != dp)
    {
        DpDependent* dependent = new DpDependent(d, dp, expr);
        dependent->ref();
        _items.Add(dependent);
    }
}

void DependentList::InvalidateDeps(DpObject* source, DpProperty* dp)
{
    for (int i = 0; i < _items.GetCount(); ++i)
    {
        DpDependent* dpDep = _items.GetItem(i);
        Expression* expr = dpDep->GetExpr();
        DpObject* d = dpDep->GetDO();

        if (expr != NULL && NULL != d)
        {
            if (expr->IsEnabled())
            {
                expr->SetEnabled(false);
                if (expr->OnUpdateProp(source, dpDep->GetDP()))
                {
                    d->InvalidateProp(dpDep->GetDP());
                }
                expr->SetEnabled(true);
            }
        }
        else
        {
            dpDep->unref();
            _items.RemoveAt(i);
            --i;
        }
    }
}

void DependentList::Remove(DpObject* d, DpProperty* dp, Expression* expr)
{
    DpDependent dependent(d, dp, expr);
    dependent.setAutoDelete(false);

    for (int i = 0; i < _items.GetCount(); ++i)
    {
        DpDependent* dpDep = _items.GetItem(i);
        if (dpDep && dpDep->Equals(&dependent))
        {
            dpDep->unref();
            _items.RemoveAt(i);
            break;
        }
    }
}

bool DependentList::IsEmpty()
{
    if (_items.GetCount() == 0)
    {
        return true;
    }

    ClearDeadWeakRefs(false);

    return (0 == _items.GetCount());
}

void DependentList::ClearDeadWeakRefs(bool doAll)
{
    if (_items.GetCount() != 0)
    {
        for (int i = _items.GetCount() - 1; i >= 0; --i)
        {
            DpDependent* dpDep = _items.GetItem(i);
            if (dpDep && !dpDep->IsValid())
            {
                dpDep->unref();
                _items.RemoveAt(i);
            }
            else if (!doAll)
            {
                return;
            }
        }
    }
}

}

