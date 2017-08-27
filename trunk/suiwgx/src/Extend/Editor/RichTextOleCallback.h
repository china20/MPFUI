// 华勤科技版权所有 2008-2022
// 
// 文件名：TextHost20.h
// 功  能：实现ITextHost接口
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// ============================================================================

#ifndef _RICHEDITOLECALLBACK_H_
#define _RICHEDITOLECALLBACK_H_

#include <Richedit.h>
#include <RichOle.h>
#include <textservex.h>

#include <Extend/Editor/OleEmbbed.h>
#include <Extend/Editor/Embbed.h>

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

struct OleHeader
{
    DWORD cbSize;
    DWORD type;
    char typeName[64];

    OleHeader()
    {
        cbSize = sizeof(OleHeader);
        type = 0;
        memset(typeName, 0, sizeof(typeName));
    }
};

EXTERN_C const GUID CLSID_UIOleObject;

//-------------------------------------------------------
//
class UIOleObject : public OleEmbbed
{
public:

    UIOleObject(suic::Embbed* embbed);
    ~UIOleObject();

    suic::Embbed* GetEmbbedObj() const;

    HRESULT WINAPI GetUserClassID(CLSID *pClsid);

    static bool IsMPFUIClsid(const CLSID& clsid);
    static bool InsertOleObj(IRichEditOle* pRichEditOle, suic::Int32 cp, suic::Embbed* embbed);

protected:

    void MeasureEmbbed(LPSIZEL lpsizel);

    void SetHolder(suic::Object* holder);
    void Render(suic::Drawing* drawing, suic::Rect rect);
    bool SaveToStream(suic::ISStream* stm);
    bool LoadFromStream(suic::ISStream* stm);

    HRESULT WINAPI SetExtent(DWORD dwDrawAspect, SIZEL *lpsizel);
    HRESULT WINAPI GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel);

protected:

    SIZEL _desiredSize;
    IOleClientSite *_clientSite;
    // 用户数据接口(用于系列化和反系列化对象)
    suic::Embbed* _embbedObj;
};

inline void UIOleObject::SetHolder(suic::Object* holder)
{
    if (NULL != _embbedObj)
    {
        _embbedObj->SetHolder(holder);
    }
}

//=======================================================
//
class RichEditOleCallback : public IRichEditOleCallback
{
public:

    RichEditOleCallback();
    ~RichEditOleCallback();

    suic::WeakRef GetHolder() const
    {
        return _holder;
    }

    void SetTextService(ITextServices* txtSrv, suic::Object* holder);

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
    suic::WeakRef _holder;
};

inline void RichEditOleCallback::SetTextService(ITextServices* txtSrv, suic::Object* holder)
{
    _textSrv = txtSrv;
    _holder = holder;
}

#endif
