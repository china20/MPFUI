// 华勤科技版权所有 2008-2022
// 
// 文件名：uihwndsubclass.h
// 功  能：封装windows的句柄。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIHWNDSUBCLASS_H_
#define _UIHWNDSUBCLASS_H_

#include <System/Windows/Object.h>
#include <System/Windows/HwndObject.h>
#include <System/Windows/RenderInfo.h>
#include <System/Windows/FrameworkElement.h>

#include <System/Types/Interface.h>
#include <System/Tools/Collection.h>

namespace suic
{

class Window;

class HwndSubclass
{
public:

    HwndSubclass(const MessageHook& hook);

    static void HandleNcCreate(HWND hwnd, WPARAM wParam, LPARAM lParam);
    static LRESULT __stdcall SubclassWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT __stdcall DoWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, bool callDef, bool& handled);

    static LRESULT __stdcall CallWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, bool& handled);

    static void Close();

    static void SubclassNativeWnd(HWND hwnd);
    static void UnsubclassNativeWnd(HWND hwnd);

    static void RemoveHwndObject(HWND hwnd);

    static void InvalidateAllWindows(Rect* lprc);
    static bool HasHwnds();
    static Handle GetTopActiveHwnd();
    static Window* GetTopActiveWnd();

    static Element* LookRootElement(String name);

    static VisualHost* MatchRenderSource(Element* element);
    static VisualHost* FindRenderTarget(HWND hwnd);

    static void RemoveHwnd(Handle h);
    static SysRenderSource* MatchHwndObject(ElementPtr);

    static void UpdateResources();
    static void InvalidateResourceReference();

    static void Render();
    static void RenderHwnd(HWND hwnd);
    static void RenderVisualHost(VisualHost* visualHost);

protected:

    static LRESULT CallDefWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    static void RemovePendingHwndSourceHook(AssignerFrame* curf);

    static void HandleCommand(MessageParam* mp, bool handled);
    static void DoHwndDestroyMessage(HWND hwnd, SysRenderSource* visual, MessageParam* mp);

protected:

    static PointerAutoDic<HWND, VisualHost> _hwnds;
};

};

#endif
