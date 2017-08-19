
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
    ///  ��ȡ��ǰӦ�ó�������Ŀ¼
    /// </summary>
    /// <param name="elem">��</param>
    /// <returns>����Ŀ¼</returns>
    static String GetAppDir();
    static String CalculatePath(const String& path);
    static String SplitToDir(const String& path);
    static String SplitToFile(const String& path, String& filename);
    static String SplitToPath(const String& path, String& filename, String& ext);

    /// <summary>
    ///  �ݹ鴴��Ԫ������Ŀ¼��
    /// </summary>
    /// <param name="dir">Ŀ¼</param>
    /// <returns>�ɹ�����true;ʧ�ܷ���false</returns>
    static bool DupCreateDir(const String& dir);

    static bool FileExist(const String& path);
    static eFileProp GetFileProp(const String& path);

    static bool RemoveFile(const String& path);
    static bool MoveFileTo(const String& existPath, const String& path);
    static bool CopyFileTo(const String& existPath, const String& path, bool bFailIfExist);

    // ��ʱ�ļ�����
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
    ///   �������(��)ת��Ϊʵ��������
    /// </summary>
    /// <param name="size">�������</param>
    /// <returns>�����С(����)</returns> 
    static Float FontPoundToPixel(Float size);

    /// <summary>
    ///   �����С(����)ת��Ϊ�������(��)
    /// </summary>
    /// <param name="size">�����С</param>
    /// <returns>�������</returns> 
    static Float FontPoundFromPixel(Float size);

    static Byte ToKeyFlags(Uint32 val);

    static void SetDpiScale(bool bScale);

    static Float GetScaleRatio();
    static void SetScaleRatio(Float fScale);

private:

    CoreHelper();
};

// ��ʼ������
SUICORE_API bool __stdcall InitUICore();
SUICORE_API void __stdcall ExitUICore();
SUICORE_API void __stdcall InitUILogLevel(int level);

}

#endif
