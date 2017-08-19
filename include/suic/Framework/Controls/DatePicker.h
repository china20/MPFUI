// 华勤科技版权所有 2010-2020
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
//
// SHARPUI界面引擎采用业界成熟的体系架构，其属性系统、事件系统以
// 及模版在目前c++界面库是一大特点，真正做到样式和代码分离，底层
// 更采用了SSE2指令进行加速，不论在普通窗口还是半透明窗口下性能都
// 非常优秀，完善、灵活的动画接口，完全可以满足各种商业使用。
// 
// 如果需购买技术支持服务或咨询请联系
// china0851@foxmail.com；QQ:386666951。
// ====================================================================

#ifndef _UIDATEPICKER_H_
#define _UIDATEPICKER_H_

#include <System/Types/DateTime.h>

#include <Framework/Controls/Popup.h>
#include <Framework/Controls/Button.h>
#include <Framework/Controls/Calendar.h>
#include <Framework/Controls/TextEditor.h>

namespace suic
{

class CalendarDownButton : public Button
{
public:

    RTTIOfClass(CalendarDownButton)
};

/// <summary>
/// 时间选择控件
/// </summary>
class SUICORE_API DatePicker : public Control
{
public:

    DatePicker();
    virtual ~DatePicker();

    RTTIOfClass(DatePicker)

    static String ElementRoot;
    static String ElementTextBox;
    static String ElementButton;
    static String ElementPopup;

    static RoutedEvent* SelectedDateChangedEvent;

    // 选择的时间属性
    static DpProperty* TextProperty;
    static DpProperty* DisplayDateProperty;
    static DpProperty* SelectedDateProperty;
    static DpProperty* CalendarStyleProperty;
    static DpProperty* IsDropDownOpenProperty;

    static void OnTextPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnSelectedDatePropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIsDropDownOpenChanged(DpObject* d, DpPropChangedEventArg* e);

    static bool StaticInit();

    String ToString();
    String GetText();

    void AddDays(int iCount);
    void UpdateDate();

    DateTime GetDate();
    void SetDate(const DateTime& date);

    Style* GetCalendarStyle();
    void SetCalendarStyle(Style* val);

    bool IsDropDownOpen();
    void SetIsDropDownOpen(bool val);

    void AddSelectedDateChanged(RoutedEventHandler* h);
    void RemoveSelectedDateChanged(RoutedEventHandler* h);

public:

    void OnDayClick(Object* sender, EventArg* e);
    void OnCalendarLoaded(Element* sender, LoadedEventArg* e);
    bool OnMessageFilter(Object* sender, MessageParam* mp, bool& interrupt);
    void OnPopupOpened(Object* sender, EventArg* e);
    void OnPopupClosed(Object* sender, EventArg* e);

protected:

    void ClosePopup();
    void OnApplyTemplate();
    void OnInitialized(EventArg* e);

    void OnMouseMove(MouseButtonEventArg* e);

    void OnKeyDown(KeyboardEventArg* e);
    void OnTextInput(KeyboardEventArg* e);

    void OnPreviewMouseLeftButtonUp(MouseButtonEventArg* e);
    void OnPreviewMouseLeftButtonDown(MouseButtonEventArg* e);

    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);

protected:

    ODateTime* _date;

    Popup* _popup;
    Button* _button;
    Calendar* _calendar;
    TextEditor* _textInput;
};

}

#endif
