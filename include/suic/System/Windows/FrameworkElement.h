// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����FrameworkElement.h
// ��  �ܣ�������Ŀ��FrameworkElement���ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-05-02
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
///  �����ڽ����ϵĿ���Ԫ�ػ��࣬ʵ�ֿ�ܿ���Ԫ�ع��������ͽӿ�.
///  ������������ӵ�д�����Դ�����綨ʱ��������ʽ�ȡ�
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

    // ����֧��
    virtual ICommandSource* GetCommandSource();
    // ����֧��
    virtual IScrollInfo* GetScrollInfo();    
    virtual Rect MakeVisible(Element* visual, Rect rect);

    Element* GetTemplateChild(const String& childName);
    DpObject* GetTemplatedParent();

    /// <summary>
    ///  �ݹ��ʼ��Ԫ��
    /// </summary>
    /// <returns>��</returns> 
    void RecursiveInit();

    /// <summary>
    /// ���ҽ���Ԫ�ص���Դ
    /// </summary>
    /// <remarks>
    /// ��Դ�ǿ��Լ̳еģ�ϵͳ���ӱ�Ԫ������ָ����Դ��
    /// Ȼ������������ֱ���ҵ�һ����Դ��
    /// ��󽫻�����ϵͳ��Դ��Application���壩��
    /// </remarks>
    /// <param name="name">��Դ����</param>
    /// <returns>��Դ����</returns> 
    Object* FindRes(const String& name);
    Object* FindRes(FrameworkElement* parent, const String& name);

    /// <summary>
    /// ���ý���Ԫ�ر������Դ����
    /// </summary>
    /// <param name="resources">��Դ����</param>
    /// <returns>��</returns> 
    void SetResources(ResourceDictionary* resources);

    /// <summary>
    /// ��ý���Ԫ�ر������Դ����
    /// </summary>
    /// <returns>��Դ����</returns> 
    ResourceDictionary* GetResources();

    void ShowToolTip(Point pt);

    bool ReadInnerFlag(Uint32 flag) const;
    void WriteInnerFlag(Uint32 flag, bool val);

    bool GetIsStyleSetFromGenerator() const;
    void SetIsStyleSetFromGenerator(bool val);

    /// <summary>
    ///  ʹԪ�ؽ���ɼ���Χ
    /// </summary>
    /// <remarks>
    ///  ���Ԫ�ز��ڿɼ���Χ���˷���ʹ��Ԫ�ؽ���ɼ���Χ������ʾ��
    /// </remarks>
    /// <returns>��</returns>
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

    // ֻ������
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

    static bool StaticInit();

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
    /// ���ý���Ԫ�ص���ʽ����
    /// </summary>
    /// <remarks>
    /// ���ý�����ʽ����Ӱ��Ԫ�صĳ��֣����bStyleΪtrue��
    /// �򽫻�ʹ����ʽ��ʼ��Ԫ�صĿ�ȡ��߶ȵȹ������ԡ�
    /// ���һᴥ��OnPropertyChanged()�¼����á�
    /// </remarks>
    /// <param name="thePtr">
    ///     ��ʽ����
    /// </param>
    /// <returns>��</returns> 
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
    ///  �õ�����Ԫ�ص��߼���Ԫ�ء�
    /// </summary>
    /// <returns>�߼���Ԫ��</returns>
    FrameworkElement* GetParent() const;

    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

    /// <summary>
    /// ����Ԫ����ʾ��Ϣ
    /// </summary>
    /// <remarks>
    /// ������Ԫ��ʱ������ʾ���뿪ʱ�ر�
    /// ��ʾ��Ϣ����Ϊ�ı���Ҳ����ΪContent����
    /// </remarks>
    /// <returns>��ʾ��Ϣ</returns> 
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
    /// ��ȡԪ����߾�
    /// </summary>
    /// <remarks>
    /// Ԫ����߾���Ϊ�������Сһ���֣�����������Ⱦ�����ڡ�
    /// </remarks>
    /// <returns>Ԫ����߾�</returns> 
    Rect GetMargin();
    void SetMargin(Rect val);

    /// <summary>
    /// ����Ԫ����С�߶�
    /// </summary>
    /// <returns>��С�߶�</returns> 
    Int32 GetMinHeight();
    void SetMinHeight(Int32 val);

    Int32 GetMinWidth();
    void SetMinWidth(Int32 val);

    Int32 GetMaxHeight();
    void SetMaxHeight(Int32 val);

    Int32 GetMaxWidth();
    void SetMaxWidth(Int32 val);

    /// <summary>
    ///  ȡ��Ԫ�ص�ˮƽ���뷽ʽ
    /// </summary>
    /// <remarks>
    ///  ���뷽ʽ��Left��Center��Right��Stretch��
    /// </remarks>
    /// <returns>ˮƽ���뷽ʽ</returns> 
    int GetHorizontalAlignment();
    void SetHorizontalAlignment(int val);

    /// <summary>
    ///  ȡ��Ԫ�صĴ�ֱ���뷽ʽ
    /// </summary>
    /// <remarks>
    ///  ���뷽ʽ��Top��Center��Bottom��Stretch��
    /// </remarks>
    /// <returns>��ֱ���뷽ʽ</returns> 
    int GetVerticalAlignment();
    void SetVerticalAlignment(int val);

    // ģ��֧��
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
    ///  ��ʼ����ʽ���ԣ���EndInit����
    /// </summary>
    /// <returns>��</returns> 
    void UpdateStyleProperty();

    void ChangeLogicalParent(FrameworkElement* newParent);
    Point ComputeAlignmentOffset(Size clientSize, Size inkSize);

    void Dispose();

    void OnVisualParentChanged(Element* oldParent);
    void ChangedTemplateParent(DpObject* parent);

    /// <summary>
    ///  ״̬�����仯ʱ�ص�
    /// </summary>
    /// <remarks>
    ///  ���ע����OnVisualStatePropChanged��ϵͳ���Զ�֪ͨ
    ///  ����Ԫ��״̬�����仯��UI�ؼ�Ӧ��ʹ��VisualState�ı�״̬��
    /// </remarks>
    /// <returns>��</returns> 
    virtual void OnChangeVisualState(bool useTransitions);

    virtual void OnAncestorChanged(TreeChangeInfo& parentTreeState);
    virtual void InvalidateChildrenResourceReference();
    virtual FrameworkElement* FindResTree(Object** res, const String& name);

protected:
    
    Int32 _width;
    Int32 _height;

    Size _unclipSize;

    // �ڲ���־
    Uint32 _innerFlag;
    MinMaxInfo _mmInfo;

    // �������Ԫ����Դ��ʽ
    Style* _styleCache;
    // �����Ԫ����ģ�����ɣ���Ϊ��
    WeakRef _templatedParent;
    // ���汾����Դ
    ResourceDictionary* _resources;
    // ��Ԫ����ģ������
    FrameworkElement* _templateChild;

    friend class FEFactory;
    friend class StyleHelper;
    friend class NotifyPropChanged;

private:

    Rect _margin;

    Byte _verticalAlignment;
    Byte _horizontalAlignment;

    // �߼���Ԫ��
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
