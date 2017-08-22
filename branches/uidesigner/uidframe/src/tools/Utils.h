
#ifndef _UIUTILS_H_
#define _UIUTILS_H_

#include <zip/XRZip.h>

#define RES_FILTER _U(".png;.tif;.tff;.bmp;.jpg;.jpeg;.gif;.cur;.xaml;.xml")
#define RES_IMGFILTER _U(".png;.tif;.tff;.bmp;.jpg;.jpeg;.gif;.cur;")
#define APP_FILENAME _U("Application.xaml")

class Utils
{
public:

    static bool ZipFolder(const String& strZip, const String& strPath, const String& zipExt, const String& pwd);
    static void RemoveFiles(const String& strDir, bool bRemoveDir);

    static bool CheckUseTimeout(const String& info);
    static int ReadResFromFile(const char* filename, const char* mode, Mulstr& buff);

    static String CheckUIXmlRoot(const String& strPath);

private:

    static void InternalRemoveFiles(const String& strDir, bool bRemoveSubDir);
    static void ZipFileFolder(XRZip* pZip, const String& strRoot, const String& strPath, const String& zipExt);

private:

    static time_t _startRunTime;
};

#endif
