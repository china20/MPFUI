// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
//
// ======================================================================

#include <System/Resources/TriggerCollection.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

TriggerCollection::TriggerCollection()
    : _owner(NULL)
    , _sealed(false)
{
    _triggers.Resize(4);
}

TriggerCollection::TriggerCollection(FrameworkElement* owner)
    : _owner(owner)
    , _sealed(false)
{
    _triggers.Resize(4);
}

TriggerCollection::~TriggerCollection()
{
    Clear();
}

int TriggerCollection::GetCount() const
{
    return _triggers.GetCount();
}

TriggerBase* TriggerCollection::GetTrigger(int index) const
{
    return _triggers[index];
}

void TriggerCollection::Clear()
{
    for (int i = 0; i < _triggers.GetCount(); ++i)
    {
        _triggers[i]->unref();
    }
    _triggers.Clear();
}

void TriggerCollection::AddTrigger(TriggerBase* trg)
{
    CheckTrigger(trg);
    trg->ref();
    _triggers.Add(trg);
}

void TriggerCollection::InsertTrigger(int index, TriggerBase* trg)
{ 
    CheckTrigger(trg);

    trg->ref();
    _triggers.Insert(index, trg);
}

void TriggerCollection::SetTrigger(int index, TriggerBase* trg)
{ 
    CheckTrigger(trg);

    trg->ref();
    _triggers[index]->unref();
    _triggers.SetItem(index, trg);
}

void TriggerCollection::CheckTrigger(TriggerBase* trigger)
{
    CheckSealed();
    TriggerValidation(trigger);
}

void TriggerCollection::Seal() 
{
    _sealed = true;

    for (int i = 0; i < GetCount(); i++)
    {
        TriggerBase* trigger = (TriggerBase*)GetTrigger(i);
        if (trigger)
        {
            trigger->Seal();
        }
    }
} 

FrameworkElement* TriggerCollection::GetOwner()
{
    return _owner;
}

bool TriggerCollection::CheckSealed()
{
    return !_sealed;
}

void TriggerCollection::TriggerValidation(TriggerBase* trigger)
{
    if (trigger == NULL)
    { 
        throw;
    }
}

}
