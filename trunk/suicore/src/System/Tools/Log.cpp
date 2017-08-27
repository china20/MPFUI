
#include <System/Tools/Log.h>
#include <System/Windows/CoreTool.h>
#include <System/Tools/CoreHelper.h>

#include <time.h>

namespace suic
{

Log::~Log()
{
    Close();
}

bool Log::IsValid()
{
    return (_file != NULL);
}

bool Log::Init(const String& path, int level)
{
    Close();

    _path = path;
    _level = level;
    _path = FileDir::CalculatePath(path);

    if (FileDir::DupCreateDir(_path))
    {
        _file = fopen(Mulstr(_path.c_str()).c_str(), "w+");
    }

    return IsValid();
}

bool Log::Open()
{
    Close();
    if (FileDir::DupCreateDir(_path))
    {
        _file = fopen(Mulstr(_path.c_str()).c_str(), "a");
    }

    return IsValid();
}

void Log::Close()
{
    if (_file != NULL)
    {
        fclose(_file);
        _file = NULL;
    }
}

void Log::Print(const String& info)
{
    
}

static String __CurrentTime()
{
    time_t tnow;
    time(&tnow);
    struct tm lt;
    localtime_s(&lt, &tnow);

    String szFmt;
    szFmt.Format(_T("%02d:%02d:%02d %04d-%02d-%02d")
        , lt.tm_hour, lt.tm_min, lt.tm_sec 
        , (lt.tm_year+1900), (lt.tm_mon+1), lt.tm_mday);

    return szFmt;
}

void __PrintLog(Log* pLog, FILE* file, const String& title, const String& info)
{
    String strInfo;
    strInfo.Format(_T("%s -> %s\n> %s\n")
        , __CurrentTime().c_str()
        , title.c_str(), info.c_str());
    Mulstr strLog(strInfo.c_str());

    fwrite(strLog.c_str(), strLog.Length(), 1, file);
    fflush(file);

    Debug::Trace(strInfo.c_str());
}

void Log::Trace(const String title, const String& info)
{
    if (NULL != _file && _level >= Log::eTrace)
    {
        __PrintLog(this, _file, _T("Trace"), info);
    }
}

void Log::Warn(const String& info)
{
    if (NULL != _file && _level >= Log::eWarn)
    {
        __PrintLog(this, _file, _T("Warn"), info);
    }
}

void Log::Info(const String& info)
{
    if (NULL != _file && _level >= Log::eInfo)
    {
        __PrintLog(this, _file, _T("Info"), info);
    }
}

void Log::Error(const String& info)
{
    if (NULL != _file != NULL && _level >= Log::eError)
    {
        __PrintLog(this, _file, _T("Error"), info);
    }
}

Log::Log()
{
    _file = NULL;
    _level = 0;
}

}

