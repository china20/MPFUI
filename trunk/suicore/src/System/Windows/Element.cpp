// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================


/////////////////////////////////////////////////////////////////////////
// Element.cpp

#include <System/Types/Const.h>
#include <System/Types/Micro.h>

#include <System/Windows/Element.h>
#include <System/Windows/CoreTool.h>
#include <System/DragDrop/DragDrop.h>

#include <System/Render/Skia/SkiaDrawing.h>

#include <Framework/Controls/Panel.h>
#include <Framework/Controls/Popup.h>
#include <Framework/Controls/Window.h>
#include <Framework/Controls/Tooltip.h>
#include <Framework/Controls/Decorator.h>
#include <Framework/Controls/Application.h>
#include <Framework/Controls/ItemCollection.h>

#include <Framework/Animation/Storyboard.h>

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Input/MouseDevice.h>
#include <System/Input/FocusManager.h>

#include <System/Tools/Thread.h>
#include <System/Tools/AlphaHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/DesignHelper.h>

#include <System/Types/StringCore.h>
#include <System/Animation/AnimationStorage.h>

#include <System/Windows/EventManager.h>
#include <System/Windows/CommandManager.h>
#include <System/Windows/HwndSubclass.h>
#include <System/Windows/InvManager.h>
#include <System/Windows/EventBuilder.h>

#include <System/Graphics/SolidcolorBrush.h>

/*--------内部使用头文件--------*/
#include <System/Interop/InternalOper.h>
#include <System/Interop/System.h>

//#include <lua/LuaManager.h>
#include <typeinfo>

namespace suic
{

static void IsFocused_Changed(DpObject* sender, DpPropChangedEventArg* e)
{
    ElementPtr elem(sender);

    if (elem)
    {
        if (e->GetNewValue()->ToBool())
        {
            RoutedEventArg fe(sender);
            elem->OnGotFocus(&fe);
        }
        else
        {
            RoutedEventArg fe(sender);
            elem->OnLostFocus(&fe);
        }
    }
}

enum ElemInvokeType
{
    eiUpdateLayout,
    eiInvaliateVisual,
    eiInvalidateForce,
    eiInvalidateMeasure,
    eiInvalidateArrange,
    eiUpdateMeasure,
    eiUpdateArrange,
    eiSetVisibility,
    eiEnableElement,
};

class HandlerAssignerInvoker : public AssignerInvoker
{
public:

    HandlerAssignerInvoker(InvokeProxy* sender, int what, Object* data)
    {
        _what = what;
        _invoke = sender;
        _data = data;

        if (_invoke != NULL)
        {
            _invoke->ref();
        }

        if (_data != NULL)
        {
            _data->ref();
        }
    }

    ~HandlerAssignerInvoker()
    {
        FREEREFOBJ(_data);
        FREEREFOBJ(_invoke);
    }

    void Invoke()
    {
        if (_invoke)
        {
            InvokerArg e(_what, _data);
            _invoke->OnInvoker(&e);
        }
    }

protected:

    int _what;
    Object* _data;
    InvokeProxy* _invoke;
};

InvokeProxy::InvokeProxy()
{

}

InvokeProxy::InvokeProxy(Element* target)
    : _target(target)
{
    
}

InvokeProxy::~InvokeProxy()
{

}

Element* InvokeProxy::GetTarget() const
{
    return _target.GetElement<Element>();
}

void InvokeProxy::SetTarget(Element* target)
{
    _target = target;
}

void InvokeProxy::PostInvoker(int what, Object* data)
{
    Assigner* assigner = GetAssigner();
    HandlerAssignerInvoker* invoker = NULL;
    Element* target = _target.GetElement<Element>();

    if (NULL != target)
    {
        assigner = target->GetAssigner();
    }

    if (assigner->GetThreadId() != Thread::CurrentThreadId())
    {
        invoker = new HandlerAssignerInvoker(this, what, data);

        invoker->ref();
        assigner->BeginInvoke(invoker);
        invoker->unref();
    }
    else
    {
        HandlerAssignerInvoker hInvoker(this, what, data);
        hInvoker.setAutoDelete(false);
        hInvoker.Invoke();
    }
}

void InvokeProxy::SendInvoker(int what, Object* data, int timeout)
{
    Assigner* assigner = GetAssigner();
    HandlerAssignerInvoker* invoker = NULL;
    Element* target = _target.GetElement<Element>();

    if (NULL != target)
    {
        assigner = target->GetAssigner();
    }

    if (assigner->GetThreadId() != Thread::CurrentThreadId())
    {
        invoker = new HandlerAssignerInvoker(this, what, data);
        invoker->ref();
        assigner->Invoke(invoker, timeout);
        invoker->unref();
    }
    else
    {
        HandlerAssignerInvoker hInvoker(this, what, data);
        hInvoker.setAutoDelete(false);
        hInvoker.Invoke();
    }
}

void InvokeProxy::OnInvoker(InvokerArg* arg)
{
    Element* target = _target.GetElement<Element>();
    if (target != NULL)
    {
        target->OnInvokerArrived(arg);
    }
    if (Invoker.safe_bool())
    {
        Invoker(this, arg);
    }
}

ElementInvoker::ElementInvoker(Element* sender, Object* data, int type)
    : _target(sender)
    , _data(data)
    , _type(type)
{
    if (data)
    {
        data->ref();
    }
}

ElementInvoker::~ElementInvoker()
{
    if (_data)
    {
        _data->unref();
    }
}

void ElementInvoker::CallInvoke(Element* elem, Object* data, int type)
{
    if (elem->GetAssigner()->GetThreadId() != Thread::CurrentThreadId())
    {
        AssignerInvoker* invoker = new ElementInvoker(elem, data, type);
        invoker->ref();
        elem->GetAssigner()->Invoke(invoker, -1);
        invoker->unref();
    }
    else
    {
        ElementInvoker invoker(elem, data, type);
        invoker.Invoke();
    }
}

void ElementInvoker::Invoke()
{
    FrameworkElement* invoker = RTTICast<FrameworkElement>(_target.GetTarget());
    if (invoker)
    {
        switch (_type)
        {
        case ElemInvokeType::eiUpdateLayout:
            {
                invoker->InvalidateMeasure();
                invoker->InvalidateArrange();
                InvManager::InvalidateElement(invoker, NULL, false);
            }
            break;

        case ElemInvokeType::eiInvaliateVisual:
            {
                bool autoDel = invoker->setAutoDelete(false);
                InvManager::InvalidateElement(invoker, NULL, false);
                invoker->setAutoDelete(autoDel);
            }
            break;

        case ElemInvokeType::eiSetVisibility:
            {
                invoker->SetValue(Element::VisibilityProperty, _data);
                break;
            }

        case ElemInvokeType::eiUpdateArrange:
            invoker->Arrange(invoker->_finalArrange);
            break;

        case ElemInvokeType::eiUpdateMeasure:
            invoker->Measure(invoker->_measureSize);
            break;

        case ElemInvokeType::eiInvalidateForce:
            {
                bool autoDel = invoker->setAutoDelete(false);
                InvManager::InvalidateElement(invoker, NULL, true);
                invoker->setAutoDelete(autoDel);
            }
            break;

        case ElemInvokeType::eiEnableElement:
            {
                bool val = _data->ToBool();
                if (val == !invoker->IsInnerEnabled())
                {
                    Window* pWnd(RTTICast<Window>(invoker));
                    Popup* pPopup(RTTICast<Popup>(invoker));

                    if ((pWnd && pWnd->GetUIParent() == NULL) || pPopup)
                    {
                        uiEnableWindow(invoker, val);
                    }

                    invoker->SetValue(Element::IsEnabledProperty, _data);
                    invoker->InnerEnableChildren(invoker, val);
                }
            }
        }
    }
}

class HitTestFilterActionConvert : public IConvertValue
{
public:

    static HitTestFilterActionConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val)
    {
        String strVal = val->ToString();

        if (strVal.Equals(_U("Stop")))
        {
            return HitTestFilterAction::StopBox;
        }
        else if (strVal.Equals(_U("ContinueSkipSelf")))
        {
            return HitTestFilterAction::ContinueSkipSelfBox;
        }
        else if (strVal.Equals(_U("ContinueSkipChildren")))
        {
            return HitTestFilterAction::ContinueSkipChildrenBox;
        }
        else if (strVal.Equals(_U("ContinueSkipSelfAndChildren")))
        {
            return HitTestFilterAction::ContinueSkipSelfAndChildrenBox;
        }
        else
        {
            return HitTestFilterAction::ContinueBox;
        }
    }

    String ToString(DpProperty* dp, Object* prop)
    {
        String strVal;
        int iVal = prop->ToInt();

        switch (iVal)
        {
        case HitTestFilterAction::Stop:
            strVal = _U("Stop");
            break;

        case HitTestFilterAction::ContinueSkipSelf:
            strVal = _U("ContinueSkipSelf");
            break;

        case HitTestFilterAction::ContinueSkipChildren:
            strVal = _U("ContinueSkipChildren");
            break;

        case HitTestFilterAction::ContinueSkipSelfAndChildren:
            strVal = _U("ContinueSkipSelfAndChildren");
            break;

        default:

            strVal = _U("Continue");
        }

        return strVal;
    }

    static void InitCursors()
    {
    }

protected:

    HitTestFilterActionConvert() {}
};

HitTestFilterActionConvert* HitTestFilterActionConvert::Convert = new HitTestFilterActionConvert();

ImplementRTTIOfClass(Element, DpObject)

//------------------------------------------------------------------------

RoutedEvent* Element::MouseDownEvent;
RoutedEvent* Element::MouseUpEvent;

RoutedEvent* Element::MouseEnterEvent;
RoutedEvent* Element::MouseMoveEvent;
RoutedEvent* Element::MouseLeaveEvent;
RoutedEvent* Element::MouseLButtonDownEvent;
RoutedEvent* Element::MouseLButtonUpEvent;
RoutedEvent* Element::MouseMButtonDownEvent;
RoutedEvent* Element::MouseMButtonUpEvent;
RoutedEvent* Element::MouseRButtonDownEvent;
RoutedEvent* Element::MouseRButtonUpEvent;

RoutedEvent* Element::ContextMenuEvent;

RoutedEvent* Element::TextInputEvent;
RoutedEvent* Element::KeyDownEvent;
RoutedEvent* Element::KeyUpEvent;

RoutedEvent* Element::CursorSetEvent;
RoutedEvent* Element::MouseWheelEvent;
RoutedEvent* Element::LostMouseCaptureEvent;
RoutedEvent* Element::GotMouseCaptureEvent;
RoutedEvent* Element::GotFocusEvent;
RoutedEvent* Element::LostFocusEvent;

RoutedEvent* Element::GotKeyboardFocusEvent;
RoutedEvent* Element::LostKeyboardFocusEvent;

RoutedEvent* Element::ExecutedEvent;
RoutedEvent* Element::PreExecutedEvent;

RoutedEvent* Element::CanExecuteEvent;
RoutedEvent* Element::PreCanExecuteEvent;

RoutedEvent* Element::ThemeChangedEvent;
RoutedEvent* Element::LoadedEvent;
RoutedEvent* Element::UnloadedEvent;
RoutedEvent* Element::TargetUpdatedEvent;
RoutedEvent* Element::HitTestEvent;
RoutedEvent* Element::InvalidateEvent;

//------------------------------------------------------------

RoutedEvent* Element::PreMouseDownEvent;
RoutedEvent* Element::PreMouseUpEvent;

RoutedEvent* Element::PreMouseMoveEvent;

RoutedEvent* Element::PreCursorSetEvent;
RoutedEvent* Element::PreMouseWheelEvent;
RoutedEvent* Element::PreMouseLButtonDownEvent;
RoutedEvent* Element::PreMouseLButtonUpEvent;
RoutedEvent* Element::PreMouseMButtonDownEvent;
RoutedEvent* Element::PreMouseMButtonUpEvent;
RoutedEvent* Element::PreMouseRButtonDownEvent;
RoutedEvent* Element::PreMouseRButtonUpEvent;

RoutedEvent* Element::PreGotKeyboardFocusEvent;
RoutedEvent* Element::PreLostKeyboardFocusEvent;

RoutedEvent* Element::PreTextInputEvent;
RoutedEvent* Element::PreKeyDownEvent;
RoutedEvent* Element::PreKeyUpEvent;

RoutedEvent* Element::InvokerEvent;

