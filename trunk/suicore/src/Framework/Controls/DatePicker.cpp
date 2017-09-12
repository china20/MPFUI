// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// DatePicker.cpp

#include <Framework/Controls/DatePicker.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/EventHelper.h>
#include <System/Windows/CoreTool.h>

namespace suic
{

/////////////////////////////////////////////////////////////////////////
// DatePicker

ImplementRTTIOfClass(DatePicker, Control)
ImplementRTTIOfClass(CalendarDownButton, Button)

DpProperty* DatePicker::TextProperty;
DpProperty* DatePicker::DisplayDateProperty;
DpProperty* DatePicker::SelectedDateProperty;
DpProperty* DatePicker::CalendarStyleProperty;
DpProperty* DatePicker::IsDropDownOpenProperty;

RoutedEvent* DatePicker::SelectedDateChangedEvent;

DatePicker::DatePicker()
    : _textInput(NULL)
    , _button(NULL)
    , _popup(NULL)
{
    _date = new ODateTime();
    _date->ref();
    SetDate(DateTime());

    _calendar = new Calendar();
    _calendar->ref();
    _calendar->DayClick += EventHandler(this, &DatePicker::OnDayClick);
}

DatePicker::~DatePicker()
{
    _date->unref();
    FREEREFOBJ(_calendar);
}

DateTime DatePicker::GetDate()
{
    return (_date->GetDateTime());
}

void DatePicker::SetDate(const DateTime& dt)
{
    if (_date->GetDateTime().Equal(dt) != 0)
    {
        _date->GetDateTime().SetDate(dt);
        SetValue(SelectedDateProperty, new ODateTime(dt));
        UpdateDate();
    }
}

void DatePicker::OnTextPropChanged(DpObject* d, DpPropChangedEventArg* e)
{

}

void DatePicker::OnSelectedDatePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    DatePicker* box = RTTICast<DatePicker>(d);

    if (NULL != box)
    {
        RoutedEventArg re(box, SelectedDateChangedEvent);
        box->RaiseEvent(&re);
    }
}

void DatePicker::OnIsDropDownOpenChanged(DpObject* d, DpPropChangedEventArg* e)
{
    DatePicker* box = RTTICast<DatePicker>(d);
    bool bIsOpen = e->GetNewValue()->ToBool();

    if (NULL != box)
    {
        if (bIsOpen)
        {
            Assigner::Current()->PushTrackPopup(box->_popup);
            box->GetAssigner()->GetTopAssignerFrame()->RegisterHook(box, MessageHook(box, &DatePicker::OnMessageFilter));
            box->UpdateVisualState(false);
        }
        else
        {
            box->GetAssigner()->GetTopAssignerFrame()->RemoveHookFromObject(box);
            Assigner::Current()->RemoveTrackPopup(box->_popup);
        }
    }
}

void DatePicker::StaticInit()
{
    if (NULL == TextProperty)
    {
        Control::StaticInit();

        SelectedDateChangedEvent = EventHelper::RegisterRoutedEvent(_U("SelectedDateChanged"), RoutingStrategy::Bubble, RTTIType(), RTTIType());

        IsMouseOverProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, 0, &OnVisualStatePropChanged));

        SelectedDateProperty = DpProperty::Register(_T("SelectedDate"), RTTIType(), ODateTime::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender, &DatePicker::OnSelectedDatePropChanged));

        TextProperty = DpProperty::Register(_T("Text"), RTTIType(), OString::RTTIType()
            , DpPropMemory::GetPropMeta(OString::EmptyString, PropMetadataOptions::AffectsRender, &DatePicker::OnTextPropChanged));
        DisplayDateProperty = DpProperty::Register(_T("DisplayDate"), RTTIType(), ODateTime::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender));
        CalendarStyleProperty = DpProperty::Register(_T("CalendarStyle"), RTTIType(), Style::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender));
        IsDropDownOpenProperty = DpProperty::Register(_T("IsDropDownOpen"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender, &DatePicker::OnIsDropDownOpenChanged));
    }
}

String DatePicker::ElementRoot = _T("PART_Root"); 
String DatePicker::ElementTextBox = _T("PART_TextBox"); 
String DatePicker::ElementButton = _T("PART_Button");
String DatePicker::ElementPopup = _T("PART_Popup"); 

bool DatePicker::OnMessageFilter(Object* sender, MessageParam* mp, bool& interrupt)
{
    DatePicker* elem = RTTIEqualCast<DatePicker>(_popup->GetTemplatedParent());

    if (NULL == elem)
    {
        return false;
    }

    if (_popup->GetPopupRoot() != sender)
    {
        if (mp->message == WM_MOUSEMOVE && !PointTransform::PointInElement(elem, NULL))
        {
            return true;
        }
    }
    else
    {
        interrupt = true;
        return false;
    }

    return _popup->OnSysFilterMessage(sender, mp, interrupt);
}

