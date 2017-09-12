// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////
// Calendar.cpp

#include <Framework/Controls/Calendar.h>
#include <Framework/Controls/UniformGrid.h>

#include <System/Tools/Debug.h>
#include <System/Graphics/SolidcolorBrush.h>

namespace suic
{

ImplementRTTIOfClass(Calendar, Control)
ImplementRTTIOfClass(CalendarDayButton, Button)
ImplementRTTIOfClass(CalendarButton, Button)

static String MONTHS[] =
{
    _U("一月"), _U("二月"), _U("三月"), _U("四月"), 
    _U("五月"), _U("六月"), _U("七月"), _U("八月"), 
    _U("九月"), _U("十月"), _U("十一月"), _U("十二月"), 
};

static String WEEKS[] =
{
    _U("一"), _U("二"), _U("三"), _U("四"), _U("五"), _U("六"), _U("日"), 
};

static int WEEKSIDX[] ={1, 2, 3, 4, 5, 6, 7, };

DpProperty* Calendar::DisplayModeProperty;
DpProperty* Calendar::FirstDayOfWeekProperty;
DpProperty* Calendar::SelectedDateProperty;

DpProperty* Calendar::CalendarItemStyleProperty;
DpProperty* Calendar::CalendarButtonStyleProperty;
DpProperty* Calendar::CalendarDayButtonStyleProperty;

DpProperty* CalendarDayButton::IsSelectedProperty;
DpProperty* CalendarDayButton::IsTodayProperty;
DpProperty* CalendarDayButton::IsInactiveProperty;

DpProperty* CalendarButton::HasSelectedDaysProperty;

CalendarButton::CalendarButton()
{
}

void CalendarButton::StaticInit()
{
    if (NULL == HasSelectedDaysProperty)
    {
        HasSelectedDaysProperty = DpProperty::Register(_T("HasSelectedDays"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));
    }
}

bool CalendarButton::HasSelectedDays()
{
    return GetValue(HasSelectedDaysProperty)->ToInt() != 0;
}

void CalendarButton::SetHasSelectedDays(bool val)
{
    SetValue(HasSelectedDaysProperty, BOOLTOBOOLEAN(val));
}

CalendarDayButton::CalendarDayButton()
{
}

void CalendarDayButton::StaticInit()
{
    if (NULL == IsSelectedProperty)
    {
        IsSelectedProperty = DpProperty::Register(_T("IsSelected"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));
        IsTodayProperty = DpProperty::Register(_T("IsToday"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));
        IsInactiveProperty = DpProperty::Register(_T("IsInactive"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));
    }
}

bool CalendarDayButton::IsSelected()
{
    return GetValue(IsSelectedProperty)->ToInt() != 0;
}

void CalendarDayButton::SetIsSelected(bool val)
{
    SetValue(IsSelectedProperty, BOOLTOBOOLEAN(val));
}

bool CalendarDayButton::IsToday()
{
    return GetValue(IsTodayProperty)->ToInt() != 0;
}

void CalendarDayButton::SetIsToday(bool val)
{
    SetValue(IsTodayProperty, BOOLTOBOOLEAN(val));
}

bool CalendarDayButton::IsInactive()
{
    return GetValue(IsInactiveProperty)->ToInt() != 0;
}

void CalendarDayButton::SetIsInactive(bool val)
{
    SetValue(IsInactiveProperty, BOOLTOBOOLEAN(val));
}

Calendar::Calendar()
    : _displayDate(NULL)
    , _currentDate(NULL)
{
    _monthView = NULL;
    _yearView = NULL;
    _headerButton = NULL;
    _nextButton = NULL;
    _previousButton = NULL;
    _dayTitleTemplate = NULL;

    _isDayPressed = false;
    _isMonthPressed = false;

    SetDisplayDate(DateTime::Today());
    SetCurrentDate(GetDisplayDate());
}

Calendar::~Calendar()
{
    _displayDate->unref();
    _currentDate->unref();
}

void Calendar::StaticInit()
{
    if (NULL == DisplayModeProperty)
    {
        if (NULL == Control::PaddingProperty)
        {
            Control::StaticInit();
        }

        CalendarButton::StaticInit();
        CalendarDayButton::StaticInit();

        IsMouseOverProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, 0, &OnVisualStatePropChanged));
        BackgroundProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(new SolidColorBrush(ARGB(255,255,255,255)), 0));

        DisplayModeProperty = DpProperty::Register(_U("DisplayMode"), RTTIType(), Integer::RTTIType(), 
            DpPropMemory::GetPropMeta(Integer::GetPosInt(Calendar::Month), 0, &Calendar::OnDisplayModePropertyChanged));
        SelectedDateProperty = DpProperty::Register(_U("SelectedDate"), RTTIType(), ODateTime::RTTIType(), DpPropMemory::GetPropMeta(NULL, 0));

        FirstDayOfWeekProperty = DpProperty::Register(_U("FirstDayOfWeek"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(new Integer(Calendar::Sunday), 0));

        CalendarItemStyleProperty = DpProperty::Register(_U("CalendarItemStyle"), RTTIType(), Style::RTTIType(), DpPropMemory::GetPropMeta(NULL));
        CalendarButtonStyleProperty = DpProperty::Register(_U("CalendarButtonStyle"), RTTIType(), Style::RTTIType(), DpPropMemory::GetPropMeta(NULL));
        CalendarDayButtonStyleProperty = DpProperty::Register(_U("CalendarDayButtonStyle"), RTTIType(), Style::RTTIType(), DpPropMemory::GetPropMeta(NULL));
    }
}