//RoutedEvent* Element::PreDropEvent;
//RoutedEvent* Element::PreDragEnterEvent;
//RoutedEvent* Element::PreDragLeaveEvent;
//RoutedEvent* Element::PreDragOverEvent;

DpProperty* Element::HitTestFilterActionProperty;

DpProperty* Element::NameProperty;
DpProperty* Element::OpacityProperty;
DpProperty* Element::VisibilityProperty;
DpProperty* VisualHost::RootSourceProperty;

DpProperty* Element::AllowDropProperty;

DpProperty* Element::CacheModeProperty;
DpProperty* Element::BitmapEffectProperty;

DpProperty* Element::ClipProperty;
DpProperty* Element::ClipToBoundsProperty;
DpProperty* Element::IsEnabledProperty;
DpProperty* Element::IsVisibleProperty;
DpProperty* Element::IsMouseOverProperty;
DpProperty* Element::IsFocusedProperty;
DpProperty* Element::IsKeyboardFocusedProperty;
DpProperty* Element::IsKeyboardFocusWithinProperty;
DpProperty* Element::IsMouseCaptureWithinProperty;

DpProperty* Element::OpacityMaskProperty;
DpProperty* Element::IsHitTestVisibleProperty;

DpProperty* Element::IsContextMenuProperty;
DpProperty* Element::FocusableProperty;
DpProperty* Element::DataContextProperty;

DpProperty* Element::UseLayoutRoundingProperty;

DpProperty* Element::RenderTransformProperty;
//DpProperty* Element::VisualTransformProperty;
DpProperty* Element::RenderTransformOriginProperty;

DpProperty* Element::CacheLayerProperty;

DpProperty* Element::Tag0Property;
DpProperty* Element::Tag1Property;
DpProperty* Element::Tag2Property;
DpProperty* Element::Tag3Property;

DpProperty* Element::VisualEffectProperty;

bool Element::_setDpi = true;
Float Element::_dpiScaleX = 1.0f;
Float Element::_dpiScaleY = 1.0f;

static PropMetadata* _isVisibleMetadata;

void Element::OnVisibilityPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Element* elem = RTTICast<Element>(d);
    Visibility newValue = (Visibility)e->GetNewValue()->ToInt();

    elem->SetVisibilityCache(newValue);    
    elem->UpdateIsVisibleCache();
    elem->SwitchVisibilityIfNeeded(newValue);
    elem->InvalidateIsVisiblePropertyOnChildren(IsVisibleProperty);
}

void Element::SwitchVisibilityIfNeeded(Visibility val)
{
    switch (val)
    {
    case Visibility::Visible:
        EnsureVisible();
        break;

    case Visibility::Hidden:
        EnsureInvisible(false);
        break;

    case Visibility::Collapsed:
        EnsureInvisible(true);
        break;
    }
}

void Element::OnVisualEffectPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Element* elem = RTTICast<Element>(d);
    if (NULL != elem)
    {
        elem->_effect = RTTICast<Effect>(e->GetNewValue());
    }
}

void Element::OnIsVisiblePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Element* elem = RTTICast<Element>(d);
    elem->InvalidateIsVisiblePropertyOnChildren(e->GetProp());
}

void Element::OnIsEnabledPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Element* elem = RTTICast<Element>(d);
    IsEnabledChangedEventArg ee(e->GetNewValue()->ToBool());
    elem->OnIsEnabledChanged(&ee);
    elem->InvalidateIsEnablePropertyOnChildren(e->GetProp());
}

void Element::OnClipPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Element* elem = RTTICast<Element>(d);
    if (NULL != elem)
    {
        elem->_clip = RTTICast<Geometry>(e->GetNewValue());
    }
}

void Element::OnClipToBoundsPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Element* elem = RTTICast<Element>(d);
    if (NULL != elem)
    {
        elem->WriteDoFlag(ViewFlags::IsClipToBoundsVF, e->GetNewValue()->ToBool());
    }
}

void Element::OnDataContextPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Element* elem = RTTICast<Element>(d);

    if (elem != NULL)
    {
        elem->_dataCtx = e->GetNewValue();
        elem->OnDataContextChanged(e);
        // 刷新子控件的binding表达式
        elem->InvalidateBinding(!elem->ReadFlag(CoreFlags::IsForbidInvalidateDC));
    }
}

void Element::OnRenderTransformNotify(Animatable* d, DpProperty* dp)
{
    InvalidateVisual();
}

void Element::OnLayoutTransformNotify(Animatable* d, DpProperty* dp)
{
    UpdateLayout();
}

void Element::OnRenderTransformPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Element* elem = RTTICast<Element>(d);
    Animatable* oldAni = RTTICast<Animatable>(e->GetNewValue());
    Animatable* newAni = RTTICast<Animatable>(e->GetNewValue());

    if (NULL != oldAni)
    {
        oldAni->AnimateNofity -= AnimateNofityHandler(elem, &Element::OnRenderTransformNotify);
    }

    if (NULL != newAni)
    {
        newAni->AnimateNofity += AnimateNofityHandler(elem, &Element::OnRenderTransformNotify);
    }

    elem->_renderTrans = RTTICast<Transform>(e->GetNewValue());
    if (NULL == elem->_renderTrans)
    {
        elem->_renderTrans = Transform::GetIdentity();
    }
}

void Element::OnCacheLayerPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Element* elem = RTTICast<Element>(d);

    if (elem->_renderLayer)
    {
        delete elem->_renderLayer;
        elem->_renderLayer = NULL;
    }

    if (e->GetNewValue()->ToBool())
    {
        elem->_renderLayer = new RenderContext();
        elem->RenderOpen(elem->_renderLayer);
    }
}

void Element::OnIsHitTestVisiblePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Element* elem = RTTICast<Element>(d);
    if (NULL != elem)
    {
        elem->WriteFlag(CoreFlags::IsHitTestHidden, !e->GetNewValue()->ToBool());
    }
}

void Element::InvalidateIsVisiblePropertyOnChildren(DpProperty* prop)
{
    int count = GetVisualChildrenCount();

    if (prop == IsVisibleProperty)
    {
        for (int i = 0; i < count; i++)
        {
            Element* child = GetVisualChild(i);

            if (child != NULL)
            {
                child->UpdateIsVisibleCache();
                child->InvalidateIsVisiblePropertyOnChildren(prop);
            }
        }
    }
}

void Element::OnPreviewGotKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e)
{
    if (sender != NULL) 
    {
        sender->OnPreviewGotKeyboardFocus(e);
    } 
}

void Element::OnGotKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e)
{
    if (sender != NULL) 
    {
        sender->OnGotKeyboardFocus(e);
    } 
}

void Element::OnPreviewLostKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e)
{
    if (sender != NULL)
    {
        sender->OnPreviewLostKeyboardFocus(e);
    }
}

void Element::OnLostKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e)
{
    if (sender != NULL)
    {
        sender->OnLostKeyboardFocus(e);
    }
}

void Element::InvalidateIsEnablePropertyOnChildren(DpProperty* property)
{
    
}

void Element::PostInvoker(int what, Object* data)
{
    InvokeProxy* h = new InvokeProxy(this);
    h->PostInvoker(what, data);
}

void Element::SendInvoker(int what, Object* data, int timeout)
{
    InvokeProxy* h = new InvokeProxy(this);
    h->SendInvoker(what, data, timeout);
}

void Element::AddInvoker(InvokerEventHandler* h)
{
    AddHandler(InvokerEvent, h);
}

void Element::RemoveInvoker(InvokerEventHandler* h)
{
    RemoveHandler(InvokerEvent, h);
}