String DatePicker::ToString()
{
    return _date->ToString();
}

String DatePicker::GetText()
{
    return ToString();
}

void DatePicker::AddDays(int iCount)
{
    _date->GetDateTime().AddDays(iCount);
    UpdateDate();
}

Style* DatePicker::GetCalendarStyle()
{
    return RTTICast<Style>(GetValue(CalendarStyleProperty));
}

void DatePicker::SetCalendarStyle(Style* val)
{
    SetValue(CalendarStyleProperty, val);
}

bool DatePicker::IsDropDownOpen()
{
    return GetValue(IsDropDownOpenProperty)->ToBool();
}

void DatePicker::SetIsDropDownOpen(bool val)
{
    SetValue(IsDropDownOpenProperty, BOOLTOBOOLEAN(val));
}

void DatePicker::AddSelectedDateChanged(RoutedEventHandler* h)
{
    AddHandler(SelectedDateChangedEvent, h);
}

void DatePicker::RemoveSelectedDateChanged(RoutedEventHandler* h)
{
    RemoveHandler(SelectedDateChangedEvent, h);
}

void DatePicker::UpdateDate()
{
    if (NULL != _textInput)
    {
        String strDate;
        DateTime& dt = _date->GetDateTime();

        strDate.Format(_U("%04d-%02d-%02d")
            , dt.GetYear()
            , dt.GetMonth()
            , dt.GetDay());

        _textInput->SetText(strDate);
        UpdateLayout();
    }
}

void DatePicker::OnDayClick(Object* sender, EventArg* e)
{
    _popup->AsyncClose();
    SetDate(_calendar->GetCurrentDate());
}

void DatePicker::OnCalendarLoaded(Element* sender, LoadedEventArg* e)
{
}

void DatePicker::OnPopupOpened(Object* sender, EventArg* e)
{
    _calendar->SetDisplayMode(Calendar::CalendarMode::Month);
    UpdateDate();
}

void DatePicker::OnPopupClosed(Object* sender, EventArg* e)
{
}

void DatePicker::ClosePopup()
{
    if (IsDropDownOpen() && NULL != _popup)
    {
        _popup->Close();
    }
}

void DatePicker::OnInitialized(EventArg* e)
{
    Control::OnInitialized(e);
    UpdateDate();
}

void DatePicker::OnApplyTemplate()
{
    if (_popup != NULL)
    {
        _popup->Opened -= EventHandler(this, &DatePicker::OnPopupOpened);
        _popup->Closed -= EventHandler(this, &DatePicker::OnPopupClosed);
        _popup->SetChild(NULL);
    }

    Control::OnApplyTemplate();;

    _textInput = RTTICast<TextEditor>(GetTemplateChild("PART_TextBox"));

    _popup = RTTICast<Popup>(GetTemplateChild("PART_Popup"));
    _button = RTTICast<Button>(GetTemplateChild("PART_Button"));

    if (_popup != NULL)
    {
        _popup->Opened += EventHandler(this, &DatePicker::OnPopupOpened);
        _popup->Closed += EventHandler(this, &DatePicker::OnPopupClosed);
        _popup->SetChild(_calendar);
        _popup->SetPlacementTarget(this);
    }

    Style* calStyle = GetCalendarStyle();
    if (NULL == calStyle)
    {
        calStyle = RTTICast<Style>(FindRes(_U("Calendar")));
    }
    if (calStyle != NULL)
    {
        _calendar->SetStyle(calStyle);
    }

    UpdateDate();
}

void DatePicker::OnKeyDown(KeyboardEventArg* e)
{
    if (_textInput)
    {
        _textInput->Focus();
    }

    if (e->GetKey() == Key::kEscape && _popup)
    {
        _popup->Close();
        e->SetHandled(true);
    }
}

void DatePicker::OnTextInput(KeyboardEventArg* e)
{
    e->SetHandled(true);
}

void DatePicker::OnMouseMove(MouseButtonEventArg* e)
{
    e->SetHandled(true);
    Control::OnMouseMove(e);
}

void DatePicker::OnPreviewMouseLeftButtonDown(MouseButtonEventArg* e)
{
    Control::OnPreviewMouseLeftButtonDown(e);
}

void DatePicker::OnPreviewMouseLeftButtonUp(MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

void DatePicker::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    Control::OnMouseLeftButtonDown(e);
    e->SetHandled(true);
    Focus();
}

void DatePicker::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    Control::OnMouseLeftButtonUp(e);
    e->SetHandled(true);
    InvalidateVisual();
}

}
