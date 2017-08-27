// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// ActiveXSite.cpp

#include "stdafx.h"

#include <Extend/ActiveX/ActiveXSite.h>
#include <System/Windows/CoreTool.h>
#include <System/Tools/HwndHelper.h>

#pragma comment(lib,"comsuppw.lib")

#define RECTWIDTH(r) ((r).right - (r).left)
#define RECTHEIGHT(r) ((r).bottom - (r).top)

ActiveXSite::ActiveXSite(void)
    : _refNum(0)
    , _bInPlaced(false)
    , _oleObj(NULL)
    , _inPlaceObj(NULL)
    , _viewObject(NULL)
    , _container(NULL)
    , _oleSite(NULL)
    , _owner(NULL)
    , _bWindowless(false)
{
}

ActiveXSite::~ActiveXSite(void)
{
    Clear();
}

void ActiveXSite::Clear()
{
    if (_oleSite != NULL) 
    {
        _oleSite->Release();
        _oleSite = NULL;
    }

    if (_inPlaceObj != NULL) 
    {
        _inPlaceObj->Release();
        _inPlaceObj = NULL;
    }

    if (_viewObject != NULL) 
    {
        _viewObject->Release();
        _viewObject = NULL;
    }

    _oleObj = NULL;
}

void ActiveXSite::SetOwner(suic::Element* val)
{
    _owner = val;
}

suic::Handle ActiveXSite::GetHandle() const
{
    return suic::HwndHelper::GetHostHwnd(_owner); 
}

bool ActiveXSite::DoVerb(int verb)
{
    HRESULT hr = S_FALSE;

    if (GetOleObject() && !_bInPlaced)
    {
        IOleClientSite* pClntSite = NULL;
        QueryInterface(IID_IOleClientSite, (void**)&pClntSite);

        // OLEIVERB_INPLACEACTIVATE
        hr = GetOleObject()->DoVerb(verb, NULL, pClntSite, 0, HANDLETOHWND(GetHandle()), 0);
        pClntSite->Release();
    }

    return SUCCEEDED(hr);
}

STDMETHODIMP ActiveXSite::QueryInterface(REFIID riid, void** ppvObject)
{
    *ppvObject = NULL;

    if (riid == IID_IUnknown)                       
    {
        *ppvObject = static_cast<IOleWindow*>(this);
    }
    else if (riid == IID_IOleClientSite)            
    {
        *ppvObject = static_cast<IOleClientSite*>(this);
    }
    else if (riid == IID_IOleInPlaceSiteWindowless) 
    {
        *ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
    }
    else if (riid == IID_IOleInPlaceSiteEx)        
    {
        *ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
    }
    else if (riid == IID_IOleInPlaceSite)           
    {
        *ppvObject = static_cast<IOleInPlaceSite*>(this);
    }
    else if (riid == IID_IOleWindow)                
    {
        *ppvObject = static_cast<IOleWindow*>(this);
    }
    else if (riid == IID_IOleControlSite)           
    {
        *ppvObject = static_cast<IOleControlSite*>(this);
    }
    else if (riid == IID_IOleContainer)             
    {
        *ppvObject = static_cast<IOleContainer*>(_container);
    }
    else if (riid == IID_IObjectWithSite)           
    {
        *ppvObject = static_cast<IObjectWithSite*>(this);
    }

    if (*ppvObject != NULL) 
    {
        static_cast<IUnknown*>(*ppvObject)->AddRef();
    }

    return (*ppvObject == NULL ? E_NOINTERFACE : S_OK);
}

STDMETHODIMP_(ULONG) ActiveXSite::AddRef()
{
    return ::InterlockedIncrement(&_refNum);
}

STDMETHODIMP_(ULONG) ActiveXSite::Release()
{
    ::InterlockedDecrement(&_refNum);

    if (_refNum == 0)
    {
        delete this;
    }

    return S_OK;
}

// IObjectWithSite
STDMETHODIMP ActiveXSite::SetSite(IUnknown *pOleSite)
{
    if (_oleSite)
    {
        _oleSite->Release();
        _oleSite = NULL;
    }

    if (pOleSite)
    {
        _oleSite = pOleSite;
        _oleSite->AddRef();
    }

    return S_OK;
}

STDMETHODIMP ActiveXSite::GetSite(REFIID riid, LPVOID* ppvSite)
{
    if (ppvSite == NULL) 
    {
        return E_POINTER;
    }

    *ppvSite = NULL;

    if (_oleSite == NULL) 
    {
        return E_FAIL;
    }

    return _oleSite->QueryInterface(riid, ppvSite);
}

// IOleClientSite methods

STDMETHODIMP ActiveXSite::SaveObject()
{
    return S_OK;
}

STDMETHODIMP ActiveXSite::GetMoniker(DWORD dwA, DWORD dwW, IMoniker** pm)
{
    if (pm != NULL) 
    {
        *pm = NULL;
    }

    return E_NOTIMPL;
}

