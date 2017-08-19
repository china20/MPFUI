// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����FocusManager.h
// ��  �ܣ�Ԫ�ؽ������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-05-02
// 
// ============================================================================

#ifndef _UIFOCUSMANAGER_H_
#define _UIFOCUSMANAGER_H_

#include <System/Input/InputDevice.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

class FocusManager : public Object
{
public:

    static DpProperty* IsFocusScopeProperty;
    static DpProperty* FocusedElementProperty;

    static bool StaticInit();

    static InputDevice* GetCurrentInputDevice();
    static void SetCurrentInputDevice(InputDevice* inputDevice);

    static bool GetIsFocusScope(DpObject* d);
    static DpObject* GetFocusScope(DpObject* d);
    static Element* GetFocusedElement(DpObject* d);
    static Element* GetFocusedElement(DpObject* d, bool validate);

    static void SetFocusedElement(DpObject* element, Element* value);
    static void SetIsFocusScope(DpObject* element, bool value);

    static void OnFocusedElementChanged(DpObject* d, DpPropChangedEventArg* e);
   
private:

    FocusManager();

    static InputDevice* _inputDevice;
};

}

#endif
