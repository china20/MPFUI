// ���ڿƼ���Ȩ���� 2010-2020
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
//
// SHARPUI�����������ҵ��������ϵ�ܹ���������ϵͳ���¼�ϵͳ��
// ��ģ����Ŀǰc++�������һ���ص㣬����������ʽ�ʹ�����룬�ײ�
// ��������SSE2ָ����м��٣���������ͨ���ڻ��ǰ�͸�����������ܶ�
// �ǳ����㣬���ơ����Ķ����ӿڣ���ȫ�������������ҵʹ�á�
// 
// ����蹺����֧�ַ������ѯ����ϵ
// china0851@foxmail.com��QQ:386666951��
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
/// ʱ��ѡ��ؼ�
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

    // ѡ���ʱ������
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
