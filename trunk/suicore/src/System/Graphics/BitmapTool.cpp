/////////////////////////////////////////////////////////////////////////
// uibitmaptool.cpp

#include <System/Tools/SSE.h>
#include <System/Interop/System.h>
#include <System/Tools/Debug.h>
#include <System/Graphics/Bitmaptool.h>

namespace suic
{

BitmapTool::BitmapTool()
{

}

BitmapTool::~BitmapTool()
{
}

void BitmapTool::CopyFrom(const Bitmap* src, Bitmap* dest)
{
    dest->Create(src->Width(), src->Height(), src->GetBitsPerPixel());
    CSSE::MemCopy(dest->GetPixels(), src->GetPixels(), src->GetSize());
}

void BitmapTool::ClipBitmap(const Bitmap* src, int x, int y, int w, int h, Bitmap* bmp)
{
    bmp->Create(w, h, src->GetBitsPerPixel());
    FillFrom(bmp, 0, 0, src, x, y, w, h);
}

void BitmapTool::SetColor(Bitmap* bmp, Color clr)
{
    if (bmp->IsValid())
    {
        CSSE::MemSetDWord(clr, bmp->GetPixels(), bmp->GetSize());
    }
}

void BitmapTool::SetGray(Bitmap* bmp)
{
    if (bmp->IsValid())
    {
        CSSE::DoGray(bmp->GetPixels(), bmp->GetSize());
    }
}

void BitmapTool::SetHSL(Bitmap* bmp, int h, int s, int l)
{
    if (bmp->IsValid())
    {
        CSSE::SetHSL(bmp->GetPixels(), bmp->GetSize(), h, s, l);
    }
}

void BitmapTool::MirrorHori(Bitmap* bmp)
{
    if (bmp->IsValid())
    {
        CSSE::MirrorX(bmp->GetPixels(), bmp->Width(), bmp->Height());
    }
}

void BitmapTool::MirrorVert(Bitmap* bmp)
{
    if (bmp->IsValid())
    {
        CSSE::MirrorY(bmp->GetPixels(), bmp->Width(), bmp->Height());
    }
}

void BitmapTool::ScaleBitmap(const Bitmap* src, Bitmap* dest, int w, int h)
{
    if (w > src->Width())
    {
        w = src->Width();
    }
    if (h > src->Height())
    {
        h = src->Height();
    }

    Float dRatio = (Float)src->Width()/(Float)src->Height();
    int wid = (int)(dRatio * h);

    if (wid < w)
    {
        h = (int)((Float)w / dRatio);
    }
    else
    {
        w = wid;
    }

    dest->Create(w, h, dest->GetBitsPerPixel());
}

void BitmapTool::FillByColor(Bitmap* bmp, int srcX, int srcY, int cx, int cy, Color clr)
{
    if (cx >= (int)bmp->Width() - srcX)
    {
        cx = bmp->Width() - srcX - 1;
    }

    if (cy >= (int)bmp->Height() - srcY)
    {
        cy = bmp->Height() - srcY - 1;
    }

    if (cx > 0 && cy > 0)
    {
        cx = cx << 2;

        int selfRow = bmp->Width() << 2;
        LPBYTE pData = (LPBYTE)bmp->GetPixels() + (srcX << 2);

        pData += srcY * selfRow;

        for (int i = 0; i < cy; ++i)
        {
            CSSE::MemSetDWord(clr, pData, cx);
            pData += selfRow;
        }
    }
}

void BitmapTool::FillFrom(Bitmap* bmp, int srcX, int srcY, const Bitmap* Other, int otherX, int otherY, int cx, int cy)
{
    if (cx >= (int)bmp->Width() - srcX)
    {
        cx = bmp->Width() - srcX - 1;
    }

    if (cy >= (int)bmp->Height() - srcY)
    {
        cy = bmp->Height() - srcY - 1;
    }

    if (cx <= 0 || cy <= 0)
    {
        return;
    }

    cx = cx << 2;

    int selfRow = bmp->Width() << 2;
    int otherRow = Other->Width() << 2;

    LPBYTE pData = (LPBYTE)bmp->GetPixels() + (srcX << 2);
    LPBYTE pOther = (LPBYTE)Other->GetPixels() + (otherX << 2);

    pData += srcY * selfRow;
    pOther += otherY * otherRow;

    for (int i = 0; i < cy; ++i)
    {
        CSSE::MemCopy(pData, pOther, cx);

        pData += selfRow;
        pOther += otherRow;
    }
}

}
