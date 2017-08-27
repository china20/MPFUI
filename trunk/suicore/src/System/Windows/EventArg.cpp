/////////////////////////////////////////////////////////////////////////
// uieventarg.cpp

#include <System/Windows/EventArg.h>
#include <System/Windows/Element.h>
#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Windows/CoreTool.h>

#include <System/Interop/InternalOper.h>

namespace suic
{

EventArg EventArg::Empty;

EventArg::EventArg()
    : _timestamp(0)
{
}

Uint32 EventArg::Timestamp()
{
    return _timestamp;
}

}
