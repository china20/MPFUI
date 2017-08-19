// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Element.h
// ��  �ܣ�������Ŀ��Element���ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-05-02
// 
// ============================================================================

#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_

#include <System/Windows/Event.h>
#include <System/Windows/Effect.h>
#include <System/Windows/Assigner.h>
#include <System/Windows/DpObject.h>
#include <System/Windows/DpProperty.h>
#include <System/Windows/DragEventArg.h>
#include <System/Windows/ICommandSource.h>

#include <System/Tools/Array.h>
#include <System/Types/ValueBox.h>
#include <System/Animation/Transform.h>

#include <System/Graphics/Drawing.h>
#include <System/Graphics/Geometry.h>

#include <System/Windows/EventHandler.h>
#include <System/Windows/UncommonField.h>
#include <System/Windows/BindingExpression.h>
#include <System/Windows/EventHandlersStore.h>
#include <System/Windows/SystemParameters.h>

namespace suic
{

typedef shared<Element> ElementPtr;
typedef suic::StringDic<FnEventCall> FnEventDic;

struct MeasureData
{
    // �ڸ����ڵĿ��������С 
    Rect viewPort;
    // ���ڸ����Ŀ��ò�����С 
    Size availableSize;

    void SetAvailableSize(Size size)
    {
        availableSize = size;
    }

    void SetViewPort(Rect rect)
    {
        viewPort = rect;
    }

    Size GetAvailableSize() const
    {
        return availableSize;
    }

    Rect GetViewPort() const
    {
        return viewPort;
    }
};

struct LayoutTransformInfo
{
    Matrix transform;
    fPoint offset;
    Size transformedSize;
    Size untransformedSize;
};

struct HitTestCtx
{
    // ��궯�����
    ElementPtr hitResult;
    Point hitPoint;
    Array<Point> trans;

    HitTestCtx(Point pt);
    ~HitTestCtx();

    void EnterHitTest(Element* elem);
    void ExitHitTest(Element* elem);
};

class ElementInvoker : public AssignerInvoker
{
public:

    ElementInvoker(Element* sender, Object* data, int type);
    ~ElementInvoker();

    static void CallInvoke(Element* elem, Object* data, int type);

private:

    void Invoke();

private:

    int _type;
    Object* _data;
    WeakRef _target;
};

class SUICORE_API InvokeProxy : public DpObject
{
public:

    InvokerHandler Invoker;

    InvokeProxy();
    InvokeProxy(Element* target);
    ~InvokeProxy();

    Element* GetTarget() const;
    void SetTarget(Element* target);

    void PostInvoker(int what, Object* data);
    void SendInvoker(int what, Object* data, int timeout);

protected:

    void OnInvoker(InvokerArg* arg);

    friend class HandlerAssignerInvoker;

private:

    WeakRef _target;
};

class ItemEntry;

/// <summary>
/// ����Ԫ����Ļ��࣬ʵ��һ�����Ԫ�صĹ��������ͽӿ�.
/// </summary>
class SUICORE_API Element : public DpObject
{
public:

    Element();
    virtual ~Element();

    Element* Clone();

    void Dispose();

    RTTIOfAbstractClass(Element)

    // �¼�����
    static RoutedEvent* MouseDownEvent;
    static RoutedEvent* MouseUpEvent;

    static RoutedEvent* MouseEnterEvent;
    static RoutedEvent* MouseMoveEvent;
    static RoutedEvent* MouseLeaveEvent;
    static RoutedEvent* MouseLButtonDownEvent;
    static RoutedEvent* MouseLButtonUpEvent;
    static RoutedEvent* MouseMButtonDownEvent;
    static RoutedEvent* MouseMButtonUpEvent;
    static RoutedEvent* MouseRButtonDownEvent;
    static RoutedEvent* MouseRButtonUpEvent;
    static RoutedEvent* CursorSetEvent;
    static RoutedEvent* MouseWheelEvent;

    static RoutedEvent* ContextMenuEvent;

    static RoutedEvent* LostMouseCaptureEvent;
    static RoutedEvent* GotMouseCaptureEvent;
    
    static RoutedEvent* GotKeyboardFocusEvent;
    static RoutedEvent* LostKeyboardFocusEvent;

    static RoutedEvent* GotFocusEvent;
    static RoutedEvent* LostFocusEvent;

    static RoutedEvent* ExecutedEvent;
    static RoutedEvent* PreExecutedEvent;

    static RoutedEvent* CanExecuteEvent;
    static RoutedEvent* PreCanExecuteEvent;

    // �����¼�
    static RoutedEvent* TextInputEvent;
    static RoutedEvent* KeyDownEvent;
    static RoutedEvent* KeyUpEvent;

    static RoutedEvent* ThemeChangedEvent;

    static RoutedEvent* LoadedEvent;
    static RoutedEvent* UnloadedEvent;

    static RoutedEvent* TargetUpdatedEvent;
    static RoutedEvent* HitTestEvent;

    static RoutedEvent* InvalidateEvent;

    // Ԥ���¼�,ִ��Դ�¼�֮ǰ�����ȵ��ô��¼�
    // ���¼���Ӹ�Ԫ�ؿ�ʼִ��
    static RoutedEvent* PreMouseDownEvent;
    static RoutedEvent* PreMouseUpEvent;

    static RoutedEvent* PreCursorSetEvent;
    static RoutedEvent* PreMouseWheelEvent;
    static RoutedEvent* PreMouseMoveEvent;

