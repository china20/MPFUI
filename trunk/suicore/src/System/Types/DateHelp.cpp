// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// DateHelp.cpp

#include "DateHelp.h"
#include <stdio.h>

namespace suic
{

static const int MONTHDAYS[]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void DateHelp::DaysToYMD(int iDate, suic::Uint16& iYear, suic::Byte& iMonth, suic::Byte& iDay)
{
    int a = iDate + 32044 + 2415019;
    int b = (4 * a + 3) / 146097;
    int c = a-((146097 * b) / 4);
    int d = (4 * c + 3) / 1461;
    int e = c - (1461 * d) / 4;
    int m = (5 * e + 2) / 153;

    iDay = static_cast<int>(e - ((153 * m + 2) / 5) + 1);
    iMonth = static_cast<int>(m + 3 - 12 * (m / 10));
    iYear = static_cast<int>(100 * b + d - 4800 + (m / 10));
}

int DateHelp::YMDToDays(int iYear, int iMonth, int iDay)
{
    unsigned short a = static_cast<unsigned short>((14 - iMonth) / 12);
    unsigned short y = static_cast<unsigned short>(iYear + 4800 - a);
    unsigned short m = static_cast<unsigned short>(iMonth + 12 * a - 3);
    int iDate = iDay + ((153 * m + 2) / 5) + 365 * y 
        + (y / 4) - (y / 100) + (y / 400) - 32045 - 2415019;

    return iDate;
}

int DateHelp::MonthEndDay(int iYear, int iMonth)
{
    int nDay = 0;

    nDay = MONTHDAYS[iMonth - 1];

    if (iMonth == 2 && DateHelp::IsLeapYear(iYear))
    {
        nDay++;
    }

    return nDay;
}

bool DateHelp::IsLeapYear(int iYear)
{
    if ((iYear % 400) == 0 
        || ((iYear % 4) == 0 && (iYear % 100) != 0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

}
