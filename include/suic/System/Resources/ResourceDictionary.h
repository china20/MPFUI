// 华勤科技版权所有 2008-2022
// 
// 文件名：ResourceDictionary.h
// 功  能：ResourceDictionary的访问实现。
// 
// 作  者：汪荣
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UIRESOURCEDICTIONARY_H_
#define _UIRESOURCEDICTIONARY_H_

#include <System/Windows/ResourceUri.h>
#include <System/Resources/SetterCollection.h>
#include <System/Resources/TriggerCollection.h>

namespace suic
{

class FrameworkElement;
class ResourceDictionary;

struct ChildBindDep
{
    int Index;
    String Name;
    Object* Value;
    DpProperty* Prop;
};

struct DepRecord
{
    DeleteColl<ChildPropDep> childPropDeps;

    ~DepRecord();

    void Add(ChildPropDep* cpDep)
    {
        childPropDeps.AddItem(cpDep);
    }
};

struct TriggerDepRecord
{
    DeleteDic<int, DepRecord, ValComparer<int> > depRecords;

    DepRecord* GetDepRecord(int index);
    DepRecord* AddDepRecord(int index);
};

struct TriggerDepRecords
{
    TriggerDepRecord trgDepRecord;
    DeleteDic<int, TriggerDepRecord, ValComparer<int> > trgDepRecords;

    TriggerDepRecords();
    ~TriggerDepRecords();

    TriggerDepRecord* GetTrgDepRecord(int index);
    TriggerDepRecord* AddTrgDepRecord(int index);
};

struct BindDepRecord
{
    DeleteColl<ChildBindDep> bindRecords;
};

class SUICORE_API RDCollection : public AutoColl<ResourceDictionary>
{
public:

    RDCollection();
};

class SUICORE_API ResourceDictionary : public DpObject
{
private:

    bool _isReadOnly;
    DpObject* _owner;
    ResourceUri _source;
    RDCollection* _mergedDictionaries;
    StringAutoDic<Object> _baseDictionary;

    static StringDic<ResourceDictionary*> _caches;

public:

    ResourceDictionary();
    ~ResourceDictionary();

    RTTIOfClass(ResourceDictionary)

    ResourceUri GetSource() const;
    void SetSource(const ResourceUri& source);
   
    bool IsReadOnly();
    void SetIsReadOnly(bool val);

    void Add(const String& key, Object* value);
    Object* Lookup(const String& key);

    void Clear();

    bool Contains(const String& key);
    void Remove(const String& key);

    int GetCount();

    bool LoadResourceDictionary(const String& uri);

    void AddOwner(DpObject* owner);
    void RemoveOwner(DpObject* owner);
    bool ContainsOwner(DpObject* owner);

    void ClearMergedDictionaries();
    void RemoveMergedDictionary(int index);
    void RemoveMergedDictionaryRange(int iStart, int iCount);
    void AddMergedDictionary(ResourceDictionary* val);
    
    int GetMergedDictionariesCount();
    ResourceDictionary* GetMergedDictionary(int index);

    int IndexOfMergedDictionary(ResourceDictionary* resDic);

    void Seal();

    static ResourceDictionary* FindResourceDictionary(const String& strUri);

protected:

    bool ContainsCycle(ResourceDictionary* origin);
    void RemoveWithoutLock(const String& key);
    void AddWithoutLock(const String& key, Object* value);
    Object* GetValueWithoutLock(const String& key, bool& canCache);
};

typedef shared<ResourceDictionary> ResourceDictionaryPtr;

inline ResourceUri ResourceDictionary::GetSource() const
{
    return _source;
}

inline void ResourceDictionary::SetSource(const ResourceUri& source)
{
    _source = source;
}

inline bool ResourceDictionary::IsReadOnly()
{
    return _isReadOnly;
}

inline void ResourceDictionary::SetIsReadOnly(bool val)
{
    _isReadOnly = val;
}

}

#endif
