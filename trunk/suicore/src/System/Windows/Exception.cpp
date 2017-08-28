// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
//
// ======================================================================

#include <System/Windows/Exception.h>
#include <System/Interop/System.h>

namespace suic 
{

Exception::Exception()
{

}

Exception::Exception(String errorMsg, char* f, int l)
{
    _lineInfo.Format(_U("line:%d;file:%s"), l, String(f).c_str());
    _errorMsg = errorMsg;
    SystemLog()->Error(String().Format(_U("%s\n%s\n"), _errorMsg.c_str(), _lineInfo.c_str()));
}

Exception::Exception(String tag, String errorMsg, char* f, int l)
{
    _lineInfo.Format(_U("line:%d;file:%s"), l, String(f).c_str());
    _errorMsg.Format(_U("%s -> %s"), tag.c_str(), errorMsg.c_str());
    SystemLog()->Error(String().Format(_U("%s\n%s\n"), _errorMsg.c_str(), _lineInfo.c_str()));
}

String Exception::GetErrorMsg() const
{
    return _errorMsg;
}

String Exception::GetLineInfo() const
{
    return _lineInfo;
}

InvalidOperationException::InvalidOperationException(String info, char* f, int l)
    : Exception(info, f, l)
{
}

InvalidOperationException::InvalidOperationException(String oper, String info, char* f, int l)
    : Exception(oper, info, f, l)
{
}

ArgumentException::ArgumentException(String arg, char* f, int l)
    : Exception(arg, f, l)
{
}

ArgumentException::ArgumentException(String arg, String info, char* f, int l)
    : Exception(arg, info, f, l)
{
}

ArgumentNullException::ArgumentNullException(String arg, char* f, int l)
    : Exception(arg, f, l)
{
}

ArgumentNullException::ArgumentNullException(String arg, String info, char* f, int l)
    : Exception(arg, info, f, l)
{
}

InvalidValueException::InvalidValueException(String err, char* f, int l)
    : Exception(err, f, l)
{
}

}
