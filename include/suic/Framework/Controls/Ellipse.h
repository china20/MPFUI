// 华勤科技版权所有 2008-2022
// 
// 文件名：Ellipse.h
// 功  能：实现椭圆控件。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UIELLIPSE_H_
#define _UIELLIPSE_H_

#include <Framework/Controls/Shape.h>

namespace suic
{

class SUICORE_API Ellipse : public Shape
{
public:

    static bool StaticInit();

    Ellipse();
    virtual ~Ellipse();

    RTTIOfClass(Ellipse)

protected:

    Size OnMeasure(const Size& size);
    void OnRender(Drawing * drawing);
};

}

#endif
