#ifndef _FILEFINDER_H_
#define _FILEFINDER_H_

#include <System/Windows/Object.h>

class FileFinder
{
public:

    FileFinder();
    ~FileFinder();

    bool Open(const String& strDir, const String& strFilter);
    bool FindNext();

    bool IsDot() const;
    bool IsDir() const;

    String GetFileName() const;
    String GetFilePath() const;
    String GetCurrentDir() const;

    void Close();

private:

    String _findDir;
    String _filter;

    HANDLE _fileHandle;
    WIN32_FIND_DATA _findFileData;
};

#endif
