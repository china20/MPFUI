
#include <System/Types/Types.h>
#include <System/Windows/Object.h>

#include <Skia/core/SkPreConfig.h>
#include <Skia/core/SkCanvas.h>
#include <Skia/core/SkRect.h>
#include <Skia/core/SkStream.h>
#include <Skia/core/SkPoint.h>
#include <Skia/effects/SkGradientShader.h>
#include <Skia/core/SkPath.h>
#include <Skia/effects/SkColorMatrix.h>
#include <Skia/effects/SkColorMatrixFilter.h>
#include <Skia/utils/skcamera.h>

#define PI (3.141592653)

using namespace suic;

class BookPage
{
public:

    BookPage();
    ~BookPage();

    void InitBookPage(int w, int h);
    void ClearBookPage();

    void CreateShader();
    void CalcCornerXY(Float x,Float y);
    SkPoint GetCross(SkPoint p1,SkPoint p2,SkPoint p3,SkPoint p4);
    void CalcPoints();

    void DrawCurrentPageArea(SkCanvas *canvas, SkBitmap *bitmap);
    void DrawNextPageAreaAndShadow(SkCanvas *canvas, SkBitmap *bitmap);
    void SetBitmaps(SkBitmap *bm1, SkBitmap *bm2);

    void SetScreen(int w, int h);
    void DrawCurrentPageShadow(SkCanvas *canvas);
    void DrawCurrentBackArea(SkCanvas *canvas, SkBitmap *bitmap);
    void StartAnimation(int delayMillis);
    void AbortAnimation();
    BOOL CanDragOver();
    BOOL DragToRight();

    void DrawPage(SkCanvas *canvas, SkPoint touch);
    BOOL LButtonDown(SkCanvas *canvas, SkPoint pt);
    BOOL MouseMove(SkCanvas *canvas, SkPoint pt);
    BOOL LButtonUp(SkCanvas *canvas, SkPoint pt);

    int _cornerX ;  // 拖拽点对应的页脚
    int _cornerY ;

private:

    int _width;     // 屏幕宽度
    int _height;    // 屏幕高度

    SkPath mPath0;  //裁剪路径
    SkPath mPath1;

    SkBitmap *_curPage ;            // 当前页,由使用者传入
    SkBitmap *_nextPage ;

    SkPoint _touchPoint;            // 拖拽点
    SkPoint _bezierStartOne;        // 贝塞尔曲线起始点
    SkPoint _bezierControlOne;      // 贝塞尔曲线控制点
    SkPoint _bezierVertexOne;       // 贝塞尔曲线顶点
    SkPoint _bezierEndOne;          // 贝塞尔曲线结束点

    SkPoint _bezierStartTwo;        // 另一条贝塞尔曲线
    SkPoint _bezierControlTwo;
    SkPoint _bezierVertexTwo;
    SkPoint _bezierEndTwo;

    Float _middleX;
    Float _middleY;
    Float _degrees;
    Float _touchToCornerDis;
    SkColorMatrixFilter *_colorMatrixFilter;
    SkMatrix _matrix;

    bool mIsRTandLB;            // 是否属于右上左下
    Float _matrixArray[9] ;
    Float _maxLength ;          // 屏幕对角线

    SkShader *mBackShadowDrawableLR;
    SkShader *mBackShadowDrawableRL;
    SkShader *mFolderShadowDrawableLR;
    SkShader *mFolderShadowDrawableRL;

    SkShader *mFrontShadowDrawableHBT;
    SkShader *mFrontShadowDrawableHTB;
    SkShader *mFrontShadowDrawableVLR;
    SkShader *mFrontShadowDrawableVRL;

    SkPaint _paint;
};
