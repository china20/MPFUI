
#include "skiadrawing.h"
#include <System/Types/Const.h>
#include <System/Types/Structure.h>
#include <System/Graphics/SkiaPaint.h>

#include <System/Interop/System.h>

#include "SkTextOp.h"
#include <Skia/effects/SkGradientShader.h>
#include <Skia/core/SkTypeface.h>

namespace suic
{

SkiaDrawing::SkiaDrawing(Handle h, bool layeredMode)
{
    _bmp = NULL;
    _handle = h;
    _layered.Resize(16);
    _layered.Add(layeredMode);
}

SkiaDrawing::~SkiaDrawing()
{
}

void SkiaDrawing::SetOffset(const fPoint& pt)
{
    SkMatrix matrix;

    _offset.x += pt.x;
    _offset.y += pt.y;

    matrix.setTranslate(pt.x, pt.y);
    GetCanvas()->concat(matrix);
}

void SkiaDrawing::SetBitmap(Bitmap* dib)
{
    SkBitmap* bmp = reinterpret_cast<SkBitmap*>(static_cast<LONG_PTR>(dib->GetData()));
    GetCanvas()->SetBitmap(*bmp);
    _bmp = dib;
}

Handle SkiaDrawing::GetHandle()
{
    return _handle;
}

Bitmap* SkiaDrawing::GetBitmap()
{
    return _bmp;
}

void* SkiaDrawing::GetCanvas(int type)
{
    return &_canvas;
}

int SkiaDrawing::Save()
{
    _layered.Add(_layered[_layered.Length() - 1]);
    return GetCanvas()->save();
}

int SkiaDrawing::SaveLayerAlpha(const fRect* rect, Byte alpha)
{
    _layered.Add(true);

    if (NULL == rect)
    {
        return GetCanvas()->saveLayerAlpha(NULL, alpha);
    }
    else
    {
        SkRect srect = ToSkRect(rect);
        return GetCanvas()->saveLayerAlpha(&srect, alpha);
    }
}

int SkiaDrawing::SaveLayer(const fRect* rect, DrawInfo* drawInfo)
{
    SkPaint& paint = DrawInfoToSkia(drawInfo);

   _layered.Add(true);

    paint.setFilterBitmap(true);

    if (NULL == rect)
    {
        return GetCanvas()->saveLayer(NULL, &paint);
    }
    else
    {
        SkRect srect = ToSkRect(rect);
        return GetCanvas()->saveLayer(&srect, &paint);
    }
}

void SkiaDrawing::Restore()
{
    GetCanvas()->restore();
    _layered.RemoveAt(_layered.Length() - 1);
}

int SkiaDrawing::GetSaveCount()
{
    return GetCanvas()->getSaveCount();
}

void SkiaDrawing::ResotreToCount(int count)
{
    GetCanvas()->restoreToCount(count);
}

Rect SkiaDrawing::GetClipBound()
{
    SkIRect rect;
    GetCanvas()->getClipDeviceBounds(&rect);
    return Rect(rect.left(), rect.top(), rect.width(), rect.height());
}

bool SkiaDrawing::ContainsClip(fRect* clip)
{
    return true;
}

bool SkiaDrawing::ContainsClip(fRRect* clip)
{
    return true;
}

bool SkiaDrawing::ContainsClip(Geometry* clip)
{
    return true;
}

bool SkiaDrawing::ClipRect(const fRect* clip, ClipOp op, bool anti)
{
    SkRect rect = ToSkRect(clip);
    GetCanvas()->clipRect(rect, SkRegion::Op(op), anti);
    return !GetCanvas()->quickReject(rect);
}

bool SkiaDrawing::ClipRound(const fRRect* clip, ClipOp op, bool anti)
{
    SkRRect rrect;
    SkVector raddi[4];

    memcpy(&raddi[0], &clip->radii[0], sizeof(clip->radii));
    rrect.setRectRadii(ToSkRect(&clip->rect), raddi);
    GetCanvas()->clipRRect(rrect, SkRegion::Op(op), anti);
    return true;
}

bool SkiaDrawing::ClipPath(OPath* rgn, ClipOp op, bool anti)
{
    SkPath* path = (SkPath*)rgn->GetData();
    GetCanvas()->clipPath(*path, SkRegion::Op(op), anti);
    return true;
}

bool SkiaDrawing::ClipRegion(Geometry* gmt, ClipOp op, bool anti)
{
    SkPath dst;
    SkRegion* region = (SkRegion*)gmt->GetData();

    region->getBoundaryPath(&dst);
    GetCanvas()->clipPath(dst, SkRegion::Op(op), anti);

    return true;
}

void SkiaDrawing::SetPixel(Int32 x, Int32 y, Color clr)
{
    _bmp->SetColor(x, y, clr);
}

Color SkiaDrawing::GetPixel(Int32 x, Int32 y)
{
    return _bmp->GetColor(x, y);
}

void SkiaDrawing::ReadPixels(Bitmap* dest, Point offset)
{
    SkBitmap* bmp = reinterpret_cast<SkBitmap*>(static_cast<LONG_PTR>(dest->GetData()));
    GetCanvas()->readPixels(bmp, offset.x, offset.y);
}

void SkiaDrawing::WritePixels(const Bitmap* dest, Point casOff)
{
    const SkBitmap* bmp = reinterpret_cast<const SkBitmap*>(static_cast<LONG_PTR>(dest->GetData()));
    GetCanvas()->writePixels(*bmp, casOff.x, casOff.y);
}

void SkiaDrawing::SetMatrix(const Matrix& m, bool bSet)
{
    SkMatrix skm;
    skm.readFromMemory(m.GetBuffer(), 9 * sizeof(float));

    if (bSet)
    {
        GetCanvas()->setMatrix(skm);
    }
    else
    {
        GetCanvas()->concat(skm);
    }
}

void SkiaDrawing::EraseColor(Color color)
{
    GetCanvas()->drawColor(color);
}

void SkiaDrawing::EraseRect(const fRect* rc, Color color)
{
    SkPaint paint;
    paint.setColor(color);
    GetCanvas()->drawIRect(ToSkIRect(rc), paint);
}

void SkiaDrawing::DrawImageBrush(ImageBrush* brush, const fRect* lprc)
{
    Bitmap* pImage = brush->GetImage();

    if (pImage)
    {
        SkRect rcImg(ToSkRect(brush->GetViewBox().TofRect()));
        SkRect drawRect(ToSkRect(*lprc));

        SkBitmap* skbmp = reinterpret_cast<SkBitmap*>(static_cast<LONG_PTR>(pImage->GetData()));
        SkPaint paint;

        paint.setAlpha(brush->GetOpacity());
        paint.setAntiAlias(true);

        if (brush->GetGrey())
        {
            pImage->Backup();
            pImage->EraseGray();
        }

        if (brush->GetStretch() == Stretch::UniformToFill)
        {
            if (TileMode::Tile == brush->GetTileMode())
            {
                for (int y = lprc->top; y < lprc->bottom;)
                {
                    for (int x = lprc->left; x < lprc->right;)
                    {
                        SkRect rect;
                        
                        rect.setXYWH(x, y, rcImg.width(), rcImg.height());
                        GetCanvas()->drawBitmapRect(*skbmp, &rect, rcImg, &paint);
                        x += rcImg.width();
                    }

                    y += rcImg.height();
                }
            }
            else
            {
                DeflatSkRect(rcImg, brush->GetCornerBox());

                if (rcImg.width() > lprc->Width() && rcImg.height() > lprc->Height())
                {
                    rcImg.right = rcImg.left + lprc->Width();
                    rcImg.bottom = rcImg.top + lprc->Height();
                }

                GetCanvas()->drawBitmapRect(*skbmp, &drawRect, rcImg, &paint);
            }
        }
        else if (brush->GetStretch() == Stretch::Uniform)
        {
            DeflatSkRect(rcImg, brush->GetCornerBox());

            drawRect.right = drawRect.left + rcImg.width();
            drawRect.bottom = drawRect.top + rcImg.height();

            GetCanvas()->drawBitmapRect(*skbmp, &drawRect, rcImg, &paint);
        }
        else
        {
            //SkRect rcCorner(ToSkRect(brush->GetCornerBox().TofRect()));
            //GetCanvas()->drawBitmapNine();
        }

        if (brush->GetGrey() > 0)
        {
            pImage->Restore();
            pImage->ResetBackup();
        }
    }
}

void SkiaDrawing::DrawLine(Pen* pen, fPoint pt0, fPoint pt1)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    InitPenIntoSkiaPaint(paint, pen);
    GetCanvas()->drawLine(pt0.x, pt0.y, pt1.x , pt1.y, paint);
}

