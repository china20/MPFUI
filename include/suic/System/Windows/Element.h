// 华勤科技版权所有 2008-2022
// 
// 文件名：Element.h
// 功  能：定义核心库的Element基类。
// 
// 作  者：MPF开发组
// 时  间：2010-05-02
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
    // 在父窗口的可视区域大小 
    Rect viewPort;
    // 窗口给出的可用测量大小 
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
    Byte hitFilter;
    Point hitPoint;
    // 
    // 鼠标动作结果
    //
    ElementPtr hitRes;
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
/// 界面元素类的基类，实现一般界面元素的公共操作和接口.
/// </summary>
class SUICORE_API Element : public DpObject
{
public:

    Element();
    virtual ~Element();

    Element* Clone();

    void Dispose();

    RTTIOfAbstractClass(Element)

    // 事件定义
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

    // 键盘事件
    static RoutedEvent* TextInputEvent;
    static RoutedEvent* KeyDownEvent;
    static RoutedEvent* KeyUpEvent;

    static RoutedEvent* ThemeChangedEvent;

    static RoutedEvent* LoadedEvent;
    static RoutedEvent* UnloadedEvent;

    static RoutedEvent* TargetUpdatedEvent;
    static RoutedEvent* HitTestEvent;

    static RoutedEvent* InvalidateEvent;

    // 预览事件,执行源事件之前会首先调用此事件
    // 此事件会从根元素开始执行
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

    // 键盘预览事件
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
    ///  测量界面元素的尺寸
    /// </summary>
    /// <remarks>
    ///  此方法会递归测量自身以及所有子界面元素的尺寸。
    ///  默认将会检测界面元素的最小值和最大值限制，如果
    ///  设置了Width和Height，则使用设置值，计算的值将被忽略，
    ///  availableSize作为测量的约束值，测量值不能大于其设定值。
    /// </remarks>
    /// <param name="availableSize">
    ///  参考大小
    /// </param>
    /// <returns>无</returns> 
    void Measure(const Size& availableSize);

    /// <summary>
    ///  对界面元素进行布局
    /// </summary>
    /// <remarks>
    ///  此方法会递归布局自身以及所有子界面元素的布局
    ///  在对子界面元素进行布局之前，将会清除其所有的
    ///  可视界面元素。
    /// </remarks>
    /// <param name="offset">父界面元素画布偏移</param>
    /// <param name="finalRect">父界面元素给出的最终位置大小</param>
    /// <returns>无</returns> 
    void Arrange(const Rect& arrangeRect);

    /// <summary>
    ///  设置上下文数据信息
    /// </summary>
    /// <remarks>
    ///  拥有相同跟的界面元素都共享一个Context容器对象。
    /// </remarks>
    /// <param name="key">数据对象Key</param>
    /// <param name="value">数据对象Value</param>
    /// <returns>无</returns> 
    void SetContext(const String& key, Object* value);

    /// <summary>
    ///  取得上下文数据信息
    /// </summary>
    /// <param name="key">数据对象Key</param>
    /// <returns>上下文数据对象值</returns> 
    Object* GetContext(const String& key);

    Effect* GetVisualEffect();
    void SetVisualEffect(Effect* val);

    /// <summary>
    ///  设置鼠标的焦点状态
    /// </summary>
    /// <remarks>
    ///  如果本界面元素能够拥有焦点，那么将会调用其OnGotFocus()，
    ///  同时调用之前的焦点元素的OnLostFocus()。
    /// </remarks>
    /// <returns>如果界面元素拥有焦点返回true；否则false</returns> 
    bool Focus();

    /// <summary>
    ///  激活界面元素，此方法线程安全
    /// </summary>
    /// <remarks>
    ///  如果其父界面元素处于禁用状态，此界面元素也
    ///  会被禁用。
    /// </remarks>
    /// <returns>返回上次界面元素激活状态</returns> 
    void Enable(bool val);

    Visibility GetVisibility() const;

    // 线程安全调用
    void SetVisibility(Visibility val);

    bool IsLogicalElement();
 
