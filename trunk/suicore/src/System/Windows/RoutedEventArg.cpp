/////////////////////////////////////////////////////////////////////////
// uiroutedeventarg.cpp

#include <System/Windows/RoutedEventArg.h>
#include <System/Windows/Element.h>
#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Windows/CoreTool.h>

#include <System/Interop/InternalOper.h>

namespace suic
{

/////////////////////////////////////////////////////////////////////////
// RoutedEventArg
//
RoutedEventArg::RoutedEventArg() 
    : _handled(false)
    , _routedEvent(NULL)
    , _originalSource(NULL)
    , _source(NULL)
{

}

RoutedEventArg::RoutedEventArg(Object* source) 
    : _handled(false)
    , _originalSource(source)
    , _source(source)
    , _routedEvent(NULL)
{
}

RoutedEventArg::RoutedEventArg(Object* source, RoutedEvent* routedEvent)
    : _handled(false)
    , _originalSource(source)
    , _source(source)
    , _routedEvent(routedEvent)
{
}

RoutedEventArg::~RoutedEventArg()
{

}

RoutedEvent* RoutedEventArg::GetRoutedEvent()
{
    return _routedEvent;
}

void RoutedEventArg::SetRoutedEvent(RoutedEvent* val)
{
    _routedEvent = val;
}

bool RoutedEventArg::IsHandled()
{
    return _handled;
}

void RoutedEventArg::SetHandled(bool val)
{
    _handled = val;
}

Object* RoutedEventArg::GetSource()
{
    return _source;
}

void RoutedEventArg::SetSource(Object* src)
{
    _source = src;
}

void RoutedEventArg::OnSetSource(Object* source)
{
}

void RoutedEventArg::OnOverrideOriginalSource(Object* ori)
{
    _originalSource = ori;
}

void RoutedEventArg::CallEventHandler(Object* handler, Object* target)
{
    InternalCall<DpObject, RoutedEventHandler, RoutedEventArg>(handler, target);
}

Object* RoutedEventArg::GetOriginalSource()
{
    return _originalSource;
}

ClickEventArg::ClickEventArg()
{
}

ClickEventArg::ClickEventArg(Object* source)
    : RoutedEventArg(source)
{
}

ClickEventArg::ClickEventArg(Object* source, RoutedEvent* routedEvent)
    : RoutedEventArg(source, routedEvent)
{
}

ClickEventArg::~ClickEventArg()
{
}

}
