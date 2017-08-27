
#include "DataObject.h"
#include <ShlObj.h>

DataStoreEntry::DataStoreEntry()
    : _aspect(DVASPECT::DVASPECT_CONTENT)
{
    _data = NULL;
}

DataStoreEntry::DataStoreEntry(suic::Object* data, bool autoConvert, DVASPECT aspect, int index)
: _aspect(aspect)
, _autoConvert(autoConvert)
, _index(index)
{
    _data = data;
    if (NULL != _data)
    {
        _data->ref();
    }
}


DataStore::DataStore()
{
    for (int i = 0; i < 8; ++i)
    {
        _data[i].SetData(NULL);
        _data[i].SetFormat("Object");
    }
}

DataStore::DataStore(suic::Object* data)
{
    for (int i = 0; i < 8; ++i)
    {
        _data[i].SetData(NULL);
        _data[i].SetFormat("Object");
    }

    _data[0].SetData(data);
}

DataStore::~DataStore()
{

}

DataStoreEntry* DataStore::FindDataStoreEntry(suic::String format, DVASPECT aspect, int index)
{
    for (int i = 0; i < 8; ++i)
    {
        if (_data[i].GetFormat().Equals(format))
        {
            return &(_data[i]);
        }
    }

    return NULL;

    /*DataStoreEntry[] entryArray = (DataStoreEntry[]) _data[format];
    DataStoreEntry entry2 = null;
    DataStoreEntry entry3 = null;
    if (entryArray != null)
    {
        for (int i = 0; i < entryArray.Length; i++)
        {
            DataStoreEntry entry = entryArray[i];
            if ((entry.Aspect == aspect) && ((index == -1) || (entry.Index == index)))
            {
                entry2 = entry;
                break;
            }
            if ((entry.Aspect == DVASPECT::DVASPECT_CONTENT) && (entry.Index == 0))
            {
                entry3 = entry;
            }
        }
    }
    if ((entry2 == NULL) && (entry3 != NULL))
    {
        entry2 = entry3;
    }
    return entry2;*/
}

suic::Object* DataStore::GetData(suic::String format)
{
    return GetData(format, true);
}

suic::Object* DataStore::GetData(suic::String format, bool autoConvert)
{
    return GetData(format, autoConvert, DVASPECT::DVASPECT_CONTENT, -1);
}

int DataStore::GetFormatsCount()
{
    return 1;
}

suic::String DataStore::GetFormats(int index, bool autoConvert)
{
    return "Object";
}

suic::Object* DataStore::GetData(suic::String format, bool autoConvert, DVASPECT aspect, int index)
{
    DataStoreEntry* dataStoreEntry = FindDataStoreEntry(format, aspect, index);
    if (NULL != dataStoreEntry)
    {
        return dataStoreEntry->GetData();
    }
    else
    {
        return NULL;
    }

    /*DataStoreEntry* dataStoreEntry = FindDataStoreEntry(format, aspect, index);
    suic::Object* dataFromDataStoreEntry = GetDataFromDataStoreEntry(dataStoreEntry, format);
    suic::Object* obj3 = dataFromDataStoreEntry;
    if ((autoConvert && ((dataStoreEntry == NULL) || dataStoreEntry->GetAutoConvert)) && ((dataFromDataStoreEntry == null) || (dataFromDataStoreEntry is MemoryStream)))
    {
        string[] mappedFormats = DataObject.GetMappedFormats(format);
        if (mappedFormats != null)
        {
            for (int i = 0; i < mappedFormats.Length; i++)
            {
                if (!DataObject.IsFormatEqual(format, mappedFormats[i]))
                {
                    DataStoreEntry entry2 = this.FindDataStoreEntry(mappedFormats[i], aspect, index);
                    dataFromDataStoreEntry = this.GetDataFromDataStoreEntry(entry2, mappedFormats[i]);
                    if ((dataFromDataStoreEntry != null) && !(dataFromDataStoreEntry is MemoryStream))
                    {
                        if (DataObject.IsDataSystemBitmapSource(dataFromDataStoreEntry) || AssemblyHelper.IsBitmap(dataFromDataStoreEntry))
                        {
                            dataFromDataStoreEntry = DataObject.EnsureBitmapDataFromFormat(format, autoConvert, dataFromDataStoreEntry);
                        }
                        obj3 = null;
                        break;
                    }
                }
            }
        }
    }
    if (obj3 != null)
    {
        return obj3;
    }
    return dataFromDataStoreEntry;*/
}

