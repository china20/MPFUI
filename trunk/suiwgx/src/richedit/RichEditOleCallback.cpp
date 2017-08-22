// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////////
// RichEditOleCallback.cpp

#include "stdafx.h"
#include "RichEditOleCallback.h"
#include <TOM.h>

EXTERN_C const GUID IID_IImageOle = 
{ 0x13e671f4, 0x1a5a, 0x12cf, { 0xab, 0xdb, 0x0, 0xfa, 0x0, 0xe6, 0x5e, 0xa8 } };

//======================================================
//
OleImage::OleImage()
    : _image(false)
    , _isClosed(true)
    , _elapse(0)
{
	_refCount = 0;
	_frameCount = 0;
	_frameIndex = 0;

	ZeroMemory(_filePath, MAX_PATH * sizeof(TCHAR));
    ::SetRectEmpty(&_drawRect);
}

OleImage::~OleImage()
{
	Clear();
}

HRESULT WINAPI OleImage::QueryInterface(REFIID iid, void ** ppvObject)
{
	if (iid == IID_IUnknown || iid == IID_IOleObject)
	{
		*ppvObject = (IOleObject *)this;
		((IOleObject *)(*ppvObject))->AddRef();
		return S_OK;
	}
	else if (iid == IID_IViewObject || iid == IID_IViewObject2)
	{
		*ppvObject = (IViewObject2 *)this;
		((IViewObject2 *)(*ppvObject))->AddRef();
		return S_OK;
	}
    else if (iid == IID_IDataObject)
    {
        *ppvObject = (IDataObject *)this;
        ((IDataObject *)(*ppvObject))->AddRef();
        return S_OK;
    }

	return E_NOINTERFACE;
}

ULONG WINAPI OleImage::AddRef(void)
{
	_refCount++;
	return _refCount;
}

ULONG WINAPI OleImage::Release(void)
{
	if (--_refCount == 0)
	{
		delete this;
		return 0;
	}

	return _refCount;
}

HRESULT WINAPI OleImage::SetClientSite(IOleClientSite *pClientSite)
{
	return S_OK;
}

HRESULT WINAPI OleImage::GetClientSite(IOleClientSite **ppClientSite)
{
	return S_OK;
}

HRESULT WINAPI OleImage::SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj)
{
	return S_OK;
}

HRESULT WINAPI OleImage::Close(DWORD dwSaveOption)
{
    _isClosed = true;
	return S_OK;
}

HRESULT WINAPI OleImage::SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved)
{
	return S_OK;
}

HRESULT WINAPI OleImage::GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
{
	return S_OK;
}

HRESULT WINAPI OleImage::EnumVerbs(IEnumOLEVERB **ppEnumOleVerb)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::Update(void)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::IsUpToDate(void)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::GetUserClassID(CLSID *pClsid)
{
	*pClsid = IID_NULL;
	return S_OK;
}

HRESULT WINAPI OleImage::GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType)
{
    //static LPTSTR type = TEXT("Embedded Object");
    //int len = (_tcslen(type) + 1) * sizeof(TCHAR);

    //*pszUserType = (LPOLESTR)CoTaskMemAlloc(len);
    //wcscpy_s(*pszUserType, len, type);
    ////*pszUserType = "Embedded Object";
	return S_OK;
}

HRESULT WINAPI OleImage::SetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::GetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::Unadvise(DWORD dwConnection)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::EnumAdvise(IEnumSTATDATA **ppenumAdvise)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::SetColorScheme(LOGPALETTE *pLogpal)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
							HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
							BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue)
{
	if (lindex != -1)
    {
        return S_FALSE;
    }

	_drawRect.left = lprcBounds->left;
	_drawRect.top = lprcBounds->top;
	_drawRect.right = lprcBounds->right;
	_drawRect.bottom = lprcBounds->bottom;

	DrawFrame(hdcDraw, &_drawRect);

	return S_OK;
}

HRESULT WINAPI OleImage::GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
								   DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::Unfreeze(DWORD dwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
{
	return E_NOTIMPL;
}

HRESULT WINAPI OleImage::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel)
{
	SIZE size;
	GetImageSize(&size);

	HDC hDC = ::GetDC(NULL);
	lpsizel->cx = ::MulDiv(size.cx + 2, 2540, GetDeviceCaps(hDC, LOGPIXELSX));
	lpsizel->cy = ::MulDiv(size.cy + 2, 2540, GetDeviceCaps(hDC, LOGPIXELSY));
	::ReleaseDC(NULL, hDC);

	return S_OK;
}

//=================================================================
// OleDataObj

OleDataObj::OleDataObj()
{
    _refCount = 0;
}

HRESULT STDMETHODCALLTYPE 
OleDataObj::QueryInterface(REFIID iid, void** ppvObject)
{
    HRESULT hr = S_OK;
    *ppvObject = NULL;
    hr = E_NOINTERFACE;

    return hr;
}

ULONG STDMETHODCALLTYPE OleDataObj::AddRef()
{
    return ++_refCount;
}

ULONG STDMETHODCALLTYPE OleDataObj::Release()
{
    if (--_refCount == 0)
    {
        delete this ;
    }
    return _refCount;
}

