// 华勤科技版权所有 2008-2022
// 
// 文件名：FrameworkElement.h
// 功  能：定义核心库的FrameworkElement基类。
// 
// 作  者：MPF开发组
// 时  间：2010-05-02
// 
// ============================================================================

#ifndef _UIFRAMEWORKELEMENT_H_
#define _UIFRAMEWORKELEMENT_H_

#include <System/Windows/Element.h>
#include <System/Windows/IScrollInfo.h>
#include <System/Windows/RoutedCommand.h>
#include <System/Windows/ICommandSource.h>
#include <System/Windows/RoutedEventArg.h>
#include <System/Windows/AssignerTimer.h>

#include <System/Graphics/Brush.h>
#include <System/Graphics/Cursor.h>

#include <System/Types/Interface.h>
#include <System/Tools/CollectionX.h>

#include <System/Resources/Style.h>
#include <System/Resources/ResourceDictionary.h>

namespace suic
{

class FEFactory;
class ContextMenu;
class FrameworkElement;
class FrameworkTemplate;

typedef shared<FrameworkElement> FrameworkElementPtr;

class StoryBoard;

struct TreeChangeInfo
{
    Array<DpObject*> dpStack;
    DpObject* rootOfChange;
};

struct ResourcesChangeInfo
{
    String key;
    DpObject* container;

    Array<ResourceDictionary*> oldDictionaries;
    Array<ResourceDictionary*> newDictionaries;
};

class FrameworkInvoker : public AssignerInvoker
{
public:

    FrameworkInvoker(FrameworkElement* sender, Object* data, int type);
    ~FrameworkInvoker();

    static void CallInvoke(FrameworkElement* elem, Object* data, int type);

private:

    void Invoke();

private:

    int _type;
    Object* _data;
    WeakRef _target;
};

struct SUICORE_API MinMaxInfo
{ 
    MinMaxInfo();
    void UpdateMinMax(FrameworkElement* fe, int w, int h);

    int minWidth;
    int maxWidth; 
    int minHeight;
    int maxHeight; 
};

/// <summary>
///  绘制于界面上的可视元素基类，实现框架可视元素公共操作和接口.
///  从类派生可以拥有窗口资源（比如定时器）、样式等。
/// </summary>
class SUICORE_API FrameworkElement : public Element
{
public:

    FrameworkElement();
    virtual ~FrameworkElement();

    FrameworkElement* Clone();

    RTTIOfAbstractClass(FrameworkElement)

    virtual FrameworkElement* GetTemplateChild();
    virtual void SetTemplateChild(FrameworkElement* elem);

    // 命令支持
    virtual ICommandSource* GetCommandSource();
    // 滚动支持
    virtual IScrollInfo* GetScrollInfo();    
    virtual Rect MakeVisible(Element* visual, Rect rect);

    Element* GetTemplateChild(const String& childName);
    DpObject* GetTemplatedParent();

    /// <summary>
    ///  递归初始化元素
    /// </summary>
    /// <returns>无</returns> 
    void RecursiveInit();

    /// <summary>
    /// 查找界面元素的资源
    /// </summary>
    /// <remarks>
    /// 资源是可以继承的，系统将从本元素搜索指定资源，
    /// 然后向上搜索，直到找到一个资源，
    /// 最后将会搜索系统资源（Application定义）。
    /// </remarks>
    /// <param name="name">资源名称</param>
    /// <returns>资源对象</returns> 
    Object* FindRes(const String& name);
    Object* FindRes(FrameworkElement* parent, const String& name);

    /// <summary>
    /// 设置界面元素本身的资源对象
    /// </summary>
    /// <param name="resources">资源对象</param>
    /// <returns>无</returns> 
    void SetResources(ResourceDictionary* resources);

    /// <summary>
    /// 获得界面元素本身的资源对象
    /// </summary>
    /// <returns>资源对象</returns> 
    ResourceDictionary* GetResources();

    void ShowToolTip(Point pt);

    bool ReadInnerFlag(Uint32 flag) const;
    void WriteInnerFlag(Uint32 flag, bool val);

    bool GetIsStyleSetFromGenerator() const;
    void SetIsStyleSetFromGenerator(bool val);

    /// <summary>
    ///  使元素进入可见范围
    /// </summary>
    /// <remarks>
    ///  如果元素不在可见范围，此方法使的元素进入可见范围进行显示。
    /// </remarks>
    /// <returns>无</returns>
    void BringIntoView();
    void BringIntoView(Rect targetRect);

