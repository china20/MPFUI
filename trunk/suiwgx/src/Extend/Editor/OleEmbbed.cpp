// ======================================================================
//
// Copyright (c) 2008-2022 汪荣, Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// OleEmbbed.cpp

#include "stdafx.h"
#include <TOM.h>
#include "ReadStmOp.h"

#include <Extend/Editor/OleEmbbed.h>
#include <Extend/Editor/RichTextOleCallback.h>

#include <richedit/INC/richedit.h>
#include <richedit/src/textserv.h>

static suic::String MPFUIStmName = L"MPFUIStm";

static bool CreateNewStorageObject(IStorage** lpStg)
{
    const UINT flag = STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE;
    HRESULT lr = ::StgCreateDocfile(NULL, flag, 0 , lpStg);

    if (NOERROR == lr)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//--------------------------------------------------
// OleEmbbed

OleEmbbed::OleEmbbed()
{
	_refCount = 1;
    _isDirty = true;
}

OleEmbbed::~OleEmbbed()
{
}

HRESULT WINAPI OleEmbbed::QueryInterface(REFIID iid, void ** ppvObject)
{
	if (iid == IID_IUnknown || iid == IID_IOleObject)
	{
		*ppvObject = (IOleObject*)this;
		((IOleObject*)(*ppvObject))->AddRef();
		return S_OK;
	}
    else if (iid == IID_IOleCache)
    {
        *ppvObject = (IOleCache*)this;
		((IOleCache*)(*ppvObject))->AddRef();
		return S_OK;
    }
	else if (iid == IID_IViewObject || iid == IID_IViewObject2)
	{
		*ppvObject = (IViewObject2*)this;
		((IViewObject2*)(*ppvObject))->AddRef();
		return S_OK;
	}
    /*else if (iid == IID_IOleObjectCb)
    {
        *ppvObject = (IOleObjectCb*)this;
        ((IOleObjectCb *)(*ppvObject))->AddRef();
        return S_OK;
    }*/
    /*else if (iid == IID_IDataObject)
    {
        *ppvObject = (IDataObject*)this;
        ((IDataObject *)(*ppvObject))->AddRef();
        return S_OK;
    }*/
    else if (iid == IID_IPersistStorage)
    {
        *ppvObject = (IPersistStorage*)this;
        ((IPersistStorage *)(*ppvObject))->AddRef();
        return S_OK;
    }

	return E_NOINTERFACE;
}

ULONG WINAPI OleEmbbed::AddRef(void)
{
	_refCount++;
	return _refCount;
}

ULONG WINAPI OleEmbbed::Release(void)
{
	if (--_refCount == 0)
	{
		delete this;
		return 0;
	}

	return _refCount;
}

//---------------------------------------------------------
//
 HRESULT WINAPI OleEmbbed::GetClassID(CLSID *pClassID)
 {
     return GetUserClassID(pClassID);
 }

 // IOleCache
HRESULT WINAPI OleEmbbed::Cache(FORMATETC *pformatetc, DWORD advf,DWORD *pdwConnection)
{
    return S_OK;
}

HRESULT WINAPI OleEmbbed::Uncache(DWORD dwConnection)
{
    return S_OK;
}

HRESULT WINAPI OleEmbbed::EnumCache(IEnumSTATDATA **ppenumSTATDATA)
{
    return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::InitCache(IDataObject *pDataObject)
{
    return S_OK;
}

HRESULT WINAPI OleEmbbed::SetData(FORMATETC *pformatetc,STGMEDIUM *pmedium,BOOL fRelease)
{
    return S_OK;
}

 // IPersistStorage接口
 HRESULT WINAPI OleEmbbed::IsDirty(void)
 {
     return S_OK;
 }

 HRESULT WINAPI OleEmbbed::InitNew(IStorage *pStg)
 {
     return S_OK;
 }

 HRESULT WINAPI OleEmbbed::Load(IStorage *pStgSave)
 {
     //
     // 此函数从IStorage反系列化数据，重新构建我们自己的对象
     //
     StorageOp sOp(pStgSave);

     if (sOp.LocateReadStm(MPFUIStmName))
     {
         //
         // 此函数把我们自己对象的数据系列化到IStorage进行保存
         //
         if (LoadFromStream(&sOp))
         {
             return NOERROR;
         }
         else
         {
             return E_NOTIMPL;
         }
     }
     else
     {
         return E_NOTIMPL;
     }
 }

 HRESULT WINAPI OleEmbbed::Save(IStorage *pStgSave, BOOL fSameAsLoad)
 {
     /*if (!_isDirty)
     {
         return NOERROR;
     }*/

     StorageOp sOp(pStgSave);

     if (sOp.LocateWriteStm(MPFUIStmName))
     {
         //
         // 此函数把我们自己对象的数据系列化到IStorage进行保存
         //
         if (SaveToStream(&sOp))
         {
             return NOERROR;
         }
     }

     return E_NOTIMPL;
 }

 HRESULT WINAPI OleEmbbed::SaveCompleted(IStorage *pStgNew)
 {
     _isDirty = false;
     //
     // 简单返回NOERROR标记表示已经成功
     //
     return NOERROR;
 }

 HRESULT WINAPI OleEmbbed::HandsOffStorage(void)
 {
     return S_OK;
 }


//---------------------------------------------------------
//
HRESULT WINAPI OleEmbbed::SetClientSite(IOleClientSite *pClientSite)
{
	return S_OK;
}

HRESULT WINAPI OleEmbbed::GetClientSite(IOleClientSite **ppClientSite)
{
	return S_OK;
}

HRESULT WINAPI OleEmbbed::SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj)
{
	return S_OK;
}

HRESULT WINAPI OleEmbbed::Close(DWORD dwSaveOption)
{
	return S_OK;
}

HRESULT WINAPI OleEmbbed::SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved)
{
	return S_OK;
}

