// 华勤科技版权所有 2008-2022
// 
// 文件名：uirendertarget.h
// 功  能：可视跟元素的宿主类，在此元素中显示的元素相当于虚拟窗体
// 
// 作  者：MPF开发组
// 时  间：2011-12-25
// 
// ============================================================================

#ifndef _UIRENDERTARGET_H_
#define _UIRENDERTARGET_H_

#include <System/Windows/HwndAdapter.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

class Menu;
class RenderInfo;

/// <summary>
/// VisualHost，窗口宿主信息
/// </summary>
class SUICORE_API VisualHost : public Object
{
public:

    RTTIOfClass(VisualHost)

    VisualHost();
    virtual ~VisualHost();

    static DpProperty* RootSourceProperty;

    /// <summary>
    ///  获取元素的目标宿主
    /// </summary>
    /// <remarks>
    ///  目标宿主真正拥有真实窗体，能渲染内容到屏幕，接收消息等。
    /// </remarks>
    /// <param name="elem">目标元素</param>
    /// <returns>目标宿主</returns>
    static VisualHost* GetVisualHost(Element* elem);
    static FrameworkElement* FromHwnd(Handle h);

    virtual void SetValue(const String&, Object* val);
    virtual Object* GetValue(const String&);

    virtual Handle GetHandle();
    void SetHandle(Handle h);

    void SetNeedRender(bool val);
    bool IsNeedRender();

    void SetRootElement(FrameworkElement* host);
    virtual FrameworkElement* GetRootElement();

    /// <summary>
    ///     获取根元素在屏幕中的位置
    /// </summary>
    /// <remarks>
    ///     根元素在屏幕中的位置由画布的屏幕位置偏移
    ///     由SetOffset设置的值。
    /// </remarks>
    /// <returns>屏幕位置偏移</returns>
    virtual Point GetScreenPoint();

    void SetSize(Size size);
    Size GetSize();

    virtual void UpdateLayout();
    virtual void Invalidate(const Rect* lprc, bool bForce);

    virtual void SetMouseCapture();
    virtual void ReleaseMouseCapture();

    //-----------------------------------------------

    /// <summary>
    ///     渲染界面元素
    /// </summary>
    /// <remarks>
    ///     此方法刷新指定区域的界面元素，如果lprc为null，
    ///     则刷行整个界面元素。
    /// </remarks>
    /// <param name="h">
    ///     窗口句柄
    /// </param>
    /// <param name="lprc">
    ///     无效区域
    /// </param>
    /// <returns>无</returns>
    virtual void OnRender(Handle h, const Rect* lprc);
    virtual void OnRender(const Rect* lprc);

    RenderInfo* GetRenderInfo();
    Menu* GetMainMenu();

public:

    virtual bool HandleEvent(MessageParam* mp);
    virtual bool HandleEvent(Handle h, Uint32 message, Uint32 wp, Uint32 lp, Uint32& result);

private:
    
    Handle _handle;
    Size _size;

    WeakRef _mainMenu;

    RenderInfo* _renderInfo;
    FrameworkElement* _rootElement;

    friend class Menu;
};

typedef shared<VisualHost> VisualHostPtr;

};

#endif
