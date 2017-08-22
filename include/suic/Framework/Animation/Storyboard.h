// =============================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// =============================================================

#ifndef _UISTORYBOARD_H_
#define _UISTORYBOARD_H_

#include <System/Windows/FrameworkElement.h>

namespace suic
{

class SUICORE_API StoryBoard : public Object
{
public:

    StoryBoard();
    ~StoryBoard();

    FrameworkElement* GetOwner();
    void SetOwner(FrameworkElement* owner);
};

}

#endif