void SkiaDrawing::InitBrushIntoSkPaint(SkPaint& paint, Brush* brush)
{
    if (brush->GetRTTIType() == ImageBrush::RTTIType())
    {
        ImageBrush* imBrush = (ImageBrush*)brush;
        SkBitmap* skbmp = reinterpret_cast<SkBitmap*>(static_cast<LONG_PTR>(imBrush->GetImage()->GetData()));
        SkShader* shader = SkShader::CreateBitmapShader(*skbmp, SkShader::kRepeat_TileMode, SkShader::kRepeat_TileMode);
        paint.setShader(shader);
        shader->unref();
    }
    else if (brush->GetRTTIType() == SolidColorBrush::RTTIType())
    {
        paint.setColor(brush->ToColor());
    }
}

void SkiaDrawing::InitPenIntoSkiaPaint(SkPaint& paint, Pen* pen)
{
    if (pen != NULL)
    {
        paint.setStrokeWidth(pen->GetThickness());
        paint.setStrokeCap((SkPaint::Cap)pen->GetDashCap());
        paint.setStrokeJoin((SkPaint::Join)pen->GetLineJoin());

        if (pen->GetBrush())
        {
            paint.setColor(pen->GetBrush()->ToColor());
        }

        if (pen->GetDashStyle()->dashes.Length() > 0)
        {
            ;
        }
    }
}

