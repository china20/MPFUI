// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// WebkitBox.cpp

#include "stdafx.h"

#include <Extend/Controls/WebkitBox.h>
#include <Framework/Animation/AnimationAsKeyFrame.h>

namespace suic
{

ImplementRTTIOfClass(WebkitBox, suic::Control)

suic::DpProperty* WebkitBox::UriProperty;

void WebkitBox::OnUriPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    WebkitBox* webBox = suic::RTTICast<WebkitBox>(d);
    if (webBox->IsLoaded())
    {
        webBox->NavigateUri(e->GetNewValue()->ToString());
    }
}

void WebkitBox::StaticInit()
{
    if (NULL == UriProperty)
    {
        UriProperty = suic::DpProperty::Register(_U("Uri"), RTTIType(), suic::OString::RTTIType(), 
            suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsRender, &WebkitBox::OnUriPropChanged));
    }
}

//============================================================================
//

DWORD WINAPI CheckThreadFun(LPVOID lpParam)  
{  
    WebkitBox* pWebkitUI = (WebkitBox*)lpParam;  
    wkeWebView  pWebView = pWebkitUI->GetWebView();  
    if (NULL == pWebView)  
    {
        return 1;  
    }

    WebkitLoadCallback* pCallback = pWebkitUI->GetLoadCallback();  
    if (NULL == pCallback)  
    {
        return 1;  
    }

    bool bOver = false;  
    while (!pWebView->isLoaded())  
    {  
        if (!bOver && pWebView->isDocumentReady())  
        {  
            pCallback->OnDocumentReady();  
            bOver = true;  
        }  
        if (pWebView->isLoadFailed())  
        {  
            pCallback->OnLoadFailed();  
            return 1;  
        }
        ::Sleep(30);  
    }

    if (pWebView->isLoadComplete())  
    {
        pCallback->OnLoadComplete(); 
    }

    return 0;  
}
  
//////////////////////////////////////////////////////  
int WebkitBox::_webkitCount = 0; 

WkeLoader * WkeLoader::_pInst = 0;

WkeLoader* WkeLoader::Ins()
{
    if (0 == _pInst)
    {
        _pInst = new WkeLoader;
    }
    return _pInst;
}

WkeLoader::WkeLoader() 
    : _hModWke(0)
{
}

WkeLoader::~WkeLoader()
{
    if (_hModWke) 
    {
        FreeLibrary(_hModWke);
    }
}

bool WkeLoader::Init(const String& wkePath)
{
    if (_hModWke) 
    {
        return true;
    }

    HMODULE _hModWke = LoadLibrary(wkePath.c_str());

    if (!_hModWke) 
    {
        return false;
    }

    _wkeInit = (FnWkeInit)GetProcAddress(_hModWke,"wkeInit");
    _wkeShutdown = (FnWkeShutdown)GetProcAddress(_hModWke,"wkeShutdown");
    _wkeCreateWebView = (FnWkeCreateWebView)GetProcAddress(_hModWke,"wkeCreateWebView");
    _wkeDestroyWebView = (FnWkeDestroyWebView)GetProcAddress(_hModWke,"wkeDestroyWebView");

    if (!_wkeInit || !_wkeShutdown
        || !_wkeCreateWebView || !_wkeDestroyWebView)
    {
        FreeLibrary(_hModWke);
        return false;
    }

    return true;
}

void WkeLoader::WkeInit()
{
    if (NULL != _wkeInit)
    {
        _wkeInit();
    }
}

void WkeLoader::WkeShutdown()
{
    if (0 != _wkeShutdown)
    {
        _wkeShutdown();
    }

    delete _pInst;
    _pInst = NULL;
}

wkeWebView WkeLoader::WkeCreateWebView()
{
    if (_wkeCreateWebView != 0)
    {
        return _wkeCreateWebView();
    }
    else
    {
        return 0;
    }
}

