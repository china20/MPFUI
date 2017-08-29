// 华勤科技版权所有 2010-2012
// 
// 文件名：uimath.h
// 功  能：算术帮助类。
// 
// 作  者：MPF开发组
// 时  间：2010-05-01
// 
// ============================================================================

#ifndef _UIMATH_H_
#define _UIMATH_H_

#include <System/Types/Numeric.h>
#include <math.h>

#define UIMIN(x, y) (x < y ? x : y)
#define UIMAX(x, y) (x > y ? x : y)

#define UFEQUAL(x, y) (suic::FloatUtil::Equals(x, y))
#define UFMIN(x, y) (suic::FloatUtil::LessThanOrClose(x, y) ? x : y)
#define UFMAX(x, y) (suic::FloatUtil::LessThanOrClose(y, x) ? x : y)

namespace suic
{

class Math
{
public:

    static int Round(Float v)
    {
        return (int)(v + 0.5);
    }

    static Float Max(Float f1, Float f2)
    {
        if (f1 > f2 || f1 != f1)
        {
            return f1;
        }
        else
        {
            return f2;
        }

    }

    static Float Min(Float f1, Float f2)
    {
        if (f1 < f2 || f1 != f1)
        {
            return f1;
        }
        else
        {
            return f2;
        }
    }

	static Float Abs(Float v)
	{
		if (v < 0)
		{
			return -v;
		}
		else
		{
			return v;
		}
	}

	static long Abs(long v)
	{
		if (v < 0)
		{
			return -v;
		}
		else
		{
			return v;
		}
	}

    static double Log(double a, double newBase);

protected:

    Math();
};

}

#endif
