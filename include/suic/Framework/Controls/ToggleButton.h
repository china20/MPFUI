// 华勤科技版权所有 2008-2022
// 
// 文件名：ToggleButton.h
// 功  能：按钮基类
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UITOGGLEBUTTON_H_
#define _UITOGGLEBUTTON_H_

#include <Framework/Controls/ButtonBase.h>

namespace suic
{

/// <summary>
/// ToggleButton。
/// </summary>
class SUICORE_API ToggleButton : public ButtonBase
{
public:

    static RoutedEvent* CheckedEvent;
    static RoutedEvent* UncheckedEvent;
    static RoutedEvent* IndeterminateEvent;

    // 是否三态（Boolean类型）
    static DpProperty* IsThreeStateProperty;
    // 是否选中属性（Boolean类型）
    static DpProperty* IsCheckedProperty;

    static void StaticInit();

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
