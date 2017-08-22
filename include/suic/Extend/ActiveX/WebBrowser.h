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

#ifndef _UIWEBBROWSER_H_
#define _UIWEBBROWSER_H_

#include <Extend/ActiveX/ActiveX.h>

namespace suic
{

/// <summary>
///  WebBrowser，实现标准的WEB控件。
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
