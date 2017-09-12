// 华勤科技版权所有 2008-2022
// 
// 文件名：ScrollViewer.h
// 功  能：实现滚动视图容器
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UISCROLLVIEWER_H_
#define _UISCROLLVIEWER_H_

#include <Framework/Controls/Panel.h>
#include <Framework/Controls/Content.h>
#include <Framework/Controls/DataTemplate.h>

namespace suic
{

class ScrollBar;

enum ScrollBarVisibility : int
{
    sbvDisabled,
    sbvAutoScroll,
    sbvHidden,
    sbvVisible,
    sbvCollapsed,
};

class SUICORE_API SBVisibilityConvert : public IConvertValue
{
public:

    static SBVisibilityConvert* Convert;
    AutoObj ConvertValue(DpProperty* dp, Object* val);
};

/// <summary>
/// ScrollViewer。
/// </summary>
class SUICORE_API ScrollViewer : public ContentControl
{
public:

    static const int scScrollLineDelta;
    static const int scMouseWheelDelta;

    static DpProperty* CanContentScrollProperty;
    static DpProperty* SmoothingScrollProperty;
;
    static DpProperty* VerticalOffsetProperty;
    static DpProperty* HorizontalOffsetProperty;

    static DpProperty* HorizontalScrollBarVisibilityProperty;
    static DpProperty* VerticalScrollBarVisibilityProperty;
    static DpProperty* ComputedHorizontalScrollBarVisibilityProperty;
    static DpProperty* ComputedVerticalScrollBarVisibilityProperty;

    static RoutedEvent* ScrollChangedEvent;

    static void OnScrollCommand(Object* target, ExecutedRoutedEventArg* e);
    static void OnVerticalOffsetPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnHorizontalOffsetPropChanged(DpObject* d, DpPropChangedEventArg* e);

    static void StaticInit();

    ScrollViewer();
    virtual ~ScrollViewer();

    RTTIOfClass(ScrollViewer)

public:

    void AddScrollChanged(ScrollChangedEventHandler* ev);
    void RemoveScrollChanged(ScrollChangedEventHandler* ev);

    IScrollInfo* GetScrollInfo();
    void SetScrollInfo(IScrollInfo* scrollInfo);

    bool CanContentScroll();
    bool GetSmoothingScroll();
    void SetSmoothingScroll(bool val);

    int GetExtentWidth();
    int GetExtentHeight();

    int GetScrollableHeight();
    int GetScrollableWidth();

    int GetViewportHeight();
    int GetViewportWidth();

    int GetHorizontalOffset();
    int GetVerticalOffset();

    void SetHorizontalOffset(int val);
    void SetVerticalOffset(int val);

    ScrollBarVisibility GetHorizontalScrollBarVisibility();
    void SetHorizontalScrollBarVisibility(ScrollBarVisibility val);

    ScrollBarVisibility GetVerticalScrollBarVisibility();
    void SetVerticalScrollBarVisibility(ScrollBarVisibility val);

    void PageDown();
    void PageLeft();
    void PageRight();
    void PageUp();

    void LineDown();
    void LineLeft();
    void LineRight();
    void LineUp();

    void ScrollToEnd(AxisDirection axis);
    void ScrollToHome(AxisDirection axis);

    void ScrollToLeft();
    void ScrollToRight();
    void ScrollToTop();
    void ScrollToBottom();

    void RefleshLayout(bool bOnlyRender);

    bool GetInMeasure() const;
    void SetInMeasure(bool val);
    bool GetInChildInvalidateMeasure() const;
    void SetInChildInvalidateMeasure(bool val);

    bool GetRequestComputeMeasure() const;
    void SetRequestComputeMeasure(bool val);

    void InvalidateScrollInfo(bool bRaised);
    void ScrollToVerticalOffsetByAnimation(int iStart, int iOffset, int duration, IEasingFunction* func);
    
    // 下面这两个函数不要直接调用(内部使用)
    void ScrollToVerticalOffset(int offset);
    void ScrollToHorizontalOffset(int offset);

protected:

    Size OnMeasure(const Size& constraint);
    void OnArrange(const Size& arrangeSize);

    void OnApplyTemplate();
    void OnPreApplyTemplate();

    void OnMouseWheel(MouseWheelEventArg* e);

    virtual void OnScrollChanged(ScrollChangedEventArg* e);

private:

    void CoerceVerticalPos(int& iPos);
    void CoerceHorizontalPos(int& iPos);

    void SetHoriScrollBarVisibility(Visibility val);
    void SetVertScrollBarVisibility(Visibility val);

    Size RecalcMeasure(const Size& constraint);

    void SetScrollChangedDirty(bool val);
    bool IsScrollChangedDirty() const;

    void RaiseScrollChanged();

    void BindToTemplatedParent(DpProperty* property);
    void SetFlagValue(int flag, bool val);
    void DoLayoutUpdated(Object* sender, EventArg* e);

private:

    int _xSize;
    int _ySize;

    int _xExtent;
    int _yExtent;

    int _flags;

    Visibility _horiVisible;
    Visibility _vertVisible;

    ScrollBar* _horiScroll;
    ScrollBar* _vertScroll;
    IScrollInfo* _scrollInfo;
};

inline int ScrollViewer::GetExtentWidth()
{
    return _xExtent;
}

inline int ScrollViewer::GetExtentHeight()
{
    return _yExtent;
}

}

#endif
