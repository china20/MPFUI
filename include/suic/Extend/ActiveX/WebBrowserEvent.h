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

#ifndef _UIWEBBROWSEREVENT_H_
#define _UIWEBBROWSEREVENT_H_

#include <Extend/ActiveX/ActiveX.h>

namespace suic
{

class WebBrowser;

/// <summary>
///  WebBrowserEvent��ʵ�ֱ�׼��WEB�ؼ��¼���
/// </summary>
class SUIWGX_API WebBrowserEvent : public DWebBrowserEvents2
{
public:

    WebBrowserEvent(WebBrowser* web);
    virtual ~WebBrowserEvent();

protected:

    WebBrowser* _web;
};

}

#endif
