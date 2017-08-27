
#include "xzip.h"

class XRZip
{
public:

    XRZip()
        : _hzip(NULL)
    {
    }

    ~XRZip()
    {
        Close();
    }

    bool OpenFromFile(TCHAR* lpszName)
    {
        Close();

        _hzip = CreateZip((void*)lpszName, 0, ZIP_FILENAME);

        return (NULL != _hzip);
    }

    bool OpenFromMemory(void* data, int len)
    {
        Close();

        _hzip = CreateZip(data, len, ZIP_MEMORY);

        return (NULL != _hzip);
    }

    int AddFile(const TCHAR* lpszSection, const TCHAR* lpszFile)
    {
        int iRet = (int)ZipAdd(_hzip, lpszSection, (void*)lpszFile, 0, ZIP_FILENAME);

        return iRet;
    }

    int AddData(const TCHAR* lpszSection, void* data, int len)
    {
        int iRet = (int)ZipAdd(_hzip, lpszSection, data, len, ZIP_MEMORY);

        return iRet;
    }

    int AddFolder(TCHAR* lpszFolder, TCHAR* lpszDir, TCHAR* lpszExt)
    {
        return (0 == AddFolderContent(_hzip, lpszFolder, lpszDir, lpszExt));
    }

    void Close()
    {
        if (NULL != _hzip)
        {
            CloseZip(_hzip);
            _hzip = NULL;
        }
    }


protected:

    HZIP _hzip;
};
