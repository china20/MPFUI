
#ifndef _UIDIBHELPER_H_
#define _UIDIBHELPER_H_

#include <System/Graphics/Bitmap.h>

namespace suic
{

class SUICORE_API DIBHelper
{
public:

    static void FreeBytes(suic::Byte* lpBytes);
    static suic::Byte* DIBFromBitmap(HBITMAP bmp, suic::Uint32& size);

    static HBITMAP BitmapToHandle(suic::Bitmap* bmp);
    static HBITMAP DIBToBitmap(suic::Byte* lpBytes, suic::Uint32 size);
};

}

#endif
