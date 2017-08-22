// 华勤科技版权所有 2010-2011
// 
// 文件名：TextHost20.h
// 功  能：实现ITextHost接口
// 
// 作  者：Sharpui开发组
// 时  间：2010-08-31
// 
// ============================================================================

#ifndef _RICHEDITOLECALLBACK_H_
#define _RICHEDITOLECALLBACK_H_

#include <Richedit.h>
#include <RichOle.h>
#include <textserv.h>
#include <System/Graphics/Bitmap.h>

class OleDataObj : public IDataObject
{
public:

    OleDataObj();

    // IUnknown接口
    virtual HRESULT WINAPI QueryInterface(REFIID iid, void ** ppvObject);
    virtual ULONG   WINAPI AddRef(void);
    virtual ULONG   WINAPI Release(void);

    // IDataObject接口
    HRESULT WINAPI GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
    HRESULT WINAPI GetDataHere(FORMATETC* pformatetc, STGMEDIUM*  pmedium);
    HRESULT WINAPI QueryGetData(FORMATETC*  pformatetc);
    HRESULT WINAPI GetCanonicalFormatEtc(FORMATETC*  pformatectIn, FORMATETC* pformatetcOut);
    HRESULT WINAPI SetData(FORMATETC* pformatetc, STGMEDIUM*  pmedium, BOOL  fRelease);
    HRESULT WINAPI EnumFormatEtc(DWORD  dwDirection , IEnumFORMATETC** ppenumFormatEtc);
    HRESULT WINAPI DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);
    HRESULT WINAPI DUnadvise(DWORD dwConnection);
    HRESULT WINAPI EnumDAdvise(IEnumSTATDATA **ppenumAdvise);

protected:

    ULONG _refCount;
};
//-------------------------------------------------------
//
class OleImage : public IOleObject, public IViewObject2
{
public:

    OleImage();
    ~OleImage();

    // IUnknown接口
    virtual HRESULT WINAPI QueryInterface(REFIID iid, void ** ppvObject);
    virtual ULONG   WINAPI AddRef(void);
    virtual ULONG   WINAPI Release(void);

    // IOleObject接口
    virtual HRESULT WINAPI SetClientSite(IOleClientSite *pClientSite);
    virtual HRESULT WINAPI GetClientSite(IOleClientSite **ppClientSite);
    virtual HRESULT WINAPI SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj);
    virtual HRESULT WINAPI Close(DWORD dwSaveOption);
    virtual HRESULT WINAPI SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk);
    virtual HRESULT WINAPI GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk);
    virtual HRESULT WINAPI InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved);
    virtual HRESULT WINAPI GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject);
    virtual HRESULT WINAPI DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect);
    virtual HRESULT WINAPI EnumVerbs(IEnumOLEVERB **ppEnumOleVerb);
    virtual HRESULT WINAPI Update(void);
    virtual HRESULT WINAPI IsUpToDate(void);
    virtual HRESULT WINAPI GetUserClassID(CLSID *pClsid);
    virtual HRESULT WINAPI GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType);
    virtual HRESULT WINAPI SetExtent(DWORD dwDrawAspect, SIZEL *psizel);
    virtual HRESULT WINAPI GetExtent(DWORD dwDrawAspect, SIZEL *psizel);
    virtual HRESULT WINAPI Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection);
    virtual HRESULT WINAPI Unadvise(DWORD dwConnection);
    virtual HRESULT WINAPI EnumAdvise(IEnumSTATDATA **ppenumAdvise);
    virtual HRESULT WINAPI GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus);
    virtual HRESULT WINAPI SetColorScheme(LOGPALETTE *pLogpal);

    // IViewObject接口
    virtual HRESULT WINAPI Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd
        , HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds
        ,BOOL(WINAPI *pfnContinue)(ULONG_PTR dwContinue), ULONG_PTR dwContinue);

    virtual HRESULT WINAPI GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect
        , DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet);

    virtual HRESULT WINAPI Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze);
    virtual HRESULT WINAPI Unfreeze(DWORD dwFreeze);
    virtual HRESULT WINAPI SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink);
    virtual HRESULT WINAPI GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink);

    // IViewObject2接口
    virtual HRESULT WINAPI GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel);

    void DrawFrame(HDC hDC, LPRECT lpRect);

    BOOL LoadGif(const TCHAR* filename);
    BOOL LoadImage(const TCHAR* filename);

    void GetImageSize(LPSIZE lpSize);
    RECT GetDrawRect() const;

    UINT GetFrameCount();

    void Clear();

    BOOL CanAnimate();
    bool CheckElapse(int dif);
    void MoveNextFrame();

    static BOOL AddImage(IRichEditOle* pRichEditOle, const TCHAR* lpPath, DWORD dwUserData);

protected:

    int _width;
    int _height;
    int _elapse;
    bool _isClosed;

    RECT _drawRect;
    suic::Bitmap _image;

    ULONG _refCount;
    TCHAR _filePath[MAX_PATH];

    UINT _frameCount;
    UINT _frameIndex;
};

//=======================================================
//
class RichEditOleCallback : public IRichEditOleCallback
{
public:

    RichEditOleCallback();
    ~RichEditOleCallback();

    void SetTextService(ITextServices* txtSrv);

    virtual HRESULT STDMETHODCALLTYPE GetNewStorage(LPSTORAGE * lplpstg);
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT STDMETHODCALLTYPE GetInPlaceContext(LPOLEINPLACEFRAME FAR * lplpFrame,
        LPOLEINPLACEUIWINDOW FAR * lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo);
    virtual HRESULT STDMETHODCALLTYPE ShowContainerUI(BOOL fShow);
    virtual HRESULT STDMETHODCALLTYPE QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp);
    virtual HRESULT STDMETHODCALLTYPE DeleteObject(LPOLEOBJECT lpoleobj);
    virtual HRESULT STDMETHODCALLTYPE QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR * lpcfFormat,
        DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
    virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
    virtual HRESULT STDMETHODCALLTYPE GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj);
    virtual HRESULT STDMETHODCALLTYPE GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu);

protected:

    IStorage* _storage;
    int _storageCount;
    DWORD _refCount;

    OleDataObj* _oldDataObj;
    ITextServices* _textSrv;
};

inline void RichEditOleCallback::SetTextService(ITextServices* txtSrv)
{
    _textSrv = txtSrv;
    _textSrv->AddRef();
}

#endif