void WkeLoader::WkeDestroyWebView(wkeWebView wkeWeb)
{
    if (0 != wkeWeb && 0 != _wkeDestroyWebView)
    {
        _wkeDestroyWebView(wkeWeb);
    }
}

WebkitBox::WebkitBox(void)
    : _webView(NULL)
    , _checkThread(NULL)
    , _loadCallback(NULL)
    , _clientCallback(NULL)
{
    if (0 == _webkitCount)  
    {
        suic::String wkePath = suic::FileDir::CalculatePath(_U("wke.dll"));
        WkeLoader::Ins()->Init(wkePath);
        WkeLoader::Ins()->WkeInit();
    }

    _tickTimer = new suic::AssignerTimer();
    _webView = WkeLoader::Ins()->WkeCreateWebView();

    _tickTimer->ref();

    if (NULL != _webView)
    {
        _webView->setBufHandler(this);  
    }

    _clientHandler.onTitleChanged = &WebkitBox::OnTitleChange;  
    _clientHandler.onURLChanged = &WebkitBox::OnUriChange;  
    _webkitCount++;
}

WebkitBox::~WebkitBox(void)  
{  
    StopCheckThread();
    WkeLoader::Ins()->WkeDestroyWebView(_webView);
    _webkitCount--;

    if (0 == _webkitCount)  
    {
        WkeLoader::Ins()->WkeShutdown();
    }

    _tickTimer->unref();
}

void WebkitBox::OnGotFocus()
{
    if (_webView) 
    {
        _webView->focus();
    }
}

void WebkitBox::OnLostFocus()
{
    if (_webView) 
    {
        _webView->unfocus();
    }
}

void WebkitBox::OnRenderSizeChanged(SizeChangedInfo& e)
{
    suic::Control::OnRenderSizeChanged(e);
    if (_webView) 
    {
        Size newSize = e.GetNewSize();
        _webView->resize(newSize.Width(), newSize.Height());
        _webView->tick();
    }
}

void WebkitBox::OnTextInput(KeyboardEventArg* e)
{
    if (_webView)
    {
        suic::Uint32 flags = 0;
        suic::Uint32 charCode = e->GetKey();

        if (e->GetKeyFlags() & KeyFlags::kfRepeat)
        {
            flags |= WKE_REPEAT;
        }

        if (e->GetKeyFlags() & KeyFlags::kfExtended)
        {
            flags |= WKE_EXTENDED;
        }

        _webView->keyPress(charCode, flags, false);
    }
}

void WebkitBox::OnKeyDown(KeyboardEventArg* e)
{
    if (_webView) 
    {
        suic::Uint32 flags = 0;
        suic::Uint32 charCode = e->GetKey();

        if (e->GetKeyFlags() & KeyFlags::kfRepeat)
        {
            flags |= WKE_REPEAT;
        }

        if (e->GetKeyFlags() & KeyFlags::kfExtended)
        {
            flags |= WKE_EXTENDED;
        }

        _webView->keyDown(charCode, flags, false);

        if (charCode == VK_F5)  
        {
            Refresh();
        }
    }
}

void WebkitBox::OnKeyUp(KeyboardEventArg* e)
{
    if (_webView) 
    {
        suic::Uint32 flags = 0;
        suic::Uint32 charCode = e->GetKey();

        if (e->GetKeyFlags() & KeyFlags::kfRepeat)
        {
            flags |= WKE_REPEAT;
        }

        if (e->GetKeyFlags() & KeyFlags::kfExtended)
        {
            flags |= WKE_EXTENDED;
        }

        _webView->keyUp(charCode, flags, false);
    }
}

