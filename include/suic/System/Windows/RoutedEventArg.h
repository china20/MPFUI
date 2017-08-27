// 华勤科技版权所有 2008-2022
// 
// 文件名：uiroutedeventarg.h
// 功  能：定义核心库的路由事件基类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIROUTEDEVENTARG_H_
#define _UIROUTEDEVENTARG_H_

#include <System/Input/Keyboard.h>
#include <System/Windows/Event.h>
#include <System/Windows/DpObject.h>
#include <System/Windows/HwndAdapter.h>
#include <System/Windows/RoutetEvent.h>

namespace suic
{

class DpProperty;
class Element;

class SUICORE_API RoutedEventArg : public EventArg
{
public:

    RoutedEventArg();
    RoutedEventArg(Object* source);
    RoutedEventArg(Object* source, RoutedEvent* routedEvent);

    ~RoutedEventArg();

    // 事件是否已被处理
    bool IsHandled();

    // 设置事件处理标志
    void SetHandled(bool val);

    RoutedEvent* GetRoutedEvent();
    void SetRoutedEvent(RoutedEvent* val);

    // 激发事件的界面元素
    Object* GetSource() ;
    void SetSource(Object* el);

    // 拥有鼠标的界面元素
    // 如果此值为空，鼠标离开窗口
    Object* GetOriginalSource();

    virtual void OnSetSource(Object* source);
    virtual void OnOverrideOriginalSource(Object* ori);
    virtual void CallEventHandler(Object* handler, Object* target);

    template<typename T, typename H, typename A>
    void InternalCall(Object* handler, Object* target)
    {
        T* pElem(dynamic_cast<T*>(target));
        H* pHandler(dynamic_cast<H*>(handler));
        if (pHandler && pElem)
        {
            (*pHandler)(pElem, ((A*)this));
        }
    }

public:

    bool _handled;
    Object* _source;
    Object* _originalSource;
    RoutedEvent* _routedEvent;
};

class SUICORE_API ToolTipEventArg : public RoutedEventArg
{
public:

    ToolTipEventArg(bool opening, HwndParam* hp);

    HwndParam* GetHwndParam() const;

protected:

    void CallEventHandler(Object* handler, Object* target);

private:

    HwndParam* _hp;
};

class SUICORE_API InvokerArg : public EventArg
{
public:

    InvokerArg(int what, Object* data);
    ~InvokerArg();

    int GetWhat() const;
    Object* GetData() const;

private:

    int _what;
    Object* _data;
};

class SUICORE_API InvokerEventArg : public RoutedEventArg
{
public:

    InvokerEventArg(Object* source, int what, Object* data);
    ~InvokerEventArg();

    int GetWhat() const;
    Object* GetData() const;

private:

    int _what;
    Object* _data;
};

class ClickEventArg : public RoutedEventArg
{
public:

    ClickEventArg(Object* source, RoutedEvent* routedEvent);

protected:

    void CallEventHandler(Object* handler, Object* target);
};

//==============================================================
//

class SUICORE_API SizeModeChangedEventArg : public RoutedEventArg
{
public:

    SizeModeChangedEventArg(Size size, bool bEnd)
        : _finialSize(size), _isExit(bEnd)
    {
    }

    bool IsExitMode()
    {
        return _isExit;
    }

    Size GetFinalSize()
    {
        return _finialSize;
    }

protected:

    bool _isExit;
    Size _finialSize;
};

/// <summary>
/// 窗口尺寸位置改变时结构
/// </summary>
struct SUICORE_API SizeChangedInfo
{
public:

    SizeChangedInfo(Size prev, Size n, Point pPrev, Point pNew);

    Size GetOldSize();
    Size GetNewSize();

    Point GetOldOffset();
    Point GetNewOffset();

    bool WidthChanged();
    bool HeightChanged();

protected:

    bool bWidChanged;
    bool bHeiChanged;

    Size sizeOld;
    Size sizeNew;

    Point pointOld;
    Point pointNew;
};

class SUICORE_API SizeChangedEventArg : public RoutedEventArg
{
public:

    SizeChangedEventArg(SizeChangedInfo sizeInfo);

    SizeChangedInfo* GetSizeChangedInfo() { return &_sizeInfo; }

private:

    SizeChangedInfo _sizeInfo;
};

class SUICORE_API IsVisibleChangedEventArg : public EventArg
{
public:

    IsVisibleChangedEventArg(bool v) : _v(v) {}

    bool GetValue()
    {
        return _v;
    }

protected:

    bool _v;
};

class SUICORE_API IsEnabledChangedEventArg : public IsVisibleChangedEventArg
{
public:

    IsEnabledChangedEventArg(bool v)
        : IsVisibleChangedEventArg(v)
    {}
};

class SUICORE_API FindToolTipEventArg : public RoutedEventArg
{
public:

