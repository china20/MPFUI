// 华勤科技版权所有 2010-2011
// 
// 文件名：WebkitBox.h
// 功  能：使用Webkit实现的浏览器控件。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UIWEBKITBOX_H_
#define _UIWEBKITBOX_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/Content.h>
#include <Extend/Controls/wke.h>

using namespace wke;

namespace suic
{

class SUIWGX_API WebkitLoadCallback  
{  
public:

    virtual void OnLoadFailed() = 0;  
    virtual void OnLoadComplete() = 0;  
    virtual void OnDocumentReady() = 0;  
};

class WebkitClientCallback  
{  
public:

    virtual void OnTitleChange(const suic::String& title) = 0;  
    virtual void OnUriChange(const suic::String& uri) = 0;  
};

class WkeLoader
{
public:

    typedef void (*FnWkeInit)();
    typedef void (*FnWkeShutdown)();
    typedef wkeWebView (*FnWkeCreateWebView)();
    typedef void (*FnWkeDestroyWebView)(wkeWebView);

public:

    WkeLoader();
    ~WkeLoader();

    bool Init(const String& wkePath);

    void WkeInit();
    void WkeShutdown();
    wkeWebView WkeCreateWebView();
    void WkeDestroyWebView(wkeWebView wkeWeb);

    static WkeLoader* Ins();

protected:

    HMODULE _hModWke;

    FnWkeInit _wkeInit;
    FnWkeShutdown _wkeShutdown;
    FnWkeCreateWebView _wkeCreateWebView;
    FnWkeDestroyWebView _wkeDestroyWebView;

    static WkeLoader * _pInst;
};

class WebkitBox : public suic::Control
                 , public wkeBufHandler
{  
public:

    static suic::DpProperty* UriProperty;

    static void OnUriPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static bool StaticInit();
    
    RTTIOfClass(WebkitBox)

    WebkitBox(void);
    ~WebkitBox(void);

    virtual void onBufUpdated(const HDC hdc,int x, int y, int cx, int cy);  

    suic::String GetUrl();

    bool CanGoBack() const;  
    bool GoBack();  
    bool CanGoForward() const;  
    bool GoForward();  
    void StopLoad();  
    void Refresh();  
    wkeWebView GetWebView();
    void SetLoadCallback(WebkitLoadCallback* pCallback);  
    WebkitLoadCallback* GetLoadCallback();  
    void Navigate(suic::String uri);

    void LoadFile(suic::String file);  
    void LoadHtml(const suic::String& html);  

protected:  

    void OnLoaded(suic::LoadedEventArg* e);
    void OnRender(suic::Drawing* drawing);

    void OnTick(suic::Object* sender, suic::EventArg* e);

    void OnGotFocus();
    void OnLostFocus();
    void OnRenderSizeChanged(SizeChangedInfo& e);

    void OnTextInput(KeyboardEventArg* e);
    void OnKeyDown(KeyboardEventArg* e);
    void OnKeyUp(KeyboardEventArg* e);

    void OnMouseMove(MouseButtonEventArg* e);
    void OnMouseWheel(MouseWheelEventArg* e);
    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);

    void OnSetCursor(suic::CursorEventArg* e);

    void OnContextMenu(ContextMenuEventArg* e);

    void StartCheckThread();  
    void StopCheckThread();

    void NavigateUri(suic::String uri);
    suic::Uint32 GetDeviceFlags() const;

    static void OnTitleChange(const struct _wkeClientHandler* clientHandler, const wkeString title);  
    static void OnUriChange(const struct _wkeClientHandler* clientHandler, const wkeString url);  

private:

    static int _webkitCount;

    HANDLE _checkThread; 
    wkeWebView _webView;
    wkeClientHandler _clientHandler;
    WebkitLoadCallback* _loadCallback;
    WebkitClientCallback* _clientCallback;

    suic::AssignerTimer* _tickTimer;
};

}

#endif
