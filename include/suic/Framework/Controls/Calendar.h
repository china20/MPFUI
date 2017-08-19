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

#ifndef _UICALENDAR_H_
#define _UICALENDAR_H_

#include <System/Types/DateTime.h>
#include <Framework/Controls/Panel.h>
#include <Framework/Controls/Button.h>
#include <Framework/Controls/TextBlock.h>
#include <Framework/Controls/CalendarItem.h>

namespace suic
{

class CalendarDayButton : public Button
{
public:

    static DpProperty* IsSelectedProperty;
    static DpProperty* IsTodayProperty;
    static DpProperty* IsInactiveProperty;

    static bool StaticInit();

    RTTIOfClass(CalendarDayButton)

    CalendarDayButton();

    bool IsSelected();
    void SetIsSelected(bool val);

    bool IsToday();
    void SetIsToday(bool val);

    bool IsInactive();
    void SetIsInactive(bool val);
};

class CalendarButton : public Button
{
public:

    static DpProperty* HasSelectedDaysProperty;

    static bool StaticInit();

    RTTIOfClass(CalendarButton)

    CalendarButton();

    bool HasSelectedDays();
    void SetHasSelectedDays(bool val);
};


/// <summary>
/// �����ؼ�
/// </summary>
class SUICORE_API Calendar : public Control
{
public:

    enum CalendarMode
    {
        Month,
        Year,
        Decade,
    };

    enum DayOfWeek
    {
        Monday,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday,
        Sunday,
    };

    EventHandler DayClick;

    Calendar();
    virtual ~Calendar();

    static String ElementHeaderButton;
    static String ElementPreviousButton;
    static String ElementNextButton;

    static DpProperty* DisplayModeProperty;
    static DpProperty* FirstDayOfWeekProperty;

    // ѡ���ʱ������
    static DpProperty* SelectedDateProperty;

    static DpProperty* CalendarItemStyleProperty;
    static DpProperty* CalendarButtonStyleProperty;
    static DpProperty* CalendarDayButtonStyleProperty;

    static bool StaticInit();

    RTTIOfClass(Calendar)

    Style* GetCalendarButtonStyle();
    void SetCalendarButtonStyle(Style* val);

    Style* GetCalendarDayButtonStyle();
    void SetCalendarDayButtonStyle(Style* val);

    DateTime GetDisplayDate() const;
    void SetDisplayDate(DateTime date);

    DateTime GetCurrentDate() const;
    void SetCurrentDate(DateTime date);

    CalendarMode GetDisplayMode();
    void SetDisplayMode(CalendarMode mode);

    DayOfWeek GetFirstDayOfWeek();
    void SetFirstDayOfWeek(DayOfWeek val);

    void UpdateMonthMode();
    void UpdateYearMode();
    void UpdateDecadeMode();

    void SetMonthModeHeaderButton();
    void SetMonthModePreviousButton(); 
    void SetMonthModeNextButton();

    void SetDecadeModeHeaderButton(int decade);
    void SetDecadeModePreviousButton(int decade); 
    void SetDecadeModeNextButton(int decadeEnd);

    void SetYearModeHeaderButton(); 
    void SetYearModePreviousButton();
    void SetYearModeNextButton();

    void SetMonthModeDayTitles();
    void SetMonthModeCalendarDayButtons();

    void SetYearModeMonthButtons();
    void SetYearButtons(int decade, int decadeEnd);

    void UpdateDisplayDay();
    void UpdateDisplayMode(CalendarMode mode);

public:

    void OnPreviousButtonClick(Element* sender, RoutedEventArg* e);
    void OnNextButtonClick(Element* sender, RoutedEventArg* e);
    void OnHeaderButtonClick(Element* sender, RoutedEventArg* e);

    void OnCalendarButtonClick(Element* sender, RoutedEventArg* e);
    void OnCalendarDayButtonClick(Element* sender, RoutedEventArg* e);

    static void OnDisplayModePropertyChanged(DpObject* d, DpPropChangedEventArg* e);

protected:
    
    String CalcJieri(int iMon, int iDay);

    void SwitchCalendarView();
    void InitializeViewContent();
    int CalcuDayIndexOfMonth(const DateTime& date);

    void OnApplyTemplate();

protected:

    // �������
    Panel* _monthView;
    Panel* _yearView;

    Button* _headerButton;
    Button* _nextButton;
    Button* _previousButton;

    DataTemplate* _dayTitleTemplate;

    bool _isDayPressed;
    bool _isMonthPressed;

    // ��ǰ��ʾʱ��
    ODateTime* _displayDate;
    // ѡ��ʱ��
    ODateTime* _currentDate;
};

}

#endif