suic::Object* DataStore::GetDataFromDataStoreEntry(DataStoreEntry* dataStoreEntry, suic::String format)
{
    suic::Object* data = NULL;
    if (dataStoreEntry != NULL)
    {
        data = dataStoreEntry->GetData();
    }
    return data;
}

bool DataStore::GetDataPresent(suic::String format)
{
    return GetDataPresent(format, true);
}

bool DataStore::GetDataPresent(suic::String format, bool autoConvert)
{
    return GetDataPresent(format, autoConvert, DVASPECT::DVASPECT_CONTENT, -1);
}

bool DataStore::GetDataPresent(suic::String format, bool autoConvert, DVASPECT aspect, int index)
{
    return false;
    /*if (autoConvert)
    {
        string[] formats = this.GetFormats(autoConvert);
        for (int j = 0; j < formats.Length; j++)
        {
            if (DataObject.IsFormatEqual(format, formats[j]))
            {
                return true;
            }
        }
        return false;
    }
    if (!this._data.ContainsKey(format))
    {
        return false;
    }
    DataStoreEntry[] entryArray = (DataStoreEntry[]) this._data[format];
    DataStoreEntry entry2 = null;
    DataStoreEntry entry3 = null;
    for (int i = 0; i < entryArray.Length; i++)
    {
        DataStoreEntry entry = entryArray[i];
        if ((entry.Aspect == aspect) && ((index == -1) || (entry.Index == index)))
        {
            entry2 = entry;
            break;
        }
        if ((entry.Aspect == DVASPECT.DVASPECT_CONTENT) && (entry.Index == 0))
        {
            entry3 = entry;
        }
    }
    if ((entry2 == null) && (entry3 != null))
    {
        entry2 = entry3;
    }
    return (entry2 != null);*/
}

void DataStore::SetData(suic::Object* data)
{
    /*if ((data is ISerializable))
    {
        SetData(DataFormats.Serializable, data);
    }*/
    SetData("Object", data);
}

void DataStore::SetData(suic::String format, suic::Object* data)
{
    SetData(format, data, true);
}

void DataStore::SetData(suic::String format, suic::Object* data, bool autoConvert)
{
    /*if ((DataObject.IsFormatEqual(format, DataFormats.Dib) && autoConvert) && (AssemblyHelper.IsBitmap(data) || DataObject.IsDataSystemBitmapSource(data)))
    {
        format = DataFormats.Bitmap;
    }*/
    SetData(format, data, autoConvert, DVASPECT::DVASPECT_CONTENT, 0);
}

void DataStore::SetData(suic::String format, suic::Object* data, bool autoConvert, DVASPECT aspect, int index)
{
    DataStoreEntry* dataStoreEntry = FindDataStoreEntry(format, aspect, index);
    if (NULL != dataStoreEntry)
    {
        dataStoreEntry->SetData(data);
    }

    /*DataStoreEntry entry = new DataStoreEntry(data, autoConvert, aspect, index);
    DataStoreEntry[] entryArray = (DataStoreEntry[]) this._data[format];
    if (entryArray == null)
    {
        entryArray = (DataStoreEntry[]) Array.CreateInstance(typeof(DataStoreEntry), 1);
    }
    else
    {
        DataStoreEntry[] array = (DataStoreEntry[]) Array.CreateInstance(typeof(DataStoreEntry), (int) (entryArray.Length + 1));
        entryArray.CopyTo(array, 1);
        entryArray = array;
    }
    entryArray[0] = entry;
    _data[format] = entryArray;*/
}

