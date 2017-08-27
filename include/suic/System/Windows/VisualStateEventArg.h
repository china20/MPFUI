// 华勤科技版权所有 2008-2022
// 
// 文件名：uivisualstateeventarg.h
// 功  能：定义核心库的状态事件基类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIVISUALSTATEEVENTARG_H_
#define _UIVISUALSTATEEVENTARG_H_

#include <System/Windows/VisualStates.h>
#include <System/Windows/EventArg.h>
#include <suic/framework/uiframeview.h>

namespace suic
{

/// <summary>
/// 界面元素的事件，这是所有界面元素的基类，其它的界面元素都会直接或
/// 或间接地派生它
/// </summary>
class SUICORE_API ViewStateEventArg : public EventArg
{
public:

    ViewStateEventArg();

protected:

    VisualState _oldState;
    VisualState _newState; 
    FrameworkElementPtr _control; 
};

}

#endif
