// 华勤科技版权所有 2008-2022
// 
// 文件名：datehelp.h
// 功  能：日期辅助类。
// 
// 作  者：MPF开发组
// 时  间：2011-05-02
// 
// ============================================================================

#ifndef _UIDATEHELP_H_
#define _UIDATEHELP_H_

#include <System/Windows/Object.h>

namespace suic
{

class DateHelp
{
public:

    static void DaysToYMD(int iDate, Uint16& iYear, Byte& iMonth, Byte& iDay);
    static int YMDToDays(int iYear, int iMonth, int iDay);
    static int MonthEndDay(int iYear, int iMonth);
    static bool IsLeapYear(int iYear);
};

}

#endif