    static RoutedEvent* PreMouseLButtonDownEvent;
    static RoutedEvent* PreMouseLButtonUpEvent;
    static RoutedEvent* PreMouseMButtonDownEvent;
    static RoutedEvent* PreMouseMButtonUpEvent;
    static RoutedEvent* PreMouseRButtonDownEvent;
    static RoutedEvent* PreMouseRButtonUpEvent;

    static RoutedEvent* PreGotKeyboardFocusEvent;
    static RoutedEvent* PreLostKeyboardFocusEvent;

    // ����Ԥ���¼�
    static RoutedEvent* PreTextInputEvent;
    static RoutedEvent* PreKeyDownEvent;
    static RoutedEvent* PreKeyUpEvent;

    static RoutedEvent* InvokerEvent;

    EventHandler LayoutUpdated;

public:

    static DpProperty* HitTestFilterActionProperty;

    static DpProperty* NameProperty;
    static DpProperty* OpacityProperty;
    static DpProperty* OpacityMaskProperty;
    static DpProperty* VisibilityProperty;
    static DpProperty* VisualEffectProperty;
    static DpProperty* AllowDropProperty;

    static DpProperty* CacheModeProperty;
    static DpProperty* BitmapEffectProperty;

    static DpProperty* ClipProperty;
    static DpProperty* ClipToBoundsProperty;

    static DpProperty* IsEnabledProperty;
    static DpProperty* IsVisibleProperty;
    static DpProperty* IsMouseOverProperty;
    static DpProperty* IsFocusedProperty;
    static DpProperty* IsKeyboardFocusedProperty;
    static DpProperty* IsKeyboardFocusWithinProperty;
    static DpProperty* IsHitTestVisibleProperty;

    static DpProperty* IsMouseCapturedProperty;
    static DpProperty* IsMouseCaptureWithinProperty;

    static DpProperty* IsMouseDirectlyOverProperty;
    static DpProperty* IsContextMenuProperty;

    static DpProperty* UseLayoutRoundingProperty;
    static DpProperty* FocusableProperty;

    static DpProperty* UserDataProperty;
    static DpProperty* DataContextProperty;

    static DpProperty* RenderTransformProperty;
    static DpProperty* RenderTransformOriginProperty;

    static DpProperty* CacheLayerProperty;

    static DpProperty* Tag0Property;
    static DpProperty* Tag1Property;
    static DpProperty* Tag2Property;
    static DpProperty* Tag3Property;
    
    static bool StaticInit();
    
    static void OnClipPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnClipToBoundsPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnVisualEffectPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnVisibilityPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIsVisiblePropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIsHitTestVisiblePropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIsEnabledPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnDataContextPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnRenderTransformPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnCacheLayerPropChanged(DpObject* d, DpPropChangedEventArg* e);

    static void OnGotKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e);
    static void OnLostKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e);

    static void OnPreviewGotKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e);
    static void OnPreviewLostKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e);

    static suic::FnEventDic* __stdcall ClassEventInfo();

