// 华勤科技版权所有 2008-2022
// 
// 文件名：RadialGradientBrush.h
// 功  能：定义径向渐变刷子基类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIRADIALGRADIENTBRUSH_H_
#define _UIRADIALGRADIENTBRUSH_H_

#include <System/Tools/Array.h>
#include <System/Graphics/GradientBrush.h>

namespace suic
{

class SUICORE_API RadialGradientBrush : public GradientBrush
{
public:

    RTTIOfClass(RadialGradientBrush)

    RadialGradientBrush();
    ~RadialGradientBrush();

    RadialGradientBrush* Clone() const;

    void SetOrigin(Float x, Float y);
    fPoint GetOrigin() const;

    Float GetRadiusX() const;
    Float GetRadiusY() const;

    void SetRadiusXY(Float x, Float y);

    bool Parse(const String& xml);
    bool Parse(IXamlNode* pNode);

protected:

    fPoint _origin;
    Float _radiusX;
    Float _radiusY;
};

inline fPoint RadialGradientBrush::GetOrigin() const
{
    return _origin;
}

inline Float RadialGradientBrush::GetRadiusX() const
{
    return _radiusX;
}

inline Float RadialGradientBrush::GetRadiusY() const
{
    return _radiusY;
}

}

#endif
