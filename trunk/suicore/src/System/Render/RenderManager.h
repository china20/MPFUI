// 华勤科技版权所有 2008-2022
// 
// 文件名：rendermanager.h
// 功  能：管理系统的绘制
// 
// 作  者：MPF开发组
// 时  间：2010-07-12
// 
// ============================================================================

#ifndef _RENDERMANAGER_H_
#define _RENDERMANAGER_H_

#include <System/Windows/HwndObject.h>
#include <System/Windows/RenderInfo.h>

#include <System/Animation/TimerManager.h>

namespace suic
{

class RenderEngine
{
public:

    RenderEngine(Element* root, const fRect* lprc);
    
    bool RenderToMemory(VisualHost* pHost, Win32Bitmap& bmp);
    bool RenderLayerToMemory(VisualHost* pHost, Win32Bitmap& bmp);

    void RenderToScreen(VisualHost* visualHost, HDC hdc, Point offset);

    static fPoint RenderShot(Element* elem, Bitmap& bmp, Rect clip);
    static void RenderToImage(FrameworkElement* element, Bitmap* dib);
    static void RenderToDib(FrameworkElement* root, Bitmap* pDib, fRect* lprc);

    static void RenderChild(FrameworkElement* elem, Drawing* drawing);

    static void RenderElementToSurface(FrameworkElement* root, Win32Bitmap& pDib);
    static void RenderLayerWindow(FrameworkElement* root,Win32Bitmap& pDib, HWND hwnd);

protected:
   
    bool CheckRender();

    void RenderNormalWindow(Point offset, Win32Bitmap& pDib, HDC hdc);
    //void HandleRootTransform(FrameworkElement* root, Transform& trans);
  
protected:

    FrameworkElement* _root;
    fRect _clip;
    fRect _rect;
};

}

#endif