    FindToolTipEventArg();
    ~FindToolTipEventArg();

    void SetTargetElement(Object* obj);
    Object* GetTargetElement() const;

private:

    Object* _target;
};

class SUICORE_API ContextMenuEventArg : public RoutedEventArg
{
public:

    ContextMenuEventArg(DpObject* source, int cursorLeft, int cursorTop);

    int GetCursorLeft();
    int GetCursorTop();

protected:

    int _cursorLeft;
    int _cursorTop;
};

/// <summary>
/// 定义鼠标测试结果结构，当鼠标在某一个界面元素上时会返回一个测试码。
/// </summary>
typedef struct tagHitResult
{
public:

    tagHitResult() {}

    tagHitResult(Point pt) 
        : hitcode(-1), hitpoint(pt) {}

    Int32 HitTestCode() { return hitcode; }
    void HitTestCode(Int32 code) { hitcode = code; }

    Point HitTestPoint() { return hitpoint; }

    Point hitpoint;
    Int32 hitcode;

}HitResult;

class SUICORE_API HitResultEventArg : public RoutedEventArg
{
public:

    HitResultEventArg(DpObject* source, Point point)
        : RoutedEventArg(source)
        , _hr(point) {}

    Point GetPoint()
    {
        return _hr.HitTestPoint();
    }

    HitResult& GetHitResult()
    {
        return _hr;
    }

protected:

    HitResult _hr;
};

class SUICORE_API DataTransferEventArg : public RoutedEventArg
{
public:
};

class SUICORE_API StateChangedEventArg : public RoutedEventArg
{
public:

    StateChangedEventArg(int iType, bool val);

    void SetStateName(const String& name);
    String GetStateName();

    int GetType();
    bool GetValue();

    void CallEventHandler(Object* handler, Object* target);

protected:

    String _name;
    int _type;
    bool _value;
};

class SUICORE_API ScrollChangedEventArg : public RoutedEventArg
{
public:

    ScrollChangedEventArg(Float horz, Float vert
        , Float viewwid, Float viewhei, Float maxwid, Float maxhei);

    Float GetHoriScrollOffset() { return _horz; }
    Float GetVertScrollOffset() { return _vert; }

    Float GetViewportWid() { return _viewportWid; }
    Float GetViewportHei() { return _viewportHei; }

    Float GetExtentWid() { return _extentWid; }
    Float GetExtentHei() { return _extentHei; }

    bool IsOnlySet() { return _onlySet; }
    void SetOnlySet(bool val) { _onlySet = val; }

    bool IsArrange() { return _arrange; }
    void SetIsArrange(bool val) { _arrange = val; }

    bool IsInterrupted() { return _interrupted; }
    void SetInterrupted(bool val) { _interrupted = val; }

    void CallEventHandler(Object* handler, Object* target);

protected:

    Float _horz;
    Float _vert;    
    Float _extentWid;
    Float _extentHei;
    Float _viewportWid;
    Float _viewportHei;

    bool _interrupted;
    bool _onlySet;
    bool _arrange;
};

class SUICORE_API LoadedEventArg : public RoutedEventArg
{
public:

    LoadedEventArg(DpObject* source, Object* obj);
    ~LoadedEventArg();

    Object* GetTarget();

protected:

    Object* _targetObj;
};

/// <summary>
/// 处理键盘事件
/// </summary>
class SUICORE_API KeyboardEventArg : public RoutedEventArg
{
public:

    KeyboardEventArg(DpObject* source, int c, Byte keyFlags);

    int GetKey() const;
    void SetKey(int ch);
    int GetSystemKey() const;

    Byte GetKeyFlags() const;
    ModifierKey GetModifierKey() const;

    bool IsDown() const;
    bool IsUp() const;

    bool IsAltDown() const;
    bool IsShiftDown() const;
    bool IsControlDown() const;
    bool IsWinDown() const;

    bool IsArrowKey() const;
    bool IsDigitalKey() const;
    bool IsAlphaKey() const;

    bool IsRepeat() const;

    void CallEventHandler(Object* handler, Object* target);

protected:

    int _ch;
    Byte _keyFlags;
};

inline int KeyboardEventArg::GetKey() const
{ 
    return _ch; 
}

inline void KeyboardEventArg::SetKey(int ch)
{
    _ch = ch;
}

class SUICORE_API KeyboardFocusEventArg : public KeyboardEventArg
{
public:

    KeyboardFocusEventArg(DpObject* source, Element* oldFocus, Element* newFocus);

    Element* GetOldFocus() const;
    Element* GetNewFocus() const;

    void CallEventHandler(Object* handler, Object* target);

private:

    Element* _oldFocus;
    Element* _newFocus;
};

class SUICORE_API MessageEventArg : public EventArg
{
public:

    MessageEventArg(MessageParam* mp);
    ~MessageEventArg();

    MessageParam* GetMessageParam() const;

