// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// DateTime.cpp


#include <stdio.h>
#include <time.h>

#include "DateHelp.h"

#include <System/Tools/Array.h>
#include <System/Types/DateTime.h>

namespace suic
{

static const int PQUARTERSDAYS[]={0, 12, 12, 31, 3, 3, 30, 6, 6, 30, 9, 9, 31};
static const int NQUARTERSDAYS[]={0, 3, 3, 31, 6, 6, 30, 9, 9, 30, 12, 12, 31};

static const int PSEMIYEARDAYS[]={0, 12, 12, 12, 12, 12, 30, 6, 6, 6, 6, 6, 31};
static const int NSEMIYEARDAYS[]={0, 6, 6, 6, 6, 6, 30, 12, 12, 12, 12, 12, 31};

ImplementRTTIOfClass(ODateTime, suic::Object)

ODateTime::ODateTime()
{

}

ODateTime::ODateTime(const DateTime& dt)
{
    _dateTime = dt;
}

DateTime& ODateTime::GetDateTime()
{
    return _dateTime;
}

void ODateTime::SetDateTime(const DateTime& dt)
{
    _dateTime = dt;
}

bool ODateTime::Equals(Object* Other)
{
    if (Other == NULL || Other->GetRTTIType() != ODateTime::RTTIType())
    {
        return false;
    }
    else
    {
        return _dateTime.Equal(((ODateTime*)Other)->GetDateTime()) == 0;
    }
}

DateTime::DateTime()
{
    ResetToday();
}

DateTime::DateTime(const DateTime & dt)
{
    _iYear = dt._iYear;
    _iMonth = dt._iMonth;
    _iDay = dt._iDay;
    _iHour = dt._iHour;
    _iMin = dt._iMin;
    _iSec = dt._iSec;
}

DateTime::DateTime(int dt)
{
    SetDate(dt);
}

DateTime::DateTime(int iYear, int iMonth, int iDay)
    : _iYear(1900)
    , _iMonth(1)
    , _iDay(1)
{
    SetDate(iYear, iMonth, iDay);
}

DateTime::DateTime(const String & strDate)
{
    SetDate(strDate);
}

bool DateTime::IsValid() const
{
    if (_iYear >= 1900 && _iMonth >= 1 
        && _iMonth <= 12)
    {
        if (_iDay <= GetMonthEndDay()
            && _iHour >= 0 && _iHour <= 23
            && _iMin >= 0 && _iMin <= 59
            && _iSec >= 0 && _iSec <= 59)
        {
            return true;
        }
    }
    return false;
}

DateTime DateTime::Today()
{
    return DateTime();
}

void DateTime::Reset()
{

}

void DateTime::ResetToday()
{
    struct tm Tm = {0};
    time_t tCount = time(NULL);
    localtime_s(&Tm, &tCount);

    _iYear = Tm.tm_year + 1900;
    _iMonth = Tm.tm_mon + 1;
    _iDay = Tm.tm_mday;
    _iHour = Tm.tm_hour;
    _iMin = Tm.tm_min;
    _iSec = Tm.tm_sec;
}

void DateTime::SetYear(Uint16 val)
{
    if (val >= 1900)
    {
        _iYear = val;
    }
}

void DateTime::SetMonth(Byte val)
{
    if (val >= 1 && val <= 12)
    {
        _iMonth = val;
    }
}

void DateTime::SetDayOfMonth(Byte val)
{
    int iMonEnd = DateHelp::MonthEndDay(GetYear(), GetMonth());
    if (val >= 1 && val <= iMonEnd)
    {
        _iDay = val;
    }
}

Byte DateTime::GetWeekDay() const
{
    struct tm Tm = {0};

    Tm.tm_year = _iYear - 1900;
    Tm.tm_mon = _iMonth - 1;
    Tm.tm_mday = _iDay;

    time_t tCount = mktime(&Tm);
    localtime_s(&Tm, &tCount);

    return Tm.tm_wday;
}

Uint16 DateTime::GetYearDay() const
{
    struct tm Tm = {0};

    Tm.tm_year = _iYear - 1900;
    Tm.tm_mon = _iMonth - 1;
    Tm.tm_mday = _iDay;

    time_t tCount = mktime(&Tm);
    localtime_s(&Tm, &tCount);

    int iDays = DateHelp::YMDToDays(_iYear, _iMonth, _iDay);
    int iBegDays = DateHelp::YMDToDays(_iYear, 1, 1);

    return (iDays - iBegDays);
}

int DateTime::GetDayCount() const
{
    return DateHelp::YMDToDays(_iYear, _iMonth, _iDay);
}

int DateTime::GetMonthEndDay() const
{
    return DateHelp::MonthEndDay(_iYear, _iMonth);
}

void DateTime::SetDate(const String& strDate)
{
    ResetToday();

    StringArray vecms;
    StringArray vec;

    vecms.SplitString(strDate, _T(" "));

    if (vecms.Length() == 0)
    {
        vec.SplitString(strDate, _T("-"));
    }
    else if (vecms.Length() >= 2)
    {
        vec.SplitString(vecms[0], _T("-"));
        StringArray vecs;
        vecs.SplitString(vecms[1], _T(":"));

        if (vecs.Length() == 3)
        {
            _iHour = vecs[0].ToInt();
            _iMin = vecs[1].ToInt();
            _iSec = vecs[2].ToInt();
        }
    }

    if (vec.Length() == 3)
    {
        SetDate(vec[0].ToInt(), vec[1].ToInt(), vec[2].ToInt());
    }
}

void DateTime::SetDate(const DateTime& dt)
{
    _iYear = dt._iYear;
    _iMonth = dt._iMonth;
    _iDay = dt._iDay;
}

void DateTime::SetDate(int iYear, int iMonth, int iDay)
{
    if (iYear >= 1900 && iMonth >= 1 
        && iMonth <= 12 
        && iDay <= DateHelp::MonthEndDay(iYear, iMonth))
    {
        _iYear = iYear;
        _iMonth = iMonth;
        _iDay = iDay;
    }
}

void DateTime::SetDate(int iDayCount)
{
    DateHelp::DaysToYMD(iDayCount, _iYear, _iMonth, _iDay);
}

void DateTime::SetTime(Byte h, Byte m, Byte s)
{
    _iHour = h;
    _iMin = m;
    _iSec = s;
}

String DateTime::ToString()
{
    String strFmt;
    strFmt.Format(_T("%04d-%02d-%02d"), _iYear, _iMonth, _iDay);
    return strFmt;
}

suic::String DateTime::ToTime()
{
    String strFmt;
    strFmt.Format(_T("%02d:%02d:%02d"), _iHour, _iMin, _iSec);
    return strFmt;
}

String DateTime::ToLongDate()
{
    String strFmt;

    strFmt.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), 
        _iYear, _iMonth, _iDay, _iHour, _iMin, _iSec);
    return strFmt;
}

