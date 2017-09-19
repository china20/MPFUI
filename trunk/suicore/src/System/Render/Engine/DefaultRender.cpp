
#include <System/Windows/CoreTool.h>
#include <Framework/Animation/Storyboard.h>
#include <System/Tools/HwndHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <Framework/Controls/Border.h>

#include <System/Graphics/ImageBrush.h>
#include <System/Graphics/Bitmaptool.h>

#include <System/Graphics/Geometry.h>
#include <System/Graphics/SolidColorBrush.h>

/*--------内部使用头文件--------*/
#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>

#include <System/Interop/System.h>
#include <System/Interop/InternalOper.h>

#include <System/Render/Engine/DefaultRender.h>
#include <System/Render/RenderManager.h>
#include <System/Render/Skia/SkiaDrawing.h>

#include <System/Windows/Environment.h>

#include <Skia/effects/SkGradientShader.h>
#include <Skia/effects/SkPaintFlagsDrawFilter.h>
#include <System/Render/Skia/BookPage.h>

int g_child_dount = 0;

namespace suic
{

void DefRender::DrawDebugLine(FrameworkElement* child, Drawing* drawing)
{
    Pen pen;
    pen.SetBrush(suic::SolidColorBrush::Red);
    pen.SetThickness(2);
    drawing->DrawRect(DrawCtx::DefDraw, NULL, &pen, &child->GetRenderRect().TofRect());
}

void DefRender::RenderCanvas(FrameworkElement* root, Bitmap* canvas, fRect clip, Point pt, bool bDebugLine)
{
    DWORD dwBegin = Environment::GetSystemTick();

    SkiaDrawing drawing(HwndHelper::IsAllowsTransparency(root), canvas, clip);

    drawing.Save();
    drawing.ClipRect(&clip, ClipOp::OpIntersect);

    if (root->GetClip())
    {
        drawing.ClipGeometry(root->GetClip(), ClipOp::OpIntersect, true);
    }

    root->Render(&drawing);

    drawing.Restore();
}

}