    void AddRequestBringIntoView(RoutedEventHandler* handler);
    void RemoveRequestBringIntoView(RoutedEventHandler& handler);

public:

    static RoutedEvent* FindToolTipEvent;
    static RoutedEvent* SizeChangedEvent;
    static RoutedEvent* ScrollPosChangedEvent;
    static RoutedEvent* ContextMenuClosingEvent;
    static RoutedEvent* ContextMenuOpeningEvent;
    static RoutedEvent* RequestBringIntoViewEvent;

    static RoutedEvent* ToolTipOpeningEvent;
    static RoutedEvent* ToolTipClosingEvent;
 
    static DpProperty* WidthProperty;
    static DpProperty* HeightProperty;
    static DpProperty* MinWidthProperty;
    static DpProperty* MinHeightProperty;
    static DpProperty* MaxWidthProperty;
    static DpProperty* MaxHeightProperty;

    // 只读属性
    static DpProperty* ActualWidthProperty;
    static DpProperty* ActualHeightProperty;
    
    static DpProperty* FlowDirectionProperty;
    static DpProperty* FocusVisualStyleProperty;
    static DpProperty* VerticalAlignmentProperty;
    static DpProperty* HorizontalAlignmentProperty;

    static DpProperty* MarginProperty;

    static DpProperty* CursorProperty;
    static DpProperty* ToolTipProperty;

    static DpProperty* StyleProperty;
    static DpProperty* TagProperty;
    static DpProperty* LanguageProperty;
    static DpProperty* LayoutTransformProperty;
    static DpProperty* ContextMenuProperty;

    static Style* DefaultFocusVisualStyle;
    static UncommonField<OSize> UnclippedDesiredSizeField;

    static void StaticInit();

    static void OnWidthPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnHeightPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMinWidthPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMinHeightPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMaxWidthPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMaxHeightPropChanged(DpObject* d, DpPropChangedEventArg* e);

    static void OnFlowDirectionPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnHorizontalAlignmentPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnVerticalAlignmentPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMarginPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnLayoutTransformPropChanged(DpObject* d, DpPropChangedEventArg* e);

    static void OnGotKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e);
    static void OnPreviewGotKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e);
    static void LostKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e);

    static void OnToolTipOpeningThunk(Element* sender, ToolTipEventArg* e);
    static void OnToolTipClosingThunk(Element* sender, ToolTipEventArg* e);
    static void OnRequestBringIntoViewThunk(DpObject* sender, RoutedEventArg* e);

    static void OnFindToolTip(Object* target, RoutedEventArg* e);

