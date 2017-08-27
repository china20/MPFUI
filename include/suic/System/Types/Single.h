// 华勤科技版权所有 2008-2022
// 
// 文件名：Single.h
// 功  能：定义核心库的Single类型。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UISINGLE_H_
#define _UISINGLE_H_

#include <System/Types/String.h>

namespace suic
{

struct SUICORE_API Single
{
    static float MinValue;
    static float Epsilon;
    static float MaxValue;
    static float PositiveInfinity;
    static float NegativeInfinity;
    static float NaN;

    static bool IsInfinity(float f);
    static bool IsPositiveInfinity(float f);
    static bool IsNegativeInfinity(float f);
    static bool IsNaN(float f);
    static float Parse(String s);

    Single();

    int CompareTo(float value);
    int GetHashCode();
    String ToString();

    operator float() const
    {
        return _value;
    }

    float ToFloat() const
    {
        return _value;
    }

private:

    float _value;
};

}

#endif
