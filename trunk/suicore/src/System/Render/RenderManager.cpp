
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

RenderEngine::RenderEngine(Element* root, const fRect* lprc)
{
    _root = dynamic_cast<FrameworkElement*>(root);

    if (lprc)
    {
        _clip = *lprc;
    }
    else
    {
        _clip.SetLTRB(0, 0, _root->GetActualWidth(), _root->GetActualHeight());
    }
}

void RenderEngine::RenderToDib(FrameworkElement* root, Bitmap* pDib, fRect* lprc)
{
    HWND hwnd = HANDLETOHWND(HwndHelper::GetHostHwnd(root));
    HDC hdc = ::GetDC(hwnd);

    VisualHost* visualHost = VisualHost::GetVisualHost(root);

    Point ptOff;
    fRect rcDraw;
    fRect rcThickness;
    fRect rcClip;

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
    SkiaDrawing drawing(0, HwndHelper::IsAllowsTransparency(root), pDib, rcClip);

    pDib->Clear();
    pDib->Create(rcDraw.right, rcDraw.bottom, 32);

    root->SetVisualOffset(Point());

    drawing.Save();
    drawing.ClipRect(&rcClip, ClipOp::OpIntersect);

    root->Render(&drawing);

    drawing.Restore();
    root->SetVisualOffset(offset);

    ::ReleaseDC(hwnd, hdc);
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

    dib.AllocHandle(rcBounds.Width(), rcBounds.Height(), 0);

    SkiaDrawing subDrawing(0, true, &dib, clip.TofRect());

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

bool RenderEngine::RenderLayerToMemory(VisualHost* pHost, Win32Bitmap& bmp)
{
    DWORD dwTime = Environment::GetSystemTick();
    RenderInfo* pInfo(pHost->GetRenderInfo());
    HWND hwnd = HANDLETOHWND(pHost->GetHandle());
    Rect rectWnd;
    bool bSucc = false;

    ::GetWindowRect(hwnd, &rectWnd);

    _rect = rectWnd.TofRect();

    int cx = _rect.Width();
    int cy = _rect.Height();

    if (CheckRender())
    {
        Win32Bitmap* win32Bmp = pInfo->GetDib();
        Bitmap* canvas = win32Bmp->GetBitmap();
        HDC hdc = ::GetDC(hwnd);

        if (pInfo->IsNeedRender() || !canvas->IsValid() || 
            (cx == (int)_clip.Width() && cy == (int)_clip.Height()))
        {
            pInfo->SetNeedRender(false);
            win32Bmp->CreateBmp(hdc, cx, cy);
            canvas = win32Bmp->GetBitmap();
        }
        else
        {
            canvas->EraseRect(_clip.ToRect(), 0);
        }

        if (!canvas->IsValid())
        {
            ::ReleaseDC(hwnd ,hdc);
            return false;
        }

        DefRender::RenderCanvas(_root, HDCTOHANDLE(hdc), canvas, HDCTOHANDLE(win32Bmp->GetHBitmap())
            , _clip, Point(), pInfo->DrawDebugLine());

        ::ReleaseDC(hwnd ,hdc);

        bmp.Clear();
        bmp.GetBitmap()->SetConfig(canvas->Width(), canvas->Height(), 32);
        bmp.GetBitmap()->SetPixels(canvas->GetPixels());
        bmp.SetHBitmap(win32Bmp->GetHBitmap(), false);

        bSucc = true;
    }

    return bSucc;
}

bool RenderEngine::RenderToMemory(VisualHost* pHost, Win32Bitmap& bmp)
{
    if (HwndHelper::IsAllowsTransparency(_root))
    {
        return RenderLayerToMemory(pHost, bmp);
    }

    Rect rectWnd;
    DWORD dwTime = Environment::GetSystemTick();
    RenderInfo* pInfo(pHost->GetRenderInfo());

    int cx = _root->GetActualWidth();
    int cy = _root->GetActualHeight();

    bool bSucc = false;
    HWND hwnd = HANDLETOHWND(pHost->GetHandle());

    ::GetWindowRect(hwnd, &rectWnd);

    _rect = rectWnd.TofRect();

    if (CheckRender())
    {
        pInfo->GetDib()->Clear();

        HDC hdc = ::GetDC(hwnd);

        bmp.CreateBmp(hdc, cx, cy);
        DefRender::RenderCanvas(_root, HDCTOHANDLE(hdc), bmp.GetBitmap()
            , HDCTOHANDLE(bmp.GetHBitmap()), _clip, Point(0, 0), pInfo->DrawDebugLine());

        ::ReleaseDC(hwnd ,hdc);

        bSucc = true;
    }

    return bSucc;
}

void RenderEngine::RenderToScreen(VisualHost* visualHost, HDC hdc, Point offset)
{
    Win32Bitmap bmp;

    // 先绘制到内存
    RenderToMemory(visualHost, bmp);

    if (bmp.GetBitmap()->IsValid())
    {
        HWND hwnd(HANDLETOHWND(visualHost->GetHandle()));

        if (HwndHelper::IsAllowsTransparency(_root))
        {
            RenderLayerWindow(_root, bmp, hwnd);
        }
        else
        {
            RenderNormalWindow(offset, bmp, hdc);
        }
    }

    bmp.Clear();
}

void RenderEngine::RenderNormalWindow(Point offset, Win32Bitmap& pDib, HDC hdc)
{
    HDC mdc = CreateCompatibleDC(hdc);
    int w = _clip.Width();
    int h = _clip.Height();

    if (NULL != mdc)
    {
        HBITMAP hOldBitmap = (HBITMAP)::SelectObject(mdc, pDib.GetHBitmap());

        BitBlt(hdc, _clip.left, _clip.top, w, h, mdc, _clip.left, _clip.top, SRCCOPY);

        SelectObject(mdc, hOldBitmap);
        DeleteDC(mdc);
    }
}

void RenderEngine::RenderElementToSurface(FrameworkElement* root, Win32Bitmap& pDib)
{
    HWND hwnd = HANDLETOHWND(HwndHelper::GetHostHwnd(root));
    RenderLayerWindow(root, pDib, hwnd);
}

void RenderEngine::RenderLayerWindow(FrameworkElement* root, Win32Bitmap& pDib, HWND hwnd)
{
    HDC drawdc = ::GetDC(hwnd);
    HDC mdc = CreateCompatibleDC(drawdc);

    if (NULL != mdc)
    {
        HBITMAP hOldBitmap = (HBITMAP)::SelectObject(mdc, pDib.GetHBitmap());

        Rect rect;
        ::GetWindowRect(hwnd, &rect);

        POINT ptsrc = {rect.left, rect.top};
        SIZE szWnd = {rect.Width(), rect.Height()};
        POINT ptself = {0, 0};
        BLENDFUNCTION alBlend = {AC_SRC_OVER, 0, root->GetOpacity() * 255, AC_SRC_ALPHA};

        ::UpdateLayeredWindow(hwnd, NULL, &ptsrc, &szWnd, mdc, &ptself, 0, &alBlend, ULW_ALPHA);

        SelectObject(mdc, hOldBitmap);
        DeleteDC(mdc);
    }

    ::ReleaseDC(hwnd, drawdc);
}

}
