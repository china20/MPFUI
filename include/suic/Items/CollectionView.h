// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����CollectionView.h
// ��  �ܣ�ItemsControl�������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UICOLLECTIONVIEWEX_H_
#define _UICOLLECTIONVIEWEX_H_

#include <System/Windows/Binding.h>
#include <Framework/Items/Enumerable.h>

namespace suic
{

class NotifyCollChanged
{
public:

    virtual ~NotifyCollChanged() {}
    NotifyCollChangedEventHandler CollChanged;
};

class CollectionView : public NotifyCollChanged
{
public:

    virtual bool IsEmpty() { return false; }
    virtual IList* GetView() = 0;
};

}

#endif
