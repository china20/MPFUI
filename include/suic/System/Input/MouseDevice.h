// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����UIMouseDriver.h
// ��  �ܣ�������Ŀ������¼������ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIMOUSEDRIVER_H_
#define _UIMOUSEDRIVER_H_

#include <System/Windows/Element.h>
#include <System/Input/InputDevice.h>

namespace suic
{

class SUICORE_API MouseDevice : public InputDevice
{
public:

    static MouseDevice* Current();

    //---------------------------------------------------

    static Element* GetRawMouseOver();
    static void SetRawMouseOver(Element* val);

    static Element* GetMouseOver();
    static void SetMouseOver(Element* val);
    static void ClearMouseOver(bool bCheck);

    static Element* GetRawMouseDown(Int32 mouseType);
    static void SetRawMouseDown(Element* val, Int32 mouseType);

    static Element* GetMouseDown(Int32 mouseType);
    static void SetMouseDown(Element* val, Int32 mouseType);

    static Element* GetMouseCapture();
    static void SetMouseCapture(Element* val);

    static MouseButtonState GetLeftButton();
    static MouseButtonState GetRightButton();
    static MouseButtonState GetMiddleButton();
    static MouseButtonState GetXButton1();
    static MouseButtonState GetXButton2();

    static MouseButtonState GetButtonState(int mouseButton);

    Point GetPrevClick();
    void SetPrevClick(Point val);

    Point GetLastClick();
    void SetLastClick(Point val);

    Uint32 GetClickCount();
    void SetClickCount(Uint32 val);

    Uint32 GetLastClickTime();
    void SetLastClickTime(Uint32 val);

    Int32 GetLastButton();
    void SetLastButton(Int32 val);

    Int32 GetDoubleDeltaTime();

    VisualHost* GetVisualHost();
    void SetVisualHost(VisualHost* pHost);

    void Clear();

private:

    Point _prevClick;
    Point _lastClick;

    // ��굥������
    Uint32 _clickCount;
    // ���һ�����ѹ�´���
    Uint32 _lastClickTime;
    // ���һ�����ѹ�µļ�����
    Int32 _lastButton;
    Int32 _doubleClickTime;
    VisualHost* _visualHost;

private:

    MouseDevice();
};

};

#endif
