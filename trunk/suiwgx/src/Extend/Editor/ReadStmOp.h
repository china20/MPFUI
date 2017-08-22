
#ifndef _UISTORAGEOP_H_
#define _UISTORAGEOP_H_

#include <Extend/Editor/Embbed.h>

class StorageOp : public suic::ISStream
{
public:

    StorageOp(IStorage* pStg)
    {
        _stream = NULL;
        _stg = pStg;
        _stg->EnumElements(NULL, NULL, NULL, &_senum);
        memset(&_statStg, 0, sizeof(_statStg));
    }

    ~StorageOp()
    {
        if (_statStg.pwcsName)
        {
            CoTaskMemFree(_statStg.pwcsName); 
        }

        if (NULL != _senum)
        {
            _senum->Release();
        }

        if (_stream)
        {
            _stream->Release();
        }
    }

    bool LocateReadStm(suic::String name)
    {
        _stg->OpenStream(name.c_str(), NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &_stream);
        if (_stream != NULL)
        {
            _stream->Stat(&_statStg, 0);
        }
        return (NULL != _stream);

        if (_statStg.pwcsName)
        {
            CoTaskMemFree(_statStg.pwcsName); 
        }

        HRESULT hr = _senum->Next(1, &_statStg, NULL);

        if (S_OK != hr)
        {
            return false;
        }

        if (NULL != _stream)
        {
            _stream->Release();
            _stream = NULL;
        }

        if (_statStg.type == STGTY_STREAM)
        {
            _stg->OpenStream(_statStg.pwcsName, NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, &_stream);
        }

        return true;
    }

    bool LocateWriteStm(suic::String name)
    {
        HRESULT hr = S_OK;

        //_stg->DestroyElement(name.c_str());
        _stg->OpenStream(name.c_str(), NULL, STGM_WRITE | STGM_SHARE_EXCLUSIVE, 0, &_stream);
        if (NULL == _stream)
        {
            _stg->CreateStream(name.c_str(), STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, 0, &_stream);
        }

        return (NULL != _stream);
    }

    void Clear()
    {

    }

    int GetSize() const
    {
        return 0;
    }

    virtual int Seek(int pos)
    {
        return 0;
    }

    virtual int Read(suic::Byte* data, int bytes)
    {
        ULONG gotBytes = 0;

        if (NULL != _stream)
        {
            _stream->Read(data, bytes, &gotBytes);
        }

        return gotBytes;
    }

    virtual int Write(suic::Byte* data, int bytes)
    {
        ULONG gotBytes = 0;

        if (NULL != _stream)
        {
            return _stream->Write(data, bytes, &gotBytes);
        }

        return gotBytes;
    }

private:

    IStorage* _stg;
    IStream* _stream;
    STATSTG _statStg;
    IEnumSTATSTG* _senum;
};

#endif
