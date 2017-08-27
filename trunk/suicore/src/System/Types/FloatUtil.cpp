/////////////////////////////////////////////////////////////////////////
// uiobject.cpp

#include <System/Tools/Memory.h>
#include <System/Tools/SSE.h>
#include <System/Windows/Object.h>
#include <System/Windows/CoreTool.h>

#include <System/Types/Integer.h>
#include <System/Types/Numeric.h>

#include <Float.h>

namespace suic
{
const unsigned int G_C_NOTANUMBER = 0x7FFFFFFF;
const unsigned int G_C_POSITIVEINFINITY = 0x7F800000;
const unsigned int G_C_NEGATIVEINFINITY = 0xFF800000;

//----------------------------------------------------------

Int32 Numeric::NanInt32 = -1;
Int16 Numeric::MaxInt16 = ((1 << 15) - 15);
Int16 Numeric::MinInt16 = Numeric::MaxInt16 * -1;
Uint16 Numeric::MaxUint16 = -1;
Int32 Numeric::MaxInt32 = ((Uint32)(1 << 31) - 1);
Int32 Numeric::MinInt32 = Numeric::MaxInt32 * -1;
Uint32 Numeric::MaxUint32 = -1;
Uint64 Numeric::MaxUint64 = -1;

Int32 Numeric::MeasureInt = 200000000;

//----------------------------------------------------------

Float FloatUtil::FloatClosedZero = 1.192093E-07f;
//Float FloatUtil::FloatClosedZero = (1.0 / (1 << 12));
Float FloatUtil::NegInfinity = (*UITCast<const Float*>(&G_C_NEGATIVEINFINITY));
Float FloatUtil::PosInfinity = (*UITCast<const Float*>(&G_C_POSITIVEINFINITY));
Float FloatUtil::FNaN = (*UITCast<const Float*>(&G_C_NOTANUMBER));

Float FloatUtil::MaxPrecision = 1.677722E+07f;
Float FloatUtil::MinPrecision = 1 / FloatUtil::MaxPrecision;

Float FloatUtil::MaxFloat = (3.402823466e+38f);
Float FloatUtil::MinFloat = -FloatUtil::MaxFloat;

bool Numeric::IsMeasureInt(Int32 x)
{
    return (x - MeasureInt) >= -100;
}

bool FloatUtil::IsInfinity(Float d)
{
    //return ((*(((int*) &d)) & 0x7fffffff) == 0x7ff00000);
    return _finite(d) == 0;
}

bool FloatUtil::Equals(Float val1, Float val2)
{
    return ((val2 == val1) || (IsNaN(val2) && IsNaN(val1)));
}

int FloatUtil::Compare(Float val1, Float val2)
{
    if (val1 < val2)
    {
        return -1;
    }
    if (val1 > val2)
    {
        return 1;
    }
    if (val1 != val2)
    {
        if (!IsNaN(val2))
        {
            return 1;
        }
        if (!IsNaN(val2))
        {
            return -1;
        }
    }
    return 0;
}

bool FloatUtil::IsNaN(Float val)
{
    return (val != val);
}

bool FloatUtil::IsZero(Float x)
{
    return fabs(x) <= FloatClosedZero;
}

bool FloatUtil::AreClose(Float value1, Float value2)
{
    return fabs(value1-value2) <= FloatClosedZero;
}

bool FloatUtil::LessThan(Float value1, Float value2)
{
    return ((value1 < value2) && !AreClose(value1, value2));
}

bool FloatUtil::LessThanOrClose(Float value1, Float value2)
{
    if (value1 >= value2)
    {
        return AreClose(value1, value2);
    }
    return true;
}

bool FloatUtil::GreaterThan(Float value1, Float value2)
{
    return ((value1 > value2) && !AreClose(value1, value2));
}

bool FloatUtil::GreaterThanOrClose(Float value1, Float value2)
{
    if (value1 <= value2)
    {
        return AreClose(value1, value2);
    }
    return true;
}

int FloatUtil::FloatToInt(Float val)
{
    if (0.0 >= val)
    {
        return (int) (val - 0.5);
    }
    return (int) (val + 0.5);
}

}
