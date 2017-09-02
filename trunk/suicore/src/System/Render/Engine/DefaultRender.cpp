
#include <System/Windows/CoreTool.h>
#include <Framework/Animation/Storyboard.h>
#include <System/Tools/HwndHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <Framework/Controls/Border.h>

#include <System/Graphics/ImageBrush.h>
#include <System/Graphics/Bitmaptool.h>
#include <System/Graphics/Geometry.h>

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
    //pen.SetColor(ARGB(255,255,0,0));
    pen.SetThickness(2);
    drawing->DrawRect(DrawCtx::DefDraw, NULL, &pen, &child->GetRenderRect().TofRect());
}

void DefRender::RenderCanvas(FrameworkElement* root, Handle hdc, Bitmap* canvas, Handle h, fRect clip, Point pt, bool bDebugLine)
{
    DWORD dwBegin = Environment::GetSystemTick();
    //Rect rc = clip.ToRect();

    VisualHost* visualHost = VisualHost::GetVisualHost(root);
    SkiaDrawing drawing(h, HwndHelper::IsAllowsTransparency(root), canvas, clip);

    drawing.Save();
    drawing.ClipRect(&clip, ClipOp::OpIntersect);

    if (root->GetClip())
    {
        drawing.ClipGeometry(root->GetClip(), ClipOp::OpIntersect, true);
    }

    root->Render(&drawing);

    /*String strMPF(_U("欢迎使用MPF"));
    FormattedText fmt;
    fRect rcMPF(0, 0, 100, 32);
    fmt.SetColor(ARGB(11,0,0,0));
    drawing.DrawString(&fmt, strMPF.c_str(), strMPF.Length(), &rcMPF);*/

    drawing.Restore();
}

}