//==========================================================
//

DataObject::DataObject()
    : _dataStore(NULL)
{

}

DataObject::~DataObject(void)  
{  
    _refCount = 0;  
  
    /*int nSize = (int)m_dataStorageCL.size();  
    for (int i = 0; i < nSize; ++i)  
    {  
        DATASTORAGE_t dataEntry = m_dataStorageCL.at(i);  
        ReleaseStgMedium(dataEntry.m_stgMedium);  
        SAFE_DELETE(dataEntry.m_stgMedium);  
        SAFE_DELETE(dataEntry.m_formatEtc);  
    }*/ 

    if (_dataStore != NULL)
    {
        _dataStore->unref();
    }
}

suic::IDataStore* DataObject::GetDataStore() const
{
    return _dataStore;
}

void DataObject::SetDataStore(suic::IDataStore* dataStore)
{
    SETREFOBJ(_dataStore, dataStore);
}

STDMETHODIMP DataObject::QueryInterface(REFIID riid, void **ppv)
{
    if (IsEqualIID(riid, IID_IUnknown))
    {
        *ppv = (IUnknown *)this;
    }
    else if (IsEqualIID(riid, IID_IDataObject))
    {
        *ppv = (IDataObject*)this;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return NOERROR;
}

STDMETHODIMP_(ULONG) DataObject::AddRef()
{
    return InterlockedIncrement(&_refCount);  
}

STDMETHODIMP_(ULONG) DataObject::Release()
{
    ULONG lRef = InterlockedDecrement(&_refCount);  
    if (0 == lRef)  
    {  
        delete this;  
    }
    return _refCount; 
}

STDMETHODIMP DataObject::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
{
    if ((NULL == pformatetcIn) || (NULL == pmedium))  
    {  
        return E_INVALIDARG;  
    }  

    pmedium->hGlobal = NULL;  

    /*int nSize = (int)m_dataStorageCL.size();

    for (int i = 0; i < nSize; ++i)  
    {
        DATASTORAGE_t dataEntry = m_dataStorageCL.at(i);  
        if ((pformatetcIn->tymed & dataEntry.m_formatEtc->tymed) &&  
            (pformatetcIn->dwAspect == dataEntry.m_formatEtc->dwAspect) &&  
            (pformatetcIn->cfFormat == dataEntry.m_formatEtc->cfFormat))  
        {
            return CopyMedium(pmedium, dataEntry.m_stgMedium, dataEntry.m_formatEtc);  
        }
    }*/

    /*
    if (this._innerData is OleConverter)
    {
    ((OleConverter) this._innerData).OleDataObject.GetData(ref formatetc, out medium);
    }
    else
    {
    int hr = -2147221399;
    medium = new STGMEDIUM();
    if (this.GetTymedUseable(formatetc.tymed))
    {
    if ((formatetc.tymed & TYMED.TYMED_HGLOBAL) != TYMED.TYMED_NULL)
    {
    medium.tymed = TYMED.TYMED_HGLOBAL;
    medium.unionmember = Win32GlobalAlloc(0x2042, (IntPtr) 1);
    hr = this.OleGetDataUnrestricted(ref formatetc, ref medium, false);
    if (NativeMethods.Failed(hr))
    {
    Win32GlobalFree(new HandleRef(this, medium.unionmember));
    }
    }
    else if ((formatetc.tymed & TYMED.TYMED_ISTREAM) != TYMED.TYMED_NULL)
    {
    if (SecurityHelper.CheckUnmanagedCodePermission())
    {
    medium.tymed = TYMED.TYMED_ISTREAM;
    IStream istream = null;
    hr = Win32CreateStreamOnHGlobal(IntPtr.Zero, true, ref istream);
    if (NativeMethods.Succeeded(hr))
    {
    medium.unionmember = Marshal.GetComInterfaceForObject(istream, typeof(IStream));
    Marshal.ReleaseComObject(istream);
    hr = this.OleGetDataUnrestricted(ref formatetc, ref medium, false);
    if (NativeMethods.Failed(hr))
    {
    Marshal.Release(medium.unionmember);
    }
    }
    }
    else
    {
    hr = -2147467259;
    }
    }
    else
    {
    medium.tymed = formatetc.tymed;
    hr = this.OleGetDataUnrestricted(ref formatetc, ref medium, false);
    }
    }
    if (NativeMethods.Failed(hr))
    {
    medium.unionmember = IntPtr.Zero;
    Marshal.ThrowExceptionForHR(hr);
    }
    }
    */

    return DV_E_FORMATETC;  
}

STDMETHODIMP DataObject::SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease)  
{
    if ((NULL == pformatetc) || (NULL == pmedium))  
    {  
        return E_INVALIDARG;  
    }  
  
    if (pformatetc->tymed != pmedium->tymed)  
    {  
        return E_FAIL;  
    }  
  
    FORMATETC* fetc = new FORMATETC;  
    STGMEDIUM* pStgMed = new STGMEDIUM;  
    ZeroMemory(fetc, sizeof(FORMATETC));  
    ZeroMemory(pStgMed, sizeof(STGMEDIUM));  
  
    *fetc = *pformatetc;  
  
    if (fRelease)  
    {  
        *pStgMed = *pmedium;  
    }  
    else  
    {  
        CopyMedium(pStgMed, pmedium, pformatetc);  
    }
  
    //DATASTORAGE_t dataEntry = { fetc, pStgMed };  
    //m_dataStorageCL.push_back(dataEntry);  

    /*
    SecurityHelper.DemandAllClipboardPermission();
    if (this._innerData is OleConverter)
    {
    ((OleConverter) this._innerData).OleDataObject.SetData(ref pFormatetcIn, ref pmedium, fRelease);
    }
    else
    {
    Marshal.ThrowExceptionForHR(-2147467263);
    }

    */
  
    return S_OK;  
}

