// 华勤科技版权所有 2010-2011
// 
// 文件名：Canvas.h
// 功  能：画布布局，绝对坐标。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UICANVAS_H_
#define _UICANVAS_H_

#include <Framework/Controls/Panel.h>

namespace suic
{

class SUICORE_API Canvas : public Panel
{
public:

    // 子控件在Canvas的左上角的x坐标（Integer类型，附加属性）
    static DpProperty* LeftProperty;
    static DpProperty* TopProperty;
    static DpProperty* RightProperty;
    static DpProperty* BottomProperty;

    static bool StaticInit();

    Canvas();
    virtual ~Canvas();

    RTTIOfClass(Canvas)

    static int GetLeft(Element* element);
    static int GetTop(Element* element);
    static int GetRight(Element* element);
    static int GetBottom(Element* element);

    static void SetLeft(Element* element, int val);
    static void SetTop(Element* element, int val);
    static void SetRight(Element* element, int val);
    static void SetBottom(Element* element, int val);

protected:

    Size OnMeasure(const Size& size);
    void OnArrange(const Size& arrangeSize);
};

}

#endif