public:

    void AddSizeChanged(RoutedEventHandler* h);
    void RemoveSizeChanged(RoutedEventHandler* h);

    bool IsNeedsClipBounds();
    void SetIsNeedsClipBounds(bool val);

    ContextMenu* GetContextMenu();
    void SetContextMenu(ContextMenu* val);

    Transform* GetLayoutTransform();
    void SetLayoutTransform(Transform* val);

    /// <summary>
    /// 设置界面元素的样式对象
    /// </summary>
    /// <remarks>
    /// 设置界面样式将会影响元素的呈现，如果bStyle为true，
    /// 则将会使用样式初始化元素的宽度、高度等固有属性。
    /// 而且会触发OnPropertyChanged()事件调用。
    /// </remarks>
    /// <param name="thePtr">
    ///     样式对象
    /// </param>
    /// <returns>无</returns> 
    Style* GetStyle();
    void SetStyle(Style* val);

    OCursor* GetCursor();
    void SetCursor(OCursor* val);

    FlowDirection GetFlowDirection();
    void SetFlowDirection(FlowDirection val);

    Style* GetFocusVisualStyle();
    void SetFocusVisualStyle(Style* style);

    bool MoveFocus(FocusNavDirection req);

    /// <summary>
    ///  得到界面元素的逻辑父元素。
    /// </summary>
    /// <returns>逻辑父元素</returns>
    FrameworkElement* GetParent() const;

    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

    /// <summary>
    /// 设置元素提示信息
    /// </summary>
    /// <remarks>
    /// 鼠标进入元素时触发提示，离开时关闭
    /// 提示信息可以为文本，也可以为Content对象
    /// </remarks>
    /// <returns>提示信息</returns> 
    Object* GetToolTip();
    void SetToolTip(Object* val);
    void SetToolTip(const String& val);

    Object* GetTag();
    void SetTag(Object* val);

    Int32 GetWidth();
    void SetWidth(Int32 val);

    Int32 GetHeight();
    void SetHeight(Int32 val);

    void UpdateMinMaxInfo();

    Int32 CoercedWidth(int w);
    Int32 CoercedHeight(int h);

    /// <summary>
    /// 获取元素外边距
    /// </summary>
    /// <remarks>
    /// 元素外边距作为其测量大小一部分，但不算在渲染区域内。
    /// </remarks>
    /// <returns>元素外边距</returns> 
    Rect GetMargin();
    void SetMargin(Rect val);

    /// <summary>
    /// 设置元素最小高度
    /// </summary>
    /// <returns>最小高度</returns> 
    Int32 GetMinHeight();
    void SetMinHeight(Int32 val);

    Int32 GetMinWidth();
    void SetMinWidth(Int32 val);

    Int32 GetMaxHeight();
    void SetMaxHeight(Int32 val);

    Int32 GetMaxWidth();
    void SetMaxWidth(Int32 val);

    /// <summary>
    ///  取得元素的水平对齐方式
    /// </summary>
    /// <remarks>
    ///  对齐方式有Left、Center、Right、Stretch。
    /// </remarks>
    /// <returns>水平对齐方式</returns> 
    int GetHorizontalAlignment();
    void SetHorizontalAlignment(int val);

    /// <summary>
    ///  取得元素的垂直对齐方式
    /// </summary>
    /// <remarks>
    ///  对齐方式有Top、Center、Bottom、Stretch。
    /// </remarks>
    /// <returns>垂直对齐方式</returns> 
    int GetVerticalAlignment();
    void SetVerticalAlignment(int val);

    // 模板支持
    bool ApplyTemplate();

    void Render(Drawing* drawing);

    virtual void OnTemplateChildChanged();
    virtual void OnPreApplyTemplate();
    virtual void OnApplyTemplate();
    virtual void OnPostApplyTemplate();

    void UpdateVisualState(bool useTransitions);
    void InvalidateDataProp(Object* data);

    static void OnStylePropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnVisualStatePropChanged(DpObject* sender, DpPropChangedEventArg* e);

    void BeginInit();
    void EndInit();

    bool InvalidateStyle();

    void InvalidateResourceReference(bool onlySelf);
    void UpdateLeftMouseDownState(bool isDown);

public:

    void AddLogicalChild(Object* child);
    void RemoveLogicalChild(Object* child);

    virtual Element* GetLogicalChild(int index);
    virtual int GetLogicalChildrenCount();

    virtual FrameworkElement* GetStateGroupsRoot();
    virtual FrameworkTemplate* GetTemplateInternal();
    virtual FrameworkTemplate* GetTemplateCache();

    virtual void SetTemplateCache(FrameworkTemplate* temp);
    virtual void OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate);

    virtual void OnNewParent(FrameworkElement* newParent);
    virtual bool OnReadingChild(IXamlNode* node);

    virtual void OnToolTipOpening(ToolTipEventArg* e);
    virtual void OnToolTipClosing(ToolTipEventArg* e);

    virtual void OnRequestBringIntoView(RequestBringIntoViewEventArg* e);

protected:

    void ArrangeCore(const Rect& arrangeRect);
    Size MeasureCore(const Size& availableSize);

    void OnContextMenu(ContextMenuEventArg* e);

    void OnMouseEnter(MouseButtonEventArg* e);
    void OnSetCursor(CursorEventArg* e);

    void OnRenderSizeChanged(SizeChangedInfo& sizeInfo);
    void OnDpPropertyChanged(DpPropChangedEventArg* e);
    void OnDataContextChanged(DpPropChangedEventArg* e);
    void OnDataPropertyChanged(Object* data, PropertyChangedEventArg* e);

    virtual void OnStyleChanged(Style* oldStyle, Style* newStyle);
    virtual void UpdateMeasureSize(const Size& availableSize, Size& measureSize);

    void DoPropertyAffects(DpProperty* dp, PropMetadata* meta);
    Size CalcMaximalSpaceRect(const Matrix& matrix, Size size);

    void UpdateMinMaxInfo(int w, int h);

    void PrivateInitialized();
    void TryFireInitialized();

    /// <summary>
    ///  初始化样式属性，在EndInit调用
    /// </summary>
    /// <returns>无</returns> 
    void UpdateStyleProperty();

    void ChangeLogicalParent(FrameworkElement* newParent);
    Point ComputeAlignmentOffset(Size clientSize, Size inkSize);

    void Dispose();

    void OnVisualParentChanged(Element* oldParent);
    void ChangedTemplateParent(DpObject* parent);

    /// <summary>
    ///  状态发生变化时回调
    /// </summary>
    /// <remarks>
    ///  如果注册了OnVisualStatePropChanged，系统会自动通知
    ///  界面元素状态发生变化，UI控件应该使用VisualState改变状态。
    /// </remarks>
    /// <returns>无</returns> 
    virtual void OnChangeVisualState(bool useTransitions);

    virtual void OnAncestorChanged(TreeChangeInfo& parentTreeState);
    virtual void InvalidateChildrenResourceReference();
    virtual FrameworkElement* FindResTree(Object** res, const String& name);