void SkiaDrawing::DrawRect(Brush* brush, Pen* pen, const fRect* rc)
{
    SkRect rect = ToSkRect(rc);

    if (brush != NULL)
    {
        if (brush->GetRTTIType() == ImageBrush::RTTIType())
        {
            DrawImageBrush(RTTICast<ImageBrush>(brush), &rect);
        }
        else
        {
            SkPaint paint;

            paint.setColor(brush->ToColor());
            paint.setStyle(SkPaint::Style::kFill_Style);
            GetCanvas()->drawRect(rect, paint);
        }
    }

    if (pen != NULL)
    {
        SkPaint paint;

        paint.setStyle(SkPaint::Style::kStroke_Style);
        InitPenIntoSkiaPaint(paint, pen);
        GetCanvas()->drawRect(rect, paint);
    }
}

void SkiaDrawing::DrawRRect(Brush* brush, Pen* pen, const fRRect* rc)
{
    SkRRect rrect;
    SkVector raddi[4];

    memcpy(&raddi[0], &rc->radii[0], sizeof(rc->radii));
    rrect.setRectRadii(ToSkRect(&rc->rect), raddi);

    if (brush != NULL)
    {
        if (brush->GetRTTIType() == ImageBrush::RTTIType())
        {
            DrawImageBrush(RTTICast<ImageBrush>(brush), &rc->rect);
        }
        else
        {
            SkPaint paint;

            paint.setColor(brush->ToColor());
            paint.setAntiAlias(true);
            paint.setStyle(SkPaint::Style::kFill_Style);
            GetCanvas()->drawRRect(rrect, paint);
        }
    }

    if (pen != NULL)
    {
        SkPaint paint;

        paint.setStyle(SkPaint::Style::kStroke_Style);
        InitPenIntoSkiaPaint(paint, pen);
        GetCanvas()->drawRRect(rrect, paint);
    }
}

void SkiaDrawing::DrawRoundRect(Brush* brush, Pen* pen, const fRect* rc, Float radiusX, Float radiusY)
{
    SkRect rect = ToSkRect(rc);

    if (brush != NULL)
    {
        if (brush->GetRTTIType() == ImageBrush::RTTIType())
        {
            DrawImageBrush(RTTICast<ImageBrush>(brush), &rect);
        }
        else
        {
            SkPaint paint;

            paint.setColor(brush->ToColor());
            paint.setAntiAlias(true);
            paint.setStyle(SkPaint::Style::kFill_Style);
            GetCanvas()->drawRoundRect(rect, radiusX, radiusY, paint);
        }
    }

    if (pen != NULL)
    {
        SkPaint paint;

        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::Style::kStroke_Style);
        InitPenIntoSkiaPaint(paint, pen);
        GetCanvas()->drawRoundRect(ToSkRect(rc), radiusX, radiusY, paint);
    }
}

