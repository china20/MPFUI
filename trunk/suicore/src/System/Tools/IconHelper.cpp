
#include <System/Tools/IconHelper.h>

namespace suic
{

Size IconHelper::s_iconSize;
Size IconHelper::s_smallIconSize;
int IconHelper::s_systemBitDepth;

void IconHelper::EnsureSystemMetrics()
{
    if (s_systemBitDepth == 0)
    {
        HDC hDC = GetDC(NULL);

        int num = GetDeviceCaps(hDC, 12) * GetDeviceCaps(hDC, 14);
        if (num == 8)
        {
            num = 4;
        }

        int systemMetrics = GetSystemMetrics(SM_CXSMICON);
        int num3 = GetSystemMetrics(SM_CYSMICON);
        int num4 = GetSystemMetrics(SM_CXICON);
        int num5 = GetSystemMetrics(SM_CYICON);

        s_smallIconSize.cx = systemMetrics;
        s_smallIconSize.cy = num3;

        s_iconSize.cx = num4;
        s_iconSize.cy = num5;

        s_systemBitDepth = num;

        ReleaseDC(NULL, hDC);
    }
}

HCURSOR IconHelper::CreateHCursorFromBitmap(const Bitmap* bmp)
{
    return NULL;
    /*const suic::Bitmap* source = bmp;

    if (source->GetBitsPerPixel() != 32)
    {
        suic::Bitmap bmp;
        bmp.Create(source->Width(), source->Height(), source->GetPixels(), 32);
        source->DrawTo(&bmp, NULL, NULL);
        return CreateIconCursor(bmp.GetPixels(), bmp.GetSize(), bmp.Width(), bmp.Height(), 0, 0, false);
    }
    else
    {
        return CreateIconCursor(source->GetPixels(), source->GetSize(), source->Width(), source->Height(), 0, 0, false);
    }*/
}

HICON IconHelper::CreateHIconFromBitmap(const Bitmap* bmp, bool bSmall)
{
    EnsureSystemMetrics();

    const suic::Bitmap* source = bmp;
    suic::Bitmap bmp32(false);

    if (bSmall)
    {
        bmp32.Create(s_smallIconSize.Width(), s_smallIconSize.Height(), source->GetPixels(), 32);
    }
    else
    {
        bmp32.Create(s_iconSize.Width(), s_iconSize.Height(), source->GetPixels(), 32);
    }

    source->DrawTo(&bmp32, NULL, NULL);

    return CreateIconCursor(bmp32.GetPixels(), bmp32.GetSize(), bmp32.Width(), bmp32.Height(), 0, 0, true);
}

int IconHelper::AlignToBytes(int original, int nBytesCount)
{
    int num = ((int)8) << (nBytesCount - 1);
    return (((((int)original) + (num - 1)) / num) * num);
}

HICON IconHelper::CreateIconCursor(Byte* colorArray, int colorSize, int width, int height, int xHotspot, int yHotspot, bool isIcon)
{
    HBITMAP handle = NULL;
    HBITMAP handle2 = NULL;
    HICON handle3 = NULL;

    BITMAPINFO bitmapInfo = {0};

    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = 0;
    bitmapInfo.bmiHeader.biSizeImage = 4 * width * height;

    Byte* zero = NULL;
    handle = ::CreateDIBSection(NULL, &bitmapInfo, DIB_RGB_COLORS, (LPVOID*)&zero, NULL, 0);;

    if (handle == NULL || NULL == zero)
    {
        return NULL;
    }

    memcpy(zero, colorArray, colorSize);
    Byte* lpvBits = GenerateMaskArray(width, height, colorArray, colorSize);
    handle2 = CreateBitmap(width, height, 1, 1, lpvBits);

    if (handle2 == NULL)
    {
        ::DeleteObject(handle);
    }
    else
    {
        ICONINFO iconInfo;
        iconInfo.fIcon = isIcon;
        iconInfo.xHotspot = xHotspot;
        iconInfo.yHotspot = yHotspot;
        iconInfo.hbmMask = handle2;
        iconInfo.hbmColor = handle;

        handle3 = CreateIconIndirect(&iconInfo);

        ::DeleteObject(handle);
        ::DeleteObject(handle2);
    }

    delete[] lpvBits;

    return handle3;
}

Byte* IconHelper::GenerateMaskArray(int width, int height, Byte* colorArray, int colorSize)
{
    int num = width * height;
    int num2 = AlignToBytes(width, 2) / 8;
    Byte* buffer = new Byte[num2 * height];

    for (int i = 0; i < num; i++)
    {
        int num4 = i % width;
        int num5 = (int) ((Float) i / (Float) width);
        int num6 = (int) ((Float) num4 / (Float) 8);
        Byte num7 = (Byte)(((int) 0x80) >> (num4 % 8));

        if (colorArray[(i * 4) + 3] == 0)
        {
            buffer[num6 + (num2 * num5)] = (Byte) (buffer[num6 + (num2 * num5)] | num7);
        }
        else
        {
            buffer[num6 + (num2 * num5)] = (Byte) (buffer[num6 + (num2 * num5)] & ~num7);
        }

        if ((num4 == (width - 1)) && (width == 8))
        {
            buffer[1 + (num2 * num5)] = 0xff;
        }
    }
    return buffer;
}

}
