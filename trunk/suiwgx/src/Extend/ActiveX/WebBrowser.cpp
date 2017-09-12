
/////////////////////////////////////////////////////////////////////////
// WebBrowser.cpp

#include "stdafx.h"
#include <Extend/ActiveX/WebBrowser.h>

namespace suic
{

ImplementRTTIOfClass(WebBrowser, ActiveX)

WebBrowser::WebBrowser()
    : ActiveX(new ActiveXSite())
    , _canGoForward(false)
    , _canGoBack(false)
    , _web2(NULL)
{
}

WebBrowser::~WebBrowser()
{
    if (_web2)
    {
        _web2->Release();
    }
}

bool WebBrowser::CanGoBack() const
{
    return _canGoBack;
}

bool WebBrowser::CanGoForward() const
{
    return _canGoForward;
}

void WebBrowser::GoBack()
{
    if (_web2)
    {
        _web2->GoBack();
        _web2->Release();
    }
}

void WebBrowser::GoForward()
{
    if (_web2)
    {
        _web2->GoForward();
        _web2->Release();
    }
}

void WebBrowser::GoHome()
{
    if (_web2)
    {
        _web2->GoHome();
        _web2->Release();
    }
}

suic::Object* WebBrowser::InvokeScript(suic::String scriptName)
{
    return NULL;
}

void WebBrowser::Navigate(const suic::String& uri)
{
    // 没有激活，首先激活
    if (!_site)
    {
        _site = new ActiveXSite();
    }

    // 创建WebBrowser控件
    if (!IsValid())
    {
        LoadActiveX(CLSID_WebBrowser);
    }

    if (IsValid())
    {
        if (!_web2)
        {
            //
            // 查询Web控件，用来加载网页
            //
            QueryControl(IID_IWebBrowser2,(void**)&_web2);
        }

        if (_web2)
        {
            _variant_t vUri = _variant_t(uri.c_str());
            _web2->Navigate2(&vUri, 0, 0, 0, 0);

            //
            // 激活控件
            //
            DoVerb(OLEIVERB_INPLACEACTIVATE);
        }
    }
}

void WebBrowser::Refresh()
{
    if (_web2)
    {
        _web2->Refresh();
        _web2->Release();
    }
}

void WebBrowser::CreateEventSink()
{
    //(new WebBrowserEvent(this));
}

//============================================================
//
void WebBrowser::OnArrange(const suic::Size& arrangeSize)
{
    ActiveX::OnArrange(arrangeSize);
}

suic::DpProperty* WebBrowser::UriProperty;

void WebBrowser::StaticInit()
{
    if (NULL == UriProperty)
    {
        UriProperty = suic::DpProperty::Register(_U("Uri"), RTTIType(), suic::OString::RTTIType()
            , suic::DpPropMemory::GetPropMeta(suic::OString::EmptyString, suic::PropMetadataOptions::AffectsRender, &WebBrowser::OnUriPropChanged));
    }
}

void WebBrowser::OnUriPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{

}

void WebBrowser::OnRenderSizeChanged(suic::SizeChangedInfo& e)
{
    if (_site)
    {
        suic::Point point = PointToScreen(suic::Point());
        suic::Rect rect(point.x, point.y, GetRenderSize().cx, GetRenderSize().cy);
        _site->SetControlRect(&rect);
    }
}

void WebBrowser::OnInitialized(suic::EventArg* e)
{
    ActiveX::OnInitialized(e);
}

void WebBrowser::OnLoaded(suic::LoadedEventArg* e)
{
    ActiveX::OnLoaded(e);

    if (_site)
    {
        suic::String strUri = GetUri();
        _site->SetOwner(this);

        //
        // 如果设置了需要打开的网页地址，则进行打开
        // 这时的地址一般是配置文件的Uri属性配置
        //
        if (!strUri.Empty())
        {
            Navigate(strUri);
        }
    }
}

void WebBrowser::OnRender(suic::Drawing * drawing)
{
    ActiveX::OnRender(drawing);
}

void WebBrowser::OnTextInput(suic::KeyboardEventArg* e)
{
}

void WebBrowser::OnKeyDown(suic::KeyboardEventArg* e)
{
}

void WebBrowser::OnGotFocus(suic::RoutedEventArg* e)
{
}

void WebBrowser::OnLostFocus(suic::RoutedEventArg* e)
{
}

void WebBrowser::OnSetCursor(suic::CursorEventArg* e)
{
}

void WebBrowser::OnMouseEnter(suic::MouseButtonEventArg* e)
{
    ActiveX::OnMouseEnter(e);
}

void WebBrowser::OnMouseMove(suic::MouseButtonEventArg* e)
{
    ActiveX::OnMouseMove(e);
}

void WebBrowser::OnMouseLeave(suic::MouseButtonEventArg* e)
{
    ActiveX::OnMouseLeave(e);
}

void WebBrowser::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    ActiveX::OnMouseLeftButtonDown(e);
}

void WebBrowser::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
{
    ActiveX::OnMouseLeftButtonUp(e);
}

}