public:

    void AddHandler(const RoutedEvent* routedEvent, Handler* handler);
    void AddHandler(const RoutedEvent* routedEvent, Handler* handler, bool handledToo);
    void RemoveHandler(const RoutedEvent* routedEvent, Handler* handler);

    void InvalidateBinding(bool invalidateChild);
    void InvalidateDepProp(DpProperty* dp, bool invalidateChild);

    //------------------------------------------------------------------

    void PostInvoker(int what, Object* data);
    void SendInvoker(int what, Object* data, int timeout);
    
    void AddInvoker(InvokerEventHandler* h);
    void RemoveInvoker(InvokerEventHandler* h);

    void OnRenderTransformNotify(Animatable* d, DpProperty* dp);
    void OnLayoutTransformNotify(Animatable* d, DpProperty* dp);

    void AddTextInput(KeyboardEventHandler* handler);
    void RemoveTextInput(KeyboardEventHandler* handler);

    void AddKeyDown(KeyboardEventHandler* handler);
    void RemoveKeyDown(KeyboardEventHandler* handler);

    void AddKeyUp(KeyboardEventHandler* handler);
    void RemoveKeyUp(KeyboardEventHandler* handler);

    void AddMouseDown(MouseButtonEventHandler* handler, bool handledToo);
    void RemoveMouseDown(MouseButtonEventHandler* handler);

    void AddMouseUp(MouseButtonEventHandler* handler, bool handledToo);
    void RemoveMouseUp(MouseButtonEventHandler* handler);

    void AddMouseMove(MouseButtonEventHandler* handler, bool handledToo);
    void RemoveMouseMove(MouseButtonEventHandler* handler);

    void AddMouseEnter(MouseButtonEventHandler* handler, bool handledToo);
    void RemoveMouseEnter(MouseButtonEventHandler* handler);

    void AddMouseLeave(MouseButtonEventHandler* handler, bool handledToo);
    void RemoveMouseLeave(MouseButtonEventHandler* handler);

    void AddMouseLeftButtonDown(MouseButtonEventHandler* handler, bool handledToo);
    void RemoveMouseLeftButtonDown(MouseButtonEventHandler* handler);

    void AddMouseLeftButtonUp(MouseButtonEventHandler* handler, bool handledToo);
    void RemoveMouseLeftButtonUp(MouseButtonEventHandler* handler);

    void AddMouseMiddleButtonDown(MouseButtonEventHandler* handler, bool handledToo);
    void RemoveMouseMiddleButtonDown(MouseButtonEventHandler* handler);

    void AddMouseMiddleButtonUp(MouseButtonEventHandler* handler, bool handledToo);
    void RemoveMouseMiddleButtonUp(MouseButtonEventHandler* handler);

    void AddMouseRightButtonDown(MouseButtonEventHandler* handler, bool handledToo);
    void RemoveMouseRightButtonDown(MouseButtonEventHandler* handler);

    void AddMouseRightButtonUp(MouseButtonEventHandler* handler, bool handledToo);
    void RemoveMouseRightButtonUp(MouseButtonEventHandler* handler);

    void AddMouseWheel(MouseButtonEventHandler* handler);
    void RemoveMouseWheel(MouseButtonEventHandler* handler);

    void AddContextMenu(RoutedEventHandler* handler);
    void RemoveContextMenu(RoutedEventHandler* handler);

    void AddGotKeyboardFocus(KeyboardFocusEventHandler* handler);
    void RemoveGotKeyboardFocus(KeyboardFocusEventHandler* handler);

    void AddLostKeyboardFocus(KeyboardFocusEventHandler* handler);
    void RemoveLostKeyboardFocus(KeyboardFocusEventHandler* handler);

    void AddCursorSet(RoutedEventHandler* handler);
    void RemoveCursorSet(RoutedEventHandler* handler);

    //------------------------------------------------------------------
    void AddPreTextInput(KeyboardEventHandler* handler);
    void RemovePreTextInput(KeyboardEventHandler* handler);

    void AddPreKeyDown(KeyboardEventHandler* handler);
    void RemovePreKeyDown(KeyboardEventHandler* handler);

    void AddPreKeyUp(KeyboardEventHandler* handler);
    void RemovePreKeyUp(KeyboardEventHandler* handler);

    void AddPreMouseDown(MouseButtonEventHandler* handler);
    void RemovePreMouseDown(MouseButtonEventHandler* handler);

    void AddPreMouseUp(MouseButtonEventHandler* handler);
    void RemovePreMouseUp(MouseButtonEventHandler* handler);

    void AddPreMouseMove(MouseButtonEventHandler* handler);
    void RemovePreMouseMove(MouseButtonEventHandler* handler);

    void AddPreMouseLeftButtonDown(MouseButtonEventHandler* handler);
    void RemovePreMouseLeftButtonDown(MouseButtonEventHandler* handler);

    void AddPreMouseLeftButtonUp(MouseButtonEventHandler* handler);
    void RemovePreMouseLeftButtonUp(MouseButtonEventHandler* handler);

    void AddPreMouseMiddleButtonDown(MouseButtonEventHandler* handler);
    void RemovePreMouseMiddleButtonDown(MouseButtonEventHandler* handler);

    void AddPreMouseMiddleButtonUp(MouseButtonEventHandler* handler);
    void RemovePreMouseMiddleButtonUp(MouseButtonEventHandler* handler);

    void AddPreMouseRightButtonDown(MouseButtonEventHandler* handler);
    void RemovePreMouseRightButtonDown(MouseButtonEventHandler* handler);

    void AddPreMouseRightButtonUp(MouseButtonEventHandler* handler);
    void RemovePreMouseRightButtonUp(MouseButtonEventHandler* handler);

    void AddPreMouseWheel(MouseButtonEventHandler* handler);
    void RemovePreMouseWheel(MouseButtonEventHandler* handler);

    void AddPreGotKeyboardFocus(KeyboardFocusEventHandler* handler);
    void RemovePreGotKeyboardFocus(KeyboardFocusEventHandler* handler);

    void AddPreLostKeyboardFocus(KeyboardFocusEventHandler* handler);
    void RemovePreLostKeyboardFocus(KeyboardFocusEventHandler* handler);

    void AddPreCursorSet(RoutedEventHandler* handler);
    void RemovePreCursorSet(RoutedEventHandler* handler);

