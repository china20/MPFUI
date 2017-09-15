
#include "stdafx.h"
#include <System/Tools/AlphaHelper.h>

namespace suic
{

static HBITMAP __CreateDib(HDC hdc, suic::Byte** rq, int w, int h)
{
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 4 * h * w;

    return ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (LPVOID*)rq, NULL, 0);
}

AlphaOp::AlphaOp(HDC h, suic::Bitmap* bmp, suic::Rect rect)
    : hbmp(NULL)
    , hdc(NULL)
    , bytes(NULL)
{
    this->oBmp = bmp;
    this->rect = rect;
    this->hdc = CreateCompatibleDC(h);
}

AlphaOp::~AlphaOp()
{
    if (hdc != NULL)
    {
        SelectObject(hdc, ohbmp);
        if (NULL != hbmp)
        {
            ::DeleteObject(hbmp);
        }
        ::DeleteDC(hdc);
    }
}

HDC AlphaOp::GetDrawDc() const
{
    return hdc;
}

void AlphaOp::Backup(Drawing* drawing, const Rect& clip)
{
    if (NULL == hbmp)
    {
        BackupLayer(drawing, clip);
    }
    else
    {
        ohbmp = (HBITMAP)SelectObject(hdc, hbmp);

        ::SelectClipRgn(hdc, NULL);
        ::IntersectClipRect(hdc, clip.left, clip.top, clip.right, clip.bottom);

        if (drawing->IsLayeredMode())
        {
            Byte* aData = oBmp->GetPixels();
            aData += oBmp->GetRowBytes() * rect.top;
            int iBot = min(rect.bottom, oBmp->Height());
            int iRight = min(rect.right, oBmp->Width());

            for (int y = rect.top; y < iBot; ++y)
            {
                for (int x = rect.left; x < iRight; ++x)
                {
                    Byte* b = aData + (x << 2) + 3;
                    *b += 1;
                }

                aData += oBmp->GetRowBytes();
            }
        }
    }
}

void AlphaOp::Restore(Drawing* drawing)
{
    Byte* imData = oBmp->GetPixels();

    if (hbmp != NULL)
    {
        fRect rcimg(0, 0, rect.Width(), rect.Height());
        Bitmap dBmp;
        dBmp.SetConfig(rect.Width(), rect.Height(), 32);
        dBmp.SetPixels(bytes);

        drawing->WritePixels(&dBmp, rect.LeftTop());
    }

    if (drawing->IsLayeredMode())
    {
        imData += rect.top * oBmp->GetRowBytes();
        int iBot = min(rect.bottom, oBmp->Height());
        int iRight = min(rect.right, oBmp->Width());

        for (int y = rect.top; y < iBot; ++y)
        {
            for (int x = rect.left; x < iRight; ++x)
            {
                Byte* b = imData + (x << 2) + 3;
                *b -= 1;
            }

            imData += oBmp->GetRowBytes();
        }
    }

    ::SelectClipRgn(hdc, NULL);

    SelectObject(hdc, ohbmp);
    if (NULL != hbmp)
    {
        ::DeleteObject(hbmp);
    }
    ::DeleteDC(hdc);

    hdc = NULL;
}

void AlphaOp::BackupLayer(Drawing* drawing, const Rect& clip)
{
    Bitmap dBmp;
    int w = rect.Width();
    int h = rect.Height();

    hbmp = __CreateDib(hdc, &bytes, w, h);

    dBmp.SetConfig(w, h, 32);
    dBmp.SetPixels(bytes);

    drawing->ReadPixels(&dBmp, rect.LeftTop());

    ::SetViewportOrgEx(hdc, -rect.left, -rect.top, NULL);

    ohbmp = (HBITMAP)SelectObject(hdc, hbmp);

    if (drawing->IsLayeredMode())
    {
        Byte* aData = dBmp.GetPixels();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                suic::Byte* b = aData + (x << 2) + 3;
                *b += 1;
            }

            aData += dBmp.GetRowBytes();
        }
    }

    ::SelectClipRgn(hdc, NULL);
    ::IntersectClipRect(hdc, clip.left, clip.top, clip.right, clip.bottom);
}

