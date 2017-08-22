
#ifndef _UIHWNDHELPER_H_
#define _UIHWNDHELPER_H_

#include <Framework/Controls/Window.h>

namespace suic
{ 

class SUICORE_API HwndHelper 
{
public:

    static Handle GetActiveHandle();
    static Window* GetActiveWnd();

    /// <summary>
    ///  获取元素所在的主窗体句柄
    /// </summary>
    /// <param name="elem">目标元素</param>
    /// <returns>窗体句柄</returns>
    static Handle GetHostHwnd(Element* elem);

    static bool IsAllowsTransparency(Element* d);

    static bool IsWindowEnable(Element* elem);
    static bool IsWindowVisible(Element* elem);
    static bool IsWindowMaximize(Element* elem);
    static bool IsWindowMinimize(Element* elem);

    /// <summary>
    ///  恢复元素所在的主窗体
    /// </summary>
    /// <param name="elem">目标元素</param>
    /// <returns>无</returns>
    static void RestoreWindow(Element* elem);

    /// <summary>
    ///  最小化元素所在的主窗体
    /// </summary>
    /// <param name="elem">目标元素</param>
    /// <returns>无</returns>
    static void MinimizeWindow(Element* elem);

    /// <summary>
    ///  最大化元素所在的主窗体
    /// </summary>
    /// <param name="elem">目标元素</param>
    /// <returns>无</returns>
    static void MaximizeWindow(Element* elem);

    /// <summary>
    ///  关闭元素所在的主窗体
    /// </summary>
    /// <param name="elem">目标元素</param>
    /// <returns>无</returns>
    static void CloseWindow(Element* elem, bool bAsync);

    static void HandleLastMessage();

    /// <summary>
    ///  调用系统默认分发消息处理
    /// </summary>
    /// <param name="mp">消息信息</param>
    /// <returns>无</returns>
    static void HandleDefMessage(MessageParam* mp);

    /// <summary>
    ///  刷新所有的窗体显示
    /// </summary>
    /// <returns>无</returns>
    static void InvalidateAllWidows();

    static void DragMoveWindow(Element* elem);
    static void SetWindowTitle(Element* elem, String title);
};

}

#endif
