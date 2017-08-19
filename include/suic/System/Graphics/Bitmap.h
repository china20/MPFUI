// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Bitmap.h>
// ��  �ܣ���װͼ�����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-12
// 
// ============================================================================

#ifndef _UIBITMAP_H_
#define _UIBITMAP_H_

#include <System/Windows/ResourceUri.h>

namespace suic
{

struct BitmapInfo;

/// <summary>
///  ͼ�����ݴ����װ
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
    ///  ����ͼ��Ļ�����Ϣ
    /// </summary>
    /// <remarks>
    ///  �˷���������¼����ͼ����Ҫ����Ϣ�������������Ĵ���
    ///  ���ô˷��������ͼ��
    /// </remarks>
    /// <param name="wid">ͼ����</param>
    /// <param name="hei">ͼ��߶�</param>
    /// <param name="bits">ͼ��ÿ����λ��</param>
    /// <returns>�ɹ�����true������false</returns>
    bool SetConfig(Int32 wid, Int32 hei, int bits);

    /// <summary>
    ///  ����ͼ��
    /// </summary>
    /// <remarks>
    ///  �˷�������Config��AllocPixel����ͼ�񴴽�
    /// </remarks>
    /// <param name="wid">ͼ����</param>
    /// <param name="hei">ͼ��߶�</param>
    /// <param name="bits">ͼ��ÿ����λ��</param>
    /// <returns>�ɹ�����true������false</returns>
    bool Create(Int32 wid, Int32 hei, int bits);
    bool Create(Int32 wid, Int32 hei, Byte* data, int bits);

    /// <summary>
    ///  ����ͼ��
    /// </summary>
    /// <remarks>
    ///  ���ô˷���ǰ�����SetConfig
    /// </remarks>
    /// <param name="clr">���ɫ</param>
    /// <returns>�ɹ�����true������false</returns>
    bool AllocPixel(Color clr);

    /// <summary>
    ///  ����32λͼ��ͬʱ����һ��ͼ������windows��HBITMAP��
    /// </summary>
    /// <remarks>
    ///  �˷����ڲ��Զ�����SetConfig
    /// </remarks>
    /// <param name="wid">ͼ����</param>
    /// <param name="hei">ͼ��߶�</param>
    /// <param name="clr">���ɫ</param>
    /// <returns>�ɹ�����true������false</returns>
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
    // ����ͼ������
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