public:

    void BeginAnimation(DpProperty* dp, AnimationTimeline* animation);

    /// <summary>
    ///  ��������Ԫ�صĳߴ�
    /// </summary>
    /// <remarks>
    ///  �˷�����ݹ���������Լ������ӽ���Ԫ�صĳߴ硣
    ///  Ĭ�Ͻ��������Ԫ�ص���Сֵ�����ֵ���ƣ����
    ///  ������Width��Height����ʹ������ֵ�������ֵ�������ԣ�
    ///  availableSize��Ϊ������Լ��ֵ������ֵ���ܴ������趨ֵ��
    /// </remarks>
    /// <param name="availableSize">
    ///  �ο���С
    /// </param>
    /// <returns>��</returns> 
    void Measure(const Size& availableSize);

    /// <summary>
    ///  �Խ���Ԫ�ؽ��в���
    /// </summary>
    /// <remarks>
    ///  �˷�����ݹ鲼�������Լ������ӽ���Ԫ�صĲ���
    ///  �ڶ��ӽ���Ԫ�ؽ��в���֮ǰ��������������е�
    ///  ���ӽ���Ԫ�ء�
    /// </remarks>
    /// <param name="offset">������Ԫ�ػ���ƫ��</param>
    /// <param name="finalRect">������Ԫ�ظ���������λ�ô�С</param>
    /// <returns>��</returns> 
    void Arrange(const Rect& arrangeRect);

    /// <summary>
    ///  ����������������Ϣ
    /// </summary>
    /// <remarks>
    ///  ӵ����ͬ���Ľ���Ԫ�ض�����һ��Context��������
    /// </remarks>
    /// <param name="key">���ݶ���Key</param>
    /// <param name="value">���ݶ���Value</param>
    /// <returns>��</returns> 
    void SetContext(const String& key, Object* value);

    /// <summary>
    ///  ȡ��������������Ϣ
    /// </summary>
    /// <param name="key">���ݶ���Key</param>
    /// <returns>���������ݶ���ֵ</returns> 
    Object* GetContext(const String& key);

    Effect* GetVisualEffect();
    void SetVisualEffect(Effect* val);

    /// <summary>
    ///  �������Ľ���״̬
    /// </summary>
    /// <remarks>
    ///  ���������Ԫ���ܹ�ӵ�н��㣬��ô���������OnGotFocus()��
    ///  ͬʱ����֮ǰ�Ľ���Ԫ�ص�OnLostFocus()��
    /// </remarks>
    /// <returns>�������Ԫ��ӵ�н��㷵��true������false</returns> 
    bool Focus();

    /// <summary>
    ///  �������Ԫ�أ��˷����̰߳�ȫ
    /// </summary>
    /// <remarks>
    ///  ����丸����Ԫ�ش��ڽ���״̬���˽���Ԫ��Ҳ
    ///  �ᱻ���á�
    /// </remarks>
    /// <returns>�����ϴν���Ԫ�ؼ���״̬</returns> 
    void Enable(bool val);

    Visibility GetVisibility() const;

    // �̰߳�ȫ����
    void SetVisibility(Visibility val);

    bool IsLogicalElement();
 
    /// <summary>
    ///  ����Ԫ���¼�
    /// </summary>
    /// <param name="e">Ԫ���¼�</param>
    /// <returns>��</returns> 
    void RaiseEvent(RoutedEventArg* e);

    /// <summary>
    ///  �жϵ�ǰ����Ԫ���Ƿ񱻳�ʼ����
    /// </summary>
    /// <remarks>
    ///  ֻ�б���ʼ���Ľ���Ԫ������ص���ʽ����Դ�Ż��ʼ����
    ///  ��ʱ����Ҳ���ᴥ��������׼��������Ҫ�����ݡ�
    /// </remarks>
    /// <returns>true: �������Ԫ���Ѿ���ʼ��</returns> 
    bool IsInitialized();
    bool IsLoaded();

    /// <summary>
    ///  ˢ�½���Ԫ�ػ��ơ�
    /// </summary>
    /// <remarks>
    ///  ���ڷֲ㴰�ڽ���ֱ�ӻ��ƣ�������ڻ����߳���
    ///  ���첽ִ�С�
    /// </remarks>
    /// <returns>��</returns>
    void InvalidateVisual();
    void InvalidateForce(); 

    /// <summary>
    ///  ˢ�½���Ԫ�ػ��ơ�
    /// </summary>
    /// <remarks>
    ///  �˷���ָ���ü�����
    /// </remarks>
    /// <param name="lprc">�ü�����</param>
    /// <param name="force">�Ƿ�ǿ��ˢ��</param>
    /// <returns>��</returns>
    void InvalidateRect(Rect* lprc, bool force);

    /// <summary>
    ///  ˢ�½���Ԫ�ػ��ơ�
    /// </summary>
    /// <remarks>
    ///  �˷�������ˢ���¼������ʵĸ�Ԫ�أ��ɸ�Ԫ�ؽ���ʵ��ˢ��
    ///  ���û�и�Ԫ�ش�����Ĭ�ϵ��ñ�Ԫ�ؽ���ˢ��,��ʱ��ͬ��
    ///  ������UpdateLayout��iActionָ����ˢ�µ���Ϊ��ȡֵΪ
    ///  InvTags::Measure;InvTags::Arrange;InvTags::Render�����Ϊ0����Ч
    /// </remarks>
    /// <param name="iAction">ˢ�¶�����־</param>
    /// <param name="force">�Ƿ�ǿ����Ⱦ</param>
    /// <returns>��</returns>
    void Invalidate(int iAction, bool force);
    void Invalidate(DpProperty* dp, int iAction, bool force);

    static void DoInvalidate(Element* sender, InvalidateEventArg& e);

    static Float GetDpiScaleX();
    static Float GetDpiScaleY();

    static Float RoundLayoutValue(Float val, Float dpiScale);


    /// <summary>
    ///  ���Բ������Ŀ���Ȩ��
    /// </summary>
    /// <returns>��</returns>
    void SetCaptureMouse(int iMode=0);

    /// <summary>
    ///  �����ͷ����Ŀ���Ȩ��
    /// </summary>
    /// <remarks>
    ///  �����ǰ����Ĳ��Ǳ�Ԫ�أ����ԡ�
    /// </remarks>
    /// <returns>��</returns>
    void ReleaseCaptureMouse();

    String ToString();

    void RenderOpen(RenderContext* renderCtx);

