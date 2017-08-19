// 华勤科技版权所有 2010-2013
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
//
// SHARPUI界面引擎采用业界成熟的体系架构，其属性系统、事件系统以
// 及模版在目前c++界面库是一大特点，真正做到样式和代码分离，底层
// 更采用了SSE2指令进行加速，不论在普通窗口还是半透明窗口下性能都
// 非常优秀，完善、灵活的动画接口，完全可以满足各种商业使用。
// 
// 如果需购买技术支持服务或咨询请联系
// china0851@foxmail.com；QQ:386666951。
// ====================================================================

#ifndef _UIACTIVEXHOST_H_
#define _UIACTIVEXHOST_H_

#include <Extend/suiwgxconfig.h>

#include <Framework/Controls/HwndHost.h>
#include <Extend/ActiveX/ActiveXSite.h>
#include <Extend/ActiveX/ActiveXContainer.h>

namespace suic
{

/// <summary>
///  ActiveX，实现ActiveXHost控件的封装。
/// </summary>
class SUIWGX_API ActiveX : public suic::HwndHost
{
public:

    ActiveX();
    ActiveX(ActiveXSite* active);
    virtual ~ActiveX();

    RTTIOfClass(ActiveX)

    suic::Handle GetControlHandle() const;

    void Dispose();

    bool IsValid() const;
    bool DoVerb(int verb);

    /// <summary>
    ///  加载指定ID的com控件
    /// </summary>
    /// <param name="clsid">字符串形式的组件id</param>
    /// <returns>成功返回true;否则false</returns>
    bool LoadActiveX(suic::String clsid);
    bool LoadActiveX(const CLSID& clsid);

    /// <summary>
    ///  查询控件包含的接口
    /// </summary>
    /// <remarks>
    ///     此方法查询载入的控件实现的指定接口指针
    /// </remarks>
    /// <param name="iid">控件接口iid</param>
    /// <param name="ppRet">控件控件接口指针</param>
    /// <returns>成功返回true;否则false</returns>
    bool QueryControl(const IID iid, LPVOID* ppRet);

    bool Reopen();
    void Close();

protected:

    void Clear();

    virtual bool OpenOverride();
    virtual void CloseOveride();

protected:

    void OnArrange(const suic::Size& arrangeSize);
    void ArrangeCore(const suic::Rect& arrangeRect);

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);
    void OnUnloaded(suic::LoadedEventArg* e);
    void OnRender(suic::Drawing * drawing);

    void OnVisibilityChanged();

protected:

    // 组件class id
    CLSID _clsid;

    ActiveXSite* _site;
    ActiveXContainer _container;
    IOleObject* _oleObj;
    suic::Rect _boundRect;
};

typedef suic::shared<ActiveX> ActiveXPtr;

};

#endif