suic::Uint32 WebkitBox::GetDeviceFlags() const
{
    suic::Uint32 flags = 0;

    if (Keyboard::IsControlDown())
    {
        flags |= WKE_CONTROL;
    }

    if (Keyboard::IsShiftDown())
    {
        flags |= WKE_SHIFT;
    }

    if (MouseDevice::GetLeftButton() == MouseButtonState::mbMousePress)
    {
        flags |= WKE_LBUTTON;
    }

    if (MouseDevice::GetMiddleButton() == MouseButtonState::mbMousePress)
    {
        flags |= WKE_MBUTTON;
    }

    if (MouseDevice::GetRightButton() == MouseButtonState::mbMousePress)  
    {
        flags |= WKE_RBUTTON;  
    }

    return flags;
}

void WebkitBox::OnContextMenu(ContextMenuEventArg* e)
{
    if (_webView)
    {
        suic::Uint32 flags = GetDeviceFlags();
        bool handled = _webView->contextMenuEvent(e->GetCursorLeft(), e->GetCursorTop(), flags);

        if (handled)
        {
            return;
        }
    }
}

void WebkitBox::OnSetCursor(suic::CursorEventArg* e)
{
    e->SetHandled(true);
    e->SetIgnore(true);
    e->SetCursor(NULL);
}

void WebkitBox::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    suic::Control::OnMouseLeftButtonDown(e);
    Focus();
    SetCaptureMouse();

    if (NULL != _webView)
    {
        Point pt = e->GetMousePoint();
        Point ptOffset = PointToScreen(Point());
        suic::Uint32 flags = GetDeviceFlags();
        bool bHandled = false;
        
        if (e->GetClickCount() % 2 == 0)
        {
            bHandled = _webView->mouseEvent(WM_LBUTTONDBLCLK, pt.x - ptOffset.x, pt.y - ptOffset.y, flags);
        }
        else
        {
            bHandled = _webView->mouseEvent(WM_LBUTTONDOWN, pt.x - ptOffset.x, pt.y - ptOffset.y, flags);
        }

        e->SetHandled(true);
    }
}

void WebkitBox::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    suic::Control::OnMouseLeftButtonUp(e);
    ReleaseCaptureMouse();
    if (NULL != _webView)
    {
        Point pt = e->GetMousePoint();
        Point ptOffset = PointToScreen(Point());
        suic::Uint32 flags = GetDeviceFlags();
        bool bHandled = _webView->mouseEvent(WM_LBUTTONUP, pt.x - ptOffset.x, pt.y - ptOffset.y, flags);  
        e->SetHandled(true);
    }
}

void WebkitBox::OnMouseMove(MouseButtonEventArg* e)
{
    suic::Control::OnMouseMove(e);
    if (NULL != _webView)
    {
        Point pt = e->GetMousePoint();
        Point ptOffset = PointToScreen(Point());
        suic::Uint32 flags = GetDeviceFlags();
        bool bHandled = _webView->mouseEvent(WM_MOUSEMOVE, pt.x - ptOffset.x, pt.y - ptOffset.y, flags);  
        e->SetHandled(true);
    }
}

void WebkitBox::OnMouseWheel(MouseWheelEventArg* e)
{
    suic::Control::OnMouseWheel(e);
    if (NULL != _webView)
    {
        Point pt = e->GetMousePoint();
        suic::Uint32 flags = GetDeviceFlags();
        bool bHandled = _webView->mouseWheel(pt.x, pt.y, e->GetDelta(), flags);;  
        e->SetHandled(bHandled);
    }
}

void WebkitBox::OnTick(suic::Object* sender, suic::EventArg* e)
{
    if (_webView)  
    {
        _webView->tick();
    }
}

void WebkitBox::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Control::OnLoaded(e);

    NavigateUri(GetUrl());

    if (NULL != _webView)
    {
        _tickTimer->SetTick(suic::EventHandler(this, &WebkitBox::OnTick));
        _tickTimer->SetInterval(50);
        _tickTimer->Start();
    }
}