STDMETHODIMP ActiveXSite::GetContainer(IOleContainer** pc)
{
    if (pc == NULL ) 
    {
        return E_POINTER;
    }

    *pc = NULL;
    HRESULT hr = E_NOTIMPL;

    if (_oleSite != NULL) 
    {
        hr = _oleSite->QueryInterface(IID_IOleContainer, (LPVOID*)pc);
    }

    if (FAILED(hr)) 
    {
        hr = QueryInterface(IID_IOleContainer, (LPVOID*)pc);
    }

    return hr;
}

STDMETHODIMP ActiveXSite::ShowObject()
{
    if (GetViewObject() != NULL && !_rect.IsZero()) 
    {
        HDC hdc = ::GetDC(HANDLETOHWND(GetHandle()));

        if (hdc == NULL) 
        {
            return E_FAIL;
        }

        GetViewObject()->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hdc, (RECTL*)&_rect, (RECTL*)&_rect, NULL, NULL);
        ::ReleaseDC(HANDLETOHWND(GetHandle()), hdc);
    }

    return S_OK;
}

STDMETHODIMP ActiveXSite::OnShowWindow(BOOL f)
{
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXSite::RequestNewObjectLayout()
{
    return E_NOTIMPL;
}

// IOleInPlaceSiteWindowless

STDMETHODIMP ActiveXSite::CanWindowlessActivate(void)
{
    return S_OK;
}

STDMETHODIMP ActiveXSite::GetCapture(void)
{
    if (GetHandle() == NULL) 
    {
        return E_UNEXPECTED;
    }

    return  S_OK;
}

STDMETHODIMP ActiveXSite::SetCapture(BOOL fCapture)
{
    if (GetHandle() == NULL) 
    {
        return E_UNEXPECTED;
    }
    else
    {
        if (fCapture) 
        {
            ::SetCapture(HANDLETOHWND(GetHandle())); 
        }
        else 
        {
            ::ReleaseCapture();
        }
    }

    return S_OK;
}

STDMETHODIMP ActiveXSite::GetFocus(void)
{
    if (GetHandle() == NULL || !_owner) 
    {
        return E_UNEXPECTED;
    }
    else
    {
        return _owner->IsFocused() ? S_OK : S_FALSE; 
    }
}

STDMETHODIMP ActiveXSite::SetFocus(BOOL fFocus)
{
    if (GetHandle() == NULL || !_owner) 
    {
        return E_UNEXPECTED;
    }
    else if (fFocus)
    {
        ::SetFocus(HANDLETOHWND(GetHandle()));
        _owner->Focus();
    }

    return S_OK;
}

STDMETHODIMP ActiveXSite::GetDC(LPCRECT pRect, DWORD grfFlags, HDC* phDC)
{
    if (IsWindowless() || GetHandle() == NULL) 
    {
        return E_POINTER;
    }

    *phDC = ::GetDC(HANDLETOHWND(GetHandle()));

    return S_OK;
}

STDMETHODIMP ActiveXSite::ReleaseDC(HDC hDC)
{
    if (IsWindowless() || GetHandle() == NULL) 
    {
        return E_UNEXPECTED;
    }

    ::ReleaseDC(HANDLETOHWND(GetHandle()), hDC);

    return S_OK;
}

STDMETHODIMP ActiveXSite::InvalidateRect(LPCRECT pRect, BOOL fErase)
{
    if (_owner == NULL) 
    {
        return E_UNEXPECTED;
    }

    _owner->InvalidateVisual();

    return S_OK;
}

STDMETHODIMP ActiveXSite::InvalidateRgn(HRGN hRGN, BOOL fErase)
{
    if (GetHandle() == NULL) 
    {
        return E_UNEXPECTED;
    }

    return ::InvalidateRgn(HANDLETOHWND(GetHandle()), hRGN, fErase) ? S_OK : E_FAIL;
}

STDMETHODIMP ActiveXSite::ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
{
    return S_OK;
}

STDMETHODIMP ActiveXSite::AdjustRect(LPRECT prc)
{
    return S_OK;
}

STDMETHODIMP ActiveXSite::OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult)
{
    *plResult = ::DefWindowProc(HANDLETOHWND(GetHandle()), msg, wParam, lParam);

    return S_OK;
}

STDMETHODIMP ActiveXSite::GetWindow(HWND *p)
{
    *p = HANDLETOHWND(GetHandle());
    return S_OK;
}

STDMETHODIMP ActiveXSite::ContextSensitiveHelp(BOOL)
{
    return E_NOTIMPL;
}

// IOleInPlaceSite methods|

STDMETHODIMP ActiveXSite::CanInPlaceActivate()
{
    if (!_owner || !_owner->IsVisible())
    {
        return S_FALSE;
    }
    else
    {
        return S_OK;
    }
}

STDMETHODIMP ActiveXSite::OnInPlaceActivate()
{
    BOOL pfNoRedraw = FALSE;
    return OnInPlaceActivateEx(&pfNoRedraw, 0);
}

STDMETHODIMP ActiveXSite::OnUIActivate()
{
    return S_OK;
}