    bool IsHandled() const;
    void SetHandled(bool handled);

private:

    bool _handled;
    MessageParam* _mp;
};

inline MessageParam* MessageEventArg::GetMessageParam() const
{
    return _mp;
}

/// <summary>
/// 处理鼠标事件
/// </summary>
class SUICORE_API MouseButtonEventArg : public RoutedEventArg
{
public:

    MouseButtonEventArg(DpObject* source, Point pt);

    MouseButtonState GetMouseState();
    MouseButton GetMouseButton();
    void SetMouseButton(MouseButton mbType);

    int GetClickCount();
    void SetClickCount(int val);

    Point GetMousePoint();
 
    void CallEventHandler(Object* handler, Object* target);

protected:

    int _clickCount;
    Point _mousept;
    MouseButton _buttonType;
    MessageParam* _mp;
};

inline int MouseButtonEventArg::GetClickCount()
{
    return _clickCount;
}

inline void MouseButtonEventArg::SetClickCount(int val)
{
    _clickCount = val;
}

inline Point MouseButtonEventArg::GetMousePoint() 
{ 
    return _mousept; 
}

inline MouseButton MouseButtonEventArg::GetMouseButton()
{
    return _buttonType;
}

inline void MouseButtonEventArg::SetMouseButton(MouseButton mbType)
{
    _buttonType = mbType;
}

/// <summary>
/// 处理鼠标的滚轮事件
/// </summary>
class SUICORE_API MouseWheelEventArg : public MouseButtonEventArg
{
public:

    MouseWheelEventArg(Point pt, int timestamp, int delta);
    int GetDelta() { return _delta; }

protected:

    int _delta;
};

class OCursor;

/// <summary>
/// 处理光标事件
/// </summary>
class SUICORE_API CursorEventArg : public MouseButtonEventArg
{
public:

    CursorEventArg(Point pt);
    ~CursorEventArg();

    void SetCursor(OCursor* curs);
    OCursor* GetCursor();

    bool IsIgnore() { return _ignore; }
    void SetIgnore(bool val) { _ignore = val; }

    void CallEventHandler(Object* handler, Object* target);

protected:

    OCursor* _cursor;
    bool _ignore;
};

/// <summary>
/// 显示元素到可视区
/// </summary>
class RequestBringIntoViewEventArg : public RoutedEventArg
{
public:

    RequestBringIntoViewEventArg(DpObject* sender, Rect rect)
        : RoutedEventArg(sender)
        , _rect(rect) {}

    Rect GetRect();

protected:

    Rect _rect;
};

inline Rect RequestBringIntoViewEventArg::GetRect()
{
    return _rect;
}

/// <summary>
/// 测量、布局、刷新界面事件参数
/// </summary>
class SUICORE_API InvalidateEventArg : public RoutedEventArg
{
public:

    InvalidateEventArg(bool measure, bool arrange, bool render, bool force);

    bool IsMeasure() { return _measure; }
    bool IsArrange() { return _arrange; }
    bool IsRender() { return _render; }
    bool IsForce() { return _force; }

    Rect GetBounds() { return _rect; }
    void SetBounds(Rect rect) { _rect = rect; }

    DpProperty* GetProp() const;
    void SetProp(DpProperty* dp);

protected:

    bool _measure;
    bool _arrange;
    bool _render;
    bool _force;

    DpProperty* _prop;

    Rect _rect;
};

class SUICORE_API DpPropChangedEventArg : public RoutedEventArg
{
public:

    DpPropChangedEventArg(DpProperty* prop, PropMetadata* metadata, Object* oldValue, Object* newValue);
    ~DpPropChangedEventArg();

    String GetName();

    Object* GetNewValue();
    Object* GetOldValue();

    int GetFlags() const;

    bool IsAValueChange();
    void SetIsAValueChange(bool val);

    void CallEventHandler(Object* handler, Object* target);

public:

    DpProperty* GetProp();
    PropMetadata* GetMetadata();

protected:

    int _flags;
    Object* _oldValue;
    Object* _newValue;
    DpProperty* _property;    
    PropMetadata* _metadata;
};

inline int DpPropChangedEventArg::GetFlags() const
{
    return _flags;
}

class SUICORE_API PropertyNotifyEventArg : public EventArg
{
public:

    PropertyNotifyEventArg(const String& name, const String& propname, Uint32 flag);

    String GetElemName();
    String GetPropName();
    bool IsSelfElement();
    bool IsNeedUpdate();

protected:

    // 绑定的元素名称(Name标定的值)
    String _elemName;
    // 绑定的元素的属性
    String _propName;
    // 操作标志
    Uint32 _flag;
};

inline String PropertyNotifyEventArg::GetElemName()
{
    return _elemName;
}

inline String PropertyNotifyEventArg::GetPropName()
{
    return _propName;
}

}

#endif
