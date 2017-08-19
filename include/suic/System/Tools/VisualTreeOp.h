
#ifndef _UIVISUALTREEHELPER_H_
#define _UIVISUALTREEHELPER_H_

#include <System/Windows/Exception.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{ 

class SUICORE_API VisualTreeOp 
{
public:

    static Element* GetVisualRoot(DpObject* d);
    static Element* GetParent(DpObject* child);
    static Element* FindName(Element* root, const String& name);

    static bool IsAllowsTransparency(DpObject* d);
    static bool IsAncestorOf(Element* ancestor, Element* child);

    static bool IsInVisualTree(Element* child, const Element* elem);
};

class SUICORE_API PointTransform
{
public:

    static Point GetCursorPoint();

    /// <summary>
    ///  获取当前鼠标相对根元素的偏移坐标
    /// </summary>
    /// <param name="elem">界面元素</param>
    /// <returns>当前鼠标相对根元素的偏移坐标</returns>
    static Point GetCursorPointOfRoot(Element* elem);

    /// <summary>
    ///  计算屏幕坐标点是否在指定元素所在窗口之内
    /// </summary>
    /// <param name="elem">界面元素</param>
    /// <param name="point">屏幕坐标</param>
    /// <returns>屏幕坐标点在指定元素所在窗口之内返回true；否则false</returns>
    static bool PointInWindow(Element* elem, Point* point);

    /// <summary>
    ///  计算屏幕坐标点是否在指定元素之内
    /// </summary>
    /// <param name="elem">界面元素</param>
    /// <param name="point">屏幕坐标</param>
    /// <returns>屏幕坐标点在指定元素之内返回true；否则false</returns>
    static bool PointInElement(Element* elem, Point* point);

    /// <summary>
    ///  获取指定元素所在窗口在屏幕上的左上点坐标
    /// </summary>
    /// <param name="elem">界面元素</param>
    /// <returns>指定元素所在窗口在屏幕上的左上点坐标</returns>
    static Point GetRootScreenPos(Element* elem);

    /// <summary>
    ///  获取指定元素在屏幕上的左上点坐标
    /// </summary>
    /// <param name="elem">界面元素</param>
    /// <returns>指定元素在屏幕上的左上点坐标</returns>
    static Point GetElementScreenPos(Element* elem);
};

}

#endif
