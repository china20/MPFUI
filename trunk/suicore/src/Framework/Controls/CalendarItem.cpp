// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Calendar.cpp

#include <Framework/Controls/CalendarItem.h>
#include <System/Tools/Debug.h>
#include <System/Graphics/SolidcolorBrush.h>

namespace suic
{

ImplementRTTIOfClass(CalendarItem, Control)

CalendarItem::CalendarItem()
{
}

void CalendarItem::OnApplyTemplate()
{
    Control::OnApplyTemplate();

    if (_previousButton != NULL)
    {
        ClickEventHandler e(this, &CalendarItem::OnPreviousButtonClick);
        _previousButton->RemoveClick(&e);
    }
    if (_nextButton != NULL)
    {
        ClickEventHandler e(this, &CalendarItem::OnNextButtonClick);
        _nextButton->RemoveClick(&e);
    }
    if (_headerButton != NULL)
    {
        ClickEventHandler e(this, &CalendarItem::OnHeaderButtonClick);
        _headerButton->RemoveClick(&e);
    }

    _monthView = DynamicCast<Panel>(GetTemplateChild("PART_MonthView"));
    _yearView = DynamicCast<Panel>(GetTemplateChild("PART_YearView"));
    _previousButton = DynamicCast<Button>(GetTemplateChild("PART_PreviousButton"));
    _nextButton = DynamicCast<Button>(GetTemplateChild("PART_NextButton"));
    _headerButton = DynamicCast<Button>(GetTemplateChild("PART_HeaderButton"));
    _disabledVisual = DynamicCast<FrameworkElement>(GetTemplateChild("PART_DisabledVisual"));
    _dayTitleTemplate = NULL;
}

void CalendarItem::OnPreviousButtonClick(Element* sender, RoutedEventArg* e)
{
}

void CalendarItem::OnNextButtonClick(Element* sender, RoutedEventArg* e)
{
}

void CalendarItem::OnHeaderButtonClick(Element* sender, RoutedEventArg* e)
{
}

}
