// 华勤科技版权所有 2010-2011
// 
// 文件名：threadrender.h
// 功  能：默认绘制方式（串行）
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-12
// 
// ============================================================================

#ifndef _THREADRENDER_H_
#define _THREADRENDER_H_

#include <System/Windows/HwndObject.h>

namespace suic
{

struct RenderItem
{
    FrameworkElement* elem;
    Rect clip;
};

class RenderSplitUnit
{
public:

    Vector<RenderItem> rdItem;
};

class RenderSplit
{
public:

    // 裁剪区域
    Rect splitClip[8];
    RenderSplitUnit rdInfo[8];
};

class ThreadRender
{
public:

    static void RenderCanvas(FrameworkElement* root, Handle hdc, Bitmap* canvas, fRect clip);
    static void RenderVisualTree(FrameworkElement* elem, Drawing * drawing, fRect clip);
    static void RenderChild(FrameworkElement* elem, Drawing* drawing);

protected:

    static void DispatchRender(FrameworkElement* elem, Drawing * drawing);
    static void SplitRenderArea(FrameworkElement* elem, fRect clip, RenderSplit& rdInfo);
};

}

#endif