STDMETHODIMP DataObject::GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium)
{
    return S_OK;
}

STDMETHODIMP DataObject::QueryGetData(FORMATETC *pformatetc)
{
    if (NULL == pformatetc)  
    {  
        return E_INVALIDARG;  
    }

    if (!(DVASPECT_CONTENT & pformatetc->dwAspect))  
    {  
        return DV_E_DVASPECT;  
    }

    HRESULT hr = DV_E_TYMED;  
    /*int nSize = m_dataStorageCL.size();

    for (int i = 0; i < nSize; ++i)  
    {  
        DATASTORAGE_t dataEnrty = m_dataStorageCL.at(i);

        if (dataEnrty.m_formatEtc->tymed & pformatetc->tymed)  
        {  
            if (dataEnrty.m_formatEtc->cfFormat == pformatetc->cfFormat)  
            {  
                return S_OK;  
            }  
            else  
            {  
                hr = DV_E_CLIPFORMAT;  
            }  
        }  
        else  
        {  
            hr = DV_E_TYMED;  
        }  
    }  */

    /*
    if (this._innerData is OleConverter)
    {
    return ((OleConverter) this._innerData).OleDataObject.QueryGetData(ref formatetc);
    }
    if (formatetc.dwAspect != DVASPECT.DVASPECT_CONTENT)
    {
    return -2147221397;
    }
    if (!this.GetTymedUseable(formatetc.tymed))
    {
    return -2147221399;
    }
    if (formatetc.cfFormat == 0)
    {
    return 1;
    }
    if (this.GetDataPresent(DataFormats.GetDataFormat(formatetc.cfFormat).Name))
    {
    return 0;
    }
    return -2147221404;

    */
    return hr;
}

STDMETHODIMP DataObject::GetCanonicalFormatEtc(FORMATETC *pformatetcIn, FORMATETC *pformatetcOut)
{
    return S_OK;
}

