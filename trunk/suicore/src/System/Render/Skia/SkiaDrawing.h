// 华勤科技版权所有 2008-2022
// 
// 文件名：skiadrawing.h
// 功  能：使用skia进行绘制。
// 
// 作  者：MPF开发组
// 时  间：2010-07-08
// 
// ============================================================================

#ifndef _UISKIADRAWCONTEXT_H_
#define _UISKIADRAWCONTEXT_H_

#include <System/Graphics/Bitmap.h>
#include <System/Graphics/ImageBrush.h>
#include <System/Graphics/Drawing.h>

#include <System/Tools/SSE.h>
#include <System/Tools/Array.h>

#include <System/Interop/System.h>

#include <Skia/core/SkBitmap.h>
#include <Skia/core/SkPaint.h>
#include <Skia/core/SkCanvas.h>

#include <Skia/utils/SkCamera.h>
#include <Skia/core/SkImageFilter.h>

#include <System/Graphics/RadialGradientBrush.h>
#include <System/Graphics/LinearGradientBrush.h>

namespace suic
{

class SkiaDrawing;
const int DEFAULT_DRAW_STACK_SIZE = 256;
typedef void (*lpfnDrawBrush)(DrawCtx*, SkiaDrawing*,Brush*,const SkRect*,const fRect*);

struct CanvasLayer
{
    SkPaint p;
    Bitmap* bmp;
    SkCanvas* cv;

    Byte alpha;
    bool layered;

    CanvasLayer()
        : bmp(NULL)
        , cv(NULL) 
        , alpha(255)
        , layered(false) {}

    ~CanvasLayer() {}

    void Clear()
    {
        if (NULL != cv)
        {
            delete cv;
            cv = NULL;
        }
    }
};

struct DrawLayer
{
    fPoint offset;
    SkImageFilter* filter;
};

struct DrawStack
{
    int capacity;
    int currLayer;

    DrawLayer dl[DEFAULT_DRAW_STACK_SIZE];

    DrawStack()
    {
        currLayer = 0;
        capacity = DEFAULT_DRAW_STACK_SIZE;
    }

    ~DrawStack()
    {
    }

    inline DrawLayer& SaveLayer()
    {
        ++currLayer;
        dl[currLayer].offset = dl[currLayer - 1].offset;
        return dl[currLayer];
    }

    inline void RestoreLayer()
    {
        --currLayer;
    }

    inline DrawLayer& _CurrLayer()
    {
        return dl[currLayer];
    }

    inline DrawLayer& _IndexLayer(int index)
    {
        return dl[index];
    }
};

class SkiaDrawing : public Drawing
{
public:

    SkiaDrawing(bool layeredMode, Bitmap* dib, fRect topClip);
    virtual ~SkiaDrawing();

    //virtual Handle GetHandle();
    virtual Bitmap* GetBitmap();
    virtual fPoint GetOffset();
    //virtual fPoint GetScreenOffset();
    //virtual fPoint GetLayerOffset();
    virtual void* GetCanvas(int type);

    virtual bool IsLayeredMode();

    //--------------------------------------------------------------------
    virtual int Save();
    //virtual int SaveLayered(const fRect* rect, SaveLayerInfo* layInfo);
    virtual void Restore();

    virtual int PushOffset(const fPoint& pt);
    virtual void PopOffset();

    virtual int GetSaveCount();
    //virtual fPoint GetLayerOffset(int iLay);
    virtual fRect GetClipBound();
    virtual fRect GetTopClipBound();

    virtual bool ContainsClip(fRect* clip);
    virtual bool ContainsClip(fRRect* clip);
    virtual bool ContainsClip(Geometry* clip);

    virtual void PushEffect(const Effect* effect);
    virtual void PopEffect();

    //virtual void SetEffects(EffectInfo* effectInfo);
    virtual void SetMatrix(const Matrix* m);
    virtual void ConcatMatrix(const Matrix* m);
    virtual void ResetMatrix();

