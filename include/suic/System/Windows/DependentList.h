// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����DependentList.h
// ��  �ܣ��������������Ե��б�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIDEPENDENTLIST_H_
#define _UIDEPENDENTLIST_H_

#include <System/Windows/Object.h>
#include <System/Windows/PropertyValue.h>
#include <System/Tools/CollectionX.h>

namespace suic
{

/// <summary>
/// DependentList
/// </summary>
class SUICORE_API DependentList : public Object
{
public:

    RTTIOfClass(DependentList)

    DependentList();
    ~DependentList();

    void Add(DpObject* d, DpProperty* dp, Expression* expr);
    void Remove(DpObject* d, DpProperty* dp, Expression* expr);
    
    void InvalidateDeps(DpObject* source, DpProperty* dp);

    bool IsEmpty();

private:

    void ClearDeadWeakRefs(bool doAll);

    Array<DpDependent*> _items;
    static int _skipper;
};

};

#endif
