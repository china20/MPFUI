// 华勤科技版权所有 2010-2011
// 
// 文件名：uieffects.h
// 功  能：封装一般的窗口动画效果。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIEFFECTS_H_
#define _UIEFFECTS_H_

#include <System/Types/Const.h>
#include <System/Windows/FrameworkElement.h>

namespace suic 
{

SUICORE_API void EFRenderWindow(FrameworkElement* root, Bitmap* dib);
SUICORE_API BitmapPtr EFRotateWindow(Float angle, const Bitmap* srcbmp);

}

#endif
