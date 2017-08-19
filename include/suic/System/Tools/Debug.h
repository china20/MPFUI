// 华勤科技版权所有 2010-2012
// 
// 文件名：uidebug.h
// 功  能：调式帮助类。
// 
// 作  者：MPF开发组
// 时  间：2010-05-01
// 
// ============================================================================

#ifndef _UIDEBUG_H_
#define _UIDEBUG_H_

#include <System/Windows/Object.h>

namespace suic
{


class SUICORE_API Debug
{
public:

    static void Assert(bool val);
    static void Assert(bool val, const String& msg);

    static void Trace(const Char * format, ...);

protected:

    Debug();

};

};

#endif
