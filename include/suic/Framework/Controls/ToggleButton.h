// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ToggleButton.h
// ��  �ܣ���ť����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UITOGGLEBUTTON_H_
#define _UITOGGLEBUTTON_H_

#include <Framework/Controls/ButtonBase.h>

namespace suic
{

/// <summary>
/// ToggleButton��
/// </summary>
class SUICORE_API ToggleButton : public ButtonBase
{
public:

    static RoutedEvent* CheckedEvent;
    static RoutedEvent* UncheckedEvent;
    static RoutedEvent* IndeterminateEvent;

    // �Ƿ���̬��Boolean���ͣ�
    static DpProperty* IsThreeStateProperty;
    // �Ƿ�ѡ�����ԣ�Boolean���ͣ�
    static DpProperty* IsCheckedProperty;

    static bool StaticInit();

    static void OnIsCheckedChanged(DpObject* d, DpPropChangedEventArg* e);

    ToggleButton();
    virtual ~ToggleButton();

    RTTIOfClass(ToggleButton)

    Boolean* GetChecked();
    void SetChecked(Boolean* val);

    bool IsIndeterminate();
    bool IsChecked();

    bool IsThreeState();
    void SetIsThreeState(bool val);

    void AddCheckedEvent(RoutedEventHandler* h);
    void RemoveCheckedEvent(RoutedEventHandler* h);

    void AddUncheckedEvent(RoutedEventHandler* h);
    void RemoveUncheckedEvent(RoutedEventHandler* h);

    void AddIndeterminateEvent(RoutedEventHandler* h);
    void RemoveIndeterminateEvent(RoutedEventHandler* h);

protected:

    void OnMouseLeftButtonDown(MouseButtonEventArg* e);

    virtual void OnChecked(RoutedEventArg* e);
    virtual void OnUnchecked(RoutedEventArg* e);
    virtual void OnIndeterminate(RoutedEventArg* e);

    virtual void OnToggle();

    void OnClick();
};

inline bool ToggleButton::IsThreeState()
{
    return GetValue(IsThreeStateProperty)->ToInt() != 0;
}

inline void ToggleButton::SetIsThreeState(bool val)
{
    SetValue(IsThreeStateProperty, BOOLTOBOOLEAN(val));
}

}

#endif
