// 华勤科技版权所有 2008-2022
// 
// 文件名：Keyboard.h
// 功  能：实现标准的窗口样式。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIKEYBOARD_H_
#define _UIKEYBOARD_H_

#include <System/Windows/Object.h>

namespace suic
{

class Element;

class SUICORE_API Keyboard
{
private:

    Keyboard();
    ~Keyboard();

public:

    static Element* GetFocusedElement();
    static Element* Focus(Element* elem);

    static bool IsFocusable(Element* elem);
    static bool IsFocusedWithin(Element* elem);

    static Uint16 GetModifiers();

    static bool IsModifierKey(int val);

    static bool IsKeyDown(int iKey);

    static bool IsAltDown();
    static bool IsShiftDown();
    static bool IsControlDown();
    static bool IsWinDown();

private:

    static bool _pending;
    static WeakRef _focus;
};

}

#endif