HRESULT WINAPI OleDataObj::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
{
    return S_OK;
}

HRESULT WINAPI OleDataObj::GetDataHere(FORMATETC* pformatetc, STGMEDIUM*  pmedium)
{
    return E_NOTIMPL;
}

HRESULT WINAPI OleDataObj::QueryGetData(FORMATETC*  pformatetc)
{
    return E_NOTIMPL;
}

HRESULT WINAPI OleDataObj::GetCanonicalFormatEtc(FORMATETC*  pformatectIn, FORMATETC* pformatetcOut)
{
    return E_NOTIMPL;
}

HRESULT WINAPI OleDataObj::SetData(FORMATETC* pformatetc, STGMEDIUM*  pmedium, BOOL  fRelease)
{
    return S_OK;
}

HRESULT WINAPI OleDataObj::EnumFormatEtc(DWORD  dwDirection , IEnumFORMATETC** ppenumFormatEtc)
{
    return E_NOTIMPL;
}

HRESULT WINAPI OleDataObj::DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
    return E_NOTIMPL;
}

HRESULT WINAPI OleDataObj::DUnadvise(DWORD dwConnection)
{
    return E_NOTIMPL;
}

HRESULT WINAPI OleDataObj::EnumDAdvise(IEnumSTATDATA **ppenumAdvise)
{
    return E_NOTIMPL;
}
//=================================================================
//
BOOL OleImage::LoadGif(const TCHAR *filename)
{
	return FALSE;
}

BOOL OleImage::LoadImage(const TCHAR* filename)
{
    if (_image.Load(filename))
    {
        _tcscpy_s(_filePath, MAX_PATH, filename);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void OleImage::GetImageSize(LPSIZE lpSize)
{
	if (lpSize == NULL)
    {
        return;
    }

    /*if (_frames.GetCount() > 0)
    {
        lpSize->cx = _width;
	    lpSize->cy = _height;
    }
    else
    {
        lpSize->cx = _image.Width();
        lpSize->cy = _image.Height();
    }*/
}

RECT OleImage::GetDrawRect() const
{
    return _drawRect;
}

UINT OleImage::GetFrameCount()
{
	return 0;//_frames.GetCount();
}

void OleImage::Clear()
{ 
    //_frames.Clear();
	ZeroMemory(_filePath, MAX_PATH * sizeof(TCHAR));
	_frameCount = 0;
	_frameIndex = 0;
}

BOOL OleImage::CanAnimate()
{
	return _frameCount > 1;
}

bool OleImage::CheckElapse(int dif)
{
    _elapse -= dif;
    return (_elapse <= 5);
}

void OleImage::MoveNextFrame()
{
    _frameIndex = (++_frameIndex) % _frameCount;
}

void OleImage::DrawFrame(HDC hdc, LPRECT lpRect)
{
    /*if (_image.IsValid())
    {
        suic::Rect rcDest(0, 0, _image.Width(), _image.Height());
        suic::Rect rect(lpRect);
        suic::DrawTools dibDraw(HdcToHandle(hdc), &_image);

        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        ::AlphaBlend(hdc, rect.left, rect.top, rect.Width(), rect.Height()
            , HandleToHdc(dibDraw), 0, 0, _image.Width(), _image.Height(), bf);
    }
    else if (_frames.GetCount() > 0)
    {
        suic::BitmapPtr img = _frames.GetFrame(_frameIndex)->img;
        suic::Rect rect(lpRect);
        suic::DrawTools dibDraw(img);
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

        if (_frameIndex > 0 && _frames.GetFrame(_frameIndex)->flag == 3)
        {
        }
        else
        {
            ::AlphaBlend(hdc, rect.left, rect.top, rect.Width(), rect.Height()
                , HandleToHdc(dibDraw), 0, 0, _width, _height, bf);
        }
    }*/
}

BOOL OleImage::AddImage(IRichEditOle* pRichEditOle, const TCHAR* lpPath, DWORD dwUserData)
{
    SCODE sc;
    suic::String filename(lpPath);
    IOleClientSite *pOleClientSite = NULL;
    pRichEditOle->GetClientSite(&pOleClientSite);
    IStorage *pStorage = NULL;
    LPLOCKBYTES lpLockBytes = NULL;

    UINT flag = STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE;

    filename.ToLower();

    sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);

    if (sc != S_OK)
    {
        return FALSE;
    }

    sc = ::StgCreateDocfileOnILockBytes(lpLockBytes, flag, 0, &pStorage);

    if (sc != S_OK)
    {
        lpLockBytes->Release();
        lpLockBytes = NULL;
        return FALSE;
    }

    OleImage *pOleImage = new OleImage();

    if (filename.IndexOf(_T(".gif")) >= 0)
    {
        pOleImage->LoadGif(lpPath);
    }
    else
    {
        pOleImage->LoadImage(lpPath);
    }

    IOleObject *pOleObject = NULL;
    pOleImage->QueryInterface(IID_IOleObject, (void**)&pOleObject);
    pOleObject->SetClientSite(pOleClientSite);

    HRESULT hr = ::OleSetContainedObject(pOleObject, TRUE);

    REOBJECT reobject;
    ZeroMemory(&reobject, sizeof(REOBJECT));
    reobject.cbStruct = sizeof(REOBJECT);

    reobject.clsid = IID_IImageOle;
    reobject.cp = REO_CP_SELECTION;
    reobject.dvaspect = DVASPECT_CONTENT;
    reobject.poleobj = pOleObject;
    reobject.polesite = pOleClientSite;
    reobject.pstg = pStorage;
    reobject.dwUser = dwUserData;
    reobject.dwFlags = REO_BELOWBASELINE;

    pRichEditOle->InsertObject(&reobject);

    pOleObject->Release();
    pOleClientSite->Release();
    pStorage->Release();

    return TRUE;
}

//============================================================
//
RichEditOleCallback::RichEditOleCallback()
    : _textSrv(NULL)
{
    _oldDataObj = new OleDataObj();
    _storage = NULL;
    _storageCount = 0 ;
    _refCount = 0 ;

    _oldDataObj->AddRef();

    const UINT flag = STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE /* | STGM_DELETEONRELEASE */ | STGM_CREATE;

    HRESULT hResult = ::StgCreateDocfile(NULL, flag, 0 , &_storage);
}

RichEditOleCallback::~RichEditOleCallback()
{
    _oldDataObj->Release();
}

HRESULT STDMETHODCALLTYPE 
RichEditOleCallback::GetNewStorage(LPSTORAGE * lplpstg)
{
    const UINT flag = STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE;

    _storageCount++ ;
    suic::String storageName;
    storageName.Format(_T("REOLEStorage%d"), _storageCount);
    HRESULT hResult = _storage ->CreateStorage(storageName.c_str(), flag, 0 , 0 , lplpstg);

    return hResult;
}

//EXTERN_C const IID IID_IRichEditOleExCallback = { 0x00020D03,0,0,{0xC0,0,0,0,0,0,0,0x46}};

HRESULT STDMETHODCALLTYPE 
RichEditOleCallback::QueryInterface(REFIID iid, void** ppvObject)
{
    HRESULT hr = S_OK;
    * ppvObject = NULL;

    /*if (iid == IID_IUnknown || iid == IID_IRichEditOleExCallback)
    {
        *ppvObject = this ;
        hr = NOERROR;

        AddRef();
    }
    else */
    {
        hr = E_NOINTERFACE;
    }

    return hr;
}

ULONG STDMETHODCALLTYPE RichEditOleCallback::AddRef()
{
    return ++ _refCount;
}

ULONG STDMETHODCALLTYPE RichEditOleCallback::Release()
{
    if (--_refCount == 0)
    {
        if (_textSrv)
        {
            _textSrv->Release();
            _textSrv = NULL;
        }

        delete this ;
        return 0 ;
    }
    return _refCount;
}

HRESULT STDMETHODCALLTYPE 
RichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR* lplpFrame,
    LPOLEINPLACEUIWINDOW FAR* lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleCallback::ShowContainerUI(BOOL fShow)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleCallback::
QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
    //OleImage* imgole;//(dynamic_cast<OleImage*>(lpoleobj));

    if (*lpclsid != IID_IImageOle)
    {
        return S_OK;
    }
    else
    {
        return S_OK;
    }
}