HRESULT WINAPI OleEmbbed::GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
{
	return S_OK;
}

HRESULT WINAPI OleEmbbed::EnumVerbs(IEnumOLEVERB **ppEnumOleVerb)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::Update(void)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::IsUpToDate(void)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::GetUserClassID(CLSID *pClsid)
{
	return S_OK;
}

HRESULT WINAPI OleEmbbed::GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::SetExtent(DWORD dwDrawAspect, SIZEL *lpsizel)
{
    return S_OK;
}

HRESULT WINAPI OleEmbbed::GetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::Unadvise(DWORD dwConnection)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::EnumAdvise(IEnumSTATDATA **ppenumAdvise)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::SetColorScheme(LOGPALETTE *pLogpal)
{
	return E_NOTIMPL;
}

static void __RenderBmp(HBITMAP hbmp, HDC hdc, suic::Rect rect)
{
    HDC hmem = ::CreateCompatibleDC(hdc);
    HBITMAP obmp = (HBITMAP)::SelectObject(hmem, hbmp);

    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

    int iMode = ::SetStretchBltMode(hdc, COLORONCOLOR);
    ::AlphaBlend(hdc, rect.left, rect.top, rect.Width(), rect.Height()
        , hmem, 0, 0, rect.Width(), rect.Height(), bf);

    ::SetStretchBltMode(hdc, iMode);
    ::SelectObject(hmem, obmp);
    ::DeleteObject(hbmp);

    ::DeleteDC(hmem);
}

HRESULT WINAPI OleEmbbed::Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
							HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
							BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue)
{
	if (lindex != -1)
    {
        return S_FALSE;
    }

    _rect.left = lprcBounds->left;
    _rect.top = lprcBounds->top;
    _rect.right = lprcBounds->right;
    _rect.bottom = lprcBounds->bottom;

    suic::Drawing* drawing = NULL;
    suic::RenderContext rCtx;
    suic::Rect rcDraw(0, 0, _rect.Width(), _rect.Height());

    drawing = rCtx.Open(_rect.Width(), _rect.Height(), 0);
    Render(drawing, rcDraw);

    HBITMAP hbmp = HANDLETOBITMAP(suic::Bitmap::ToHandle(rCtx.bitmap));

    // 绘制到Richedit
    __RenderBmp(hbmp, hdcDraw, _rect);

	return S_OK;
}

HRESULT WINAPI OleEmbbed::GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
								   DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::Unfreeze(DWORD dwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink)
{
    if (NULL != pAdvSink)
    {
        RichEditOleCallback* txtHost = NULL;
        pAdvSink->QueryInterface(IID_IRichEditOleCallback, (void**)&txtHost);
        if (NULL != txtHost)
        {
            SetHolder(txtHost->GetHolder());
            txtHost->Release();
        }
    }
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleEmbbed::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel)
{
	return S_OK;
}
