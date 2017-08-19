// 华勤科技版权所有 2010-2011
// 
// 文件名：HwndHost.h
// 功  能：实现真实窗口嵌入
// 
// 作  者：MPF开发组
// 时  间：2012-08-31
// 
// ============================================================================

#ifndef _UIHWNDHOST_H_
#define _UIHWNDHOST_H_

#include <System/Windows/FrameworkElement.h>

namespace suic
{

/// <summary>
/// 适配真实窗口句柄，在排列时放大或缩小窗口。
/// </summary>
class SUICORE_API HwndHost : public FrameworkElement
{
public:

    HwndHost();
    virtual ~HwndHost();

    RTTIOfClass(HwndHost)

    Handle GetHandle();
    void Sethandle(Handle h);

protected:

    void OnInitialized(EventArg* e);

    void OnLoaded(LoadedEventArg* e);
    void OnUnloaded(LoadedEventArg* e);

    void OnRender(Drawing * drawing);
    void OnRenderSizeChanged(SizeChangedInfo& e);

protected:

    void BuildWindow(Handle hwndParent);
    void UpdateWindowPos();
    void OnWindowPositionChanged(Rect rcBox);
    void OnIsVisibleChanged(IsVisibleChangedEventArg* e);

protected:

    // 真实窗口句柄
    Handle _handle;
    // 是否为内部创建句柄
    bool _selfHost;
};

inline Handle HwndHost::GetHandle()
{
    return _handle;
}

};

#endif