String Calendar::ElementHeaderButton = _T("PART_HeaderButton"); 
String Calendar::ElementPreviousButton = _T("PART_PreviousButton");
String Calendar::ElementNextButton = _T("PART_NextButton");

Style* Calendar::GetCalendarButtonStyle()
{
    return RTTICast<Style>(GetValue(CalendarButtonStyleProperty));
}

void Calendar::SetCalendarButtonStyle(Style* val)
{
    SetValue(CalendarButtonStyleProperty, val);
}

Style* Calendar::GetCalendarDayButtonStyle()
{
    return RTTICast<Style>(GetValue(CalendarDayButtonStyleProperty));
}

void Calendar::SetCalendarDayButtonStyle(Style* val)
{
    SetValue(CalendarDayButtonStyleProperty, val);
}

DateTime Calendar::GetDisplayDate() const
{
    return _displayDate->GetDateTime();
}

void Calendar::SetDisplayDate(DateTime date)
{
    if (_displayDate == NULL)
    {
        _displayDate = new ODateTime(date);
        _displayDate->ref();
    }
    else
    {
        _displayDate->GetDateTime().SetDate(date);
    }
}

DateTime Calendar::GetCurrentDate() const
{
    return _currentDate->GetDateTime();
}

void Calendar::SetCurrentDate(DateTime date)
{
    if (_currentDate == NULL)
    {
        _currentDate = new ODateTime(date);
        _currentDate->ref();
    }

    if (_currentDate->GetDateTime().Equal(date) == 0)
    {
        return;
    }
    else
    {
        _currentDate->GetDateTime().SetDate(date);
    }

    SetValue(SelectedDateProperty, new ODateTime(date));
}

Calendar::CalendarMode Calendar::GetDisplayMode()
{
    return (CalendarMode)GetValue(DisplayModeProperty)->ToInt();
}

void Calendar::SetDisplayMode(CalendarMode mode)
{
    if (mode != GetDisplayMode())
    {
        SetValue(DisplayModeProperty, Integer::GetPosInt(mode));
    }
}

Calendar::DayOfWeek Calendar::GetFirstDayOfWeek()
{
    return (DayOfWeek)GetValue(FirstDayOfWeekProperty)->ToInt();
}

void Calendar::SetFirstDayOfWeek(Calendar::DayOfWeek val)
{
    SetValue(FirstDayOfWeekProperty, new Integer(val));
}

void Calendar::SwitchCalendarView()
{
    if (GetDisplayMode() == Calendar::Month)
    {
        if (_monthView)
        { 
            _monthView->SetVisibility(Visibility::Visible);
        }
        if (_yearView)
        {
            _yearView->SetVisibility(Visibility::Hidden);
        }
    }
    else
    {
        if (_monthView)
        { 
            _monthView->SetVisibility(Visibility::Hidden);
        }
        if (_yearView)
        {
            _yearView->SetVisibility(Visibility::Visible);
        }
    }
    
    UpdateLayout();
}

int Calendar::CalcuDayIndexOfMonth(const DateTime& date)
{
    DateTime datebeg(date);
    datebeg.SetMonthBegin();
    // 计算第一个月是星期几？
    int dayofweek = datebeg.GetWeekDay();

    // 星期日 
    if (dayofweek == 0)
    {
        dayofweek = 7;
    }

    dayofweek = (dayofweek + 6 - GetFirstDayOfWeek()) % 7;

    return dayofweek;
}