//-----------------------------------------------------------------


SelfAlphaOp::SelfAlphaOp(HDC h, suic::Bitmap* bmp, suic::Rect rect)
{
    this->hdc = CreateCompatibleDC(h);
    this->oBmp = bmp;
    this->rect = rect;
}

SelfAlphaOp::~SelfAlphaOp()
{
    if (hdc != NULL)
    {
        SelectObject(hdc, ohbmp);
        if (NULL != hbmp)
        {
            ::DeleteObject(hbmp);
        }
        ::DeleteDC(hdc);
    }
}

HDC SelfAlphaOp::GetDrawDc() const
{
    return hdc;
}

void SelfAlphaOp::Backup(Drawing* drawing, const Rect& clip)
{
    Bitmap bmp;
    BITMAPINFO bmi = {0};
    int w = rect.Width();
    int h = rect.Height();

    hbmp = __CreateDib(hdc, &bytes, w, h);
    ohbmp = (HBITMAP)SelectObject(hdc, hbmp);

    bmp.SetConfig(w, h, 32);
    bmp.SetPixels(bytes);
    drawing->ReadPixels(&bmp, rect.LeftTop());

    ::SetViewportOrgEx(hdc, -rect.left, -rect.top, NULL);

    ::SelectClipRgn(hdc, NULL);
    ::IntersectClipRect(hdc, clip.left, clip.top, clip.right, clip.bottom);
}

void SelfAlphaOp::Restore(Drawing* drawing, Byte alpha)
{
#define ABLEND(o,d,a) (suic::Byte)(((Float)((o) * a) / 255) + ((Float)((d) * (255-a)) / 255))

    Bitmap dBmp;
    Byte* imData = NULL;
    Byte* txtData = NULL;
    Byte ralpha = 255 - alpha;
    
    dBmp.SetConfig(rect.Width(), rect.Height(), 32);
    dBmp.SetPixels(bytes);

    imData = oBmp->GetPixels();
    txtData = dBmp.GetPixels();

    imData += rect.top * oBmp->GetRowBytes();

    int iBot = min(rect.bottom, oBmp->Height());
    int iRight = min(rect.right, oBmp->Width());

    for (int y = rect.top; y < iBot; ++y)
    {
        for (int x = rect.left; x < iRight; ++x)
        {
            Byte* rgb = txtData + (x - rect.left) * 4;
            Byte* orgb = imData + x * 4;

            // 文字区域，做透明度融合
            if ((*(rgb + 3)) == 0)
            {
                /**orgb = *rgb + Math::Round((ralpha * (*orgb)) >> 8); 
                *(orgb + 1) = *(rgb+1) + Math::Round((ralpha * (*(orgb + 1))) >> 8); 
                *(orgb + 2) = *(rgb+2) + Math::Round((ralpha * (*(orgb + 2))) >> 8); 
                *(orgb + 3) = *(rgb+3) + Math::Round((ralpha * (*(orgb + 3))) >> 8);
                */

                Byte a = (ABLEND(alpha,*(orgb + 3),alpha));

                *orgb = (ABLEND(*rgb,*orgb,alpha));
                *(orgb + 1) = (ABLEND(*(rgb+1),*(orgb+1),alpha));
                *(orgb + 2) = (ABLEND(*(rgb+2),*(orgb+2),alpha));
                //*(orgb + 3) = a;
                //*(orgb + 3) = Math::Round((ralpha * (*(orgb + 3))) >> 8);
            }
        }

        imData += oBmp->GetRowBytes();
        txtData += dBmp.GetRowBytes();
    }

    ::SelectClipRgn(hdc, NULL);

    SelectObject(hdc, ohbmp);
    DeleteObject(hbmp);
    DeleteDC(hdc);

    hdc = NULL;

#undef ABLEND
}

}