void SkiaDrawing::DrawCircle(Brush* brush, Pen* pen, fPoint center, Float radius)
{
    if (brush != NULL)
    {
        if (brush->GetRTTIType() == ImageBrush::RTTIType())
        {
            fRect rect(center.x - radius, center.y - radius, center.x + radius, center.y + radius);

            DrawImageBrush(RTTICast<ImageBrush>(brush), &rect);
        }
        else
        {
            SkPaint paint;

            paint.setColor(brush->ToColor());
            paint.setAntiAlias(true);
            paint.setStyle(SkPaint::Style::kFill_Style);

            GetCanvas()->drawCircle(center.x, center.y, radius, paint);
        }
    }

    if (pen != NULL)
    {
        SkPaint paint;

        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::Style::kStroke_Style);
        InitPenIntoSkiaPaint(paint, pen);
        GetCanvas()->drawCircle(center.x, center.y, radius, paint);
    }
}

void SkiaDrawing::DrawEllipse(Brush* brush, Pen* pen, const fRect* rc)
{
    SkRect rect = ToSkRect(rc);

    if (brush != NULL)
    {
        if (brush->GetRTTIType() == ImageBrush::RTTIType())
        {
            DrawImageBrush(RTTICast<ImageBrush>(brush), &rect);
        }
        else
        {
            SkPaint paint;

            paint.setAntiAlias(true);
            paint.setStyle(SkPaint::Style::kFill_Style);
            InitBrushIntoSkPaint(paint, brush);
            GetCanvas()->drawOval(rect, paint);
        }
    }

    if (pen != NULL)
    {
        SkPaint paint;

        paint.setColor(brush->ToColor());
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::Style::kStroke_Style);

        GetCanvas()->drawOval(rect, paint);
    }
}

void SkiaDrawing::DrawPath(Brush* brush, Pen* pen, OPath* opath)
{
    SkPath* path = (SkPath*)opath->GetData();

    if (brush != NULL)
    {
        SkPaint paint;

        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::Style::kFill_Style);
        InitBrushIntoSkPaint(paint, brush);
        GetCanvas()->drawPath(*path, paint);
    }

    if (pen != NULL)
    {
        SkPaint paint;

        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::Style::kStroke_Style);
        InitPenIntoSkiaPaint(paint, pen);
        GetCanvas()->drawPath(*path, paint);
    }
}

void SkiaDrawing::DrawRegion(Brush* brush, Pen* pen, Geometry* regm)
{
    SkPath path;
    SkRegion* region = (SkRegion*)regm->GetData();

    region->getBoundaryPath(&path);
}

void SkiaDrawing::DrawArc(Brush* brush, Pen* pen, fRect* oval, Float starta, Float sweepa, bool usecenter)
{
    SkRect rect = ToSkRect(oval);

    if (brush != NULL)
    {
        SkPaint paint;

        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::Style::kFill_Style);
        InitBrushIntoSkPaint(paint, brush);
        GetCanvas()->drawArc(rect, starta, sweepa, usecenter, paint);
    }

    if (pen != NULL)
    {
        SkPaint paint;

        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::Style::kStroke_Style);
        InitPenIntoSkiaPaint(paint, pen);
        GetCanvas()->drawArc(rect, starta, sweepa, usecenter, paint);
    }
}

void SkiaDrawing::DrawImage(Bitmap* bmp, const fRect* rcdc, const fRect* rcimg, Byte alpha)
{
    SkBitmap* skbmp = reinterpret_cast<SkBitmap*>(static_cast<LONG_PTR>(bmp->GetData()));
    SkIRect irc = SkIRect::MakeLTRB(rcimg->left, rcimg->top, rcimg->right, rcimg->bottom);
    SkPaint paint;

    if (alpha < 255)
    {
        paint.setAlpha(alpha);
    }

    GetCanvas()->drawBitmapRect(*skbmp, &irc, ToSkRect(rcdc), &paint);
}

void SkiaDrawing::DrawImage(Bitmap* bmp, const fRect* rcdc, const fRect* rcimg, Byte alpha, Color trans)
{
    SkBitmap* skbmp = reinterpret_cast<SkBitmap*>(static_cast<LONG_PTR>(bmp->GetData()));
    SkPaint paint;
    SkIRect irc = SkIRect::MakeLTRB(rcimg->left, rcimg->top, rcimg->right, rcimg->bottom);

    paint.setAlpha(alpha);
    paint.setAntiAlias(true);

    GetCanvas()->drawBitmapRect(*skbmp, &irc, ToSkRect(rcdc), &paint);
}

