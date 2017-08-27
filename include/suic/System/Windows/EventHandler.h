// 华勤科技版权所有 2008-2022
// 
// 文件名：uieventhandler.h
// 功  能：定义核心库的事件处理回调。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIEVENTHANDLER_H_
#define _UIEVENTHANDLER_H_

#include <System/Windows/DpObject.h>
#include <System/Windows/RoutedEventArg.h>
#include <System/Windows/ItemsChangedEventArg.h>

namespace suic
{

class Element;

typedef delegate<void(Object*, EventArg*), suic::UnrefObj> EventHandler;
typedef delegate<void(Object*, InvokerArg*), suic::UnrefObj> InvokerHandler;

typedef delegate<void(DpObject*, RoutedEventArg*)> RoutedEventHandler;
typedef delegate<void(Element*,RoutedEventArg*)> ClickEventHandler;
typedef delegate<void(Element*,ToolTipEventArg*)> ToolTipEventHandler;

typedef delegate<void(Element*, InvokerEventArg*)> InvokerEventHandler;
typedef delegate<void(Element*, KeyboardEventArg*)> KeyboardEventHandler;
typedef delegate<void(Element*, MouseButtonEventArg*)> MouseButtonEventHandler;
typedef delegate<void(Element*, KeyboardFocusEventArg*)> KeyboardFocusEventHandler;
typedef delegate<void(Element*, ScrollChangedEventArg*)> ScrollChangedEventHandler;

}

#endif
