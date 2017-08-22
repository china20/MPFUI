// 华勤科技版权所有 2010-2013
// 
// 作  者：Sharpui开发组
// 时  间：2010-08-31
//
// SHARPUI界面引擎采用业界成熟的体系架构，其属性系统、事件系统以
// 及模版在目前c++界面库是一大特点，真正做到样式和代码分离，底层
// 更采用了SSE2指令进行加速，不论在普通窗口还是半透明窗口下性能都
// 非常优秀，完善、灵活的动画接口，完全可以满足各种商业使用。
// 
// 目前SUIWGX库源码遵循BSD协议进行开源，任何个人或团体可以免费使用于
// 商业产品，但不能从此库衍生任何商业性质的界面库或代码，如果需购买
// 技术支持服务或咨询请联系china0851@foxmail.com。
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