Uint16 DateTime::GetYear() const
{
    return _iYear;
}

Byte DateTime::GetMonth() const
{
    return _iMonth;
}

Byte DateTime::GetDay() const
{
    return _iDay;
}

Byte DateTime::GetHour() const
{
    return _iHour;
}

Byte DateTime::GetMinute() const
{
    return _iMin;
}

Byte DateTime::GetSecond() const
{
    return _iSec;
}

bool DateTime::IsQuarterEnd()
{
    if ((_iMonth % 3) ==0 && IsMonthEnd())
    {
        return true;
    }
    else 
    {
        return false;
    }
}

bool DateTime::IsMonthEnd()
{
    return (_iDay == GetMonthEndDay());
}

void DateTime::SetMonthBegin()
{
    _iDay = 1;
}

void DateTime::SetMonthEnd()
{
    _iDay = GetMonthEndDay();
}

void DateTime::SetQuarterBegin()
{
    _iDay = 31;
    _iMonth = 3;
}

void DateTime::SetQuarterEnd()
{
    _iDay = 31;
    _iMonth = 12;
}

void DateTime::SetPrevQuarter()
{
    int iPrev = PQUARTERSDAYS[_iMonth];

    if (iPrev < 28)
    {
        _iMonth = iPrev;
        _iDay = PQUARTERSDAYS[iPrev];
        _iYear -= (iPrev / 12);
    }
    else
    {
        if (_iDay != iPrev)
        {
            _iMonth -= 1;
            SetPrevQuarter();
        }
        else
        {
            _iDay = iPrev;
        }
    }
}