void Calendar::UpdateDisplayDay()
{
    int i = 0;
    int count = _monthView->GetVisibleChildrenCount() - 7;
    int dayofweek = CalcuDayIndexOfMonth(GetDisplayDate());
    int indexofmoth = dayofweek + GetDisplayDate().GetMonthEndDay();

    for (i = 0; i < dayofweek; ++i)
    {
        CalendarDayButton* btn = DynamicCast<CalendarDayButton>(_monthView->GetVisibleChild(i + 7));
        btn->SetIsSelected(false);
        btn->SetIsInactive(true);
    }

    for (i = dayofweek; i < indexofmoth; ++i)
    {
        CalendarDayButton* btn = DynamicCast<CalendarDayButton>(_monthView->GetVisibleChild(i + 7));
        btn->SetIsSelected(i == GetDisplayDate().GetDay() + dayofweek - 1);
        btn->SetIsInactive(false);
    }

    for (; i < count; ++i)
    {
        CalendarDayButton* btn = DynamicCast<CalendarDayButton>(_monthView->GetVisibleChild(i + 7));
        btn->SetIsSelected(false);
        btn->SetIsInactive(true);
    }
}

void Calendar::UpdateMonthMode()
{
    SetMonthModeHeaderButton();
    SetMonthModePreviousButton(); 
    SetMonthModeNextButton();

    if (_monthView)
    { 
        SetMonthModeDayTitles();
        SetMonthModeCalendarDayButtons(); 
    }

    SwitchCalendarView();
}

void Calendar::UpdateYearMode()
{
    SetYearModeHeaderButton(); 
    SetYearModePreviousButton();
    SetYearModeNextButton(); 

    if (_yearView)
    { 
        SetYearModeMonthButtons();
    }

    SwitchCalendarView();
}

void Calendar::UpdateDecadeMode()
{
    DateTime selectedYear = GetDisplayDate(); 

    int decade = selectedYear.GetYear();
    int decadeEnd = decade + 9;

    decade -= 2;

    if (decade < 1900)
    {
        decade = 1900;
        decadeEnd = 1912;
    }

    SetDecadeModeHeaderButton(decade);
    SetDecadeModePreviousButton(decade); 
    SetDecadeModeNextButton(decadeEnd); 

    if (_yearView) 
    {
        SetYearButtons(decade, decadeEnd);
    }

    UpdateLayout();
}

void Calendar::SetMonthModeHeaderButton()
{
    if (_headerButton)
    {
        String title;

        title.Format(_U("%04d年%02d月"), GetDisplayDate().GetYear(), GetDisplayDate().GetMonth());
        _headerButton->SetContent(new OString(title));
        _headerButton->Enable(true);
    }
}

void Calendar::SetMonthModePreviousButton()
{
}

void Calendar::SetMonthModeNextButton()
{
}

void Calendar::SetDecadeModeHeaderButton(int decade)
{
    if (_headerButton)
    {
        String title;

        title.Format(_U("%04d-%04d"), decade + 1, decade + 10);
        _headerButton->SetContent(new OString(title));
        _headerButton->Enable(false);
    }
}

void Calendar::SetDecadeModePreviousButton(int decade)
{
}

void Calendar::SetDecadeModeNextButton(int decadeEnd)
{
}

void Calendar::SetYearModeHeaderButton()
{
    if (_headerButton)
    {
        String title;

        title.Format(_U("%04d"), GetDisplayDate().GetYear());
        _headerButton->SetContent(new OString(title));
        _headerButton->Enable(true);
    }
}

void Calendar::SetYearModePreviousButton()
{
}

void Calendar::SetYearModeNextButton()
{
}

