
#ifndef _UICOREHELPER_H_
#define _UICOREHELPER_H_

#include <Framework/Controls/Window.h>

namespace suic
{ 

class SUICORE_API FileDir
{
public:

    enum eFileProp
    {
        fpNone,
        fpFile,
        fpDir,
    };

    /// <summary>
    ///  获取当前应用程序所在目录
    /// </summary>
    /// <param name="elem">无</param>
    /// <returns>程序目录</returns>
    static String GetAppDir();
    static String CalculatePath(const String& path);
    static String SplitToDir(const String& path);
    static String SplitToFile(const String& path, String& filename);
    static String SplitToPath(const String& path, String& filename, String& ext);

    /// <summary>
    ///  递归创建元素所在目录。
    /// </summary>
    /// <param name="dir">目录</param>
    /// <returns>成功返回true;失败返回false</returns>
    static bool DupCreateDir(const String& dir);

    static bool FileExist(const String& path);
    static eFileProp GetFileProp(const String& path);

    static bool RemoveFile(const String& path);
    static bool MoveFileTo(const String& existPath, const String& path);
    static bool CopyFileTo(const String& existPath, const String& path, bool bFailIfExist);

    // 临时文件操作
    static String GetTempPath();
    static String GetTempFileName();

private:

    FileDir();
};

class SUICORE_API CoreHelper
{
public:

    static int GetDefaultFontSize();
    static String GetDefaultFontFamily();

    /// <summary>
    ///   字体磅数(点)转换为实际像素数
    /// </summary>
    /// <param name="size">字体磅数</param>
    /// <returns>字体大小(像素)</returns> 
    static Float FontPoundToPixel(Float size);

    /// <summary>
    ///   字体大小(像素)转换为字体磅数(点)
    /// </summary>
    /// <param name="size">字体大小</param>
    /// <returns>字体磅数</returns> 
    static Float FontPoundFromPixel(Float size);

    static Byte ToKeyFlags(Uint32 val);

    static void SetDpiScale(bool bScale);

    static Float GetScaleRatio();
    static void SetScaleRatio(Float fScale);

    static fSize ComputeScaleFactor(Size availableSize, Size contentSize, int stretch, int stretchDirection);

private:

    CoreHelper();
};

// 初始化函数
SUICORE_API bool __stdcall InitUICore();
SUICORE_API void __stdcall ExitUICore();
SUICORE_API void __stdcall InitUILogLevel(int level);

}

#endif