void Element::StaticInit()
{
    if (HitTestFilterActionProperty == NULL)
    {
        _setDpi = true;
        _dpiScaleX = 1.0f;
        _dpiScaleY = 1.0f;

        DpObject::StaticInit();
        FocusManager::StaticInit();
        Transform::StaticInit();
        DragDrop::StaticInit();

        HitTestFilterActionProperty = DpProperty::Register(_U("HitTestFilterAction"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(new Integer(HitTestFilterAction::Continue), PropMetadataOptions::AffectsNone));

        HitTestFilterActionProperty->SetConvertValueCb(HitTestFilterActionConvert::Convert);

        MouseDownEvent = EventControl::RegisterRoutedEvent(_U("MouseDown"), RoutingStrategy::Bubble, MouseButtonEventHandler::RTTIType(), RTTIType());
        MouseUpEvent = EventControl::RegisterRoutedEvent(_U("MouseUp"), RoutingStrategy::Bubble, MouseButtonEventHandler::RTTIType(), RTTIType());
        PreMouseDownEvent = EventControl::RegisterRoutedEvent(_U("PreMouseDown"), RoutingStrategy::Tunnel, MouseButtonEventHandler::RTTIType(), RTTIType());
        PreMouseUpEvent = EventControl::RegisterRoutedEvent(_U("PreMouseUp"), RoutingStrategy::Tunnel, MouseButtonEventHandler::RTTIType(), RTTIType());

        MouseEnterEvent = EventControl::RegisterRoutedEvent(_U("MouseEnter"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());
        MouseMoveEvent = EventControl::RegisterRoutedEvent(_U("MouseMove"), RoutingStrategy::Bubble, MouseButtonEventHandler::RTTIType(), RTTIType());
        MouseLeaveEvent = EventControl::RegisterRoutedEvent(_U("MouseLeave"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());;
        MouseLButtonDownEvent = EventControl::RegisterRoutedEvent(_U("MouseLButtonDown"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());
        MouseLButtonUpEvent = EventControl::RegisterRoutedEvent(_U("MouseLButtonUp"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());
        MouseMButtonDownEvent = EventControl::RegisterRoutedEvent(_U("MouseMButtonDown"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());
        MouseMButtonUpEvent = EventControl::RegisterRoutedEvent(_U("MouseMButtonUp"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());

        MouseRButtonDownEvent = EventControl::RegisterRoutedEvent(_U("MouseRButtonDown"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());
        MouseRButtonUpEvent = EventControl::RegisterRoutedEvent(_U("MouseRButtonUp"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());
        CursorSetEvent = EventControl::RegisterRoutedEvent(_U("CursorSet"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
        MouseWheelEvent = EventControl::RegisterRoutedEvent(_U("MouseWheel"), RoutingStrategy::Bubble, RTTIType(), RTTIType());

        ContextMenuEvent = EventControl::RegisterRoutedEvent(_U("ContextMenu"), RoutingStrategy::Bubble, RTTIType(), RTTIType());

        LostMouseCaptureEvent = EventControl::RegisterRoutedEvent(_U("LostMouseCapture"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
        GotMouseCaptureEvent = EventControl::RegisterRoutedEvent(_U("GotMouseCapture"), RoutingStrategy::Bubble, RTTIType(), RTTIType());

        GotFocusEvent = EventControl::RegisterRoutedEvent(_U("GotFocus"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
        LostFocusEvent = EventControl::RegisterRoutedEvent(_U("LostFocus"), RoutingStrategy::Bubble, RTTIType(), RTTIType());

        ExecutedEvent = EventControl::RegisterRoutedEvent(_U("Executed"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
        PreExecutedEvent = EventControl::RegisterRoutedEvent(_U("PreExecuted"), RoutingStrategy::Tunnel, RTTIType(), RTTIType());

        CanExecuteEvent = EventControl::RegisterRoutedEvent(_U("CanExecute"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
        PreCanExecuteEvent = EventControl::RegisterRoutedEvent(_U("PreCanExecute"), RoutingStrategy::Tunnel, RTTIType(), RTTIType());

        GotKeyboardFocusEvent = EventControl::RegisterRoutedEvent(_U("GotKeyboardFocus"), RoutingStrategy::Bubble, KeyboardFocusEventHandler::RTTIType(), RTTIType());
        LostKeyboardFocusEvent = EventControl::RegisterRoutedEvent(_U("LostKeyboardFocus"), RoutingStrategy::Bubble, KeyboardFocusEventHandler::RTTIType(), RTTIType());

        PreGotKeyboardFocusEvent = EventControl::RegisterRoutedEvent(_U("PreGotKeyboardFocus"), RoutingStrategy::Tunnel, KeyboardFocusEventHandler::RTTIType(), RTTIType());
        PreLostKeyboardFocusEvent = EventControl::RegisterRoutedEvent(_U("PreLostKeyboardFocus"), RoutingStrategy::Tunnel, KeyboardFocusEventHandler::RTTIType(), RTTIType());

        PreCursorSetEvent = EventControl::RegisterRoutedEvent(_U("PreCursorSet"), RoutingStrategy::Tunnel, RTTIType(), RTTIType());
        PreMouseWheelEvent = EventControl::RegisterRoutedEvent(_U("PreMouseWheel"), RoutingStrategy::Tunnel, RTTIType(), RTTIType());
        PreMouseMoveEvent = EventControl::RegisterRoutedEvent(_U("PreMouseMove"), RoutingStrategy::Tunnel, MouseButtonEventHandler::RTTIType(), RTTIType());

        PreMouseLButtonDownEvent = EventControl::RegisterRoutedEvent(_U("PreMouseLButtonDown"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());
        PreMouseLButtonUpEvent = EventControl::RegisterRoutedEvent(_U("PreMouseLButtonUp"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());
        PreMouseMButtonDownEvent = EventControl::RegisterRoutedEvent(_U("PreMouseMButtonDown"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());
        PreMouseMButtonUpEvent = EventControl::RegisterRoutedEvent(_U("PreMouseMButtonUp"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());

        PreMouseRButtonDownEvent = EventControl::RegisterRoutedEvent(_U("PreMouseRButtonDown"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());
        PreMouseRButtonUpEvent = EventControl::RegisterRoutedEvent(_U("PreMouseRButtonUp"), RoutingStrategy::Direct, MouseButtonEventHandler::RTTIType(), RTTIType());

        TextInputEvent = EventControl::RegisterRoutedEvent(_U("TextInput"), RoutingStrategy::Bubble, KeyboardEventHandler::RTTIType(), RTTIType());
        KeyDownEvent = EventControl::RegisterRoutedEvent(_U("KeyDown"), RoutingStrategy::Bubble, KeyboardEventHandler::RTTIType(), RTTIType());
        KeyUpEvent = EventControl::RegisterRoutedEvent(_U("KeyUp"), RoutingStrategy::Bubble, KeyboardEventHandler::RTTIType(), RTTIType());

        PreTextInputEvent = EventControl::RegisterRoutedEvent(_U("PreTextInput"), RoutingStrategy::Tunnel, KeyboardEventHandler::RTTIType(), RTTIType());
        PreKeyDownEvent = EventControl::RegisterRoutedEvent(_U("PreKeyDown"), RoutingStrategy::Tunnel, KeyboardEventHandler::RTTIType(), RTTIType());
        PreKeyUpEvent = EventControl::RegisterRoutedEvent(_U("PreKeyUp"), RoutingStrategy::Tunnel, KeyboardEventHandler::RTTIType(), RTTIType());

        InvokerEvent = EventControl::RegisterRoutedEvent(_U("Invoker"), RoutingStrategy::Bubble, RTTIType(), RTTIType());

        ThemeChangedEvent = EventControl::RegisterRoutedEvent(_U("ThemeChanged"), RoutingStrategy::Direct, RTTIType(), RTTIType());
        LoadedEvent = EventControl::RegisterRoutedEvent(_U("Loaded"), RoutingStrategy::Direct, RTTIType(), RTTIType());
        UnloadedEvent = EventControl::RegisterRoutedEvent(_U("Unloaded"), RoutingStrategy::Direct, RTTIType(), RTTIType());
        TargetUpdatedEvent = EventControl::RegisterRoutedEvent(_U("TargetUpdated"), RoutingStrategy::Direct, RTTIType(), RTTIType());
        HitTestEvent = EventControl::RegisterRoutedEvent(_U("HitTest"), RoutingStrategy::Bubble, RTTIType(), RTTIType());
        InvalidateEvent = EventControl::RegisterRoutedEvent(_U("Invalidate"), RoutingStrategy::Bubble, RTTIType(), RTTIType());

        //--------------------------------------------------------------------

        EventControl::RegisterClassHandler(RTTIType(), CursorSetEvent, new RoutedEventHandler(&EventBuilder::OnSetCursorThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), PreMouseDownEvent, new MouseButtonEventHandler(&EventBuilder::OnPreMouseDownThunk), true);
        EventControl::RegisterClassHandler(RTTIType(), MouseDownEvent, new MouseButtonEventHandler(&EventBuilder::OnMouseDownThunk), true);
        EventControl::RegisterClassHandler(RTTIType(), PreMouseUpEvent, new MouseButtonEventHandler(&EventBuilder::OnPreMouseUpThunk), true);
        EventControl::RegisterClassHandler(RTTIType(), MouseUpEvent, new MouseButtonEventHandler(&EventBuilder::OnMouseUpThunk), true);

        EventControl::RegisterClassHandler(RTTIType(), PreCursorSetEvent, new RoutedEventHandler(&EventBuilder::OnPreSetCursorThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), PreMouseWheelEvent, new MouseButtonEventHandler(&EventBuilder::OnPreMouseWheelThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), MouseWheelEvent, new MouseButtonEventHandler(&EventBuilder::OnMouseWheelThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), PreExecutedEvent, new ExecutedRoutedEventHandler(&EventBuilder::OnPreExecutedThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), ExecutedEvent, new ExecutedRoutedEventHandler(&EventBuilder::OnExecutedThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), PreMouseLButtonDownEvent, new MouseButtonEventHandler(&EventBuilder::OnPreMouseLeftDownThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), MouseLButtonDownEvent, new MouseButtonEventHandler(&EventBuilder::OnMouseLeftDownThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), PreMouseLButtonUpEvent, new MouseButtonEventHandler(&EventBuilder::OnPreMouseLeftUpThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), MouseLButtonUpEvent, new MouseButtonEventHandler(&EventBuilder::OnMouseLeftUpThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), PreMouseMButtonDownEvent, new MouseButtonEventHandler(&EventBuilder::OnPreMouseMiddleDownThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), MouseMButtonDownEvent, new MouseButtonEventHandler(&EventBuilder::OnMouseMiddleDownThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), PreMouseMButtonUpEvent, new MouseButtonEventHandler(&EventBuilder::OnPreMouseMiddleUpThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), MouseMButtonUpEvent, new MouseButtonEventHandler(&EventBuilder::OnMouseMiddleUpThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), PreMouseRButtonDownEvent, new MouseButtonEventHandler(&EventBuilder::OnPreMouseRightDownThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), MouseRButtonDownEvent, new MouseButtonEventHandler(&EventBuilder::OnMouseRightDownThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), PreMouseRButtonUpEvent, new MouseButtonEventHandler(&EventBuilder::OnPreMouseRightUpThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), MouseRButtonUpEvent, new MouseButtonEventHandler(&EventBuilder::OnMouseRightUpThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), PreMouseMoveEvent, new MouseButtonEventHandler(&EventBuilder::OnPreMouseMoveThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), MouseMoveEvent, new MouseButtonEventHandler(&EventBuilder::OnMouseMoveThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), MouseEnterEvent, new MouseButtonEventHandler(&EventBuilder::OnMouseEnterThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), MouseLeaveEvent, new MouseButtonEventHandler(&EventBuilder::OnMouseLeaveThunk), false);

        // 拖动
        EventControl::RegisterClassHandler(RTTIType(), DragDrop::DropEvent, new RoutedEventHandler(&EventBuilder::OnDropThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), DragDrop::DragEnterEvent, new RoutedEventHandler(&EventBuilder::OnDragEnterThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), DragDrop::DragLeaveEvent, new RoutedEventHandler(&EventBuilder::OnDragLeaveThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), DragDrop::DragOverEvent, new RoutedEventHandler(&EventBuilder::OnDragOverThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), DragDrop::PreviewDropEvent, new RoutedEventHandler(&EventBuilder::OnPreviewDropThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), DragDrop::PreviewDragEnterEvent, new RoutedEventHandler(&EventBuilder::OnPreviewDragEnterThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), DragDrop::PreviewDragLeaveEvent, new RoutedEventHandler(&EventBuilder::OnPreviewDragLeaveThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), DragDrop::PreviewDragOverEvent, new RoutedEventHandler(&EventBuilder::OnPreviewDragOverThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), ContextMenuEvent, new RoutedEventHandler(&EventBuilder::OnContextMenuThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), GotKeyboardFocusEvent, new KeyboardFocusEventHandler(&Element::OnGotKeyboardFocusThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), LostKeyboardFocusEvent, new KeyboardFocusEventHandler(&Element::OnLostKeyboardFocusThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), PreGotKeyboardFocusEvent, new KeyboardFocusEventHandler(&Element::OnPreviewGotKeyboardFocusThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), PreLostKeyboardFocusEvent, new KeyboardFocusEventHandler(&Element::OnPreviewLostKeyboardFocusThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), PreTextInputEvent, new KeyboardEventHandler(&EventBuilder::OnPreTextInputThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), TextInputEvent, new KeyboardEventHandler(&EventBuilder::OnTextInputThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), PreKeyDownEvent, new KeyboardEventHandler(&EventBuilder::OnPreKeyDownThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), KeyDownEvent, new KeyboardEventHandler(&EventBuilder::OnKeyDownThunk), false);

        EventControl::RegisterClassHandler(RTTIType(), PreKeyUpEvent, new KeyboardEventHandler(&EventBuilder::OnPreKeyUpThunk), false);
        EventControl::RegisterClassHandler(RTTIType(), KeyUpEvent, new KeyboardEventHandler(&EventBuilder::OnKeyUpThunk), false);

        NameProperty = DpProperty::Register(_U("Name"), RTTIType(), OString::RTTIType(), DpPropMemory::GetPropMeta(OString::EmptyString, PropMetadataOptions::AffectsNone));
        OpacityProperty = DpProperty::Register(_U("Opacity"), RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(1.0), PropMetadataOptions::AffectsRender));
        VisibilityProperty = DpProperty::Register(_U("Visibility"), RTTIType(), Integer::RTTIType(), 
            DpPropMemory::GetPropMeta(VisibilityBox::VisibleBox, 
            PropMetadataOptions::AffectsParentMeasure | PropMetadataOptions::AffectsParentArrange, 
            &Element::OnVisibilityPropChanged));

        VisibilityProperty->SetConvertValueCb(VisibilityConvert::Convert);

        VisualHost::RootSourceProperty = DpProperty::Register(_U("RootSource"), VisualHost::RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));

        AllowDropProperty = DpProperty::Register(_U("AllowDrop"), RTTIType(), Boolean::RTTIType(),DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        ClipProperty = DpProperty::Register(_U("Clip"), RTTIType(), Geometry::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender, &Element::OnClipPropChanged));
        ClipToBoundsProperty = DpProperty::Register(_U("ClipToBounds"), RTTIType(), Boolean::RTTIType(),DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsParentMeasure, &Element::OnClipToBoundsPropChanged));

        IsEnabledProperty = DpProperty::Register(_U("IsEnabled"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsArrange, &Element::OnIsEnabledPropChanged));

        _isVisibleMetadata = DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsMeasure, &Element::OnIsVisiblePropChanged);

        IsVisibleProperty = DpProperty::RegisterReadOnly(_U("IsVisible"), RTTIType(), Boolean::RTTIType(), _isVisibleMetadata);
        IsMouseOverProperty = DpProperty::RegisterReadOnly(_U("IsMouseOver"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        IsFocusedProperty = DpProperty::Register(_U("IsFocused"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender, IsFocused_Changed));
        IsKeyboardFocusedProperty = DpProperty::RegisterReadOnly(_U("IsKeyboardFocused"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        IsKeyboardFocusWithinProperty = DpProperty::RegisterReadOnly(_U("IsKeyboardFocusWithin"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));

        IsMouseCaptureWithinProperty = DpProperty::RegisterReadOnly(_U("IsMouseCaptureWithin"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));

        IsContextMenuProperty = DpProperty::Register(_U("IsContextMenu"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        FocusableProperty = DpProperty::Register(_U("Focusable"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));

        UseLayoutRoundingProperty = DpProperty::Register(_U("UseLayoutRounding"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        IsHitTestVisibleProperty = DpProperty::Register(_U("IsHitTestVisible"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsNone, &Element::OnIsHitTestVisiblePropChanged));

        Tag0Property = DpProperty::Register(_U("Tag0"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
        Tag1Property = DpProperty::Register(_U("Tag1"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
        Tag2Property = DpProperty::Register(_U("Tag2"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
        Tag3Property = DpProperty::Register(_U("Tag3"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));

        DataContextProperty = DpProperty::Register(_U("DataContext"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsInherits, &Element::OnDataContextPropChanged));
        VisualEffectProperty  = DpProperty::Register(_U("VisualEffect"), RTTIType(), Effect::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender, &Element::OnVisualEffectPropChanged));

        IsMouseOverProperty->SetCacheType(0x00000001);

        RenderTransformProperty = DpProperty::Register(_U("RenderTransform"), RTTIType(), Transform::RTTIType(), DpPropMemory::GetPropMeta(Transform::GetIdentity(), PropMetadataOptions::AffectsMeasure, &Element::OnRenderTransformPropChanged));
        //VisualTransformProperty = DpProperty::Register(_U("VisualTransform"), RTTIType(), Transform::RTTIType(), DpPropMemory::GetPropMeta(Transform::GetIdentity(), PropMetadataOptions::AffectsNone));
        RenderTransformOriginProperty = DpProperty::Register(_U("RenderTransformOrigin"), RTTIType(), OfPoint::RTTIType(), DpPropMemory::GetPropMeta(OfPoint::GetEmpty(), PropMetadataOptions::AffectsMeasure));

        CacheLayerProperty = DpProperty::Register(_U("CacheLayer"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone, &Element::OnCacheLayerPropChanged));
    }
}

//------------------------------------------------------------------

void Element::AddTextInput(KeyboardEventHandler* handler)
{
    AddHandler(TextInputEvent, handler, false);
}

void Element::RemoveTextInput(KeyboardEventHandler* handler)
{
    RemoveHandler(TextInputEvent, handler);
}

void Element::AddKeyDown(KeyboardEventHandler* handler)
{
    AddHandler(KeyDownEvent, handler, false);
}
void Element::RemoveKeyDown(KeyboardEventHandler* handler)
{
    RemoveHandler(KeyDownEvent, handler);
}

void Element::AddKeyUp(KeyboardEventHandler* handler)
{
    AddHandler(KeyUpEvent, handler, false);
}

void Element::RemoveKeyUp(KeyboardEventHandler* handler)
{
    RemoveHandler(KeyUpEvent, handler);
}

void Element::AddMouseDown(MouseButtonEventHandler* handler, bool handledToo)
{
    AddHandler(MouseDownEvent, handler, handledToo);
}
void Element::RemoveMouseDown(MouseButtonEventHandler* handler)
{
    RemoveHandler(MouseDownEvent, handler);
}

void Element::AddMouseUp(MouseButtonEventHandler* handler, bool handledToo)
{
    AddHandler(MouseUpEvent, handler, handledToo);
}
void Element::RemoveMouseUp(MouseButtonEventHandler* handler)
{
    RemoveHandler(MouseUpEvent, handler);
}

void Element::AddMouseEnter(MouseButtonEventHandler* handler, bool handledToo)
{
    AddHandler(MouseEnterEvent, handler, handledToo);
}
void Element::RemoveMouseEnter(MouseButtonEventHandler* handler)
{
    RemoveHandler(MouseEnterEvent, handler);
}

void Element::AddMouseMove(MouseButtonEventHandler* handler, bool handledToo)
{
    AddHandler(MouseMoveEvent, handler, handledToo);
}
void Element::RemoveMouseMove(MouseButtonEventHandler* handler)
{
    RemoveHandler(MouseMoveEvent, handler);
}

void Element::AddMouseLeave(MouseButtonEventHandler* handler, bool handledToo)
{
    AddHandler(MouseLeaveEvent, handler, handledToo);
}
void Element::RemoveMouseLeave(MouseButtonEventHandler* handler)
{
    RemoveHandler(MouseLeaveEvent, handler);
}

void Element::AddMouseLeftButtonDown(MouseButtonEventHandler* handler, bool handledToo)
{
    AddHandler(MouseLButtonDownEvent, handler, handledToo);
}
void Element::RemoveMouseLeftButtonDown(MouseButtonEventHandler* handler)
{
    RemoveHandler(MouseLButtonDownEvent, handler);
}

void Element::AddMouseMiddleButtonUp(MouseButtonEventHandler* handler, bool handledToo)
{
    AddHandler(MouseMButtonUpEvent, handler, handledToo);
}
void Element::RemoveMouseMiddleButtonUp(MouseButtonEventHandler* handler)
{
    RemoveHandler(MouseMButtonUpEvent, handler);
}

void Element::AddMouseMiddleButtonDown(MouseButtonEventHandler* handler, bool handledToo)
{
    AddHandler(MouseMButtonDownEvent, handler, handledToo);
}
void Element::RemoveMouseMiddleButtonDown(MouseButtonEventHandler* handler)
{
    RemoveHandler(MouseMButtonDownEvent, handler);
}

void Element::AddMouseLeftButtonUp(MouseButtonEventHandler* handler, bool handledToo)
{
    AddHandler(MouseLButtonUpEvent, handler, handledToo);
}
void Element::RemoveMouseLeftButtonUp(MouseButtonEventHandler* handler)
{
    RemoveHandler(MouseLButtonUpEvent, handler);
}

void Element::AddMouseRightButtonDown(MouseButtonEventHandler* handler, bool handledToo)
{
    AddHandler(MouseRButtonDownEvent, handler, handledToo);
}
void Element::RemoveMouseRightButtonDown(MouseButtonEventHandler* handler)
{
    RemoveHandler(MouseRButtonDownEvent, handler);
}

void Element::AddMouseRightButtonUp(MouseButtonEventHandler* handler, bool handledToo)
{
    AddHandler(MouseRButtonUpEvent, handler, handledToo);
}
void Element::RemoveMouseRightButtonUp(MouseButtonEventHandler* handler)
{
    RemoveHandler(MouseRButtonUpEvent, handler);
}

void Element::AddMouseWheel(MouseButtonEventHandler* handler)
{
    AddHandler(MouseWheelEvent, handler, false);
}

void Element::RemoveMouseWheel(MouseButtonEventHandler* handler)
{
    RemoveHandler(MouseWheelEvent, handler);
}

void Element::AddContextMenu(RoutedEventHandler* handler)
{
    AddHandler(ContextMenuEvent, handler, false);
}
void Element::RemoveContextMenu(RoutedEventHandler* handler)
{
    RemoveHandler(ContextMenuEvent, handler);
}

void Element::AddGotKeyboardFocus(KeyboardFocusEventHandler* handler)
{
    AddHandler(GotKeyboardFocusEvent, handler, false);
}
void Element::RemoveGotKeyboardFocus(KeyboardFocusEventHandler* handler)
{
    RemoveHandler(GotKeyboardFocusEvent, handler);
}

void Element::AddLostKeyboardFocus(KeyboardFocusEventHandler* handler)
{
    AddHandler(LostKeyboardFocusEvent, handler, false);
}
void Element::RemoveLostKeyboardFocus(KeyboardFocusEventHandler* handler)
{
    RemoveHandler(LostKeyboardFocusEvent, handler);
}

void Element::AddCursorSet(RoutedEventHandler* handler)
{
    AddHandler(CursorSetEvent, handler, false);
}
void Element::RemoveCursorSet(RoutedEventHandler* handler)
{
    RemoveHandler(CursorSetEvent, handler);
}

//------------------------------------------------------------------
void Element::AddPreTextInput(KeyboardEventHandler* handler)
{
    AddHandler(PreTextInputEvent, handler, false);
}
void Element::RemovePreTextInput(KeyboardEventHandler* handler)
{
    RemoveHandler(PreTextInputEvent, handler);
}

void Element::AddPreKeyDown(KeyboardEventHandler* handler)
{
    AddHandler(PreKeyDownEvent, handler, false);
}
void Element::RemovePreKeyDown(KeyboardEventHandler* handler)
{
    RemoveHandler(PreKeyDownEvent, handler);
}

void Element::AddPreKeyUp(KeyboardEventHandler* handler)
{
    AddHandler(PreKeyUpEvent, handler, false);
}
void Element::RemovePreKeyUp(KeyboardEventHandler* handler)
{
    RemoveHandler(PreKeyUpEvent, handler);
}

void Element::AddPreMouseDown(MouseButtonEventHandler* handler)
{
    AddHandler(PreMouseDownEvent, handler, false);
}
void Element::RemovePreMouseDown(MouseButtonEventHandler* handler)
{
    RemoveHandler(PreMouseDownEvent, handler);
}

void Element::AddPreMouseUp(MouseButtonEventHandler* handler)
{
    AddHandler(PreMouseUpEvent, handler, false);
}
void Element::RemovePreMouseUp(MouseButtonEventHandler* handler)
{
    RemoveHandler(PreMouseUpEvent, handler);
}

void Element::AddPreMouseMove(MouseButtonEventHandler* handler)
{
    AddHandler(PreMouseMoveEvent, handler, false);
}
void Element::RemovePreMouseMove(MouseButtonEventHandler* handler)
{
    RemoveHandler(PreMouseMoveEvent, handler);
}

void Element::AddPreMouseLeftButtonDown(MouseButtonEventHandler* handler)
{
    AddHandler(PreMouseLButtonDownEvent, handler, false);
}
void Element::RemovePreMouseLeftButtonDown(MouseButtonEventHandler* handler)
{
    RemoveHandler(PreMouseLButtonDownEvent, handler);
}

void Element::AddPreMouseLeftButtonUp(MouseButtonEventHandler* handler)
{
    AddHandler(PreMouseLButtonUpEvent, handler, false);
}
void Element::RemovePreMouseLeftButtonUp(MouseButtonEventHandler* handler)
{
    RemoveHandler(PreMouseLButtonUpEvent, handler);
}

void Element::AddPreMouseMiddleButtonDown(MouseButtonEventHandler* handler)
{
    AddHandler(PreMouseMButtonDownEvent, handler, false);
}
void Element::RemovePreMouseMiddleButtonDown(MouseButtonEventHandler* handler)
{
    RemoveHandler(PreMouseMButtonDownEvent, handler);
}

void Element::AddPreMouseMiddleButtonUp(MouseButtonEventHandler* handler)
{
    AddHandler(PreMouseMButtonUpEvent, handler, false);
}
void Element::RemovePreMouseMiddleButtonUp(MouseButtonEventHandler* handler)
{
    RemoveHandler(PreMouseMButtonUpEvent, handler);
}

void Element::AddPreMouseRightButtonDown(MouseButtonEventHandler* handler)
{
    AddHandler(PreMouseRButtonDownEvent, handler, false);
}
void Element::RemovePreMouseRightButtonDown(MouseButtonEventHandler* handler)
{
    RemoveHandler(PreMouseRButtonDownEvent, handler);
}

void Element::AddPreMouseRightButtonUp(MouseButtonEventHandler* handler)
{
    AddHandler(PreMouseRButtonUpEvent, handler, false);
}
void Element::RemovePreMouseRightButtonUp(MouseButtonEventHandler* handler)
{
    RemoveHandler(PreMouseRButtonUpEvent, handler);
}

void Element::AddPreMouseWheel(MouseButtonEventHandler* handler)
{
    AddHandler(PreMouseWheelEvent, handler, false);
}
void Element::RemovePreMouseWheel(MouseButtonEventHandler* handler)
{
    RemoveHandler(PreMouseWheelEvent, handler);
}

void Element::AddPreGotKeyboardFocus(KeyboardFocusEventHandler* handler)
{
    AddHandler(PreGotKeyboardFocusEvent, handler, false);
}
void Element::RemovePreGotKeyboardFocus(KeyboardFocusEventHandler* handler)
{
    RemoveHandler(PreGotKeyboardFocusEvent, handler);
}

void Element::AddPreLostKeyboardFocus(KeyboardFocusEventHandler* handler)
{
    AddHandler(PreLostKeyboardFocusEvent, handler, false);
}
void Element::RemovePreLostKeyboardFocus(KeyboardFocusEventHandler* handler)
{
    RemoveHandler(PreLostKeyboardFocusEvent, handler);
}

void Element::AddPreCursorSet(RoutedEventHandler* handler)
{
    AddHandler(PreCursorSetEvent, handler, false);
}
void Element::RemovePreCursorSet(RoutedEventHandler* handler)
{
    RemoveHandler(PreCursorSetEvent, handler);
}

//------------------------------------------------------------------

void Element::SetName(const String & name)
{
    SetValue(NameProperty, new OString(name));
}

String Element::GetName()
{
    return GetValue(NameProperty)->ToString();
}

Size Element::GetDesiredSize()
{
    if (GetVisibility() != Visibility::Collapsed)
    {
       return _desiredSize;
    }
    else
    {
        return Size();
    }
}

void Element::SetDesiredSize(Size value)
{
    _desiredSize = value;
}

Float Element::GetOpacity()
{
    return GetValue(OpacityProperty)->ToFloat();
}

void Element::SetOpacity(Float iOpacity)
{
    SetValue(OpacityProperty, new OFloat(iOpacity));
}

bool Element::BlockVisualState()
{
    return false;
}

bool Element::BlockRouteEvent()
{
    return false;
}

int Element::GetPersistId()
{
    return _persistId;
}

bool Element::IsRenderable()
{
    return !ReadFlag(CoreFlags::IsNotRender);
}

//------------------------------------------------------------------------

Element::Element()
    : _flag(0)
    , _evthStore(NULL)
    , _parentIndex(-1)
    , _clip(NULL)
    , _effect(NULL)
    , _persistId(0)
    , _dataCtx(NULL)
    , _itemBase(NULL)
    , _renderLayer(NULL)
    , _ltData(NULL)
{
    Visibility visible = Visibility::Visible;

    _measureSize.cx = -1;
    _visualTans = Transform::GetIdentity();
    _renderTrans = Transform::GetIdentity();
    _evthStore = new EventHandlersStore();

    _visualTans->ref();

    WriteFlag(CoreFlags::IsMeasureDirty, true);
    WriteFlag(CoreFlags::IsArrangeDirty, true);
    //WriteDoFlag(ViewFlags::IsClipToBoundsVF, true);

    SetVisibilityCache(visible);
    WriteFlag(CoreFlags::IsVisibleCache, visible == Visibility::Visible);
}

Element::~Element()
{
    _visualTans->unref();

    Dispose();

    for (int i = 0; i < GetVisualChildrenCount(); ++i)
    {
        Element* child = GetVisualChild(i);
        if (child)
        {
            RemoveVisualChild(child);
        }
    }

    if (NULL != _ltData)
    {
        delete _ltData;
    }
}

void Element::Dispose()
{
    if (NULL != _renderLayer)
    {
        delete _renderLayer;
        _renderLayer = NULL;
    }

    if (_evthStore)
    {
        _evthStore->Clear();
        delete _evthStore;
        _evthStore = NULL;
    }

    DpObject::Dispose();
    //_parent = NULL;
}

WeakRef Element::GetDataContext()
{
    WeakRef dataCtx = _dataCtx;

    if (!dataCtx.IsAlive())
    {
        Element* parent = GetUIParent();
        while (parent != NULL)
        {
            dataCtx = parent->_dataCtx;
            if (dataCtx.IsAlive())
            {
                break;
            }
            parent = parent->GetUIParent();
        }
    }

    return dataCtx;
}

Element* Element::Clone()
{
    // 先拷贝基类
    Element* pElem(DynamicCast<Element>(DpObject::Clone()));

    pElem->_flag = _flag;
    pElem->_parent = _parent;

    //pElem->_size = _size;
    //pElem->_measureSize = _measureSize;
    //pElem->_desiredSize = _desiredSize;
    //pElem->_availableSize = _availableSize;
    //pElem->_finalRect = _finalRect;
    //pElem->_viewPort = _viewPort;

    //_setters->CloneTo(pElem->_setters);

    return pElem;
}

void Element::AddHandler(const RoutedEvent* routedEvent, Handler* handler, bool handledToo)
{
    _evthStore->AddHandler(routedEvent, handler, handledToo);
}

void Element::RemoveHandler(const RoutedEvent* routedEvent, Handler* handler)
{
    _evthStore->RemoveHandler(routedEvent, handler);
}

Object* Element::GetExtent(int iType)
{
    return NULL;
}

void Element::SetUserTag(int index, Object* data)
{
    if (data != NULL)
    {
        data->ref();
    }

    switch (index)
    {
    case 0:
        SetValue(Tag0Property, data);
        break;

    case 1:
        SetValue(Tag1Property, data);
        break;

    case 2:
        SetValue(Tag2Property, data);
        break;

    case 3:
        SetValue(Tag3Property, data);
        break;

    default:
        ;
    }

    if (data != NULL)
    {
        data->unref();
    }
}

Object* Element::GetUserTag(int index)
{
    switch (index)
    {
    case 0:
        return GetValue(Tag0Property);

    case 1:
        return GetValue(Tag1Property);

    case 2:
        return GetValue(Tag2Property);

    case 3:
        return GetValue(Tag3Property);

    default:
        return DpProperty::UnsetValue();
    }
}

Point Element::TransformToAncestor(Element* ancestor)
{
    Point point;
    Point ancestorPoint;
    const Element* parent = this;

    ancestorPoint = ancestor->PointToScreen(Point());
    point = PointToScreen(Point());

    point.x -= ancestorPoint.x;
    point.y -= ancestorPoint.y;

    return point;
}

bool Element::IsAncestorOf(const Element* child) const
{
    bool bIsAncestor = false;
    const Element* parent = child;

    while (parent)
    {
        if (parent == this)
        {
            bIsAncestor = true;
            break;
        }

        parent = parent->GetUIParent();
    }

    return bIsAncestor;
}

bool Element::IsDescendantOf(const Element* ancestor) const
{
    if (NULL == ancestor)
    {
        return false;
    }
    else
    {
        return ancestor->IsAncestorOf(this);
    }
}

Point Element::PointFromScreen(Point point)
{
    point.x -= _canvasOffset.x;
    point.y -= _canvasOffset.y;

    return point;
}

Point Element::PointToScreen(Point point)
{
    point.x += _canvasOffset.x;
    point.y += _canvasOffset.y;

    return point;
}

void Element::BeginAnimation(DpProperty* dp, AnimationTimeline* animation)
{   
    if (dp == NULL)
    {
        throw ArgumentNullException("dp", __FILELINE__);
    }

    AnimationStorage::BeginAnimation(this, dp, animation);
}

Size Element::OnMeasure(const Size& availableSize)
{
    return Size(0, 0);
}

Size Element::MeasureCore(const Size& availableSize)
{
    return Size(0, 0);
}

void Element::Measure(const Size& availableSize)
{
    Element* parent = GetUIParent();

    // 
    // 如果没有初始化，先初始化
    // 
    if (!ReadFlag(CoreFlags::IsInitPending) && (parent == NULL || parent->IsInitialized()))
    {
        RecursiveInit();
    }

    WriteFlag(CoreFlags::IsInvalidateLocked, true);

    if (GetVisibility() != Visibility::Collapsed)
    {
        Size desiredSize = MeasureCore(availableSize);

        _measureSize = availableSize;

        if (desiredSize.cx != _desiredSize.cx || desiredSize.cy != _desiredSize.cy)
        {
            SetDesiredSize(desiredSize);
            InvalidateArrange();
        }
    }

    WriteFlag(CoreFlags::IsInvalidateLocked, false);
    WriteFlag(CoreFlags::IsMeasureDirty, false);
}

void Element::UpdateLayout()
{
    ElementInvoker::CallInvoke(this, NULL, ElemInvokeType::eiUpdateLayout);
}

void Element::InvalidateArrange()
{
    VerifyAccess(__FILELINE__);
    WriteFlag(CoreFlags::IsArrangeDirty, true);
}

void Element::InvalidateArrange(suic::Rect rect)
{
    _finalArrange = rect;
    InvalidateArrange();
}

void Element::InvalidateVisual()
{
    ElementInvoker::CallInvoke(this, NULL, ElemInvokeType::eiInvaliateVisual);
}

void Element::InvalidateForce()
{
    ElementInvoker::CallInvoke(this, NULL, ElemInvokeType::eiInvalidateForce);
}

void Element::UpdateArrange()
{
    if (GetAssigner()->GetThreadId() != Thread::CurrentThreadId())
    {
        ElementInvoker::CallInvoke(this, NULL, ElemInvokeType::eiUpdateArrange);
    }
    else
    {
        Arrange(_finalArrange);
    }
}

void Element::UpdateMeasure()
{
    if (GetAssigner()->GetThreadId() != Thread::CurrentThreadId())
    {
        ElementInvoker::CallInvoke(this, NULL, ElemInvokeType::eiUpdateMeasure);
    }
    else
    {
        Measure(_measureSize);
    }
}

/*Element* Element::FindFixedAncestor()
{
    return NULL;
}

void Element::UpdateAncestorMeasure()
{
    ElementInvoker::CallInvoke(this, NULL, ElemInvokeType::eiUpdateMeasure);
}

void Element::UpdateAncestorArrange()
{
    ElementInvoker::CallInvoke(this, NULL, ElemInvokeType::eiUpdateArrange);
}*/

void Element::InvalidateMeasure()
{
    VerifyAccess(__FILELINE__);
    WriteFlag(CoreFlags::IsMeasureDirty, true);
    InvalidateArrange();
}

void Element::InvalidateMeasure(suic::Size size)
{
    _measureSize = size;
    InvalidateMeasure();
}

void Element::WriteFlag(int key, bool add)
{
    bool tmp = ReadFlag(key);

    if (tmp != add)
    {
        if (add)
        {
            _flag |= key;
        }
        else
        {
            _flag &= ~key;
        }
    }
}

void Element::SetOperationPending(bool val)
{
    WriteDoFlag(ViewFlags::OperationPendingVF, val);
}

bool Element::IsOperationPending() const
{
    return ReadDoFlag(ViewFlags::OperationPendingVF);
}


void Element::InputHitTest(Point pt, ElementPtr& enabledHit, ElementPtr& rawHit)
{
    HitTestCtx hitRes(pt);
    HitTest(&hitRes);

    enabledHit = hitRes.hitRes;
    rawHit = hitRes.hitRes;
}

#pragma optimize("t", on)

void Element::OnArrange(const Size& arrangeSize)
{
}

void Element::ArrangeCore(const Rect& finalRect)
{
    _offset.x = finalRect.left;
    _offset.y = finalRect.top;
    _renderSize = finalRect.ToSize();
    _finalArrange = finalRect;

    if (GetUIParent() != NULL)
    {
        _canvasOffset = GetUIParent()->GetCanvasOffset();
        _canvasOffset.x += _offset.x;
        _canvasOffset.y += _offset.y;
    }
    else
    {
        _canvasOffset.x = 0;
        _canvasOffset.y = 0;
    }
}

void Element::OnRender(Drawing* drawing)
{

}

void Element::Render(Drawing* drawing)
{
}

void Element::RenderChildren(Drawing* drawing)
{
    int iCount = GetVisibleChildrenCount();

    for (int i = 0; i < iCount; ++i)
    {
        Element* child = GetVisibleChild(i);

        if (NULL != child && child->IsVisible())
        {
            child->Render(drawing);
        }
    }
}

//void Element::DoRender(Drawing * drawing)
//{
//    //
//    // 需要进行重新布局
//    //
//    if (ReadFlag(CoreFlags::IsArrangeDirty))
//    {
//        UpdateArrange();
//    }
//
//    Rect clip(0, 0, GetActualWidth(), GetActualHeight());
//
//    // 
//    // 转换坐标(元素偏移都是相对父元素，需要转换到窗口坐标)
//    // 
//    drawing->PushTranslate(GetVisualOffset());
//    //drawing->Translate(GetCanvasOffset().x, GetCanvasOffset().y);
//
//    //
//    // 设置裁剪区域
//    //
//    if (drawing->PushClip(&clip))
//    {
//        if (_round.cx > 0)
//        {
//            Point offset(drawing->GetOffset());
//            HRGN hrgn = CreateRoundRectRgn(offset.x, offset.y
//                , offset.x + GetActualWidth(), offset.y + GetActualHeight(), _round.cx, _round.cy);
//            drawing->SetClipRgn((Handle)(DWORD_PTR)hrgn);
//        }
//
//        //
//        // 元素是否需要绘制(未设置标记IsNotRender)
//        //
//        if (!ReadFlag(CoreFlags::IsNotRender))
//        {
//            // 执行绘制
//            OnRender(drawing);
//            // 描边
//            OnStroke(drawing);
//        }
//
//        //
//        // 绘制元素子元素
//        //
//        DispatchRender(drawing);
//
//        if (_maskColor > 0)
//        {
//            drawing->FillRect(&clip, _maskColor);
//        }
//    }
//    else
//    {
//        Debug::Trace(_U("element is not in clip range[%s]!\n"), GetTypeName().c_str());
//    }
//
//    drawing->Pop();
//    drawing->PopTranslate(GetVisualOffset());
//}

void Element::SetMeasureData(const MeasureData& measureData)
{
    _measureData = measureData;
}

void Element::MeasureChild(Element* child, Size constraint)
{
    SetMeasureDataOnChild(child, constraint);
    child->Measure(constraint);
}

void Element::SetMeasureDataOnChild(Element* child, Size constraint)
{
    MeasureData* measureData = child->GetMeasureData();
    measureData->SetViewPort(_measureData.GetViewPort());
    measureData->SetAvailableSize(constraint);
}

HitTestCtx::HitTestCtx(Point pt) 
    : hitPoint(pt)
    , hitFilter(HitTestFilterAction::Continue)
{
    trans.Resize(4);
    trans.Add(pt);
}

HitTestCtx::~HitTestCtx()
{
}

static void _TransformPoint(const SkMatrix& m, fPoint& pt)
{
    float mx = m.getScaleX();
    float my = m.getScaleY();
    float kx = m.getSkewX();
    float ky = m.getSkewY();

    int m00 = SkScalarAs2sCompliment(mx);
    int m01 = SkScalarAs2sCompliment(kx);
    int m10 = SkScalarAs2sCompliment(ky);
    int m11 = SkScalarAs2sCompliment(my);

    float fx = pt.x;
    float fy = pt.y;

    if (m01 != 0 || m10 != 0)
    {
        float fm = mx * my - kx * ky;
        pt.x = (fx * my - fy * kx) / (fm);
        pt.y = (fx * ky - fy * mx) / (fm * -1);
    }
    else if (m00 != 0 || m11 != 0)
    {
        pt.y = pt.y / my;
        pt.x = pt.x / mx;
    }
}

static void _TransformRect(const SkMatrix& m, fRect& rect)
{
    float mx = m.getScaleX();
    float my = m.getScaleY();
    float kx = m.getSkewX();
    float ky = m.getSkewY();

    int m00 = SkScalarAs2sCompliment(mx);
    int m01 = SkScalarAs2sCompliment(kx);
    int m10 = SkScalarAs2sCompliment(ky);
    int m11 = SkScalarAs2sCompliment(my);

    float sx = rect.left;
    float sy = rect.top;

    if (m01 != 0 || m10 != 0)
    {
        rect.left = sx * ky + (sy * my);
        rect.top = sx * mx + (sy * kx);

        sx = rect.right;
        sy = rect.bottom;

        rect.right = sx * ky + (sy * my);
        rect.bottom = sx * mx + (sy * kx);
    }
    else if (m00 != 0 || m11 != 0)
    {
        rect.top = rect.top / my;
        rect.left = rect.left / mx;
        rect.bottom = rect.bottom / my;
        rect.right = rect.right / mx;
    }
}

void HitTestCtx::EnterHitTest(Element* elem)
{
    LayoutTransformInfo* data = elem->GetLayoutTransformInfo();
    if (NULL != data)
    {
        int count = trans.GetCount();
        fPoint pt = trans[count - 1].TofPoint();
        elem->TransformPoint(pt);
        trans.Add(pt.ToPoint());
    }
}

void HitTestCtx::ExitHitTest(Element* elem)
{
    if (NULL != elem->GetLayoutTransformInfo())
    {
        int index = trans.GetCount() - 1;
        trans.RemoveAt(index);
    }
}

Size Element::GetRealSize()
{
    fRect rect(0, 0, GetRenderSize().Width(), GetRenderSize().Height());
    TransformAncestorRect(rect, NULL);
    return rect.ToRect().ToSize();
}

bool Element::HitTest(HitTestCtx* hitResult, Rect rect)
{
    Point pt;
    bool ptIn = false;
    int count = hitResult->trans.GetCount();
    Point ptTrans = hitResult->trans[count - 1];

    pt.x = hitResult->hitPoint.x - rect.left;
    pt.y = hitResult->hitPoint.y - rect.top;

    ptIn = rect.PointIn(ptTrans);

    if (ptIn)
    {
        if (GetClip() != NULL)
        {
            return GetClip()->Contains(pt);
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

bool Element::HitTestCore(HitTestCtx* hitResult, bool onlySelf)
{
    Rect rect;

    if (onlySelf || IsClipToBounds())
    {
        rect = Rect(GetCanvasOffset(), GetRenderSize());
    }
    else
    {
        rect = GetDescendantBounds();
    }

    return HitTest(hitResult, rect);
}

void Element::UpdateInheritanceContext(const EffectiveValue& val)
{

}

void Element::RemoveInheritanceContext(const DpProperty* dp)
{

}

#pragma optimize("", on)

void Element::Arrange(const Rect& arrangeRect)
{
    WriteFlag(CoreFlags::IsInvalidateLocked, true);

    if (GetVisibility() != Visibility::Collapsed)
    {
        Point offset = GetVisualOffset();
        Size oldSize = GetRenderSize();

        ArrangeCore(arrangeRect);

        if (offset != GetVisualOffset() || 
            oldSize.cx != GetActualWidth() || 
            oldSize.cy != GetActualHeight())
        {
            SizeChangedInfo e(oldSize, GetRenderSize(), offset, GetVisualOffset());
            OnRenderSizeChanged(e);
            if (LayoutUpdated.safe_bool())
            {
                LayoutUpdated(this, &EventArg::Empty);
            }
        }

        if (IsKeyboardFocused())
        {
            AdornerLayer* adornerLayer = AdornerLayer::GetAdornerLayer(this);
            if (NULL != adornerLayer)
            {
                WriteFlag(CoreFlags::IsArrangeDirty, false);
                adornerLayer->UpdateArrange();
                adornerLayer->InvalidateVisual();
            }
        }
    }

    if (GetAssigner()->GetBindingWorker()->GetTaskCount() > 0 && IsInitialized())
    {
        Assigner::Current()->GetBindingWorker()->Run();
    }

    WriteFlag(CoreFlags::IsInvalidateLocked, false);
    WriteFlag(CoreFlags::IsArrangeDirty, false);
}

void Element::SetClipToBounds(bool val)
{
    // WriteViewFlag(ViewFlags::IsClipToBounds, value);
    SetValue(ClipToBoundsProperty, BOOLTOBOOLEAN(val));
}

bool Element::IsAllowDrop()
{
    return GetValue(AllowDropProperty)->ToBool();
}

void Element::SetAllowDrop(bool val)
{
    SetValue(AllowDropProperty, BOOLTOBOOLEAN(val));
}

Geometry* Element::GetClip()
{
    return _clip;
}

void Element::SetClip(Geometry* clip)
{
    SetValue(ClipProperty, clip);
}

//Rect Element::ComputeClipBound()
//{
//    return Rect(Point(), GetRenderSize());
//}

void Element::SetContext(const String& key, Object* value)
{
    ObjectPtr val(value);
    VisualHostPtr obj = HwndSubclass::MatchRenderSource(this);

    if (obj)
    {
        obj->SetValue(key, value);
    }
}

Effect* Element::GetVisualEffect()
{
    return _effect;
}

void Element::SetVisualEffect(Effect* val)
{
    SetValue(VisualEffectProperty, val);
}

bool Element::Focus()
{
    bool bAuto = setAutoDelete(false);

    if (Keyboard::Focus(this) != this)
    {
        setAutoDelete(bAuto);
        return false;
    }
    else
    {
        if (IsFocusable() && IsEnabled())
        {
            DpObject* focusScope = FocusManager::GetFocusScope(this);
            if (FocusManager::GetFocusedElement(focusScope) == NULL) 
            {
                FocusManager::SetFocusedElement(focusScope, this);
            }
        }

        setAutoDelete(bAuto);

        return true;
    }
}

void Element::InnerEnableChildren(Element* elem, bool val)
{
    for (int i = 0; i < elem->GetVisualChildrenCount(); ++i)
    {
        Element* pElem(elem->GetVisualChild(i));

        if (NULL != pElem)
        {
            if (val == !pElem->IsInnerEnabled())
            {
                pElem->WriteFlag(CoreFlags::IsInnerDisabled, !val);

                if (val)
                {
                    pElem->SetValue(Element::IsEnabledProperty, Boolean::True);
                }
                else
                {
                    pElem->SetValue(Element::IsEnabledProperty, Boolean::False);
                }
            }

            InnerEnableChildren(pElem, val);
        }
    }
}

void Element::Enable(bool val)
{
    ElementInvoker::CallInvoke(this, BOOLTOBOOLEAN(val), ElemInvokeType::eiEnableElement);
}

Visibility Element::GetVisibility() const
{
    return GetVisibilityCache();
}

void Element::SetVisibility(Visibility val)
{
    ElementInvoker::CallInvoke(this, VisibilityBox::From(val), ElemInvokeType::eiSetVisibility);
}

void Element::UpdateIsVisibleCache()
{
    bool flag = GetVisibility() == Visibility::Visible;

    if (flag)
    {
        bool flagp = false;

        if (GetUIParent() != NULL)
        {
            flagp = GetUIParent()->IsVisible();
        }
        else
        {
            flagp = true;
        }

        if (!flagp)
        {
            flag = false;
        }
    }

    if (!flag)
    {
        if (Keyboard::GetFocusedElement() == this)
        {
            Element* parent = GetUIParent();
            while (parent != NULL)
            {
                if (parent->IsVisible() && parent->IsFocusable())
                {
                    break;
                }
                parent = parent->GetUIParent();
            }
            Keyboard::Focus(parent);
        }
    }

    if (flag != IsVisible())
    {
        WriteFlag(CoreFlags::IsVisibleCache, flag);
        IsVisibleChangedEventArg ev(flag);
        DpPropChangedEventArg e(IsVisibleProperty, _isVisibleMetadata, BOOLTOBOOLEAN(!flag), BOOLTOBOOLEAN(flag));

        NotifyDpPropertyChanged(&e);
        OnIsVisibleChanged(&ev);
    }
}

Visibility Element::GetVisibilityCache() const
{
    if ((_viewflag & ViewFlags::VisibilityCache_Visible) == ViewFlags::VisibilityCache_Visible)
    {
        return Visibility::Visible;
    }
    else  if ((_viewflag & ViewFlags::VisibilityCache_TakesSpace) == ViewFlags::VisibilityCache_TakesSpace)
    {
        return Visibility::Hidden;
    }
    else
    {
        return Visibility::Collapsed;
    }
}

void Element::SetVisibilityCache(Visibility val)
{
    switch (val)
    {
    case Visibility::Visible:
        WriteDoFlag(ViewFlags::VisibilityCache_Visible, true);
        WriteDoFlag(ViewFlags::VisibilityCache_TakesSpace, false);
        break;

    case Visibility::Hidden:
        WriteDoFlag(ViewFlags::VisibilityCache_Visible, false);
        WriteDoFlag(ViewFlags::VisibilityCache_TakesSpace, true);
        break;

    case Visibility::Collapsed:
        WriteDoFlag(ViewFlags::VisibilityCache_Visible, false);
        WriteDoFlag(ViewFlags::VisibilityCache_TakesSpace, false);
        break;
    }
}

void Element::SignalDesiredSizeChange()
{
    Element* parent = GetUIParent();

    if (parent != NULL)
    {
        // parent->OnChildDesiredSizeChanged(this);
    }
}

void Element::EnsureVisible()
{
    WriteFlag(CoreFlags::IsVisibleCache, true);
}

void Element::EnsureInvisible(bool collapsed)
{
    if (!ReadFlag(CoreFlags::IsCollapsed) && collapsed)
    {
        WriteFlag(CoreFlags::IsCollapsed, true);
        SignalDesiredSizeChange();
    }
    else if (ReadFlag(CoreFlags::IsCollapsed) && !collapsed)
    {
        WriteFlag(CoreFlags::IsCollapsed, false);
        SignalDesiredSizeChange();
    }
}

bool Element::IsLogicalElement()
{
    if (GetUIParent())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Element::InvalidateDataContextDeps(Object* data, bool invalidateChild)
{
    if (!_dataCtx.IsAlive() || data == _dataCtx.GetTarget())
    {
        UpdateDepProp(DataContextProperty);

        if (invalidateChild)
        {
            int iCount = GetVisualChildrenCount();
            for (int i = 0; i < iCount; ++i)
            {
                Element* childfe = GetVisualChild(i);
                if (childfe != NULL)
                {
                    childfe->InvalidateDataContextDeps(data, invalidateChild);
                }
            }
        }
    }
}

void Element::InvalidateDepProp(DpProperty* dp, bool invalidateChild)
{
    UpdateDepProp(dp);

    if (invalidateChild)
    {
        int iCount = GetVisualChildrenCount();
        for (int i = 0; i < iCount; ++i)
        {
            Element* childfe = GetVisualChild(i);
            if (childfe != NULL)
            {
                childfe->InvalidateDepProp(dp, invalidateChild);
            }
        }
    }
}

void Element::InvalidateBinding(bool invalidateChild)
{
    InvalidateDepProp(DataContextProperty, invalidateChild);
}

void Element::RaiseEvent(RoutedEventArg* e)
{
    VerifyAccess(__FILELINE__);
    EventBuilder eb;
    eb.RaisedEvent(this, e);
}

bool Element::IsInnerEnabled()
{
    return (!ReadFlag(CoreFlags::IsInnerDisabled) && GetValue(IsEnabledProperty)->ToBool());
}

bool Element::IsEnabled()
{
    bool bEnable = true;
    Element* pTemp = const_cast<Element*>(this);

    while (pTemp)
    {
        if (!pTemp->IsInnerEnabled())
        {
            bEnable = false;
            break;
        }

        pTemp = pTemp->GetUIParent();
    }
    
    return bEnable;
}

bool Element::IsFocused()
{
    return GetValue(IsFocusedProperty)->ToBool();
}

bool Element::IsKeyboardFocused() const
{
    return (Keyboard::GetFocusedElement() == this);
}

bool Element::IsKeyboardFocusWithin() const
{
    return ReadFlag(CoreFlags::IsKeyboardFocusWithinCache);
}

bool Element::IsFocusable()
{
    return GetValue(FocusableProperty)->ToBool();    
}

void Element::SetFocusable(bool value)
{
    SetValue(FocusableProperty, value ? Boolean::True : Boolean::False);
}

bool Element::IsRightToLeft()
{
    return false;
}

bool Element::IsMouseOver()
{
    return ReadFlag(CoreFlags::IsMouseOverWithinCache);
}

void Element::TransformPoint(fPoint& pt)
{
    Point offset = GetCanvasOffset();
    LayoutTransformInfo* data = GetLayoutTransformInfo();

    if (NULL != data)
    {
        pt.x += data->offset.x - offset.x;
        pt.y += data->offset.y - offset.y;
        _TransformPoint(data->transform.GetMatrixInfo()->m, pt);
        pt.x += offset.x;
        pt.y += offset.y;
    }
}

Element* Element::TransformAncestorRect(fRect& rect, Element* ancestor)
{
    Element* fe = this;
    Element* root = this;
    LayoutTransformInfo* data = NULL;

    while (NULL != fe)
    {
        data = fe->GetLayoutTransformInfo();
        if (data != NULL)
        {
            Point offset(fe->GetCanvasOffset());
            rect.Offset(-offset.x, -offset.y);
            rect = data->transform.TransformRect(rect);
            rect.Offset(-data->offset.x + offset.x, -data->offset.y + offset.y);
        }
        root = fe;
        fe = fe->GetUIParent();
        if (ancestor == fe)
        {
            break;
        }
    }

    return root;
}

Element* Element::TransformAncestorPoint(fPoint& pt, Element* ancestor)
{
    Element* fe = this;
    Element* root = this;
    LayoutTransformInfo* layInfo = NULL;
    Array<Element*, false> trans;

    trans.Resize(4);

    while (fe != NULL)
    {
        layInfo = fe->GetLayoutTransformInfo();
        if (NULL != layInfo)
        {
            trans.Add(fe);
        }

        root = fe;
        fe = fe->GetUIParent();

        if (ancestor == fe)
        {
            break;
        }
    }

    for (int i = trans.GetCount() - 1; i >= 0; --i)
    {
        trans[i]->TransformPoint(pt);
    }

    return root;
}

bool Element::HitTestMouse(Point pt)
{
    fPoint hitPt = pt.TofPoint();
    Rect rect(GetCanvasOffset(), GetRenderSize());
    TransformAncestorPoint(hitPt, NULL);
    return rect.PointIn(hitPt.ToPoint());
}

bool Element::IsMouseDirectlyOver()
{
    return Mouse::GetDirectlyOver() == this;
}

bool Element::IsMouseCaptured()
{
    return Mouse::GetCaptured() == this;
}

bool Element::IsMouseCaptureWithin()
{
    return ReadFlag(CoreFlags::IsMouseCapturedWithinCache);
}

bool Element::IsVisible()
{
    return ReadFlag(CoreFlags::IsVisibleCache);
}

void Element::InvalidateRect(Rect* lprc, bool bForce)
{
    bool autoDel = setAutoDelete(false);
    InvManager::InvalidateElement(this, lprc, bForce);
    setAutoDelete(autoDel);
}

void Element::DoInvalidate(Element* sender, InvalidateEventArg& e)
{
    if (e.IsMeasure())
    {
        sender->InvalidateMeasure();
    }
    if (e.IsArrange())
    {
        sender->InvalidateArrange();
    }

    if (e.IsRender())
    {
        if (e.IsForce())
        {
            sender->InvalidateForce();
        }
        else
        {
            sender->InvalidateVisual();
        }
    }
}

Float Element::GetDpiScaleX()
{
    if (SystemParameters::DpiX != 0x60)
    {
        return (((Float) SystemParameters::DpiX) / 96.0f);
    }
    return 1.0;
}

Float Element::GetDpiScaleY()
{
    if (SystemParameters::DpiY != 0x60)
    {
        return (((Float) SystemParameters::DpiY) / 96.0f);
    }
    return 1.0;
}

Float Element::RoundLayoutValue(Float val, Float dpiScale)
{
    if (!FloatUtil::AreClose(dpiScale, 1.0))
    {
        Float num = Math::Round((Float)(val * dpiScale)) / dpiScale;
        if ((!FloatUtil::IsNaN(num) && !FloatUtil::IsInfinity(num)))
        {
            return num;
        }
        return val;
    }
    return Math::Round(val);
}

void Element::EnsureDpiScale()
{
    if (_setDpi)
    {
        _setDpi = false;
        _dpiScaleX = SystemParameters::DpiX / 96.0;
        _dpiScaleY = SystemParameters::DpiY / 96.0;
    }
}

Size Transform::TransformSize(const Matrix& m, Size size)
{
    SkRect rect;
    rect.set(0, 0, size.cx, size.cy);
    m.GetMatrixInfo()->m.mapRect(&rect);
    return Size(rect.width(), rect.height());
}

void Element::Invalidate(DpProperty* dp, int iAction, bool force)
{
    if (iAction == 0)
    {
        InvalidateEventArg e(true, true, true, force);

        e.SetRoutedEvent(InvalidateEvent);
        e.SetProp(dp);

        RaiseEvent(&e);

        if (!e.IsHandled())
        {
            UpdateLayout();
        }
    }
    else
    {
        bool mesuare = (InvTags::Measure & iAction) == InvTags::Measure;
        bool arrange = (InvTags::Arrange & iAction) == InvTags::Arrange;
        bool render = (InvTags::Render & iAction) == InvTags::Render;

        InvalidateEventArg e(mesuare, arrange, render, force);

        e.SetRoutedEvent(InvalidateEvent);
        e.SetProp(dp);

        RaiseEvent(&e);

        if (!e.IsHandled())
        {
            if (mesuare)
            {
                InvalidateMeasure();
            }

            if (arrange)
            {
                InvalidateArrange();
            }

            if (render)
            {
                if (force)
                {
                    InvalidateForce();
                }
                else
                {
                    InvalidateVisual();
                }
            }
        }
    }
}

void Element::Invalidate(int iAction, bool force)
{
    Invalidate(NULL, iAction, force);
}

void Element::SetCaptureMouse(int iMode)
{
    Mouse::SetCaptured(this);
}

void Element::ReleaseCaptureMouse()
{
    bool bAuto = setAutoDelete(false);
    if (Mouse::GetCaptured() == this)
    {
        Mouse::SetCaptured(NULL);
    }
    setAutoDelete(bAuto);
}

String Element::ToString()
{
    return GetRTTIType()->typeName;
}

bool Element::IsHitTestVisible()
{
    return !(ReadFlag(CoreFlags::IsHitTestHidden));
}

void Element::SetIsHitTestVisible(bool val)
{
    SetValue(IsHitTestVisibleProperty, BOOLTOBOOLEAN(val));
}

bool Element::IsMouseDownVisible()
{
    return !(ReadFlag(CoreFlags::IsMouseDownHidden));
}

void Element::SetIsMouseDownVisible(bool val)
{
    if (val != IsMouseDownVisible())
    {
        WriteFlag(CoreFlags::IsMouseDownHidden, !val);
    }
}

Object* Element::GetContainerItem() const
{
    ItemEntry* itemb = GetItemEntry();
    return (itemb ? itemb->GetItem() : NULL);
}

ItemEntry* Element::GetItemEntry() const
{
    return (ItemEntry*)(_itemBase.GetTarget());
}

void Element::SetItemEntry(ItemEntry* item)
{
    _itemBase = item;
}

int Element::HitTestFilter()
{
    int iFilter = GetHitTestFilterAction();

    if (HitTestFilterAction::ContinueSkipSelf != iFilter)
    {
        if (!IsHitTestVisible())
        {
            iFilter = HitTestFilterAction::ContinueSkipSelf;
        }
    }
    
    return iFilter;
}

void Element::RenderOpen(RenderContext* renderCtx)
{
    renderCtx->Open(GetActualWidth(), GetActualHeight(), 0);
}

//--------------------------------------------------

RenderContext::RenderContext()
    : bitmap(NULL)
    , drawing(NULL)
{

}

RenderContext::~RenderContext()
{
    Close();
}

Drawing* RenderContext::Open(int w, int h, int iType)
{
    Close();

    bitmap = new Bitmap();
    bitmap->Create(w, h, 32);
    drawing = new SkiaDrawing(true, bitmap, fRect(0, 0, w, h));
    return drawing;
}

Drawing* RenderContext::Open(Bitmap* bmp, int iType)
{
    Close();

    bitmap = bmp;
    drawing = new SkiaDrawing(true, bitmap, fRect(0, 0, bitmap->Width(), bitmap->Height()));
    return drawing;
}

void RenderContext::Close()
{
    if (bitmap != NULL)
    {
        delete bitmap;
        bitmap = NULL;
    }

    if (NULL != drawing)
    {
        delete drawing;
        drawing = NULL;
    }
}

//--------------------------------------------------

void Element::OnPreviewSetCursor(CursorEventArg* e)
{
}

void Element::OnSetCursor(CursorEventArg* e)
{
}

Element* Element::FindName(const String& name)
{
    if (name.Equals(GetName()))
    {
        return this;
    }

    int iCount = GetLogicalChildrenCount();

    for (int i = 0; i < iCount; ++i)
    {
        Element* elem(GetLogicalChild(i));
        if (elem)
        {
            Element* ret(elem->FindName(name));

            if (ret)
            {
                return ret;
            }
        }
    }

    return NULL;
}

void Element::OnMouseDown(MouseButtonEventArg* e)
{
}

void Element::OnMouseUp(MouseButtonEventArg* e)
{
}

void Element::OnMouseEnter(MouseButtonEventArg* e)
{

}

void Element::OnMouseMove(MouseButtonEventArg* e)
{

}

void Element::OnMouseLeave(MouseButtonEventArg* e)
{
    // BeginAnimation(OpacityProperty, NULL);
    // DoubleAnimation* pAni = new DoubleAnimation(1, 0, Duration(2000), FillBehavior::fbStop);
    // BeginAnimation(OpacityProperty, pAni);
}

void Element::OnMouseWheel(MouseWheelEventArg* e)
{
}

void Element::OnContextMenu(ContextMenuEventArg* e)
{
}

void Element::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
}

int lua_LButtonUp(String strName, bool bIn)
{
    /*if (strName.Empty())
    {
        return 1;
    }

    lua_State* l = GetLuaState();
    int iRet = LUA_ERRRUN;
    int iPrev = lua_gettop(l); //lua_isfunction

    //lua_pushusertype

    //
    // 获取函数名称
    //
    lua_getglobal(l, "OnLButtonUp");

    int iCur = lua_gettop(l);

    if (iCur > iPrev)
    {
        //
        // 压入一个参数
        //
        int iIn = (bIn ? 1 : 0);

        MString name(strName.c_str());

        lua_pushstring(l, name.c_str());
        lua_pushnumber(l, iIn);

        try
        {
            //
            // 使用1个参数，0个返回值调用OnLButtonUp
            //
            //lua_call(l, 1, 0);
            iRet = lua_pcall(l, 2, 0, 0);
        
            //tonumber tostring

            //int sum = (int)lua_tonumber(l, -1);
            //清掉返回值  
            //lua_pop(l, 1);
        }
        catch (...)
        {
        }
    }*/

    return 0;//iRet;
}

void Element::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    String strName = GetName();

    if (lua_LButtonUp(strName, IsMouseOver()) != 0)
    {
        ;
    }
}

void Element::OnMouseMiddleButtonDown(MouseButtonEventArg* e)
{
}

void Element::OnMouseMiddleButtonUp(MouseButtonEventArg* e)
{
}

void Element::OnMouseRightButtonDown(MouseButtonEventArg* e)
{
}

void Element::OnMouseRightButtonUp(MouseButtonEventArg* e)
{
}

// 鼠标预览事件定义

void Element::OnPreviewMouseDown(MouseButtonEventArg* e)
{
}

void Element::OnPreviewMouseUp(MouseButtonEventArg* e)
{
}

void Element::OnPreviewMouseMove(MouseButtonEventArg* e)
{
}

void Element::OnPreviewMouseWheel(MouseButtonEventArg* e)
{
}

void Element::OnPreviewMouseLeftButtonDown(MouseButtonEventArg* e)
{
}

void Element::OnPreviewMouseLeftButtonUp(MouseButtonEventArg* e)
{
}

void Element::OnPreviewMouseMiddleButtonDown(MouseButtonEventArg* e)
{
}

void Element::OnPreviewMouseMiddleButtonUp(MouseButtonEventArg* e)
{
}

void Element::OnPreviewMouseRightButtonDown(MouseButtonEventArg* e)
{
}

void Element::OnPreviewMouseRightButtonUp(MouseButtonEventArg* e)
{
}

// 拖动事件定义

void Element::OnDragEnter(DragEventArg* e)
{
}

void Element::OnDragLeave(DragEventArg* e)
{
}

void Element::OnDragOver(DragEventArg* e)
{
}

void Element::OnDrop(DragEventArg* e)
{
}

void Element::OnPreviewDragEnter(DragEventArg* e)
{

}

void Element::OnPreviewDragLeave(DragEventArg* e)
{

}

void Element::OnPreviewDragOver(DragEventArg* e)
{

}

void Element::OnPreviewDrop(DragEventArg* e)
{

}

void Element::OnTextInput(KeyboardEventArg* e)
{
}

void Element::OnKeyDown(KeyboardEventArg* e)
{
}

void Element::OnKeyUp(KeyboardEventArg* e) 
{
}

void Element::OnImeEvent(MessageEventArg* e)
{

}

void Element::OnInputEvent(MessageEventArg* e)
{

}

// 预定义键盘事件定义

void Element::OnPreviewTextInput(KeyboardEventArg* e)
{
}

void Element::OnPreviewKeyDown(KeyboardEventArg* e)
{
}

void Element::OnPreviewKeyUp(KeyboardEventArg* e)
{
}

// 焦点事件

void Element::OnPreviewGotKeyboardFocus(KeyboardFocusEventArg* e)
{

}

void Element::OnPreviewLostKeyboardFocus(KeyboardFocusEventArg* e)
{

}

void Element::OnGotKeyboardFocus(KeyboardFocusEventArg* e)
{
}

void Element::OnLostKeyboardFocus(KeyboardFocusEventArg* e)
{
}

void Element::OnLostMouseCapture(MouseButtonEventArg* e)
{
}

void Element::OnGotMouseCapture(MouseButtonEventArg* e)
{
}

void Element::OnGotFocus(RoutedEventArg* e)
{
    e->SetRoutedEvent(Element::GotFocusEvent);
    RaiseEvent(e);
}

void Element::OnLostFocus(RoutedEventArg* e)
{
    e->SetRoutedEvent(Element::LostFocusEvent);
    RaiseEvent(e);
}

void Element::OnChildrenChanged(Element* kidAdded, Element* kidRemoved)
{
}

void Element::OnVisualParentChanged(const Element* oldParent, Element* newParent)
{
    if (NULL != newParent)
    {
        if (!newParent->IsVisible() && IsVisible())
        {
            UpdateIsVisibleCache();
            InvalidateIsVisiblePropertyOnChildren(IsVisibleProperty);
        }
    }
}

void Element::OnRenderSizeChanged(SizeChangedInfo& sizeInfo)
{
    WriteFlag(CoreFlags::IsMeasureDirty, true);
    WriteFlag(CoreFlags::IsArrangeDirty, true);
}

void Element::OnRenderSizeModeChanged(SizeModeChangedEventArg* e)
{
    WriteFlag(CoreFlags::IsSizeModing, !e->IsExitMode());
}

void Element::OnInitialized(EventArg* e)
{
}

void Element::PrivateInitialized()
{
    _descendantBounds.left = 0;
    _descendantBounds.top = 0;
    _descendantBounds.right = _renderSize.cx;
    _descendantBounds.bottom = _renderSize.cy;
}

void Element::BeginInit()
{
    if (ReadFlag(CoreFlags::IsInitPending))
    {
        throw InvalidOperationException(_U("InitPending is in processing!"), __FILELINE__);
    }

    WriteFlag(CoreFlags::IsInitPending, true);
    Object* visible = GetValue(VisibilityProperty);

    if (DpProperty::UnsetValue() != visible)
    {
        Visibility v = (Visibility)visible->ToInt();
        SetVisibilityCache(v);
        WriteFlag(CoreFlags::IsVisibleCache, v == Visibility::Visible);
    }
}

void Element::EndInit()
{
    WriteFlag(CoreFlags::IsInitPending, false);
}

void Element::RecursiveInit()
{
    if (!IsInitialized())
    {
        BeginInit();
        EndInit();
    }
}

void Element::OnLoaded(LoadedEventArg* e)
{
}

void Element::OnUnloaded(LoadedEventArg* e)
{
}

void Element::OnTargetUpdated(DataTransferEventArg* e)
{
}

void Element::OnIsVisibleChanged(IsVisibleChangedEventArg* e)
{
}

void Element::OnIsEnabledChanged(IsEnabledChangedEventArg* e)
{
}

void Element::OnHitTest(HitResultEventArg* e) 
{
}

void Element::OnInvokerArrived(InvokerArg* arg)
{
    InvokerEventArg e(this, arg->GetWhat(), arg->GetData());
    e.SetRoutedEvent(InvokerEvent);
    RaiseEvent(&e);
}

void Element::OnAccessKey(AccessKeyEventArg* e)
{

}

void Element::OnKeyboardFocusWithinChanged(bool withinFocus)
{
    ;
}

void Element::OnMouseCaptureWithinChanged(bool withinFocus)
{

}

void Element::OnDataContextChanged(DpPropChangedEventArg* e)
{

}

void Element::SetDataContext(Object* value)
{
    SetValue(DataContextProperty, value);
}

/*void Element::SetDataContext(Object* value, bool invalidateChild)
{
    SetDataContext(value);
    InvalidatePropDependents(DataContextProperty, invalidateChild);
}*/

void Element::AddVisualChild(Element* child)
{
    if (NULL != child)
    {
        Debug::Assert(child != this, _U("Parent of child can't equals its self!\n"));

        bool inDesignMode = InDesignMode();
        Element* oldParent(child->GetUIParent());
        child->_parent = WeakRef(this);

        if (inDesignMode)
        {
            DesignHelper::SetInDesignMode(child);
        }

        if (IsInitialized() && !child->IsInitialized())
        {
            if (!child->ReadFlag(CoreFlags::IsInitPending))
            {
                child->RecursiveInit();
            }
        }

        if (IsLoaded() && !child->IsLoaded())
        {
            RecursiveLoaded(child);
        }

        child->OnVisualParentChanged(oldParent, this);
    }
}

void Element::RemoveVisualChild(Element* child)
{
    if (child != NULL)
    {
        if (child->_parent == this)
        {
            Element* oldParent(child->GetUIParent());
            child->_parent = WeakRef();

            if (child->IsLoaded())
            {
                RecursiveUnloaded(child);
            }

            child->OnVisualParentChanged(oldParent, NULL);
        }
        else
        {
            Debug::Trace(_U("Parent of child is not same!\n"));
        }
    }
    else
    {
        
    }
}

Transform* Element::GetRenderTransform()
{
    return _renderTrans;
}

void Element::SetRenderTransform(Transform* val)
{
    SetValue(RenderTransformProperty, val);
}

fPoint Element::GetRenderTransformOrigin()
{
    OfPoint* origin = RTTICast<OfPoint>(GetValue(RenderTransformOriginProperty));
    if (origin)
    {
        return origin->TofPoint();
    }
    else
    {
        return fPoint();
    }
}

void Element::SetRenderTransformOrigin(fPoint pt)
{
    SetValue(RenderTransformOriginProperty, new OfPoint(pt));
}

Transform* Element::GetVisualTransform()
{
    return _visualTans;
}

void Element::SetVisualTransform(Transform* val)
{
    if (NULL != val)
    {
        SETREFOBJ(_visualTans, val);
    }
}

void Element::OnVisualChildrenChanged(DpObject* visualAdded, DpObject* visualRemoved)
{

}

void Element::OnVisualParentChanged(DpObject* oldParent)
{

}

bool Element::MoveFocus(FocusNavDirection req)
{
    return false;
}

int Element::GetVisualChildrenCount()
{
    return 0;
}

int Element::GetVisibleChildrenCount()
{
    return GetVisualChildrenCount();
}

Element* Element::GetLogicalChild(int index)
{
    return NULL;
}

int Element::GetLogicalChildrenCount()
{
    return 0;
}

Element* Element::GetVisualChild(int index)
{
    return NULL;
}

Element* Element::GetVisibleChild(int index)
{
    Debug::Assert(index < GetVisualChildrenCount());
    return GetVisualChild(index);
}

void Element::SetBinding(DpProperty* prop, Binding* bind)
{
    if (NULL == bind)
    {
        SetValue(prop, NULL);
    }
    else
    {
        BindingExpression* bindExpr = NULL;
        
        bindExpr = bind->CreateBindingExpression();
        bindExpr->ref();
        SetValue(prop, bindExpr);
        bindExpr->unref();
    }
}

void Element::SetBinding(DpProperty* prop, String path)
{
    Binding* bind = new Binding();
    PropertyPath ppath;

    ppath.Path = path;
    bind->SetPath(ppath);
    SetBinding(prop, bind);
}

//-------------------------------------------------------------------
//

int Element::GetHitTestFilterAction()
{
    return GetValue(HitTestFilterActionProperty)->ToInt();
}

void Element::SetHitTestFilterAction(int val)
{
    SetValue(HitTestFilterActionProperty, new Integer(val));
}

void DpObject::WriteDoFlag(Uint32 key, bool add)
{
    bool tmp = ReadDoFlag(key);

    if (tmp != add && key > 1)
    {
        if (add)
        {
            _viewflag |= key;
        }
        else
        {
            _viewflag &= ~key;
        }
    }
}

int Element::HitTest(HitTestCtx* hitResult)
{
    return HitTestPoint(hitResult);
}

int Element::HitTestPoint(HitTestCtx* hitResult)
{
    int iHitRes = HitTestFilter();

    // Debug::Trace(_U("HitTestPoint:%d,%d\n"), hitResult->hitPoint.x, hitResult->hitPoint.y);

    if (iHitRes == HitTestFilterAction::ContinueSkipSelfAndChildren)
    {
        return HitTestFilterAction::Continue;
    }
    else if (iHitRes == HitTestFilterAction::Stop)
    {
        hitResult->hitRes = this;
        return HitTestFilterAction::Stop;
    }

    hitResult->EnterHitTest(this);

    bool bInElem = HitTestCore(hitResult, false);

    if (bInElem)
    {
        if (hitResult->hitFilter == HitTestFilterAction::Stop)
        {
            hitResult->ExitHitTest(this);
            return HitTestFilterAction::Stop;
        }

        // 
        // 测量子元素
        // 
        if (iHitRes != HitTestFilterAction::ContinueSkipChildren)
        {
            int iCount = GetVisibleChildrenCount();
            for (int i = iCount - 1; i >= 0; --i)
            {
                Element* pChild(GetVisibleChild(i));
                if (pChild && pChild->IsVisible())
                {
                    int iHitTest = pChild->HitTestPoint(hitResult);
                    if (iHitTest == HitTestFilterAction::Stop)
                    {
                        hitResult->ExitHitTest(this);
                        return iHitTest;
                    }
                }
            }
        }

        if (iHitRes != HitTestFilterAction::ContinueSkipSelf)
        {
            if (HitTestCore(hitResult, true))
            {
                hitResult->ExitHitTest(this);
                hitResult->hitRes = this;
                return HitTestFilterAction::Stop;
            }
        }
    }

    hitResult->ExitHitTest(this);
    return HitTestFilterAction::Continue;
}

}