String Calendar::CalcJieri(int iMon, int iDay)
{
    String strJieri;

    if (iMon == 10 && iDay == 1)
    {
        strJieri = _U("国庆节");
    }
    else if (iMon == 10 && iDay == 16)
    {
        strJieri = _U("世界粮食日");
    }
    else if (iMon == 1 && iDay == 1)
    {
        strJieri = _U("元旦节");
    }
    else if (iMon == 2 && iDay == 14)
    {
        strJieri = _U("情人节");
    }
    else if (iMon == 2 && iDay == 10)
    {
        strJieri = _U("国际气象节");
    }
    else if (iMon == 5 && iDay == 1)
    {
        strJieri = _U("国庆劳动节");
    }
    else if (iMon == 5 && iDay == 22)
    {
        strJieri = _U("国际护士节");
    }
    else if (iMon == 5 && iDay == 31)
    {
        strJieri = _U("世界无烟日");
    }
    else if (iMon == 8 && iDay == 1)
    {
        strJieri = _U("建军节");
    }
    else if (iMon == 12 && iDay == 25)
    {
        strJieri = _U("圣诞节");
    }
    else if (iMon == 12 && iDay == 10)
    {
        strJieri = _U("世界人权日");
    }
    else if (iMon == 4 && iDay == 1)
    {
        strJieri = _U("愚人节");
    }
    else if (iMon == 6 && iDay == 1)
    {
        strJieri = _U("儿童节");
    }
    else if (iMon == 3 && iDay == 8)
    {
        strJieri = _U("妇女节");
    }
    else if (iMon == 3 && iDay == 12)
    {
        strJieri = _U("植树节");
    }
    else if (iMon == 3 && iDay == 14)
    {
        strJieri = _U("国际警察节");
    }
    else if (iMon == 4 && iDay == 5)
    {
        strJieri = _U("清明节");
    }
    else if (iMon == 4 && iDay == 22)
    {
        strJieri = _U("世界地球日");
    }
    else if (iMon == 7 && iDay == 1)
    {
        strJieri = _U("建党节");
    }
    else if (iMon == 9 && iDay == 10)
    {
        strJieri = _U("教师节");
    }

    return strJieri;
}

void Calendar::SetMonthModeDayTitles()
{
    if (_monthView)
    {
        int iToDay = DateTime::Today().GetDay();
        int iToMonth = GetDisplayDate().GetMonth();

        bool bIsToday = false;

        if (DateTime::Today().GetMonth() == iToMonth)
        {
            bIsToday = true;
        }

        DateTime date(GetDisplayDate());
        int count = _monthView->GetVisibleChildrenCount();

        date.SetMonthBegin();
        // 计算第一个月是星期几？
        int dayofweek = CalcuDayIndexOfMonth(date);
        int indexofmoth = dayofweek + date.GetMonthEndDay();

        // 计算前一个月
        if (dayofweek > 0)
        {
            DateTime prevdate = date;
            prevdate.AddMonths(-1);
            int prevdaycount = prevdate.GetMonthEndDay();
            int prevcount = dayofweek;

            for (int i = 0; i < prevcount; ++i)
            {
                CalendarDayButton* btn = DynamicCast<CalendarDayButton>(_monthView->GetVisibleChild(i + 7));
                String title;
                int iDay = prevdaycount - prevcount + i  + 1;
                title.Format(_U("%d"), prevdaycount - prevcount + i  + 1);

                btn->SetContent(new OString(title));
                btn->SetIsInactive(true);
                btn->SetIsToday(false);
                btn->SetIsSelected(false);

                title = CalcJieri(prevdate.GetMonth(), iDay);
                btn->SetToolTip(title);
            }
        }

        for (int i = dayofweek; i < indexofmoth; ++i)
        {
            CalendarDayButton* btn = DynamicCast<CalendarDayButton>(_monthView->GetVisibleChild(i + 7));
            String title;
            int iDay = i - dayofweek + 1;

            title.Format(_U("%d"), iDay);
            btn->SetContent(new OString(title));
            title = CalcJieri(date.GetMonth(), iDay);
            btn->SetToolTip(title);

            if (bIsToday && iToDay == iDay)
            {
                btn->SetIsToday(true);
            }
            else
            {
                btn->SetIsToday(false);
            }

            btn->SetIsInactive(false);
            btn->SetIsSelected(GetDisplayDate().GetDay() == iDay);
        }

        DateTime nextdate = date;
        nextdate.AddMonths(1);

        // 计算后一个月
        for (int i = indexofmoth; i < count - 7; ++i)
        {
            CalendarDayButton* btn = DynamicCast<CalendarDayButton>(_monthView->GetVisibleChild(i + 7));
            String title;
            int iDay = i - indexofmoth + 1;
            title.Format(_U("%d"), iDay);

            btn->SetContent(new OString(title));
            btn->SetIsToday(false);
            btn->SetIsSelected(false);
            btn->SetIsInactive(true);

            title = CalcJieri(nextdate.GetMonth(), iDay);
            btn->SetToolTip(title);
        }
    }
}

void Calendar::SetMonthModeCalendarDayButtons()
{
}

