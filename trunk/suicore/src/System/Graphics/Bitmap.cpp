/////////////////////////////////////////////////////////////////////////
// uibitmap.cpp

#include <System/Graphics/Bitmap.h>
#include <System/Windows/CoreTool.h>

#include <Skia/core/SkCanvas.h>
#include <Skia/core/SkImageDecoder.h>

/*--------内部使用头文件--------*/
#include <System/Tools/SSE.h>
#include <System/Tools/Debug.h>
#include <System/Tools/DIBHelper.h>
#include <System/Tools/ThemeOp.h>
#include <System/Interop/System.h>

#include <stdio.h>

#include <Skia/core/SkImageDecoder.h>

namespace suic
{

Bitmap::Bitmap()
    : _backupData(NULL)
    , _flag(0)
{
    _bmpInfo = new BitmapInfo();
}

Bitmap::Bitmap(bool del)
    : _backupData(NULL)
    , _flag(0)
{
    _bmpInfo = new BitmapInfo();
}

Bitmap::~Bitmap()
{
    if(_backupData)
    {
        delete [] _backupData;
        _backupData = NULL;
    }

    _bmpInfo->bmp.reset();
    delete _bmpInfo;
}

bool Bitmap::Load(const ResourceUri* uri)
{
    Clear();

    bool bSucc = false;
    ByteStream data;
    int iSize = 0;
    BitmapInfo* bmpInfo = GetBitmapInfo();

    iSize = ThemeOp::ReadRes(uri, &data);

    if (iSize > 0)
    {
        bSucc = SkImageDecoder::DecodeMemory((const Byte*)data.GetBuffer(), iSize, &(bmpInfo->bmp));
    }

    SetLoaded(true);

    return bSucc;
}

bool Bitmap::Load(const String& strFile)
{
    ResourceUri resUri(strFile);
    return Load(&resUri);
}

bool Bitmap::LoadMemory(const Byte* data, Uint32 size)
{
    Clear();
    SetLoaded(true);
    return SkImageDecoder::DecodeMemory(data, size, &(GetBitmapInfo()->bmp));
}

bool Bitmap::LoadHandle(Handle h)
{
    bool bRet = false;
    HBITMAP hbmp = (HBITMAP)h;
    BITMAP bm;

    int iWid = 0;
    int iHei = 0;

    if (!GetObject(hbmp, sizeof(bm), &bm))
    {
        BITMAPINFO* pBmpInfo = (BITMAPINFO*)hbmp;
        if (pBmpInfo->bmiHeader.biSize == sizeof(BITMAPINFOHEADER))
        {
            iWid = pBmpInfo->bmiHeader.biWidth;
            iHei = pBmpInfo->bmiHeader.biHeight;
        }
    }
    else
    {
        suic::Uint32 size = 0;
        Byte* lpBytes = NULL;
        lpBytes = DIBHelper::DIBFromBitmap(hbmp, size);
        if (NULL != lpBytes)
        {
            bRet = LoadDib(lpBytes, size);
            DIBHelper::FreeBytes(lpBytes);
        }
    }

    SetLoaded(true);
    
    return bRet;
}

bool Bitmap::LoadDib(const Byte* data, Uint32 size)
{
    int iHeaderSize = 0;
    LPBITMAPINFO pbmi = (LPBITMAPINFO)data;

    if (pbmi->bmiHeader.biSize == sizeof(BITMAPINFOHEADER))
    {
        int iBitCount = pbmi->bmiHeader.biBitCount;
        int iColors = 0;
        if (iBitCount <= 8)
        {
            iColors = 1 <<iBitCount;
        }
        iHeaderSize = sizeof(BITMAPINFOHEADER) + (iColors * sizeof(RGBQUAD));
    }
    else
    {
        return false;
    }

    Clear();

    bool bRet = false;
    int iImageSize = size + sizeof(BITMAPFILEHEADER);
    Byte* buff = new Byte[iImageSize];

    BITMAPFILEHEADER* bmf = (BITMAPFILEHEADER*)buff;

    bmf->bfType = 0x4D42;

    //
    // 图像数据在文件中的偏移位置
    //
    bmf->bfOffBits = sizeof(BITMAPFILEHEADER) + iHeaderSize;

    //
    // 除了文件头（BITMAPFILEHEADER）之外的总长度
    //
    bmf->bfSize = size;
    bmf->bfReserved1 = 0;
    bmf->bfReserved2 = 0;

    memcpy(buff + sizeof(BITMAPFILEHEADER), data, size);

    bRet = SkImageDecoder::DecodeMemory(buff, iImageSize, &(GetBitmapInfo()->bmp));

    delete[] buff;

    SetLoaded(true);

    return bRet;
}

bool Bitmap::Save(const String& filename, ImageType iType)
{
    if (IsValid())
    {
        Mulstr str(filename.c_str());
        SkImageEncoder::EncodeFile(str.c_str(), GetBitmapInfo()->bmp, SkImageEncoder::Type(iType), 80);
        
        //FILE *fd = fopen(str.c_str(), "wb");
        //bool ret = false;

        //if (fd)
        //{
        //    BITMAPFILEHEADER bmf;

        //    bmf.bfType = 0x4D42;

        //    //
        //    // 图像数据在文件中的偏移位置
        //    //
        //    bmf.bfOffBits = sizeof(BITMAPINFOHEADER) + 0;
        //    bmf.bfOffBits += sizeof(bmf);

        //    //
        //    // 除了文件头（BITMAPFILEHEADER）之外的总长度
        //    //
        //    bmf.bfSize = sizeof(BITMAPINFO) + GetSize();
        //    bmf.bfReserved1 = 0;
        //    bmf.bfReserved2 = 0;

        //    // 写文件头
        //    fwrite(&bmf, sizeof(bmf), 1, fd);

        //    // 写位图头
        //    BITMAPINFO bmp;
        //    memset(&bmp, 0, sizeof(BITMAPINFO));

        //    bmp.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
        //    bmp.bmiHeader.biWidth  = Width();
        //    bmp.bmiHeader.biHeight = (LONG)-Height();
        //    bmp.bmiHeader.biPlanes = 1;
        //    bmp.bmiHeader.biBitCount = (unsigned short)32;
        //    bmp.bmiHeader.biCompression = BI_RGB;
        //    bmp.bmiHeader.biSizeImage = GetSize();
        //    bmp.bmiHeader.biXPelsPerMeter = 0;
        //    bmp.bmiHeader.biYPelsPerMeter = 0;
        //    bmp.bmiHeader.biClrUsed = 0;
        //    bmp.bmiHeader.biClrImportant = 0;

        //    fwrite(&bmp, sizeof(BITMAPINFOHEADER), 1, fd);

        //    // 写入调色板数据
        //    ;

        //    // 写入图像数据
        //    fwrite(GetPixels(), GetSize(), 1, fd);

        //    fclose(fd);
        //}

        return true;
    }
    else
    {
        return false;
    }
}

void Bitmap::CopyTo(Bitmap* Other) const
{
    Other->Clear();
    GetBitmapInfo()->bmp.copyTo(&(Other->GetBitmapInfo()->bmp));
}

void Bitmap::DrawTo(Bitmap* Other, const Rect* src, const Rect* dst) const
{
    SkBitmap* bmpSelf = &(GetBitmapInfo()->bmp);
    SkBitmap* bmpOther = &(Other->GetBitmapInfo()->bmp);

    SkCanvas canvas(*bmpOther);
    SkPaint paint;
    SkRect rect;
    SkRect imgrc;

    if (src == NULL)
    {
        rect = SkRect::MakeWH(bmpSelf->width(), bmpSelf->height());
    }
    else
    {
        rect = SkRect::MakeLTRB(src->left, src->top, src->right, src->bottom);
    }

    if (dst == NULL)
    {
        imgrc = SkRect::MakeWH(bmpOther->width(), bmpOther->height());
    }
    else
    {
        imgrc = SkRect::MakeLTRB(dst->left, dst->top, dst->right, dst->bottom);
    }

    paint.setFilterLevel(SkPaint::FilterLevel::kMedium_FilterLevel);
    canvas.drawBitmapRectToRect(*bmpSelf, &rect, imgrc, &paint);
}

static void __SetConfig(SkBitmap* bmp, Int32 wid, Int32 hei, int bits)
{
    #if 0
    switch (bits)
    {
    case 32:
        bmp->setInfo(SkImageInfo::Make(wid, hei, kN32_SkColorType, kPremul_SkAlphaType));
        //bmp->setConfig(SkBitmap::kARGB_8888_Config, wid, hei); 
        break;
    case 16:
        //bmp->setConfig(SkBitmap::kRGB_565_Config, wid, hei); 
        bmp->setInfo(SkImageInfo::Make(wid, hei, kRGB_565_SkColorType, kIgnore_SkAlphaType));
        break;
    case 8:
        //bmp->setConfig(SkBitmap::kA8_Config, wid, hei); 
        bmp->setInfo(SkImageInfo::Make(wid, hei, kIndex_8_SkColorType, kIgnore_SkAlphaType));
        break;
    case 1:
        //bmp->setConfig(SkBitmap::kNo_Config, wid, hei);
        bmp->setInfo(SkImageInfo::Make(wid, hei, kUnknown_SkColorType, kIgnore_SkAlphaType));
        break;
    default:
        //bmp->setConfig(SkBitmap::kARGB_8888_Config, wid, hei);
        bmp->setInfo(SkImageInfo::Make(wid, hei, kN32_SkColorType, kPremul_SkAlphaType));
        break;
    }
#else

    switch (bits)
    {
    case 32:
        bmp->setConfig(SkBitmap::kARGB_8888_Config, wid, hei); 
        break;
    case 16:
        bmp->setConfig(SkBitmap::kRGB_565_Config, wid, hei); 
        break;
    case 8:
        bmp->setConfig(SkBitmap::kA8_Config, wid, hei); 
        break;
    case 1:
        bmp->setConfig(SkBitmap::kNo_Config, wid, hei);
        break;
    default:
        bmp->setConfig(SkBitmap::kARGB_8888_Config, wid, hei);
        break;
    }

#endif
    /*switch (bits)
    {
    case 32:
        ct = SkColorType::kN32_SkColorType; 
        break;
    case 16:
        ct = SkColorType::kRGB_565_SkColorType; 
        break;
    case 8:
        ct = SkColorType::kAlpha_8_SkColorType; 
        break;
    case 1:
        ct = SkColorType::kAlpha_8_SkColorType; 
        break;
    default:
        ct = SkColorType::kN32_SkColorType; 
        break;
    }

    SkImageInfo sii = SkImageInfo::Make(wid, hei, ct, SkAlphaType::kPremul_SkAlphaType);
    bmp->setInfo(sii);*/
}

bool Bitmap::SetConfig(Int32 wid, Int32 hei, int bits)
{
    Clear();

    //SkColorType ct = SkColorType::kN32_SkColorType;
    SkBitmap* bmp = &(GetBitmapInfo()->bmp);

    __SetConfig(bmp, wid, hei, bits);

    return (!bmp->empty());
}

bool Bitmap::AllocPixel(Color clr)
{
    SkBitmap* bmp = &(GetBitmapInfo()->bmp);

    bmp->allocPixels();
    bmp->eraseColor(clr);
    SetLoaded(true);

    return IsValid();
}

bool Bitmap::AllocPixels()
{
    SkBitmap* bmp = &(GetBitmapInfo()->bmp);

    bmp->allocPixels();
    bmp->eraseColor(0);

    SetLoaded(true);

    return IsValid();
}

/*bool Bitmap::AllocHandle(int wid, int hei, Color clr)
{
    SetConfig(wid, hei, 32);

    SkBitmap* bmp = &(GetBitmapInfo()->bmp);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = wid;
    bmi.bmiHeader.biHeight = -hei;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 4 * wid * hei;
    Byte* rq = NULL;
    HBITMAP hbmp = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (LPVOID*)&rq, NULL, 0);

    SetPixels(rq, (Handle)(LONG_PTR)hbmp, true);
    bmp->eraseColor(clr);
    SetLoaded(true);

    return IsValid();
}*/

bool Bitmap::Create(Int32 wid, Int32 hei, int bits)
{
    if (SetConfig(wid, hei, bits))
    {
        return AllocPixels();
    }
    else
    {
        return false;
    }
}

bool Bitmap::Create(Int32 wid, Int32 hei, Byte* data, int bits)
{
    Clear();

    SkBitmap sbmp;
    SkBitmap* bmp = &(GetBitmapInfo()->bmp);

    __SetConfig(&sbmp, wid, hei, bits);
    sbmp.setPixels(data);
    sbmp.copyTo(bmp);

    return (!sbmp.empty());

    /*if (SetConfig(wid, hei, bits))
    {
        SetPixels(data);
        return true;
    }

    return false;*/
}

bool Bitmap::Backup()
{
    if (NULL != GetPixels())
    {
        if(_backupData == NULL)
        {
            _backupData = new Byte[GetSize()];
        }

        CSSE::MemCopy(_backupData, GetPixels(), GetSize());
        return true;
    }
    else
    {
        return false;
    }
}

void Bitmap::ResetBackup()
{
    if (_backupData)
    {
        delete [] _backupData;
        _backupData = NULL;
    }
}

bool Bitmap::Restore()
{
    if (GetBackupPixels() != NULL)
    {
        SkBitmap* bmp = &(GetBitmapInfo()->bmp);
        CSSE::MemCopy(bmp->getPixels(), _backupData, bmp->getSize());
        return true;
    }
    else
    {
        return false;
    }
}

void Bitmap::SetLoaded(bool bLoad)
{
    if (bLoad)
    {
        _flag |= 0x4;
    }
    else
    {
        _flag &= ~0x4;
    }
}

void Bitmap::EraseColor(Color clr)
{
    if (IsValid())
    {
        GetBitmapInfo()->bmp.eraseColor(clr);
        //CSSE::MemSetDWord(clr, GetPixels(), GetSize());
    }
}

void Bitmap::EraseRect(Rect rect, Color clr)
{
    if (IsValid())
    {
        GetBitmapInfo()->bmp.eraseArea(SkIRect::MakeLTRB(rect.left, rect.top, rect.right, rect.bottom), clr);
    }
}

void Bitmap::EraseGray()
{
    if (IsValid())
    {
        CSSE::DoGray(GetPixels(), GetSize());
    }
}

void Bitmap::EraseHSL(int h, int s, int l)
{
    if (IsValid())
    {
        CSSE::SetHSL(GetPixels(), GetSize(), h, s, l);
    }
}

void Bitmap::SetColor(Int32 x, Int32 y, Color clr)
{
    EraseRect(Rect(x,y,1,1), clr);
}

Color Bitmap::GetColor(Int32 x, Int32 y)
{
    return GetBitmapInfo()->bmp.getColor(x, y);
}

void Bitmap::SetPixels(Byte* pixels)
{
    GetBitmapInfo()->bmp.setPixels(pixels);
}

//void Bitmap::EraseColor(Color clr)
//{
//    SkBitmap* bmp = reinterpret_cast<SkBitmap*>(static_cast<LONG_PTR>(_data));
//    bmp->eraseColor(clr);
//}

void Bitmap::MirrorHori()
{
    if (IsValid())
    {
        CSSE::MirrorX(GetPixels(), Width(), Height());
    }
}

void Bitmap::MirrorVert()
{
    if (IsValid())
    {
        CSSE::MirrorY(GetPixels(), Width(), Height());
    }
}

bool Bitmap::IsBackup() const
{
    return (NULL != _backupData);
}

bool Bitmap::IsLoaded() const
{
    return ((_flag & 0x4) != 0);
}

void Bitmap::Clear()
{
    SkBitmap* bmp = &(GetBitmapInfo()->bmp);

    if (!bmp->isNull())
    {
        bmp->setPixels(NULL);
        bmp->reset();

        if(_backupData)
        {
            delete [] _backupData;
            _backupData = NULL;
        }
    }

    SetLoaded(false);
}

bool Bitmap::IsValid() const
{
    return (!(GetBitmapInfo()->bmp.isNull())); 
}

BitmapInfo* Bitmap::GetBitmapInfo() const
{
    /*if (((_flag & 0x4) == 0) && !_uri.GetResourcePath().Empty())
    {
        Load(&_uri);
    }*/
    return _bmpInfo;
}

Handle Bitmap::ToHandle(Bitmap* bitmap)
{
    HBITMAP hBitmap = NULL;
    int w = bitmap->Width();
    int h = bitmap->Height();

    BITMAP bmp;

    bitmap->LockPixels();

    bmp.bmType = 0;
    bmp.bmWidth  = w;
    bmp.bmHeight = h;
    bmp.bmWidthBytes = w << 2;
    bmp.bmPlanes = 1;
    bmp.bmBitsPixel = 32;
    bmp.bmBits = bitmap->GetPixels();

    hBitmap = CreateBitmapIndirect(&bmp);

    bitmap->UnlockPixels();

    return (Handle)(DWORD_PTR)hBitmap;
}

int Bitmap::Width() const
{
    return GetBitmapInfo()->bmp.width();
}

int Bitmap::Height() const
{
    return GetBitmapInfo()->bmp.height();
}

Uint32 Bitmap::GetSize() const
{
    return (GetRowBytes() * Height());
}

int Bitmap::GetRowBytes() const
{
    return GetBitmapInfo()->bmp.rowBytes();
}

int Bitmap::GetBitsPerPixel() const
{
    return GetBitmapInfo()->bmp.bytesPerPixel();
}

Byte* Bitmap::GetPixels() const 
{ 
    return (Byte*)GetBitmapInfo()->bmp.getPixels(); 
}

void Bitmap::LockPixels()
{
    GetBitmapInfo()->bmp.lockPixels();
}

void Bitmap::UnlockPixels()
{
    GetBitmapInfo()->bmp.unlockPixels();
}

}
