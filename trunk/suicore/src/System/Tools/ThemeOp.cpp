
#include "Decrypt.h"

#include <stdio.h>
#include <System/Tools/ThemeOp.h>
#include <System/Tools/XRUnzip.h>
#include <System/Tools/CoreHelper.h>

#include <System/Interop/System.h>
#include <System/Interop/ResourceInfo.h>
#include <System/Interop/InternalOper.h>
#include <System/Interop/InternalWindowOper.h>

#include <Skia/core/SkImageDecoder.h>

namespace suic
{

IThemeInfo* ThemeOp::_systemDir = EmptyThemeInfo::Ins();
IThemeInfo* ThemeOp::_systemRes = EmptyThemeInfo::Ins();

suic::Uint16 ThemeOp::_readMask = ResReadMask::ResReadAllMask;

StringAutoDic<IThemeInfo> ThemeOp::_reses;

typedef struct tagResourceHeader
{
    // 本结构大小
    WORD dwSize;
    // 资源加密算法
    WORD dwEncry;
    // 文件长度
    DWORD dwLen;
    // 版本号
    DWORD dwVersion;
    // 资源创建日期
    DWORD dwDate;
    // 资源更新日期
    DWORD dwUpdateDate;

    // 资源文件密码
    char pwd[20];
    // 作者
    char author[16];
    // 资源描述
    char comment[64];
}ResourceHeader;

class SysResourceEncry : public IResDecryption
{
public:

    virtual ~SysResourceEncry()
    {
        ;
    }

    static IResDecryption* Ins()
    {
        static SysResourceEncry _ins;
        return &_ins;
    }

