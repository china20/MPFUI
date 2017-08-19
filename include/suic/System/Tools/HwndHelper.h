
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
    ///  ��ȡԪ�����ڵ���������
    /// </summary>
    /// <param name="elem">Ŀ��Ԫ��</param>
    /// <returns>������</returns>
    static Handle GetHostHwnd(Element* elem);

    static bool IsAllowsTransparency(Element* d);

    static bool IsWindowEnable(Element* elem);
    static bool IsWindowVisible(Element* elem);
    static bool IsWindowMaximize(Element* elem);
    static bool IsWindowMinimize(Element* elem);

    /// <summary>
    ///  �ָ�Ԫ�����ڵ�������
    /// </summary>
    /// <param name="elem">Ŀ��Ԫ��</param>
    /// <returns>��</returns>
    static void RestoreWindow(Element* elem);

    /// <summary>
    ///  ��С��Ԫ�����ڵ�������
    /// </summary>
    /// <param name="elem">Ŀ��Ԫ��</param>
    /// <returns>��</returns>
    static void MinimizeWindow(Element* elem);

    /// <summary>
    ///  ���Ԫ�����ڵ�������
    /// </summary>
    /// <param name="elem">Ŀ��Ԫ��</param>
    /// <returns>��</returns>
    static void MaximizeWindow(Element* elem);

    /// <summary>
    ///  �ر�Ԫ�����ڵ�������
    /// </summary>
    /// <param name="elem">Ŀ��Ԫ��</param>
    /// <returns>��</returns>
    static void CloseWindow(Element* elem, bool bAsync);

    static void HandleLastMessage();

    /// <summary>
    ///  ����ϵͳĬ�Ϸַ���Ϣ����
    /// </summary>
    /// <param name="mp">��Ϣ��Ϣ</param>
    /// <returns>��</returns>
    static void HandleDefMessage(MessageParam* mp);

    /// <summary>
    ///  ˢ�����еĴ�����ʾ
    /// </summary>
    /// <returns>��</returns>
    static void InvalidateAllWidows();

    static void DragMoveWindow(Element* elem);
    static void SetWindowTitle(Element* elem, String title);
};

}

#endif
