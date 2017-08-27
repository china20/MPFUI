
#include <System/Windows/CoreTool.h>
#include <Framework/Animation/Storyboard.h>
#include <System/Graphics/ImageBrush.h>
#include <System/Graphics/Bitmaptool.h>
#include <System/Tools/VisualTreeHelper.h>

/*--------内部使用头文件--------*/
#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>

#include <System/Interop/System.h>
#include <System/Interop/Packaged.h>
#include <System/Interop/InternalOper.h>

#include <System/Render/Engine/ThreadRender.h>
#include <System/Render/RenderManager.h>
#include <System/Render/Skia/SkiaDrawing.h>

#include <System/Windows/Environment.h>
#include <System/Windows/Effects.h>

#include <Skia/effects/SkGradientShader.h>
#include <Skia/effects/SkPaintFlagsDrawFilter.h>
#include <System/Render/Skia/BookPage.h>

namespace suic
{

void ThreadRender::RenderChild(FrameworkElement* child, Drawing * drawing)
{
}

void ThreadRender::SplitRenderArea(FrameworkElement* elem, fRect clip, RenderSplit& rdInfo)
{
    ;
}

void ThreadRender::RenderVisualTree(FrameworkElement* elem, Drawing * drawing, fRect clip)
{
    //
    // 计算并分割需要渲染的区域
    //
    RenderSplit rdSplit;
    fRect rects[8];
    int num = 1;

    rects[0] = clip;

    for (int i = 0; i < 3; ++i)
    {
        fRect temp[8];
        int index = 0;

        if (rects[0].Width() > rects[0].Height())
        {
            for (int j = 0; j < num; ++j)
            {
                temp[index] = rects[j];
                temp[index].right = rects[j].Width() / 2;

                ++index;

                temp[index] = temp[index - 1];
                temp[index].left = temp[index].right;
                temp[index].right = rects[j].right;

                ++index;
            }
        }
        else
        {
            for (int j = 0; j < num; ++j)
            {
                temp[index] = rects[j];
                temp[index].bottom = rects[j].Height() / 2;

                ++index;

                temp[index] = temp[index - 1];
                temp[index].top = temp[index].bottom;
                temp[index].bottom = rects[j].bottom;

                ++index;
            }
        }

        memcpy(&rects[0], &temp[0], sizeof(Rect) * index);
    }

    memcpy(&rdSplit.splitClip[0], &rects[0], sizeof(Rect) * 8);

    SplitRenderArea(elem, clip, rdSplit);
}

void ThreadRender::DispatchRender(FrameworkElement* elem, Drawing * drawing)
{
    int iCount = elem->GetVisibleChildrenCount();
    fRect clip(elem->GetRenderRect().TofRect());

    //clip.Deflate(elem->GetPadding());

    drawing->Save();
    drawing->ClipRect(&clip);

    for (int i = 0; i < iCount; ++i)
    {
        FrameworkElement* child(DynamicCast<FrameworkElement>(elem->GetVisibleChild(i)));

        if (child->IsVisible())
        {
            RenderChild(child, drawing);
        }
    }

    drawing->Restore();
}

void ThreadRender::RenderCanvas(FrameworkElement* root, Handle hdc, Bitmap* canvas, fRect clip)
{
    HFONT pOldFont = NULL;

    SkiaDrawing drawing(0);

    drawing.setAutoDelete(false);
    drawing.SetBitmap(canvas);

    drawing.Save();
    drawing.ClipRect(&clip);
    ThreadRender::RenderVisualTree(root, &drawing, clip);
    drawing.Restore();
}

}
