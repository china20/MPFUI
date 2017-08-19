// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Double.h
// ��  �ܣ�������Ŀ��Double���͡�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIDOUBLE_H_
#define _UIDOUBLE_H_

#include <System/Types/String.h>

namespace suic
{

struct SUICORE_API Double
{
    static double MinValue;
    static double MaxValue;
    static double Epsilon;
    static double NegativeInfinity;
    static double PositiveInfinity;
    static double NaN;
    static double NegativeZero;

    static bool IsInfinity(double d);
    static bool IsPositiveInfinity(double d);
    static bool IsNegativeInfinity(double d);
    static bool IsNegative(double d);
    static bool IsNaN(double d);
    static double Int64ToDouble(Int64 value);

    Double();

    int CompareTo(double value);

    bool Equals(double obj);
    int GetHashCode();
    String ToString();

    operator double() const
    {
        return _value;
    }

    double ToDouble() const
    {
        return _value;
    }
    
private:

    double _value;
};

}

#endif

