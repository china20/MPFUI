
#include <System/Render/Skia/SkiaDrawing.h>

#include <System/Types/Const.h>
#include <System/Types/Structure.h>
#include <System/Graphics/SolidcolorBrush.h>

#include <System/Tools/CoreHelper.h>
#include <System/Interop/System.h>

#include <System/Render/Skia/SkTextOp.h>
#include <Skia/effects/SkDashPathEffect.h>
#include <Skia/effects/SkGradientShader.h>
#include <Skia/core/SkTypeface.h>
#include <Skia/core/SkColorShader.h>

namespace suic
{

Array<lpfnDrawBrush> SkiaDrawing::_drawBrushes;
void DrawImageBrush(DrawCtx* drawCtx, Drawing* drawing, ImageBrush* brImg, const fRect* lprc);

void SkiaDrawing::DrawImageBrush(DrawCtx* drawCtx, SkiaDrawing* drawing, Brush* brush, const SkRect* lprc, const fRect* frc)
{
    suic::DrawImageBrush(drawCtx, drawing, (ImageBrush*)brush, frc);
}

void SkiaDrawing::DrawSolidColorBrush(DrawCtx* drawCtx, SkiaDrawing* drawing, Brush* brush, const SkRect* lprc, const fRect* frc)
{
    SolidColorBrush* clrBrush = (SolidColorBrush*)brush;
    drawing->FillRectWith(drawCtx, clrBrush->ToColor(), lprc);
}

static SkShader* CreateLinearShader(LinearGradientBrush* linearBr, const SkRect* lprc)
{
    SkShader* shader = NULL;
    GradientStops* linearInfo = linearBr->GetGradientStops();
    int iCount = linearInfo->colors.GetCount();

    if (iCount > 0)
    {
        SkPoint pts[2];
        fSize szStart = *((fSize*)(&linearBr->GetStartPoint()));//SystemParameters::TransformToDevice(*((fSize*)(&linearBr->GetStartPoint())));
        fSize szEnd = *((fSize*)(&linearBr->GetEndPoint()));//SystemParameters::TransformToDevice(*((fSize*)(&linearBr->GetEndPoint())));

        pts[0].fX = lprc->left() + lprc->width() * szStart.cx;
        pts[0].fY = lprc->top() + lprc->height() * szStart.cy;

        pts[1].fX = lprc->left() + lprc->width() * szEnd.cx;
        pts[1].fY = lprc->top() + lprc->height() * szEnd.cy;

        shader = SkGradientShader::CreateLinear(pts, &linearInfo->colors[0], &linearInfo->ratios[0], iCount, SkShader::TileMode::kRepeat_TileMode);
    }

    return shader;
}

void SkiaDrawing::DrawLinearGradientBrush(DrawCtx* drawCtx, SkiaDrawing* drawing, Brush* brush, const SkRect* lprc, const fRect* frc)
{
    LinearGradientBrush* linearBr = (LinearGradientBrush*)brush;
    SkShader* shader = CreateLinearShader(linearBr, lprc);

    if (shader != NULL)
    {
        SkPaint paint;
        paint.setShader(shader);
        drawing->GetDrawCv()->drawRect(*lprc, paint);
        shader->unref();
    }
}

static SkShader* CreateRadialShader(RadialGradientBrush* radialBr, const SkRect* lprc)
{
    SkShader* shader = NULL;
    GradientStops* radialInfo = radialBr->GetGradientStops();
    int iCount = radialInfo->colors.Length();

    if (iCount > 0)
    {
        SkPoint pt;
        fSize szOri = *((fSize*)(&radialBr->GetOrigin()));//SystemParameters::TransformToDevice(*((fSize*)(&radialBr->GetOrigin())));

        pt.fX = lprc->left() + lprc->width() * szOri.cx;
        pt.fY = lprc->top() + lprc->height() * szOri.cy;

        if (radialBr->GetRadiusX() <= 0)
        {
            shader = SkGradientShader::CreateSweep(pt.fX, pt.fY, &radialInfo->colors[0], &radialInfo->ratios[0], iCount);
        }
        else
        {
            szOri = fSize(radialBr->GetRadiusX(), radialBr->GetRadiusY());//SystemParameters::TransformToDevice(fSize(radialBr->GetRadiusX(), radialBr->GetRadiusY()));

            Float radiusX = lprc->width() * szOri.cx;
            Float radiusY = lprc->height() * szOri.cy;
            if (FloatUtil::GreaterThan(radiusY, radiusX))
            {
                radiusX = radiusY;
            }

            shader = SkGradientShader::CreateRadial(pt, radiusX, &radialInfo->colors[0], &radialInfo->ratios[0], iCount, SkShader::TileMode::kMirror_TileMode);
        }
    }

    return shader;
}

void SkiaDrawing::DrawRadialGradientBrush(DrawCtx* drawCtx, SkiaDrawing* drawing, Brush* brush, const SkRect* lprc, const fRect* frc)
{
    RadialGradientBrush* radialBr = (RadialGradientBrush*)brush;
    SkShader* shader = CreateRadialShader(radialBr, lprc);

    if (NULL != shader)
    {
        SkPaint paint;
        paint.setShader(shader);
        drawing->GetDrawCv()->drawRect(*lprc, paint);
        shader->unref();
    }
}

void SkiaDrawing::InitDrawBrushes()
{
    _drawBrushes.Add(SkiaDrawing::DrawImageBrush);
    _drawBrushes.Add(SkiaDrawing::DrawSolidColorBrush);    
    _drawBrushes.Add(SkiaDrawing::DrawLinearGradientBrush);
    _drawBrushes.Add(SkiaDrawing::DrawRadialGradientBrush);
}

static bool __findDrawBrush(Brush* brush, lpfnDrawBrush& fnDraw)
{
    switch (brush->GetIndex())
    {
    case Brush::eBrushIndex::biImage:
        fnDraw = &SkiaDrawing::DrawImageBrush;
        break;

    case Brush::eBrushIndex::biSolidBrush:
        fnDraw = &SkiaDrawing::DrawSolidColorBrush;
        break;

    case Brush::eBrushIndex::biLinearBrush:
        fnDraw = &SkiaDrawing::DrawLinearGradientBrush;
        break;

    case Brush::eBrushIndex::biRadialBrush:
        fnDraw = &SkiaDrawing::DrawRadialGradientBrush;
        break;

    default:
        fnDraw = NULL;
    }

    return (NULL != fnDraw);
}

SkiaDrawing::SkiaDrawing(bool layeredMode, Bitmap* dib, fRect topClip)
{
    SkBitmap* bmp = BitmapToSk(dib);
    DrawLayer& dInfo = _CurrLayer();

    _topClip = topClip;

    _canvas.layered = layeredMode;
    _canvas.cv = new SkCanvas(*bmp);
    _canvas.bmp = dib;
}

SkiaDrawing::~SkiaDrawing()
{
    _canvas.Clear();
}

//Handle SkiaDrawing::GetHandle()
//{
//    return _DrawLayer().cvi->h;
//}

Bitmap* SkiaDrawing::GetBitmap()
{
    return _canvas.bmp;
}

void* SkiaDrawing::GetCanvas(int type)
{
    return _canvas.cv;
}

int SkiaDrawing::Save()
{
    DrawLayer& _currLay = _drawStack.SaveLayer();
    _currLay.filter = NULL;
    return GetDrawCv()->save();
}

int SkiaDrawing::PushOffset(const fPoint& pt)
{
    DrawLayer& _currLay = _drawStack.SaveLayer();

    _currLay.filter = NULL;
    _currLay.offset.x += pt.x;
    _currLay.offset.y += pt.y;

    return GetDrawCv()->save();
}

void SkiaDrawing::Restore()
{
    DrawLayer& _currLay = _CurrLayer();

    GetDrawCv()->restore();
    _drawStack.RestoreLayer();
}

void SkiaDrawing::PopOffset()
{
    Restore();
}

void SkiaDrawing::PushEffect(const Effect* effect)
{
    /*if (effect == BlurEffect::RTTIType())
    {
        SkImageFilter* pFilter = SkBlurImageFilter::Create(_sigmaX, _sigmaY);
        paint.setImageFilter(pFilter)->unref();
    }*/
}

void SkiaDrawing::PopEffect()
{
    ;
}

void SkiaDrawing::ConcatMatrix(const Matrix* m)
{
    SkMatrix skm;
    ConvertMatrix(m, skm);
    GetDrawCv()->concat(skm);
}

void SkiaDrawing::ResetMatrix()
{
    GetDrawCv()->resetMatrix();
}

//void SkiaDrawing::SetEffects(EffectInfo* effectInfo)
//{
//
//}

void SkiaDrawing::SetMatrix(const Matrix* m)
{
    SkMatrix skm;
    ConvertMatrix(m, skm);
    GetDrawCv()->setMatrix(skm);
}

int SkiaDrawing::GetSaveCount()
{
    return _drawStack.currLayer + 1;
}

//fPoint SkiaDrawing::GetLayerOffset(int iLay)
//{
//    return _drawStack._IndexLayer(iLay).offset;
//}

fRect SkiaDrawing::GetClipBound()
{
    fRect rect;
    fPoint offset = _CurrLayer().offset;
    GetDrawCv()->getClipBounds((SkRect*)&rect);

    //rect = SystemParameters::TransformFromDevice(rect);
    rect.Offset(-offset.x, -offset.y);
    
    return rect;
}

bool SkiaDrawing::ContainsClip(fRect* clip)
{
    return !GetDrawCv()->quickReject(AdjustSkRect(clip));
}

bool SkiaDrawing::ContainsClip(fRRect* clip)
{
    return true;
}

bool SkiaDrawing::ContainsClip(Geometry* clip)
{
    return true;
}

void SkiaDrawing::ClipRect(const fRect* clip, ClipOp op)
{
    SkRect rect = AdjustSkRect(clip);
    _canvas.cv->clipRect(rect, SkRegion::Op(op), false);
}

void SkiaDrawing::ClipRound(const fRRect* clip, ClipOp op, bool anti)
{
    SkRRect rrect;
    SkVector raddi[4];
    SkRect rect = AdjustSkRect(&(clip->rect));

    memcpy(&raddi[0], &clip->radii[0], sizeof(clip->radii));
    rrect.setRectRadii(rect, raddi);
    GetDrawCv()->clipRRect(rrect, SkRegion::Op(op), anti);
}

void SkiaDrawing::ClipGeometry(Geometry* gmt, ClipOp op, bool anti)
{
    DrawLayer& _currLay = _CurrLayer();
    PathGeometry* pathClip = gmt->GetAsPathGeometry();

    if (NULL != pathClip)
    {
        SkPath& region = pathClip->GetPathFigure()->GetPathInfo()->path;

        region.offset(_currLay.offset.x, _currLay.offset.y);
        _canvas.cv->clipPath(region, SkRegion::Op(op), anti);
        region.offset(-_currLay.offset.x, -_currLay.offset.y);
        pathClip->unref();
    }
}

void SkiaDrawing::SetPixel(Int32 x, Int32 y, Color clr)
{
    DrawLayer& _currLay = _CurrLayer();
    _canvas.bmp->SetColor(x + _currLay.offset.x, y + _currLay.offset.y, clr);
}

Color SkiaDrawing::GetPixel(Int32 x, Int32 y)
{
    DrawLayer& _currLay = _CurrLayer();
    return _canvas.bmp->GetColor(x + _currLay.offset.x, y + _currLay.offset.y);
}

void SkiaDrawing::ReadPixels(Bitmap* dest, Point offset)
{
    SkBitmap* bmp = BitmapToSk(dest);
    GetDrawCv()->readPixels(bmp, offset.x, offset.y);
}

void SkiaDrawing::WritePixels(Bitmap* dest, Point casOff)
{
    SkBitmap* bmp = BitmapToSk(dest);
    GetDrawCv()->writePixels(*bmp, casOff.x, casOff.y);
}

void SkiaDrawing::EraseColor(DrawCtx* drawCtx, Color color)
{
    GetDrawCv()->drawColor(color);
}

void SkiaDrawing::EraseRect(DrawCtx* drawCtx, Color color, const fRect* rc)
{
    SkPaint paint;
    SkIRect rect = AdjustSkIRect(rc);

    paint.setColor(color);
    GetDrawCv()->drawIRect(rect, paint);
}

void SkiaDrawing::EraseRect(DrawCtx* drawCtx, Bitmap* bmp, const fRect* rc)
{

}

void SkiaDrawing::DrawLine(DrawCtx* drawCtx, Pen* pen, fPoint pt0, fPoint pt1)
{
    DrawLayer& _currLay = _CurrLayer();
    SkPaint paint;
    SkRect rect;

    paint.setAntiAlias(pen->GetAntiAlias());

    rect.fLeft = pt0.x + _currLay.offset.x;
    rect.fTop = pt0.y + _currLay.offset.y;
    rect.fRight = pt1.x + _currLay.offset.x;
    rect.fBottom = pt1.y + _currLay.offset.y;

    InitPaintPen(paint, pen);
    InitPaintBrush(paint, pen->GetBrush(), &rect);

    _canvas.cv->drawLine(rect.fLeft, rect.fTop, rect.fRight, rect.fBottom, paint);
}

void SkiaDrawing::InitPaintPen(SkPaint& paint, Pen* pen)
{
    Brush* brush = pen->GetBrush();
    SkDashPathEffect* effPath = NULL;
    int iCount = pen->GetDashStyle()->dashes.Length();

    paint.setStrokeWidth(pen->GetThickness());
    paint.setStrokeCap((SkPaint::Cap)pen->GetDashCap());
    paint.setStrokeJoin((SkPaint::Join)pen->GetLineJoin());

    if (iCount > 0 && (iCount % 2) == 0)
    {
        effPath = SkDashPathEffect::Create(&(pen->GetDashStyle()->dashes[0]), iCount, 0);
        if (NULL != effPath)
        {
            paint.setPathEffect(effPath);
            effPath->unref();
        }
    }
}

void SkiaDrawing::InitPaintBrush(SkPaint& paint, Brush* brush, const SkRect* lprc)
{
    SkShader* shader = NULL;

    if (brush->GetRTTIType() == SolidColorBrush::RTTIType())
    {
        paint.setColor(brush->ToColor());
    }
    else if (brush->GetRTTIType() == ImageBrush::RTTIType())
    {
        ImageBrush* imBrush = (ImageBrush*)brush;
        ImageSource* bmp = imBrush->GetImageSource();
        if (NULL != bmp)
        {
            SkBitmap* skbmp = BitmapToSk(bmp->GetBitmap());
            shader = SkShader::CreateBitmapShader(*skbmp, SkShader::kRepeat_TileMode, SkShader::kRepeat_TileMode);
        }
    }
    else if (brush->GetRTTIType() == LinearGradientBrush::RTTIType())
    {
        shader = CreateLinearShader((LinearGradientBrush*)brush, lprc);
    }
    else if (brush->GetRTTIType() == RadialGradientBrush::RTTIType())
    {
        shader = CreateRadialShader((RadialGradientBrush*)brush, lprc);
    }

    paint.setShader(shader);

    if (shader != NULL)
    {
        shader->unref();
    }
}

void SkiaDrawing::FillRect(DrawCtx* drawCtx, Color color, const fRect* rc)
{
    SkRect rect = AdjustSkRect(rc);

    ResetSkPaint(_canvas.p);

    _canvas.p.setColor(color);
    _canvas.p.setStyle(SkPaint::Style::kStrokeAndFill_Style);
    _canvas.cv->drawRect(rect, _canvas.p);
}

void SkiaDrawing::FillRectWith(DrawCtx* drawCtx, Color color, const SkRect* rect)
{
    ResetSkPaint(_canvas.p);

    _canvas.p.setColor(color);
    _canvas.p.setStyle(SkPaint::Style::kStrokeAndFill_Style);
    _canvas.cv->drawRect(*rect, _canvas.p);
}

void SkiaDrawing::DrawRect(DrawCtx* drawCtx, Brush* brush, Pen* pen, const fRect* rc)
{
    SkRect rect = AdjustSkRect(rc);

    _canvas.p.setAntiAlias(false);

    if (brush != NULL)
    {
        lpfnDrawBrush fnDraw = NULL;

        if (__findDrawBrush(brush, fnDraw))
        {
            _canvas.p.setStyle(SkPaint::Style::kStrokeAndFill_Style);
            fnDraw(drawCtx, this, brush, &rect, rc);
        }
    }

    if (pen != NULL && FloatUtil::GreaterThan(pen->GetThickness(), 0))
    {
        Float thickness = pen->GetThickness() / 2.0f;

        ResetSkPaint(_canvas.p);
        _canvas.p.setStyle(SkPaint::Style::kStroke_Style);

        InitPaintPen(_canvas.p, pen);
        InitPaintBrush(_canvas.p, pen->GetBrush(), &rect);

        rect.fLeft += thickness;
        rect.fTop += thickness;

        thickness += 0.5f;

        rect.fRight -= thickness;
        rect.fBottom -= thickness;

        _canvas.cv->drawRect(rect, _canvas.p);
    }
}

void SkiaDrawing::DrawRRect(DrawCtx* drawCtx, Brush* brush, Pen* pen, const fRRect* rc)
{
    SkRRect rrect;
    SkVector raddi[4];

    memcpy(&raddi[0], &rc->radii[0], sizeof(rc->radii));

    ResetSkPaint(_canvas.p);

    _canvas.p.setAntiAlias(true);

    if (brush != NULL)
    {
        fRect frc = AdjustfRect(&rc->rect);
        SkRect* rect = ToSkRect(frc);

        rrect.setRectRadii(*rect, raddi);
        _canvas.p.setStyle(SkPaint::Style::kStrokeAndFill_Style);

        lpfnDrawBrush fnDraw = NULL;

        if (__findDrawBrush(brush, fnDraw))
        {
            Save();
            _canvas.cv->clipRRect(rrect, SkRegion::Op::kIntersect_Op, true);
            fnDraw(drawCtx, this, brush, rect, &(rc->rect));
            Restore();
        }
        else
        {
            _canvas.p.setColor(brush->ToColor());
            _canvas.cv->drawRRect(rrect, _canvas.p);
        }
    }

    if (pen != NULL && FloatUtil::GreaterThan(pen->GetThickness(), 0))
    {
        Float thickness = pen->GetThickness() / 2.0f;

        fRect frc = AdjustfRect(&rc->rect);
        SkRect* rect = ToSkRect(&frc);

        _canvas.p.setStyle(SkPaint::Style::kStroke_Style);

        InitPaintPen(_canvas.p, pen);
        InitPaintBrush(_canvas.p, pen->GetBrush(), rect);

        rect->fLeft += thickness;
        rect->fTop += thickness;

        thickness += 0.5f;

        rect->fRight -= thickness;
        rect->fBottom -= thickness;

        rrect.setRectRadii(*rect, raddi);

        _canvas.cv->drawRRect(rrect, _canvas.p);
    }
}

void SkiaDrawing::DrawRoundRect(DrawCtx* drawCtx, Brush* brush, Pen* pen, const fRect* rc, Float radiusX, Float radiusY)
{
    fRect frc = AdjustfRect(rc);
    SkRect* rect = ToSkRect(frc);

    ResetSkPaint(_canvas.p);

    _canvas.p.setAntiAlias(true);

    if (brush != NULL)
    {
        lpfnDrawBrush fnDraw = NULL;

        _canvas.p.setStyle(SkPaint::Style::kFill_Style);

        if (__findDrawBrush(brush, fnDraw))
        {
            SkRRect rrect;
  
            rrect.setNinePatch(*rect, radiusX, radiusY, radiusX, radiusY);

            Save();
            _canvas.cv->clipRRect(rrect, SkRegion::Op::kIntersect_Op, true);
            fnDraw(drawCtx, this, brush, rect, rc);
            Restore();
        }
        else
        {
           _canvas.p.setColor(brush->ToColor());
            _canvas.cv->drawRoundRect(*rect, radiusX, radiusY, _canvas.p);
        }
    }

    if (pen != NULL && FloatUtil::GreaterThan(pen->GetThickness(), 0))
    {
        Float thickness = pen->GetThickness() / 2.0f + 0.5;

        InitPaintPen(_canvas.p, pen);
        InitPaintBrush(_canvas.p, pen->GetBrush(), rect);

        DeflatfRect(*rect, thickness);
        _canvas.p.setStyle(SkPaint::Style::kStroke_Style);
        _canvas.cv->drawRoundRect(*rect, radiusX, radiusY, _canvas.p);
    }
}

void SkiaDrawing::DrawCircle(DrawCtx* drawCtx, Brush* brush, Pen* pen, fPoint center, Float radius)
{
    DrawLayer& _currLay = _CurrLayer();

    center.x += _currLay.offset.x;
    center.y += _currLay.offset.y;

    ResetSkPaint(_canvas.p);
    _canvas.p.setAntiAlias(true);

    if (brush != NULL)
    {
        SkRect rect = SkRect::MakeLTRB(center.x - radius, center.y + radius, center.x - radius, center.y + radius);

        InitPaintBrush(_canvas.p, brush, &rect);

        _canvas.p.setStyle(SkPaint::Style::kFill_Style);
        _canvas.cv->drawCircle(center.x, center.y, radius, _canvas.p);
    } 

    if (pen != NULL && FloatUtil::GreaterThan(pen->GetThickness(), 0))
    {
        Float thickness = pen->GetThickness() / 2.0f + 0.5;
        SkRect rect;

        radius -= thickness;
        rect.fLeft = center.x - radius;
        rect.fTop = center.y - radius;
        rect.fRight = center.x + radius;
        rect.fBottom = center.y + radius;

        InitPaintPen(_canvas.p, pen);
        InitPaintBrush(_canvas.p, pen->GetBrush(), &rect);

        _canvas.p.setStyle(SkPaint::Style::kStroke_Style);
        _canvas.cv->drawCircle(center.x, center.y, radius, _canvas.p);
    }
}

void SkiaDrawing::DrawEllipse(DrawCtx* drawCtx, Brush* brush, Pen* pen, const fRect* rc)
{
    fRect frc = AdjustfRect(rc);
    SkRect* rect = ToSkRect(frc);

    ResetSkPaint(_canvas.p);

    _canvas.p.setAntiAlias(true);
    
    if (brush != NULL)
    {
        InitPaintBrush(_canvas.p, brush, rect);

        _canvas.p.setStyle(SkPaint::Style::kFill_Style);
        _canvas.cv->drawOval(*rect, _canvas.p);
    }

    if (pen != NULL && FloatUtil::GreaterThan(pen->GetThickness(), 0))
    {
        Float thickness = pen->GetThickness() / 2.0f + 0.5;
        DeflatfRect(*rect, thickness);

        InitPaintPen(_canvas.p, pen);
        InitPaintBrush(_canvas.p, pen->GetBrush(), rect);

        _canvas.p.setStyle(SkPaint::Style::kStroke_Style);        
        _canvas.cv->drawOval(*rect, _canvas.p);
    }   
}

void SkiaDrawing::DrawPath(DrawCtx* drawCtx, Brush* brush, Pen* pen, PathFigure* pathF)
{
    DrawLayer& _currLay = _CurrLayer();

    SkPath& path = pathF->GetPathInfo()->path;
    const SkRect& rect = path.getBounds();
    ResetSkPaint(_canvas.p);

    path.offset(_currLay.offset.x, _currLay.offset.y);
    _canvas.p.setAntiAlias(true);

    if (brush != NULL)
    {
        InitPaintBrush(_canvas.p, brush, &rect);

        _canvas.p.setStyle(SkPaint::Style::kFill_Style);
        _canvas.cv->drawPath(path, _canvas.p);
    }

    if (pen != NULL && FloatUtil::GreaterThan(pen->GetThickness(), 0))
    {
        InitPaintPen(_canvas.p, pen);
        InitPaintBrush(_canvas.p, pen->GetBrush(), &rect);

        _canvas.p.setStyle(SkPaint::Style::kStroke_Style);
        _canvas.cv->drawPath(path, _canvas.p);
    }

    path.offset(-_currLay.offset.x, -_currLay.offset.y);
}

void SkiaDrawing::DrawGeometry(DrawCtx* drawCtx, Brush* brush, Pen* pen, Geometry* geometry)
{
    if (geometry->GetRTTIType() == PathGeometry::RTTIType())
    {
        PathGeometry* pathClip = (PathGeometry*)geometry;
        DrawPath(drawCtx, brush, pen, pathClip->GetPathFigure());
    }
}

void SkiaDrawing::DrawArc(DrawCtx* drawCtx, Brush* brush, Pen* pen, fRect* oval, Float starta, Float sweepa, bool usecenter)
{
    fRect frc = AdjustfRect(oval);
    SkRect* rect = ToSkRect(frc);

    ResetSkPaint(_canvas.p);

    _canvas.p.setAntiAlias(true);

    if (brush != NULL)
    {
        InitPaintBrush(_canvas.p, brush, rect);

        _canvas.p.setStyle(SkPaint::Style::kStrokeAndFill_Style);
        _canvas.cv->drawArc(*rect, starta, sweepa, usecenter, _canvas.p);
    }

    if (pen != NULL && FloatUtil::GreaterThan(pen->GetThickness(), 0))
    {
        Float thickness = pen->GetThickness() / 2.0f + 0.5;
        DeflatfRect(*rect, thickness);

        InitPaintPen(_canvas.p, pen);
        InitPaintBrush(_canvas.p, pen->GetBrush(), rect);

        _canvas.p.setStyle(SkPaint::Style::kStroke_Style);
        _canvas.cv->drawArc(*rect, starta, sweepa, usecenter, _canvas.p);
    }
}

void SkiaDrawing::DrawSprite(DrawCtx* drawCtx, Bitmap* bmp, int x, int y)
{
    SkPaint paint;
    SkBitmap* skbmp = InitBitmap(bmp, drawCtx, paint);
    DrawLayer& _drawLay = _CurrLayer();

    GetDrawCv()->drawSprite(*skbmp, x + _drawLay.offset.x, y + _drawLay.offset.y, &paint);
}

void SkiaDrawing::DrawImage(DrawCtx* drawCtx, Bitmap* bmp, int x, int y)
{
    SkPaint paint;
    SkBitmap* skbmp = InitBitmap(bmp, drawCtx, paint);
    DrawLayer& _drawLay = _CurrLayer();

    GetDrawCv()->drawBitmap(*skbmp, x + _drawLay.offset.x, y + _drawLay.offset.y, &paint);
}

void SkiaDrawing::DrawImage(DrawCtx* drawCtx, Bitmap* bmp, const Matrix* m)
{
    SkPaint paint;
    SkBitmap* skbmp = InitBitmap(bmp, drawCtx, paint);
    SkMatrix skm;

    Save();

    ConvertMatrix(m, skm);
    GetDrawCv()->drawBitmapMatrix(*skbmp, skm, &paint);

    Restore();
}

void SkiaDrawing::DrawImage(DrawCtx* drawCtx, Bitmap* bmp, const fRect* rcdc, const fRect* rcimg)
{
    SkPaint paint;
    SkBitmap* skbmp = InitBitmap(bmp, drawCtx, paint);

    SkIRect irc = SkIRect::MakeLTRB(rcimg->left, rcimg->top, rcimg->right, rcimg->bottom);
    SkRect rect = AdjustSkRect(rcdc);

    GetDrawCv()->drawBitmapRect(*skbmp, &irc, rect, &paint);
}

void SkiaDrawing::DrawImage(DrawCtx* drawCtx, Bitmap* bmp, const fRect* rcdc, const fRect* rcimg, Color trans)
{
    if (rcdc->right > rcdc->left && rcdc->bottom > rcdc->top)
    {
        SkPaint paint;
        SkBitmap* skbmp = InitBitmap(bmp, drawCtx, paint);
        SkIRect irc = SkIRect::MakeLTRB(rcimg->left, rcimg->top, rcimg->right, rcimg->bottom);
        SkRect rect = AdjustSkRect(rcdc);

        GetDrawCv()->drawBitmapRect(*skbmp, &irc, rect, &paint);
    }
}

void SkiaDrawing::DrawString(FormattedText* fmtText, const Char* text, int size, const fRect* rc)
{
    SkTypeface* skface = NULL;
    SkRect rect = AdjustSkRect(rc);

    const int textFlag = SkPaint::kGenA8FromLCD_Flag | SkPaint::Flags::kAntiAlias_Flag
        | SkPaint::Flags::kEmbeddedBitmapText_Flag;

    Brush* textBrush = fmtText->GetForeground();
    Mulstr fName(fmtText->GetFontFamily().c_str());
    int iSize = (int)CoreHelper::FontPoundToPixel(fmtText->GetFontSize());

    if (fmtText->GetFontWeight() >= FontWeightStyle::fwSemiBold)
    {
        skface = SkTypeface::CreateFromName(fName.c_str(), SkTypeface::Style::kBold);
    }
    else
    {
        skface = SkTypeface::CreateFromName(fName.c_str(), SkTypeface::Style::kNormal);
    }

    SkPaint& p = _canvas.p;

    ResetSkPaint(p);
    //p.setShader(NULL);
    //p.setAlpha(255);

    //p.setStrokeWidth(0);
    p.setStyle(SkPaint::Style::kFill_Style);
    p.setTypeface(skface);
    p.setTextSize(iSize);

    p.setFlags(textFlag);

    p.setTextEncoding(SkPaint::kUTF16_TextEncoding);

    //_canvas.p.setAntiAlias(true);
    //_canvas.p.setAutohinted(true);
    //_canvas.p.setLCDRenderText(true);
    p.setHinting(SkPaint::Hinting::kNormal_Hinting);
    //_canvas.p.setSubpixelText(true);
    //_canvas.p.setEmbeddedBitmapText(true);

    if (skface->isItalic())
    {
        p.setTextSkewX((SkScalar)0.4);
    }
    else
    {
        p.setTextSkewX(0.0f);
    }

    p.setUnderlineText(fmtText->GetUnderline());
    p.setStrikeThruText(fmtText->GetStrikeout());

    if (textBrush->GetRTTIType() == SolidColorBrush::RTTIType())
    {
        p.setColor(textBrush->ToColor());
    }
    else
    {
        InitPaintBrush(p, textBrush, &rect);
    }

    int iFmt = 0;

    switch (fmtText->GetTextTrimming())
    {
    case TextTrimming::tCharacterEllipsis:
        iFmt |= SkTextOp::TextFormat::tEllipsisText;
        break;

    case TextTrimming::tPathEllipsis:
        iFmt |= SkTextOp::TextFormat::tPathEllipsisText;
        break;
    }

    if (fmtText->GetSingleline())
    {
        if (0 == iFmt)
        {
            SkTextOp::DrawSingleText(_canvas.cv, p, rect, text, size);
        }
        else
        {
            iFmt |= SkTextOp::TextFormat::tSingleLine;
            SkTextOp::DrawText(_canvas.cv, p, rect, text, size, iFmt);
        }
    }
    else
    {
        iFmt |= SkTextOp::TextFormat::tWrapText;
        SkTextOp::DrawText(_canvas.cv, p, rect, text, size, iFmt);
    }
}

void SkiaDrawing::MeasureString(TmParam& tm, FormattedText* fmtText, const Char* text, int size)
{
    fmtText->MeasureString(tm, text, size);
}

}
