// 华勤科技版权所有 2008-2022
// 
// 文件名：Bitmap.h>
// 功  能：封装图像操作
// 
// 作  者：MPF开发组
// 时  间：2010-07-12
// 
// ============================================================================

#ifndef _UIBITMAP_H_
#define _UIBITMAP_H_

#include <System/Windows/ResourceUri.h>

namespace suic
{

struct BitmapInfo;

/// <summary>
///  图像数据处理封装
/// </summary>
class SUICORE_API Bitmap
{
public:

    enum ImageType 
    {
        kUnknown,
        kBMP,
        kGIF,
        kICO,
        kJPEG,
        kPNG,
        kWBMP,
        kWEBP,
        kTiff,
    };

    Bitmap();
    Bitmap(bool del);

    ~Bitmap();

    void CopyTo(Bitmap* Other) const;
    void DrawTo(Bitmap* Other, const Rect* src, const Rect* dst) const;

    /// <summary>
    ///  配置图像的基本信息
    /// </summary>
    /// <remarks>
    ///  此方法仅仅记录创建图像需要的信息，不进行真正的创建
    ///  调用此方法需清除图像
    /// </remarks>
    /// <param name="wid">图像宽度</param>
    /// <param name="hei">图像高度</param>
    /// <param name="bits">图像每像素位数</param>
    /// <returns>成功返回true；否则false</returns>
    bool SetConfig(Int32 wid, Int32 hei, int bits);

    /// <summary>
    ///  创建图像
    /// </summary>
    /// <remarks>
    ///  此方法调用Config和AllocPixel进行图像创建
    /// </remarks>
    /// <param name="wid">图像宽度</param>
    /// <param name="hei">图像高度</param>
    /// <param name="bits">图像每像素位数</param>
    /// <returns>成功返回true；否则false</returns>
    bool Create(Int32 wid, Int32 hei, int bits);
    bool Create(Int32 wid, Int32 hei, Byte* data, int bits);

    /// <summary>
    ///  创建图像
    /// </summary>
    /// <remarks>
    ///  调用此方法前需调用SetConfig
    /// </remarks>
    /// <param name="clr">填充色</param>
    /// <returns>成功返回true；否则false</returns>
    bool AllocPixel(Color clr);
    bool AllocPixels();

    /// <summary>
    ///  创建32位图像，同时返回一个图像句柄（windows是HBITMAP）
    /// </summary>
    /// <remarks>
    ///  此方法内部自动调用SetConfig
    /// </remarks>
    /// <param name="wid">图像宽度</param>
    /// <param name="hei">图像高度</param>
    /// <param name="clr">填充色</param>
    /// <returns>成功返回true；否则false</returns>
    bool AllocHandle(int wid, int hei, Color clr);

    bool Load(const ResourceUri* uri);
    bool Load(const String& strFile);
    bool LoadHandle(Handle h);
    bool LoadDib(const Byte* data, Uint32 size);
    bool LoadMemory(const Byte* data, Uint32 size);

    bool Save(const String& filename, ImageType iType);

    int Width() const;
    int Height() const;

    int GetRowBytes() const;
    int GetBitsPerPixel() const;

    bool IsAlpha() const;
    bool IsValid() const;
    bool IsBackup() const;
    bool IsLoaded() const;
 
    Handle GetHandle() const;
    BitmapInfo* GetBitmapInfo() const;

    Uint32 GetSize() const;
    Byte* GetPixels() const;
    void SetPixels(Byte* pixels);
    Byte* GetBackupPixels() const;

    void LockPixels();
    void UnlockPixels();

    void Clear();

public:

    void EraseGray();
    void EraseColor(Color clr);
    void EraseRect(Rect rect, Color clr);
    void EraseHSL(int h, int s, int l);

    void SetColor(Int32 x, Int32 y, Color clr);
    Color GetColor(Int32 x, Int32 y);

    void MirrorHori();
    void MirrorVert();

    bool Backup();
    bool Restore();
    void ResetBackup();

    void SetPixels(Byte* pixels, Handle h, bool autoDel);

private:

    void SetLoaded(bool bLoad);

protected:

    Byte _flag;
    BitmapInfo* _bmpInfo;
    // 备份图像数据
    Byte* _backupData;
};

inline bool Bitmap::IsAlpha() const 
{ 
    return (_flag & 0x1) != 0;
}

inline Byte* Bitmap::GetBackupPixels() const 
{ 
    return _backupData; 
}

}

#endif
