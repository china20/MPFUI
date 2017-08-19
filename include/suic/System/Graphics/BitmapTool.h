// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uibitmaptool.h>
// ��  �ܣ���װͼ��������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-12
// 
// ============================================================================

#ifndef _UIIMAGETOOL_H_
#define _UIIMAGETOOL_H_

#include <System/Graphics/Bitmap.h>

namespace suic
{

/// <summary>
///  ͼ�����ݲ��������װ
/// </summary>
class SUICORE_API BitmapTool
{
public:

    static void CopyFrom(const Bitmap* src, Bitmap* dest);
    static void ClipBitmap(const Bitmap* src, int x, int y, int w, int h, Bitmap* bmp);

    static void SetGray(Bitmap* bmp);
    static void SetColor(Bitmap* bmp, Color clr);  
    static void SetHSL(Bitmap* bmp, int h, int s, int l);

    static void MirrorHori(Bitmap* bmp);
    static void MirrorVert(Bitmap* bmp);

    static void ScaleBitmap(const Bitmap* src, Bitmap* dest, int w, int h);

    static void FillByColor(Bitmap* bmp, int srcX, int srcY, int cx, int cy, Color clr);
    static void FillFrom(Bitmap* bmp, int srcX, int srcY, const Bitmap* Other, int otherX, int otherY, int w, int h);

protected:

    BitmapTool();
    ~BitmapTool();
};

}

#endif