void Calendar::SetYearModeMonthButtons()
{
    int iMonty = GetDisplayDate().GetMonth();
    int count = _yearView->GetVisibleChildrenCount();

    for (int i = 0; i < count; ++i)
    {
        CalendarButton* btn = DynamicCast<CalendarButton>(_yearView->GetVisibleChild(i));
        btn->SetContent(new OString(MONTHS[i]));
        btn->SetHasSelectedDays(i == iMonty - 1);
    }
}

void Calendar::SetYearButtons(int decade, int decadeEnd)
{
    int iYear = GetDisplayDate().GetYear();
    int count = _yearView->GetVisibleChildrenCount();

    for (int i = 0; i < count; ++i)
    {
        CalendarButton* btn = RTTICast<CalendarButton>(_yearView->GetVisibleChild(i));
        String title;
        title.Format(_U("%04d"), decade);
        btn->SetContent(new OString(title));
        btn->SetHasSelectedDays(decade == iYear);

        decade += 1;
    }
}

void Calendar::UpdateDisplayMode(CalendarMode mode)
{
    switch (mode)
    {
    case Calendar::Month:
        UpdateMonthMode();
        //UpdateDisplayDay();
        break;

    case Calendar::Year:
        UpdateYearMode();
        break;

    case Calendar::Decade:
        UpdateDecadeMode();
        break;
    }
}

void Calendar::OnDisplayModePropertyChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Calendar* c = DynamicCast<Calendar>(d);
    Debug::Assert(c != NULL); 
    CalendarMode mode = (CalendarMode)e->GetNewValue()->ToInt(); 

    c->UpdateDisplayMode(mode);
}

void Calendar::InitializeViewContent()
{
    Style* cb(GetCalendarButtonStyle());
    Style* cdb(GetCalendarDayButtonStyle());

    int i = 0;

    //-----------------------------------------------------

    // 构建日历月模板
    UniformGrid* monthView = DynamicCast<UniformGrid>(_monthView);
    // 设置为7行7列
    monthView->SetColumns(7);
    monthView->SetRows(7);

    int iFdw = (int)GetFirstDayOfWeek();

    // 先加入标题
    for (i = 0; i < 7; ++i)
    {
        // 获取星期几
        int windex = (i + iFdw) % 7;
        if (NULL == _dayTitleTemplate)
        {
            TextBlock* elem = new TextBlock();
            elem->SetText(WEEKS[windex]);
            elem->SetHorizontalAlignment(HoriAlign::haCenter);
            elem->SetVerticalAlignment(VertAlign::vaCenter);
            monthView->AddChild(elem);
        }
        else
        {
            AutoObj root = _dayTitleTemplate->LoadContent(this);
            if (root.get() != NULL)
            {
                monthView->AddChild(root.get());
            }
        }
    }

    // 加入6 * 7个button
    for (i = 7; i < 49; ++i)
    {
        CalendarDayButton* btn(new CalendarDayButton());

        monthView->AddChild(btn);
        btn->SetStyle(cdb);
        // 挂接事件
        btn->AddClick(new ClickEventHandler(this, &Calendar::OnCalendarDayButtonClick));
    }

    //-----------------------------------------------------

    // 构建日历的年模板
    UniformGrid* yearView = DynamicCast<UniformGrid>(_yearView);
    // 设置为3行4列
    yearView->SetColumns(4);
    yearView->SetRows(3);

    for (i = 0; i < 12; ++i)
    {
        CalendarButton* btn(new CalendarButton());

        btn->SetStyle(cb);
        btn->SetContent(new OString(MONTHS[i]));
        yearView->AddChild(btn);

        // 挂接事件
        btn->AddClick(new ClickEventHandler(this, &Calendar::OnCalendarButtonClick));
    }
}

void Calendar::OnPreviousButtonClick(Element* sender, RoutedEventArg* e)
{
    switch (GetDisplayMode())
    {
    case CalendarMode::Month:
        _displayDate->GetDateTime().AddMonths(-1);
        UpdateMonthMode();
        break;

    case CalendarMode::Year:
        _displayDate->GetDateTime().AddMonths(-12);
        UpdateYearMode();
        break;

    case CalendarMode::Decade:
        _displayDate->GetDateTime().AddMonths(-120);
        UpdateDecadeMode();
        break;
    }
}