public:

    virtual void OnPreviewSetCursor(CursorEventArg* e);
    virtual void OnSetCursor(CursorEventArg* e);

    // ����¼�����
    virtual void OnMouseDown(MouseButtonEventArg* e);
    virtual void OnMouseUp(MouseButtonEventArg* e);

    virtual void OnMouseEnter(MouseButtonEventArg* e);
    virtual void OnMouseMove(MouseButtonEventArg* e);
    virtual void OnMouseLeave(MouseButtonEventArg* e);

    virtual void OnMouseWheel(MouseWheelEventArg* e);

    virtual void OnContextMenu(ContextMenuEventArg* e);

    virtual void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    virtual void OnMouseLeftButtonUp(MouseButtonEventArg* e);
    virtual void OnMouseMiddleButtonDown(MouseButtonEventArg* e);
    virtual void OnMouseMiddleButtonUp(MouseButtonEventArg* e);
    virtual void OnMouseRightButtonDown(MouseButtonEventArg* e);
    virtual void OnMouseRightButtonUp(MouseButtonEventArg* e);

    // ���Ԥ���¼�����
    virtual void OnPreviewMouseDown(MouseButtonEventArg* e);
    virtual void OnPreviewMouseUp(MouseButtonEventArg* e);

    virtual void OnPreviewMouseMove(MouseButtonEventArg* e);
    virtual void OnPreviewMouseWheel(MouseButtonEventArg* e);

    virtual void OnPreviewMouseLeftButtonDown(MouseButtonEventArg* e);
    virtual void OnPreviewMouseLeftButtonUp(MouseButtonEventArg* e);
    virtual void OnPreviewMouseMiddleButtonDown(MouseButtonEventArg* e);
    virtual void OnPreviewMouseMiddleButtonUp(MouseButtonEventArg* e);
    virtual void OnPreviewMouseRightButtonDown(MouseButtonEventArg* e);
    virtual void OnPreviewMouseRightButtonUp(MouseButtonEventArg* e);

    // �϶��¼�����
    virtual void OnDragEnter(DragEventArg* e);
    virtual void OnDragLeave(DragEventArg* e);
    virtual void OnDragOver(DragEventArg* e);
    virtual void OnDrop(DragEventArg* e);

    // �϶�Ԥ���¼�����
    virtual void OnPreviewDragEnter(DragEventArg* e);
    virtual void OnPreviewDragLeave(DragEventArg* e);
    virtual void OnPreviewDragOver(DragEventArg* e);
    virtual void OnPreviewDrop(DragEventArg* e);

    // �����¼�����
    virtual void OnTextInput(KeyboardEventArg* e);
    virtual void OnKeyDown(KeyboardEventArg* e);
    virtual void OnKeyUp(KeyboardEventArg* e);

    virtual void OnImeEvent(MessageEventArg* e);
    virtual void OnInputEvent(MessageEventArg* e);

    // Ԥ��������¼�����
    virtual void OnPreviewTextInput(KeyboardEventArg* e);
    virtual void OnPreviewKeyDown(KeyboardEventArg* e);
    virtual void OnPreviewKeyUp(KeyboardEventArg* e); 

    // ������׽�¼�����
    virtual void OnPreviewGotKeyboardFocus(KeyboardFocusEventArg* e);
    virtual void OnPreviewLostKeyboardFocus(KeyboardFocusEventArg* e);

    virtual void OnGotKeyboardFocus(KeyboardFocusEventArg* e);
    virtual void OnLostKeyboardFocus(KeyboardFocusEventArg* e);

    virtual void OnLostMouseCapture(MouseButtonEventArg* e);
    virtual void OnGotMouseCapture(MouseButtonEventArg* e);

    virtual void OnGotFocus(RoutedEventArg* e);
    virtual void OnLostFocus(RoutedEventArg* e);

    virtual void OnKeyboardFocusWithinChanged(bool withinFocus);
    virtual void OnMouseCaptureWithinChanged(bool withinFocus);

    // Ԫ����ӱ仯�¼�����
    virtual void OnChildrenChanged(Element* kidAdded, Element* kidRemoved);
    virtual void OnVisualParentChanged(const Element* oldParent, Element* newParent);

    virtual void OnRenderSizeChanged(SizeChangedInfo& sizeInfo);
    virtual void OnRenderSizeModeChanged(SizeModeChangedEventArg* e);

    virtual void BeginInit();
    virtual void EndInit();
    virtual void RecursiveInit();

    /// <summary>
    ///  ���ڼ����¼���
    /// </summary>
    /// <remarks>
    ///  �ڴ��ڴ���׼����ʾʱÿ��Ԫ�ض��ᱻ���á�
    /// </remarks>
    /// <param name="e">�����¼�</param>
    /// <returns>��</returns>
    virtual void OnLoaded(LoadedEventArg* e);
    virtual void OnUnloaded(LoadedEventArg* e);
    virtual void OnTargetUpdated(DataTransferEventArg* e);

    virtual void OnDataContextChanged(DpPropChangedEventArg* e);

    virtual void OnIsVisibleChanged(IsVisibleChangedEventArg* e);
    virtual void OnIsEnabledChanged(IsEnabledChangedEventArg* e);

    virtual void OnHitTest(HitResultEventArg* e);

    virtual void OnInvokerArrived(InvokerArg* e);

    virtual void OnAccessKey(AccessKeyEventArg* e);

    /// <summary>
    ///  ���½���Ԫ�صĲ��֡���ʾ���˷����̰߳�ȫ����
    /// </summary>
    /// <remarks>
    ///  �˷����ڲ�����InvalidateArrange()��InvalidateVisual()��
    /// </remarks>
    /// <returns>��</returns>
    void UpdateLayout();

    /// <summary>
    ///  ����ˢ��Ԫ�ز���
    /// </summary>
    /// <returns>��</returns>
    void UpdateArrange();
    void UpdateMeasure();

    /// <summary>
    ///  ˢ��Ԫ�ز���,�˺�����������ˢ�²��֣�ֻ�л���ʱ�Ż�ˢ��
    /// </summary>
    /// <returns>��</returns>
    void InvalidateArrange();

    /// <summary>
    ///  ˢ��Ԫ�ش�С��
    /// </summary>
    /// <returns>��</returns>    
    void InvalidateMeasure();

    bool ReadFlag(int key) const;
    void WriteFlag(int key, bool add);

    void SetOperationPending(bool val);
    bool IsOperationPending() const;

    void InputHitTest(Point pt, ElementPtr& enabledHit, ElementPtr& rawHit);

