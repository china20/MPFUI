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
/// 日历控件
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

    // 选择的时间属性
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

    // 布局面板
    Panel* _monthView;
    Panel* _yearView;

    Button* _headerButton;
    Button* _nextButton;
    Button* _previousButton;

    DataTemplate* _dayTitleTemplate;

    bool _isDayPressed;
    bool _isMonthPressed;

    // 当前显示时间
    ODateTime* _displayDate;
    // 选中时间
    ODateTime* _currentDate;
};

}

#endif
