// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// eventbuilder.cpp

#include <System/Windows/EventBuilder.h>
#include <System/Windows/EventManager.h>
#include <System/Windows/CommandManager.h>

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Input/MouseDevice.h>
#include <System/Windows/FrameworkElement.h>

#include <System/Interop/System.h>
#include <System/Interop/InternalWindowOper.h>

#include <Framework/Controls/Menu.h>

namespace suic
{

EventBuilder::EventBuilder()
{
    _routedItemList.Resize(16);
}

void EventBuilder::BuildEventHandler(Element* sender, RoutedEvent* routedEvent)
{
    if (NULL == routedEvent)
    {
        throw ArgumentNullException(_U("routed event is NULL!"), __FILELINE__);
    }
    _routedEvent = routedEvent;

    // 构建事件处理
    if (routedEvent->GetRoutingStrategy() == RoutingStrategy::Direct)
    {
        AddToRoutedEvent(sender, routedEvent);
    }
    else
    {
        Element* fe = sender;

        while (fe != NULL)
        {
            /*if (fe->GetRTTIType() == PopupRoot::RTTIType())
            {
                PopupRoot* pRoot = RTTIEqualCast<PopupRoot>(fe);
                Popup* pPopup = DynamicCast<Popup>(pRoot->GetParent());

                fe = pPopup->GetPlacementTarget();
                if (NULL == fe)
                {
                    break;
                }
            }*/

            /*if (fe->BlockVisualState())
            {
                //break;
            }*/

            AddToRoutedEvent(fe, routedEvent);

            /*if (fe->GetUIParent() == NULL)
            {
                FrameworkElement* feTmp = RTTICast<FrameworkElement>(fe);
                if (feTmp != NULL)
                {
                    fe = feTmp->GetParent();
                }
            }
            else*/
            {
                fe = fe->GetUIParent();
            }
        }
    }
}

void EventBuilder::AddRoutedEventFromList(Element* target, RoutedEventHandlerInfoList* lsInfo)
{
    for (int i = 0; i < lsInfo->RoutedEHIList.Length(); ++i)
    {
        RoutedItem* rItem = new RoutedItem();

        rItem->target = target;
        rItem->item = lsInfo->RoutedEHIList[i];
        _routedItemList.Add(rItem);
    }
}

void EventBuilder::AddToRoutedEvent(Element* sender, RoutedEvent* routedEvent)
{
    int count = _routedItemList.Length();
    RoutedEventHandlerInfoList* listInfo = NULL;

    // 加入用户回调
    EventHandlersStore* ehs = sender->GetEventHandlersStore();
    listInfo = ehs->GetRoutedEventHandlerInfoList(routedEvent);

    if (listInfo)
    {
        AddRoutedEventFromList(sender, listInfo);
    }

    // 加入类型回调
    listInfo = EventControl::GetClassRoutedEventHandlerInfo(sender->GetRTTIType(), routedEvent);
    if (listInfo)
    {
        AddRoutedEventFromList(sender, listInfo);
    }

    // 加入附加事件回调
    /*listInfo = EventControl::GetAttachedRoutedEventHandlerInfo(sender->GetClassType(), routedEvent);
    if (listInfo)
    {
    AddRoutedEventFromList(tr, listInfo);
    }*/
}

void EventBuilder::InternalCall(RoutedItem* rItem, RoutedEventArg* pArg)
{
    if (!pArg->IsHandled() || rItem->item->invokeHandledEventsToo)
    {
        pArg->CallEventHandler(rItem->item->handler, pArg->GetSource());
    }

    pArg->SetSource(pArg->GetOriginalSource());
}

void EventBuilder::CallEventHandler(RoutedEventArg* pArg)
{
    pArg->SetRoutedEvent(_routedEvent);

    if (_routedEvent->GetRoutingStrategy() == RoutingStrategy::Tunnel)
    {
        for (int i = _routedItemList.Length() - 1; i >= 0; --i)
        {
            RoutedItem* rItem = _routedItemList[i];

            //
            // 如果等于1，表示已经被删掉
            //
            if (rItem->target.IsAlive())
            {
                pArg->SetSource(rItem->target.GetTarget());
                InternalCall(rItem, pArg);
            }
        }
    }
    else
    {
        for (int i = 0; i < _routedItemList.Length(); ++i)
        {
            RoutedItem* rItem = _routedItemList[i];

            //
            // 如果等于1，表示已经被删掉
            //
            if (rItem->target.IsAlive())
            {
                pArg->SetSource(rItem->target.GetTarget());
                InternalCall(rItem, pArg);
            }
        }
    }
}

void EventBuilder::RaisedEvent(Element* sender, RoutedEventArg* pArg)
{
    //g_build_count = 0;
    //DWORD dwTime = Environment::GetSystemTick();
    BuildEventHandler(sender, pArg->GetRoutedEvent());
    //SystemLog()->Warn(String().Format(_U("[%d]EventBuilder::RaisedEvent->BuildEventHandler Cost : %ld\n")
    //    , g_build_count, Environment::GetSystemTick()-dwTime));

    CallEventHandler(pArg);
    Clear();
}

void EventBuilder::Clear()
{
    for (int i = 0; i < _routedItemList.Length(); ++i)
    {
        delete _routedItemList[i];
    }
    _routedItemList.Clear();
}

//================================================================
//
void EventBuilder::OnSetCursorThunk(DpObject* sender, RoutedEventArg* e)
{
    CursorEventArg* cee = (CursorEventArg*)e;
    ((Element*)sender)->OnSetCursor(cee);
}

void EventBuilder::OnPreSetCursorThunk(DpObject* sender, RoutedEventArg* e)
{
    CursorEventArg* cee = (CursorEventArg*)e;
    ((Element*)sender)->OnPreviewSetCursor(cee);
}

void EventBuilder::OnPreMouseDownThunk(Element* sender, MouseButtonEventArg* e)
{
    if (!sender->IsMouseDownVisible())
    {
        return;
    }

    MouseButtonEventArg* mbe = (MouseButtonEventArg*)e;
    mbe->SetClickCount(MouseDevice::Current()->GetClickCount());
    sender->OnPreviewMouseDown(mbe);

    RoutedEvent* routedEvent = NULL;

    switch (mbe->GetMouseButton())
    {
    case MouseButton::mbLeftButton:
        routedEvent = Element::PreMouseLButtonDownEvent;
        break;
    case MouseButton::mbMiddleButton:
        routedEvent = Element::PreMouseMButtonDownEvent;
        break;
    case MouseButton::mbRightButton:
        routedEvent = Element::PreMouseRButtonDownEvent;
        break;
    }

    if (routedEvent)
    {
        e->SetRoutedEvent(routedEvent);
        sender->RaiseEvent(mbe);
    }

    if (e->IsHandled() && !suic::CoreSystem::Ins()->IsSetMouseDown())
    {
        suic::CoreSystem::Ins()->SetMouseDown(true);
        MouseDevice::SetMouseDown(sender, mbe->GetMouseButton());
    }
}

void EventBuilder::OnMouseDownThunk(Element* sender, MouseButtonEventArg* e)
{
    if (!sender->IsHitTestVisible())
    {
        return;
    }

    MouseButtonEventArg* mbe = (MouseButtonEventArg*)e;
    mbe->SetClickCount(MouseDevice::Current()->GetClickCount());
    sender->OnMouseDown(mbe);

    RoutedEvent* routedEvent = NULL;

    switch (mbe->GetMouseButton())
    {
    case MouseButton::mbLeftButton:
        routedEvent = Element::MouseLButtonDownEvent;
        break;
    case MouseButton::mbMiddleButton:
        routedEvent = Element::MouseMButtonDownEvent;
        break;
    case MouseButton::mbRightButton:
        routedEvent = Element::MouseRButtonDownEvent;
        break;
    }

    if (routedEvent)
    {
        e->SetRoutedEvent(routedEvent);
        sender->RaiseEvent(e);
    }

    if (e->IsHandled() && !suic::CoreSystem::Ins()->IsSetMouseDown())
    {
        suic::CoreSystem::Ins()->SetMouseDown(true);
        MouseDevice::SetMouseDown(sender, mbe->GetMouseButton());
    }
}

void EventBuilder::OnPreMouseUpThunk(Element* sender, MouseButtonEventArg* e)
{
    MouseButtonEventArg* mbe = (MouseButtonEventArg*)e;
    mbe->SetClickCount(MouseDevice::Current()->GetClickCount());

    sender->OnPreviewMouseUp(mbe);

    RoutedEvent* routedEvent = NULL;

    switch (mbe->GetMouseButton())
    {
    case MouseButton::mbLeftButton:
        routedEvent = Element::PreMouseLButtonUpEvent;
        break;
    case MouseButton::mbRightButton:
        routedEvent = Element::PreMouseRButtonUpEvent;
        break;
    }

    if (routedEvent)
    {
        e->SetRoutedEvent(routedEvent);
        sender->RaiseEvent(mbe);
    }
}

void EventBuilder::OnMouseUpThunk(Element* sender, MouseButtonEventArg* e)
{
    MouseButtonEventArg* mbe = (MouseButtonEventArg*)e;
    mbe->SetClickCount(MouseDevice::Current()->GetClickCount());

    sender->OnMouseUp(mbe);

    RoutedEvent* routedEvent = NULL;

    switch (mbe->GetMouseButton())
    {
    case MouseButton::mbLeftButton:
        routedEvent = Element::MouseLButtonUpEvent;
        break;
    case MouseButton::mbRightButton:
        routedEvent = Element::MouseRButtonUpEvent;
        break;
    }

    if (routedEvent)
    {
        mbe->SetRoutedEvent(routedEvent);
        sender->RaiseEvent(mbe);
    }
}

void EventBuilder::OnPreMouseLeftDownThunk(Element* sender, MouseButtonEventArg* e)
{
    MouseButtonEventArg* mbe = (MouseButtonEventArg*)e;
    sender->OnPreviewMouseLeftButtonDown(mbe);
}

void EventBuilder::OnMouseLeftDownThunk(Element* sender, MouseButtonEventArg* e)
{
    sender->OnMouseLeftButtonDown(e);
}

void EventBuilder::OnPreMouseLeftUpThunk(Element* sender, MouseButtonEventArg* e)
{
    sender->OnPreviewMouseLeftButtonUp(e);
}

void EventBuilder::OnMouseLeftUpThunk(Element* sender, MouseButtonEventArg* e)
{
    sender->OnMouseLeftButtonUp(e);
}

void EventBuilder::OnPreMouseMiddleDownThunk(Element* sender, MouseButtonEventArg* e)
{
    MouseButtonEventArg* mbe = (MouseButtonEventArg*)e;
    sender->OnPreviewMouseMiddleButtonDown(mbe);
}

void EventBuilder::OnMouseMiddleDownThunk(Element* sender, MouseButtonEventArg* e)
{
    sender->OnMouseLeftButtonDown(e);
}

void EventBuilder::OnPreMouseMiddleUpThunk(Element* sender, MouseButtonEventArg* e)
{
    sender->OnPreviewMouseLeftButtonUp(e);
}

void EventBuilder::OnMouseMiddleUpThunk(Element* sender, MouseButtonEventArg* e)
{
    sender->OnMouseLeftButtonUp(e);
}

void EventBuilder::OnPreMouseRightDownThunk(Element* sender, MouseButtonEventArg* e)
{
    sender->OnPreviewMouseRightButtonDown(e);
}

void EventBuilder::OnMouseRightDownThunk(Element* sender, MouseButtonEventArg* e)
{
    sender->OnMouseRightButtonDown(e);
}

void EventBuilder::OnPreMouseRightUpThunk(Element* sender, MouseButtonEventArg* e)
{
    sender->OnPreviewMouseRightButtonUp(e);
}

void EventBuilder::OnMouseRightUpThunk(Element* sender, MouseButtonEventArg* e)
{
    sender->OnMouseRightButtonUp(e);
}

void EventBuilder::OnPreMouseMoveThunk(Element* sender, MouseButtonEventArg* e)
{
    sender->OnPreviewMouseMove(e);
}

void EventBuilder::OnMouseMoveThunk(Element* sender, MouseButtonEventArg* e)
{
    sender->OnMouseMove(e);
}

void EventBuilder::OnMouseEnterThunk(Element* sender, MouseButtonEventArg* e)
{
    FrameworkElementPtr frame(sender);

    if (frame && frame->GetToolTip())
    {
        Point pt = __GetCusorPoint(sender);
        pt.x = SystemParameters::TransformXFromDevice(pt.x);
        pt.y = SystemParameters::TransformYFromDevice(pt.y);
        // 默认弹出提示栏
        frame->ShowToolTip(pt);
    }

    sender->OnMouseEnter(e);
}

void EventBuilder::OnMouseLeaveThunk(Element* sender, MouseButtonEventArg* e)
{
    if (ToolTip::Ins()->GetOwner() == sender)
    {
        ToolTip::Ins()->Close();
    }

    sender->OnMouseLeave(e);
}

void EventBuilder::OnDropThunk(DpObject* sender, RoutedEventArg* e)
{
    Element* elem = ((Element*)sender);
    if (elem->IsAllowDrop())
    {
        elem->OnDrop((DragEventArg*)e);
    }
}

void EventBuilder::OnDragEnterThunk(DpObject* sender, RoutedEventArg* e)
{
    Element* elem = ((Element*)sender);
    if (elem->IsAllowDrop())
    {
        elem->OnDragEnter((DragEventArg*)e);
    }
}

void EventBuilder::OnDragLeaveThunk(DpObject* sender, RoutedEventArg* e)
{
    Element* elem = ((Element*)sender);
    if (elem->IsAllowDrop())
    {
        elem->OnDragLeave((DragEventArg*)e);
    }
}

void EventBuilder::OnDragOverThunk(DpObject* sender, RoutedEventArg* e)
{
    Element* elem = ((Element*)sender);
    if (elem->IsAllowDrop())
    {
        elem->OnDragOver((DragEventArg*)e);
    }
}

void EventBuilder::OnPreviewDropThunk(DpObject* sender, RoutedEventArg* e)
{
    Element* elem = ((Element*)sender);
    if (elem->IsAllowDrop())
    {
        elem->OnPreviewDrop((DragEventArg*)e);
    }
}

void EventBuilder::OnPreviewDragEnterThunk(DpObject* sender, RoutedEventArg* e)
{
    Element* elem = ((Element*)sender);
    if (elem->IsAllowDrop())
    {
        elem->OnPreviewDragEnter((DragEventArg*)e);
    }
}

void EventBuilder::OnPreviewDragLeaveThunk(DpObject* sender, RoutedEventArg* e)
{
    Element* elem = ((Element*)sender);
    if (elem->IsAllowDrop())
    {
        elem->OnPreviewDragLeave((DragEventArg*)e);
    }
}

void EventBuilder::OnPreviewDragOverThunk(DpObject* sender, RoutedEventArg* e)
{
    Element* elem = ((Element*)sender);
    if (elem->IsAllowDrop())
    {
        elem->OnPreviewDragOver((DragEventArg*)e);
    }
}

void EventBuilder::OnContextMenuThunk(DpObject* sender, RoutedEventArg* e)
{
    Element* elem = ((Element*)sender);
    ContextMenuEventArg* cme = (ContextMenuEventArg*)e;
    elem->OnContextMenu(cme);
}

void EventBuilder::OnInitializedThunk(Element* sender, RoutedEventArg* e)
{
    sender->OnInitialized(e);
}

void EventBuilder::OnGotFocusThunk(Element* sender, KeyboardFocusEventArg* e)
{
    sender->OnGotFocus(e);
}

void EventBuilder::OnLostFocusThunk(Element* sender, KeyboardFocusEventArg* e)
{
    sender->OnLostFocus(e);
}

void EventBuilder::OnPreMouseWheelThunk(Element* sender, MouseButtonEventArg* e)
{
    MouseWheelEventArg* mwe = (MouseWheelEventArg*)e;
    sender->OnPreviewMouseWheel(mwe);
}

void EventBuilder::OnMouseWheelThunk(Element* sender, MouseButtonEventArg* e)
{
    MouseWheelEventArg* mwe = (MouseWheelEventArg*)e;
    sender->OnMouseWheel(mwe);
}

void EventBuilder::OnPreExecutedThunk(Object* sender, ExecutedRoutedEventArg* e)
{
    CommandManager::OnPreExecuted(sender, e);
}

void EventBuilder::OnExecutedThunk(Object* sender, ExecutedRoutedEventArg* e)
{
    CommandManager::OnExecuted(sender, e);
}

void EventBuilder::OnPreTextInputThunk(Element* sender, KeyboardEventArg* e)
{
    sender->OnPreviewTextInput(e);
}

void EventBuilder::OnTextInputThunk(Element* sender, KeyboardEventArg* e)
{
    sender->OnTextInput(e);
}

void EventBuilder::OnPreKeyDownThunk(Element* sender, KeyboardEventArg* e)
{
    sender->OnPreviewKeyDown(e);
}

void EventBuilder::OnKeyDownThunk(Element* sender, KeyboardEventArg* e)
{
    sender->OnKeyDown(e);
}

void EventBuilder::OnPreKeyUpThunk(Element* sender, KeyboardEventArg* e)
{
    sender->OnPreviewKeyUp(e);
}

void EventBuilder::OnKeyUpThunk(Element* sender, KeyboardEventArg* e)
{
    sender->OnKeyUp(e);
}

}