    virtual void ClipRect(const fRect* clip, ClipOp op);
    virtual void ClipRound(const fRRect* clip, ClipOp op, bool anti);
    virtual void ClipGeometry(Geometry* gmt, ClipOp op, bool anti);
    //--------------------------------------------------------------------

    virtual void SetPixel(Int32 x, Int32 y, Color clr);
    virtual Color GetPixel(Int32 x, Int32 y);

    virtual void ReadPixels(Bitmap* dest, Point offset);
    virtual void WritePixels(Bitmap* dest, Point casOff);

    virtual void EraseColor(DrawCtx* drawCtx, Color color);
    virtual void EraseRect(DrawCtx* drawCtx, Color color, const fRect* rc);
    virtual void EraseRect(DrawCtx* drawCtx, Bitmap* bmp, const fRect* rc);
    virtual void FillRect(DrawCtx* drawCtx, Color color, const fRect* rc);

    virtual void DrawLine(DrawCtx* drawCtx, Pen* pen, fPoint pt0, fPoint pt1);
    virtual void DrawRect(DrawCtx* drawCtx, Brush* brush, Pen* pen, const fRect* rc);
    virtual void DrawRRect(DrawCtx* drawCtx, Brush* brush, Pen* pen, const fRRect* rc);
    virtual void DrawRoundRect(DrawCtx* drawCtx, Brush* brush, Pen* pen, const fRect* rc, Float radiusX, Float radiusY);
    virtual void DrawCircle(DrawCtx* drawCtx, Brush* brush, Pen* pen, fPoint center, Float radius);
    virtual void DrawEllipse(DrawCtx* drawCtx, Brush* brush, Pen* pen, const fRect* rc);
    virtual void DrawPath(DrawCtx* drawCtx, Brush* brush, Pen* pen, PathFigure* path);
    virtual void DrawGeometry(DrawCtx* drawCtx, Brush* brush, Pen* pen, Geometry* regm);
    virtual void DrawArc(DrawCtx* drawCtx, Brush* brush, Pen* pen, fRect* oval, Float starta, Float sweepa, bool usecenter);

    virtual void DrawSprite(DrawCtx* drawCtx, Bitmap* bmp, int x, int y);

    virtual void DrawImage(DrawCtx* drawCtx, Bitmap* bmp, int x, int y);
    virtual void DrawImage(DrawCtx* drawCtx, Bitmap* bmp, const Matrix* m);
    virtual void DrawImage(DrawCtx* drawCtx, Bitmap* bmp, const fRect* rcdc, const fRect* rcimg);
    virtual void DrawImage(DrawCtx* drawCtx, Bitmap* bmp, const fRect* rcdc, const fRect* rcimg, Color trans);

    virtual void DrawString(FormattedText* fmtText, const Char* text, int size, const fRect* rc);
    virtual void MeasureString(TmParam& tm, FormattedText* fmtText, const Char* text, int size);

    static void InitDrawBrushes();

    static void DrawImageBrush(DrawCtx* drawCtx, SkiaDrawing* drawing, Brush* brush, const SkRect* lprc, const fRect* frc);
    static void DrawSolidColorBrush(DrawCtx* drawCtx, SkiaDrawing* drawing, Brush* brush, const SkRect* lprc, const fRect* frc);
    static void DrawLinearGradientBrush(DrawCtx* drawCtx, SkiaDrawing* drawing, Brush* brush, const SkRect* lprc, const fRect* frc);
    static void DrawRadialGradientBrush(DrawCtx* drawCtx, SkiaDrawing* drawing, Brush* brush, const SkRect* lprc, const fRect* frc);

    static SkIRect ToSkIRect(const fRect* rc)
    {
        return SkIRect::MakeLTRB((int32_t)rc->left, (int32_t)rc->top, (int32_t)rc->right, (int32_t)rc->bottom);
    }

    static SkRect* ToSkRect(const fRect* clip)
    {
        return (SkRect*)clip;
    }

    static SkBitmap* BitmapToSk(Bitmap* bmp)
    {
        return  &(bmp->GetBitmapInfo()->bmp);
    }

