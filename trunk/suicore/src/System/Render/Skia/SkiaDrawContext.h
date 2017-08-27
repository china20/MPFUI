// 华勤科技版权所有 2010-2011
// 
// 文件名：skiadrawing.h
// 功  能：使用skia进行绘制。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-08
// 
// ============================================================================

#ifndef _UISKIADRAWCONTEXT_H_
#define _UISKIADRAWCONTEXT_H_

#include <System/Graphics/Bitmap.h>
#include <System/Graphics/ImageBrush.h>
#include <System/Graphics/DrawingContext.h>

#include <System/Tools/SSE.h>
#include <System/Tools/Array.h>
#include <System/Tools/Vector.h>

#include <Skia/core/SkBitmap.h>
#include <Skia/core/SkPaint.h>
#include <Skia/core/SkCanvas.h>

#include <Skia/utils/SkCamera.h>

namespace suic
{

class SkiaDrawingContext : public DrawingContext
{
public:

    SkiaDrawingContext(Handle h, bool layeredMode);
    virtual ~SkiaDrawing();

    virtual Handle GetHandle();
    virtual Bitmap* GetBitmap();
    virtual void SetBitmap(Bitmap* dib);
    virtual void* GetCanvas(int type);
    virtual void SetOffset(const fPoint& pt);
 
    virtual bool IsLayeredMode();

    //--------------------------------------------------------------------
    virtual int Save();
    virtual int SaveLayerAlpha(const fRect* rect, Byte alpha);
    virtual int SaveLayer(const fRect* rect, DrawInfo* drawInfo);
    virtual void Restore();
    virtual int GetSaveCount();
    virtual void ResotreToCount(int count);

    virtual Rect GetClipBound();

    virtual bool ContainsClip(fRect* clip);
    virtual bool ContainsClip(fRRect* clip);
    virtual bool ContainsClip(Geometry* clip);

    virtual bool ClipRect(const fRect* clip, ClipOp op=ClipOp::OpIntersect, bool anti=false);
    virtual bool ClipRound(const fRRect* clip, ClipOp op=ClipOp::OpIntersect, bool anti=false);
    virtual bool ClipRegion(Geometry* gmt, ClipOp op=ClipOp::OpIntersect, bool anti=false);
    virtual bool ClipPath(OPath* gmt, ClipOp op=ClipOp::OpIntersect, bool anti=false);
    //--------------------------------------------------------------------

    virtual void SetPixel(Int32 x, Int32 y, Color clr);
    virtual Color GetPixel(Int32 x, Int32 y);

    virtual void ReadPixels(Bitmap* dest, Point offset);
    virtual void WritePixels(const Bitmap* dest, Point casOff);

    virtual void SetMatrix(const Matrix& m, bool bSet);

    virtual void EraseColor(Color color);
    virtual void EraseRect(const fRect* rc, Color color);

    virtual void DrawLine(Pen* pen, fPoint pt0, fPoint pt1);
    virtual void DrawRect(Brush* brush, Pen* pen, const fRect* rc);
    virtual void DrawRRect(Brush* brush, Pen* pen, const fRRect* rc);
    virtual void DrawRoundRect(Brush* brush, Pen* pen, const fRect* rc, Float radiusX, Float radiusY);
    virtual void DrawCircle(Brush* brush, Pen* pen, fPoint center, Float radius);
    virtual void DrawEllipse(Brush* brush, Pen* pen, const fRect* rc);
    virtual void DrawPath(Brush* brush, Pen* pen, OPath* path);
    virtual void DrawRegion(Brush* brush, Pen* pen, Geometry* regm);
    virtual void DrawArc(Brush* brush, Pen* pen, fRect* oval, Float starta, Float sweepa, bool usecenter);

    virtual void DrawGradient(const fRect* rc, Color first, Color second, bool hori);

    virtual void DrawImage(Bitmap* bmp, const fRect* rcdc, const fRect* rcimg, Byte alpha);
    virtual void DrawImage(sp, Bitmap* bmp, const fRect* rcdc, const fRect* rcimg, Byte alpha, Color trans);

    virtual void DrawString(FormattedText* formattedText, const Char* text, int size, const fRect* rc);
    virtual void MeasureString(TmParam& tm, FormattedText* formattedText, const Char* text, int size);

protected:

    SkIRect ToSkIRect(const fRect* rc)
    {
        return SkIRect::MakeLTRB((int32_t)rc->left, (int32_t)rc->top, (int32_t)rc->right, (int32_t)rc->bottom);
    }

    SkRect ToSkRect(const fRect* rc)
    {
        return SkRect::MakeLTRB(rc->left, rc->top, rc->right, rc->bottom);
    }

    void DeflatSkRect(SkRect& rect, const Rect& dif)
    {
        rect.left += dif.left;
        rect.top += dif.top;
        rect.right -= dif.right;
        rect.bottom -= dif.bottom;
    }

    void InitBrushIntoSkPaint(SkPaint& paint, Brush* brush);
    void InitPenIntoSkiaPaint(SkPaint& paint, Pen* pen);

    void DrawImageBrush(ImageBrush* brush, const fRect* lprc);

protected:

    SkCanvas _canvas;
    Bitmap* _bmp;
    Handle _handle;
    fPoint _offset;
    fPoint _distance;
    
    Array<bool> _layered;
};

inline bool SkiaDrawing::IsLayeredMode()
{
    return _layered[_layered.Length() - 1];
}

inline SkCanvas* SkiaDrawing::GetCanvas()
{
    return &_canvas;
}

}

#endif
