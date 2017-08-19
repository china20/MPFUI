// 华勤科技版权所有 2010-2011
// 
// 文件名：ScrollBar.h
// 功  能：滚动条。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UISCROLLBAR_H_
#define _UISCROLLBAR_H_

#include <Framework/Controls/Track.h>
#include <Framework/Controls/RangeBase.h>
#include <Framework/Controls/RepeatButton.h>

namespace suic
{

enum ScrollEventType
{
    EndScroll = 0,
    First = 1,
    LargeDecrement = 2,
    LargeIncrement = 3,
    Last = 4,
    SmallDecrement = 5,
    SmallIncrement = 6,
    ThumbPosition = 7,
    ThumbTrack = 8,
};

class ScrollEventArg : public RoutedEventArg
{
public:
    
    ScrollEventArg(ScrollEventType scrollEventType, Float newValue);

    Float GetNewValue() const;
    ScrollEventType GetScrollEventType() const;

private:

    Float _newValue;
    ScrollEventType _scrollEventType;
};

inline Float ScrollEventArg::GetNewValue() const
{
    return _newValue;
}

inline ScrollEventType ScrollEventArg::GetScrollEventType() const
{
    return _scrollEventType;
}

class SUICORE_API ScrollBar : public RangeBase
{
public:

    static DpProperty* OrientationProperty;
    static DpProperty* ViewportSizeProperty;

    static RoutedEvent* ScrollEvent;

    static RoutedCommand* ScrollHereCommand;
    static RoutedCommand* PageUpCommand;
    static RoutedCommand* PageDownCommand;

    static RoutedCommand* PageLeftCommand;
    static RoutedCommand* PageRightCommand;

    static RoutedCommand* LineUpCommand;
    static RoutedCommand* LineDownCommand;

    static RoutedCommand* LineLeftCommand;
    static RoutedCommand* LineRightCommand;

    static RoutedCommand* ScrollToTopCommand;
    static RoutedCommand* ScrollToBottomCommand;
    static RoutedCommand* ScrollToLeftCommand;
    static RoutedCommand* ScrollToRightCommand;

    static RoutedCommand* ScrollToVerticalOffsetCommand;
    static RoutedCommand* ScrollToHorizontalOffsetCommand;

    static void OnOrientationPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnScrollCommand(Object* target, ExecutedRoutedEventArg* args);
    static void OnPropThumbDragCompleted(DpObject* sender, RoutedEventArg* e);
    static void OnPropThumbDragDelta(DpObject* sender, RoutedEventArg* e);
    static void OnPropThumbDragStarted(DpObject* sender, RoutedEventArg* e);
    static void OnViewChanged(DpObject* d, DpPropChangedEventArg* e);

    ScrollBar();
    virtual ~ScrollBar();

    static bool StaticInit();

    RTTIOfClass(ScrollBar)

    /// <summary>
    ///  获取界面元素的方向
    /// </summary>
    /// <remarks>
    ///  界面元素的方向分为水平和垂直（Horizontal和Vertical）。
    /// </remarks>
    /// <returns>Horizontal: 水平; Vertical: 垂直</returns> 
    Orientation GetOrientation();
    void SetOrientation(Orientation val);

    int GetViewportSize();
    void SetViewportSize(int val);

    void AddScroll(RoutedEventHandler* arg);
    void RemoveScroll(RoutedEventHandler* arg);

    Track* GetTrack() const;

    void OnThumbDragCompleted(DragCompletedEventArg* e);

protected:

    void OnApplyTemplate();
    void OnMouseWheel(MouseWheelEventArg* e);

    void OnPreviewMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnPreviewMouseRightButtonUp(MouseButtonEventArg* e);

    void ScrollToPosByAnimation(DpProperty* dp, int iStart, int iOffset, int duration, IEasingFunction* func);

protected:

    bool IsStandalone() const;
    void SetIsStandalone(bool val);

    void RaiseScrollEvent(ScrollEventType scrollEventType);
    void ScrollToLastMousePoint();

    static bool IsValidOrientation(Object* o);

    friend class ScrollViewer;

private:

    void ChangeValue(int newValue, bool defer);

    void ToLargeDecrement();
    void ToLargeIncrement();

    void LineDown();
    void LineLeft();
    void LineRight();
    void LineUp();

    void PageDown();
    void PageLeft();
    void PageRight();
    void PageUp();

    void ScrollToBottom();
    void ScrollToLeftEnd();
    void ScrollToRightEnd();
    void ScrollToTop();
    void ToSmallDecrement();
    void ToSmallIncrement();
    void ToMaximum();
    void ToMinimum();
    void UpdateValue(int horizontalDragDelta, int verticalDragDelta);

    void FinishDrag();

private:

    // 滑块滚动信息
    bool _isStandalone;

    Track* _track;
    bool _canScroll;
    bool _hasScrolled;
    Point _thumbOffset;
    Float _previousValue;
    Point _latestRightButtonClickPoint;
};

inline Track* ScrollBar::GetTrack() const
{
    return _track;
}

inline Orientation ScrollBar::GetOrientation()
{
    return (Orientation)DpObject::GetValue(OrientationProperty)->ToInt();
}

inline void ScrollBar::SetOrientation(Orientation val)
{
    DpObject::SetValue(OrientationProperty, OrientationBox::From(val));
}

inline bool ScrollBar::IsStandalone() const
{
    return _isStandalone;
}

inline void ScrollBar::SetIsStandalone(bool val)
{
    _isStandalone = val;
}

inline int ScrollBar::GetViewportSize()
{
    return DpObject::GetValue(ViewportSizeProperty)->ToInt();
}

inline void ScrollBar::SetViewportSize(int val)
{
    DpObject::SetValue(ViewportSizeProperty, new Integer(val));
}

}

#endif