    bool Decrypt(const String& pwd, EncryItem& encryItem)
    {
        int iSize = UIDecrypt::Decrypt(pwd, encryItem.input, encryItem.inputSize, &(encryItem.output));
        return (iSize > 0 && iSize == encryItem.output.GetSize());
    }
};

bool MemThemeInfo::ReadResData(const String& pwd)
{
    int iSize = _data.GetSize();
    bool bSucc = false;

    if (iSize > 0)
    {
        EncryItem encryItem;
        Byte* temp = NULL;
        int iDataSize = 0;
        IResDecryption* resEncry = _decrypt;
        
        if (NULL == resEncry)
        {
            resEncry = CoreSystem::Ins()->GetResDecryption();
        }

        encryItem.input = (Byte*)_data.GetBuffer();
        encryItem.inputSize = iSize;

        if (NULL == resEncry)
        {
            resEncry = SysResourceEncry::Ins();
        }

        if (NULL != resEncry)
        {
            bSucc = resEncry->Decrypt(pwd, encryItem);
        }

        if (bSucc)
        {
            encryItem.output.Detach(&_data);
            iSize = _data.GetSize();
        }

        bSucc = _unzip.OpenFromMemory(_data.GetBuffer(), iSize, false);
    }

    return bSucc;
}

int MemThemeInfo::ReadFileRes(const String& strPath, ISStream* data)
{
    return CoreSystem::ReadFileData(strPath, data);
}

void MemThemeInfo::HandleResName(LPTSTR lpResName)
{
    if (lpResName == NULL)
    {
        return;
    }
    do 
    {
        if(*lpResName == '\\') *lpResName = '_';
        if(*lpResName == '/') *lpResName = '_';
        if(*lpResName == '.') *lpResName = '_';
    }while(*lpResName++);
}

int MemThemeInfo::ReadZipRes(const String& name, suic::ISStream* data)
{
    String strName(name);

    strName.Replace(_T('\\'), _T('/'));

    // 首先从压缩资源包读取
    int iSize = 0;

    if (_path.Empty())
    {
        iSize = _unzip.GetZipItemData(strName.c_str(), data);
    }

    if (iSize == 0)
    {
        if (!_resInit)
        {
            _resInit = true;
            // 其次从资源库（dll）读取
            if (!_resHandle && !_path.Empty())
            {
                _resHandle = ::LoadLibrary(_path.c_str());
                if (_resHandle == NULL)
                {
                    _resHandle = ::LoadLibraryEx(_path.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
                }
            }
        }

        // 成功获取资源
        if (_resHandle)
        {
            iSize = CoreSystem::ReadObjFromRes(_resHandle, strName.c_str(), data);
        }
        else
        {
            // 如果没有加载资源则从进程读取
            iSize = CoreSystem::ReadObjFromRes(GetModuleHandle(NULL), strName.c_str(), data);
        }
    }

    return iSize;
}

class UriThemeInfo : public MemThemeInfo
{
public:

    UriThemeInfo(IResDecryption* pDecrypt)
        : MemThemeInfo(pDecrypt)
    {
    }

    ~UriThemeInfo()
    {
    }

    bool InitRes(const String& path, const String& pwd)
    {
        _data.Clear();
        String strFile(FileDir::CalculatePath(path));

        if (CoreSystem::ReadFileData(strFile, &_data) > 0)
        {
            ReadResData(pwd);
            return (_data.GetSize() > 0);
        }
        else if (FileDir::FileExist(strFile))
        {
            // 作为dll资源
            _path = strFile;
            return true;
        }
        else
        {
            return false;
        }
    }
};

class DirThemeInfo : public IThemeInfo
{
public:

    DirThemeInfo(const suic::String& dir)
    {
        _dir = FileDir::CalculatePath(dir);
    }

    ~DirThemeInfo()
    {
    }

    int ReadRes(const String& key, ISStream* data)
    {
        suic::String strPath = _dir + key;
        return suic::CoreSystem::ReadFileData(strPath, data);
    }

private:

    suic::String _dir;
};

ThemeLoader::ThemeLoader()
{

}

IThemeInfo* ThemeLoader::LoadDir(const String& dir)
{
    IThemeInfo* themeInfo = NULL;
    String _dir = FileDir::CalculatePath(dir);

    if (FileDir::GetFileProp(_dir) == FileDir::eFileProp::fpDir)
    {
        themeInfo = new DirThemeInfo(_dir);
        themeInfo->ref();
    }
    return themeInfo;
}

IThemeInfo* ThemeLoader::LoadUri(const String& path, const String& pwd, IResDecryption* pDecrypt)
{
    IThemeInfo* themeInfo = NULL;
    UriThemeInfo* uriTheme = new UriThemeInfo(pDecrypt);

    uriTheme->ref();

    if (uriTheme->InitRes(path, pwd))
    {
        themeInfo = uriTheme;
        themeInfo->ref();
    }

    uriTheme->unref();

    return themeInfo;
}

IThemeInfo* ThemeLoader::LoadMemory(ByteStream& data, const String& pwd, IResDecryption* pDecrypt)
{
    IThemeInfo* themeInfo = NULL;
    MemThemeInfo* memTheme = new MemThemeInfo(pDecrypt);

    memTheme->ref();

    if (memTheme->InitRes(data, pwd))
    {
        themeInfo = memTheme;
        themeInfo->ref();
    }

    memTheme->unref();

    return themeInfo;
}

ThemeOp::ThemeOp()
{

}

void ThemeOp::AddTheme(const String& name, const String& path, const String& pwd)
{
    if (!_reses.Contains(name))
    {
        IThemeInfo* themeInfo = ThemeLoader::LoadUri(path, pwd, NULL);
        if (themeInfo != NULL)
        {
            _reses.Add(name, themeInfo);
            themeInfo->unref();
        }
    }
}

void ThemeOp::AddMemory(const String& name, ByteStream& data, const String& pwd)
{
    if (!_reses.Contains(name))
    {
        IThemeInfo* themeInfo = ThemeLoader::LoadMemory(data, pwd, NULL);
        if (themeInfo != NULL)
        {
            _reses.Add(name, themeInfo);
            themeInfo->unref();
        }
    }
}

void ThemeOp::AddThemeInfo(const String& name, IThemeInfo* themeInfo)
{
    if (NULL != themeInfo) 
    {
        themeInfo->ref();

        if (!_reses.Contains(name))
        {
            _reses.Add(name, themeInfo);
        }
        else
        {
            themeInfo->unref();
        }
    }
}

void ThemeOp::AddDir(const String& name, const String& dir)
{
    if (!_reses.Contains(name))
    {
        IThemeInfo* themeInfo = ThemeLoader::LoadDir(dir);
        if (themeInfo != NULL)
        {
            _reses.Add(name, themeInfo);
            themeInfo->unref();
        }
    }
}

void ThemeOp::RemoveTheme(const String& name)
{
    _reses.Remove(name);
}

IThemeInfo* ThemeOp::FindTheme(const String& name)
{
    IThemeInfo* resInfo = NULL;
    _reses.TryGetValue(name, resInfo);
    return resInfo;
}

int ThemeOp::ReadRes(const String& resid, const String& key, ISStream* buff)
{
    IThemeInfo* pResInfo = NULL;
    _reses.TryGetValue(resid, pResInfo);

    if (pResInfo != NULL)
    {
        return pResInfo->ReadRes(key, buff);
    }
    else
    {
        return 0;
    }
}

int ThemeOp::ReadRes(const ResourceUri* resUri, ISStream* buff)
{
    ReadResContext(resUri, buff);
    return buff->GetSize();
}

int ThemeOp::ReadResById(ResContext& resCtx, const String& themeName, const String& strId, ISStream* buff)
{
    int iRet = 0;
    resCtx.resInfo = NULL;
    _reses.TryGetValue(themeName, resCtx.resInfo);

    if (resCtx.resInfo != NULL)
    {
        iRet = resCtx.resInfo->ReadRes(strId, buff);
    }

    resCtx.rftType = ResReadFrom::rrfRes;

    return iRet;
}

void ThemeOp::SetReadResourceMask(suic::Uint16 uMask)
{
    _readMask = uMask;
}

int ThemeOp::ReadDefRes(ResContext& resCtx, const String& name, ISStream* buff)
{
    int iSucc = 0;
    suic::Uint16 flag = GetReadResourceMask();

    if ((flag & ResReadMask::ReadSysMask) == ResReadMask::ReadSysMask)
    {
        if ((iSucc = GetSystemDir()->ReadRes(name, buff)) > 0)
        {
            resCtx.rftType = ResReadFrom::rrfSysDir;
            resCtx.resInfo = GetSystemDir();
            return iSucc;
        }
    }

    if ((flag & ResReadMask::ReadAppMask) == ResReadMask::ReadAppMask)
    {
        // 从EXE目录查找
        suic::String strPath = FileDir::CalculatePath(name);
        iSucc = CoreSystem::ReadFileData(strPath, buff);

        if (iSucc > 0)
        {
            resCtx.rftType = ResReadFrom::rrfApp;
            resCtx.resPath = strPath;
            return iSucc;
        }
    }

    // 从资源读取
    if ((flag & ResReadMask::ReadResMask) == ResReadMask::ReadResMask)
    {
        if ((iSucc = GetSystemTheme()->ReadRes(name, buff)) > 0)
        {
            resCtx.rftType = ResReadFrom::rrfSysRes;
            resCtx.resInfo = GetSystemTheme();
            return iSucc;
        }
    }

    if ((flag & ResReadMask::ReadDefMask) == ResReadMask::ReadDefMask)
    {
        iSucc = suic::CoreSystem::Ins()->ReadDefRes(resCtx, name, buff);
    }
    
    return iSucc;
}

ResContext ThemeOp::ReadResContext(const ResourceUri* resUri, ISStream* buff)
{
    ResContext resCtx;

    int iBytes = 0;
    String strCompo = resUri->GetCompoment();
    String strResPath = resUri->GetResourcePath();

    resCtx.rftType = ResReadFrom::rrfDef;
    resCtx.resInfo = NULL;

    // 从系统资源或应用程序根目录查找
    if (strCompo.Empty())
    {
        iBytes = ReadDefRes(resCtx, strResPath, buff);
    }
    // 从指定资源包读取
    else
    {
        iBytes = iBytes = ReadResById(resCtx, strCompo, strResPath, buff);

        if (0 == iBytes)
        {
            iBytes = ReadDefRes(resCtx, strResPath, buff);
        }
    }

    return resCtx;
}

ResContext ThemeOp::ReadResCommon(const String& key, ISStream* buff)
{
    ResContext resCtx;
    int pos = key.IndexOf(_U(":"));

    resCtx.rftType = ResReadFrom::rrfDef;
    resCtx.resInfo = NULL;

    //
    // 绝对路径，直接读取文件内容
    //
    if (pos != -1)
    {
        CoreSystem::ReadFileData(key, buff);
        resCtx.rftType = ResReadFrom::rrfFile;
        resCtx.resPath = key;
    }
    else
    {
        ResourceUri resUri(key);
        ReadResContext(&resUri, buff);
    }

    return resCtx;
}

int ThemeOp::ReadRes(const String& key, ISStream* buff)
{
    ReadResCommon(key, buff);
    return buff->GetSize();
}

ResContext ThemeOp::ReadResContext(const String& key, ISStream* buff)
{
    return ReadResCommon(key, buff);
}

IThemeInfo* ThemeOp::GetSystemDir()
{
    return _systemDir;
}

void ThemeOp::SetSystemDir(const String& dir)
{
    IThemeInfo* themeInfo = ThemeLoader::LoadDir(dir);
    if (NULL != themeInfo)
    {
        _systemDir->unref();
        _systemDir = themeInfo;
    }
}

IThemeInfo* ThemeOp::GetSystemTheme()
{
    return _systemRes;
}

void ThemeOp::SetSystemTheme(IThemeInfo* themeInfo)
{
    if (themeInfo != _systemRes)
    {
        _systemRes = themeInfo;
        if (NULL != _systemRes)
        {
            _systemRes->ref();
        }
    }
}

void ThemeOp::SetSystemTheme(const String& path, const String& pwd)
{
    IThemeInfo* themeInfo = ThemeLoader::LoadUri(path, pwd, NULL);
    if (NULL != themeInfo)
    {
        _systemRes->unref();
        _systemRes = themeInfo;
    }
}

}
