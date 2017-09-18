#ifndef _FILEFINDER_H_
#define _FILEFINDER_H_

class FileFinder
{
public:

    FileFinder();
    ~FileFinder();

    bool Open(const suic::String& strDir, const suic::String& strFilter);
    bool FindNext();

    bool IsDot() const;
    bool IsDir() const;

    suic::String GetFileName() const;
    suic::String GetFilePath() const;
    suic::String GetCurrentDir() const;

    void Close();

private:

    suic::String _findDir;
    suic::String _filter;

    HANDLE _fileHandle;
    WIN32_FIND_DATA _findFileData;
};

#endif
