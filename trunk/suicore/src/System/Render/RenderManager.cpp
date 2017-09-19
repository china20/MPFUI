
#include <System/Windows/CoreTool.h>
#include <Framework/Animation/Storyboard.h>
#include <System/Graphics/ImageBrush.h>
#include <System/Graphics/Bitmaptool.h>
#include <System/Tools/HwndHelper.h>
#include <System/Tools/VisualTreeOp.h>

/*--------内部使用头文件--------*/
#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>

#include <System/Interop/System.h>
#include <System/Render/RenderManager.h>
#include <System/Interop/InternalOper.h>

#include <System/Render/Engine/DefaultRender.h>
#include <System/Render/Skia/SkiaDrawing.h>

#include <System/Windows/Environment.h>

#include <Skia/effects/SkGradientShader.h>
#include <Skia/effects/SkPaintFlagsDrawFilter.h>
#include <System/Render/Skia/BookPage.h>

namespace suic
{

class CaretProxy;
CaretProxy* GetCaretProxy();

RenderEngine::RenderEngine()
    : _root(NULL)
{
}

void RenderEngine::UpdateDrawRect(VisualHost* pHost, const fRect* lprc)
{
    _root = pHost->GetRootElement();

    if (_root->ReadFlag(CoreFlags::IsArrangeDirty))
    {
        _root->UpdateArrange();
    }

    _clip.SetLTRB(0, 0, _root->GetActualWidth(), _root->GetActualHeight());

    if (lprc != NULL)
    {
        _clip.Intersect(lprc);
    }
}

void RenderEngine::RenderToDib(FrameworkElement* root, Bitmap* pDib, fRect* lprc)
{
    HWND hwnd = HANDLETOHWND(HwndHelper::GetHostHwnd(root));
    VisualHost* visualHost = NULL;

    Point ptOff;
    fRect rcDraw;
    fRect rcThickness;
    fRect rcClip;

    visualHost = VisualHost::GetVisualHost(root);
    rcDraw.SetXYWH(0, 0, root->GetActualWidth(), root->GetActualHeight());        

    if (lprc)
    {
        rcClip = *lprc;
    }
    else
    {
        rcClip = rcDraw;
    }

    Point offset(root->GetVisualOffset());
    SkiaDrawing drawing(HwndHelper::IsAllowsTransparency(root), pDib, rcClip);

    pDib->Clear();
    pDib->Create(rcDraw.right, rcDraw.bottom, 32);

    root->SetVisualOffset(Point());

    drawing.Save();
    drawing.ClipRect(&rcClip, ClipOp::OpIntersect);

    root->Render(&drawing);

    drawing.Restore();
    root->SetVisualOffset(offset);
}

fPoint RenderEngine::RenderShot(Element* elem, Bitmap& dib, Rect clip)
{
    // 元素相对于窗口的偏移
    Point offset = elem->GetCanvasOffset();
    Rect rcBounds;

    if (!elem->IsClipToBounds())
    {
        rcBounds = elem->GetDescendantBounds();
        rcBounds.Offset(-offset.x, -offset.y);
    }
    else
    {
        rcBounds.SetXYWH(0, 0, elem->GetRenderSize().Width(), elem->GetRenderSize().Height());
    }

    fPoint bOffset(-rcBounds.left, -rcBounds.top);

    clip.Offset(bOffset.x, bOffset.y);
    clip.Intersect(&rcBounds);

    rcBounds.right = clip.right;
    rcBounds.bottom = clip.bottom;

    dib.Create(rcBounds.Width(), rcBounds.Height(), 32);
    //dib.AllocHandle(rcBounds.Width(), rcBounds.Height(), 0);

    SkiaDrawing subDrawing(true, &dib, clip.TofRect());

    // 执行绘制
    subDrawing.ClipRect(clip.TofRect(), ClipOp::OpIntersect);
    subDrawing.PushOffset(bOffset);

    elem->OnRender(&subDrawing);
    elem->RenderChildren(&subDrawing);

    return bOffset;
}

bool RenderEngine::CheckRender()
{
    int cx = _rect.Width();
    int cy = _rect.Height();

    if (_clip.left < 0)
    {
        _clip.left = 0;
    }

    if (_clip.top < 0)
    {
        _clip.top = 0;
    }

    if (_clip.right > cx)
    {
        _clip.right = cx;
    }

    if (_clip.bottom > cy)
    {
        _clip.bottom = cy;
    }

    if (_clip.right <= _clip.left || _clip.bottom <= _clip.top)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void RenderEngine::RenderChild(FrameworkElement* elem, Drawing* drawing)
{
    //DefRender::RenderChild(NULL, elem, drawing, false);
}

void RenderEngine::RenderToImage(FrameworkElement* element, Bitmap* dib)
{
    RenderEngine::RenderToDib(element, dib, NULL);
}

Bitmap* RenderEngine::RenderLayerToMemory(VisualHost* pHost)
{
    DWORD dwTime = Environment::GetSystemTick();
    RenderInfo* pInfo(pHost->GetRenderInfo());
    HWND hwnd = HANDLETOHWND(pHost->GetHandle());
    Rect rectWnd;
    Bitmap* canvas = NULL;

    ::GetWindowRect(hwnd, &rectWnd);

    _rect = rectWnd.TofRect();

    int cx = _rect.Width();
    int cy = _rect.Height();

    if (CheckRender())
    {
        canvas = pInfo->GetDib();

        if (pInfo->IsNeedRender() || !canvas->IsValid() || 
            (cx == (int)_clip.Width() && cy == (int)_clip.Height()))
        {
            pInfo->SetNeedRender(false);
            canvas->Create(cx, cy, 32);
        }
        else
        {
            canvas->EraseRect(_clip.ToRect(), 0);
        }

        if (canvas->IsValid())
        {
            DefRender::RenderCanvas(_root, canvas, _clip, Point(), pInfo->DrawDebugLine());
        }
    }

    return canvas;
}

Bitmap* RenderEngine::RenderToMemory(VisualHost* pHost)
{
    if (HwndHelper::IsAllowsTransparency(_root))
    {
        return RenderLayerToMemory(pHost);
    }

    Rect rectWnd;
    DWORD dwTime = Environment::GetSystemTick();
    RenderInfo* pInfo(pHost->GetRenderInfo());
    Bitmap* bmp = pInfo->GetDib();

    int cx = _root->GetActualWidth();
    int cy = _root->GetActualHeight();

    bool bSucc = false;
    HWND hwnd = HANDLETOHWND(pHost->GetHandle());

    ::GetWindowRect(hwnd, &rectWnd);

    _rect = rectWnd.TofRect();

    if (CheckRender())
    {
        bmp->Create(cx, cy, 32);

        DefRender::RenderCanvas(_root, bmp, _clip, Point(0, 0), pInfo->DrawDebugLine());
    }

    return bmp;
}

void RenderEngine::RenderToScreen(VisualHost* visualHost, const fRect* lprc, HDC hdc)
{
    suic::Bitmap* bmp = NULL;

    UpdateDrawRect(visualHost, lprc);

    _root->GetAssigner()->GetTimeManager()->Tick();    

    // 先绘制到内存
    bmp = RenderToMemory(visualHost);

    if (bmp->IsValid())
    {
        HWND hwnd(HANDLETOHWND(visualHost->GetHandle()));

        if (HwndHelper::IsAllowsTransparency(_root))
        {
            RenderLayerWindow(_root, bmp, hwnd);
        }
        else
        {
            RenderNormalWindow(bmp, hdc);
            bmp->Clear();
        }
    }
}

void RenderEngine::RenderNormalWindow(suic::Bitmap* bmp, HDC hdc)
{
    HDC mdc = CreateCompatibleDC(hdc);
    int w = _clip.Width();
    int h = _clip.Height();

    if (NULL != mdc)
    {
        HBITMAP hBitmap = HANDLETOBITMAP(Bitmap::ToHandle(bmp));
        HBITMAP hOldBitmap = (HBITMAP)::SelectObject(mdc, hBitmap);

        BitBlt(hdc, _clip.left, _clip.top, w, h, mdc, _clip.left, _clip.top, SRCCOPY);

        SelectObject(mdc, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(mdc);
    }
}

void RenderEngine::RenderElementToSurface(FrameworkElement* root, Bitmap* bmp)
{
    HWND hwnd = HANDLETOHWND(HwndHelper::GetHostHwnd(root));
    RenderLayerWindow(root, bmp, hwnd);
}

void RenderEngine::RenderLayerWindow(FrameworkElement* root, Bitmap* bmp, HWND hwnd)
{
    HDC drawdc = ::GetDC(hwnd);
    HDC mdc = CreateCompatibleDC(drawdc);

    if (NULL != mdc)
    {
        HBITMAP hBitmap = HANDLETOBITMAP(Bitmap::ToHandle(bmp));
        HBITMAP hOldBitmap = (HBITMAP)::SelectObject(mdc, hBitmap);

        Rect rect;
        ::GetWindowRect(hwnd, &rect);

        POINT ptsrc = {rect.left, rect.top};
        SIZE szWnd = {rect.Width(), rect.Height()};
        POINT ptself = {0, 0};
        BLENDFUNCTION alBlend = {AC_SRC_OVER, 0, root->GetOpacity() * 255, AC_SRC_ALPHA};

        ::UpdateLayeredWindow(hwnd, NULL, &ptsrc, &szWnd, mdc, &ptself, 0, &alBlend, ULW_ALPHA);

        SelectObject(mdc, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(mdc);
    }

    ::ReleaseDC(hwnd, drawdc);
}

}
