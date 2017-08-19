// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uieventhandler.h
// ��  �ܣ�������Ŀ���¼�����ص���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
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
