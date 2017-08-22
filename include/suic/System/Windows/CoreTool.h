// 华勤科技版权所有 2010-2011
// 
// 文件名：uicorehelper.h
// 功  能：定义核心库帮助方法。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UICORETOOL_H_
#define _UICORETOOL_H_

#include <System/Tools/Debug.h>
#include <System/Types/Interface.h>
#include <System/Windows/RenderTarget.h>

#include <Framework/Controls/Tooltip.h>
#include <Framework/Animation/Storyboard.h>

namespace suic
{

/// <summary>
///  启动UI运行，进入消息分发
/// </summary>
/// <remarks>
///  此方法会调用LoadComponent先建立元素，然后创建窗体进入消息循环。
/// </remarks>
/// <param name="root">默认根元素</param>
/// <param name="path">路径</param>
/// <returns>无</returns>
SUICORE_API void RunApplication(Object* root, const String& path);

//-----------------------------------------------------------

/// <summary>
///  取得元素的渲染边界
/// </summary>
/// <param name="elem">目标元素</param>
/// <returns>元素的渲染边界</returns>
SUICORE_API Rect GetRenderBound(Element*);

/// <summary>
///  刷新整个窗体显示
/// </summary>
/// <param name="elem">目标元素</param>
/// <returns>无</returns>
SUICORE_API void InvalidateWindow(Element*);

/// <summary>
///  获取元素的渲染快照
/// </summary>
/// <param name="elem">目标元素</param>
/// <param name="bmp">元素的渲染快照</param>
/// <returns>无</returns>
SUICORE_API void RenderToImage(Element* elem, Bitmap& bmp);

SUICORE_API bool IsMouseInElement(Element*);
SUICORE_API void SetElementTopMost(Element*, bool);
SUICORE_API void uiEnableWindow(Element*, bool);

//-----------------------------------------------------------

SUICORE_API void* uiMalloc(int);
SUICORE_API void uiFree(void*);

SUICORE_API void UIInit(int level);
SUICORE_API void UIClear();

SUICORE_API Handle GetUIFont();

SUICORE_API Point ToScreenPoint(Element* element, Point pt);
SUICORE_API Point GetCursorPoint(Element* element);
SUICORE_API bool PointInWindow(Element* element);
SUICORE_API Point CalcuCusorPoint(Element* element);
SUICORE_API Point CalcElementAlign(Element* element, int flag);

SUICORE_API int RegisterWindowFilter(Element*, MessageHook);
SUICORE_API void RemoveWindowFilter(Element*, MessageHook);

SUICORE_API void CloseAssignerFrame(AssignerFrame* frame);

}

#endif