public:

    virtual Object* GetExtent(int iType);

    /// <summary>
    ///  �õ�����Ԫ�صĿ��Ӹ�Ԫ�ء�
    /// </summary>
    /// <returns>���Ӹ�Ԫ��</returns>
    Element* GetUIParent() const;
    DpObject* GetContextParent() const;

    LayoutTransformInfo* GetLayoutTransformInfo();

    Transform* GetRenderTransform();
    void SetRenderTransform(Transform* val);

    fPoint GetRenderTransformOrigin();
    void SetRenderTransformOrigin(fPoint pt);

    Transform* GetVisualTransform();
    void SetVisualTransform(Transform* val);

    //------------------------------------------------------

    virtual void OnVisualChildrenChanged(DpObject* visualAdded, DpObject* visualRemoved);
    virtual void OnVisualParentChanged(DpObject* oldParent);

    virtual bool MoveFocus(FocusNavDirection req);

    /// <summary>
    ///     ȡ�ÿ�����Ԫ�ظ�����
    /// </summary>
    /// <returns>������Ԫ�ظ���</returns>
    virtual int GetVisualChildrenCount();
    virtual int GetVisibleChildrenCount();

    /// <summary>
    ///  ȡ���߼���Ԫ�ء�
    /// </summary>
    /// <param name="index">Ԫ������</param>
    /// <returns>�߼���Ԫ��</returns>
    virtual Element* GetLogicalChild(int index);
    virtual int GetLogicalChildrenCount();

    /// <summary>
    ///  ȡ�ÿ�����Ԫ�ء�
    /// </summary>
    /// <remarks>
    ///  ��Ԫ����ÿ�ε���Arrangeʱ���ܻᱻ���¼��㡣
    /// </remarks>
    /// <param name="index">Ԫ������</param>
    /// <returns>������Ԫ��</returns>
    virtual Element* GetVisualChild(int index);
    virtual Element* GetVisibleChild(int index);

    /// <summary>
    ///  ָ����Ԫ�صĸ�Ԫ�أ��˷����ᴥ��OnParentChanged
    /// </summary>
    /// <param name="child">��Ԫ��</param>
    /// <returns>��</returns>
    void AddVisualChild(Element* child);

    /// <summary>
    ///  �����Ԫ�صĸ�Ԫ�غͱ�Ԫ����ͬ��
    ///  ���Ƴ���Ԫ�صĸ�Ԫ�أ��˷����ᴥ��OnParentChanged
    /// </summary>
    /// <param name="child">��Ԫ��</param>
    /// <returns>��</returns>
    void RemoveVisualChild(Element* child);

    //------------------------------------------------------

    void SetBinding(DpProperty* prop, Binding* bind);
    void SetBinding(DpProperty* prop, String path);

public:

    template<typename T>
    T* Cast()
    {
        return dynamic_cast<T*>(this);
    }

    template<typename T>
    T* FindElem(const suic::String& name)
    {
        return dynamic_cast<T*>(FindName(name));
    }

    /// <summary>
    ///  ����ָ���Ľ���Ԫ��
    /// </summary>
    /// <remarks>
    ///  ʹ��Name=""��������Բ��ұ����Լ����ӽڵ�Ľ���Ԫ�ء�
    /// </remarks>
    /// <param name="name">Ԫ�����ƣ���Name���塣</param>
    /// <returns>����Ԫ�ض���</returns> 
    virtual Element* FindName(const String& name);

    virtual bool BlockVisualState();
    virtual bool BlockRouteEvent();

    bool IsRenderable();

