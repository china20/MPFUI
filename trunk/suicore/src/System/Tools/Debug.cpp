/////////////////////////////////////////////////////////////////////////
// Debug.cpp

#include <System/Tools/Debug.h>
#include <System/Tools/VisualTreeOp.h>

#include <System/Interop/System.h>

//#include <cassert>

/*--------内部使用头文件--------*/
#include <System/Interop/InternalOper.h>

namespace suic
{

Debug::Debug()
{
}

void Debug::Assert(bool val)
{
#ifdef _DEBUG
    Assert(val, _T(""));
#endif
}

void Debug::Assert(bool val, const String& msg)
{
#ifdef _DEBUG
    if (!val)
    {
        //Trace(_U("Assert: %s"), msg.c_str());
        SystemLog()->Trace(_U("Assert: "), msg);
    }
    
    //assert(val);
#endif
}

void Debug::Trace(const Char * format, ...)
{
#ifdef _DEBUG
    Char buffer[1024] = {0};

    /*va_list argptr;
    va_start(argptr, format);
    wvsprintf(buffer, format, argptr);
    va_end(argptr);*/

    va_list argList;
    va_start(argList, format);
    int len = _vsctprintf(format, argList);
    _vstprintf_s(buffer, len + 1, format, argList);
    va_end(argList);

    OutputDebugString(buffer);
#endif
}

}

