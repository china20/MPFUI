///
/// 文件名：UserFrameworkElement.h
///
/// 功  能：实现一个模拟的用户自定义控件
///
/// 作  者：汪荣
///

#ifndef _USERFRAMEWORKELEMENT_H
#define _TEXTBOXRANGE_H

#include <Framework/Controls/RangeBase.h>

class UserFrameworkElement : public suic::FrameworkElement
{
public:

    RTTIOfClass(UserFrameworkElement)

    UserFrameworkElement();
    virtual ~UserFrameworkElement();

protected:

    void OnRender(Drawing* drawing);
};

#endif