void Calendar::OnNextButtonClick(Element* sender, RoutedEventArg* e)
{
    switch (GetDisplayMode())
    {
    case CalendarMode::Month:
        _displayDate->GetDateTime().AddMonths(1);
        UpdateMonthMode();

        break;

    case CalendarMode::Year:
        _displayDate->GetDateTime().AddMonths(12);
        UpdateYearMode();
        break;

    case CalendarMode::Decade:
        _displayDate->GetDateTime().AddMonths(120);
        UpdateDecadeMode();
        break;
    }
}

void Calendar::OnHeaderButtonClick(Element* sender, RoutedEventArg* e)
{
    switch (GetDisplayMode())
    {
    case CalendarMode::Month:
        SetDisplayMode(CalendarMode::Year);
        break;

    case CalendarMode::Year:
        SetDisplayMode(CalendarMode::Decade);
        break;
    }
}

void Calendar::OnCalendarButtonClick(Element* sender, RoutedEventArg* e)
{
    Button* btn(RTTICast<Button>(sender));

    switch (GetDisplayMode())
    {
    case CalendarMode::Year:
        _displayDate->GetDateTime().SetMonth(btn->GetParentIndex() + 1);
        SetDisplayMode(CalendarMode::Month);        
        break;

    case CalendarMode::Decade:
        _displayDate->GetDateTime().SetYear(btn->ToString().ToInt());
        SetDisplayMode(CalendarMode::Year);
        break;
    }
}

void Calendar::OnCalendarDayButtonClick(Element* sender, RoutedEventArg* e)
{
    int parentIndex = ((Element*)sender)->GetParentIndex() - 6;
    DateTime date(GetDisplayDate());
    int count = _monthView->GetVisibleChildrenCount();
    date.SetMonthBegin();

    // 计算第一个月是星期几？
    int dayofweek = CalcuDayIndexOfMonth(date);
    int indexofmoth = dayofweek + date.GetMonthEndDay();

    // 上一个月
    if (parentIndex <= dayofweek)
    {
        _displayDate->GetDateTime().AddMonths(-1);
        _displayDate->GetDateTime().SetDayOfMonth(sender->ToString().ToInt());
        UpdateMonthMode();
    }
    else if (parentIndex > dayofweek && parentIndex <= indexofmoth)
    {
        _displayDate->GetDateTime().SetDayOfMonth(sender->ToString().ToInt());
        UpdateDisplayDay();
    }
    // 下一个月
    else
    {
        _displayDate->GetDateTime().AddMonths(1);
        _displayDate->GetDateTime().SetDayOfMonth(sender->ToString().ToInt());
        UpdateMonthMode();
    }

    SetCurrentDate(GetDisplayDate());

    if (DayClick)
    {
        EventArg e;
        DayClick(this, &e);
    }
}

void Calendar::OnApplyTemplate()
{
    Control::OnApplyTemplate();

    if (_previousButton != NULL)
    {
        ClickEventHandler e(this, &Calendar::OnPreviousButtonClick);
        _previousButton->RemoveClick(&e);
    }
    if (_nextButton != NULL)
    {
        ClickEventHandler e(this, &Calendar::OnNextButtonClick);
        _nextButton->RemoveClick(&e);
    }
    if (_headerButton != NULL)
    {
        ClickEventHandler e(this, &Calendar::OnHeaderButtonClick);
        _headerButton->RemoveClick(&e);
    }

    _monthView = DynamicCast<Panel>(GetTemplateChild("PART_MonthView"));
    _yearView = DynamicCast<Panel>(GetTemplateChild("PART_YearView"));
    _previousButton = DynamicCast<Button>(GetTemplateChild("PART_PreviousButton"));
    _nextButton = DynamicCast<Button>(GetTemplateChild("PART_NextButton"));
    _headerButton = DynamicCast<Button>(GetTemplateChild("PART_HeaderButton"));

    _dayTitleTemplate = NULL;
    if (NULL != GetResources())
    {
        _dayTitleTemplate = DynamicCast<DataTemplate>(GetResources()->Lookup(_U("DayTitleTemplate")));
    }

    if (_previousButton != NULL)
    {
        _previousButton->AddClick(new ClickEventHandler(this, &Calendar::OnPreviousButtonClick));
    }
    if (_nextButton != NULL)
    {
        _nextButton->AddClick(new ClickEventHandler(this, &Calendar::OnNextButtonClick));
    }
    if (_headerButton != NULL)
    {
        _headerButton->AddClick(new ClickEventHandler(this, &Calendar::OnHeaderButtonClick));
    }

    InitializeViewContent();
    UpdateDisplayMode(GetDisplayMode());
}

}
