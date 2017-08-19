// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����SystemParameters.h
// ��  �ܣ�����ϵͳ������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UISYSTEMPARAMETERS_H_
#define _UISYSTEMPARAMETERS_H_

#include <System/Types/StringCore.h>

namespace suic
{

class Matrix;

struct SUICORE_API SystemParameters
{
    static int MessageFontSize;
    static String MessageFontName;

    static Float DpiX;
    static Float DpiY;

    static Float DpiToX;
    static Float DpiToY;

    static Float DpiFromX;
    static Float DpiFromY;

    static float RoundDpiValue(float val, float dpiScale);

    static Rect TransformToDevice(Rect rect);
    static Rect TransformFromDevice(Rect rect);
    static fRect TransformToDevice(fRect rect);
    static fRect TransformFromDevice(fRect rect);

    static Size TransformToDevice(Size size);

    static Float TransformXToDevice(Float x);
    static Float TransformYToDevice(Float y);
    static Float TransformXFromDevice(Float x);
    static Float TransformYFromDevice(Float y);

    static bool isNeedTransform();
    static suic::Matrix& GetTransform();
};

}

#endif