// ���Է��ʶ���
public:

    int GetPersistId();

    bool IsRightToLeft();

    bool IsHitTestVisible();
    void SetIsHitTestVisible(bool val);

    bool IsMouseDownVisible();
    void SetIsMouseDownVisible(bool val);

    /// <summary>
    ///  �жϵ�ǰ����Ԫ�صĿɼ��ԡ�
    /// </summary>
    /// <remarks>
    ///  �����Ԫ�ز��ɼ�����Ҳ���ɼ���
    /// </remarks>
    /// <returns>true: ����Ԫ�ؿɼ�</returns>
    bool IsVisible();

    /// <summary>
    ///  �жϵ�ǰ����Ԫ�صĽ����ԡ�
    /// </summary>
    /// <remarks>
    ///  �����Ԫ�ؽ��ã���Ҳ���á�
    /// </remarks>
    /// <returns>true: ����Ԫ�ؿ���</returns> 
    bool IsEnabled();

    /// <summary>
    ///  �жϽ���Ԫ���Ƿ�ӵ�н��㡣
    /// </summary>
    /// <returns>true: ӵ�н���</returns> 
    bool IsFocused();

    bool IsKeyboardFocused() const;
    bool IsKeyboardFocusWithin() const;

    void TransformPoint(fPoint& pt);
    /// <summary>
    ///  ����Ԫ���Լ���Ԫ�ص�ת������Ե����ת����
    /// </summary>
    /// <returns>���ش��ڸ�Ԫ��</returns> 
    Element* TransformAncestorPoint(fPoint& pt, Element* ancestor);
    Element* TransformAncestorRect(fRect& rect, Element* ancestor);

    /// <summary>
    ///  �ж�����ڽ���Ԫ���ϻ����ӽ���Ԫ���ϡ�
    /// </summary>
    /// <remarks>
    ///  �����Ԫ��ӵ����꣬��Ҳ����true��
    /// </remarks>
    /// <returns>true: ӵ�����</returns> 
    virtual bool IsMouseOver();

    /// <summary>
    ///  �ж�����ڽ���Ԫ���ϡ�
    /// </summary>
    /// <remarks>
    ///  ֻ�����ֱ����Ԫ���ϲŷ���true��
    /// </remarks>
    /// <returns>true: ӵ�����</returns> 
    bool IsMouseDirectlyOver();

    /// <summary>
    ///  �жϽ������Ԫ�ص�����Ƿ�ĳ������Ԫ�ؿ��ơ�
    /// </summary>
    /// <remarks>
    ///  ĳЩ����Ԫ�ػ���hot״̬ʱ������Ҫ�жϴ˱�־��
    /// </remarks>
    /// <returns>true: ӵ�����</returns> 
    bool IsMouseCaptured();

    /// <summary>
    ///  �жϽ������Ԫ�ص�����Ƿ�ĳ������Ԫ�ؿ��ơ�
    /// </summary>
    /// <remarks>
    ///  ���ӽ���Ԫ�ؿ���Ҳ����true��
    /// </remarks>
    /// <returns>true: ӵ�����</returns> 
    bool IsMouseCaptureWithin();

    /// <summary>
    ///  �жϵ�ǰ����Ԫ�ص������ԡ�
    /// </summary>
    /// <returns>true: ����Ԫ������</returns> 
    bool IsCollapsed();
    void SetIsCollapsed(bool val);

    /// <summary>
    ///  ���ý���Ԫ�ص��Զ������ݶ���
    /// </summary>
    /// <remarks>
    ///  �û������Զ����Լ������ݶ���Ȼ��ͨ���˷���
    ///  �󶨵�����Ԫ��
    /// </remarks>
    /// <param name="pData">�Զ������ݶ���</param>
    /// <returns>��</returns> 
    Object* GetUserTag(int index);
    void SetUserTag(int index, Object* data);

    WeakRef GetDataContext();
    void SetDataContext(Object* value);

    /// <summary>
    ///  �����Ƿ�ü��ӽ���Ԫ�ص�������Ԫ�ص�����
    /// </summary>
    /// <remarks>
    ///  ������Ҫ�ü������к��ӽ���Ԫ�ػ���ʱ����
    ///  �ü����������������
    /// </remarks>
    /// <param name="value">�ü���־</param>
    /// <returns>��</returns> 
    bool IsClipToBounds();
    void SetClipToBounds(bool val);

    virtual Geometry* GetClip();
    virtual void SetClip(Geometry* clip);

    /// <summary>
    ///  �жϽ���Ԫ���Ƿ����㡣
    /// </summary>
    /// <returns>true: ����</returns> 
    bool IsFocusable();
    void SetFocusable(bool val);

    bool IsAllowDrop();
    void SetAllowDrop(bool val);

    /// <summary>
    ///  ���ý���Ԫ�ر�ʶ
    /// </summary>
    /// <remarks>
    ///  ��ͬһ�������д˽���Ԫ�ر�ʶ����Ψһ��
    /// </remarks>
    /// <param name="name">����Ԫ�����Ʊ�ʶ</param>
    /// <returns>��</returns> 
    String GetName();
    void SetName(const String& val);

    Float GetOpacity();
    void SetOpacity(Float val);

    Size GetDesiredSize();
    void SetDesiredSize(Size val);

    //--------------------------------------- �����ת��

    /// <summary>
    ///  ����LayoutTransformת����Ĵ�С
    /// </summary>
    Size GetRealSize();

    /// <summary>
    ///  ���������ת��Ϊ����Ԫ�ص����ꡣ
    /// </summary>
    /// <param name="point">��������</param>
    /// <returns>����Ԫ������</returns>
    Point PointFromScreen(Point point);

    /// <summary>
    ///  ����Ԫ�������ת��Ϊ�������ꡣ
    /// </summary>
    /// <param name="point">��������</param>
    /// <returns>��������</returns>
    Point PointToScreen(Point point);
    Point TransformToAncestor(Element* ancestor);

    bool IsAncestorOf(const Element* child) const;
    bool IsDescendantOf(const Element* ancestor) const;

    EventHandlersStore* GetEventHandlersStore();

    MeasureData* GetMeasureData();
    void SetMeasureData(const MeasureData& measureData);

    void MeasureChild(Element* child, Size constraint);
    void SetMeasureDataOnChild(Element* child, Size constraint);

public:

    /// <summary>
    ///     ��ȡ����Ԫ�ص����д�С��
    /// </summary>
    /// <remarks>
    ///     ��������ܻ����Ԫ�ر������Ⱦ����
    /// </remarks>
    /// <returns>����Ԫ�ص����д�С</returns>
    Rect& GetDescendantBounds();

    /// <summary>
    ///     ��ȡ����Ԫ����Ը�Ԫ�ص�ƫ�ơ�
    /// </summary>
    /// <returns>��Ը�Ԫ�ص�ƫ��</returns>
    Point GetVisualOffset() const;
    void SetVisualOffset(const Point& pt);

    Rect GetViewClip();
    void SetViewClip(const Rect& clip);

    int GetHitTestFilterAction();
    void SetHitTestFilterAction(int val);

    int GetParentIndex() const;

    ItemEntry* GetItemEntry() const;
    void SetItemEntry(ItemEntry* item);

    Object* GetContainerItem() const;

    //--------------------------------------- ������

    bool HitTestMouse(Point pt);
    int HitTest(HitTestCtx* hitCtx);
    bool HitTest(HitTestCtx* hitCtx, Rect rect);

    virtual int HitTestFilter();
    
    virtual int HitTestPoint(HitTestCtx* hitCtx);
    virtual bool HitTestCore(HitTestCtx* hitCtx, bool onlySelf);

