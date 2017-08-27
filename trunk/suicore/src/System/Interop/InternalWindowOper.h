
#include <Framework/Controls/Window.h>

/*--------内部使用头文件--------*/
#include <System/Windows/HwndObject.h>

namespace suic
{

HWND __HwndFromElement(Element* element);

void __InvalidateElementByObject(HwndObject* hwndObj, Rect* lprc);

void __SetParentByLongWithHwnd(HWND parent, HWND child);
void __SetParentByLong(Element*, Element*);

Point __GetCanvasLeftTop(Element* element);

Point __GetScreenPoint(Element* element);
Point __ToScreenPoint(Element* element, Point pt);
Point __GetCusorPoint(Element* element);
Point __GetCusorPointFromHwnd(HWND h);

Point __CalcElementAlignPoint(Element* element, int flag);

}

