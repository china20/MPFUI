
#ifndef _UIDATAOBJECT_H_
#define _UIDATAOBJECT_H_

#include <System/Windows/DataHostObj.h>
#include <System/Windows/DpObject.h>
#include <System/Windows/DragEventArg.h>

class DataStore;

class DataObject : public IDataObject
{
public:

    DataObject();
    virtual ~DataObject();

    suic::IDataStore* GetDataStore() const;
    void SetDataStore(suic::IDataStore* dataStore);

    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);  
    IFACEMETHODIMP_(ULONG) AddRef(void);  
    IFACEMETHODIMP_(ULONG) Release(void);  

    IFACEMETHODIMP GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
    IFACEMETHODIMP SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease);
    IFACEMETHODIMP GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium);
    IFACEMETHODIMP QueryGetData(FORMATETC *pformatetc);
    IFACEMETHODIMP GetCanonicalFormatEtc(FORMATETC *pformatetcIn, FORMATETC *pformatetcOut);
    IFACEMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc);
    IFACEMETHODIMP DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSnk, DWORD *pdwConnection);
    IFACEMETHODIMP DUnadvise(DWORD dwConnection);
    IFACEMETHODIMP EnumDAdvise(IEnumSTATDATA **ppenumAdvise);

private:

    HRESULT CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc);
    HRESULT SetBlob(CLIPFORMAT cf, const void *pvBlob, UINT cbBlob);

private:

    volatile LONG _refCount;
    suic::IDataStore* _dataStore;
};

class DataStoreEntry
{
public:

    DataStoreEntry();
    DataStoreEntry(suic::Object* data, bool autoConvert, DVASPECT aspect, int index);

    void Clear()
    {
        FREEREFOBJ(_data);
    }

public:

    DVASPECT GetAspect() const
    {
        return _aspect;
    }

    bool GetAutoConvert() const
    {
        return _autoConvert;
    }

    suic::Object* GetData() const
    {
        return _data;
    }

    void SetData(suic::Object* data)
    {
        SETREFOBJ(_data, data);
    }

    int GetIndex() const
    {
        return _index;
    }

    suic::String GetFormat() const
    {
        return _format;
    }

    void SetFormat(suic::String val)
    {
        _format = val;
    }

private:

    DVASPECT _aspect;
    suic::Object* _data;
    suic::String _format;
    int _index;
    bool _autoConvert;
};

class DataStore : public suic::IDataStore
{
public:

    DataStore();
    DataStore(suic::Object* data);
    ~DataStore();

    suic::Object* GetData(suic::String format);
    suic::Object* GetData(suic::String format, bool autoConvert);

    int GetFormatsCount();
    suic::String GetFormats(int index, bool autoConvert);

    bool GetDataPresent(suic::String format);
    bool GetDataPresent(suic::String format, bool autoConvert);

    void SetData(suic::Object* data);
    void SetData(suic::String format, suic::Object* data);
    void SetData(suic::String format, suic::Object* data, bool autoConvert);

private:

    DataStoreEntry* FindDataStoreEntry(suic::String format, DVASPECT aspect, int index);
    suic::Object* GetData(suic::String format, bool autoConvert, DVASPECT aspect, int index);
    suic::Object* GetDataFromDataStoreEntry(DataStoreEntry* dataStoreEntry, suic::String format);
    bool GetDataPresent(suic::String format, bool autoConvert, DVASPECT aspect, int index);
    void SetData(suic::String format, suic::Object* data, bool autoConvert, DVASPECT aspect, int index);

private:

    DataStoreEntry _data[8];
};

class DataObjectStore : public suic::IDataStore
{
public:

    DataObjectStore(IDataObject* dataObj)
    {
        _dataObj = new DataHostObj(dataObj);
        _dataObj->ref();
    }

    ~DataObjectStore()
    {
        if (_dataObj)
        {
            _dataObj->unref();
        }
    }

    suic::Object* GetData(suic::String format)
    {
        return _dataObj;
    }
    suic::Object* GetData(suic::String format, bool autoConvert)
    {
        return _dataObj;
    }

    int GetFormatsCount()
    {
        return 1;
    }
    suic::String GetFormats(int index, bool autoConvert)
    {
        return _U("DataObj");
    }

    bool GetDataPresent(suic::String format)
    {
        return true;
    }
    bool GetDataPresent(suic::String format, bool autoConvert)
    {
        return true;
    }

    void SetData(suic::Object* data)
    {
        ;
    }
    void SetData(suic::String format, suic::Object* data)
    {
        ;
    }
    void SetData(suic::String format, suic::Object* data, bool autoConvert)
    {
        ;
    }


private:

    DataHostObj* _dataObj;
};

#endif
