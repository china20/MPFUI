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

#ifndef _UICALENDARITEM_H_
#define _UICALENDARITEM_H_

#include <System/Types/DateTime.h>
#include <Framework/Controls/Panel.h>
#include <Framework/Controls/Button.h>
#include <Framework/Controls/TextBlock.h>

namespace suic
{

class Calendar;

class SUICORE_API CalendarItem : public Control
{
public:

    CalendarItem();

    RTTIOfClass(CalendarItem)

public:

    void OnPreviousButtonClick(Element* sender, RoutedEventArg* e);
    void OnNextButtonClick(Element* sender, RoutedEventArg* e);
    void OnHeaderButtonClick(Element* sender, RoutedEventArg* e);

protected:

    void OnApplyTemplate();

private:

    Calendar* _calendar;
    DataTemplate* _dayTitleTemplate;
    FrameworkElement* _disabledVisual;
    
    Panel* _monthView;
    Panel* _yearView;

    Button* _headerButton;
    Button* _nextButton;
    Button* _previousButton;
    
    bool _isDayPressed;
    bool _isMonthPressed;
};

}

#endif
