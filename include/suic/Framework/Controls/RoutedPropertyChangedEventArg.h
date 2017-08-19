// ���ڿƼ���Ȩ���� 2010-2013
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-08-31
//
// SHARPUI�����������ҵ��������ϵ�ܹ���������ϵͳ���¼�ϵͳ��
// ��ģ����Ŀǰc++�������һ���ص㣬����������ʽ�ʹ�����룬�ײ�
// ��������SSE2ָ����м��٣���������ͨ���ڻ��ǰ�͸�����������ܶ�
// �ǳ����㣬���ơ����Ķ����ӿڣ���ȫ�������������ҵʹ�á�
// 
// ĿǰSUIWGX��Դ����ѭBSDЭ����п�Դ���κθ��˻�����������ʹ����
// ��ҵ��Ʒ�������ܴӴ˿������κ���ҵ���ʵĽ�������룬����蹺��
// ����֧�ַ������ѯ����ϵchina0851@foxmail.com��
// ====================================================================

#ifndef _UIROUTEDPROPERTYCHANGEDEVENTARG_H_
#define _UIROUTEDPROPERTYCHANGEDEVENTARG_H_

#include <Framework/Controls/Control.h>

namespace suic
{

template <typename T>
class RoutedPropChangedBaseArg : public suic::RoutedEventArg
{ 
public:

    typedef delegate<void(Element*, RoutedPropChangedBaseArg<T>*)> PropChangedBaseEventHandler;

    RoutedPropChangedBaseArg(T newValue) 
        : suic::RoutedEventArg(newValue) {}
    RoutedPropChangedBaseArg(T oldValue, T newValue)
        : suic::RoutedEventArg(NULL)
        , _oldValue(oldValue)
        , _newValue(newValue) {}
    RoutedPropChangedBaseArg(T oldValue, T newValue, suic::RoutedEvent* routedEvent)
        : suic::RoutedEventArg(NULL, routedEvent)
        , _oldValue(oldValue)
        , _newValue(newValue) {}

    T GetOldValue() const
    {
        return _oldValue;
    }

    T GetNewValue() const
    {
        return _newValue;
    } 

    void CallEventHandler(suic::Object* handler, suic::Object* target)
    {
        InternalCall<suic::Element, PropChangedBaseEventHandler, RoutedPropChangedBaseArg<T>>(handler, target);
    }

protected:

    T _oldValue;
    T _newValue;
};

typedef RoutedPropChangedBaseArg<float> FloatPropChangedEventArg;
typedef FloatPropChangedEventArg::PropChangedBaseEventHandler FloatPropChangedEventHandler;

typedef RoutedPropChangedBaseArg<int> IntPropertyChangedEventArg;
typedef IntPropertyChangedEventArg::PropChangedBaseEventHandler IntPropChangedEventHandler;

typedef RoutedPropChangedBaseArg<Object*> RoutedPropChangedEventArg;
typedef RoutedPropChangedEventArg::PropChangedBaseEventHandler RoutedPropChangedEventHandler;

};

#endif
