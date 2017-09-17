// 华勤科技版权所有 2008-2022
// 
// 文件名：uieventbuilder.h
// 功  能：定义核心库的事件构建类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIEVENTBUILDER_H_
#define _UIEVENTBUILDER_H_

#include <System/Tools/Array.h>
#include <System/Windows/Element.h>
#include <System/Windows/ICommandSource.h>

namespace suic
{

struct RoutedItem
{
    WeakRef target;
    RoutedEventHandlerInfo* item;

    RoutedItem()
    {
        item = NULL;
    }

    ~RoutedItem() {}

    RoutedItem(const RoutedItem& Other)
    {
        target = Other.target;
        item = Other.item;
    }
};

class EventBuilder
{
public:

    EventBuilder();

    void RaisedEvent(Element* sender, RoutedEventArg* pArg);
    void Clear();

public:

    static void OnSetCursorThunk(DpObject* sender, RoutedEventArg* e);
    static void OnPreSetCursorThunk(DpObject* sender, RoutedEventArg* e);

    static void OnPreMouseDownThunk(Element* sender, MouseButtonEventArg* e);
    static void OnMouseDownThunk(Element* sender, MouseButtonEventArg* e);
    static void OnPreMouseUpThunk(Element* sender, MouseButtonEventArg* e);
    static void OnMouseUpThunk(Element* sender, MouseButtonEventArg* e);

    static void OnPreMouseLeftDownThunk(Element* sender, MouseButtonEventArg* e);
    static void OnMouseLeftDownThunk(Element* sender, MouseButtonEventArg* e);
    static void OnPreMouseLeftUpThunk(Element* sender, MouseButtonEventArg* e);
    static void OnMouseLeftUpThunk(Element* sender, MouseButtonEventArg* e);
    static void OnPreMouseMiddleDownThunk(Element* sender, MouseButtonEventArg* e);
    static void OnMouseMiddleDownThunk(Element* sender, MouseButtonEventArg* e);
    static void OnPreMouseMiddleUpThunk(Element* sender, MouseButtonEventArg* e);
    static void OnMouseMiddleUpThunk(Element* sender, MouseButtonEventArg* e);
    static void OnPreMouseRightDownThunk(Element* sender, MouseButtonEventArg* e);
    static void OnMouseRightDownThunk(Element* sender, MouseButtonEventArg* e);
    static void OnPreMouseRightUpThunk(Element* sender, MouseButtonEventArg* e);
    static void OnMouseRightUpThunk(Element* sender, MouseButtonEventArg* e);

    static void OnPreMouseMoveThunk(Element* sender, MouseButtonEventArg* e);
    static void OnMouseMoveThunk(Element* sender, MouseButtonEventArg* e);
    static void OnMouseEnterThunk(Element* sender, MouseButtonEventArg* e);
    static void OnMouseLeaveThunk(Element* sender, MouseButtonEventArg* e);

    static void OnDropThunk(DpObject* sender, RoutedEventArg* e);
    static void OnDragEnterThunk(DpObject* sender, RoutedEventArg* e);
    static void OnDragLeaveThunk(DpObject* sender, RoutedEventArg* e);
    static void OnDragOverThunk(DpObject* sender, RoutedEventArg* e);

    static void OnPreviewDropThunk(DpObject* sender, RoutedEventArg* e);
    static void OnPreviewDragEnterThunk(DpObject* sender, RoutedEventArg* e);
    static void OnPreviewDragLeaveThunk(DpObject* sender, RoutedEventArg* e);
    static void OnPreviewDragOverThunk(DpObject* sender, RoutedEventArg* e);

    static void OnContextMenuThunk(DpObject* sender, RoutedEventArg* e);

    static void OnInitializedThunk(Element* sender, RoutedEventArg* e);

    static void OnPreMouseWheelThunk(Element* sender, MouseButtonEventArg* e);
    static void OnMouseWheelThunk(Element* sender, MouseButtonEventArg* e);

    static void OnPreExecutedThunk(Object* sender, ExecutedRoutedEventArg* e);
    static void OnExecutedThunk(Object* sender, ExecutedRoutedEventArg* e);

    static void OnGotFocusThunk(Element* sender, KeyboardFocusEventArg* e);
    static void OnLostFocusThunk(Element* sender, KeyboardFocusEventArg* e);

    static void OnPreTextInputThunk(Element* sender, KeyboardEventArg* e);
    static void OnTextInputThunk(Element* sender, KeyboardEventArg* e);

    static void OnPreKeyDownThunk(Element* sender, KeyboardEventArg* e);
    static void OnKeyDownThunk(Element* sender, KeyboardEventArg* e);

    static void OnPreKeyUpThunk(Element* sender, KeyboardEventArg* e);
    static void OnKeyUpThunk(Element* sender, KeyboardEventArg* e);

protected:

    void AddToRoutedEvent(Element* sender, RoutedEvent* routedEvent);

    void InternalCall(RoutedItem* rItem, RoutedEventArg* pArg);
    void BuildEventHandler(Element* sender, RoutedEvent* routedEvent);
    void CallEventHandler(RoutedEventArg* pArg);

    void AddRoutedEventFromList(Element* target, RoutedEventHandlerInfoList* lsInfo);

protected:

    RoutedEvent* _routedEvent;
    Array<RoutedItem*, false> _routedItemList;
};

}

#endif