void SkiaDrawing::DrawString(FormattedText* formattedText, const Char* text, int size, const fRect* rc)
{
    SkRect skrc = ToSkRect(rc);
    SkPaint paint;
    SkTypeface* skface = SkTypeface::CreateFromName(formattedText->GetFontFamily().c_str(), SkTypeface::Style::kNormal);

    paint.setTypeface(skface);
    paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
    paint.setAntiAlias(true);
    paint.setAutohinted(true);
    paint.setLCDRenderText(true);
    paint.setEmbeddedBitmapText(true);

    if (skface->isItalic())
    {
        paint.setTextSkewX((SkScalar)0.4);
    }

    int iFmt = 0;

    if (formattedText->GetSingleLine())
    {
        iFmt |= SkTextOp::TextFormat::tSingleLine;
        SkTextOp::DrawSingleText(GetCanvas(), paint, skrc, text, size, iFmt);
    }
    else
    {
        iFmt |= SkTextOp::TextFormat::tWrapText;
        SkTextOp::DrawText(GetCanvas(), paint, skrc, text, size, iFmt);
    }
}

void SkiaDrawing::MeasureString(TmParam& tm, FormattedText* formattedText, const Char* text, int size)
{
}

fSize SkiaDrawing::MeasureString(DrawInfo* tp, Float w, const Char* text, int size, bool bFlag)
{
    fSize sz;
    int iFmt = 0;
    SkPaint::FontMetrics fm;
    SkPaint& paint = DrawInfoToSkia(tp);
    SkTypeface* skface = paint.getTypeface();

    if (NULL == skface)
    {
        skface = SkTypeface::CreateFromName("Tahoma", SkTypeface::Style::kNormal);
        paint.setTypeface(skface);
    }

    paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
    paint.getFontMetrics(&fm);

    SkScalar lineSpace = CalcLineSpace(fm);

    if (!tp->GetSingleLine())
    {
        SkScalar outLen = 0;
        int iLines = SkTextOp::ComputeWrapTextLineCount(paint, outLen, w, text, size);
        sz.cy = iLines * lineSpace;
        if (iLines == 1)
        {
            sz.cx = paint.measureText(text, size * 2);
        }
        else
        {
            sz.cx = outLen;
        }
    }
    else
    {
        sz.cx = paint.measureText(text, size * 2);
        sz.cy = lineSpace;
    }

    return sz;
}

Size SkDrawMeta::MeasureText(Float w, const Char* text, int size)
{
    Size sz;
    int iFmt = 0;
    SkPaint::FontMetrics fm;
    SkPaint& paint = Paint();
    SkTypeface* skface = paint.getTypeface();

    if (NULL == skface)
    {
        skface = SkTypeface::CreateFromName("Tahoma", SkTypeface::Style::kNormal);
        paint.setTypeface(skface);
    }

    paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
    paint.getFontMetrics(&fm);
    SkScalar outLen = 0;
    int iLines = SkTextOp::ComputeWrapTextLineCount(paint, outLen, w, text, size);

    SkScalar lineSpace = CalcLineSpace(fm);

    sz.cy = iLines * lineSpace;

    if (iLines == 1)
    {
        sz.cx = paint.measureText(text, size * 2);
    }
    else
    {
        sz.cx = outLen;
    }

    return sz;
}

Size SkDrawMeta::MeasureText(Float w, const Char* text, int size, int& realCount)
{
    Size measureSize;
    SkScalar outLen = 0;
    SkPaint::FontMetrics fm;
    SkPaint& paint = Paint();
    SkTypeface* skface = paint.getTypeface();

    if (NULL == skface)
    {
        skface = SkTypeface::CreateFromName("Tahoma", SkTypeface::Style::kNormal);
        paint.setTypeface(skface);
    }

    paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
    paint.getFontMetrics(&fm);
    
    realCount = SkTextOp::ComputeTextLines(paint, outLen, w, text, size);

    measureSize.cy = (LONG)CalcLineSpace(fm);
    measureSize.cx = (LONG)outLen;

    return measureSize;
}

}
