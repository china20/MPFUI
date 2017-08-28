// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
//
// ======================================================================

#include <System/Resources/ResourceDictionary.h>
#include <System/Resources/DeferredResRef.h>
#include <System/Windows/FrameworkElement.h>

#include <System/Tools/TreeBuilder.h>

namespace suic
{

ImplementRTTIOfClass(ResourceDictionary, DpObject)

StringDic<ResourceDictionary*> ResourceDictionary::_caches;

ResourceDictionary::ResourceDictionary()
    : _owner(NULL)
    , _isReadOnly(false)
{
    _mergedDictionaries = new RDCollection();
}

ResourceDictionary::~ResourceDictionary()
{
    String strUri = _source.GenerateKey();
    if (!strUri.Empty())
    {
        _caches.Remove(strUri);
    }

    Clear();
    if (_mergedDictionaries)
    {
        delete _mergedDictionaries;
    }
}

ResourceDictionary* ResourceDictionary::FindResourceDictionary(const String& strUri)
{
    ResourceDictionary* resDic = NULL;
    String strKey;

    if (!strUri.Empty())
    {
        strKey = ResourceUri(strUri).GenerateKey();
        _caches.TryGetValue(strKey, resDic);
        if (NULL != resDic && resDic->getRefCount() <= 0)
        {
            _caches.Remove(strKey);
            resDic = NULL;
        }
    }

    return resDic;
}

void ResourceDictionary::Add(const String& key, Object* value)
{
    AddWithoutLock(key, value);
}

void ResourceDictionary::AddWithoutLock(const String& key, Object* value)
{
    if (IsReadOnly())
    {
        throw InvalidOperationException(_U("ResourceDictionaryIsReadOnly"), __FILELINE__);
    }

    _baseDictionary.Add(key, value);
    //NotifyOwners(new ResourcesChangeInfo(key));
}

Object* ResourceDictionary::Lookup(const String& key)
{
    bool cache = false;
    return GetValueWithoutLock(key, cache);
}

void ResourceDictionary::Clear()
{
    _baseDictionary.Clear();
}

bool ResourceDictionary::Contains(const String& key)
{
    return false;
}

void ResourceDictionary::Remove(const String& key)
{
    RemoveWithoutLock(key);
}

void ResourceDictionary::RemoveWithoutLock(const String& key)
{
    if (IsReadOnly())
    {
        throw InvalidOperationException(_U("ResourceDictionaryIsReadOnly"), __FILELINE__);
    }

    _baseDictionary.Remove(key);

    //NotifyOwners(new ResourcesChangeInfo(key));
}

Object* ResourceDictionary::GetValueWithoutLock(const String& key, bool& canCache)
{
    Object* obj2 = NULL;
    canCache = true;

    if (_mergedDictionaries != NULL)
    {
        for (int i = _mergedDictionaries->GetCount() - 1; i > -1; i--)
        {
            ResourceDictionary* dictionary = _mergedDictionaries->GetItem(i);
            if (dictionary != NULL)
            {
                obj2 = dictionary->GetValueWithoutLock(key, canCache);

                if (obj2 != DpProperty::UnsetValue())
                {
                    return obj2;
                }
            }
        }
    }

    if (_baseDictionary.TryGetValue(key, obj2) && obj2 != NULL)
    {
        return obj2;
    }

    return DpProperty::UnsetValue();
}

int ResourceDictionary::GetCount()
{
    return _baseDictionary.GetCount();
}

bool ResourceDictionary::LoadResourceDictionary(const String& uri)
{
    ObjectPtr root;

    root = Builder::BuildUri(this, uri);
    return (root.get() == this);
}

void ResourceDictionary::AddOwner(DpObject* owner)
{
    _owner = owner;
}

void ResourceDictionary::RemoveOwner(DpObject* owner)
{
    _owner = NULL;
}

bool ResourceDictionary::ContainsOwner(DpObject* owner)
{
    if (_owner == owner)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ResourceDictionary::ClearMergedDictionaries()
{
    _mergedDictionaries->Clear();
}

void ResourceDictionary::RemoveMergedDictionary(int index)
{
    if (index >= 0 && index < GetMergedDictionariesCount())
    {
        _mergedDictionaries->RemoveItemAt(index);
    }
}

void ResourceDictionary::RemoveMergedDictionaryRange(int iStart, int iCount)
{
    if (iStart < 0)
    {
        iStart = 0;
    }
    else if (iStart >= GetMergedDictionariesCount())
    {
        iStart = GetMergedDictionariesCount() - 1;
    }

    if (iCount > GetMergedDictionariesCount() - iStart)
    {
        iCount = GetMergedDictionariesCount() - iStart;
    }

    if (iCount < 0)
    {
        iCount = 0;
    }

    while (iCount > 0)
    {
        _mergedDictionaries->RemoveItemAt(iStart);
        --iCount;
    }
}

void ResourceDictionary::AddMergedDictionary(ResourceDictionary* val)
{
    if (NULL != val)
    {
        _mergedDictionaries->AddItem(val);
    }
}

int ResourceDictionary::GetMergedDictionariesCount()
{
    return _mergedDictionaries->GetCount();
}

ResourceDictionary* ResourceDictionary::GetMergedDictionary(int index)
{
    return _mergedDictionaries->GetItem(index);
}

int ResourceDictionary::IndexOfMergedDictionary(ResourceDictionary* resDic)
{
    for (int i = 0; i < _mergedDictionaries->GetCount(); i++)
    {
        if (GetMergedDictionary(i) == resDic)
        {
            return i; 
        }
    }
    return -1;
}

void ResourceDictionary::Seal()
{
    if (!IsSealed())
    {
        String strUri = _source.GenerateKey();
        if (!strUri.Empty())
        {
            _caches.Add(strUri, this);
        }

        DpObject::Seal();
    }
}

bool ResourceDictionary::ContainsCycle(ResourceDictionary* origin) 
{ 
    for (int i = 0; i < _mergedDictionaries->GetCount(); i++)
    { 
        ResourceDictionary* mergedDictionary = GetMergedDictionary(i);

        if (mergedDictionary == origin || mergedDictionary->ContainsCycle(origin))
        {
            return true; 
        }
    } 

    return false;
}

RDCollection::RDCollection()
    : AutoColl<ResourceDictionary>(8)
{

}

//======================================================
DepRecord::~DepRecord()
{
    childPropDeps.Clear();
}

DepRecord* TriggerDepRecord::GetDepRecord(int index)
{
    DepRecord* depRecord = NULL;
    depRecords.TryGetValue(index, depRecord);
    return depRecord;
}

DepRecord* TriggerDepRecord::AddDepRecord(int index)
{
    DepRecord* depRecord = NULL;
    if (!depRecords.TryGetValue(index, depRecord))
    {
        depRecord = new DepRecord();
        depRecords.Add(index, depRecord);
    }

    return depRecord;
}

TriggerDepRecords::TriggerDepRecords()
{
}

TriggerDepRecords::~TriggerDepRecords()
{
    trgDepRecords.Clear();
    trgDepRecord.depRecords.Clear();
}

TriggerDepRecord* TriggerDepRecords::GetTrgDepRecord(int index)
{
    if (0 == index)
    {
        return &trgDepRecord;
    }
    else
    {
        TriggerDepRecord* depRecord = NULL;
        trgDepRecords.TryGetValue(index - 1, depRecord);
        return depRecord;
    }
}

TriggerDepRecord* TriggerDepRecords::AddTrgDepRecord(int index)
{
    if (0 == index)
    {
        return &trgDepRecord;
    }
    else
    {
        --index;
        TriggerDepRecord* depRecord = NULL;
        if (!trgDepRecords.TryGetValue(index, depRecord))
        {
            depRecord = new TriggerDepRecord();
            trgDepRecords.Add(index, depRecord);
        }

        return depRecord;
    }
}

}
