// 华勤科技版权所有 2010-2011
// 
// 文件名：WebWrapper.h
// 功  能：包装WEB控件。
// 
// 作  者：汪荣
// 时  间：2012-07-02
// 
// ============================================================================

# ifndef _UIWEBWRAPPER_H_
# define _UIWEBWRAPPER_H_

#include <sui/sharpuiconfig.h>
#include <sui/activexsite.h>

class WebWrapper : public ActiveXSite
{
public:

    WebWrapper();
    ~WebWrapper(void);

public:

    IWebBrowser2* GetWebBrowser2();
    IHTMLDocument2* GetHTMLDocument2();
    IHTMLDocument3* GetHTMLDocument3();
    IHTMLWindow2* GetHTMLWindow2();
    IHTMLEventObj* GetHTMLEventObject();
  
    BOOL OpenWebBrowser();
    BOOL OpenURL(LPCTSTR pUrl);

private:

    bool  _bWebWndInited;

private:

    IStorage* _pStorage;
    IWebBrowser2* _pWB2;
    IHTMLDocument2* _pHtmlDoc2;
    IHTMLDocument3* _pHtmlDoc3;
    IHTMLWindow2* _pHtmlWnd2;
    IHTMLEventObj* _pHtmlEvent;
};

# endif
