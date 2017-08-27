// 华勤科技版权所有 2008-2022
// 
// 文件名：inputassignerinvoker.h
// 功  能：实现系统事件处理。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _INPUTASSIGNERINVOKER_H_
#define _INPUTASSIGNERINVOKER_H_

#include <System/Windows/AssignerInvoker.h>

namespace suic
{

class MouseMoveAssignerInvoker : public AssignerInvoker
{
public:

    void Invoke();
};

}

#endif
