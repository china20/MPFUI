// 华勤科技版权所有 2008-2022
// 
// 文件名：HwndKeyboardFilter.h
// 功  能：定义核心库的键/值对象。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIHWNDKEYBOARDFILTER_H_
#define _UIHWNDKEYBOARDFILTER_H_

#include <System/Windows/HwndObject.h>
#include <Framework/Controls/Control.h>
#include <Framework/Controls/Panel.h>

namespace suic
{

class HwndKeyboardFilter;
class HwndKeyboardFilter
{
public:

    HwndKeyboardFilter(HwndObject* objPtr);
    ~HwndKeyboardFilter();

    bool OnFilterMessage(Object* sender, MessageParam* mp);
    void SetHwndObject(HwndObject* obj) { _hwndObj = obj; }

protected:

    Element* GetKeyboardRoot(Element* rootElement);
    Element* GetFocusedElement(Element* rootElement);

    bool Process_WM_KEYDOWN(Element* rootElement, MessageParam* mp);
    bool Process_WM_KEYUP(Element* rootElement, MessageParam* mp);

    void Process_WM_KILLFOCUS(Element* rootElement, MessageParam* mp);
    void Process_WM_SETFOCUS(Element* rootElement, MessageParam* mp);

    void Process_WM_HOTKEY(Element* rootElement, MessageParam* mp);

protected:

    HwndObject* _hwndObj;
    bool _calledFocus;
};

}

#endif