HRESULT STDMETHODCALLTYPE RichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
    /*OleImage* imgole(dynamic_cast<OleImage*>(lpoleobj));
    if (imgole)
    {
    }*/
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleCallback::
QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR * lpcfFormat
                , DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
    return E_NOTIMPL;
    //return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE 
RichEditOleCallback::GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj)
{
    return E_NOTIMPL;
    if (!_textSrv)
    {
        return S_OK;
    }

    IRichEditOle *pRichEditOle = NULL;
    _textSrv->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle, 0);

    if (pRichEditOle == NULL)
    {
        return S_OK;
    }
    
    HRESULT hr = S_OK;
    int nObjCount = pRichEditOle->GetObjectCount();
    for (int i = 0;i < nObjCount; i++)
    {
        bool bFind = false;
        suic::Rect rect;
        REOBJECT reo;
        ZeroMemory(&reo, sizeof(REOBJECT));
        reo.cbStruct = sizeof(REOBJECT);

        hr = pRichEditOle->GetObject(i, &reo, REO_GETOBJ_POLEOBJ);
        if (reo.cp >= lpchrg->cpMin && reo.cp <= lpchrg->cpMax)
        {
            OleImage* oleimg = dynamic_cast<OleImage*>(reo.poleobj);
            *lplpdataobj = (LPDATAOBJECT)oleimg;
            bFind = true;
        }

        if (reo.poleobj)
        {
            reo.poleobj->Release();
        }
        if (reo.pstg)
        {
            reo.pstg->Release();
        }
        if (reo.polesite)
        {
            reo.polesite->Release();
        }

        if (bFind)
        {
            break;
        }
    }

    pRichEditOle->Release();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleCallback::
GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleCallback::
GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR* lphmenu)
{
    return S_OK;
}