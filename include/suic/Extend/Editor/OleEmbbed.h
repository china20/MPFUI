// 华勤科技版权所有 2010-2011
// 
// 文件名：OleEmbbed.h
// 功  能：实现OleEmbbed接口
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// ============================================================================

#ifndef _UIOLEEMBBED_H_
#define _UIOLEEMBBED_H_

#include <Extend/Editor/Embbed.h>

//-------------------------------------------------------
//
class OleEmbbed : public IOleObject,
                  public IPersistStorage,
                  public IViewObject2,
                  public IOleCache
{
public:

    OleEmbbed();
    virtual ~OleEmbbed();

    suic::Rect GetRenderRect() const
    {
        return _rect;
    }

    // IUnknown接口
    virtual HRESULT WINAPI QueryInterface(REFIID iid, void ** ppvObject);
    virtual ULONG   WINAPI AddRef(void);
    virtual ULONG   WINAPI Release(void);

    // IPersist接口
    virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID *pClassID);

    // IOleCache
    virtual HRESULT STDMETHODCALLTYPE Cache(FORMATETC *pformatetc, DWORD advf,DWORD *pdwConnection);
    virtual HRESULT STDMETHODCALLTYPE Uncache(DWORD dwConnection);
    virtual HRESULT STDMETHODCALLTYPE EnumCache(IEnumSTATDATA **ppenumSTATDATA);
    virtual HRESULT STDMETHODCALLTYPE InitCache(IDataObject *pDataObject);
    virtual HRESULT STDMETHODCALLTYPE SetData(FORMATETC *pformatetc,STGMEDIUM *pmedium,BOOL fRelease);

    // IPersistStorage接口
    virtual HRESULT STDMETHODCALLTYPE IsDirty(void);
    virtual HRESULT STDMETHODCALLTYPE InitNew(IStorage *pStg);
    virtual HRESULT STDMETHODCALLTYPE Load(IStorage *pStg);
    virtual HRESULT STDMETHODCALLTYPE Save(IStorage *pStgSave, BOOL fSameAsLoad);
    virtual HRESULT STDMETHODCALLTYPE SaveCompleted(IStorage *pStgNew);
    virtual HRESULT STDMETHODCALLTYPE HandsOffStorage( void);

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

protected:

    virtual void SetHolder(suic::Object* holder) = 0;
    virtual void Render(suic::Drawing* drawing, suic::Rect rect) = 0;
    virtual bool SaveToStream(suic::ISStream* stm) = 0;
    virtual bool LoadFromStream(suic::ISStream* stm) = 0;

protected:

    LONG _refCount;
    // 对象是否需要重新读取
    bool _isDirty;
    // 绘制区域
    suic::Rect _rect;
};

#endif
