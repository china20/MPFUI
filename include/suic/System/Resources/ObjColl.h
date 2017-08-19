// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ObjColl.h
// ��  �ܣ�Object���ϵķ���ʵ�֡�
// 
// ��  �ߣ�����
// ʱ  �䣺2011-08-02
// 
// ============================================================================

#ifndef _UIOBJCOLL_H_
#define _UIOBJCOLL_H_

#include <System/Windows/Object.h>
#include <System/Tools/Collection.h>

namespace suic
{

class SUICORE_API ObjColl
{
private:

    Array<Object*> _items;

public:

    ObjColl();
    ~ObjColl();

    int IndexOf(Object* val);
    bool Contains(Object* value);
    
    int GetCount() const;
    Object* GetItem(int index) const;

    virtual void AddItem(Object* item);
    virtual void InsertItem(int index, Object* item);
    virtual void RemoveItem(int index);
    virtual void SetItem(int index, Object* item);

    virtual void Clear();

private:

    void Attach(Object* item);
    void Detach(Object* item);
};

}

#endif
