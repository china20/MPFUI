
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
    ///  ��ȡ��ǰ�����Ը�Ԫ�ص�ƫ������
    /// </summary>
    /// <param name="elem">����Ԫ��</param>
    /// <returns>��ǰ�����Ը�Ԫ�ص�ƫ������</returns>
    static Point GetCursorPointOfRoot(Element* elem);

    /// <summary>
    ///  ������Ļ������Ƿ���ָ��Ԫ�����ڴ���֮��
    /// </summary>
    /// <param name="elem">����Ԫ��</param>
    /// <param name="point">��Ļ����</param>
    /// <returns>��Ļ�������ָ��Ԫ�����ڴ���֮�ڷ���true������false</returns>
    static bool PointInWindow(Element* elem, Point* point);

    /// <summary>
    ///  ������Ļ������Ƿ���ָ��Ԫ��֮��
    /// </summary>
    /// <param name="elem">����Ԫ��</param>
    /// <param name="point">��Ļ����</param>
    /// <returns>��Ļ�������ָ��Ԫ��֮�ڷ���true������false</returns>
    static bool PointInElement(Element* elem, Point* point);

    /// <summary>
    ///  ��ȡָ��Ԫ�����ڴ�������Ļ�ϵ����ϵ�����
    /// </summary>
    /// <param name="elem">����Ԫ��</param>
    /// <returns>ָ��Ԫ�����ڴ�������Ļ�ϵ����ϵ�����</returns>
    static Point GetRootScreenPos(Element* elem);

    /// <summary>
    ///  ��ȡָ��Ԫ������Ļ�ϵ����ϵ�����
    /// </summary>
    /// <param name="elem">����Ԫ��</param>
    /// <returns>ָ��Ԫ������Ļ�ϵ����ϵ�����</returns>
    static Point GetElementScreenPos(Element* elem);
};

}

#endif