STDMETHODIMP DataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc)
{
    if (NULL == ppenumFormatEtc)  
    {  
        return E_INVALIDARG;  
    }  

    *ppenumFormatEtc = NULL;  
    HRESULT hr = E_NOTIMPL;

    if (DATADIR_GET == dwDirection)  
    {  
        FORMATETC rgfmtetc[] =  
        {  
            {CF_UNICODETEXT, NULL, DVASPECT_CONTENT, 0, TYMED_HGLOBAL},  
        };

        hr = SHCreateStdEnumFmtEtc(ARRAYSIZE(rgfmtetc), rgfmtetc, ppenumFormatEtc);
    }

    return hr;
}

STDMETHODIMP DataObject::DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSnk, DWORD *pdwConnection)
{
    UNREFERENCED_PARAMETER(pformatetc);  
    UNREFERENCED_PARAMETER(advf);  
    UNREFERENCED_PARAMETER(pAdvSnk);  
    UNREFERENCED_PARAMETER(pdwConnection);  
    return E_NOTIMPL;
}

STDMETHODIMP DataObject::DUnadvise(DWORD dwConnection)
{
    UNREFERENCED_PARAMETER(dwConnection);  
    return E_NOTIMPL;  
}

STDMETHODIMP DataObject::EnumDAdvise(IEnumSTATDATA **ppenumAdvise)
{
    UNREFERENCED_PARAMETER(ppenumAdvise);  
    return E_NOTIMPL;  
}

HRESULT DataObject::CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc)  
{  
    if ((NULL == pMedDest) || (NULL == pMedSrc) || (NULL == pFmtSrc))  
    {  
        return E_INVALIDARG;  
    }

    switch (pMedSrc->tymed)  
    {  
    case TYMED_HGLOBAL:  
        pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal, pFmtSrc->cfFormat, NULL);  
        break;  
    case TYMED_GDI:  
        pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap, pFmtSrc->cfFormat, NULL);  
        break;  
    case TYMED_MFPICT:  
        pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict, pFmtSrc->cfFormat, NULL);  
        break;  
    case TYMED_ENHMF:  
        pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile, pFmtSrc->cfFormat, NULL);  
        break;  
    case TYMED_FILE:  
        pMedSrc->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName, pFmtSrc->cfFormat, NULL);  
        break;  
    case TYMED_ISTREAM:  
        pMedDest->pstm = pMedSrc->pstm;  
        pMedSrc->pstm->AddRef();  
        break;  
    case TYMED_ISTORAGE:  
        pMedDest->pstg = pMedSrc->pstg;  
        pMedSrc->pstg->AddRef();  
        break;  
    case TYMED_NULL:  
    default:  
        break;  
    }  

    pMedDest->tymed = pMedSrc->tymed;  
    pMedDest->pUnkForRelease = NULL;

    if (pMedSrc->pUnkForRelease != NULL)  
    {  
        pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;  
        pMedSrc->pUnkForRelease->AddRef();  
    }

    return S_OK;  
}

HRESULT DataObject::SetBlob(CLIPFORMAT cf, const void *pvBlob, UINT cbBlob)  
{  
    void *pv = GlobalAlloc(GPTR, cbBlob);  
    HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;  

    if (SUCCEEDED(hr))  
    {  
        CopyMemory(pv, pvBlob, cbBlob);  
        FORMATETC fmte = {cf, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};  
        // The STGMEDIUM structure is used to define how to handle a global memory transfer.  
        // This structure includes a flag, tymed, which indicates the medium  
        // to be used, and a union comprising pointers and a handle for getting whichever  
        // medium is specified in tymed.  
        STGMEDIUM medium = {};  
        medium.tymed = TYMED_HGLOBAL;  
        medium.hGlobal = pv;  
        hr = this->SetData(&fmte, &medium, TRUE);  

        if (FAILED(hr))  
        {  
            GlobalFree(pv);  
        }  
    }  

    return hr;  
}
