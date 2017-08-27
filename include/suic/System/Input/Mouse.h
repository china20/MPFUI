// 华勤科技版权所有 2008-2022
// 
// 文件名：uimouse.h
// 功  能：实现标准的窗口样式。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIMOUSE_H_
#define _UIMOUSE_H_

#include <System/Windows/Element.h>
#include <System/Input/MouseDevice.h>

namespace suic
{

class SUICORE_API Mouse
{
private:

    Mouse();
    ~Mouse();
    
public:

    static Point GetPosition(Element*);
    static MouseButton GetMouseButton();
    
    static void ClippingCursor(const Rect* rect);
    static Color ColorFromPoint(const Point* point);

    static Element* SetCaptured(Element*);
    static Element* GetCaptured();
    static Element* GetDirectlyOver();

    static void ChangeMouseOver(Element* newElem);
    static bool IsMouseCaptureWithin(Element*);
    static Element* HitTestElement(Element* elem, Point point);

    static void UpdateCursor();
};

};

#endif
