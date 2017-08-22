
#include "stdafx.h"
#include <System/Tools/AlphaHelper.h>

namespace suic
{
;
static HBITMAP __CreateDib(suic::Byte** rq, int w, int h)
{
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 4 * h * w;

    return ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (LPVOID*)rq, NULL, 0);
}

AlphaOp::AlphaOp(suic::Bitmap* bmp, suic::Rect rect)
{
    this->dBmp = bmp;
    this->rect = rect;
}

HDC AlphaOp::GetDrawDc() const
{
    return hdc;
}

void AlphaOp::Backup(Drawing* drawing)
{
    HBITMAP tmbmp = HandleToBitmap(oBmp->GetHandle());

    if (NULL == tmbmp)
    {
        BackupLayer(drawing);
    }
    else
    {
        hdc = CreateCompatibleDC(NULL);
        ohbmp = (HBITMAP)SelectObject(hdc, tmbmp);

        Byte* aData = oBmp->GetPixels();
        aData += oBmp->GetRowBytes() * rect.top;

        for (int y = rect.top; y < rect.bottom; ++y)
        {
            for (int x = rect.left; x < rect.right; ++x)
            {
                suic::Byte* b = aData + (x << 2) + 3;
                *b += 1;
            }

            aData += oBmp->GetRowBytes();
        }
    }
}

void AlphaOp::Restore(Drawing* drawing)
{
    Byte* imData = oBmp->GetPixels();

    if (hbmp != NULL)
    {
        Bitmap dBmp;
        dBmp.SetConfig(rect.Width(), rect.Height(), 32);
        dBmp.SetPixels(bytes);
        drawing->WritePixels(&dBmp, rect.LeftTop());
    }

    imData += rect.top * oBmp->GetRowBytes();

    for (int y = rect.top; y < rect.bottom; ++y)
    {
        for (int x = rect.left; x < rect.right; ++x)
        {
            suic::Byte* b = imData + (x << 2) + 3;
            *b -= 1;
        }

        imData += oBmp->GetRowBytes();
    }

    SelectObject(hdc, ohbmp);
    if (NULL != hbmp)
    {
        ::DeleteObject(hbmp);
    }
    ::DeleteDC(hdc);
}

void AlphaOp::BackupLayer(Drawing* drawing)
{
    Bitmap dBmp;
    int w = rect.Width();
    int h = rect.Height();

    hdc = CreateCompatibleDC(NULL);
    hbmp = __CreateDib(&bytes, w, h);

    dBmp.SetConfig(w, h, 32);
    dBmp.SetPixels(bytes);

    drawing->ReadPixels(&dBmp, rect.LeftTop());

    ::SetViewportOrgEx(hdc, -rect.left, -rect.top, NULL);

    ohbmp = (HBITMAP)SelectObject(hdc, hbmp);

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

//-----------------------------------------------------------------


SelfAlphaOp::SelfAlphaOp(suic::Bitmap* bmp, suic::Rect rect)
{
    this->oBmp = bmp;
    this->rect = rect;
}

HDC SelfAlphaOp::GetDrawDc() const
{
    return hdc;
}

void SelfAlphaOp::Backup(Drawing* drawing)
{
    Bitmap bmp;
    BITMAPINFO bmi = {0};
    int w = rect.Width();
    int h = rect.Height();

    hbmp = __CreateDib(&bytes, w, h);
    hdc = CreateCompatibleDC(NULL);
    ohbmp = (HBITMAP)SelectObject(hdc, hbmp);

    bmp.SetConfig(w, h, 32);
    bmp.SetPixels(bytes);
    drawing->ReadPixels(&bmp, rect.LeftTop());

    ::SetViewportOrgEx(hdc, -rect.left, -rect.top, NULL);
}

void SelfAlphaOp::Restore(Drawing* drawing, Byte alpha)
{
#define ABLEND(o,d) (suic::Byte)((o) * alpha / 255 + (d) * (255 - alpha) / 255)

    Bitmap dBmp;
    Byte* imData = NULL;
    Byte* txtData = NULL;
    
    dBmp.SetConfig(rect.Width(), rect.Height(), 32);
    dBmp.SetPixels(bytes);

    imData = oBmp->GetPixels();
    txtData = dBmp.GetPixels();

    imData += rect.top * oBmp->GetRowBytes();

    for (int y = rect.top; y < rect.bottom; ++y)
    {
        for (int x = rect.left; x < rect.right; ++x)
        {
            suic::Byte* rgb = txtData + (x - rect.left) * 4;
            suic::Byte* orgb = imData + x * 4;

            // 文字区域，做透明度融合
            if ((*(rgb + 3)) == 0)
            {
                suic::Byte a = *(orgb + 3);

                *orgb = ABLEND(*rgb,*orgb);
                *(orgb + 1) = ABLEND(*(rgb+1),*(orgb+1));
                *(orgb + 2) = ABLEND(*(rgb+2),*(orgb+2));
                *(orgb + 3) =ABLEND(alpha, *(orgb+3));
            }
        }

        imData += oBmp->GetRowBytes();
        txtData += dBmp.GetRowBytes();
    }

    SelectObject(hdc, ohbmp);
    DeleteObject(hbmp);
    DeleteDC(hdc);

#undef ABLEND
}

}
