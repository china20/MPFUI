/////////////////////////////////////////////////////////////////////////
// Double.cpp

#include <System/Types/Double.h>
#include <System/Types/Single.h>

namespace suic
{

float Single::MinValue = -3.402823E+38f;
float Single::Epsilon = 1.401298E-45f;
float Single::MaxValue = 3.402823E+38f;
static float s_float_zero = 0.0;
float Single::PositiveInfinity = (float)1.0 / s_float_zero;
float Single::NegativeInfinity = (float)-1.0 / s_float_zero;
float Single::NaN = (float)1.0 / s_float_zero;

Single::Single()
    : _value(0)
{
    ;
}

bool Single::IsInfinity(float f)
{
    return ((*(((int*) &f)) & 0x7fffffff) == 0x7f800000);
}

bool Single::IsPositiveInfinity(float f)
{
    return (*(((int*) &f)) == 0x7f800000);
}

bool Single::IsNegativeInfinity(float f)
{
    return (*(((int*) &f)) == -8388608);
}

bool Single::IsNaN(float f)
{
    return (f != f);
}

int Single::CompareTo(float value)
{
    if (_value < value)
    {
        return -1;
    }
    if (_value > value)
    {
        return 1;
    }
    if (_value != value)
    {
        if (!IsNaN(_value))
        {
            return 1;
        }
        if (!IsNaN(value))
        {
            return -1;
        }
    }
    return 0;
}

int Single::GetHashCode()
{
    float num = _value;

    if (num == 0.0)
    {
        return 0;
    }
    return *(((int*) &num));
}

String Single::ToString()
{
    return String();
}

float Single::Parse(String s)
{
    return 0;
}

double Double::MinValue = -1.7976931348623157E+308;
double Double::MaxValue = 1.7976931348623157E+308;
double Double::Epsilon = 4.94065645841247E-324;
static double s_double_zero = 0.0;
double Double::NegativeInfinity = (double)-1.0 / s_double_zero;
double Double::PositiveInfinity = (double)1.0 / s_double_zero;
double Double::NaN = (double)1.0 / s_double_zero;
double Double::NegativeZero = Double::Int64ToDouble(-9223372036854775808L);

Double::Double()
    : _value(0)
{
    ;
}

bool Double::IsInfinity(double d)
{
    return ((*(((Int64*)&d)) & 0x7fffffffffffffffL) == 0x7ff0000000000000L);
}

bool Double::IsPositiveInfinity(double d)
{
    return (d == PositiveInfinity);
}

bool Double::IsNegativeInfinity(double d)
{
    return (d == NegativeInfinity);
}

bool Double::IsNegative(double d)
{
    return ((*(((Int64*)&d)) & -9223372036854775808L) == -9223372036854775808L);
}

bool Double::IsNaN(double d)
{
    return (d != d);
}

int Double::CompareTo(double value)
{
    if (_value < value)
    {
        return -1;
    }
    if (_value > value)
    {
        return 1;
    }
    if (_value != value)
    {
        if (!IsNaN(_value))
        {
            return 1;
        }
        if (!IsNaN(value))
        {
            return -1;
        }
    }
    return 0;
}

bool Double::Equals(double obj)
{
    return ((obj == _value) || (IsNaN(obj) && IsNaN(_value)));
}

int Double::GetHashCode()
{
    double num = _value;
    if (num == 0.0)
    {
        return 0;
    }
    Int64 num2 = *((Int64*)&num);
    return (((int) num2) ^ ((int)(num2 >> 0x20)));
}

String Double::ToString()
{
    return String();
}

double Double::Int64ToDouble(Int64 value)
{
    return *(((double*)&value));
}

}