void DateTime::SetNextQuarter()
{
    int iNext = NQUARTERSDAYS[_iMonth];

    if (iNext < 28)
    {
        _iMonth = iNext;
        _iDay = NQUARTERSDAYS[iNext];
    }
    else
    {
        _iDay = iNext;
    }
}

void DateTime::SetPrevSemiYear()
{
    int iPrev = PSEMIYEARDAYS[_iMonth];

    if (iPrev < 28)
    {
        _iMonth = iPrev;
        _iDay = PSEMIYEARDAYS[iPrev];
        _iYear -= (iPrev / 12);
    }
    else
    {
        if (_iDay != iPrev)
        {
            _iMonth -= 1;
            SetPrevSemiYear();
        }
        else
        {
            _iDay = iPrev;
        }
    }
}

void DateTime::SetNextSemiYear()
{
    int iNext = NSEMIYEARDAYS[_iMonth];

    if (iNext < 28)
    {
        _iMonth = iNext;
        _iDay = NSEMIYEARDAYS[iNext];
    }
    else
    {
        _iDay = iNext;
    }
}

void DateTime::SetYearBegin()
{
    _iMonth = 1;
    _iDay = 1;
}

void DateTime::SetYearEnd()
{
    _iMonth = 12;
    _iDay = 31;
}

void DateTime::AddDays(int iCount)
{
    SetDate(GetDayCount() + iCount);
}

void DateTime::AddMonths(int iCount)
{
    iCount += _iMonth;

    if (iCount <= 0)
    {
        _iYear += iCount / 12 - 1;
        _iMonth = (iCount % 12) + 12;
    }
    else
    {
        iCount -= 1;
        _iMonth = (iCount % 12) + 1;
        _iYear += iCount / 12;
    }

    if (GetMonthEndDay() < _iDay)
    {
        _iDay = GetMonthEndDay();
    }
}

void DateTime::AddQuarters(int iCount)
{
    AddMonths(3 * iCount);
}

DateTime& DateTime::operator+=(int iDayCount)
{
    iDayCount += DateHelp::YMDToDays(_iYear, _iMonth, _iDay);
    SetDate(iDayCount);

    return (*this);
}

DateTime& DateTime::operator-=(int iDayCount)
{
    iDayCount = DateHelp::YMDToDays(_iYear, _iMonth, _iDay) - iDayCount;
    SetDate(iDayCount);

    return (*this);
}

DateTime& DateTime::operator=(const DateTime & dt)
{
    if (&dt != this)
    {
        _iYear = dt._iYear;
        _iMonth = dt._iMonth;
        _iDay = dt._iDay;
    }

    return (*this);
}

DateTime& DateTime::operator=(int iDayCount)
{
    SetDate(iDayCount);
    return (*this);
}

int DateTime::Equal(const DateTime& dt)
{
#define COMPAREVAL(L,R) if(L<(R)){return -1;}else if(L>R){return 1;}

    COMPAREVAL(_iYear,dt._iYear)
    COMPAREVAL(_iMonth,dt._iMonth)
    COMPAREVAL(_iDay,dt._iDay)

    return 0;
}

}
