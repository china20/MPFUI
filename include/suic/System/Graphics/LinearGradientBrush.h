// 华勤科技版权所有 2008-2022
// 
// 文件名：GradientBrush.h
// 功  能：定义渐变刷子基类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UILINEARGRADIENTBRUSH_H_
#define _UILINEARGRADIENTBRUSH_H_

#include <System/Tools/Array.h>
#include <System/Graphics/GradientBrush.h>

namespace suic
{

struct LinearGradientInfo
{
    Float endX;
    Float endY;
    Float startX;
    Float startY;

    Array<Float, false> posinfo;
    Array<Uint32, false> colors;

    LinearGradientInfo();

    ~LinearGradientInfo()
    {
        Clear();
    }

    void Clear()
    {
        colors.Clear();
        posinfo.Clear();
    }

    void AddGradientStop(Color clr, Float radio);
};

class SUICORE_API LinearGradientBrush : public GradientBrush
{
public:

    RTTIOfClass(LinearGradientBrush)

    LinearGradientBrush();
    virtual ~LinearGradientBrush();

    LinearGradientBrush* Clone() const;

    void SetStartPoint(fPoint start);
    void SetEndPoint(fPoint end);

    fPoint GetStartPoint() const;
    fPoint GetEndPoint() const;

    bool Parse(const String& xml);
    bool Parse(IXamlNode* pNode);

protected:

    fPoint _endPoint;
    fPoint _startPoint;
};

inline fPoint LinearGradientBrush::GetStartPoint() const
{
    return _startPoint;
}

inline fPoint LinearGradientBrush::GetEndPoint() const
{
    return _endPoint;
}

}

#endif
