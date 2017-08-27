// 华勤科技版权所有 2008-2022
// 
// 文件名：Numeric.h
// 功  能：定义核心库数值操作。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _NUMERIC_H_
#define _NUMERIC_H_

#include <math.h>
#include <System/Types/Single.h>

namespace suic
{

template <typename T> 
T UITCast(const void* ptr) 
{
    union 
    {
        const void* src;
        T dst;
    }data;

    data.src = ptr;
    return data.dst;
}

struct SUICORE_API Numeric
{
    static Int32 NanInt32;

    static Int16 MinInt16;
    static Int16 MaxInt16;
    static Uint16 MaxUint16;

    static Int32 MinInt32;
    static Int32 MaxInt32;
    static Uint32 MaxUint32;

    static Int32 MeasureInt;

    static Uint64 MaxUint64;

public:

    static bool IsNanInt(Int32 x);
    static bool IsMeasureInt(Int32 x);
};

inline bool Numeric::IsNanInt(Int32 x)
{
    return x == NanInt32;
}

class SUICORE_API FloatUtil
{
public:

    static Float MinFloat;
    static Float MaxFloat;

    static Float MaxPrecision;
    static Float MinPrecision;

    static Float FNaN;
    static Float NegInfinity;
    static Float PosInfinity;
    static Float FloatClosedZero;

    static int Compare(Float val1, Float val2);
    static bool Equals(Float val1, Float val2);

    static int FloatToInt(Float val);
    static bool IsNaN(Float val);
    static bool IsZero(Float x);

    static bool IsInfinity(Float d);

    static bool IsNegInfinity(Float d);
    static bool IsPosInfinity(Float d);

    static bool AreClose(Float val1, Float val2);

    static bool GreaterThan(Float val1, Float val2);
    static bool GreaterThanOrClose(Float val1, Float val2);

    static bool LessThan(Float val1, Float val2);
    static bool LessThanOrClose(Float val1, Float val2);
};

inline bool FloatUtil::IsNegInfinity(Float d)
{
    return (NegInfinity == d);
}

inline bool FloatUtil::IsPosInfinity(Float d)
{
    return (PosInfinity == d);
}


}

#endif
