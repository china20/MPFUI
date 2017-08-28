// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
//
// ======================================================================

#include <System/Resources/SetterCollection.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

SetterCollection::SetterCollection()
{
    _setters.Resize(8);
}

SetterCollection::~SetterCollection()
{
    Clear();
}

void SetterCollection::CachedItem(Setter* item)
{
    if (item->GetProp() && item->GetTargetName().Empty())
    {
        _cached.Add(item->GetProp()->GetIndex(), item);
    }
}

void SetterCollection::CheckSetter(Setter* setter)
{
    CheckSealed();
    SetterValidation(setter);
}

void SetterCollection::RemoveCacheItem(int index) 
{
    Setter* item = GetSetter(index);
    _cached.Remove(item->GetProp()->GetIndex());
}

void SetterCollection::AddSetter(Setter* item)
{
    CheckSetter(item);
    item->ref();
    _setters.Add(item);
}

void SetterCollection::InsertSetter(int index, Setter* item)
{ 
    CheckSetter(item);
    item->ref();
    _setters.Insert(index, item);
}

void SetterCollection::RemoveSetter(int index) 
{
    if (!IsSealed())
    {
        RemoveCacheItem(index);
        Setter* setter = GetSetter(index);
        setter->unref();
        _setters.RemoveAt(index);
    }
}

void SetterCollection::SetSetter(int index, Setter* item) 
{
    Setter* setter = GetSetter(index);
    if (item != setter)
    {
        RemoveCacheItem(index);
        CheckSetter(item);

        setter->unref();
        item->ref();
        _setters.SetItem(index, item);
    }
} 

void SetterCollection::Clear()
{
    _cached.Clear();
    for (int i = 0; i < GetCount(); ++i)
    {
        Setter* setter = GetSetter(i);
        setter->unref();
    }
    _setters.Clear();
}

int SetterCollection::GetCount() const
{
    return _setters.GetCount();
}

Setter* SetterCollection::GetSetter(int index) const
{
    return _setters[index];
}

void SetterCollection::Seal() 
{
    _sealed = true; 

    for (int i = 0; i < GetCount(); i++) 
    {
        Setter* setter = GetSetter(i);
        setter->Seal();
    }
} 

bool SetterCollection::CheckSealed()
{
    return !_sealed;
}

void SetterCollection::SetterValidation(Setter* setter) 
{
    if (setter == NULL)
    {
        throw; 
    }
}

Setter* SetterCollection::GetSetter(DpProperty* dp)
{
    Setter* pSetter = NULL;

    if (_cached.TryGetValue(dp->GetIndex(), pSetter))
    {
        return pSetter;
    }

    for (int i = 0; i < GetCount(); ++i)
    {
        Setter* setter = GetSetter(i);
        if (setter->GetProp() == dp)
        {
            CachedItem(setter);
            return setter;
        }
    }

    return NULL;
}

}