    /// <summary>
    ///  触发元素事件
    /// </summary>
    /// <param name="e">元素事件</param>
    /// <returns>无</returns> 
    void RaiseEvent(RoutedEventArg* e);

    /// <summary>
    ///  判断当前界面元素是否被初始化。
    /// </summary>
    /// <remarks>
    ///  只有被初始化的界面元素其相关的样式、资源才会初始化。
    ///  此时绘制也不会触发，仅仅准备好了需要的数据。
    /// </remarks>
    /// <returns>true: 如果界面元素已经初始化</returns> 
    bool IsInitialized();
    bool IsLoaded();

    /// <summary>
    ///  刷新界面元素绘制。
    /// </summary>
    /// <remarks>
    ///  对于分层窗口将会直接绘制，如果不在绘制线程中
    ///  将异步执行。
    /// </remarks>
    /// <returns>无</returns>
    void InvalidateVisual();
    void InvalidateForce(); 

    /// <summary>
    ///  刷新界面元素绘制。
    /// </summary>
    /// <remarks>
    ///  此方法指定裁剪区域。
    /// </remarks>
    /// <param name="lprc">裁剪区域</param>
    /// <param name="force">是否强制刷新</param>
    /// <returns>无</returns>
    void InvalidateRect(Rect* lprc, bool force);

    /// <summary>
    ///  刷新界面元素绘制。
    /// </summary>
    /// <remarks>
    ///  此方法发送刷新事件给合适的父元素，由父元素进行实际刷新
    ///  如果没有父元素处理，则默认调用本元素进行刷新,此时等同于
    ///  调用了UpdateLayout，iAction指定了刷新的行为，取值为
    ///  InvTags::Measure;InvTags::Arrange;InvTags::Render，如果为0则都有效
    /// </remarks>
    /// <param name="iAction">刷新动作标志</param>
    /// <param name="force">是否强制渲染</param>
    /// <returns>无</returns>
    void Invalidate(int iAction, bool force);
    void Invalidate(DpProperty* dp, int iAction, bool force);

    static void DoInvalidate(Element* sender, InvalidateEventArg& e);

    static Float GetDpiScaleX();
    static Float GetDpiScaleY();

    static Float RoundLayoutValue(Float val, Float dpiScale);


    /// <summary>
    ///  尝试捕获鼠标的控制权。
    /// </summary>
    /// <returns>无</returns>
    void SetCaptureMouse(int iMode=0);

    /// <summary>
    ///  尝试释放鼠标的控制权。
    /// </summary>
    /// <remarks>
    ///  如果当前捕获的不是本元素，忽略。
    /// </remarks>
    /// <returns>无</returns>
    void ReleaseCaptureMouse();

    String ToString();

    void RenderOpen(RenderContext* renderCtx);

public:

    virtual void OnPreviewSetCursor(CursorEventArg* e);
    virtual void OnSetCursor(CursorEventArg* e);

    // 鼠标事件定义
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

    // 鼠标预览事件定义
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

    // 拖动事件定义
    virtual void OnDragEnter(DragEventArg* e);
    virtual void OnDragLeave(DragEventArg* e);
    virtual void OnDragOver(DragEventArg* e);
    virtual void OnDrop(DragEventArg* e);

    // 拖动预览事件定义
    virtual void OnPreviewDragEnter(DragEventArg* e);
    virtual void OnPreviewDragLeave(DragEventArg* e);
    virtual void OnPreviewDragOver(DragEventArg* e);
    virtual void OnPreviewDrop(DragEventArg* e);

    // 键盘事件定义
    virtual void OnTextInput(KeyboardEventArg* e);
    virtual void OnKeyDown(KeyboardEventArg* e);
    virtual void OnKeyUp(KeyboardEventArg* e);

    virtual void OnImeEvent(MessageEventArg* e);
    virtual void OnInputEvent(MessageEventArg* e);

    // 预定义键盘事件定义
    virtual void OnPreviewTextInput(KeyboardEventArg* e);
    virtual void OnPreviewKeyDown(KeyboardEventArg* e);
    virtual void OnPreviewKeyUp(KeyboardEventArg* e); 

    // 焦点扑捉事件定义
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

