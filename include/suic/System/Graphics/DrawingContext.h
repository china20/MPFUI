// 华勤科技版权所有 2010-2011
// 
// 文件名：UIDrawing.h
// 功  能：定义绘制接口
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-12
// 
// ============================================================================

#ifndef _UIDRAWIN_H_
#define _UIDRAWIN_H_

#include <System/Graphics/Bitmap.h>
#include <System/Graphics/Geometry.h>
#include <System/Animation/Transform.h>
#include <System/Graphics/FormattedText.h>

namespace suic
{

/// <summary>
///  绘制接口类，完成一般的绘制，线的颜色采用标准的四色表示
/// </summary>
class SUICORE_API NOVTABLE Drawing : public Object
{
public:

    virtual ~Drawing() {};

    virtual Handle GetHandle() = 0;
    virtual Bitmap* GetBitmap() = 0;
    virtual void SetBitmap(Bitmap* dib) = 0;
    virtual void* GetCanvas(int type) = 0;
    virtual void SetOffset(const fPoint& pt) = 0;

    virtual bool IsLayeredMode() = 0;
    
    virtual int Save() = 0;
    virtual int SaveLayerAlpha(const fRect* rect, Byte alpha) = 0;
    virtual int SaveLayer(const fRect* rect, DrawInfo* drawInfo) = 0;
    virtual void Restore() = 0;
    virtual int GetSaveCount() = 0;

    virtual Rect GetClipBound() = 0;

    virtual bool ContainsClip(fRect* clip) = 0;
    virtual bool ContainsClip(fRRect* clip) = 0;
    virtual bool ContainsClip(Geometry* clip) = 0;

    virtual bool ClipRect(const fRect* clip, ClipOp op=ClipOp::OpIntersect, bool anti=false) = 0;
    virtual bool ClipRound(const fRRect* clip, ClipOp op=ClipOp::OpIntersect, bool anti=false) = 0;
    virtual bool ClipRegion(Geometry* gmt, ClipOp op=ClipOp::OpIntersect, bool anti=false) = 0;
    virtual bool ClipPath(Path* rgn, ClipOp=ClipOp::OpIntersect, bool anti=false) = 0;

    virtual void SetPixel(Int32 x, Int32 y, Color clr) = 0;
    virtual Color GetPixel(Int32 x, Int32 y) = 0;

    virtual void ReadPixels(Bitmap* dest, Point offset) = 0;
    virtual void WritePixels(const Bitmap* dest, Point casOff) = 0;

    virtual void SetMatrix(const Matrix& m, bool bSet) = 0;

    virtual void EraseColor(Color color) = 0;
    virtual void EraseRect(const fRect* rc, Color color) = 0;

    virtual void DrawLine(Pen* pen, fPoint pt0, fPoint pt1) = 0;
    virtual void DrawRect(Brush* brush, Pen* pen, const fRect* rc) = 0;
    virtual void DrawRRect(Brush* brush, Pen* pen, const fRRect* rc) = 0;
    virtual void DrawRoundRect(Brush* brush, Pen* pen, const fRect* rc, Float radiusX, Float radiusY) = 0;
    virtual void DrawCircle(Brush* brush, Pen* pen, fPoint center, Float radius) = 0;
    virtual void DrawEllipse(Brush* brush, Pen* pen, const fRect* rc) = 0;
    virtual void DrawPath(Brush* brush, Pen* pen, OPath* path) = 0;
    virtual void DrawRegion(Brush* brush, Pen* pen, Geometry* regm) = 0;
    virtual void DrawArc(Brush* brush, Pen* pen, fRect* oval, Float starta, Float sweepa, bool usecenter) = 0;

    virtual void DrawImage(Bitmap* bmp, const fRect* rcdc, const fRect* rcimg, Byte alpha) = 0;
    virtual void DrawImage(Bitmap* bmp, const fRect* rcdc, const fRect* rcimg, Byte alpha, Color trans) = 0;

    virtual void DrawString(FormattedText* formattedText, const Char* text, int size, const fRect* rc) = 0;
    virtual void MeasureString(TmParam& tm, FormattedText* formattedText, const Char* text, int size) = 0;
};

typedef shared<Drawing> DrawingPtr;

};

#endif
