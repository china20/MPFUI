
#include <System/Tools/DIBHelper.h>
#include <System/Tools/SSE.h>

namespace suic
{

void DIBHelper::FreeBytes(suic::Byte* lpBytes)
{
    if (lpBytes != NULL)
    {
        free(lpBytes);
    }
}

suic::Byte* DIBHelper::DIBFromBitmap(HBITMAP bmp, suic::Uint32& size)
{
    // 指向位图信息头结构  
    LPBITMAPINFOHEADER lpbi;

    if (bmp != NULL)
    {
        HDC hDC = NULL;

        // 当前分辨率下每象素所占字节数
        int iBits;

        // 位图中每象素所占字节数
        WORD wBitCount;

        // 定义调色板大小,位图中像素字节大小,位图文件大小,写入文件字节数 
        DWORD dwPaletteSize = 0;
        DWORD dwBmBitsSize = 0;
        DWORD dwDIBSize = 0;
        DWORD dwWritten = 0; 

        // 位图属性结构 
        BITMAP bmi;
        // 位图信息头结构 
        BITMAPINFOHEADER bi;

        // 定义文件，分配内存句柄，调色板句柄 
        HANDLE hPal = NULL;
        HANDLE hOldPal = NULL; 

        // 计算位图文件每个像素所占字节数 
        hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
        iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES); 
        ::DeleteDC(hDC);

        if (iBits <= 1)  
        {
            wBitCount = 1;
        }
        else if (iBits <= 4)  
        {
            wBitCount = 4; 
        }
        else if (iBits <= 8)  
        {
            wBitCount = 8; 
        }
        else     
        {
            wBitCount = 24;
        }

        // 取得Bitmap基本信息
        GetObject(bmp, sizeof(BITMAP), (LPVOID)&bmi);

        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = bmi.bmWidth;
        bi.biHeight = bmi.bmHeight;
        bi.biPlanes = 1;
        bi.biBitCount = wBitCount;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrImportant = 0;
        bi.biClrUsed  = 0;

        dwBmBitsSize = ((bmi.bmWidth * wBitCount + 31) / 32.0f) * 4 * bmi.bmHeight;

        size = dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER);
        // 为位图内容分配内存 
        lpbi = (LPBITMAPINFOHEADER)malloc(size);
        *lpbi = bi;

        // 处理调色板  
        hPal = GetStockObject(DEFAULT_PALETTE); 
        if (hPal) 
        {
            hDC = ::GetDC(NULL); 
            hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE); 
            RealizePalette(hDC); 
        }

        // 获取该调色板下新的像素值 
        GetDIBits(hDC, bmp, 0, (UINT)bmi.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) 
            + dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS); 

        //恢复调色板  
        if (hOldPal) 
        { 
            ::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE); 
            RealizePalette(hDC); 
            ::ReleaseDC(NULL, hDC); 
        }
    }

    return (suic::Byte*)lpbi;
}

HBITMAP DIBHelper::DIBToBitmap(suic::Byte* lpBytes, suic::Uint32 size)
{
    return NULL;
}

HBITMAP DIBHelper::BitmapToHandle(suic::Bitmap* bmp)
{
    HBITMAP hbmp = NULL;
    BITMAPINFO bmi = {0};
    Byte* rq = NULL;

    if (!bmp->IsValid())
    {
        return 0;
    }

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bmp->Width();
    bmi.bmiHeader.biHeight = -bmp->Height();
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = bmp->GetSize();

    hbmp = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (LPVOID*)&rq, NULL, 0);
    CSSE::MemCopy(rq, bmp->GetPixels(), bmp->GetSize());

    return hbmp;
}


}