STDMETHODIMP ActiveXSite::
GetWindowContext(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow **ppDoc,
                 LPRECT r1, LPRECT r2, LPOLEINPLACEFRAMEINFO o)
{
    // 
    // 由容器接口对象实现
    //
    if (_container)
    {
        _container->QueryInterface(IID_IOleInPlaceFrame, (void**)ppFrame);
    }

    *ppDoc = NULL;

    *r1 = _rect;
    *r2 = _rect;

    o->cb = sizeof(OLEINPLACEFRAMEINFO);
    o->fMDIApp = false;
    o->hwndFrame = GetParent(HANDLETOHWND(GetHandle()));
    o->haccel = 0;
    o->cAccelEntries = 0;

    return S_OK;
}

STDMETHODIMP ActiveXSite::Scroll(SIZE s)
{
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXSite::OnUIDeactivate(int)
{
    return S_OK;
}

STDMETHODIMP ActiveXSite::OnInPlaceDeactivate()
{
    return S_OK;
}

STDMETHODIMP ActiveXSite::DiscardUndoState()
{
    return S_OK;
}

STDMETHODIMP ActiveXSite::DeactivateAndUndo()
{
    return S_OK;
}

STDMETHODIMP ActiveXSite::OnPosRectChange(LPCRECT)
{
    return S_OK;
}

//
// IOleControlSite methods: 
//

STDMETHODIMP ActiveXSite::OnControlInfoChanged(void)      
{
    return S_OK;
}

STDMETHODIMP ActiveXSite::LockInPlaceActive(BOOL fLock)       
{
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXSite::GetExtendedControl(IDispatch** ppDisp)        
{
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXSite::TransformCoords(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags)       
{
    return S_OK;
}

STDMETHODIMP ActiveXSite::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
{
    return S_FALSE;
}

STDMETHODIMP ActiveXSite::OnFocus(BOOL fGotFocus)
{
    return S_OK;
}

STDMETHODIMP ActiveXSite::ShowPropertyFrame(void)
{
    return E_NOTIMPL;
}

// IOleInPlaceSiteEx
STDMETHODIMP ActiveXSite::OnInPlaceActivateEx(BOOL* pfNoRedraw, DWORD dwFlags)        
{
    if (!GetOleObject())
    {
        return S_FALSE;
    }

    _bInPlaced = true;

    ::OleLockRunning(GetOleObject(), TRUE, FALSE);
    
    HRESULT hr = E_FAIL;

    if ((dwFlags & ACTIVATE_WINDOWLESS) != 0) 
    {
        _bWindowless = true;
        hr = _oleObj->QueryInterface(IID_IOleInPlaceObjectWindowless, (LPVOID*)&_inPlaceObj);
    }

    if (FAILED(hr)) 
    {
        _bWindowless = false;
        _oleObj->QueryInterface(IID_IOleInPlaceObject, (LPVOID*)&_inPlaceObj);
    }

    if (_inPlaceObj != NULL) 
    {
        _inPlaceObj->SetObjectRects(&_rect, &_rect);
    }

    GetOleObject()->SetClientSite(this);

    return S_OK;
}

STDMETHODIMP ActiveXSite::OnInPlaceDeactivateEx(BOOL fNoRedraw)       
{
    if (_inPlaceObj != NULL) 
    {
        _inPlaceObj->Release();
        _inPlaceObj = NULL;
    }

    _bInPlaced = false;

    return S_OK;
}

STDMETHODIMP ActiveXSite::RequestUIActivate(void)
{
    return S_OK;
}

void ActiveXSite::OnRender(HDC hdc)
{
    RECTL rectl;

    rectl.left = _rect.left;
    rectl.top = _rect.top;
    rectl.right = _rect.right;
    rectl.bottom = _rect.bottom;
    /*rectl.left = 0;
    rectl.top = 0;
    rectl.right = _rect.Width();
    rectl.bottom = _rect.Height();*/

    GetViewObject()->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hdc, (RECTL*)&rectl, (RECTL*)&rectl, NULL, NULL); 
}

BOOL ActiveXSite::SetControlRect(LPRECT lprc)
{
    BOOL bRet = FALSE;

    // 
    // 尚未加载实际控件操作,直接写入_rect
    //
    if (!_bInPlaced)
    {
        _rect = *lprc;
    }
    //
    // 已经激活控件,通过IOleInPlaceObject::SetObjectRects调整控件区域
    //
    else
    {
        SIZEL size;

        size.cx = RECTWIDTH(*lprc);
        size.cy = RECTHEIGHT(*lprc);

        GetOleObject()->SetExtent(DVASPECT_CONTENT, &size);
        GetInPlaceObject()->SetObjectRects(lprc, lprc);

        _rect = *lprc;
    }

    bRet = TRUE;

    return bRet;
}

void ActiveXSite::InitWrapper(IOleObject* oleObj)
{
    _oleObj = oleObj;

    HRESULT hr = _oleObj->QueryInterface(IID_IViewObjectEx, (LPVOID*)&_viewObject);

    if (FAILED(hr)) 
    {
        hr = _oleObj->QueryInterface(IID_IViewObject2, (LPVOID*)_viewObject);
    }

    if (FAILED(hr)) 
    {
        hr = _oleObj->QueryInterface(IID_IViewObject, (LPVOID*)_viewObject);
    }
}
