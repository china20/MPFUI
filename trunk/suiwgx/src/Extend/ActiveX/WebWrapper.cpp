
#include "WebWrapper.h"

#include <comutil.h>
#pragma comment(lib,"comsuppw.lib")

#define HRTEST_SE(f,e) f
#define NULLTEST_SE(f,e) f
#define NULLTEST(f) f
#define NULLTEST_E(f,e) f
#define HRTEST_E(f,e) f

#define RECTWIDTH(r) ((r).right - (r).left)
#define RECTHEIGHT(r) ((r).bottom - (r).top)

WebWrapper::WebWrapper(void)
    : _bWebWndInited(false),
    _pStorage(NULL), 
    _pWB2(NULL), 
    _pHtmlDoc2(NULL), 
    _pHtmlDoc3(NULL), 
    _pHtmlWnd2(NULL), 
    _pHtmlEvent(NULL)
{
    /*HRTEST_SE(OleInitialize(0), L"Ole初始化错误");
    HRTEST_SE(StgCreateDocfile(0, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT | STGM_CREATE, 0, &_pStorage), L"StgCreateDocfile错误");
    HRTEST_SE(OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, 0, this, _pStorage, (void**)&_oleObj), L"Ole创建失败");
    HRTEST_SE(_oleObj->QueryInterface(IID_IOleInPlaceObject, (LPVOID*)&_inPlaceObj), L"OleInPlaceObject创建失败");*/
}

WebWrapper::~WebWrapper(void)
{
}

IWebBrowser2* WebWrapper::GetWebBrowser2()
{
    if (_pWB2 != NULL)
    {
        return _pWB2;
    }

    NULLTEST_SE(_oleObj, L"Ole对象为空");
    HRTEST_SE(_oleObj->QueryInterface(IID_IWebBrowser2,(void**)&_pWB2),L"QueryInterface IID_IWebBrowser2 失败");

    return _pWB2;
}

IHTMLDocument2* WebWrapper::GetHTMLDocument2()
{
    if (_pHtmlDoc2 != NULL)
    {
        return _pHtmlDoc2;
    }

    IWebBrowser2* pWB2 = NULL;
    NULLTEST(pWB2 = GetWebBrowser2());//GetWebBrowser2已经将错误原因交给LastError.
    IDispatch* pDp =  NULL;
    HRTEST_SE(pWB2->get_Document(&pDp),L"DWebBrowser2::get_Document 错误");
    HRTEST_SE(pDp->QueryInterface(IID_IHTMLDocument2,(void**)&_pHtmlDoc2),L"QueryInterface IID_IHTMLDocument2 失败");
    return _pHtmlDoc2;
}

IHTMLDocument3* WebWrapper::GetHTMLDocument3()
{
    if (_pHtmlDoc3 != NULL)
    {
        return _pHtmlDoc3;
    }

    IWebBrowser2* pWB2 = NULL;
    NULLTEST(pWB2 = GetWebBrowser2());//GetWebBrowser2已经将错误原因交给LastError.
    IDispatch* pDp =  NULL;
    HRTEST_SE(pWB2->get_Document(&pDp),L"DWebBrowser2::get_Document 错误");
    HRTEST_SE(pDp->QueryInterface(IID_IHTMLDocument3,(void**)&_pHtmlDoc3),L"QueryInterface IID_IHTMLDocument3 失败");
    
    return _pHtmlDoc3;
}

IHTMLWindow2* WebWrapper::GetHTMLWindow2()
{
    if (_pHtmlWnd2 != NULL)
    {
        return _pHtmlWnd2;
    }

    IHTMLDocument2*  pHD2 = GetHTMLDocument2();
    NULLTEST( pHD2 );
    HRTEST_SE( pHD2->get_parentWindow(&_pHtmlWnd2),L"IHTMLWindow2::get_parentWindow 错误" );
    return _pHtmlWnd2;
}

IHTMLEventObj* WebWrapper::GetHTMLEventObject()
{
    if (_pHtmlEvent != NULL)
    {
        return _pHtmlEvent;
    }

    IHTMLWindow2* pHW2;
    NULLTEST( pHW2 = GetHTMLWindow2() );
    HRTEST_SE( pHW2->get_event(&_pHtmlEvent),L"IHTMLWindow2::get_event 错误");
    
    return _pHtmlEvent;
}

BOOL WebWrapper::OpenWebBrowser()
{
    BOOL bRet = FALSE;

    NULLTEST_E(GetOleObject(), L"ActiveX对象为空");//对于本身的实现函数,其自身承担错误录入工作

    if ((RECTWIDTH(_rect) && RECTHEIGHT(_rect)) == 0)
    {
        ::GetClientRect(HandleToHwnd(GetHandle()) ,&_rect);//设置WebWrapper的大小为窗口的客户区大小.
    }

    if (!_bInPlaced)// Activate In Place
    {
        _bInPlaced = true;//_bInPlaced must be set as true, before INPLACEACTIVATE, otherwise, once DoVerb, it would return error;
        _bExternalPlace = 0;//lParam;

        HRTEST_E(GetOleObject()->DoVerb(OLEIVERB_INPLACEACTIVATE,0,this,0, HandleToHwnd(GetHandle()), &_rect), L"关于INPLACE的DoVerb错误");
        //_bInPlaced = false;

        // 挂接DWebBrwoser2Event
        IConnectionPointContainer* pCPC = NULL;
        IConnectionPoint* pCP  = NULL;
        HRTEST_E(GetWebBrowser2()->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC),L"枚举IConnectionPointContainer接口失败");
        HRTEST_E(pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2, &pCP), L"FindConnectionPoint失败");
        DWORD dwCookie = 0;
        HRTEST_E(pCP->Advise((IUnknown*)(void*)this,&dwCookie),L"IConnectionPoint::Advise失败");
    }

    bRet = TRUE;

    return bRet;
}

BOOL WebWrapper::OpenURL(LPCTSTR pUrl)
{
    _variant_t vUri = _variant_t(pUrl);
    BOOL bRet = FALSE;
    HRTEST_E(GetWebBrowser2()->Navigate2(&vUri, 0, 0, 0, 0), L"GetWebBrowser2 失败");
    bRet = TRUE;

    return bRet;
}
