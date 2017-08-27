
#ifndef _UIXRUNZIP_H_
#define _UIXRUNZIP_H_

#include "xunzip.h"

#include <System/Types/StreamDef.h>
#include <System/Types/StringCore.h>

class XRUnzip
{
public:

    XRUnzip()
        : _hzip(0)
    {
    }

    ~XRUnzip()
    {
        Close();
    }

    HZIP GetHZip() const
    {
        return _hzip;
    }

    bool OpenFromFile(TCHAR* lpszName)
    {
        Close();
        _hzip = OpenZip((void*)lpszName, 0, ZIP_FILENAME, false);
        return (NULL != _hzip);
    }

    bool OpenFromMemory(void* data, int len, bool canfree)
    {
        Close();
        _hzip = OpenZip(data, len, ZIP_MEMORY, canfree);
        return (NULL != _hzip);
    }

    void Close()
    {
        if (NULL != _hzip)
        {
            CloseZip(_hzip);
            _hzip = NULL;
        }
    }

    char* GetZipItem(TCHAR* lpszName, int& iSize)
    {
        ZIPENTRYW ze; 
        int i = -1;
        iSize = 0;

        FindZipItem(_hzip, lpszName, true, &i, &ze);

        if (i == -1)
        {
            return NULL;
        }
        else
        {
            iSize = ze.unc_size;
            char *ibuf = new char[ze.unc_size];
            UnzipItem(_hzip, i, ibuf, ze.unc_size, ZIP_MEMORY);

            return ibuf;
        }
    }

    int GetZipItemData(suic::Char* lpszName, suic::ISStream* data)
    {
        ZIPENTRYW ze; 
        int i = -1;
        int iSize = 0;
        ZRESULT zrRes = 0;

        FindZipItem(_hzip, lpszName, true, &i, &ze);

        if (i == -1)
        {
            return iSize;
        }
        else
        {
            iSize = ze.unc_size;
            if (iSize > 0)
            {
                const int READBYTES = 1024 * 64;
                int unZipSize = ze.unc_size;
                int zipSize = READBYTES;
                suic::Byte zipBuff[READBYTES];

                zipSize = zipSize < unZipSize ? zipSize : unZipSize;//min(zipSize, unZipSize);

                for (;;)
                {
                    zrRes = UnzipItem(_hzip, i, zipBuff, zipSize, ZIP_MEMORY);
                    if (ZR_MORE == zrRes || ZR_OK == zrRes)
                    {
                        data->Write(zipBuff, zipSize);
                    }
                    if (ZR_MORE == zrRes)
                    {
                        unZipSize -= zipSize;
                        zipSize = READBYTES < unZipSize ? READBYTES: unZipSize;//min(READBYTES, unZipSize);
                    }
                    else
                    {
                        break;
                    }
                }
            }

            return iSize;
        }
    }

protected:

    HZIP _hzip;
};

#endif
