// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����TriggerCollection.h
// ��  �ܣ�Trigger���ϵķ���ʵ�֡�
// 
// ��  �ߣ�����
// ʱ  �䣺2011-08-02
// 
// ============================================================================

#ifndef _UITRIGGERCOLLECTION_H_
#define _UITRIGGERCOLLECTION_H_

#include <System/Resources/Trigger.h>
#include <System/Resources/ObjColl.h>

namespace suic
{

class FrameworkElement;

class SUICORE_API TriggerCollection
{
private:

    bool _sealed;
    
    FrameworkElement* _owner;
    Array<TriggerBase*> _triggers;

public:

    TriggerCollection();
    TriggerCollection(FrameworkElement* owner);

    ~TriggerCollection();

    bool IsSealed();
    void Seal();

    int GetCount() const;
    TriggerBase* GetTrigger(int index) const;

    void Clear();

    void AddTrigger(TriggerBase* trg);
    void InsertTrigger(int index, TriggerBase* trg);
    void SetTrigger(int index, TriggerBase* trg);

    FrameworkElement* GetOwner();
    bool CheckSealed();

    void TriggerValidation(TriggerBase* trigger);

protected:

    void CheckTrigger(TriggerBase* trigger);
};

inline bool TriggerCollection::IsSealed()
{
    return _sealed;
}

}

#endif
