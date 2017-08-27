// 华勤科技版权所有 2008-2022
// 
// 文件名：HwndMouseFilter.h
// 功  能：定义核心库的键/值对象。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIHWNDMOUSEFILTER_H_
#define _UIHWNDMOUSEFILTER_H_

#include <System/Windows/Element.h>
#include <System/Windows/HwndObject.h>

namespace suic
{

class HwndMouseFilter
{
public:

    HwndMouseFilter(HwndObject* objPtr);
    ~HwndMouseFilter();

    bool OnFilterMessage(Object* sender, MessageParam* mp);
    void SetHwndObject(HwndObject* obj) { _hwndObj = obj; }

protected:

    void Process_WM_MOUSEMOVE(Element* rootElement, MessageParam* mp);
    void Process_WM_MOUSELEAVE(Element* rootElement, MessageParam* mp);

    void Process_WM_MOUSEWHEEL(Element* rootElement, MessageParam* mp);
    void Process_WM_CONTEXTMENU(Element* rootElement, MessageParam* mp);
    bool Process_WM_DROPFILES(Element* rootElement, MessageParam* mp);

    bool Process_WM_LBUTTONDOWN(Element* rootElement, MessageParam* mp);
    void Process_WM_LBUTTONUP(Element* rootElement, MessageParam* mp);

    bool Process_WM_MBUTTONDOWN(Element* rootElement, MessageParam* mp);
    bool Process_WM_MBUTTONUP(Element* rootElement, MessageParam* mp);

    bool Process_WM_RBUTTONDOWN(Element* rootElement, MessageParam* mp);
    void Process_WM_RBUTTONUP(Element* rootElement, MessageParam* mp);

    bool Process_WM_NCHITTEST(Element* rootElement, MessageParam* mp);
    bool Process_WM_SETCURSOR(Element* rootElement, MessageParam* mp);

    void HandleMouseCapture(Element* rootElement, MessageParam* mp);

protected:

    void SetMouseInputDevice();
    void HandMouseDown(Element* mouse, Point point, Int32 mouseType);

    void BeginMouseDown(int iDownFlag);
    void EndMouseDown(int iDownFlag);

protected:

    HwndObject* _hwndObj;
};

}

#endif
