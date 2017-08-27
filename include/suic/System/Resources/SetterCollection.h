// 华勤科技版权所有 2008-2022
// 
// 文件名：SetterCollection.h
// 功  能：Setters集合的访问实现。
// 
// 作  者：汪荣
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UISETTERCOLLECTION_H_
#define _UISETTERCOLLECTION_H_

#include <System/Resources/Setter.h>
#include <System/Resources/ObjColl.h>

namespace suic
{

class SUICORE_API SetterCollection
{
private:

    bool _sealed;

    Array<Setter*> _setters;
    ValueDic<int, Setter*> _cached;

public:

    SetterCollection();
    ~SetterCollection();

    void AddSetter(Setter* item);
    void InsertSetter(int index, Setter* item);
    void RemoveSetter(int index);
    void SetSetter(int index, Setter* item);

    void Clear();

    //-----------------------------------------

    int GetCount() const;
    Setter* GetSetter(int index) const;

    bool IsSealed();
    void Seal();

    bool CheckSealed();
    void SetterValidation(Setter* setter);
    Setter* GetSetter(DpProperty* dp);

private:

    void CachedItem(Setter* item);
    void CheckSetter(Setter* setter);

    void RemoveCacheItem(int index);
};

inline bool SetterCollection::IsSealed()
{
    return _sealed;
}

}

#endif
