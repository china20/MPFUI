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

#ifndef _UIWEBBROWSER_H_
#define _UIWEBBROWSER_H_

#include <Extend/ActiveX/ActiveX.h>

namespace suic
{

/// <summary>
///  WebBrowser��ʵ�ֱ�׼��WEB�ؼ���
/// </summary>
class SUIWGX_API WebBrowser : public ActiveX
{
public:

    static suic::DpProperty* UriProperty;

    static bool StaticInit();

    static void OnUriPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    WebBrowser();
    virtual ~WebBrowser();

    RTTIOfClass(WebBrowser)

    bool CanGoBack() const;
    bool CanGoForward() const;

    suic::String GetUri();
    void SetUri(suic::String uri);

    void GoBack();
    void GoForward();
    void GoHome();

    suic::Object* InvokeScript(suic::String scriptName);

    void Navigate(const suic::String& source);
    void Refresh();

    void CreateEventSink();

protected:

    void OnArrange(const suic::Size& arrangeSize);

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnRenderSizeChanged(suic::SizeChangedInfo& e);

    void OnRender(suic::Drawing * drawing);

    void OnTextInput(suic::KeyboardEventArg* e);
    void OnKeyDown(suic::KeyboardEventArg* e);

    void OnGotFocus(suic::RoutedEventArg* e);
    void OnLostFocus(suic::RoutedEventArg* e);

    void OnSetCursor(suic::CursorEventArg* e);

    void OnMouseEnter(suic::MouseButtonEventArg* e);
    void OnMouseMove(suic::MouseButtonEventArg* e);
    void OnMouseLeave(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(suic::MouseButtonEventArg* e);

protected:

    IWebBrowser2* _web2;

    bool _canGoBack; 
    bool _canGoForward;

    friend class WebBrowserEvent;
};

typedef suic::shared<WebBrowser> WebBrowserPtr;

inline suic::String WebBrowser::GetUri()
{
    return GetValue(UriProperty)->ToString();
}

inline void WebBrowser::SetUri(suic::String uri)
{
    SetValue(UriProperty, new suic::OString(uri));
}

};

#endif
