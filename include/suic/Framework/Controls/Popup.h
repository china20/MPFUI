// 华勤科技版权所有 2008-2022
// 
// 文件名：Popup.h
// 功  能：实现标准的弹出窗口（无焦点）。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIPOPUP_H_
#define _UIPOPUP_H_

#include <Framework/Controls/Content.h>

namespace suic
{

enum PlacementMode
{
    pmLeft,
    pmTop,
    pmRight,
    pmBottom,
    pmCenter,
    pmMouse,
    pmMousePoint,
    pmCustom,
};

struct SUICORE_API PlacementModeBox
{
    static Integer* LeftBox;
    static Integer* TopBox;
    static Integer* RightBox;
    static Integer* BottomBox;
    static Integer* CenterBox;
    static Integer* MouseBox;
    static Integer* MousePointBox;
    static Integer* CustomBox;

    static Integer* From(int index);
};

struct PositionInfo
{
    int x;
    int y;
    Size ChildSize;
    Rect MouseRect;
};

class SUICORE_API PopupRoot : public FrameworkElement
{
public:

    PopupRoot();
    ~PopupRoot();

    RTTIOfClass(PopupRoot)

    Element* GetChild();
    void SetChild(Element* elem);

    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

    Element* GetLogicalChild(int index);
    int GetLogicalChildrenCount();

protected:

    void OnTextInput(KeyboardEventArg* e);
    void OnKeyDown(KeyboardEventArg* e);
    void OnKeyUp(KeyboardEventArg* e);

    void SetupLayoutBindings(Popup* popup);
    Size ComputeChildDesiredSize(Size restrictedSize);
    Size ComputePopupSizeRestrictions(Popup* popup, Size desiredSize, bool& bWidth, bool& bHeight);

    friend class Popup;

public:

    Size OnMeasure(const Size& constraintSize);
    void OnArrange(const Size& arrangeSize);

private:  

    Element* _child;
};

class OpeningEventArg : public EventArg
{
public:

    OpeningEventArg(HwndParam* hp)
        : _hp(hp)
    {
    }

    HwndParam* GetHwndParam() const
    {
        return _hp;
    }

private:

    HwndParam* _hp;
};

typedef delegate<void(Object*, OpeningEventArg*), suic::UnrefObj> OpeningEventHandler;

class SUICORE_API Popup : public FrameworkElement, public IAddChild
{
public:

    EventHandler Closed;
    EventHandler Opened;
    EventHandler Closing;
    OpeningEventHandler Opening;

    static DpProperty* ChildProperty;
    static DpProperty* IsOpenProperty;
    static DpProperty* CoercedPosProperty;
    static DpProperty* PlacementProperty;
    static DpProperty* PlacementRectangleProperty;
    static DpProperty* PlacementTargetProperty;
    static DpProperty* VerticalOffsetProperty;
    static DpProperty* HorizontalOffsetProperty;
    static DpProperty* AllowsTransparencyProperty;

    static void StaticInit();
    static void CreateRootPopup(Popup* popup, Element* child);

    static void OnChildPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIsOpenPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnPlacementPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnOffsetPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnPlacementTargetPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnAllowsTransparencyPropChanged(DpObject* d, DpPropChangedEventArg* e);

    Popup();
    ~Popup();

    RTTIOfClass(Popup)

    void Hide();
    void Show(Point pt);
    void Close();
    void AsyncClose();
    bool IsClosing();

    PopupRoot* GetPopupRoot();
    PositionInfo* GetPositionInfo();

    Element* GetChild();
    void SetChild(Element* elem);

    void InvalidatePopupRoot();
    void TrackingPopup(MessageHook& hook=MessageHook());

    bool IsOpen();
    void SetIsOpen(bool val);

    bool GetCoercedPos();
    void SetCoercedPos(bool val);

    PlacementMode GetPlacement();
    void SetPlacement(PlacementMode val);

    Rect GetPlacementRectangle();
    void GetPlacementRectangle(Rect val);

    Element* GetPlacementTarget();
    void SetPlacementTarget(Element* target);

    int GetVerticalOffset();
    void SetVerticalOffset(int val);

    int GetHorizontalOffset();
    void SetHorizontalOffset(int val);

    bool AllowsTransparency();
    void SetAllowsTransparency(bool val);

    bool IsChildPopup();
    void SetChildPopup(bool value);

    Popup* GetParentPopup();
    void SetParentPopup(Popup* val);

public:

    void AddChild(Object* obj);
    void AddText(String val);
    void RemoveChild(Object* child);

    void OnTextInput(KeyboardEventArg* e);
    void OnKeyDown(KeyboardEventArg* e);
    void OnKeyUp(KeyboardEventArg* e);

    virtual void OnClosing(CancelEventArg& e);
    virtual void OnClosed(EventArg* e);
    virtual void OnOpened(EventArg* e);

    virtual bool OnShowingWindow();
    virtual void OnShowWindow();
    virtual void OnHideWindow();

    virtual bool OnFilterMessage(Object* sender, MessageParam* mp, bool& interrupt);
    virtual bool OnSysFilterMessage(Object* sender, MessageParam* mp, bool& interrupt);

protected:
    
    Size OnMeasure(const Size& constratint);
    void OnArrange(const Size& arrangeSize);

    void OnHitTest(HitResultEventArg* e);
    void OnMouseWheel(MouseWheelEventArg* e);

    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseRightButtonDown(MouseButtonEventArg* e);

protected:

    Element* GetTarget();
    void InternalClose(bool async);
    int InternalCreate(int wid, int hei);
    bool BlockVisualState();
    void EnsurePopupRoot();
    void ComputePosition(Size desiredSize);

protected:

    bool _onMouseIn;
    bool _onTimer;
    bool _onDownClose;
    bool _isMouseIn;
    bool _isClosing;

    Popup* _parentPopup;
    PopupRoot* _popupRoot;
    AssignerFrame* _frame;
    PositionInfo _positionInfo;
};

typedef shared<Popup> PopupPtr;

}

#endif
