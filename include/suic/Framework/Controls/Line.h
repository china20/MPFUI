// 华勤科技版权所有 2008-2022
// 
// 文件名：Line.h
// 功  能：实现线条。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UILINE_H_
#define _UILINE_H_

#include <Framework/Controls/Shape.h>

namespace suic
{

class SUICORE_API Line : public Shape
{
public:

    static DpProperty* X1Property;
    static DpProperty* Y1Property;
    static DpProperty* X2Property;
    static DpProperty* Y2Property;

    static void OnX1PropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnY1PropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnX2PropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnY2PropChanged(DpObject* d, DpPropChangedEventArg* e);

    static bool StaticInit();

    Line();
    virtual ~Line();

    RTTIOfClass(Line)

    Float GetX1();
    void SetX1(Float val);

    Float GetY1();
    void SetY1(Float val);

    Float GetX2();
    void SetX2(Float val);

    Float GetY2();
    void SetY2(Float val);

protected:

    Size OnMeasure(const Size& size);
    void OnArrange(const Size& arrangeSize);

    void OnRender(Drawing * drawing);

private:

    void ResetLineInfo();
};

}

#endif