    // 元素添加变化事件定义
    virtual void OnChildrenChanged(Element* kidAdded, Element* kidRemoved);
    virtual void OnVisualParentChanged(const Element* oldParent, Element* newParent);

    virtual void OnRenderSizeChanged(SizeChangedInfo& sizeInfo);
    virtual void OnRenderSizeModeChanged(SizeModeChangedEventArg* e);

    virtual void BeginInit();
    virtual void EndInit();
    virtual void RecursiveInit();

    /// <summary>
    ///  窗口加载事件。
    /// </summary>
    /// <remarks>
    ///  在窗口创建准备显示时每个元素都会被调用。
    /// </remarks>
    /// <param name="e">加载事件</param>
    /// <returns>无</returns>
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
    ///  更新界面元素的布局、显示，此方法线程安全调用
    /// </summary>
    /// <remarks>
    ///  此方法内部调用InvalidateArrange()、InvalidateVisual()。
    /// </remarks>
    /// <returns>无</returns>
    void UpdateLayout();

    /// <summary>
    ///  立即刷新元素布局
    /// </summary>
    /// <returns>无</returns>
    void UpdateArrange();
    void UpdateMeasure();

    Element* FindFixedAncestor();

    void UpdateAncestorMeasure();
    void UpdateAncestorArrange();

    /// <summary>
    ///  刷新元素布局,此函数不会立即刷新布局，只有绘制时才会刷新
    /// </summary>
    /// <returns>无</returns>
    void InvalidateArrange();

    /// <summary>
    ///  刷新元素大小。
    /// </summary>
    /// <returns>无</returns>    
    void InvalidateMeasure();

    bool ReadFlag(int key) const;
    void WriteFlag(int key, bool add);

    void SetOperationPending(bool val);
    bool IsOperationPending() const;

    void InputHitTest(Point pt, ElementPtr& enabledHit, ElementPtr& rawHit);

public:

    virtual Object* GetExtent(int iType);

    /// <summary>
    ///  得到界面元素的可视父元素。
    /// </summary>
    /// <returns>可视父元素</returns>
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
    ///     取得可视子元素个数。
    /// </summary>
    /// <returns>可视子元素个数</returns>
    virtual int GetVisualChildrenCount();
    virtual int GetVisibleChildrenCount();

    /// <summary>
    ///  取得逻辑子元素。
    /// </summary>
    /// <param name="index">元素索引</param>
    /// <returns>逻辑子元素</returns>
    virtual Element* GetLogicalChild(int index);
    virtual int GetLogicalChildrenCount();

    /// <summary>
    ///  取得可视子元素。
    /// </summary>
    /// <remarks>
    ///  子元素在每次调用Arrange时可能会被重新计算。
    /// </remarks>
    /// <param name="index">元素索引</param>
    /// <returns>可视子元素</returns>
    virtual Element* GetVisualChild(int index);
    virtual Element* GetVisibleChild(int index);

    /// <summary>
    ///  指定子元素的父元素，此方法会触发OnParentChanged
    /// </summary>
    /// <param name="child">子元素</param>
    /// <returns>无</returns>
    void AddVisualChild(Element* child);

    /// <summary>
    ///  如果子元素的父元素和本元素相同，
    ///  则移除子元素的父元素，此方法会触发OnParentChanged
    /// </summary>
    /// <param name="child">子元素</param>
    /// <returns>无</returns>
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
    ///  查找指定的界面元素
    /// </summary>
    /// <remarks>
    ///  使用Name=""定义的属性查找本身以及其子节点的界面元素。
    /// </remarks>
    /// <param name="name">元素名称，由Name定义。</param>
    /// <returns>界面元素对象</returns> 
    virtual Element* FindName(const String& name);

    virtual bool BlockVisualState();
    virtual bool BlockRouteEvent();

    bool IsRenderable();

// 属性访问定义
public:

    int GetPersistId();

    bool IsRightToLeft();

    bool IsHitTestVisible();
    void SetIsHitTestVisible(bool val);

    bool IsMouseDownVisible();
    void SetIsMouseDownVisible(bool val);

