// 华勤科技版权所有 2008-2022
// 
// 文件名：DateTime.h
// 功  能：实现日期封装（年月日时分秒）。
// 
// 作  者：MPF开发组
// 时  间：2011-05-02
// 
// ============================================================================

#ifndef _UIDATETIME_H_
#define _UIDATETIME_H_

#include <System/Windows/Object.h>

namespace suic
{

class SUICORE_API DateTime
{
public:

    DateTime();
    DateTime(const DateTime& dt);
    DateTime(int dt);
    DateTime(int iYear, int iMonth, int iDay);
    DateTime(const String& strDate);

    static DateTime Today();

    bool IsValid() const;

    void Reset();
    void ResetToday();

    Byte GetWeekDay() const;
    Uint16 GetYearDay() const;

    Uint16 GetYear() const;
    Byte GetMonth() const;
    Byte GetDay() const;

    Byte GetHour() const;
    Byte GetMinute() const;
    Byte GetSecond() const;

    void SetYear(Uint16 val);
    void SetMonth(Byte val);
    void SetDayOfMonth(Byte val);

    bool IsQuarterEnd();
    bool IsMonthEnd();
    bool IsLeapYear() const;

    int GetMonthEndDay() const;
    int GetDayCount() const;

    void SetDate(int iYear, int iMonth, int iDay);
    void SetDate(int iDayCount);
    void SetDate(const DateTime& dt);
    void SetDate(const suic::String& strDate);

    void SetTime(Byte h, Byte m, Byte s);

    suic::String ToString();
    suic::String ToTime();
    suic::String ToLongDate();

    void SetMonthBegin();
    void SetMonthEnd();

    void SetQuarterBegin();
    void SetQuarterEnd();
    void SetPrevQuarter();
    void SetNextQuarter();

    void SetPrevSemiYear();
    void SetNextSemiYear();

    void SetYearBegin();
    void SetYearEnd();

    void AddDays(int iCount);
    void AddMonths(int iCount);
    void AddQuarters(int iCount);

    DateTime& operator+=(int iDayCount);
    DateTime& operator-=(int iDayCount);
    DateTime& operator=(const DateTime& dt);
    DateTime& operator=(int iDayCount);

    bool Equals(Object* Other);
    int Equal(const DateTime& dt);

private:

    Uint16 _iYear;
    Byte _iMonth;
    Byte _iDay;

    Byte _iHour;
    Byte _iMin;
    Byte _iSec;
};

class SUICORE_API ODateTime : public suic::Object
{
public:

    RTTIOfClass(ODateTime)

    ODateTime();
    ODateTime(const DateTime& dt);

    DateTime& GetDateTime();
    void SetDateTime(const DateTime& dt);

    bool Equals(Object* Other);

private:

    DateTime _dateTime;
};

}

#endif