void WebkitBox::OnRender(suic::Drawing* drawing)  
{
    if (_webView)  
    {
        suic::Rect rcClip = drawing->GetClipBound().ToRect();
        suic::Bitmap* dBmp = drawing->GetBitmap();
        suic::Rect rcClnt = GetRenderRect();
        suic::Point webOffset = PointToScreen(Point());
        suic::fPoint offset;// = drawing->GetLayerOffset();
        
        rcClnt.Offset(webOffset);
        rcClip.Offset(offset.ToPoint());
        rcClip.Intersect(&rcClnt);
        rcClip.Offset(-offset.x, -offset.y);

        if (!rcClip.IsZero())
        {
            HWND hwnd = HANDLETOHWND(suic::HwndHelper::GetHostHwnd(this));
            HDC tmdc = ::GetDC(hwnd);
            suic::AlphaOp aOp(tmdc, dBmp, rcClip);

            aOp.Backup(drawing, rcClip);
            BitBlt(aOp.GetDrawDc(), drawing->GetOffset().x, drawing->GetOffset().y, rcClnt.Width(), rcClnt.Height(),
                _webView->getViewDC(), 0, 0, SRCCOPY);
            aOp.Restore(drawing);

            ::ReleaseDC(hwnd, tmdc);
        }
    }
}

void WebkitBox::onBufUpdated(const HDC hdc,int x, int y, int cx, int cy)  
{
    Point pt = PointToScreen(Point(x, y));
    Rect rect(pt.x, pt.y, cx, cy);
    InvalidateRect(&rect, false);
}

suic::String WebkitBox::GetUrl()
{
    return GetValue(UriProperty)->ToString();
}

void WebkitBox::Navigate(suic::String uri)  
{
    SetValue(UriProperty, new suic::OString(uri));
}

void WebkitBox::NavigateUri(suic::String uri)  
{  
    if (_webView)  
    {  
        _webView->loadURL(uri.c_str());  
        StartCheckThread();  
    }
}

void WebkitBox::StartCheckThread()  
{  
    StopCheckThread();  
    _checkThread = ::CreateThread(NULL, 0, CheckThreadFun, this, 0, NULL);  
}

void WebkitBox::StopCheckThread()  
{  
    if (_checkThread)  
    {  
        if (::WaitForSingleObject(_checkThread, 10) != WAIT_OBJECT_0)  
        {
            ::TerminateThread(_checkThread, 0);  
        }
        ::CloseHandle(_checkThread);  
        _checkThread = NULL;  
    }  
}

bool WebkitBox::CanGoBack() const  
{  
    return _webView ? _webView->canGoBack() : false;  
}

bool WebkitBox::GoBack()  
{  
    return _webView ? _webView->goBack():false;  
}

bool WebkitBox::CanGoForward() const  
{  
    return _webView ? _webView->canGoForward():false;  
}

bool WebkitBox::GoForward()  
{  
    return _webView ? _webView->goForward():false;  
}

void WebkitBox::StopLoad()  
{  
    if (_webView)  
    {
        _webView->stopLoading();
    }
}  

void WebkitBox::Refresh()  
{  
    if (_webView)  
    {  
        StopCheckThread();  
        _webView->reload();  
        StartCheckThread();  
    }  
}

wkeWebView WebkitBox::GetWebView()  
{  
    return _webView;  
}  

void WebkitBox::SetLoadCallback(WebkitLoadCallback* pCallback)  
{  
    _loadCallback = pCallback;  
}  

WebkitLoadCallback* WebkitBox::GetLoadCallback()  
{  
    return _loadCallback;  
}  

void WebkitBox::OnTitleChange(const struct _wkeClientHandler* clientHandler, const wkeString title)  
{  
  
}  

void WebkitBox::OnUriChange(const struct _wkeClientHandler* clientHandler, const wkeString url)  
{  
  
}  
  
void WebkitBox::LoadFile(suic::String file)  
{  
    if (_webView)  
    {
        _webView->loadFile(file.c_str());  
    }
}  
  
void WebkitBox::LoadHtml(const suic::String& html)  
{  
    if (_webView)  
    {
        _webView->loadHTML(html.c_str()); 
    }
}

}
