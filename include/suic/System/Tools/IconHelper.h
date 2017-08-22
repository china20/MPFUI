
#ifndef _UIICONHELPER_H_
#define _UIICONHELPER_H_

#include <System/Graphics/Bitmap.h>

namespace suic
{

class SUICORE_API IconHelper
{
public:

    static HCURSOR CreateHCursorFromBitmap(const Bitmap* bmp);
    static HICON CreateHIconFromBitmap(const Bitmap* bmp, bool bSmall);

protected:

    static void EnsureSystemMetrics();
    static int AlignToBytes(int original, int nBytesCount);
    static Byte* GenerateMaskArray(int width, int height, Byte* colorArray, int colorSize);
    static HICON CreateIconCursor(Byte* colorArray, int colorSize, int width, int height, int xHotspot, int yHotspot, bool isIcon);

private:

     static Size s_iconSize;
     static Size s_smallIconSize;
     static int s_systemBitDepth;
};

}

#endif

