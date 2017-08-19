// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����SelectionChangedEventArg.h
// ��  �ܣ�������Ŀ������ѡ��仯�¼��ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UISELECTIONCHANGEDEVENTARG_H_
#define _UISELECTIONCHANGEDEVENTARG_H_

#include <System/Windows/Delegate.h>
#include <System/Windows/RoutedEventArg.h>

namespace suic
{

class SelectionChangedEventArg;
typedef delegate<void(Element*, SelectionChangedEventArg*)> SelectionChangedEventHandler;

class SelectionChangedEventArg : public RoutedEventArg
{
public:
    
    SelectionChangedEventArg(XArray* rItems, XArray* aItems)
    {
        _addItems = aItems;
        _removeItems = rItems;
    }

    SelectionChangedEventArg(RoutedEvent* e, XArray* rItems, XArray* aItems)
        : RoutedEventArg(NULL, e)
    {
        _addItems = aItems;
        _removeItems = rItems;
    }

    XArray* GetAddedItems() { return _addItems; }
    XArray* GetRemovedItems() { return _removeItems; }

    void CallEventHandler(Object* handler, Object* target)
    {
        InternalCall<Element, SelectionChangedEventHandler, SelectionChangedEventArg>(handler, target);
    }

private:

    XArray* _addItems;
    XArray* _removeItems;
};

}

#endif
