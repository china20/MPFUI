// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����HierarchicalItem.h
// ��  �ܣ�������Ŀ�������¼��ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIHIERARCHICALITEM_H_
#define _UIHIERARCHICALITEM_H_

#include <System/Windows/Delegate.h>
#include <System/Windows/EventArg.h>
#include <System/Windows/Binding.h>

#include <Framework/Controls/ItemSource.h>

namespace suic
{

/// <summary>
/// ��νṹ�м�ڵ����������
/// </summary>
class SUICORE_API HierarchicalItem : public suic::NotifyPropChanged
{
public:

    BeginMember(HierarchicalItem, suic::NotifyPropChanged)
        MemberBool(Expanded)
    EndMember()

    RTTIOfClass(HierarchicalItem)

    HierarchicalItem();
    virtual ~HierarchicalItem();

    HierarchicalItem* GetParent();

    Int16 GetHeaderSize() const;
    void SetHeaderSize(Int16 size);
    bool GetExpanded();

    void SetExpanded(bool val);

    void HandleItemRemoved();
    ItemCollection* GetChildren();

    virtual bool IsLeafItem();

protected:

    virtual void OnItemsChanged(NotifyCollChangedEventArg* e);
    void OnItemsChanged(Object* sender, NotifyCollChangedEventArg* e);

protected:

    Int16 _size;
    Byte _flag;
    WeakRef _parent;
    ItemCollection* _children;
};

inline HierarchicalItem* HierarchicalItem::GetParent()
{
    return _parent.GetElement<HierarchicalItem>();
}

inline Int16 HierarchicalItem::GetHeaderSize() const
{
    return _size;
}

inline void HierarchicalItem::SetHeaderSize(Int16 size)
{
    _size = size;
}

inline bool HierarchicalItem::GetExpanded()
{
    return (_flag & 1) == 1;
}

inline void HierarchicalItem::SetExpanded(bool val)
{
    if (val)
    {
        _flag |= 1;
    }
    else
    {
        _flag &= ~1;
    }
}

inline bool HierarchicalItem::IsLeafItem()
{
    return false;
}

}

#endif