    /// <summary>
    ///  判断当前界面元素的可见性。
    /// </summary>
    /// <remarks>
    ///  如果父元素不可见，其也不可见。
    /// </remarks>
    /// <returns>true: 界面元素可见</returns>
    bool IsVisible();

    /// <summary>
    ///  判断当前界面元素的禁用性。
    /// </summary>
    /// <remarks>
    ///  如果父元素禁用，其也禁用。
    /// </remarks>
    /// <returns>true: 界面元素可用</returns> 
    bool IsEnabled();

    /// <summary>
    ///  判断界面元素是否拥有焦点。
    /// </summary>
    /// <returns>true: 拥有焦点</returns> 
    bool IsFocused();

    bool IsKeyboardFocused() const;
    bool IsKeyboardFocusWithin() const;

    void TransformPoint(fPoint& pt);
    /// <summary>
    ///  根据元素以及父元素的转换矩阵对点进行转换。
    /// </summary>
    /// <returns>返回窗口跟元素</returns> 
    Element* TransformAncestorPoint(fPoint& pt, Element* ancestor);
    Element* TransformAncestorRect(fRect& rect, Element* ancestor);

    /// <summary>
    ///  判断鼠标在界面元素上或其子界面元素上。
    /// </summary>
    /// <remarks>
    ///  如果子元素拥有鼠标，其也返回true。
    /// </remarks>
    /// <returns>true: 拥有鼠标</returns> 
    virtual bool IsMouseOver();

    /// <summary>
    ///  判断鼠标在界面元素上。
    /// </summary>
    /// <remarks>
    ///  只有鼠标直接在元素上才返回true。
    /// </remarks>
    /// <returns>true: 拥有鼠标</returns> 
    bool IsMouseDirectlyOver();

    /// <summary>
    ///  判断进入界面元素的鼠标是否被某个界面元素控制。
    /// </summary>
    /// <remarks>
    ///  某些界面元素绘制hot状态时可能需要判断此标志。
    /// </remarks>
    /// <returns>true: 拥有鼠标</returns> 
    bool IsMouseCaptured();

    /// <summary>
    ///  判断进入界面元素的鼠标是否被某个界面元素控制。
    /// </summary>
    /// <remarks>
    ///  其子界面元素控制也返回true。
    /// </remarks>
    /// <returns>true: 拥有鼠标</returns> 
    bool IsMouseCaptureWithin();

    /// <summary>
    ///  判断当前界面元素的收缩性。
    /// </summary>
    /// <returns>true: 界面元素收缩</returns> 
    bool IsCollapsed();
    void SetIsCollapsed(bool val);

    /// <summary>
    ///  设置界面元素的自定义数据对象
    /// </summary>
    /// <remarks>
    ///  用户可以自定义自己的数据对象，然后通过此方法
    ///  绑定到界面元素
    /// </remarks>
    /// <param name="pData">自定义数据对象</param>
    /// <returns>无</returns> 
    Object* GetUserTag(int index);
    void SetUserTag(int index, Object* data);

    WeakRef GetDataContext();
    void SetDataContext(Object* value);

    /// <summary>
    ///  设置是否裁剪子界面元素到本界面元素的区域
    /// </summary>
    /// <remarks>
    ///  设置需要裁剪后，所有孩子界面元素绘制时都会
    ///  裁剪到本身的限制区域
    /// </remarks>
    /// <param name="value">裁剪标志</param>
    /// <returns>无</returns> 
    bool IsClipToBounds();
    void SetClipToBounds(bool val);

    virtual Geometry* GetClip();
    virtual void SetClip(Geometry* clip);

    /// <summary>
    ///  判断界面元素是否处理焦点。
    /// </summary>
    /// <returns>true: 处理</returns> 
    bool IsFocusable();
    void SetFocusable(bool val);

    bool IsAllowDrop();
    void SetAllowDrop(bool val);

    /// <summary>
    ///  设置界面元素标识
    /// </summary>
    /// <remarks>
    ///  在同一个窗口中此界面元素标识建议唯一。
    /// </remarks>
    /// <param name="name">界面元素名称标识</param>
    /// <returns>无</returns> 
    String GetName();
    void SetName(const String& val);

