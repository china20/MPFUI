// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// ActiveXContainer.cpp

#include "stdafx.h"

#include <Extend/ActiveX/ActiveXContainer.h>
#include <System/Windows/CoreTool.h>
#include <System/Tools/HwndHelper.h>

#include <comutil.h>
#pragma comment(lib,"comsuppw.lib")

ActiveXContainer::ActiveXContainer(IOleObject* oleObj)
    : _refNum(0)
    , _oleObj(oleObj)
    , _inPlaceActiveObj(NULL)
    , _owner(0)
{
}

ActiveXContainer::~ActiveXContainer(void)
{
}

suic::Handle ActiveXContainer::GetHandle() const
{
    return suic::HwndHelper::GetHostHwnd(_owner);
}

STDMETHODIMP ActiveXContainer::QueryInterface(REFIID iid,void**ppvObject)
{
    *ppvObject = 0;

    if (iid == IID_IOleClientSite)
    {
        *ppvObject = (IOleClientSite*)this;
    }
    else if (iid == IID_IUnknown)
    {
        *ppvObject = this;
    }
    else if (iid == IID_IDispatch)
    {
        *ppvObject = (IDispatch*)this;
    }
    else if (iid == IID_IOleInPlaceFrame)
    {
        *ppvObject = (IOleInPlaceFrame*)this;
    }
    else if (iid == IID_IOleInPlaceUIWindow)
    {
        *ppvObject = (IOleInPlaceUIWindow*)this;
    }
    else if (iid == IID_IOleContainer)             
    {
        *ppvObject = static_cast<IOleContainer*>(this);
    }

    if (*ppvObject)
    {
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) ActiveXContainer::AddRef()
{
    return ::InterlockedIncrement(&_refNum);
}

STDMETHODIMP_(ULONG) ActiveXContainer::Release()
{
    ::InterlockedDecrement(&_refNum);

    if (_refNum == 0)
    {
        delete this;
    }

    return S_OK;
}

// IOleInPlaceFrame methods

STDMETHODIMP ActiveXContainer::GetWindow(HWND *p)
{
    *p = HANDLETOHWND(GetHandle());
    return S_OK;
}

STDMETHODIMP ActiveXContainer::ContextSensitiveHelp(BOOL)
{
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXContainer::GetBorder(LPRECT l)
{
    l->left = 0;
    l->top = 0;
    l->right = 0;
    l->bottom = 0;

    return S_OK;
}

STDMETHODIMP ActiveXContainer::RequestBorderSpace(LPCBORDERWIDTHS b)
{
    return S_OK;
}

STDMETHODIMP ActiveXContainer::SetBorderSpace(LPCBORDERWIDTHS b)
{
    return S_OK;
}

STDMETHODIMP ActiveXContainer::SetActiveObject(IOleInPlaceActiveObject*pV,LPCOLESTR s)
{
    if (pV != NULL) 
    {
        pV->AddRef();
    }

    if (_inPlaceActiveObj != NULL) 
    {
        _inPlaceActiveObj->Release();
    }

    _inPlaceActiveObj = pV;

    return S_OK;
}

STDMETHODIMP ActiveXContainer::SetStatusText(LPCOLESTR t)
{
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXContainer::EnableModeless(BOOL f)
{
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXContainer::TranslateAccelerator(LPMSG,WORD)
{
    return E_NOTIMPL;
}

HRESULT _stdcall ActiveXContainer::RemoveMenus(HMENU h)
{
    return E_NOTIMPL;
}

HRESULT _stdcall ActiveXContainer::InsertMenus(HMENU h,LPOLEMENUGROUPWIDTHS x)
{
    return E_NOTIMPL;
}

HRESULT _stdcall ActiveXContainer::SetMenu(HMENU h,HOLEMENU hO,HWND hw)
{
    return E_NOTIMPL;
}

// IOleContainer
STDMETHODIMP ActiveXContainer::EnumObjects(DWORD grfFlags, IEnumUnknown** ppenum)
{
    if (ppenum == NULL) 
    {
        return E_POINTER;
    }
    else
    {
        *ppenum = new EnumUnknown(_oleObj);
        return S_OK;
    }
}

STDMETHODIMP ActiveXContainer::LockContainer(BOOL fLock)
{
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXContainer::
ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut)
{
    if (ppmkOut != NULL)
    {
        *ppmkOut = NULL;
    }
    return E_NOTIMPL;
}
