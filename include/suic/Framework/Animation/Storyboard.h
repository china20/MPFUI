// =============================================================
//
// Copyright (c) 2008-2012 ����, Inc. All rights reserved.
//
// suiwgx��Դ����ѭCPLЭ����п�Դ���κθ��˻�����������ʹ�ã������ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
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