protected:
    
    Int32 _width;
    Int32 _height;

    Size _unclipSize;

    // 内部标志
    Uint32 _innerFlag;
    MinMaxInfo _mmInfo;

    // 缓存界面元素资源样式
    Style* _styleCache;
    // 如果此元素由模板生成，则不为空
    WeakRef _templatedParent;
    // 界面本身资源
    ResourceDictionary* _resources;
    // 此元素由模板生成
    FrameworkElement* _templateChild;

    friend class FEFactory;
    friend class StyleHelper;
    friend class NotifyPropChanged;

private:

    Rect _margin;

    Byte _verticalAlignment;
    Byte _horizontalAlignment;

    // 逻辑父元素
    WeakRef _parent;
    Transform* _layoutTrans;

    static PropMetadata* _actualWidthMetadata;
    static PropMetadata* _actualHeightMetadata;

    friend class MemberVisitor;
    friend class FrameworkInvoker;
};

inline FrameworkElement* FrameworkElement::GetTemplateChild()
{
    return _templateChild;
}

inline FrameworkElement* FrameworkElement::GetStateGroupsRoot()
{ 
    return _templateChild;
}

inline DpObject* FrameworkElement::GetTemplatedParent()
{
    return (DpObject*)_templatedParent.GetTarget();
} 

inline bool FrameworkElement::ReadInnerFlag(Uint32 flag) const
{
    return ((flag & _innerFlag) != 0);
}

inline bool FrameworkElement::GetIsStyleSetFromGenerator() const
{
    return ReadInnerFlag(InnerFlags::IsStyleSetFromGenerator);
}

inline void FrameworkElement::SetIsStyleSetFromGenerator(bool val)
{
    WriteInnerFlag(InnerFlags::IsStyleSetFromGenerator, val);
}

inline FlowDirection FrameworkElement::GetFlowDirection()
{
    return ReadInnerFlag(InnerFlags::IsRightToLeft) ? FlowDirection::RightToLeft : FlowDirection::LeftToRight;
}

inline Style* FrameworkElement::GetStyle()
{
    return _styleCache;
}

inline ResourceDictionary* FrameworkElement::GetResources()
{
    return _resources;
}

inline int FrameworkElement::GetVisualChildrenCount()
{
    return (!_templateChild ? 0 : 1);
}

inline Element* FrameworkElement::GetVisualChild(int index)
{
    return _templateChild;
}

inline bool FrameworkElement::IsNeedsClipBounds()
{
    return ReadInnerFlag(InnerFlags::IsNeedsClipBounds);
}

inline void FrameworkElement::SetIsNeedsClipBounds(bool val)
{
    WriteInnerFlag(InnerFlags::IsNeedsClipBounds, val);
}

inline Object* FrameworkElement::GetTag()
{
    return GetValue(TagProperty);
}

inline void FrameworkElement::SetTag(Object* val)
{
    SetValue(TagProperty, val);
}

inline Int32 FrameworkElement::GetWidth()
{
    return _width;
}

inline Int32 FrameworkElement::GetHeight()
{
    return _height;
}

inline Int32 FrameworkElement::GetMaxWidth()
{
    return GetValue(MaxWidthProperty)->ToInt();
}

inline Int32 FrameworkElement:: GetMaxHeight()
{
    return GetValue(MaxHeightProperty)->ToInt();
}

inline Int32 FrameworkElement::GetMinWidth()
{
    return GetValue(MinWidthProperty)->ToInt();
}

inline Int32 FrameworkElement::GetMinHeight()
{
    return GetValue(MinHeightProperty)->ToInt();
}

inline Rect FrameworkElement::GetMargin()
{
    return _margin;
}

typedef shared<FrameworkElement> FrameworkElementPtr;

}

#endif