    Float GetOpacity();
    void SetOpacity(Float val);

    Size GetDesiredSize();
    void SetDesiredSize(Size val);

    //--------------------------------------- 坐标点转换

    /// <summary>
    ///  经过LayoutTransform转换后的大小
    /// </summary>
    Size GetRealSize();

    /// <summary>
    ///  窗口坐标点转换为界面元素的坐标。
    /// </summary>
    /// <param name="point">窗口坐标</param>
    /// <returns>界面元素坐标</returns>
    Point PointFromScreen(Point point);

    /// <summary>
    ///  界面元素坐标点转换为窗口坐标。
    /// </summary>
    /// <param name="point">界面坐标</param>
    /// <returns>窗口坐标</returns>
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
    ///     获取子孙元素的所有大小。
    /// </summary>
    /// <remarks>
    ///     此区域可能会大于元素本身的渲染区域。
    /// </remarks>
    /// <returns>子孙元素的所有大小</returns>
    Rect& GetDescendantBounds();

    /// <summary>
    ///     获取界面元素相对父元素的偏移。
    /// </summary>
    /// <returns>相对父元素的偏移</returns>
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

    //--------------------------------------- 鼠标测试

    bool HitTestMouse(Point pt);
    int HitTest(HitTestCtx* hitCtx);
    bool HitTest(HitTestCtx* hitCtx, Rect rect);

    virtual int HitTestFilter();
    
    virtual int HitTestPoint(HitTestCtx* hitCtx);
    virtual bool HitTestCore(HitTestCtx* hitCtx, bool onlySelf);

public:

    /// <summary>
    ///  元素的初始化事件。
    /// </summary>
    /// <remarks>
    ///  此函数由EndInit调用，此时所有的资源包括子元素都初始化完毕。
    /// </remarks>
    /// <returns>无</returns>
    virtual void OnInitialized(EventArg* e);
    virtual void PrivateInitialized();

    virtual void OnRender(Drawing* drawing);

    /// <summary>
    ///  实现界面元素真正的大小计算逻辑。
    /// </summary>
    /// <remarks>
    ///  派生类应该继承此方法完成自身大小的计算，
    ///  此函数默认会调用Measure更新DesiredSize值。
    /// </remarks>
    /// <param name="constraintSize">参考大小</param>
    /// <returns>界面元素期望的大小</returns>
    virtual Size OnMeasure(const Size& constraintSize);

    /// <summary>
    ///  实现界面元素大小和位置的计算。
    /// </summary>
    /// <remarks>
    ///  派生类应该继承此方法完成自身位置和大小的计算，
    ///  界面元应该使用此区域更新自己或其子元素。
    /// </remarks>
    /// <param name="arrangeSize">父元素提供的可用区域位置大小</param>
    /// <returns>布局后的大小</returns>
    virtual void OnArrange(const Size& arrangeSize);

    /// <summary>
    ///  实现界面元素大小计算逻辑。
    /// </summary>
    /// <param name="availableSize">参考大小</param>
    /// <returns>界面元素期望的大小</returns>
    virtual Size MeasureCore(const Size& availableSize);    

    /// <summary>
    ///  实现界面元素最后大小和位置的计算。
    /// </summary>
    /// <param name="offset">父界面元素画布偏移</param>
    /// <param name="finalRect">父元素提供的可用区域位置大小</param>
    /// <returns>无</returns>
    virtual void ArrangeCore(const Rect& arrangeRect);
    virtual Size ComputeRenderSize();

    virtual void Render(Drawing* drawing);
    virtual void RenderChildren(Drawing* drawing);

    virtual suic::FnEventDic* GetEventInfo();

    bool GetCacheLayer();
    void SetCacheLayer(bool val);

    RenderContext* GetRenderContext() const;

    /// <summary>
    ///  获取界面元素的渲染区域，此大小将用于鼠标侦测和绘制。
    /// </summary>
    /// <remarks>
    ///  Arrange以后，此值会改变。
    /// </remarks>
    /// <returns>渲染区域大小</returns>
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