public:

    /// <summary>
    ///  Ԫ�صĳ�ʼ���¼���
    /// </summary>
    /// <remarks>
    ///  �˺�����EndInit���ã���ʱ���е���Դ������Ԫ�ض���ʼ����ϡ�
    /// </remarks>
    /// <returns>��</returns>
    virtual void OnInitialized(EventArg* e);
    virtual void PrivateInitialized();

    virtual void OnRender(Drawing* drawing);

    /// <summary>
    ///  ʵ�ֽ���Ԫ�������Ĵ�С�����߼���
    /// </summary>
    /// <remarks>
    ///  ������Ӧ�ü̳д˷�����������С�ļ��㣬
    ///  �˺���Ĭ�ϻ����Measure����DesiredSizeֵ��
    /// </remarks>
    /// <param name="constraintSize">�ο���С</param>
    /// <returns>����Ԫ�������Ĵ�С</returns>
    virtual Size OnMeasure(const Size& constraintSize);

    /// <summary>
    ///  ʵ�ֽ���Ԫ�ش�С��λ�õļ��㡣
    /// </summary>
    /// <remarks>
    ///  ������Ӧ�ü̳д˷����������λ�úʹ�С�ļ��㣬
    ///  ����ԪӦ��ʹ�ô���������Լ�������Ԫ�ء�
    /// </remarks>
    /// <param name="arrangeSize">��Ԫ���ṩ�Ŀ�������λ�ô�С</param>
    /// <returns>���ֺ�Ĵ�С</returns>
    virtual void OnArrange(const Size& arrangeSize);

    /// <summary>
    ///  ʵ�ֽ���Ԫ�ش�С�����߼���
    /// </summary>
    /// <param name="availableSize">�ο���С</param>
    /// <returns>����Ԫ�������Ĵ�С</returns>
    virtual Size MeasureCore(const Size& availableSize);    

    /// <summary>
    ///  ʵ�ֽ���Ԫ������С��λ�õļ��㡣
    /// </summary>
    /// <param name="offset">������Ԫ�ػ���ƫ��</param>
    /// <param name="finalRect">��Ԫ���ṩ�Ŀ�������λ�ô�С</param>
    /// <returns>��</returns>
    virtual void ArrangeCore(const Rect& arrangeRect);
    virtual Size ComputeRenderSize();

    virtual void Render(Drawing* drawing);
    virtual void RenderChildren(Drawing* drawing);

    virtual suic::FnEventDic* GetEventInfo();

    bool GetCacheLayer();
    void SetCacheLayer(bool val);

    RenderContext* GetRenderContext() const;

    /// <summary>
    ///  ��ȡ����Ԫ�ص���Ⱦ���򣬴˴�С������������ͻ��ơ�
    /// </summary>
    /// <remarks>
    ///  Arrange�Ժ󣬴�ֵ��ı䡣
    /// </remarks>
    /// <returns>��Ⱦ�����С</returns>
    Size GetRenderSize();
    void SetRenderSize(Size val);

    int GetActualWidth();
    int GetActualHeight();

    Rect GetRenderRect();

    void ResetCanvasOffset();
    Point GetCanvasOffset() const;

protected:

    bool IsInnerEnabled();
    void InnerEnableChildren(Element* elem, bool val);

    void InvalidateDataContextDeps(Object* data, bool invalidateChild);

    void SignalDesiredSizeChange();
    void EnsureVisible();
    void EnsureInvisible(bool collapsed);

    static void EnsureDpiScale();

    void SwitchVisibilityIfNeeded(Visibility val);

    Visibility GetVisibilityCache() const;
    void SetVisibilityCache(Visibility val);

    void UpdateIsVisibleCache();

    void InvalidateIsVisiblePropertyOnChildren(DpProperty* property);
    void InvalidateIsEnablePropertyOnChildren(DpProperty* property);

    virtual void UpdateInheritanceContext(const EffectiveValue& val);
    virtual void RemoveInheritanceContext(const DpProperty* dp);

protected:    

    Uint32 _flag;
    Uint32 _parentIndex;

    Point _offset;
    Point _canvasOffset;

    Size _renderSize;
    Size _measureSize;
    Size _desiredSize;

    Rect _finalArrange;
    Rect _descendantBounds;

    Geometry* _clip;
    WeakRef _dataCtx;

    MeasureData _measureData;
    LayoutTransformInfo* _ltData;

    Transform* _visualTans;
    Transform* _renderTrans;
    RenderContext* _renderLayer;
    EventHandlersStore* _evthStore;
    
private:

    int _persistId;
    Effect* _effect;
    WeakRef _parent;
    WeakRef _itemBase;

    static bool _setDpi;
    static Float _dpiScaleX;
    static Float _dpiScaleY;

    friend class ElementColl;
    friend class MemberVisitor;
    friend class VisualTreeOp;
    friend class ElementInvoker;
};

#include <System/Windows/Element.inl>

}

#endif
