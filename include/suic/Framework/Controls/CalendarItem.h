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
