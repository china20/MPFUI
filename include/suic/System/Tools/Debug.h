// ���ڿƼ���Ȩ���� 2010-2012
// 
// �ļ�����uidebug.h
// ��  �ܣ���ʽ�����ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-05-01
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