    void FillRectWith(DrawCtx* drawCtx, Color color, const SkRect* rect);

    inline fRect AdjustfRect(const fRect* clip)
    {
        fRect rect;
        DrawLayer& _currLay = _CurrLayer();

        rect.left = clip->left + _currLay.offset.x;
        rect.top = clip->top + _currLay.offset.y;
        rect.right = clip->right + _currLay.offset.x;
        rect.bottom = clip->bottom + _currLay.offset.y;

        return rect;
    }

    inline SkRect AdjustSkRect(const fRect* clip)
    {
        fRect rect;
        DrawLayer& _currLay = _CurrLayer();

        rect.left = clip->left + _currLay.offset.x;
        rect.top = clip->top + _currLay.offset.y;
        rect.right = clip->right + _currLay.offset.x;
        rect.bottom = clip->bottom + _currLay.offset.y;

        //rect = SystemParameters::TransformToDevice(rect);

        return *((SkRect*)&rect);
    }

    inline SkIRect AdjustSkIRect(const fRect* clip)
    {
        SkIRect rect;
        DrawLayer& _currLay = _CurrLayer();

        rect.fLeft = clip->left + _currLay.offset.x;
        rect.fTop = clip->top + _currLay.offset.y;
        rect.fRight = clip->right + _currLay.offset.x;
        rect.fBottom = clip->bottom + _currLay.offset.y;

        return rect;
    }

    inline SkBitmap* InitBitmap(Bitmap* bmp, DrawCtx* drawCtx, SkPaint& paint)
    {
        SkBitmap* skbmp = BitmapToSk(bmp);

        paint.setAlpha(drawCtx->GetAlpha());

        paint.setStrokeWidth(0);
        paint.setAntiAlias(drawCtx->GetAntiAlias());
        paint.setFilterLevel((SkPaint::FilterLevel)(drawCtx->GetDrawLevel()));

        return skbmp;
    }

    inline void ConvertMatrix(const Matrix* m, SkMatrix& skm)
    {
        DrawLayer& _drawLay = _CurrLayer();
        skm.setTranslate(_drawLay.offset.x, _drawLay.offset.y);
        skm.preConcat(m->GetMatrixInfo()->m);
    }

    static void DeflatSkRect(SkRect& rect, const Rect& dif)
    {
        rect.fLeft += dif.left;
        rect.fTop += dif.top;
        rect.fRight -= dif.right;
        rect.fBottom -= dif.bottom;
    }

    static void DeflatfRect(SkRect& rect, int thickness)
    {
        rect.fLeft += thickness;
        rect.fTop += thickness;
        rect.fRight -= thickness;
        rect.fBottom -= thickness;
    }

protected:

    inline SkCanvas* GetDrawCv()
    {
        return _canvas.cv;
    }

    inline SkPaint& GetDrawPaint()
    {
        return _canvas.p;
    }

    inline DrawLayer& _CurrLayer()
    {
        return _drawStack._CurrLayer();
    }

    void InitPaintPen(SkPaint& paint, Pen* pen);
    void InitPaintBrush(SkPaint& paint, Brush* brush, const SkRect* lprc);

    void ResetSkPaint(SkPaint& p)
    {
        p.setShader(NULL);
        p.setAlpha(255);
        p.setStrokeWidth(0);
        p.setPathEffect(NULL);
    }

protected:

    fRect _topClip;
    DrawStack _drawStack;
    CanvasLayer _canvas;
    static Array<lpfnDrawBrush> _drawBrushes;
};

inline fPoint SkiaDrawing::GetOffset()
{
    return _CurrLayer().offset;
}

inline fRect SkiaDrawing::GetTopClipBound()
{
    return _topClip;
}

//inline fPoint SkiaDrawing::GetLayerOffset()
//{
//    return _DrawLayer().cvi->offset;
//}

inline bool SkiaDrawing::IsLayeredMode()
{
    return _canvas.layered;
}

}

#endif
