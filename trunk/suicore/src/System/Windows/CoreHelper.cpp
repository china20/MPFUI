
#include <System/Tools/CoreHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/ThemeOp.h>

#include <System/Interop/System.h>
#include <System/Interop/InternalOper.h>
#include <System/Interop/InternalWindowOper.h>

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#ifndef SUICORE_LIB
extern HMODULE s_g_handle;
#endif

namespace suic
{

int _flag = 0;
void StaticUIInit();

bool __stdcall InitUICore()
{
    if (_flag != 0)
    {
        return true;
    }
    else
    {
        ::CoInitialize(NULL);
#ifdef SUICORE_LIB
        StaticUIInit();
#endif
        _flag = 1;

        suic::ThemeOp::SetSystemDir(_U(""));
        
        return true;
    }
}

void __stdcall ExitUICore()
{
    suiInternalClear();
    ::CoUninitialize();
}

void __stdcall InitUILogLevel(int level)
{
    CoreSystem::Ins()->SetLogLevel(level);
}

void CoreHelper::SetDpiScale(bool bScale)
{
    CoreSystem::Ins()->SetDpiScale(bScale);
}

Float CoreHelper::GetScaleRatio()
{
    return CoreSystem::Ins()->GetScaleRatio();
}

void CoreHelper::SetScaleRatio(Float fScale)
{
    CoreSystem::Ins()->SetScaleRatio(fScale);
}

fSize CoreHelper::ComputeScaleFactor(Size availableSize, Size contentSize, int stretch, int stretchDirection)
{
    Float width = 1.0;
    Float height = 1.0;
    bool maxWid = Numeric::IsMeasureInt(availableSize.Width());
    bool maxHei = Numeric::IsMeasureInt(availableSize.Height());

    if (maxWid && maxHei)
    {
        return fSize(width, height);
    }

    width = contentSize.Width() == 0 ? 0.0 : ((Float) availableSize.Width() / (Float) contentSize.Width());
    height = contentSize.Height() == 0 ? 0.0 : ((Float) availableSize.Height() / (Float) contentSize.Height());

    if (maxWid)
    {
        width = height;
    }
    else if (maxHei)
    {
        height = width;
    }
    else
    {
        switch (stretch)
        {
        // 拉伸至整个区域
        case Stretch::stFill:
            break;

        // 按照高和框最大值缩放显示
        case Stretch::stUniform:
            {
                Float num3 = (width < height) ? width : height;
                width = height = num3;
                break;
            }
        // 按照高和框最小值缩放显示（图像显示可能会超过显示区域）
        case Stretch::stUniformToFill:
            {
                Float num4 = (width > height) ? width : height;
                width = height = num4;
                break;
            }
        }
    }

    switch (stretchDirection)
    {
    case StretchDirection::sdUpOnly:
        if (width < 1.0)
        {
            width = 1.0;
        }
        if (height < 1.0)
        {
            height = 1.0;
        }
        break;

    case StretchDirection::sdDownOnly:
        if (width > 1.0)
        {
            width = 1.0;
        }
        if (height > 1.0)
        {
            height = 1.0;
        }
        break;

    case StretchDirection::sdBoth:
        break;
    }

    return fSize(width, height);
}

String FileDir::GetAppDir()
{
    //
    // 获取应用程序根目录
    // 
    Char cpPath[256] = {0};
#ifndef SUICORE_LIB
    GetModuleFileName(s_g_handle, cpPath, 256);
#else
    GetModuleFileName(NULL, cpPath, 256);
#endif
    String ret = FileDir::SplitToDir(cpPath);
    return ret;
}

String FileDir::CalculatePath(const String& path)
{
    int iLast = 0;
    String strPath = path;
    if (strPath.IndexOf(_T(":"), 0) == String::npos)
    {
        strPath = GetAppDir();
        strPath += path;
    }

    strPath.Trim();
    iLast = strPath.Length() - 1;

    if (iLast >= 0 && FileDir::GetFileProp(strPath) == FileDir::eFileProp::fpDir)
    {
        
        if (strPath[iLast] != _U('\\') &&  
            strPath[iLast] != _U('/'))
        {
            strPath += _U("\\");
        }

        strPath.Replace(_U("\\\\"), _U("\\"));
    }

    return strPath.Trim();
}

String FileDir::SplitToDir(const String& path)
{
    String ext;
    String filename;
    return SplitToPath(path, filename, ext);
}

String FileDir::SplitToFile(const String& path, String& filename)
{
    String ext;
    return SplitToPath(path, filename, ext);
}

String FileDir::SplitToPath(const String& path, String& filename, String& ext)
{
    String ret;
    String tmp;
    bool bFind = false;
    int count = path.Length();

    for (int i = count - 1; i >= 0; --i)
    {
        if (path[i] == _T('/') || path[i] == _T('\\'))
        {
            ret = path.Substring(0, i + 1);
            tmp = path.Substring(i + 1);

            for (int j = tmp.Length() - 1; j >= 0; --j)
            {
                if (tmp[j] == '.')
                {
                    bFind = true;
                    filename = tmp.Substring(0, j);
                    ext = tmp.Substring(j + 1);
                    break;
                }
            }

            if (!bFind)
            {
                filename = tmp;
            }

            break;
        }
    }

    return ret;
}

bool FileDir::DupCreateDir(const String& strPath)
{
    for (int i = 0; i < (int)strPath.Length(); ++i)
    {
        if (strPath[i] == _T('\\') || strPath[i] == _T('/'))
        {
            String strDir = strPath.Substring(0, i + 1);
            DWORD dwAttr = GetFileAttributes(strDir.c_str());

            // 目录不存在 
            if (dwAttr == -1 || (dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                if (!::CreateDirectory((LPCTSTR)strDir.c_str(), NULL))
                {
                    return false;
                }
            }
        }
    }

    if (strPath.IndexOf(_U(".")) != FileDir::fpNone || 
        GetFileProp(strPath) == FileDir::fpFile)
    {
        return true;
    }

    if (!::CreateDirectory((LPCTSTR)strPath.c_str(), NULL))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool FileDir::FileExist(const String& path)
{
    return ((::PathFileExists(path.c_str())) ? true : false);
}

FileDir::eFileProp FileDir::GetFileProp(const String& path)
{
    DWORD dwAttr = GetFileAttributes(path.c_str());   //得到文件属性 

    if (dwAttr == 0xFFFFFFFF)                     // 文件或目录不存在
    {
        return eFileProp::fpNone;
    }
    else if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)   // 如果是目录
    {
        return eFileProp::fpDir; 
    }
    else
    {
        return eFileProp::fpFile;
    }
}

bool FileDir::RemoveFile(const String& path)
{
    ::DeleteFile(path.c_str());
    return true;
}

bool FileDir::CopyFileTo(const String& existPath, const String& path, bool bFailIfExist)
{
    ::CopyFile(existPath.c_str(), path.c_str(), bFailIfExist ? TRUE : FALSE);
    return true;
}

bool FileDir::MoveFileTo(const String& existPath, const String& path)
{
    ::MoveFile(existPath.c_str(), path.c_str());
    return true;
}

FileDir::FileDir()
{

}

String FileDir::GetTempPath()
{
    Char buffer[261];
    Uint32 tempPath = 0;

    buffer[0] = _U('\0');
    tempPath = ::GetTempPath(260, buffer);

    if (tempPath == 0)
    {
        return _U("");
    }
    else
    {
        return buffer;
    }
}

String FileDir::GetTempFileName()
{
    String tempPath = FileDir::GetTempPath();
    Char tmpFileName[261];

    tmpFileName[0] = _U('\0');

    if (::GetTempFileName(tempPath.c_str(), _U("tmp"), 0, tmpFileName) == 0)
    {
        return _U("");
    }
    else
    {
        return tmpFileName;
    }
}

//----------------------------------------------------------

CoreHelper::CoreHelper()
{

}

int CoreHelper::GetDefaultFontSize()
{
    return SystemParameters::MessageFontSize;
}

Float CoreHelper::FontPoundToPixel(Float size)
{
    // 当前显示设备的垂直单位英寸像素
    // 1英寸 == 72磅
    //size = SystemParameters::TransformXToDevice(size);
    const Float fCaps = (Float)SystemParameters::DpiY / 72.0f;
    return size * fCaps;
}

Float CoreHelper::FontPoundFromPixel(Float size)
{
    int iCaps = SystemParameters::DpiY;
    //size *= CoreSystem::Ins()->GetScaleRatio();
    Float iSize = (Float)size * 72.0f / (Float)iCaps;
    return iSize;
}

Byte CoreHelper::ToKeyFlags(Uint32 val)
{
    Byte keyFlags = 0;

    if (HIWORD(val) == KF_REPEAT)
    {
        keyFlags |= KeyFlags::kfRepeat;
    }

    if (HIWORD(val) == KF_EXTENDED)
    {
        keyFlags |= KeyFlags::kfExtended;
    }

    return keyFlags;
}

String CoreHelper::GetDefaultFontFamily()
{
    return SystemParameters::MessageFontName;
    return _U("Tahoma");
}

}
