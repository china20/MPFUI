// 华勤科技版权所有 2010-2011
// 
// 文件名：AlphaHelper.h>
// 功  能：Alpha帮助类。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIALPHAHELPER_H_
#define _UIALPHAHELPER_H_

#include <System/Windows/Object.h>
#include <System/Graphics/Bitmap.h>

namespace suic
{

class SUICORE_API AlphaOp
{
public:

    AlphaOp(HDC h, Bitmap* bmp, Rect rect);
    ~AlphaOp();

    HDC GetDrawDc() const;
    void Backup(Drawing* drawing, const Rect& clip);
    void Restore(Drawing* drawing);

private:

    void BackupLayer(Drawing* drawing, const Rect& clip);

private:

    HDC hdc;
    HBITMAP hbmp;
    HBITMAP ohbmp;

    Rect rect;
    Byte* bytes;
    Bitmap* oBmp;
};

class SUICORE_API SelfAlphaOp
{
public:

    SelfAlphaOp(HDC h, Bitmap* bmp, Rect rect);
    ~SelfAlphaOp();

    HDC GetDrawDc() const;
    void Backup(Drawing* drawing, const Rect& clip);
    void Restore(Drawing* drawing, Byte alpha);

private:

    HDC hdc;
    HBITMAP hbmp;
    HBITMAP ohbmp;

    Rect rect;
    Byte* bytes;
    Bitmap* oBmp;
};

}

#endif
