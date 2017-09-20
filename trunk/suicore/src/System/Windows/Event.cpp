/////////////////////////////////////////////////////////////////////////
// uievent.cpp

#include <System/Windows/Event.h>
#include <System/Windows/Element.h>
#include <System/Windows/DpProperty.h>
#include <System/Windows/DragEventArg.h>
#include <System/Windows/ICommandSource.h>

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>

#include <System/Windows/CoreTool.h>
#include <System/Interop/InternalOper.h>

namespace suic
{

FileDragInfo::FileDragInfo(String* files, int count)
    : _files(files)
    , _count(count)
{

}

FileDragInfo::~FileDragInfo()
{
    if (NULL != _files)
    {
        delete [] _files;
    }
}

int FileDragInfo::GetCount() const
{
    return _count;
}

String FileDragInfo::GetFilePath(int index) const
{
    return _files[index];
}

DragEventArg::DragEventArg(IDataStore* data, DragDropKeyStates dragDropKeyStates, 
                           DragDropEffects allowedEffects, DpObject* target, Point point)
    : _dropType(suic::eDropType::dtDefault)
{
    _data = data;
    if (_data != NULL)
    {
        _data->ref();
    }

    _dragDropKeyStates = dragDropKeyStates;
    _allowedEffects = allowedEffects;
    _effects = allowedEffects;
    _dropPoint = point;
}

DragEventArg::DragEventArg(IDataStore* data, DragDropKeyStates dragDropKeyStates, 
                           DragDropEffects allowedEffects, DpObject* target, 
                           Point point, suic::eDropType t)
    : _dropType(t)
{
    _data = data;
    if (_data != NULL)
    {
        _data->ref();
    }

    _dragDropKeyStates = dragDropKeyStates;
    _allowedEffects = allowedEffects;
    _effects = allowedEffects;
    _dropPoint = point;
}

DragEventArg::~DragEventArg()
{
    if (_data != NULL)
    {
        _data->unref();
    }
}

Point DragEventArg::GetPosition(Element* relativeTo)
{
    return Point();
}

DragDropEffects DragEventArg::GetAllowedEffects() const
{
    return _allowedEffects;
}

IDataStore* DragEventArg::GetData() const
{
    return _data;
}

DragDropEffects DragEventArg::GetEffects() const
{
    return _effects;
}

void DragEventArg::SetEffects(DragDropEffects val)
{
    _effects = val;
}

DragDropKeyStates DragEventArg::GetKeyStates() const
{
    return _dragDropKeyStates;
}

/////////////////////////////////////////////////////////////////////////
// PropertyChangedEventArg
//

void DpPropChangedEventArg::CallEventHandler(Object* handler, Object* target)
{
    InternalCall<Element, PropChangedCallback, DpPropChangedEventArg>(handler, target);
}

/////////////////////////////////////////////////////////////////////////
// StateChangedEventArg
//
StateChangedEventArg::StateChangedEventArg(int iType, bool val)
    : _type(iType)
    , _value(val)
{

}

void StateChangedEventArg::SetStateName(const String& name)
{
    _name = name;
}

String StateChangedEventArg::GetStateName()
{
    return _name;
}

int StateChangedEventArg::GetType()
{
    return _type;
}

bool StateChangedEventArg::GetValue()
{
    return _value;
}

void StateChangedEventArg::CallEventHandler(Object* handler, Object* target)
{

}

/////////////////////////////////////////////////////////////////////////
// ScrollPosChangedEventArg
//
ScrollChangedEventArg::ScrollChangedEventArg(Float horz, Float vert
    , Float viewwid, Float viewhei, Float maxwid, Float maxhei)
    : _horz(horz)
    , _vert(vert)
    , _viewportWid(viewwid)
    , _viewportHei(viewhei)
    , _extentWid(maxwid)
    , _extentHei(maxhei)
    , _onlySet(false)
    , _arrange(false)
    , _interrupted(false)
{
    ;
}

void ScrollChangedEventArg::CallEventHandler(Object* handler, Object* target)
{
    InternalCall<Element, ScrollChangedEventHandler, ScrollChangedEventArg>(handler, target);
}

/////////////////////////////////////////////////////////////////////////
// LoadedEventArg
//
LoadedEventArg::LoadedEventArg(DpObject* source, Object* obj)
    : _targetObj(obj)
    , RoutedEventArg(source)
{
}

LoadedEventArg::~LoadedEventArg()
{

}

Object* LoadedEventArg::GetTarget()
{
    return _targetObj;
}

/////////////////////////////////////////////////////////////////////////
// MessageEventArg
//

MessageEventArg::MessageEventArg(MessageParam* mp)
    : _mp(mp)
    , _handled(false)
{

}

MessageEventArg::~MessageEventArg()
{

}

bool MessageEventArg::IsHandled() const
{
    return _handled;
}

void MessageEventArg::SetHandled(bool handled)
{
    _handled = handled;
}

/////////////////////////////////////////////////////////////////////////
// KeyEventArg
//
KeyboardEventArg::KeyboardEventArg(DpObject* source, int c, Byte keyFlags) 
    : _ch(c)
    , RoutedEventArg(source)
{
    _keyFlags = keyFlags;
}

Byte KeyboardEventArg::GetKeyFlags() const
{
    return _keyFlags;
}

ModifierKey KeyboardEventArg::GetModifierKey() const
{ 
    return (ModifierKey)Keyboard::GetModifiers(); 
}

bool KeyboardEventArg::IsDown() const
{
    return Keyboard::IsKeyDown(GetKey());
}

bool KeyboardEventArg::IsUp() const
{
    return !Keyboard::IsKeyDown(GetKey());
}

bool KeyboardEventArg::IsAltDown() const
{
    return Keyboard::IsAltDown();
}

bool KeyboardEventArg::IsShiftDown() const
{
    return Keyboard::IsShiftDown();
}

bool KeyboardEventArg::IsControlDown() const
{
    return Keyboard::IsControlDown();
}

bool KeyboardEventArg::IsWinDown() const
{
    return Keyboard::IsWinDown();
}

int KeyboardEventArg::GetSystemKey() const
{
    return _ch;
}

bool KeyboardEventArg::IsArrowKey() const
{
    return (GetKey() >= Key::kLeft && GetKey() <= Key::kDown);
}

bool KeyboardEventArg::IsDigitalKey() const
{
    const int A0 = 48;
    const int A9 = 57;

    return (GetKey() >= A0 && GetKey() <= A9);
}

bool KeyboardEventArg::IsAlphaKey() const
{
    const int Aa = 97;
    const int Az = 122;
    const int AA= 65;
    const int AZ = 90;

    return ((GetKey() >= Aa && GetKey() <= Az) || (GetKey() >= AA && GetKey() <= AZ));
}

bool KeyboardEventArg::IsRepeat() const
{
    return (_keyFlags & KeyFlags::kfRepeat) == KeyFlags::kfRepeat;
}

void KeyboardEventArg::CallEventHandler(Object* handler, Object* target)
{
    InternalCall<Element, KeyboardEventHandler, KeyboardEventArg>(handler, target);
}

/////////////////////////////////////////////////////////////////////////
// KeyboardFocusEventArg
//
KeyboardFocusEventArg::KeyboardFocusEventArg(DpObject* source, Element* oldFocus, Element* newFocus)
    : KeyboardEventArg(source, 0, 0)
    , _oldFocus(oldFocus)
    , _newFocus(newFocus)
{

}

Element* KeyboardFocusEventArg::GetOldFocus() const
{
    return _oldFocus;
}

Element* KeyboardFocusEventArg::GetNewFocus() const
{
    return _newFocus;
}

void KeyboardFocusEventArg::CallEventHandler(Object* handler, Object* target)
{
    InternalCall<Element, KeyboardFocusEventHandler, KeyboardFocusEventArg>(handler, target);
}

InvokerArg::InvokerArg(int what, Object* data)
{
    _what = what;
    _data = data;
    if (_data != NULL)
    {
        _data->ref();
    }
}

InvokerArg::~InvokerArg()
{
    if (NULL != _data)
    {
        _data->unref();
    }
}

int InvokerArg::GetWhat() const
{
    return _what;
}

Object* InvokerArg::GetData() const
{
    return _data;
}

InvokerEventArg::InvokerEventArg(Object* source, int what, Object* data)
    : RoutedEventArg(source)
{
    _what = what;
    _data = data;
    if (NULL != _data)
    {
        _data->ref();
    }
}

InvokerEventArg::~InvokerEventArg()
{
    if (NULL != _data)
    {
        _data->unref();
    }
}

int InvokerEventArg::GetWhat() const
{
    return _what;
}

Object* InvokerEventArg::GetData() const
{
    return _data;
}

/////////////////////////////////////////////////////////////////////////
// MouseButtonEventArg
//
SizeChangedInfo::SizeChangedInfo(Size prev, Size cur, Point pPrev, Point pNew)
    : sizeOld(prev)
    , sizeNew(cur)
    , pointOld(pPrev)
    , pointNew(pNew)
{
    bWidChanged = sizeOld.cx != sizeNew.cx;
    bHeiChanged = sizeOld.cy != sizeNew.cy;
}

SizeChangedEventArg::SizeChangedEventArg(SizeChangedInfo sizeInfo)
    : _sizeInfo(sizeInfo)
    , RoutedEventArg(NULL)
{

}

Size SizeChangedInfo::GetOldSize() 
{ 
    return sizeOld; 
}

Size SizeChangedInfo::GetNewSize() 
{ 
    return sizeNew; 
}

Point SizeChangedInfo::GetOldOffset() 
{ 
    return pointOld; 
}

Point SizeChangedInfo::GetNewOffset() 
{ 
    return pointNew; 
}

bool SizeChangedInfo::WidthChanged() 
{ 
    return bWidChanged; 
}

bool SizeChangedInfo::HeightChanged() 
{ 
    return bHeiChanged; 
}

CursorEventArg::CursorEventArg(Point pt) 
    : MouseButtonEventArg(NULL, pt)
    , _ignore(false)
    , _cursor(NULL)
{
}

CursorEventArg::~CursorEventArg()
{
    if (_cursor)
    {
        _cursor->unref();
    }
}

void CursorEventArg::SetCursor(OCursor* curs)
{
    if (_cursor)
    {
        _cursor->unref();
    }
    _cursor = curs;
    if (_cursor)
    {
        _cursor->ref();
    }
}

OCursor* CursorEventArg::GetCursor()
{
    return _cursor;
}

void CursorEventArg::CallEventHandler(Object* handler, Object* target)
{
    RoutedEventArg::CallEventHandler(handler, target);
}

FindToolTipEventArg::FindToolTipEventArg() 
    : _target(NULL)
    , RoutedEventArg(NULL)
{
}

FindToolTipEventArg::~FindToolTipEventArg()
{

}

void FindToolTipEventArg::SetTargetElement(Object* obj)
{
    _target = obj;
}

Object* FindToolTipEventArg::GetTargetElement() const
{
    return _target;
}

ContextMenuEventArg::ContextMenuEventArg(DpObject* source, int cursorLeft, int cursorTop)
    : RoutedEventArg(source)
    , _cursorLeft(cursorLeft)
    , _cursorTop(cursorTop)
{
}

int ContextMenuEventArg::GetCursorLeft()
{
    return _cursorLeft;
}

int ContextMenuEventArg::GetCursorTop()
{
    return _cursorTop;
}

/////////////////////////////////////////////////////////////////////////
// MouseButtonEventArg
//
MouseButtonEventArg::MouseButtonEventArg(DpObject* source, Point pt) 
    : RoutedEventArg(source)
    , _mousept(pt)
    , _clickCount(0)
    , _buttonType(MouseButton::mbUnknown)
{
    _buttonType = Mouse::GetMouseButton();
}

MouseButtonState MouseButtonEventArg::GetMouseState() 
{ 
    MouseButtonState released = MouseButtonState::mbMouseRelease;

    switch (_buttonType)
    {
    case MouseButton::mbLeftButton:
        return MouseDevice::GetLeftButton();

    case MouseButton::mbRightButton:
        return MouseDevice::GetRightButton();

    case MouseButton::mbMiddleButton:
        return MouseDevice::GetMiddleButton();

    }

    return released;
}

void MouseButtonEventArg::CallEventHandler(Object* handler, Object* target)
{
    InternalCall<Element, MouseButtonEventHandler, MouseButtonEventArg>(handler, target);
}

/////////////////////////////////////////////////////////////////////////
// MouseWheelEventArg
//
MouseWheelEventArg::MouseWheelEventArg(Point pt, int timestamp, int delta)
    : MouseButtonEventArg(NULL, pt)
    , _delta(delta)
{
}

InvalidateEventArg::InvalidateEventArg(bool measure, bool arrange, bool render, bool force)
    : _measure(measure)
    , _arrange(arrange)
    , _render(render)
    , _force(force)
    , _prop(NULL)
{
}

DpProperty* InvalidateEventArg::GetProp() const 
{ 
    return _prop; 
}

void InvalidateEventArg::SetProp(DpProperty* dp) 
{ 
    _prop = dp; 
}

void ExecutedRoutedEventArg::CallEventHandler(Object* handler, Object* target)
{
    InternalCall<Element, ExecutedRoutedEventHandler, ExecutedRoutedEventArg>(handler, target);
}

}
