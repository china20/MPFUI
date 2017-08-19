// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uimouse.h
// ��  �ܣ�ʵ�ֱ�׼�Ĵ�����ʽ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
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
